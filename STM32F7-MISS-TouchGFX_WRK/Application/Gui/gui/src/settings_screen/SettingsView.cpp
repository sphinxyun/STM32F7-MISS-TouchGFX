#include <gui/settings_screen/SettingsView.hpp>

SettingsView::SettingsView()
{

}

void SettingsView::setupScreen()
{
    SettingsViewBase::setupScreen();
}

void SettingsView::tearDownScreen()
{
    SettingsViewBase::tearDownScreen();
}


void SettingsView::incBrightness() {
	presenter->incBrightness();
}

void SettingsView::decBrightness() {
	presenter->decBrightness();
}

void SettingsView::brightnessValueUpdate(uint32_t u32Val) {
	Unicode::snprintf(brightnessValueBuffer, BRIGHTNESSVALUE_SIZE, "%d", u32Val);
	brightnessValue.setWildcard(brightnessValueBuffer);

	brightnessValue.invalidate();
}

void SettingsView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}
