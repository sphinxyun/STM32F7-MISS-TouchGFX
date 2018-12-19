#include <diagnostics/diagnostics.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "global.h"

TaskHandle_t DiagnosticsThreadId = 0;

//QueueHandle_t xRegulationStatus;

//extern QueueHandle_t xIrrigationPressureMMHG;
//extern QueueHandle_t xIrrigationMotorSpeedRPM;

static void Diagnostics_Thread(void * argument);

DIAGNOSTICS_ErrorTypdef DIAGNOSTICS_Init(void) {
//    PRESSURE_SENSOR_Init();
//    MOTORS_Init();

//    xRegulationStatus = xQueueCreate( 2, sizeof( DIAGNOSTICS_RegulatorStatus_t ) );

//	osThreadDef(osRegulation_Thread, Regulation_Thread, osPriorityNormal, 0, 512);
//	RegulationThreadId = osThreadCreate(osThread(osRegulation_Thread), NULL);

    xTaskCreate(Diagnostics_Thread, "DaignosticsTask",
                512,
                NULL,
				tskIDLE_PRIORITY + 3,
                &DiagnosticsThreadId);

	DIAGNOSTICS_Stop();

	return DIAGNOSTICS_ERROR_NONE;
}

DIAGNOSTICS_ErrorTypdef DIAGNOSTICS_DeInit(void) {
	return DIAGNOSTICS_ERROR_NONE;
}

DIAGNOSTICS_ErrorTypdef DIAGNOSTICS_Start(void) {
	if (DiagnosticsThreadId != 0) {
		vTaskResume(DiagnosticsThreadId);
	}

//	PRESSURE_SENSOR_Start();
//	MOTORS_Start();

//	StartSpeedMonitoring();

	return DIAGNOSTICS_ERROR_NONE;
}

DIAGNOSTICS_ErrorTypdef DIAGNOSTICS_Stop(void) {
	if (DiagnosticsThreadId != 0) {
		vTaskSuspend(DiagnosticsThreadId);
	}

//	PRESSURE_SENSOR_Stop();
//	MOTORS_Stop();

	return DIAGNOSTICS_ERROR_NONE;
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

static void Regulation_Thread(void * argument) {
//	DIAGNOSTICS_RegulatorStatus_t status;
	bool bUpdate = false;

	for (;;) {

//		if (xQueueReceive(xIrrigationPressureMMHG, &status.fIrrigationActualPressureMMHG, 50)) {
////			DEBUG_SendTextFrame("Regulation_Thread PRESSURE: %f", status.fIrrigationActualPressureMMHG);
//			bUpdate = true;
//		} else {
////			DEBUG_SendTextFrame("Regulation_Thread PRESSURE: ---");
//		}
//
//		if (xQueueReceive(xIrrigationMotorSpeedRPM, &status.fIrrigationActualSpeedRPM, 50)) {
////			DEBUG_SendTextFrame("Regulation_Thread SPEED: %f", status.fIrrigationActualSpeedRPM);
//			bUpdate = true;
//		} else {
////			DEBUG_SendTextFrame("Regulation_Thread SPEED: ---");
//		}
//
//		if (bUpdate) {
//			status.fIrrigationActualFlowLPM = FCE_fIrrGetFlowCoeff(status.fIrrigationActualSpeedRPM) * status.fIrrigationActualSpeedRPM;
//			xQueueSend( xRegulationStatus, ( void * ) &status, ( TickType_t ) 0 );
//		}

	}
}
