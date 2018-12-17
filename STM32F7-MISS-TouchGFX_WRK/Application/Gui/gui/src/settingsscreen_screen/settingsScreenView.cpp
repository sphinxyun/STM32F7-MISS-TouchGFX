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
    brightnessValue.resizeToCurrentText();

	brightnessValue.invalidate();

}
