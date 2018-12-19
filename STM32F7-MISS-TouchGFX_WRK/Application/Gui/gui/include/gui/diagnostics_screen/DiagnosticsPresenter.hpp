#ifndef DIAGNOSTICS_PRESENTER_HPP
#define DIAGNOSTICS_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DiagnosticsView;

class DiagnosticsPresenter : public Presenter, public ModelListener
{
public:
    DiagnosticsPresenter(DiagnosticsView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~DiagnosticsPresenter() {};

private:
    DiagnosticsPresenter();

    DiagnosticsView& view;
};


#endif // DIAGNOSTICS_PRESENTER_HPP
