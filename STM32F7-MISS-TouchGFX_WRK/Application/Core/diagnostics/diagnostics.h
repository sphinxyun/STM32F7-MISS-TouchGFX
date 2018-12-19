#ifndef __DIAGNOSTICS_THREAD_H
#define __DIAGNOSTICS_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	DIAGNOSTICS_ERROR_NONE = 0
} DIAGNOSTICS_ErrorTypdef;

DIAGNOSTICS_ErrorTypdef DIAGNOSTICS_Init(void);
DIAGNOSTICS_ErrorTypdef DIAGNOSTICS_DeInit(void);

DIAGNOSTICS_ErrorTypdef DIAGNOSTICS_Start(void);
DIAGNOSTICS_ErrorTypdef DIAGNOSTICS_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /*__DIAGNOSTICS_THREAD_H */
