/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/bootscreen_screen/bootScreenViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>

bootScreenViewBase::bootScreenViewBase() :
    flexButtonCallback(this, &bootScreenViewBase::flexButtonCallbackHandler)
{
    box1.setPosition(0, 0, 640, 480);
    box1.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

    image.setXY(0, 139);
    image.setBitmap(Bitmap(BITMAP_MISS_LOGO_640PX_ID));

    textArea1.setXY(462, 434);
    textArea1.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textArea1.setLinespacing(0);
    textArea1.setTypedText(TypedText(T_SINGLEUSEID1));

    textArea2.setXY(500, 454);
    textArea2.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textArea2.setLinespacing(0);
    textArea2.setTypedText(TypedText(T_SINGLEUSEID2));

    settingsBtn.setIconBitmaps(Bitmap(BITMAP_BLUE_ICONS_SETTINGS_48_ID), Bitmap(BITMAP_BLUE_ICONS_SETTINGS_48_ID));
    settingsBtn.setIconXY(0, 0);
    settingsBtn.setPosition(404, 429, 50, 50);
    settingsBtn.setAction(flexButtonCallback);

    workBtn.setIconBitmaps(Bitmap(BITMAP_BLUE_ICONS_TOOLS_48_ID), Bitmap(BITMAP_BLUE_ICONS_TOOLS_48_ID));
    workBtn.setIconXY(0, 0);
    workBtn.setPosition(335, 429, 50, 50);
    workBtn.setAction(flexButtonCallback);

    mcuLoad.setPosition(586, 0, 54, 20);
    mcuLoad.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    mcuLoad.setLinespacing(0);
    mcuLoadBuffer[0] = 0;
    mcuLoad.setWildcard(mcuLoadBuffer);
    mcuLoad.setTypedText(TypedText(T_SINGLEUSEID30));

    textArea3.setXY(486, 0);
    textArea3.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textArea3.setLinespacing(0);
    textArea3.setTypedText(TypedText(T_SINGLEUSEID36));

    add(box1);
    add(image);
    add(textArea1);
    add(textArea2);
    add(settingsBtn);
    add(workBtn);
    add(mcuLoad);
    add(textArea3);
}

void bootScreenViewBase::setupScreen()
{

}

void bootScreenViewBase::flexButtonCallbackHandler(const touchgfx::AbstractButtonContainer& src)
{
    if (&src == &settingsBtn)
    {
        //settings
        //When settingsBtn clicked change screen to settingsScreen
        //Go to settingsScreen with screen transition towards East
        application().gotosettingsScreenScreenSlideTransitionEast();
    }
    else if (&src == &workBtn)
    {
        //work
        //When workBtn clicked change screen to workScreen
        //Go to workScreen with screen transition towards West
        application().gotoworkScreenScreenSlideTransitionWest();
    }
}
