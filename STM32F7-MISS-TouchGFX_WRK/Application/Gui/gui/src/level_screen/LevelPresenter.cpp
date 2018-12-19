#include <gui/level_screen/LevelView.hpp>
#include <gui/level_screen/LevelPresenter.hpp>

LevelPresenter::LevelPresenter(LevelView& v)
    : view(v)
{
}

void LevelPresenter::activate()
{
	view.mcuLoadUpdated(model->getMcuLoad());
}

void LevelPresenter::deactivate()
{

}

void LevelPresenter::mcuLoadUpdated(uint8_t mcuLoad) {
	view.mcuLoadUpdated(mcuLoad);
}
