#include <gui/bootscreen_screen/bootScreenView.hpp>
#include <gui/bootscreen_screen/bootScreenPresenter.hpp>

bootScreenPresenter::bootScreenPresenter(bootScreenView& v)
    : view(v)
{
}

void bootScreenPresenter::activate()
{
	view.mcuLoadUpdated(model->getMcuLoad());
}

void bootScreenPresenter::deactivate()
{

}

void bootScreenPresenter::mcuLoadUpdated(uint8_t mcuLoad) {
	view.mcuLoadUpdated(mcuLoad);
}


