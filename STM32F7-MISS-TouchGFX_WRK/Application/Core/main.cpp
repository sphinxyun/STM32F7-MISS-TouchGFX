//volatile bool overrideBlitcaps = false;

#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>

using namespace touchgfx;

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "debug.h"

#include "audio/audio_app.h"
#include "settings/settings_app.h"
#include "regulation/regulation.h"
#include "state_machine/state_machine.h"

extern "C" {
	uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute__((section(".RamData2")));
}

/**
 * Define the FreeRTOS task priorities and stack sizes
 */
#define configGUI_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 3 )

#define configGUI_TASK_STK_SIZE                 ( 1500 )

#define CANVAS_BUFFER_SIZE (5500)

static void GUITask(void* params) {
    touchgfx::HAL::getInstance()->taskEntry();
}

int main(void) {
    HARDWARE_Init();

    DEBUG_Init();
    DEBUG_SendTextHeader();

    SETTINGS_Init();

    TOUCHGFX_Init();

    AUDIOPLAYER_Init(100);

    REGULATION_Init();
    WM_MAIN_Init();

    static uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];
    CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    xTaskCreate(GUITask, "GUITask",
                configGUI_TASK_STK_SIZE,
                NULL,
                configGUI_TASK_PRIORITY,
                NULL);

    vTaskStartScheduler();

    for (;;);
}
