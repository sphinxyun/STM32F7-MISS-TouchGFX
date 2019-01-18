#ifndef __REGULATION_THREAD_H
#define __REGULATION_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pressure_sensor/pressure_sensor_thread.h"
#include "motors/motors.h"

typedef enum {
	eRegIdle = 0,
	eRegIrrigation
} REGULATION_Mode_t;

typedef struct {
	REGULATION_Mode_t eRegMode;

	uint8_t u8PressureMMHG;

	float fFlowLPM;
	int16_t i16FlowRPM;
} REGULATION_IrrPresets_t;

typedef struct {
	sCarmenData_t sPressureData;

	float fFlowLPM;
	sMotorData_t sMotorData;
} REGULATION_IrrActual_t;

typedef enum {
	REGULATION_ERROR_NONE = 0
} REGULATION_ErrorTypdef;

REGULATION_ErrorTypdef REGULATION_Init(void);
REGULATION_ErrorTypdef REGULATION_DeInit(void);

REGULATION_ErrorTypdef REGULATION_TaskStart(void);
REGULATION_ErrorTypdef REGULATION_TaskStop(void);

#ifdef __cplusplus
}
#endif

#endif /*__REGULATION_THREAD_H */
