#ifndef __STATE_MACHINE_THREAD_H
#define __STATE_MACHINE_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "global.h"

#include "regulation/regulation.h"

typedef enum {
	eStartup = 0,
//	eAutoTest,
//	eLevel,
	eIdle,
	eSettings,
	eMenu,

	eUnknown

} WM_MAIN_DeviceMode;

typedef struct {
	WM_MAIN_DeviceMode eDevMode;

	REGULATION_IrrPresets_t sIrrigationPresets;
	REGULATION_IrrActual_t sIrrigationActual;

	uint32_t u32BrightnessPercent;

	uint32_t u32ButtonFlags;
	uint32_t u32AlarmFlags;
} WM_MAIN_GuiStatus;

typedef struct {
	bool inUse;
	WM_MAIN_GuiStatus status;
} WM_MAIN_GuiPool;

typedef enum {
	WM_MAIN_STOP_ACTION = 0,
	WM_MAIN_START_ACTION,

	WM_MAIN_INCREASE_IRRIGATION_PRESSURE,
	WM_MAIN_DECREASE_IRRIGATION_PRESSURE,
	WM_MAIN_INCREASE_IRRIGATION_FLOW,
	WM_MAIN_DECREASE_IRRIGATION_FLOW,

	WM_MAIN_INCREASE_BRIGHTNESS,
	WM_MAIN_DECREASE_BRIGHTNESS,

	WM_GUI_LOADED,

	WM_MAIN_DUMMY,
} WM_MAIN_ActionsTypdef;

typedef enum {
	WM_MAIN_ERROR_NONE = 0
} WM_MAIN_ErrorTypdef;

typedef enum {
	WM_MAIN_STOP = 0,
	WM_MAIN_START,
	WM_MAIN_ERROR,
} WM_MAIN_StateTypdef;

WM_MAIN_ErrorTypdef WM_MAIN_Init(void);
WM_MAIN_ErrorTypdef WM_MAIN_DeInit(void);

WM_MAIN_ErrorTypdef WM_MAIN_Start(void);
WM_MAIN_ErrorTypdef WM_MAIN_Stop(void);

void free_struct(WM_MAIN_GuiPool *toRelease);

#ifdef __cplusplus
}
#endif

#endif /*__STATE_MACHINE_THREAD_H */
