#ifndef COMMON_PRESENTER_HPP
#define COMMON_PRESENTER_HPP

#include <gui/common/CommonViewInterface.hpp>
#include <mvp/Presenter.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <gui/model/ModelListener.hpp>

using namespace touchgfx;

class CommonPresenter : public Presenter, public ModelListener
{
public:
    CommonPresenter(CommonViewInterface& view);

//    void backOptionSelected();
    virtual void mcuLoadUpdated(uint8_t mcuLoad);

    void viewStartupDone();
private:

    CommonViewInterface& viewInterface;

};

#endif
