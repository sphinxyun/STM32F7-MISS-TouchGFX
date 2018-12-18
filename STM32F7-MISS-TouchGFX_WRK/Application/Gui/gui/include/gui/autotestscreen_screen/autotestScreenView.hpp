#ifndef AUTOTESTSCREEN_VIEW_HPP
#define AUTOTESTSCREEN_VIEW_HPP

#include <gui_generated/autotestscreen_screen/autotestScreenViewBase.hpp>
#include <gui/autotestscreen_screen/autotestScreenPresenter.hpp>

class autotestScreenView : public autotestScreenViewBase
{
public:
    autotestScreenView();
    virtual ~autotestScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // AUTOTESTSCREEN_VIEW_HPP
