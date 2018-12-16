#ifndef __AUDIO_APP_H
#define __AUDIO_APP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "global.h"
#include "driver/audio_if.h"
#include "driver/audio_drv.h"

#define MUTE_OFF                      0x00
#define MUTE_ON                       0x01
#define DEFAULT_AUDIO_VOLUME          50

#define CALIBRATION_AUDIO_SETTING_BKP   RTC_BKP_DR0

typedef enum
{
  AUDIOPLAYER_ERROR_NONE = 0, 
  AUDIOPLAYER_ERROR_IO,
  AUDIOPLAYER_ERROR_HW, 
  AUDIOPLAYER_ERROR_MEM, 
  AUDIOPLAYER_ERROR_FORMAT_NOTSUPPORTED,   
  
}AUDIOPLAYER_ErrorTypdef;

#define AUDIOPLAYER_StateTypdef  OUT_StateTypdef
/* Exported types ------------------------------------------------------------*/


/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_Init(uint8_t volume);
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_Play(uint8_t u8SoundEffect);
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_Stop(void);
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_Mute(uint8_t state);
AUDIOPLAYER_StateTypdef  AUDIOPLAYER_GetState(void);
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_Process(void);
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_DeInit(void);
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_SetVolume(uint32_t volume);
uint32_t                 AUDIOPLAYER_GetVolume(void);
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_NotifyEndOfFile(void);

#ifdef __cplusplus
}
#endif

#endif /*__AUDIO_APP_H */
