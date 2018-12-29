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

void Model::tick() {
	WM_MAIN_GuiPool *guiStatus;

	if (xGuiStatus && xQueueReceive(xGuiStatus, &guiStatus, 0)  == pdTRUE) {
		if (m_guiStatus.eDevMode != guiStatus->status.eDevMode) {
			switch (guiStatus->status.eDevMode) {
//			case eAutoTest:
//				switchToDiagnosticMode();
//				break;
//			case eLevel:
//				switchToLevelMode();
//				break;
			case eIdle:
				switchToMainMode();
				break;
			default:
				break;
			}
		}

//		DEBUG_SendTextFrame("  tick: xGuiStatus");
		m_brightness = guiStatus->status.u32BrightnessPercent;

		if (m_brightness == 50) {
//			m_app->app.gotoworkScreenScreenSlideTransitionWest();
		}

		modelListener->brightnessValueUpdate(guiStatus->status.u32BrightnessPercent);

		modelListener->actualPressureMMHGUpdate(guiStatus->status.sIrrigationActual.sPressureData.fPressureMMHG);
		modelListener->actualFlowLPMUpdate(guiStatus->status.sIrrigationActual.fFlowLPM);
		modelListener->actualFlowRPMUpdate(guiStatus->status.sIrrigationActual.fFlowRPM);

		modelListener->presetPressureMMHGUpdate(guiStatus->status.sIrrigationPresets.u8PressureMMHG);
		modelListener->presetFlowLPMUpdate(guiStatus->status.sIrrigationPresets.fFlowLPM);
		modelListener->presetFlowRPMUpdate(guiStatus->status.sIrrigationPresets.i16FlowRPM);

		modelListener->updateTemp(guiStatus->status.sIrrigationActual.sPressureData.fTemperatureC);
		modelListener->updateCarmenErrStats(guiStatus->status.sIrrigationActual.sPressureData.sErrorStats.u32Valid,
				guiStatus->status.sIrrigationActual.sPressureData.sErrorStats.u32ErrCRC, 3, 4);

		if (m_guiStatus.u32AlarmFlags != guiStatus->status.u32AlarmFlags) {
			if (guiStatus->status.u32AlarmFlags) {
				modelListener->showAlarmMessage(true);
				pushAudioQueue(3);
			} else {
				modelListener->showAlarmMessage(false);
			}
		}

		m_guiStatus = guiStatus->status;
		free_struct(guiStatus);
	}

	uint8_t mcuLoadPct = touchgfx::HAL::getInstance()->getMCULoadPct();
	if (m_mcuLoad != mcuLoadPct)
	{
		m_mcuLoad = mcuLoadPct;
		modelListener->mcuLoadUpdated(m_mcuLoad);
	}
}

void Model::switchToDiagnosticMode(void) {
//	m_app->app.gotoDiagnosticsScreenNoTransition();
}

void Model::switchToLevelMode(void) {
//	m_app->app.gotoLevelScreenNoTransition();
}

void Model::switchToMainMode(void) {
	m_app->app.gotoMainScreenNoTransition();
}

void Model::guiLoaded() {
	pushActionQueue((uint16_t)WM_GUI_LOADED);
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
