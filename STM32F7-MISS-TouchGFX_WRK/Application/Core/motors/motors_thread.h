#ifndef __MOTORS_THREAD_H
#define __MOTORS_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "global.h"

typedef enum {
	MOTORS_ERROR_NONE = 0
} MOTORS_ErrorTypdef;

//typedef enum {
//	WM_MAIN_STOP = 0,
//	WM_MAIN_START,
//	WM_MAIN_ERROR,
//} WM_MAIN_StateTypdef;

MOTORS_ErrorTypdef MOTORS_Init(void);
MOTORS_ErrorTypdef MOTORS_DeInit(void);

MOTORS_ErrorTypdef MOTORS_Start(void);
MOTORS_ErrorTypdef MOTORS_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /*__MOTORS_THREAD_H */
