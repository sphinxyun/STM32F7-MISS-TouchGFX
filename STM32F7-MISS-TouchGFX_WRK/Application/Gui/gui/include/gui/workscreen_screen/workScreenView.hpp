#ifndef WORKSCREEN_VIEW_HPP
#define WORKSCREEN_VIEW_HPP

#include <gui_generated/workscreen_screen/workScreenViewBase.hpp>
#include <gui/workscreen_screen/workScreenPresenter.hpp>

class workScreenView : public workScreenViewBase
{
public:
    workScreenView();
    virtual ~workScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void actualPressureUpdate(float fVal);
    void actualFlowLPMUpdate(float fVal);
    void actualFlowRPMUpdate(float fVal);

protected:
    void startRegulation();
    void stopRegulation();

    void incPressureBtn();
    void decPressureBtn();
    void incFlowBtn();
    void decFlowBtn();
};

#endif // WORKSCREEN_VIEW_HPP
