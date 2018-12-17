#ifndef WORKSCREEN_PRESENTER_HPP
#define WORKSCREEN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class workScreenView;

class workScreenPresenter : public Presenter, public ModelListener
{
public:
    workScreenPresenter(workScreenView& v);

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

    virtual ~workScreenPresenter() {};

private:
    workScreenPresenter();

    workScreenView& view;
};


#endif // WORKSCREEN_PRESENTER_HPP
