#ifndef LEVEL_VIEW_HPP
#define LEVEL_VIEW_HPP

#include <gui_generated/level_screen/LevelViewBase.hpp>
#include <gui/level_screen/LevelPresenter.hpp>

class LevelView : public LevelViewBase
{
public:
    LevelView();
    virtual ~LevelView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void sliderChanged(int value);
    void mcuLoadUpdated(uint8_t mcuLoadValue);
protected:
};

#endif // LEVEL_VIEW_HPP
