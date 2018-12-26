#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

/**
 * ModelListener is the interface through which the Model can inform the currently
 * active presenter of events. All presenters should derive from this class.
 * It also provides a model pointer for the presenter to interact with the Model.
 *
 * The bind function is called automatically.
 *
 * Add the virtual functions Model should be able to call on the active presenter to this class.
 */
class ModelListener
{
public:
    ModelListener() : model(0) {}

    virtual ~ModelListener() {}

    /**
     * Sets the model pointer to point to the Model object. Called automatically
     * when switching screen.
     */
    void bind(Model* m)
    {
        model = m;
    }

    virtual void actualPressureMMHGUpdate(float fVal) {}
    virtual void actualFlowLPMUpdate(float fVal) {}
    virtual void actualFlowRPMUpdate(float fVal) {}

    virtual void presetPressureMMHGUpdate(uint8_t fVal) {}
	virtual void presetFlowLPMUpdate(float fVal) {}
	virtual void presetFlowRPMUpdate(uint16_t fVal) {}

    virtual void brightnessValueUpdate(uint32_t u32Val) {}

    virtual void mcuLoadUpdated(uint8_t mcuLoad) {}

    virtual void updateTemp(float fVal) {}
    virtual void updateCarmenErrStats(uint32_t u32Valid, uint32_t u32ECRC, uint32_t u32ESOF, uint32_t u32EUNK) {}

    virtual void showAlarmMessage(bool bVisible) {}

protected:
    Model* model;
};

#endif /* MODELLISTENER_HPP */
