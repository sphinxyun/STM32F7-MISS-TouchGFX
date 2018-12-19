#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void actualPressureMMHGUpdate(float fVal);
    void actualFlowLPMUpdate(float fVal);
    void actualFlowRPMUpdate(float fVal);

    void presetPressureMMHGUpdate(uint8_t fVal);
	void presetFlowLPMUpdate(float fVal);
	void presetFlowRPMUpdate(uint16_t fVal);

    void mcuLoadUpdated(uint8_t mcuLoad);

protected:
    void startRegulation();
    void stopRegulation();

    void incPressureBtn();
    void decPressureBtn();
    void incFlowBtn();
    void decFlowBtn();
};

#endif // MAIN_VIEW_HPP
