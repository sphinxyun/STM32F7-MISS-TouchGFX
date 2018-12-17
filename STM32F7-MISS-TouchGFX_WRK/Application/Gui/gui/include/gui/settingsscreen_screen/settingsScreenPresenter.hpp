#ifndef SETTINGSSCREEN_PRESENTER_HPP
#define SETTINGSSCREEN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class settingsScreenView;

class settingsScreenPresenter : public Presenter, public ModelListener
{
public:
    settingsScreenPresenter(settingsScreenView& v);

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

    virtual ~settingsScreenPresenter() {};

    //from View
    void incBrightness(void);
    void decBrightness(void);

    void brightnessValueUpdate(uint32_t u32Val);


private:
    settingsScreenPresenter();

    settingsScreenView& view;
};


#endif // SETTINGSSCREEN_PRESENTER_HPP
