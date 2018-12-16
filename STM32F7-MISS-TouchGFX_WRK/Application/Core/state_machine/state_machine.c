#include "../state_machine/state_machine.h"
#include "settings/settings_app.h"

TaskHandle_t StateMachineThreadId = 0;

QueueHandle_t xGuiActions;
QueueHandle_t xGuiStatus;

static WM_MAIN_DeviceMode device_state;

WM_MAIN_GuiStatus guiStatus;

extern QueueHandle_t xRegulationStatus;

static void StateMachine_Thread(void * argument);

WM_MAIN_ErrorTypdef WM_MAIN_Init(void) {
	xGuiActions = xQueueCreate( 5, sizeof( WM_MAIN_ActionsTypdef ) );
	xGuiStatus = xQueueCreate( 2, sizeof( WM_MAIN_GuiStatus ) );

//	osThreadDef(osStateMachine_Thread, StateMachine_Thread, osPriorityNormal, 0, 512);
//	StateMachineThreadId = osThreadCreate(osThread(osStateMachine_Thread), NULL);

    xTaskCreate(StateMachine_Thread, "StateMachineTask",
                512,
                NULL,
				tskIDLE_PRIORITY + 3,
                &StateMachineThreadId);

	return WM_MAIN_ERROR_NONE;
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

bool CheckHPresetPressure(void) {
	const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
	if ((guiStatus.fIrrigationPresetPressureMMHG + set->u32IrrigationPressureIncValue) > set->u32IrrigationPressureMaxValue)
		return true;
	else
		return false;
}

bool CheckLPresetPressure(void) {
	const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
	if ((guiStatus.fIrrigationPresetPressureMMHG - set->u32IrrigationPressureIncValue) < set->u32IrrigationPressureMinValue)
		return true;
	else
		return false;
}

static void StateMachine_Thread(void * argument) {
	WM_MAIN_ActionsTypdef action;


	uint32_t u32BtnFlags = 0xFFFF;

	bool bUpdate = true;

	device_state = eAutoTest;

	const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();

	guiStatus.fIrrigationPresetPressureMMHG = set->u32IrrigationPressure;
	guiStatus.fIrrigationPresetFlowLPM = set->fIrrigationFlow;

//	module_prop[state].in_use = 1;
//	module_prop[state].module->startup(WM_GetDesktopWindowEx(0), 0, 0);
//
//	osDelay(500);
//
//	module_prop[state].in_use = 1;
//	module_prop[state].module->startup(WM_GetDesktopWindowEx(0), 0, 0);

	TickType_t xTimeBefore;
	xTimeBefore = xTaskGetTickCount();

	for (;;) {
		  DEBUG_UART_SysTick();


//		if (((xTaskGetTickCount() - xTimeBefore) > 500) && ((xTaskGetTickCount() - xTimeBefore) < 700)) {
//			DEBUG_SendTextFrame("state change -> LEVEL");
//			device_state = eLevel;
//			bUpdate = true;
//		} else {
////			DEBUG_SendTextFrame("state change -> %d", xTaskGetTickCount() - xTimeBefore);
//		}

		if (xQueueReceive(xGuiActions, &action, 25)) {
			bUpdate = true;

			if (action == WM_MAIN_MENU) {
				DEBUG_SendTextFrame("WM_MAIN_MENU");
				device_state = eMenu;
			} else if (action == WM_MAIN_START_ACTION) {
				DEBUG_SendTextFrame("WM_MAIN_START_ACTION");
				MOTOR_Start();
			} else if (action == WM_MAIN_STOP_ACTION) {
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
				if ((guiStatus.fIrrigationPresetPressureMMHG + set->u32IrrigationPressureIncValue) >= set->u32IrrigationPressureMaxValue) {
					guiStatus.fIrrigationPresetPressureMMHG = set->u32IrrigationPressureMaxValue;
				} else
					guiStatus.fIrrigationPresetPressureMMHG += set->u32IrrigationPressureIncValue;

				DEBUG_SendTextFrame("WM_MAIN_INCREASE_IRRIGATION_PRESSURE: %f, %x", guiStatus.fIrrigationPresetPressureMMHG, u32BtnFlags);

			} else if (action == WM_MAIN_DECREASE_IRRIGATION_PRESSURE) {
				const SETTINGS_ProgramSettingsTypedef *set = SETTINGS_Get();
				if ((guiStatus.fIrrigationPresetPressureMMHG - set->u32IrrigationPressureIncValue) <= set->u32IrrigationPressureMinValue) {
					guiStatus.fIrrigationPresetPressureMMHG = set->u32IrrigationPressureMinValue;
				} else
					guiStatus.fIrrigationPresetPressureMMHG -= set->u32IrrigationPressureIncValue;

				DEBUG_SendTextFrame("WM_MAIN_DECREASE_IRRIGATION_PRESSURE: %f, %x", guiStatus.fIrrigationPresetPressureMMHG, u32BtnFlags);
			}

			if (CheckHPresetPressure())
				u32BtnFlags &= (~(1 << 1));
			else
				u32BtnFlags |= (1 << 1);

			if (CheckLPresetPressure())
				u32BtnFlags &= (~(1 << 0));
			else
				u32BtnFlags |= (1 << 0);
		}

		if (xQueueReceive(xRegulationStatus, &guiStatus.sIrrigationActual, 50)) {
//			DEBUG_SendTextFrame("Main_Thread PRESSURE: %f", guiStatus.sIrrigationActual.fIrrigationActualPressureMMHG);
//			DEBUG_SendTextFrame("Main_Thread SPEED   : %f", guiStatus.sIrrigationActual.fIrrigationActualSpeedRPM);
			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("Regulation_Thread SPEED: ---");
		}

		if (bUpdate) {
			guiStatus.eDevMode = device_state;
			guiStatus.u32ButtonFlags = u32BtnFlags;

			DEBUG_SendTextFrame("StateMachine_Thread: xGuiStatus SEND");

			xQueueSend( xGuiStatus, ( void * ) &guiStatus, ( TickType_t ) 0 );

			bUpdate = false;
		}
	}
}
