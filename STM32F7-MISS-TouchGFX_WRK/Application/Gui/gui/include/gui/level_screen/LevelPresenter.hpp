#ifndef LEVEL_PRESENTER_HPP
#define LEVEL_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class LevelView;

class LevelPresenter : public Presenter, public ModelListener
{
public:
    LevelPresenter(LevelView& v);

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

    virtual ~LevelPresenter() {};

    void mcuLoadUpdated(uint8_t mcuLoad);

private:
    LevelPresenter();

    LevelView& view;
};


#endif // LEVEL_PRESENTER_HPP
