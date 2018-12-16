#ifndef BOOTSCREEN_VIEW_HPP
#define BOOTSCREEN_VIEW_HPP

#include <gui_generated/bootscreen_screen/bootScreenViewBase.hpp>
#include <gui/bootscreen_screen/bootScreenPresenter.hpp>

class bootScreenView : public bootScreenViewBase
{
public:
    bootScreenView();
    virtual ~bootScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void incBrightness();
    void decBrightness();

    void brightnessValueUpdate(uint32_t u32Val);

protected:
    Unicode::UnicodeChar txtBuffer[10];
};

#endif // BOOTSCREEN_VIEW_HPP
