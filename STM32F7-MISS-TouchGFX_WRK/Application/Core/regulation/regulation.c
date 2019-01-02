#include "regulation.h"

#include "motors/motors_thread.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "debug.h"
#include "global.h"

TaskHandle_t RegulationTaskId = 0;

QueueHandle_t xRegulationActions;
QueueHandle_t xRegulationStatus;

extern QueueHandle_t xIrrigationPressureData;
extern QueueHandle_t xIrrigationMotorSpeedRPM;

static void Regulation_Thread(void * argument);
static inline float FCE_fIrrGetFlowCoeff(float fRPMSpeed);

REGULATION_ErrorTypdef REGULATION_Init(void) {
    PRESSURE_SENSOR_Init();
    MOTORS_Init();

    xRegulationStatus = xQueueCreate( 2, sizeof( REGULATION_IrrActual_t ) );
    xRegulationActions = xQueueCreate( 2, sizeof( REGULATION_IrrPresets_t ) );

    xTaskCreate(Regulation_Thread, "RegulationTask",
                512,
                NULL,
				tskIDLE_PRIORITY + 3,
                &RegulationTaskId);

	REGULATION_TaskStop();

	return REGULATION_ERROR_NONE;
}

REGULATION_ErrorTypdef REGULATION_DeInit(void) {
	return REGULATION_ERROR_NONE;
}

REGULATION_ErrorTypdef REGULATION_TaskStart(void) {
	if (RegulationTaskId != 0) {
		vTaskResume(RegulationTaskId);
	}

	PRESSURE_SENSOR_Start();
	MOTORS_TaskStart();

	return REGULATION_ERROR_NONE;
}

REGULATION_ErrorTypdef REGULATION_TaskStop(void) {
	if (RegulationTaskId != 0) {
		vTaskSuspend(RegulationTaskId);
	}

	PRESSURE_SENSOR_Stop();
	MOTORS_TaskStop();

	return REGULATION_ERROR_NONE;
}

#define REGULATE	0

static void Regulation_Thread(void * argument) {
	REGULATION_IrrActual_t actualState;
	REGULATION_IrrPresets_t presetState;

	sCarmenDataPool_t *sensorData;

	bool bUpdate = false;

#if REGULATE == 1
	//-------------------------------------------------------------------------------
	//-----------------       REGULATION PROCESS -> IRRIGATION      -----------------
	//-----------------------------------------=-------------------------------------
	//! slave regulator Kp in the irrigation section (speed mode)
	float KIp_S = 0;
	//! slave regulator Ki in the irrigation section (speed mode)
	float KIi_S = 0;
	//! slave regulator old Ki in the irrigation section (speed mode)
	float KIi_S_old = 0;

	float pT_S = 0, iT_S = 0, ret_S = 0;

	//! irrigation motor slave 'speed' regulator integral action sum of errors
	float sumErrIRR_S = 0;
	//! slave regulator integral action in irrigation section gain changed (@ref sumErrIRR_S should be recalculated according to values of both @ref KIi_S and @ref KIi_S_old)
	volatile bool KIi_S_changed = false;
#endif

	//! irrigation slave regulator drive - RPM mode
//	float driveRPM1;

	for (;;) {
		REGULATION_IrrPresets_t latestPresets;
		//do not wait for action queue events
		if (xRegulationActions && xQueueReceive(xRegulationActions, &latestPresets, 0)) {
			if (latestPresets.eRegMode != presetState.eRegMode) {
				switch (latestPresets.eRegMode) {
				case eRegIdle:
					MOTORS_IrrigationStop();
					break;

				case eRegIrrigation:
#if REGULATE == 1
					MOTORS_IrrigationStart(0);
#else
					MOTORS_IrrigationStart(latestPresets.i16FlowRPM);
#endif
					break;

				default:
					break;
				}
			}

			presetState = latestPresets;
#if REGULATE == 0
			MOTORS_IrrigationUpdate(presetState.i16FlowRPM);
#endif
		}

		if (xIrrigationPressureData && xQueueReceive(xIrrigationPressureData, &sensorData, 15)) {
			actualState.sPressureData = sensorData->data;
			free_sensor_struct(sensorData);
//			DEBUG_SendTextFrame("Regulation_Thread PRESSURE: %f", status.fIrrigationActualPressureMMHG);
			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("Regulation_Thread PRESSURE: ---");
		}

		if (xIrrigationMotorSpeedRPM && xQueueReceive(xIrrigationMotorSpeedRPM, &actualState.fFlowRPM, 15)) {
//			DEBUG_SendTextFrame("Regulation_Thread SPEED: %f", status.fIrrigationActualSpeedRPM);
			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("Regulation_Thread SPEED: ---");
		}

#if REGULATE == 1
		float INPUT_SLAVE_PID = presetState.i16FlowRPM;
		//rpmEN1 is read in the timer ISR routine!!!
		float FEEDBACK_SLAVE_PID = actualState.fFlowRPM;
		float ERROR_SLAVE_PID = (INPUT_SLAVE_PID - FEEDBACK_SLAVE_PID);

		float temp_KIi_S = 0;

//		if (INPUT_SLAVE_PID > 250) {
			//drive above 250 RPMs
			KIp_S = 0.36;
			temp_KIi_S = 0.029;
//		} else {
//			KIp_S = -0.19 * INPUT_SLAVE_PID * INPUT_SLAVE_PID + 44;
//			temp_KIi_S = -0.035637 * INPUT_SLAVE_PID + 10.2;
//		}

		if (temp_KIi_S != KIi_S) {
			KIi_S_changed = true;
			KIi_S_old = KIi_S;
			KIi_S = temp_KIi_S;
		}

		if (presetState.eRegMode == eRegIrrigation) {
			//P TERM calculations:
			pT_S = KIp_S * ERROR_SLAVE_PID;

			//I TERM calculations:
			if (KIi_S_changed) {
				//recalculate previous error:
				sumErrIRR_S = sumErrIRR_S * (KIi_S_old / KIi_S);
				KIi_S_changed = false;
			}

			//calculate I TERM and limit integral runaway:
			float temp = sumErrIRR_S + ERROR_SLAVE_PID;
			if (temp > 4000.0 / KIi_S) {
				sumErrIRR_S = 4000.0 / KIi_S;
				iT_S = 3999;
			} else if (temp < -4000.0 / KIi_S) {
				sumErrIRR_S = -4000.0 / KIi_S;
				iT_S = -3999;
			} else {
				iT_S = KIi_S * sumErrIRR_S;
				sumErrIRR_S = temp;
			}

			//slave output:
			ret_S = (pT_S + iT_S);

			if (ret_S > 3999) {
				ret_S = 3999;
			} else if (ret_S < -3999) {
				ret_S = -3999;
			}

			MOTORS_IrrigationUpdate(ret_S);
		}
#endif

		AD_SIG_3_TOGGLE;
#if REGULATE == 1
		float fTemp[8] = {INPUT_SLAVE_PID, FEEDBACK_SLAVE_PID, ERROR_SLAVE_PID, pT_S, KIp_S, iT_S, sumErrIRR_S, ret_S};
		DEBUG_SendRpmPidDataFrame(8, fTemp);
#else
		float fTemp[2] = {presetState.i16FlowRPM, actualState.fFlowRPM};
		DEBUG_SendRpmPidDataFrame(2, fTemp);
#endif
		if (bUpdate) {
			actualState.fFlowLPM = FCE_fIrrGetFlowCoeff(actualState.fFlowRPM) * actualState.fFlowRPM;
			xQueueSend( xRegulationStatus, ( void * ) &actualState, ( TickType_t ) 0 );
		}
	}
}

static inline float FCE_fIrrGetFlowCoeff(float fRPMSpeed) {
	//we believe, that flow coefficient will be same in both directions of the irrigation roller wheel
	fRPMSpeed = ABS(fRPMSpeed);
	//compute...
	float fFlowCoeff;
	if (fRPMSpeed <= 275.0)
		fFlowCoeff = (-0.0036 * fRPMSpeed + 4.8936) / 1000.0;
	else
		fFlowCoeff = (-0.0036 * 275.0 + 4.8936) / 1000.0;

	//sanity check...
	if ((fFlowCoeff < 0) || (fFlowCoeff > 0.006))
		fFlowCoeff = 0.00410;
	//return
	return fFlowCoeff;
}
