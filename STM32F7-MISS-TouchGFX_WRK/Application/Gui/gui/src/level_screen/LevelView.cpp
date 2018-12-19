#include <gui/level_screen/LevelView.hpp>

LevelView::LevelView()
{

}

void LevelView::setupScreen()
{
    LevelViewBase::setupScreen();
}

void LevelView::tearDownScreen()
{
    LevelViewBase::tearDownScreen();
}

void LevelView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}
