#include <motors/motors.h>
#include "drivers/irrigation.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "debug.h"

TaskHandle_t MotorsTaskId = 0;

QueueHandle_t xIrrigationMotorSpeedRPM = 0;

static void Motors_Thread(void * argument);

MOTORS_ErrorTypdef MOTORS_Init(void) {


	xIrrigationMotorSpeedRPM = xQueueCreate( 5, sizeof( float ) );

    xTaskCreate(Motors_Thread, "MotorsTask",
                512,
                NULL,
				tskIDLE_PRIORITY + 3,
                &MotorsTaskId);

	IRRIGATION_Configure();


	MOTORS_TaskStop();

	return MOTORS_ERROR_NONE;
}

//void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
//{
//  if(hspi->Instance == SPIx)
//  {
//    /*##-1- Reset peripherals ##################################################*/
//    SPIx_FORCE_RESET();
//    SPIx_RELEASE_RESET();
//
//    /*##-2- Disable peripherals and GPIO Clocks ################################*/
//    /* Configure SPI SCK as alternate function  */
//    HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
//    /* Configure SPI MISO as alternate function  */
//    HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
//    /* Configure SPI MOSI as alternate function  */
//    HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
//  }
//}

MOTORS_ErrorTypdef MOTORS_DeInit(void) {
	return MOTORS_ERROR_NONE;
}

MOTORS_ErrorTypdef MOTORS_IrrigationStart(int16_t i16PWM) {
	IRRIGATION_Start(i16PWM);
	return MOTORS_ERROR_NONE;
}

MOTORS_ErrorTypdef MOTORS_IrrigationUpdate(int16_t i16PWM) {
	IRRIGATION_UpdateSpeed(i16PWM);
	return MOTORS_ERROR_NONE;
}

MOTORS_ErrorTypdef MOTORS_IrrigationStop(void) {
	IRRIGATION_Stop();
	return MOTORS_ERROR_NONE;
}

MOTORS_ErrorTypdef MOTORS_TaskStart(void) {
	if (MotorsTaskId != 0) {
		vTaskResume(MotorsTaskId);
	}

	IRRIGATION_StartSpeedMonitoring();

	return MOTORS_ERROR_NONE;
}

MOTORS_ErrorTypdef MOTORS_TaskStop(void) {
	if (MotorsTaskId != 0) {
		vTaskSuspend(MotorsTaskId);
	}

	return MOTORS_ERROR_NONE;
}

static void Motors_Thread(void * argument) {
	int32_t i32NotifiedValue;

	IRRIGATION_ReadDiagnostics();
	TickType_t lastRun = xTaskGetTickCount();

	float fSpeed_RPM;

	for (;;) {
		if (xTaskNotifyWait(0x00000000, 0xFFFFFFFF, (uint32_t *)&i32NotifiedValue, 50)) {
			//1200 (max assumed rpms) / 60 (seconds) / (1 / 0.0125) (sampling freq = 80Hz)
			//fSpeed_RPM = (float)i32NotifiedValue / 250.0 * 60.0;
			fSpeed_RPM = (float)i32NotifiedValue / 25.0 * 60.0;
			AD_SIG_2_TOGGLE;

			xQueueSend( xIrrigationMotorSpeedRPM, ( void * ) &fSpeed_RPM, ( TickType_t ) 0 );
		} else {

		}

		if (xTaskGetTickCount() > lastRun + 500) {
			lastRun = xTaskGetTickCount();
			IRRIGATION_ReadDiagnostics();
		}
	}
}
