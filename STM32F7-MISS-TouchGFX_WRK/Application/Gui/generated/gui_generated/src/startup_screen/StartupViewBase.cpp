/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/startup_screen/StartupViewBase.hpp>
#include <touchgfx/Color.hpp>

StartupViewBase::StartupViewBase()
{
    box1.setPosition(0, 0, 640, 480);
    box1.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));

    add(box1);
}

void StartupViewBase::setupScreen()
{

}

//Called when the screen is done with transition/load
void StartupViewBase::afterTransition()
{
    //boot
    //When screen is entered change screen to Boot
    //Go to Boot with no screen transition
    application().gotoBootScreenNoTransition();
}
