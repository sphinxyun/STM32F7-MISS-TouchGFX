#ifndef __REGULATION_THREAD_H
#define __REGULATION_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pressure_sensor/pressure_sensor_thread.h"

typedef struct {
//	float fIrrigationActualPressureMMHG;
	float fIrrigationActualSpeedRPM;
	float fIrrigationActualFlowLPM;
//	float fIrrigationTemperatureC;
	sCarmenData_t sRawPressureSensorData;
} REGULATION_RegulatorStatus_t;

typedef enum {
	REGULATION_ERROR_NONE = 0
} REGULATION_ErrorTypdef;

REGULATION_ErrorTypdef REGULATION_Init(void);
REGULATION_ErrorTypdef REGULATION_DeInit(void);

REGULATION_ErrorTypdef REGULATION_Start(void);
REGULATION_ErrorTypdef REGULATION_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /*__REGULATION_THREAD_H */
