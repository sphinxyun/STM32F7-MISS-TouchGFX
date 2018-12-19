#include <gui/main_screen/MainView.hpp>
#include <gui/main_screen/MainPresenter.hpp>

MainPresenter::MainPresenter(MainView& v)
    : view(v)
{
}

void MainPresenter::activate()
{

}

void MainPresenter::deactivate()
{

}

void MainPresenter::actualPressureUpdate(float fVal) {
	view.actualPressureUpdate(fVal);
}

void MainPresenter::actualFlowLPMUpdate(float fVal) {
	view.actualFlowLPMUpdate(fVal);
}

void MainPresenter::actualFlowRPMUpdate(float fVal) {
	view.actualFlowRPMUpdate(fVal);
}

void MainPresenter::startRegulation()
{
	model->startRegulation();
}

void MainPresenter::stopRegulation()
{
	model->stopRegulation();
}

void MainPresenter::incPressureBtn() {
	model->incPressureBtn();
}

void MainPresenter::decPressureBtn() {
	model->decPressureBtn();
}

void MainPresenter::incFlowBtn() {
	model->incFlowBtn();
}

void MainPresenter::decFlowBtn() {
	model->decFlowBtn();
}

void MainPresenter::mcuLoadUpdated(uint8_t mcuLoad) {
	view.mcuLoadUpdated(mcuLoad);
}

