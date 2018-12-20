#ifndef __SETTINGS_APP_H
#define __SETTINGS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"

//#define SETTINGS_BKP   RTC_BKP_DR0


typedef struct {
	uint32_t u32IrrigationPressure;
	uint32_t u32IrrigationPressureMinValue;
	uint32_t u32IrrigationPressureMaxValue;
	uint32_t u32IrrigationPressureIncValue;

	uint16_t u16IrrigationFlowRPM;
	uint16_t u16IrrigationFlowRPMMinValue;
	uint16_t u16IrrigationFlowRPMMaxValue;
	uint16_t u16IrrigationFlowRPMIncValue;

	float fIrrigationFlowLPM;
	float fIrrigationFlowLPMMinValue;
	float fIrrigationFlowLPMMaxValue;
	float fIrrigationFlowLPMIncValue;
} SETTINGS_ProgramSettingsTypedef;

typedef enum {
	SETTINGS_ERROR_NONE = 0,
	SETTINGS_VALUE_AT_LIMIT
} SETTINGS_ErrorTypdef;

const SETTINGS_ProgramSettingsTypedef *SETTINGS_Get(void);

SETTINGS_ErrorTypdef SETTINGS_Init(void);
SETTINGS_ErrorTypdef SETTINGS_DeInit(void);

SETTINGS_ErrorTypdef SETTINGS_CheckHBrightness(void);
SETTINGS_ErrorTypdef SETTINGS_IncBrightness(void);
SETTINGS_ErrorTypdef SETTINGS_CheckLBrightness(void);
SETTINGS_ErrorTypdef SETTINGS_DecBrightness(void);
//SETTINGS_ErrorTypdef SETTINGS_SetBrightness(int brightness);
int SETTINGS_GetBrightness(void);

#ifdef __cplusplus
}
#endif

#endif /*__SETTINGS_APP_H */
