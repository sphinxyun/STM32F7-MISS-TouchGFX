#include <gui/main_screen/MainView.hpp>

MainView::MainView() {

}

void MainView::setupScreen() {
    MainViewBase::setupScreen();
}

void MainView::tearDownScreen() {
    MainViewBase::tearDownScreen();
}

void MainView::startRegulation() {
	presenter->startRegulation();
}

void MainView::stopRegulation() {
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

void MainView::showAlarmMessage(bool bVisible) {
	if (bVisible)
		alarmWindow.show();
	else
		alarmWindow.hide();
}

void MainView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}

void MainView::updateTemp(float fVal) {
	Unicode::snprintfFloats(actualTempBuffer, ACTUALTEMP_SIZE, "%3.2f", &fVal);
	actualTemp.setWildcard(actualTempBuffer);

	actualTemp.invalidate();
}

void MainView::updateCarmenErrStats(uint32_t u32Valid, uint32_t u32ECRC, uint32_t u32ESOF, uint32_t u32EUNK) {
	Unicode::snprintf(errorStatsBuffer1, ERRORSTATSBUFFER1_SIZE, "%d", u32Valid);
	Unicode::snprintf(errorStatsBuffer2, ERRORSTATSBUFFER2_SIZE, "%d", u32ECRC);

	errorStats.setWildcard1(errorStatsBuffer1);
	errorStats.setWildcard2(errorStatsBuffer2);
	errorStats.invalidate();
}

void MainView::actualPressureMMHGUpdate(float fVal) {
	int iVal = (int)fVal;
	if (iVal < 0)
		iVal = 0;
	Unicode::snprintf(actualPressureBuffer, ACTUALPRESSURE_SIZE, "%d", iVal);
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

void MainView::actualFlowDiagnosticsUpdate(uint16_t u16Diag, uint16_t u16Current) {
	Unicode::snprintf(motorDiagnosticsBuffer1, MOTORDIAGNOSTICSBUFFER1_SIZE, "%x", u16Diag);
	Unicode::snprintf(motorDiagnosticsBuffer2, MOTORDIAGNOSTICSBUFFER2_SIZE, "%x", u16Current);

	motorDiagnostics.setWildcard1(motorDiagnosticsBuffer1);
	motorDiagnostics.setWildcard2(motorDiagnosticsBuffer2);
	motorDiagnostics.invalidate();
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

void MainView::presetFlowRPMUpdate(int16_t i16Val) {
	Unicode::snprintf(presetFlowRPMBuffer, PRESETFLOWRPM_SIZE, "%d", (int)i16Val);
	presetFlowRPM.setWildcard(presetFlowRPMBuffer);

	presetFlowRPM.invalidate();
}
