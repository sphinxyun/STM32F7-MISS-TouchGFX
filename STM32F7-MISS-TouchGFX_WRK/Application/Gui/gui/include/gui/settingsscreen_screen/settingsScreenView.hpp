#ifndef SETTINGSSCREEN_VIEW_HPP
#define SETTINGSSCREEN_VIEW_HPP

#include <gui_generated/settingsscreen_screen/settingsScreenViewBase.hpp>
#include <gui/settingsscreen_screen/settingsScreenPresenter.hpp>

class settingsScreenView : public settingsScreenViewBase
{
public:
    settingsScreenView();
    virtual ~settingsScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void incBrightness();
    void decBrightness();
    void brightnessValueUpdate(uint32_t u32Val);

protected:
    Unicode::UnicodeChar txtBuffer[10];

};

#endif // SETTINGSSCREEN_VIEW_HPP
