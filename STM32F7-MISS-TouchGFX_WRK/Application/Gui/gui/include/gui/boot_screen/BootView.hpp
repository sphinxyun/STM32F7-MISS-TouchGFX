#ifndef BOOT_VIEW_HPP
#define BOOT_VIEW_HPP

#include <gui_generated/boot_screen/BootViewBase.hpp>
#include <gui/boot_screen/BootPresenter.hpp>

class BootView : public BootViewBase
{
public:
    BootView();
    virtual ~BootView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void mcuLoadUpdated(uint8_t mcuLoad);
};

#endif // BOOT_VIEW_HPP
