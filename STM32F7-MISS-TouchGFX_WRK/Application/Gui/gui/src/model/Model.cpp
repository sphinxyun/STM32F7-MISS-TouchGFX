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
		DEBUG_SendTextFrame("  tick: xGuiStatus");
		modelListener->brightnessValueUpdate(guiStatus.u32BrightnessPercent);
	}
}

void Model::incBrightness() {
	WM_MAIN_ActionsTypdef temp = WM_MAIN_INCREASE_BRIGHTNESS;
	if (xGuiActions) {
		xQueueSend(xGuiActions, (void*)&temp, 0);
	}

//	if (xAudioEffectsQueue) {
//		uint16_t u16Temp = 0;
//		BaseType_t temp = xQueueSend( xAudioEffectsQueue, ( void * ) &u16Temp, ( TickType_t ) 0 );
//		DEBUG_SendTextFrame("  xAudioEffectsQueue: %x, %x", xAudioEffectsQueue, temp);
//	} else {
//		DEBUG_SendTextFrame("  xAudioEffectsQueue: NULL");
//	}

//	SETTINGS_IncBrightness();
}

void Model::decBrightness() {
	WM_MAIN_ActionsTypdef temp = WM_MAIN_DECREASE_BRIGHTNESS;
	if (xGuiActions) {
		xQueueSend(xGuiActions, (void*)&temp, 0);
	}

//	if (xAudioEffectsQueue) {
//		uint16_t u16Temp = 1;
//		BaseType_t temp = xQueueSend( xAudioEffectsQueue, ( void * ) &u16Temp, ( TickType_t ) 0 );
//		DEBUG_SendTextFrame("  xAudioEffectsQueue: %x, %x", xAudioEffectsQueue, temp);
//	} else {
//		DEBUG_SendTextFrame("  xAudioEffectsQueue: NULL");
//	}

//	SETTINGS_DecBrightness();
}
