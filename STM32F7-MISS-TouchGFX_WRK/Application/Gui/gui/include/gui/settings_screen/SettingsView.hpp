#ifndef SETTINGS_VIEW_HPP
#define SETTINGS_VIEW_HPP

#include <gui_generated/settings_screen/SettingsViewBase.hpp>
#include <gui/settings_screen/SettingsPresenter.hpp>

class SettingsView : public SettingsViewBase
{
public:
    SettingsView();
    virtual ~SettingsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void incBrightness();
    void decBrightness();
    void brightnessValueUpdate(uint32_t u32Val);
    void mcuLoadUpdated(uint8_t mcuLoad);
};

#endif // SETTINGS_VIEW_HPP
