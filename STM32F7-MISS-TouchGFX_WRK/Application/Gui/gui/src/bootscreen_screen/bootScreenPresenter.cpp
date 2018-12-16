#include <gui/bootscreen_screen/bootScreenView.hpp>
#include <gui/bootscreen_screen/bootScreenPresenter.hpp>

bootScreenPresenter::bootScreenPresenter(bootScreenView& v)
    : view(v)
{
}

void bootScreenPresenter::activate()
{

}

void bootScreenPresenter::deactivate()
{

}

void bootScreenPresenter::incBrightness(void) {
	model->incBrightness();
}

void bootScreenPresenter::decBrightness(void) {
	model->decBrightness();
}

void bootScreenPresenter::brightnessValueUpdate(uint32_t u32Val) {
	view.brightnessValueUpdate(u32Val);
};

