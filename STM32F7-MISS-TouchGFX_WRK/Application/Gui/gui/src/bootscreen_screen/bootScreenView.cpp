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

void bootScreenView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}
