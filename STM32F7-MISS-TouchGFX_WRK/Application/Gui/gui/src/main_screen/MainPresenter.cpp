#include <gui/main_screen/MainView.hpp>
#include <gui/main_screen/MainPresenter.hpp>

MainPresenter::MainPresenter(MainView& v)
    : view(v) {

}

void MainPresenter::activate() {
	view.actualPressureMMHGUpdate(model->getGuiStatus().sIrrigationActual.sRawPressureSensorData.fPressureMMHG);
	view.actualFlowLPMUpdate(model->getGuiStatus().sIrrigationActual.fIrrigationActualFlowLPM);
	view.actualFlowRPMUpdate(model->getGuiStatus().sIrrigationActual.fIrrigationActualSpeedRPM);

	view.presetPressureMMHGUpdate(model->getGuiStatus().sIrrigationPreset.u8IrrigationPresetPressureMMHG);
	view.presetFlowLPMUpdate(model->getGuiStatus().sIrrigationPreset.fIrrigationPresetFlowLPM);
	view.presetFlowRPMUpdate(model->getGuiStatus().sIrrigationPreset.u16IrrigationPresetFlowRPM);
}

void MainPresenter::deactivate() {

}

void MainPresenter::showAlarmMessage(bool bVisible) {
	view.showAlarmMessage(bVisible);
}

void MainPresenter::actualPressureMMHGUpdate(float fVal) {
	view.actualPressureMMHGUpdate(fVal);
}

void MainPresenter::actualFlowLPMUpdate(float fVal) {
	view.actualFlowLPMUpdate(fVal);
}

void MainPresenter::actualFlowRPMUpdate(float fVal) {
	view.actualFlowRPMUpdate(fVal);
}

void MainPresenter::presetPressureMMHGUpdate(uint8_t u8Val) {
	view.presetPressureMMHGUpdate(u8Val);
}

void MainPresenter::presetFlowLPMUpdate(float fVal) {
	view.presetFlowLPMUpdate(fVal);
}

void MainPresenter::presetFlowRPMUpdate(uint16_t u16Val) {
	view.presetFlowRPMUpdate(u16Val);
}

void MainPresenter::startRegulation() {
	model->startRegulation();
}

void MainPresenter::stopRegulation() {
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

void MainPresenter::updateTemp(float fVal) {
	view.updateTemp(fVal);
}

void MainPresenter::updateCarmenErrStats(uint32_t u32Valid, uint32_t u32ECRC, uint32_t u32ESOF, uint32_t u32EUNK) {
	view.updateCarmenErrStats(u32Valid, u32ECRC, u32ESOF, u32EUNK);
}

