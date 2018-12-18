#ifndef COMMON_VIEW_HPP
#define COMMON_VIEW_HPP

#include <mvp/View.hpp>
#include <gui/common/CommonViewInterface.hpp>
#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>

using namespace touchgfx;

class CommonView : public CommonViewInterface
{
public:
    CommonView()
//        : onGotoMenuButtonClicked(this, &CommonView::gotoMenuClickedHandler)
    {
//        gotoMenuButton.setAction(onGotoMenuButtonClicked);

//        mcuLoadTxt.setTypedText(TypedText(T_DEMO_VIEW_MCU_LOAD_TEXT));
//        mcuLoadTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));

        Unicode::snprintf(mcuLoadBuffer, 5, "%d", 0);
        mcuLoad.setWildcard(mcuLoadBuffer);
        mcuLoad.setTypedText(TypedText(T_DEMO_VIEW_MCU_LOAD_VALUE));
        mcuLoad.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    }

protected:
//    Button gotoMenuButton;

//    TextArea mcuLoadTxt;
    TextAreaWithOneWildcard mcuLoad;
    Unicode::UnicodeChar mcuLoadBuffer[5];

    Callback<CommonView, const AbstractButton&> onGotoMenuButtonClicked;

//    void gotoMenuClickedHandler(const AbstractButton& button)
//    {
//        View<T>::presenter->backOptionSelected();
//    }

    virtual void updateProcessorLoad(uint8_t mcuLoadInProcent)
    {
        Unicode::snprintf(mcuLoadBuffer, 5, "%d", mcuLoadInProcent);
        mcuLoad.invalidate();
    }
};

#endif
