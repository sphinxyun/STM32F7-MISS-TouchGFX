#include <gui/settings_screen/SettingsView.hpp>
#include <gui/settings_screen/SettingsPresenter.hpp>

SettingsPresenter::SettingsPresenter(SettingsView& v)
    : view(v)
{

}

void SettingsPresenter::activate()
{
	view.mcuLoadUpdated(model->getMcuLoad());
	view.brightnessValueUpdate(model->getBrightnessSetting());
}

void SettingsPresenter::deactivate()
{

}

void SettingsPresenter::incBrightness(void) {
	model->incBrightness();
}

void SettingsPresenter::decBrightness(void) {
	model->decBrightness();
}

void SettingsPresenter::brightnessValueUpdate(uint32_t u32Val) {
	view.brightnessValueUpdate(u32Val);
};

void SettingsPresenter::mcuLoadUpdated(uint8_t mcuLoad) {
	view.mcuLoadUpdated(mcuLoad);
}
