#include "motors_thread.h"

#include "drivers/irrigation.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

TaskHandle_t MotorsThreadId = 0;

QueueHandle_t xIrrigationMotorSpeedRPM = 0;

static void Motors_Thread(void * argument);

MOTORS_ErrorTypdef MOTORS_Init(void) {
	xIrrigationMotorSpeedRPM = xQueueCreate( 5, sizeof( float ) );

    xTaskCreate(Motors_Thread, "MotorsTask",
                512,
                NULL,
				tskIDLE_PRIORITY + 3,
                &MotorsThreadId);

//	osThreadDef(osMotors_Thread, Motors_Thread, osPriorityNormal, 0, 512);
//	MotorsThreadId = osThreadCreate(osThread(osMotors_Thread), NULL);

	Configure_ENCODER();
	Configure_PWM();

	MOTORS_Stop();

	return MOTORS_ERROR_NONE;
}

MOTORS_ErrorTypdef MOTORS_DeInit(void) {
	return MOTORS_ERROR_NONE;
}

MOTORS_ErrorTypdef MOTORS_Start(void) {
	if (MotorsThreadId != 0) {
		vTaskResume(MotorsThreadId);
	}

	StartSpeedMonitoring();

	return MOTORS_ERROR_NONE;
}

MOTORS_ErrorTypdef MOTORS_Stop(void) {
	if (MotorsThreadId != 0) {
		vTaskSuspend(MotorsThreadId);
	}

	return MOTORS_ERROR_NONE;
}

static void Motors_Thread(void * argument) {
	uint32_t ulNotifiedValue;

	float fSpeed_RPM;

	for (;;) {
		if (xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulNotifiedValue, 50)) {
			fSpeed_RPM = (float)ulNotifiedValue / 2000.0 * 60.0;
//			DEBUG_SendTextFrame("Motors_Thread: %f RPM (%d spd - x2004)", fSpeed_RPM, ulNotifiedValue);

			xQueueSend( xIrrigationMotorSpeedRPM, ( void * ) &fSpeed_RPM, ( TickType_t ) 0 );
		} else {

		}
	}
}
