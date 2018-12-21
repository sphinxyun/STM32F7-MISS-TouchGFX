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

void LevelView::sliderChanged(int value) {
	Unicode::snprintf(levelValBuffer, LEVELVAL_SIZE, "%d", levelSlider.getValue() - 100);
	levelVal.setWildcard(levelValBuffer);

	levelVal.invalidate();
}

void LevelView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}
