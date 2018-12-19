#include <gui/main_screen/MainView.hpp>

MainView::MainView()
{

}

void MainView::setupScreen()
{
    MainViewBase::setupScreen();
}

void MainView::tearDownScreen()
{
    MainViewBase::tearDownScreen();
}

void MainView::startRegulation()
{
	presenter->startRegulation();
}

void MainView::stopRegulation()
{
	presenter->stopRegulation();
}

void MainView::incPressureBtn() {
	presenter->incPressureBtn();
}

void MainView::decPressureBtn() {
	presenter->decPressureBtn();
}

void MainView::incFlowBtn() {
	presenter->incFlowBtn();
}

void MainView::decFlowBtn() {
	presenter->decFlowBtn();
}

void MainView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}

void MainView::actualPressureMMHGUpdate(float fVal) {
	Unicode::snprintf(actualPressureBuffer, ACTUALPRESSURE_SIZE, "%d", (int)fVal);
//	Unicode::snprintfFloats(actualPressureBuffer, ACTUALPRESSURE_SIZE, "%4.2f", &fVal);
	actualPressure.setWildcard(actualPressureBuffer);

	actualPressure.invalidate();
}

void MainView::actualFlowLPMUpdate(float fVal) {
	Unicode::snprintfFloats(actualFlowLPMBuffer, ACTUALFLOWLPM_SIZE, "%3.2f", &fVal);
	actualFlowLPM.setWildcard(actualFlowLPMBuffer);

	actualFlowLPM.invalidate();
}

void MainView::actualFlowRPMUpdate(float fVal) {
	Unicode::snprintf(actualFlowRPMBuffer, ACTUALFLOWRPM_SIZE, "%d", (int)fVal);
//	Unicode::snprintfFloats(actualFlowRPMBuffer, ACTUALFLOWRPM_SIZE, "%4.2f", &fVal);
	actualFlowRPM.setWildcard(actualFlowRPMBuffer);

	actualFlowRPM.invalidate();
}

void MainView::presetPressureMMHGUpdate(uint8_t fVal) {
	Unicode::snprintf(presetlPressureBuffer, PRESETLPRESSURE_SIZE, "%d", (int)fVal);
	presetlPressure.setWildcard(presetlPressureBuffer);

	presetlPressure.invalidate();
}

void MainView::presetFlowLPMUpdate(float fVal) {
	Unicode::snprintfFloats(presetFlowLPMBuffer, PRESETFLOWLPM_SIZE, "%3.2f", &fVal);
	presetFlowLPM.setWildcard(presetFlowLPMBuffer);

	presetFlowLPM.invalidate();
}

void MainView::presetFlowRPMUpdate(uint16_t fVal) {
	Unicode::snprintf(presetFlowRPMBuffer, PRESETFLOWRPM_SIZE, "%d", (int)fVal);
	presetFlowRPM.setWildcard(presetFlowRPMBuffer);

	presetFlowRPM.invalidate();
}
