#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include <gui/common/FrontendHeap.hpp>

#include <touchgfx/hal/HAL.hpp>

#include "settings/settings_app.h"


#include "debug.h"
#include "FreeRTOS.h"
#include "queue.h"

extern "C" {
	extern QueueHandle_t xAudioEffectsQueue;

	extern QueueHandle_t xGuiActions;
	extern QueueHandle_t xGuiStatus;
}

Model::Model(FrontendHeap *app) :
	modelListener(0),
	mcuLoadActive(true),
	m_app(app)
{
}

void Model::tick()
{
	if (xGuiStatus && xQueueReceive(xGuiStatus, &m_guiStatus, 0)  == pdTRUE) {
//		DEBUG_SendTextFrame("  tick: xGuiStatus");
		m_brightness = m_guiStatus.u32BrightnessPercent;

		if (m_brightness == 50) {
//			m_app->app.gotoworkScreenScreenSlideTransitionWest();
		}

		modelListener->brightnessValueUpdate(m_guiStatus.u32BrightnessPercent);

		modelListener->actualPressureMMHGUpdate(m_guiStatus.sIrrigationActual.fIrrigationActualPressureMMHG);
		modelListener->actualFlowLPMUpdate(m_guiStatus.sIrrigationActual.fIrrigationActualFlowLPM);
		modelListener->actualFlowRPMUpdate(m_guiStatus.sIrrigationActual.fIrrigationActualSpeedRPM);

		modelListener->presetPressureMMHGUpdate(m_guiStatus.u8IrrigationPresetPressureMMHG);
		modelListener->presetFlowLPMUpdate(m_guiStatus.fIrrigationPresetFlowLPM);
		modelListener->presetFlowRPMUpdate(m_guiStatus.u16IrrigationPresetFlowRPM);
	}

	uint8_t mcuLoadPct = touchgfx::HAL::getInstance()->getMCULoadPct();
	if (m_mcuLoad != mcuLoadPct)
	{
		m_mcuLoad = mcuLoadPct;
		modelListener->mcuLoadUpdated(m_mcuLoad);
	}
}

void Model::switchToDiagnosticMode(void) {
	m_app->app.gotoDiagnosticsScreenSlideTransitionNorth();
}

void Model::switchToLevelMode(void) {
	m_app->app.gotoLevelScreenCoverTransitionNorth();
}

void Model::switchToMainMode(void) {
	m_app->app.gotoMainScreenSlideTransitionNorth();
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
