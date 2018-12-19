#include <gui/diagnostics_screen/DiagnosticsView.hpp>
#include <gui/diagnostics_screen/DiagnosticsPresenter.hpp>

DiagnosticsPresenter::DiagnosticsPresenter(DiagnosticsView& v)
    : view(v)
{
}

void DiagnosticsPresenter::activate()
{
	view.mcuLoadUpdated(model->getMcuLoad());
}

void DiagnosticsPresenter::deactivate()
{

}

void DiagnosticsPresenter::mcuLoadUpdated(uint8_t mcuLoad) {
	view.mcuLoadUpdated(mcuLoad);
}
