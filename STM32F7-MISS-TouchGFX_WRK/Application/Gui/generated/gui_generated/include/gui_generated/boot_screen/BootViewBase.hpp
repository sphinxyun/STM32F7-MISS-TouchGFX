/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef BOOT_VIEW_BASE_HPP
#define BOOT_VIEW_BASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/boot_screen/BootPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/containers/buttons/Buttons.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

class BootViewBase : public touchgfx::View<BootPresenter>
{
public:
    BootViewBase();
    virtual ~BootViewBase() {}

    virtual void setupScreen();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box box1;
    touchgfx::Image image;
    touchgfx::TextArea textArea1;
    touchgfx::TextArea textArea2;
    touchgfx::IconButtonStyle< touchgfx::TouchButtonTrigger > settingsBtn;
    touchgfx::IconButtonStyle< touchgfx::TouchButtonTrigger > workBtn;
    touchgfx::TextAreaWithOneWildcard mcuLoad;
    touchgfx::TextArea textArea3;
    touchgfx::IconButtonStyle< touchgfx::TouchButtonTrigger > diagBtn;
    touchgfx::IconButtonStyle< touchgfx::TouchButtonTrigger > levelBtn;

    /*
     * Wildcard Buffers
     */
    static const uint16_t MCULOAD_SIZE = 10;
    touchgfx::Unicode::UnicodeChar mcuLoadBuffer[MCULOAD_SIZE];

private:

    /*
     * Callback Handler Declarations
     */
    void flexButtonCallbackHandler(const touchgfx::AbstractButtonContainer& src);

    /*
     * Callback Declarations
     */
    touchgfx::Callback<BootViewBase, const touchgfx::AbstractButtonContainer&> flexButtonCallback;

};

#endif // BOOT_VIEW_BASE_HPP
