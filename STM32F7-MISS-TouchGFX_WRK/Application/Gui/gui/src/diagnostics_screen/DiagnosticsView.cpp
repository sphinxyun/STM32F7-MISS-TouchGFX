#include <gui/diagnostics_screen/DiagnosticsView.hpp>

DiagnosticsView::DiagnosticsView()
{

}

void DiagnosticsView::setupScreen()
{
    DiagnosticsViewBase::setupScreen();
}

void DiagnosticsView::tearDownScreen()
{
    DiagnosticsViewBase::tearDownScreen();
}

void DiagnosticsView::mcuLoadUpdated(uint8_t mcuLoadValue) {
	Unicode::snprintf(mcuLoadBuffer, MCULOAD_SIZE, "%d", mcuLoadValue);
	mcuLoad.setWildcard(mcuLoadBuffer);

	mcuLoad.invalidate();
}
