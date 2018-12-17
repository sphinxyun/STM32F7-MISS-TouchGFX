#include <gui/workscreen_screen/workScreenView.hpp>

workScreenView::workScreenView()
{

}

void workScreenView::setupScreen()
{
    workScreenViewBase::setupScreen();
}

void workScreenView::tearDownScreen()
{
    workScreenViewBase::tearDownScreen();
}

void workScreenView::startRegulation()
{
	presenter->startRegulation();
}

void workScreenView::stopRegulation()
{
	presenter->stopRegulation();
}

void workScreenView::actualPressureUpdate(float fVal) {
	Unicode::snprintf(actualPressureBuffer, ACTUALPRESSURE_SIZE, "%d", (int)fVal);
	actualPressure.setWildcard(actualPressureBuffer);
	actualPressure.resizeToCurrentText();

	actualPressure.invalidate();
}

void workScreenView::actualFlowLPMUpdate(float fVal) {
	Unicode::snprintf(actualFlowLPMBuffer, ACTUALFLOWLPM_SIZE, "%d", (int)fVal);
	actualFlowLPM.setWildcard(actualFlowLPMBuffer);
	actualFlowLPM.resizeToCurrentText();

	actualFlowLPM.invalidate();
}

void workScreenView::actualFlowRPMUpdate(float fVal) {
	Unicode::snprintf(actualFlowRPMBuffer, ACTUALFLOWRPM_SIZE, "%d", (int)fVal);
	actualFlowRPM.setWildcard(actualFlowRPMBuffer);
	actualFlowRPM.resizeToCurrentText();

	actualFlowRPM.invalidate();
}
