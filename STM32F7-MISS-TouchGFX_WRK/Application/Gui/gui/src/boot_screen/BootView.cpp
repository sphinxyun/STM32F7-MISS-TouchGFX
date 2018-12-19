#include <gui/boot_screen/BootView.hpp>

BootView::BootView()
{

}

void BootView::setupScreen()
{
    BootViewBase::setupScreen();
}

void BootView::tearDownScreen()
{
    BootViewBase::tearDownScreen();
}

void BootView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}
