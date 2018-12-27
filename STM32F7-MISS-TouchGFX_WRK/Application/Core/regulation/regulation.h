#ifndef __REGULATION_THREAD_H
#define __REGULATION_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pressure_sensor/pressure_sensor_thread.h"

typedef struct {
	uint8_t u8PressureMMHG;

	float fFlowLPM;
	uint16_t u16FlowRPM;
} REGULATION_IrrPresets_t;

typedef struct {
	sCarmenData_t sPressureData;

	float fFlowLPM;
	float fFlowRPM;
} REGULATION_IrrActual_t;

typedef enum {
	REGULATION_ERROR_NONE = 0
} REGULATION_ErrorTypdef;

REGULATION_ErrorTypdef REGULATION_Init(void);
REGULATION_ErrorTypdef REGULATION_DeInit(void);

REGULATION_ErrorTypdef REGULATION_TaskStart(void);
REGULATION_ErrorTypdef REGULATION_TaskStop(void);

//REGULATION_ErrorTypdef REGULATION_Start(uint16_t u16SpeedRPM);
//REGULATION_ErrorTypdef REGULATION_Update(uint16_t u16SpeedRPM);
//REGULATION_ErrorTypdef REGULATION_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /*__REGULATION_THREAD_H */
