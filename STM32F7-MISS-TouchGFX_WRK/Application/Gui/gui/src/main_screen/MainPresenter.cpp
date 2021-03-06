#include <gui/main_screen/MainView.hpp>
#include <gui/main_screen/MainPresenter.hpp>

MainPresenter::MainPresenter(MainView& v)
    : view(v) {

}

void MainPresenter::activate() {
	view.mcuLoadUpdated(model->getMcuLoad());

	view.actualPressureMMHGUpdate(model->getGuiStatus().sIrrigationActual.sPressureData.fPressureMMHG);
	view.actualFlowLPMUpdate(model->getGuiStatus().sIrrigationActual.fFlowLPM);
	view.actualFlowRPMUpdate(model->getGuiStatus().sIrrigationActual.sMotorData.fSpeedRPM);
	view.actualFlowDiagnosticsUpdate((model->getGuiStatus().sIrrigationActual.sMotorData.u32Diagnostics >> 16) & 0x0000FFFF,
			(model->getGuiStatus().sIrrigationActual.sMotorData.u32Diagnostics >> 0) & 0x0000FFFF);

	view.presetPressureMMHGUpdate(model->getGuiStatus().sIrrigationPresets.u8PressureMMHG);
	view.presetFlowLPMUpdate(model->getGuiStatus().sIrrigationPresets.fFlowLPM);
	view.presetFlowRPMUpdate(model->getGuiStatus().sIrrigationPresets.i16FlowRPM);
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

void MainPresenter::actualFlowDiagnosticsUpdate(uint16_t u16Diag, uint16_t u16Current) {
	view.actualFlowDiagnosticsUpdate(u16Diag, u16Current);
}

void MainPresenter::presetPressureMMHGUpdate(uint8_t u8Val) {
	view.presetPressureMMHGUpdate(u8Val);
}

void MainPresenter::presetFlowLPMUpdate(float fVal) {
	view.presetFlowLPMUpdate(fVal);
}

void MainPresenter::presetFlowRPMUpdate(int16_t i16Val) {
	view.presetFlowRPMUpdate(i16Val);
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

