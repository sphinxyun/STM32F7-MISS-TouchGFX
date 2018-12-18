#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include <touchgfx/hal/HAL.hpp>

#include "settings/settings_app.h"
#include "state_machine/state_machine.h"

#include "debug.h"
#include "FreeRTOS.h"
#include "queue.h"

extern "C" {
	extern QueueHandle_t xAudioEffectsQueue;

	extern QueueHandle_t xGuiActions;
	extern QueueHandle_t xGuiStatus;
}

Model::Model() :
	modelListener(0),
	mcuLoadActive(true)
{
}

void Model::tick()
{
	WM_MAIN_GuiStatus guiStatus;

	if (xGuiStatus && xQueueReceive(xGuiStatus, &guiStatus, 0)  == pdTRUE) {
//		DEBUG_SendTextFrame("  tick: xGuiStatus");
		m_brightness = guiStatus.u32BrightnessPercent;

		modelListener->brightnessValueUpdate(guiStatus.u32BrightnessPercent);

		modelListener->actualPressureUpdate(guiStatus.sIrrigationActual.fIrrigationActualPressureMMHG);
		modelListener->actualFlowLPMUpdate(guiStatus.sIrrigationActual.fIrrigationActualFlowLPM);
		modelListener->actualFlowRPMUpdate(guiStatus.sIrrigationActual.fIrrigationActualSpeedRPM);
	}

//	static int milliseconds = 123456;
	uint8_t mcuLoadPct = touchgfx::HAL::getInstance()->getMCULoadPct();
	if (m_mcuLoad != mcuLoadPct)
	{
		m_mcuLoad = mcuLoadPct;
		modelListener->mcuLoadUpdated(m_mcuLoad);
	}
}

void Model::startRegulation() {
	pushActionQueue((uint16_t)WM_MAIN_START_ACTION);
	pushAudioQueue(1);
}

void Model::stopRegulation() {
	pushActionQueue((uint16_t)WM_MAIN_STOP_ACTION);
	pushAudioQueue(1);
}

void Model::incPressureBtn() {
	pushActionQueue((uint16_t)WM_MAIN_INCREASE_IRRIGATION_PRESSURE);
	pushAudioQueue(1);
}

void Model::decPressureBtn() {
	pushActionQueue((uint16_t)WM_MAIN_DECREASE_IRRIGATION_PRESSURE);
	pushAudioQueue(1);
}

void Model::incFlowBtn() {
	pushActionQueue((uint16_t)WM_MAIN_INCREASE_IRRIGATION_FLOW);
	pushAudioQueue(1);
}

void Model::decFlowBtn() {
	pushActionQueue((uint16_t)WM_MAIN_DECREASE_IRRIGATION_FLOW);
	pushAudioQueue(1);
}

void Model::incBrightness() {
	pushActionQueue((uint16_t)WM_MAIN_INCREASE_BRIGHTNESS);
	pushAudioQueue(1);
}

void Model::decBrightness() {
	pushActionQueue((uint16_t)WM_MAIN_DECREASE_BRIGHTNESS);
	pushAudioQueue(1);
}

void Model::pushActionQueue(uint16_t u16ActionCode) {
	if (xGuiActions) {
		xQueueSend( xGuiActions, ( void * ) &u16ActionCode, ( TickType_t ) 0 );
	}
}

void Model::pushAudioQueue(uint16_t u16SoundCode) {
	if (xAudioEffectsQueue) {
		xQueueSend( xAudioEffectsQueue, ( void * ) &u16SoundCode, ( TickType_t ) 0 );
	}
}
