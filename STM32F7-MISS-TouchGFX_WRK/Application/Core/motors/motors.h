#ifndef __MOTORS_THREAD_H
#define __MOTORS_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"

#include "drivers/irrigation.h"

typedef struct {
	float fSpeedRPM;
	uint32_t u32Diagnostics;
} sMotorData_t;

typedef enum {
	MOTORS_ERROR_NONE = 0
} MOTORS_ErrorTypdef;

MOTORS_ErrorTypdef MOTORS_Init(void);
MOTORS_ErrorTypdef MOTORS_DeInit(void);

MOTORS_ErrorTypdef MOTORS_TaskStart(void);
MOTORS_ErrorTypdef MOTORS_TaskStop(void);

MOTORS_ErrorTypdef MOTORS_IrrigationStart(int16_t i16PWM);
MOTORS_ErrorTypdef MOTORS_IrrigationUpdate(int16_t i16PWM);
MOTORS_ErrorTypdef MOTORS_IrrigationStop(void);

#ifdef __cplusplus
}
#endif

#endif /*__MOTORS_THREAD_H */
