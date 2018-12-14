#ifndef BOOTSCREEN_PRESENTER_HPP
#define BOOTSCREEN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class bootScreenView;

class bootScreenPresenter : public Presenter, public ModelListener
{
public:
    bootScreenPresenter(bootScreenView& v);

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

    virtual ~bootScreenPresenter() {};

    //from View
    void incBrightness(void);
    void decBrightness(void);

private:
    bootScreenPresenter();

    bootScreenView& view;
};


#endif // BOOTSCREEN_PRESENTER_HPP
