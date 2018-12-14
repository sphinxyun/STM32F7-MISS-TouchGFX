#include "regulation.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "global.h"

TaskHandle_t RegulationThreadId = 0;

QueueHandle_t xRegulationStatus;

extern QueueHandle_t xIrrigationPressureMMHG;
extern QueueHandle_t xIrrigationMotorSpeedRPM;

static void Regulation_Thread(void * argument);

REGULATION_ErrorTypdef 	REGULATION_Init(void) {
    PRESSURE_SENSOR_Init();
    MOTORS_Init();

    xRegulationStatus = xQueueCreate( 2, sizeof( REGULATION_RegulatorStatus_t ) );

//	osThreadDef(osRegulation_Thread, Regulation_Thread, osPriorityNormal, 0, 512);
//	RegulationThreadId = osThreadCreate(osThread(osRegulation_Thread), NULL);

    xTaskCreate(Regulation_Thread, "RegulationTask",
                512,
                NULL,
				tskIDLE_PRIORITY + 3,
                &RegulationThreadId);

	REGULATION_Stop();

	return REGULATION_ERROR_NONE;
}

REGULATION_ErrorTypdef REGULATION_DeInit(void) {
	return REGULATION_ERROR_NONE;
}

REGULATION_ErrorTypdef REGULATION_Start(void) {
	if (RegulationThreadId != 0) {
		vTaskResume(RegulationThreadId);
	}

	PRESSURE_SENSOR_Start();
	MOTORS_Start();

	StartSpeedMonitoring();

	return REGULATION_ERROR_NONE;
}

REGULATION_ErrorTypdef REGULATION_Stop(void) {
	if (RegulationThreadId != 0) {
		vTaskSuspend(RegulationThreadId);
	}

	PRESSURE_SENSOR_Stop();
	MOTORS_Stop();

	return REGULATION_ERROR_NONE;
}

static void Regulation_Thread(void * argument) {
	REGULATION_RegulatorStatus_t status;
	bool bUpdate = false;

	for (;;) {
		if (xQueueReceive(xIrrigationPressureMMHG, &status.fIrrigationActualPressureMMHG, 50)) {
//			DEBUG_SendTextFrame("Regulation_Thread PRESSURE: %f", status.fIrrigationActualPressureMMHG);
			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("Regulation_Thread PRESSURE: ---");
		}

		if (xQueueReceive(xIrrigationMotorSpeedRPM, &status.fIrrigationActualSpeedRPM, 50)) {
//			DEBUG_SendTextFrame("Regulation_Thread SPEED: %f", status.fIrrigationActualSpeedRPM);
			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("Regulation_Thread SPEED: ---");
		}

		if (bUpdate) {
			xQueueSend( xRegulationStatus, ( void * ) &status, ( TickType_t ) 0 );
		}

	}
}
