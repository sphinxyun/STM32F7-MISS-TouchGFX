#include "../state_machine/state_machine.h"
#include "settings/settings_app.h"

TaskHandle_t StateMachineThreadId = 0;

QueueHandle_t xGuiActions;
QueueHandle_t xGuiStatus;

static WM_MAIN_DeviceMode device_state;

#define POOL_MAX_STRUCTS		5

WM_MAIN_GuiPool guiStatusPool[ POOL_MAX_STRUCTS ];

extern QueueHandle_t xRegulationStatus;

static void StateMachine_Thread(void * argument);

WM_MAIN_ErrorTypdef WM_MAIN_Init(void) {
	for (int i = 0; i < POOL_MAX_STRUCTS; i++) {
		guiStatusPool[i].inUse = false;
	}

	xGuiActions = xQueueCreate( 5, sizeof( WM_MAIN_ActionsTypdef ) );
	xGuiStatus = xQueueCreate( 2, sizeof( WM_MAIN_GuiPool* ) );

    xTaskCreate(StateMachine_Thread, "StateMachineTask",
                512,
                NULL,
				tskIDLE_PRIORITY + 3,
                &StateMachineThreadId);

	return WM_MAIN_ERROR_NONE;
}

WM_MAIN_GuiPool *get_struct(void) {
	// Find the next free struct
	for (int i = 0; i < POOL_MAX_STRUCTS; i++) {
		if (guiStatusPool[i].inUse == false) {
			// Mark struct in use and return it
			guiStatusPool[i].inUse = true;
			return &guiStatusPool[i];
		}
	}

	// All in use
	return NULL;
}

void free_struct(WM_MAIN_GuiPool *toRelease) {
	// Find the struct being freed
	for (int i = 0; i < POOL_MAX_STRUCTS; i++) {
		if (toRelease == &guiStatusPool[i]) {
			// Mark struct not in use
			guiStatusPool[i].inUse = false;
			return;
		}
	}
}


WM_MAIN_ErrorTypdef WM_MAIN_DeInit(void) {
	return WM_MAIN_ERROR_NONE;
}

WM_MAIN_ErrorTypdef WM_MAIN_Start(void) {
	if (StateMachineThreadId != 0) {
		vTaskResume(StateMachineThreadId);
	}

	return WM_MAIN_ERROR_NONE;
}

WM_MAIN_ErrorTypdef WM_MAIN_Stop(void) {
	if (StateMachineThreadId != 0) {
		vTaskResume(StateMachineThreadId);
	}

	return WM_MAIN_ERROR_NONE;
}

bool CheckHPresetPressure(WM_MAIN_GuiPool* guiStatus) {
	const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
	if ((guiStatus->status.u8IrrigationPresetPressureMMHG + set->u32IrrigationPressureIncValue) > set->u32IrrigationPressureMaxValue)
		return true;
	else
		return false;
}

bool CheckLPresetPressure(WM_MAIN_GuiPool* guiStatus) {
	const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
	if ((guiStatus->status.u8IrrigationPresetPressureMMHG - set->u32IrrigationPressureIncValue) < set->u32IrrigationPressureMinValue)
		return true;
	else
		return false;
}

bool CheckHPresetFlow(WM_MAIN_GuiPool* guiStatus) {
	const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
	if ((guiStatus->status.u16IrrigationPresetFlowRPM + set->u16IrrigationFlowRPMIncValue) > set->u16IrrigationFlowRPMMaxValue)
		return true;
	else
		return false;
}

bool CheckLPresetFlow(WM_MAIN_GuiPool* guiStatus) {
	const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
	if ((guiStatus->status.u16IrrigationPresetFlowRPM - set->u16IrrigationFlowRPMIncValue) < set->u16IrrigationFlowRPMMinValue)
		return true;
	else
		return false;
}

static void StateMachine_Thread(void * argument) {
	WM_MAIN_ActionsTypdef action;

	uint32_t u32BtnFlags = 0xFFFF;

	bool bUpdate = true;

	device_state = eStartup;

	const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();

	WM_MAIN_GuiPool* guiStatus = get_struct();

	guiStatus->status.u8IrrigationPresetPressureMMHG = set->u32IrrigationPressure;
	guiStatus->status.u16IrrigationPresetFlowRPM = set->u16IrrigationFlowRPM;
	guiStatus->status.fIrrigationPresetFlowLPM = set->fIrrigationFlowLPM;

	REGULATION_Start();

	TickType_t xTimeBefore = xTaskGetTickCount();

	for (;;) {
		DEBUG_UART_SysTick();

		if (((xTaskGetTickCount() - xTimeBefore) > 500) && ((xTaskGetTickCount() - xTimeBefore) < 2700)) {
			DEBUG_SendTextFrame("state change -> eAutoTest");
			device_state = eIdle;
			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("state change -> %d", xTaskGetTickCount() - xTimeBefore);
		}

		if (xQueueReceive(xGuiActions, &action, 25)) {
			bUpdate = true;

			if (action == WM_GUI_LOADED) {
				TOUCHGFT_SetBacklight(SETTINGS_GetBrightness());
			} else if (action == WM_MAIN_START_ACTION) {

				MOTOR_Start(guiStatus->status.u16IrrigationPresetFlowRPM);

				DEBUG_SendTextFrame("WM_MAIN_START_ACTION");
			} else if (action == WM_MAIN_STOP_ACTION) {
//				REGULATION_Stop();
				MOTOR_Stop();

				DEBUG_SendTextFrame("WM_MAIN_STOP_ACTION");
			} else if (action == WM_MAIN_INCREASE_BRIGHTNESS) {
				SETTINGS_IncBrightness();
				DEBUG_SendTextFrame("WM_MAIN_INCREASE_BRIGHTNESS");
			} else if (action == WM_MAIN_DECREASE_BRIGHTNESS) {
				SETTINGS_DecBrightness();
				DEBUG_SendTextFrame("WM_MAIN_DECREASE_BRIGHTNESS");
			} else if (action == WM_MAIN_INCREASE_IRRIGATION_PRESSURE) {
				const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
				if ((guiStatus->status.u8IrrigationPresetPressureMMHG + set->u32IrrigationPressureIncValue) >= set->u32IrrigationPressureMaxValue) {
					guiStatus->status.u8IrrigationPresetPressureMMHG = set->u32IrrigationPressureMaxValue;
				} else
					guiStatus->status.u8IrrigationPresetPressureMMHG += set->u32IrrigationPressureIncValue;

				DEBUG_SendTextFrame("WM_MAIN_INCREASE_IRRIGATION_PRESSURE: %d, %x", guiStatus->status.u8IrrigationPresetPressureMMHG, u32BtnFlags);

			} else if (action == WM_MAIN_DECREASE_IRRIGATION_PRESSURE) {
				const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
				if ((guiStatus->status.u8IrrigationPresetPressureMMHG - set->u32IrrigationPressureIncValue) <= set->u32IrrigationPressureMinValue) {
					guiStatus->status.u8IrrigationPresetPressureMMHG = set->u32IrrigationPressureMinValue;
				} else
					guiStatus->status.u8IrrigationPresetPressureMMHG -= set->u32IrrigationPressureIncValue;

				DEBUG_SendTextFrame("WM_MAIN_DECREASE_IRRIGATION_PRESSURE: %d, %x", guiStatus->status.u8IrrigationPresetPressureMMHG, u32BtnFlags);
			} else if (action == WM_MAIN_INCREASE_IRRIGATION_FLOW) {
				const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
				if ((guiStatus->status.u16IrrigationPresetFlowRPM + set->u16IrrigationFlowRPMIncValue) >= set->u16IrrigationFlowRPMMaxValue) {
					guiStatus->status.u16IrrigationPresetFlowRPM = set->u16IrrigationFlowRPMMaxValue;
				} else
					guiStatus->status.u16IrrigationPresetFlowRPM += set->u16IrrigationFlowRPMIncValue;

				MOTOR_UpdateSpeed(guiStatus->status.u16IrrigationPresetFlowRPM);

				DEBUG_SendTextFrame("WM_MAIN_INCREASE_IRRIGATION_FLOW: %d, %x", guiStatus->status.u16IrrigationPresetFlowRPM, u32BtnFlags);
			} else if (action == WM_MAIN_DECREASE_IRRIGATION_FLOW) {
				const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
				if ((guiStatus->status.u16IrrigationPresetFlowRPM - set->u16IrrigationFlowRPMIncValue) <= set->u16IrrigationFlowRPMMinValue) {
					guiStatus->status.u16IrrigationPresetFlowRPM = set->u16IrrigationFlowRPMMinValue;
				} else
					guiStatus->status.u16IrrigationPresetFlowRPM -= set->u16IrrigationFlowRPMIncValue;

				MOTOR_UpdateSpeed(guiStatus->status.u16IrrigationPresetFlowRPM);

				DEBUG_SendTextFrame("WM_MAIN_DECREASE_IRRIGATION_FLOW: %d, %x", guiStatus->status.u16IrrigationPresetFlowRPM, u32BtnFlags);
			}

			if (CheckHPresetPressure(guiStatus))
				u32BtnFlags &= (~(1 << 1));
			else
				u32BtnFlags |= (1 << 1);

			if (CheckLPresetPressure(guiStatus))
				u32BtnFlags &= (~(1 << 0));
			else
				u32BtnFlags |= (1 << 0);

			if (CheckHPresetFlow(guiStatus))
				u32BtnFlags &= (~(1 << 3));
			else
				u32BtnFlags |= (1 << 3);

			if (CheckLPresetFlow(guiStatus))
				u32BtnFlags &= (~(1 << 2));
			else
				u32BtnFlags |= (1 << 2);
		}

		if (xQueueReceive(xRegulationStatus, &guiStatus->status.sIrrigationActual, 50)) {
			if (guiStatus->status.sIrrigationActual.sRawPressureSensorData.fPressureMMHG < 0)
				guiStatus->status.sIrrigationActual.sRawPressureSensorData.fPressureMMHG = 0;
//			DEBUG_SendTextFrame("Main_Thread PRESSURE: %f", guiStatus.sIrrigationActual.fIrrigationActualPressureMMHG);
//			DEBUG_SendTextFrame("Main_Thread SPEED   : %f", guiStatus.sIrrigationActual.fIrrigationActualSpeedRPM);
			static int iAlCnt = 0;
			if (guiStatus->status.sIrrigationActual.sRawPressureSensorData.fPressureMMHG > guiStatus->status.u8IrrigationPresetPressureMMHG + 5) {
				iAlCnt++;
				if (iAlCnt > 10) {
					guiStatus->status.u32AlarmFlags = 1;
					iAlCnt = 11;
				}
			}
			else {
				iAlCnt = 0;
				guiStatus->status.u32AlarmFlags = 0;
			}

			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("Regulation_Thread SPEED: ---");
		}

		if (bUpdate) {
			guiStatus->status.eDevMode = device_state;
			guiStatus->status.u32ButtonFlags = u32BtnFlags;
			guiStatus->status.u32BrightnessPercent = SETTINGS_GetBrightness();

			DEBUG_SendTextFrame("StateMachine_Thread: xGuiStatus SEND");

			xQueueSend( xGuiStatus, ( void * ) &guiStatus, ( TickType_t ) 0 );

			guiStatus = get_struct();

			if (guiStatus == NULL) {
				DEBUG_SendTextFrame("StateMachine_Thread: get_struct ERROR");
			}

			guiStatus->status.u8IrrigationPresetPressureMMHG = set->u32IrrigationPressure;
			guiStatus->status.u16IrrigationPresetFlowRPM = set->u16IrrigationFlowRPM;
			guiStatus->status.fIrrigationPresetFlowLPM = set->fIrrigationFlowLPM;

			bUpdate = false;
		}
	}
}
