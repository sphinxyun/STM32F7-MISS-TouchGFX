#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

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

Model::Model() : modelListener(0)
{
}

void Model::tick()
{
	WM_MAIN_GuiStatus guiStatus;

	if (xGuiStatus && xQueueReceive(xGuiStatus, &guiStatus, 0)  == pdTRUE) {
//		DEBUG_SendTextFrame("  tick: xGuiStatus");
		modelListener->brightnessValueUpdate(guiStatus.u32BrightnessPercent);

		modelListener->actualPressureUpdate(guiStatus.sIrrigationActual.fIrrigationActualPressureMMHG);
		modelListener->actualFlowLPMUpdate(guiStatus.sIrrigationActual.fIrrigationActualFlowLPM);
		modelListener->actualFlowRPMUpdate(guiStatus.sIrrigationActual.fIrrigationActualSpeedRPM);
	}
}

void Model::startRegulation() {
	WM_MAIN_ActionsTypdef temp = WM_MAIN_START_ACTION;
	if (xGuiActions) {
		xQueueSend(xGuiActions, (void*)&temp, 0);
	}

	uint16_t u16Temp = 1;
	if (xAudioEffectsQueue) {
		xQueueSend( xAudioEffectsQueue, ( void * ) &u16Temp, ( TickType_t ) 0 );
	}
}

void Model::stopRegulation() {
	WM_MAIN_ActionsTypdef temp = WM_MAIN_STOP_ACTION;
	if (xGuiActions) {
		xQueueSend(xGuiActions, (void*)&temp, 0);
	}

	uint16_t u16Temp = 1;
	if (xAudioEffectsQueue) {
		xQueueSend( xAudioEffectsQueue, ( void * ) &u16Temp, ( TickType_t ) 0 );
	}
}

void Model::incBrightness() {
	WM_MAIN_ActionsTypdef temp = WM_MAIN_INCREASE_BRIGHTNESS;
	if (xGuiActions) {
		xQueueSend(xGuiActions, (void*)&temp, 0);
	}

	uint16_t u16Temp = 1;
	if (xAudioEffectsQueue) {
		xQueueSend( xAudioEffectsQueue, ( void * ) &u16Temp, ( TickType_t ) 0 );
	}
}

void Model::decBrightness() {
	WM_MAIN_ActionsTypdef temp = WM_MAIN_DECREASE_BRIGHTNESS;
	if (xGuiActions) {
		xQueueSend(xGuiActions, (void*)&temp, 0);
	}

	uint16_t u16Temp = 1;
	if (xAudioEffectsQueue) {
		xQueueSend( xAudioEffectsQueue, ( void * ) &u16Temp, ( TickType_t ) 0 );
	}
}
