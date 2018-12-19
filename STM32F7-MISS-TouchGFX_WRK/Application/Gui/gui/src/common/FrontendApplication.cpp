#include <gui/common/FrontendApplication.hpp>

#include "FreeRTOS.h"
#include "queue.h"

extern "C" {
	extern QueueHandle_t xAudioEffectsQueue;
}

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
}

void FrontendApplication::handleClickEvent(const ClickEvent& evt)
{
    MVPApplication::handleClickEvent(evt);

    pushAudioQueue(1);

    // A click event has been registered so reset last click time
//    resetScreenSaver();
}

void FrontendApplication::pushAudioQueue(uint16_t u16SoundCode) {
	if (xAudioEffectsQueue) {
		xQueueSend( xAudioEffectsQueue, ( void * ) &u16SoundCode, ( TickType_t ) 0 );
	}
}
