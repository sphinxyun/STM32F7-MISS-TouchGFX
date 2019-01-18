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
    void actualFlowDiagnosticsUpdate(uint16_t u16Diag, uint16_t u16Current);

    void presetPressureMMHGUpdate(uint8_t fVal);
	void presetFlowLPMUpdate(float fVal);
	void presetFlowRPMUpdate(int16_t i16Val);

    void mcuLoadUpdated(uint8_t mcuLoad);
    void showAlarmMessage(bool bVisible);

    void updateTemp(float fVal);
    void updateCarmenErrStats(uint32_t u32Valid, uint32_t u32ECRC, uint32_t u32ESOF, uint32_t u32EUNK);

protected:
    void startRegulation();
    void stopRegulation();

    void incPressureBtn();
    void decPressureBtn();
    void incFlowBtn();
    void decFlowBtn();
};

#endif // MAIN_VIEW_HPP
