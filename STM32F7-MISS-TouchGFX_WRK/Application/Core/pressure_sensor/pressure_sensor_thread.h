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
		uint8_t fail_loressens_1 : 1;
		uint8_t fail_loressens_2 : 1;
		uint8_t fail_ADC1 : 1;
		uint8_t fail_ADC2_1 : 1;
		uint8_t fail_ADC2_2 : 1;
		uint8_t fail_ADC2_3 : 1;
		uint8_t fail_XTAL_osc : 1;
		uint8_t fail_clock : 1;
		uint8_t fail_ldo_vdda : 1;
		uint8_t dsp_usrflag1 : 1;
		uint8_t dsp_usrflag2 : 1;
		uint8_t dsp_newminmax : 1;
		uint8_t dsp_overlimit : 1;
		uint8_t dsp_overflow : 1;
		uint8_t dsp_running : 1;
		uint8_t dsp_watchdog : 1;
		uint8_t fail_memory : 1;
		uint8_t EEPROM_WIP : 1;
		uint8_t transfer_error : 1;
		uint8_t aux_in : 1;
		uint8_t cef : 1;
		uint8_t meas_data_rdy : 1;
	};
} uCarmentStatusReg_t;

typedef struct {
	float fPressureMMHG;
	float fTemperatureC;
	uCarmentStatusReg_t uStatus;

	struct {
		uint32_t u32Valid;
		uint32_t u32ErrCRC;
		uint32_t u32ErrUNK;
		uint32_t u32ErrSOF;
	} sErrorStats;
} sCarmenData_t;

typedef struct {
	bool inUse;
	sCarmenData_t data;
} sCarmenDataPool_t;

typedef enum {
  PRESSURE_SENSOR_ERROR_NONE = 0,
} PRESSURE_SENSOR_ErrorTypdef;

PRESSURE_SENSOR_ErrorTypdef  PRESSURE_SENSOR_Init(void);

PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Start(void);
PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Stop(void);

void free_sensor_struct(sCarmenDataPool_t *toRelease);

#ifdef __cplusplus
}
#endif

#endif /*__PRESSURE_SENSOR_THREAD_H */
