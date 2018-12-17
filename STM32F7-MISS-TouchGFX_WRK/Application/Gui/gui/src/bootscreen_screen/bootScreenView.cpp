#include <gui/bootscreen_screen/bootScreenView.hpp>

bootScreenView::bootScreenView()
{

}

void bootScreenView::setupScreen()
{
    bootScreenViewBase::setupScreen();
}

void bootScreenView::tearDownScreen()
{
    bootScreenViewBase::tearDownScreen();
}

void bootScreenView::incBrightness() {
	presenter->incBrightness();
}

void bootScreenView::decBrightness() {
	presenter->decBrightness();
}

void bootScreenView::brightnessValueUpdate(uint32_t u32Val) {
	Unicode::snprintf(txtBuffer, 10, "%d", u32Val);
	brightnessValue.setWildcard(txtBuffer);
    brightnessValue.resizeToCurrentText();

	brightnessValue.invalidate();

}
