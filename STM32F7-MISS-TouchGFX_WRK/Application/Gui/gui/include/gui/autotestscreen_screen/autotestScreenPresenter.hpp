#ifndef AUTOTESTSCREEN_PRESENTER_HPP
#define AUTOTESTSCREEN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class autotestScreenView;

class autotestScreenPresenter : public Presenter, public ModelListener
{
public:
    autotestScreenPresenter(autotestScreenView& v);

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

    virtual ~autotestScreenPresenter() {};

private:
    autotestScreenPresenter();

    autotestScreenView& view;
};


#endif // AUTOTESTSCREEN_PRESENTER_HPP
