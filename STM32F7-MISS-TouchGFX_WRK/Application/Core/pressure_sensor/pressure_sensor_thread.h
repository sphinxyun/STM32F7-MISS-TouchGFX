#ifndef __PRESSURE_SENSOR_THREAD_H
#define __PRESSURE_SENSOR_THREAD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "global.h"

typedef union {
	uint8_t u8Stat[3];
	struct {
		uint8_t fail_hires : 1;
		uint8_t fail_capsens_ovp : 1;
		uint8_t fail_loressens_1: 1;
		uint8_t fail_loressens_2: 1;
		uint8_t fail_ADC1: 1;
		uint8_t fail_ADC2_1: 1;
		uint8_t fail_ADC2_2: 1;
		uint8_t fail_ADC2_3: 1;
		uint8_t stat1;
		uint8_t stat2 : 7;
		uint8_t meas_data_rdy : 1;
	};
} uCarmentStatusReg_t;

typedef struct {
	float fPressureMMHG;
	float fTemperatureC;
	uCarmentStatusReg_t uStatus;
} sCarmenData_t;

typedef enum {
  PRESSURE_SENSOR_ERROR_NONE = 0,
} PRESSURE_SENSOR_ErrorTypdef;

PRESSURE_SENSOR_ErrorTypdef  PRESSURE_SENSOR_Init(void);

PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Start(void);
PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /*__PRESSURE_SENSOR_THREAD_H */
