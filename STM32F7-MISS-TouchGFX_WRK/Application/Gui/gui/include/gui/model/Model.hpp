#ifndef MODEL_HPP
#define MODEL_HPP

#include <touchgfx/Utils.hpp>

#include "state_machine/state_machine.h"

class ModelListener;
class FrontendHeap;

/**
 * The Model class defines the data model in the model-view-presenter paradigm.
 * The Model is a singular object used across all presenters. The currently active
 * presenter will have a pointer to the Model through deriving from ModelListener.
 *
 * The Model will typically contain UI state information that must be kept alive
 * through screen transitions. It also usually provides the interface to the rest
 * of the system (the backend). As such, the Model can receive events and data from
 * the backend and inform the current presenter of such events through the modelListener
 * pointer, which is automatically configured to point to the current presenter.
 * Conversely, the current presenter can trigger events in the backend through the Model.
 */
class Model
{
public:
    Model(FrontendHeap *app);

    /**
     * Sets the modelListener to point to the currently active presenter. Called automatically
     * when switching screen.
     */
    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    /**
     * This function will be called automatically every frame. Can be used to e.g. sample hardware
     * peripherals or read events from the surrounding system and inject events to the GUI through
     * the ModelListener interface.
     */
    void tick();

    uint8_t getBrightnessSetting(void) { return m_brightness; }
    uint8_t getMcuLoad(void) { return m_mcuLoad; }
    WM_MAIN_GuiStatus getGuiStatus(void) { return m_guiStatus; }

    void guiLoaded();

    void startRegulation();
    void stopRegulation();

    void incPressureBtn() ;
    void decPressureBtn();
    void incFlowBtn();
    void decFlowBtn();

    void incBrightness();
    void decBrightness();

    void switchToDiagnosticMode(void);
    void switchToLevelMode(void);
    void switchToMainMode(void);

protected:
    /**
     * Pointer to the currently active presenter.
     */
    ModelListener* modelListener;
    bool mcuLoadActive;

private:
    void pushActionQueue(uint16_t u16ActionCode);
    void pushAudioQueue(uint16_t u16SoundCode);

private:
    FrontendHeap *m_app;

    uint8_t m_brightness;
    uint8_t m_mcuLoad;

    WM_MAIN_GuiStatus m_guiStatus;


};

#endif /* MODEL_HPP */
