#include <gui/common/CommonPresenter.hpp>

CommonPresenter::CommonPresenter(CommonViewInterface& view) :
    viewInterface(view)
{
}
//
//void DemoPresenter::backOptionSelected()
//{
//    static_cast<FrontendApplication*>(Application::getInstance())->gotoMainMenuScreen();
//}

void CommonPresenter::mcuLoadUpdated(uint8_t mcuLoad)
{
    viewInterface.updateProcessorLoad(mcuLoad);
}

void CommonPresenter::viewStartupDone()
{

}

