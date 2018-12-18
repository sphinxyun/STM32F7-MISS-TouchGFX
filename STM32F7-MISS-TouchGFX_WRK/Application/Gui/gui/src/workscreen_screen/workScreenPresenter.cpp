#include <gui/workscreen_screen/workScreenView.hpp>
#include <gui/workscreen_screen/workScreenPresenter.hpp>

workScreenPresenter::workScreenPresenter(workScreenView& v) :
	view(v)
{
}

void workScreenPresenter::activate()
{

}

void workScreenPresenter::deactivate()
{

}

void workScreenPresenter::actualPressureUpdate(float fVal) {
	view.actualPressureUpdate(fVal);
}

void workScreenPresenter::actualFlowLPMUpdate(float fVal) {
	view.actualFlowLPMUpdate(fVal);
}

void workScreenPresenter::actualFlowRPMUpdate(float fVal) {
	view.actualFlowRPMUpdate(fVal);
}

void workScreenPresenter::startRegulation()
{
	model->startRegulation();
}

void workScreenPresenter::stopRegulation()
{
	model->stopRegulation();
}

void workScreenPresenter::incPressureBtn() {
	model->incPressureBtn();
}

void workScreenPresenter::decPressureBtn() {
	model->decPressureBtn();
}

void workScreenPresenter::incFlowBtn() {
	model->incFlowBtn();
}

void workScreenPresenter::decFlowBtn() {
	model->decFlowBtn();
}

void workScreenPresenter::mcuLoadUpdated(uint8_t mcuLoad) {
	view.mcuLoadUpdated(mcuLoad);
}
