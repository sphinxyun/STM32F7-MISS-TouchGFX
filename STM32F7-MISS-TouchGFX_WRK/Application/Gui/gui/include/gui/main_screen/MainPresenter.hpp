#ifndef MAIN_PRESENTER_HPP
#define MAIN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class MainView;

class MainPresenter : public Presenter, public ModelListener
{
public:
    MainPresenter(MainView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~MainPresenter() {};

    void actualPressureMMHGUpdate(float fVal);
	void actualFlowLPMUpdate(float fVal);
	void actualFlowRPMUpdate(float fVal);

    void presetPressureMMHGUpdate(uint8_t u8Val);
	void presetFlowLPMUpdate(float fVal);
	void presetFlowRPMUpdate(int16_t i16Val);

	void startRegulation();
	void stopRegulation();

	void incPressureBtn();
	void decPressureBtn();
	void incFlowBtn();
	void decFlowBtn();

	void showAlarmMessage(bool bVisible);

	void mcuLoadUpdated(uint8_t mcuLoad);

	void updateTemp(float fVal);
	void updateCarmenErrStats(uint32_t u32Valid, uint32_t u32ECRC, uint32_t u32ESOF, uint32_t u32EUNK);
private:
    MainPresenter();

    MainView& view;
};


#endif // MAIN_PRESENTER_HPP
