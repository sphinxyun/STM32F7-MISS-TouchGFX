#include "settings_app.h"

typedef union {
	uint32_t d32;
	struct {
		uint32_t brightness :8;
		uint32_t volume :8;
		uint32_t reserved :16;
	} b;
} DeviceSettingsTypeDef;

static DeviceSettingsTypeDef DeviceSettings;

SETTINGS_ProgramSettingsTypedef ProgramSettings;

const SETTINGS_ProgramSettingsTypedef *SETTINGS_Get(void) {
	return &ProgramSettings;
}


SETTINGS_ErrorTypdef SETTINGS_Init(void) {
	DeviceSettings.b.brightness = 40;

	ProgramSettings.u32IrrigationPressure = 35;
	ProgramSettings.fIrrigationFlow = 0.5;

	ProgramSettings.u32IrrigationPressureMinValue = 20;
	ProgramSettings.u32IrrigationPressureMaxValue = 150;
	ProgramSettings.u32IrrigationPressureIncValue = 5;

	return SETTINGS_ERROR_NONE;
}

int SETTINGS_GetBrightness(void) {
	return DeviceSettings.b.brightness;
}

SETTINGS_ErrorTypdef SETTINGS_CheckHBrightness(void) {
	if ((DeviceSettings.b.brightness + 5) > 100)
		return SETTINGS_VALUE_AT_LIMIT;
	else
		return SETTINGS_ERROR_NONE;
}

extern bool SetBacklight(uint32_t backlight);
SETTINGS_ErrorTypdef SETTINGS_IncBrightness(void) {
	bool bOnLimit = false;

	if (SETTINGS_CheckHBrightness() == SETTINGS_VALUE_AT_LIMIT) {
		bOnLimit = true;
		DeviceSettings.b.brightness = 100;
	} else
		DeviceSettings.b.brightness += 5;

	TOUCHGFT_SetBacklight(DeviceSettings.b.brightness);

	if (bOnLimit || (DeviceSettings.b.brightness >= 100))
		return SETTINGS_VALUE_AT_LIMIT;
	else
		return SETTINGS_ERROR_NONE;
}

SETTINGS_ErrorTypdef SETTINGS_CheckLBrightness(void) {
	if ((DeviceSettings.b.brightness - 5) < 20)
		return SETTINGS_VALUE_AT_LIMIT;
	else
		return SETTINGS_ERROR_NONE;
}

SETTINGS_ErrorTypdef SETTINGS_DecBrightness(void) {
	bool bOnLimit = false;

	if (SETTINGS_CheckLBrightness() == SETTINGS_VALUE_AT_LIMIT) {
		bOnLimit = true;
		DeviceSettings.b.brightness = 20;
	} else
		DeviceSettings.b.brightness -= 5;

	TOUCHGFT_SetBacklight(DeviceSettings.b.brightness);

	if (bOnLimit | (DeviceSettings.b.brightness <= 20))
		return SETTINGS_VALUE_AT_LIMIT;
	else
		return SETTINGS_ERROR_NONE;
}

SETTINGS_ErrorTypdef SETTINGS_DeInit(void) {
//	k_BkupSaveParameter(SETTINGS_BKP, DeviceSettings.d32);

	return SETTINGS_ERROR_NONE;
}

