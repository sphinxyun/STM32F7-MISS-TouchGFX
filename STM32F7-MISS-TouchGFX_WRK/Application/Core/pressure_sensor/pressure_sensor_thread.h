#ifndef __PRESSURE_SENSOR_THREAD_H
#define __PRESSURE_SENSOR_THREAD_H

#ifdef __cplusplus
 extern "C" {
#endif

 /* Kernel includes. */
 #include "FreeRTOS.h"
 #include "task.h"
 #include "queue.h"

 #include "global.h"
   
typedef enum {
  PRESSURE_SENSOR_ERROR_NONE = 0,
} PRESSURE_SENSOR_ErrorTypdef;

PRESSURE_SENSOR_ErrorTypdef  PRESSURE_SENSOR_Init(void);

PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Start(void);
PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /*__PRESSURE_SENSOR_THREAD_H */
