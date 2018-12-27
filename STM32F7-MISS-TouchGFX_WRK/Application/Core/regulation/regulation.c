#include "regulation.h"

#include "motors/motors_thread.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "global.h"

TaskHandle_t RegulationTaskId = 0;

QueueHandle_t xRegulationStatus;

extern QueueHandle_t xIrrigationPressureData;
extern QueueHandle_t xIrrigationMotorSpeedRPM;

static void Regulation_Thread(void * argument);

REGULATION_ErrorTypdef REGULATION_Init(void) {
    PRESSURE_SENSOR_Init();
    MOTORS_Init();

    xRegulationStatus = xQueueCreate( 2, sizeof( REGULATION_IrrActual_t ) );

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

REGULATION_ErrorTypdef REGULATION_Start(uint16_t u16SpeedRPM) {
	MOTORS_IrrigationStart(u16SpeedRPM);
	return REGULATION_ERROR_NONE;
}

REGULATION_ErrorTypdef REGULATION_Update(uint16_t u16SpeedRPM) {
	MOTORS_IrrigationUpdate(u16SpeedRPM);
	return REGULATION_ERROR_NONE;
}

REGULATION_ErrorTypdef REGULATION_Stop(void) {
	MOTORS_IrrigationStop();
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
	REGULATION_IrrActual_t status;
	bool bUpdate = false;

	for (;;) {
		sCarmenDataPool_t *sensorData;
		if (xIrrigationPressureData && xQueueReceive(xIrrigationPressureData, &sensorData, 50)) {
			status.sPressureData = sensorData->data;
			free_sensor_struct(sensorData);
//			DEBUG_SendTextFrame("Regulation_Thread PRESSURE: %f", status.fIrrigationActualPressureMMHG);
			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("Regulation_Thread PRESSURE: ---");
		}

		if (xIrrigationMotorSpeedRPM && xQueueReceive(xIrrigationMotorSpeedRPM, &status.fFlowRPM, 50)) {
//			DEBUG_SendTextFrame("Regulation_Thread SPEED: %f", status.fIrrigationActualSpeedRPM);
			bUpdate = true;
		} else {
//			DEBUG_SendTextFrame("Regulation_Thread SPEED: ---");
		}

		if (bUpdate) {
			status.fFlowLPM = FCE_fIrrGetFlowCoeff(status.fFlowRPM) * status.fFlowRPM;
			xQueueSend( xRegulationStatus, ( void * ) &status, ( TickType_t ) 0 );
		}
	}
}
