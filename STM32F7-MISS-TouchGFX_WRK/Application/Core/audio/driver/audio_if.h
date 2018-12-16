#ifndef __AUDIO_IF_H
#define __AUDIO_IF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"

#define AUDIO_OUT_BUFFER_SIZE                      (18 * 1024)

typedef enum {
	BUFFER_OFFSET_NONE = 0,
	PLAY_BUFFER_OFFSET_HALF,
	PLAY_BUFFER_OFFSET_FULL,
} BUFFER_StateTypeDef;

typedef enum {
	AUDIOPLAYER_STOP = 0,
	AUDIOPLAYER_START,
	AUDIOPLAYER_PLAY,
	AUDIOPLAYER_PAUSE,
	AUDIOPLAYER_EOF,
	AUDIOPLAYER_ERROR,
} OUT_StateTypdef;

typedef struct {
	uint32_t volume;
	uint32_t mute;
	OUT_StateTypdef state;
} OUT_Process_t;

typedef enum {
	BUFFER_EMPTY = 0,
	BUFFER_FULL,
} WR_BUFFER_StateTypeDef;

typedef struct {
	uint8_t buff[AUDIO_OUT_BUFFER_SIZE];
	OUT_Process_t out;
} AUDIO_ProcessTypdef;

typedef struct {
	void (*TransferComplete_CallBack)(void);
	void (*HalfTransfer_CallBack)(void);
	void (*Error_CallBack)(void);
} AUDIO_IFTypeDef;

typedef void (*pFunc)(void);

extern AUDIO_ProcessTypdef haudio;

void AUDIO_IF_RegisterCallbacks(pFunc tc_cb, pFunc ht_cb, pFunc err_cb);

#ifdef __cplusplus
}
#endif

#endif /*__AUDIO_IF_H */
