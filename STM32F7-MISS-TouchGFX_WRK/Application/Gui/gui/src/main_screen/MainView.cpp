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

void MainView::actualPressureUpdate(float fVal) {
	Unicode::snprintf(actualPressureBuffer, ACTUALPRESSURE_SIZE, "%d", (int)fVal);
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
	actualFlowRPM.setWildcard(actualFlowRPMBuffer);

	actualFlowRPM.invalidate();
}
