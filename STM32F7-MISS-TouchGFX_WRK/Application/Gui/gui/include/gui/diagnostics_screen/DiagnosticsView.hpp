#ifndef DIAGNOSTICS_VIEW_HPP
#define DIAGNOSTICS_VIEW_HPP

#include <gui_generated/diagnostics_screen/DiagnosticsViewBase.hpp>
#include <gui/diagnostics_screen/DiagnosticsPresenter.hpp>

class DiagnosticsView : public DiagnosticsViewBase
{
public:
    DiagnosticsView();
    virtual ~DiagnosticsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void mcuLoadUpdated(uint8_t mcuLoadValue);

protected:
};

#endif // DIAGNOSTICS_VIEW_HPP
