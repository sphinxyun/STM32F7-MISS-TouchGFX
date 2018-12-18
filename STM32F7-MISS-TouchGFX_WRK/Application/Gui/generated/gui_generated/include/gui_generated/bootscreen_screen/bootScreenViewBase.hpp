/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef BOOTSCREEN_VIEW_BASE_HPP
#define BOOTSCREEN_VIEW_BASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/bootscreen_screen/bootScreenPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/containers/buttons/Buttons.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

class bootScreenViewBase : public touchgfx::View<bootScreenPresenter>
{
public:
    bootScreenViewBase();
    virtual ~bootScreenViewBase() {}

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
    touchgfx::Callback<bootScreenViewBase, const touchgfx::AbstractButtonContainer&> flexButtonCallback;

};

#endif // BOOTSCREEN_VIEW_BASE_HPP
