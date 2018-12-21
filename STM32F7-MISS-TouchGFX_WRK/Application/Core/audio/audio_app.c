#include <string.h>

#include "audio_app.h"

#include "audio_res.c"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "debug.h"

#include "driver/wm8994/wm8994.h"
#include "driver/audio_if.h"
#include "stm32f7xx_hal_i2s.h"

static void AUDIO_TransferComplete_CallBack(void);
static void AUDIO_HalfTransfer_CallBack(void);
static void AUDIO_Error_CallBack(void);

static void Audio_LL_Thread(void * argument);
static QueueHandle_t xSaiLowLevelEvents = 0;
static TaskHandle_t LLAudioThreadId = 0;

static void Audio_HL_Thread(void * argument);
QueueHandle_t xAudioEffectsQueue = 0;
static TaskHandle_t HLAudioThreadId = 0;

static volatile uint32_t CurrentSoundEffect;

typedef struct {
	const uint8_t *pu8RawData;

	uint32_t u32SamplesTotal;
	uint32_t u32CurrentSample;
} AUDIOPLAYER_Sound_t;

AUDIOPLAYER_Sound_t m_soundEffects[3] = {
	{clickSoundRawData, clickSoundRawData_LEN, 0},
	{errorSoundRawData, errorSoundRawData_LEN, 0},
	{alarmSoundRawData, alarmSoundRawData_LEN, 0}
};

AUDIOPLAYER_ErrorTypdef AUDIOPLAYER_Init(uint8_t volume) {
	DEBUG_SendTextFrame("AUDIOPLAYER_Init, volume: %d", volume);

	/* Try to Init Audio interface in different configurations in case of failure */
	BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, volume, I2S_AUDIOFREQ_16K);
	BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);

	/* Initialize internal audio structure */
	haudio.out.state = AUDIOPLAYER_STOP;
	haudio.out.mute = MUTE_OFF;
	haudio.out.volume = volume;

	/* Register audio BSP drivers callbacks */
	AUDIO_IF_RegisterCallbacks(AUDIO_TransferComplete_CallBack, AUDIO_HalfTransfer_CallBack, AUDIO_Error_CallBack);

	xSaiLowLevelEvents = xQueueCreate( 1, sizeof( uint16_t ) );

    xTaskCreate(Audio_LL_Thread, "AudioLLTask",
                512,
                NULL,
				tskIDLE_PRIORITY + 5,
                &LLAudioThreadId);

	xAudioEffectsQueue = xQueueCreate( 4, sizeof( uint16_t ) );

    xTaskCreate(Audio_HL_Thread, "AudioHLTask",
                256,
                NULL,
				tskIDLE_PRIORITY + 2,
                &HLAudioThreadId);

	DEBUG_SendTextFrame("  HLAudioThreadId: %x", HLAudioThreadId);

	AUDIOPLAYER_Stop();

	return AUDIOPLAYER_ERROR_NONE;
}

AUDIOPLAYER_StateTypdef AUDIOPLAYER_GetState(void) {
	return haudio.out.state;
}

uint32_t AUDIOPLAYER_GetVolume(void) {
	return haudio.out.volume;
}

AUDIOPLAYER_ErrorTypdef AUDIOPLAYER_SetVolume(uint32_t volume) {
	if (BSP_AUDIO_OUT_SetVolume(volume) == AUDIO_OK) {
		haudio.out.volume = volume;
		return AUDIOPLAYER_ERROR_NONE;
	} else {
		return AUDIOPLAYER_ERROR_HW;
	}
}

void read_raw_data(uint8_t *pu8AudioBuffer, uint8_t u8SoundEffect, uint32_t u32SamplesToBeRead, uint32_t *pu32SamplesRead) {
	//DEBUG_SendTextFrame(" read_raw_data (%d): %d to be read, %d total, %d current", u8SoundEffect,
//			u32SamplesToBeRead, m_soundEffects[u8SoundEffect].u32SamplesTotal, m_soundEffects[u8SoundEffect].u32CurrentSample);

	uint32_t u32SamplesLeft = m_soundEffects[u8SoundEffect].u32SamplesTotal - m_soundEffects[u8SoundEffect].u32CurrentSample;

	if (u32SamplesLeft > u32SamplesToBeRead) {
		memcpy(pu8AudioBuffer, &m_soundEffects[u8SoundEffect].pu8RawData[m_soundEffects[u8SoundEffect].u32CurrentSample], u32SamplesToBeRead);
		m_soundEffects[u8SoundEffect].u32CurrentSample += u32SamplesToBeRead;
		*pu32SamplesRead = u32SamplesToBeRead;
	} else {
		memcpy(pu8AudioBuffer, &m_soundEffects[u8SoundEffect].pu8RawData[m_soundEffects[u8SoundEffect].u32CurrentSample], u32SamplesLeft);
		m_soundEffects[u8SoundEffect].u32CurrentSample += u32SamplesLeft;
		*pu32SamplesRead = u32SamplesLeft;
	}

	//DEBUG_SendTextFrame("   read %d samples", *pu32SamplesRead);
	//DEBUG_SendTextFrame("     %d total, %d current", m_soundEffects[u8SoundEffect].u32SamplesTotal, m_soundEffects[u8SoundEffect].u32CurrentSample);
}

AUDIOPLAYER_ErrorTypdef AUDIOPLAYER_Play(uint8_t u8SoundEffect) {
	haudio.out.state = AUDIOPLAYER_PLAY;
	uint32_t numOfReadBytes;

	//DEBUG_SendTextFrame("AUDIOPLAYER_Play");

	m_soundEffects[u8SoundEffect].u32CurrentSample = 0;
	read_raw_data(&haudio.buff[0], u8SoundEffect, AUDIO_OUT_BUFFER_SIZE, &numOfReadBytes);

	if (numOfReadBytes != 0) {
		CurrentSoundEffect = u8SoundEffect;

		if (LLAudioThreadId != 0) {
			vTaskResume(LLAudioThreadId);
		}

		BSP_AUDIO_OUT_Play((uint16_t*) &haudio.buff[0], numOfReadBytes);
		return AUDIOPLAYER_ERROR_NONE;
	}

	return AUDIOPLAYER_ERROR_IO;
}

AUDIOPLAYER_ErrorTypdef AUDIOPLAYER_Process(void) {
	switch (haudio.out.state) {
	case AUDIOPLAYER_START:
		haudio.out.state = AUDIOPLAYER_PLAY;
		break;

	case AUDIOPLAYER_EOF:
		haudio.out.state = AUDIOPLAYER_EOF;
		AUDIOPLAYER_NotifyEndOfFile();
		break;

	case AUDIOPLAYER_ERROR:
		AUDIOPLAYER_Stop();
		break;

	case AUDIOPLAYER_STOP:
	case AUDIOPLAYER_PLAY:
	default:
		break;
	}

	return AUDIOPLAYER_ERROR_NONE;
}

AUDIOPLAYER_ErrorTypdef AUDIOPLAYER_NotifyEndOfFile(void) {
	AUDIOPLAYER_Stop();

	return AUDIOPLAYER_ERROR_NONE;
}

AUDIOPLAYER_ErrorTypdef AUDIOPLAYER_DeInit(void) {
	haudio.out.state = AUDIOPLAYER_STOP;

	BSP_AUDIO_OUT_Stop(CODEC_PDWN_HW);
	BSP_AUDIO_OUT_DeInit();

	if (xAudioEffectsQueue != 0) {
		vQueueDelete(xAudioEffectsQueue);
		xAudioEffectsQueue = 0;
	}
	if (HLAudioThreadId != 0) {
		vTaskDelete(HLAudioThreadId);
		HLAudioThreadId = 0;
	}

	if (xSaiLowLevelEvents != 0) {
		vQueueDelete(xSaiLowLevelEvents);
		xSaiLowLevelEvents = 0;
	}
	if (LLAudioThreadId != 0) {
		vTaskDelete(LLAudioThreadId);
		LLAudioThreadId = 0;
	}

	return AUDIOPLAYER_ERROR_NONE;
}

AUDIOPLAYER_ErrorTypdef AUDIOPLAYER_Stop(void) {
	haudio.out.state = AUDIOPLAYER_STOP;
	BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);

	if (LLAudioThreadId != 0) {
		vTaskSuspend(LLAudioThreadId);
	}

	return AUDIOPLAYER_ERROR_NONE;
}

AUDIOPLAYER_ErrorTypdef AUDIOPLAYER_Mute(uint8_t state) {
	BSP_AUDIO_OUT_SetMute(state);

	return AUDIOPLAYER_ERROR_NONE;
}

static void AUDIO_TransferComplete_CallBack(void) {
	if (haudio.out.state == AUDIOPLAYER_PLAY) {
		BSP_AUDIO_OUT_ChangeBuffer((uint16_t*) &haudio.buff[0], AUDIO_OUT_BUFFER_SIZE / 2);

		uint16_t u16Temp = PLAY_BUFFER_OFFSET_FULL;
		xQueueSendFromISR( xSaiLowLevelEvents, ( void * ) &u16Temp, ( TickType_t ) 0 );
	}
}

static void AUDIO_HalfTransfer_CallBack(void) {
	if (haudio.out.state == AUDIOPLAYER_PLAY) {
		BSP_AUDIO_OUT_ChangeBuffer((uint16_t*) &haudio.buff[AUDIO_OUT_BUFFER_SIZE / 2], AUDIO_OUT_BUFFER_SIZE / 2);

		uint16_t u16Temp = PLAY_BUFFER_OFFSET_HALF;
		xQueueSendFromISR( xSaiLowLevelEvents, ( void * ) &u16Temp, ( TickType_t ) 0 );
	}
}

static void AUDIO_Error_CallBack(void) {
	haudio.out.state = AUDIOPLAYER_ERROR;
}

static void Audio_LL_Thread(void * argument) {
	uint32_t numOfReadBytes;

	for (;;) {
		uint16_t u16Temp;
		if (xQueueReceive(xSaiLowLevelEvents, &u16Temp, 100)) {
			if (haudio.out.state == AUDIOPLAYER_PLAY) {
				switch (u16Temp) {
				case PLAY_BUFFER_OFFSET_HALF:
					read_raw_data(&haudio.buff[0], CurrentSoundEffect,
							AUDIO_OUT_BUFFER_SIZE / 2, &numOfReadBytes);

					if (numOfReadBytes == 0) {
						haudio.out.state = AUDIOPLAYER_EOF;
					}
					break;

				case PLAY_BUFFER_OFFSET_FULL:
					read_raw_data(&haudio.buff[AUDIO_OUT_BUFFER_SIZE / 2],
							CurrentSoundEffect, AUDIO_OUT_BUFFER_SIZE / 2,
							&numOfReadBytes);

					if (numOfReadBytes == 0) {
						haudio.out.state = AUDIOPLAYER_EOF;
					}
					break;

				default:
					break;
				}
			}
		}
	}
}

static void Audio_HL_Thread(void * argument) {
	uint16_t u16Temp = 0;

	for (;;) {
		if (xQueueReceive(xAudioEffectsQueue, &u16Temp, 100)) {
//			DEBUG_SendTextFrame("Audio_HL_Thread - SOUND: %x, %d", xAudioEffectsQueue, u16Temp);

			switch (u16Temp) {
			case 1:
				AUDIOPLAYER_Play(0);
				break;

			case 2:
				AUDIOPLAYER_Play(1);
				break;

			case 3:
				AUDIOPLAYER_Play(2);
				break;
			}
		}
//			else {
//			DEBUG_SendTextFrame("Audio_HL_Thread - NO SOUND");
//		}

		AUDIOPLAYER_Process();
	}
}

