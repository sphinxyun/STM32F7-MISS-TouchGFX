#include <gui/settingsscreen_screen/settingsScreenView.hpp>

settingsScreenView::settingsScreenView()
{

}

void settingsScreenView::setupScreen()
{
    settingsScreenViewBase::setupScreen();
}

void settingsScreenView::tearDownScreen()
{
    settingsScreenViewBase::tearDownScreen();
}

void settingsScreenView::incBrightness() {
	presenter->incBrightness();
}

void settingsScreenView::decBrightness() {
	presenter->decBrightness();
}

void settingsScreenView::brightnessValueUpdate(uint32_t u32Val) {
	Unicode::snprintf(txtBuffer, 10, "%d", u32Val);
	brightnessValue.setWildcard(txtBuffer);

	brightnessValue.invalidate();
}

void settingsScreenView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}
