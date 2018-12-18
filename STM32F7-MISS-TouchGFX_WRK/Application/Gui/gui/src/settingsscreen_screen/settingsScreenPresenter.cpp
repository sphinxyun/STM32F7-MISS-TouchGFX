#include <gui/settingsscreen_screen/settingsScreenView.hpp>
#include <gui/settingsscreen_screen/settingsScreenPresenter.hpp>

settingsScreenPresenter::settingsScreenPresenter(settingsScreenView& v)
    : view(v)
{
}

void settingsScreenPresenter::activate()
{
	view.mcuLoadUpdated(model->getMcuLoad());
	view.brightnessValueUpdate(model->getBrightnessSetting());
}

void settingsScreenPresenter::deactivate()
{

}

void settingsScreenPresenter::incBrightness(void) {
	model->incBrightness();
}

void settingsScreenPresenter::decBrightness(void) {
	model->decBrightness();
}

void settingsScreenPresenter::brightnessValueUpdate(uint32_t u32Val) {
	view.brightnessValueUpdate(u32Val);
};

void settingsScreenPresenter::mcuLoadUpdated(uint8_t mcuLoad) {
	view.mcuLoadUpdated(mcuLoad);
}
