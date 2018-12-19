#include <gui/boot_screen/BootView.hpp>
#include <gui/boot_screen/BootPresenter.hpp>

BootPresenter::BootPresenter(BootView& v)
    : view(v)
{
}

void BootPresenter::activate()
{
	view.mcuLoadUpdated(model->getMcuLoad());
}

void BootPresenter::deactivate()
{

}

void BootPresenter::mcuLoadUpdated(uint8_t mcuLoad) {
	view.mcuLoadUpdated(mcuLoad);
}
