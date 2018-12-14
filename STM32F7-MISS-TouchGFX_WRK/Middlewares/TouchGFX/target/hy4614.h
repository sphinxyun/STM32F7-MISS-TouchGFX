#ifndef __HY4614_H
#define __HY4614_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Set Multi-touch as supported */
#if !defined(TS_MONO_TOUCH_SUPPORTED)
	#define TS_MULTI_TOUCH_SUPPORTED        0
#endif /* TS_MONO_TOUCH_SUPPORTED */

#define DISCOVERY_AUDIO_I2Cx                             I2C3
#define DISCOVERY_AUDIO_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C3_CLK_ENABLE()
#define DISCOVERY_AUDIO_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()

#define DISCOVERY_AUDIO_I2Cx_FORCE_RESET()               __HAL_RCC_I2C3_FORCE_RESET()
#define DISCOVERY_AUDIO_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C3_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_AUDIO_I2Cx_SCL_PIN                     GPIO_PIN_7
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_PORT           GPIOH
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C3
#define DISCOVERY_AUDIO_I2Cx_SDA_PIN                     GPIO_PIN_8

/* I2C interrupt requests */
#define DISCOVERY_AUDIO_I2Cx_EV_IRQn                     I2C3_EV_IRQn
#define DISCOVERY_AUDIO_I2Cx_ER_IRQn                     I2C3_ER_IRQn

/**
  * @brief Touch screen interrupt signal
  */
#define TS_INT_PIN                           	GPIO_PIN_13
#define TS_INT_GPIO_PORT                     	GPIOI
#define TS_INT_GPIO_CLK_ENABLE()             	__HAL_RCC_GPIOI_CLK_ENABLE()
#define TS_INT_GPIO_CLK_DISABLE()            	__HAL_RCC_GPIOI_CLK_DISABLE()
#define TS_INT_EXTI_IRQn                     	EXTI15_10_IRQn


#define TS_RESET_PIN                           	GPIO_PIN_13
#define TS_RESET_GPIO_PORT                     	GPIOH
#define TS_RESET_GPIO_CLK_ENABLE()             	__HAL_RCC_GPIOH_CLK_ENABLE()
#define TS_RESET_GPIO_CLK_DISABLE()            	__HAL_RCC_GPIOH_CLK_DISABLE()

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated from APB1 source clock = 50 MHz */
/* Due to the big MOFSET capacity for adapting the camera level the rising time is very large (>1us) */
/* 0x40912732 takes in account the big rising and aims a clock of 100khz */
#ifndef DISCOVERY_I2Cx_TIMING
#define DISCOVERY_I2Cx_TIMING                      ((uint32_t)0x40912732)
#endif /* DISCOVERY_I2Cx_TIMING */

/* Macros --------------------------------------------------------------------*/

#if defined(HY4614_ENABLE_ASSERT)
/* Assert activated */
#define HY4614_ASSERT(__condition__)      do { if(__condition__) \
                                               {  \
                                                 while(1);  \
                                               } \
                                          } while(0)
#else
/* Assert not activated : macro has no effect */
#define HY4614_ASSERT(__condition__)    do { if(__condition__) \
                                             {  \
                                                ;  \
                                             } \
                                            }while(0)
#endif /* HY4614_ENABLE_ASSERT == 1 */

/** @typedef HY4614_handle_TypeDef
 *  HY4614 Handle definition.
 */
typedef struct
{
  uint8_t i2cInitialized;

  /* field holding the current number of simultaneous active touches */
  uint8_t currActiveTouchNb;

  /* field holding the touch index currently managed */
  uint8_t currActiveTouchIdx;

} HY4614_handle_TypeDef;

  /* I2C Slave address of touchscreen FocalTech HY4614 */
#define HY4614_I2C_SLAVE_ADDRESS            ((uint8_t)0x38<<1)

  /* Maximum border values of the touchscreen pad */
#define HY4614_MAX_WIDTH                    ((uint16_t)640)     /* Touchscreen pad max width   */
#define HY4614_MAX_HEIGHT                   ((uint16_t)480)     /* Touchscreen pad max height  */

  /* Possible values of driver functions return status */
#define HY4614_STATUS_OK                    ((uint8_t)0x00)
#define HY4614_STATUS_NOT_OK                ((uint8_t)0x01)

  /* Possible values of global variable 'TS_I2C_Initialized' */
#define HY4614_I2C_NOT_INITIALIZED          ((uint8_t)0x00)
#define HY4614_I2C_INITIALIZED              ((uint8_t)0x01)

  /* Max detectable simultaneous touches */
#define HY4614_MAX_DETECTABLE_TOUCH         ((uint8_t)0x05)

  /**
   * @brief : Definitions for HY4614 I2C register addresses on 8 bit
   **/

  /* Current mode register of the HY4614 (R/W) */
#define HY4614_DEV_MODE_REG                 ((uint8_t)0x00)

  /* Possible values of HY4614_DEV_MODE_REG */
#define HY4614_DEV_MODE_WORKING             ((uint8_t)0x00)
#define HY4614_DEV_MODE_FACTORY             ((uint8_t)0x04)

#define HY4614_DEV_MODE_MASK                ((uint8_t)0x07)
#define HY4614_DEV_MODE_SHIFT               ((uint8_t)0x04)

  /* Gesture ID register */
#define HY4614_GEST_ID_REG                  ((uint8_t)0x01)

  /* Possible values of HY4614_GEST_ID_REG */
#define HY4614_GEST_ID_NO_GESTURE           ((uint8_t)0x00)
#define HY4614_GEST_ID_MOVE_UP              ((uint8_t)0x10)
#define HY4614_GEST_ID_MOVE_RIGHT           ((uint8_t)0x14)
#define HY4614_GEST_ID_MOVE_DOWN            ((uint8_t)0x18)
#define HY4614_GEST_ID_MOVE_LEFT            ((uint8_t)0x1C)
#define HY4614_GEST_ID_SINGLE_CLICK         ((uint8_t)0x20)
#define HY4614_GEST_ID_DOUBLE_CLICK         ((uint8_t)0x22)
#define HY4614_GEST_ID_ROTATE_CLOCKWISE     ((uint8_t)0x28)
#define HY4614_GEST_ID_ROTATE_C_CLOCKWISE   ((uint8_t)0x29)
#define HY4614_GEST_ID_ZOOM_IN              ((uint8_t)0x40)
#define HY4614_GEST_ID_ZOOM_OUT             ((uint8_t)0x49)

  /* Touch Data Status register : gives number of active touch points (0..5) */
#define HY4614_TD_STAT_REG                  ((uint8_t)0x02)

  /* Values related to HY4614_TD_STAT_REG */
#define HY4614_TD_STAT_MASK                 ((uint8_t)0x0F)
#define HY4614_TD_STAT_SHIFT                ((uint8_t)0x00)

  /* Values Pn_XH and Pn_YH related */
#define HY4614_TOUCH_EVT_FLAG_PRESS_DOWN    ((uint8_t)0x00)
#define HY4614_TOUCH_EVT_FLAG_LIFT_UP       ((uint8_t)0x01)
#define HY4614_TOUCH_EVT_FLAG_CONTACT       ((uint8_t)0x02)
#define HY4614_TOUCH_EVT_FLAG_NO_EVENT      ((uint8_t)0x03)

#define HY4614_TOUCH_EVT_FLAG_SHIFT         ((uint8_t)0x06)
#define HY4614_TOUCH_EVT_FLAG_MASK          ((uint8_t)(3 << HY4614_TOUCH_EVT_FLAG_SHIFT))

#define HY4614_TOUCH_POS_MSB_MASK           ((uint8_t)0x0F)
#define HY4614_TOUCH_POS_MSB_SHIFT          ((uint8_t)0x00)

  /* Values Pn_XL and Pn_YL related */
#define HY4614_TOUCH_POS_LSB_MASK           ((uint8_t)0xFF)
#define HY4614_TOUCH_POS_LSB_SHIFT          ((uint8_t)0x00)

/* Values Pn_WEIGHT related  */
#define HY4614_TOUCH_WEIGHT_MASK            ((uint8_t)0xFF)
#define HY4614_TOUCH_WEIGHT_SHIFT           ((uint8_t)0x00)

/* Values related to HY4614_Pn_MISC_REG */
#define HY4614_TOUCH_AREA_MASK              ((uint8_t)(0x04 << 4))
#define HY4614_TOUCH_AREA_SHIFT             ((uint8_t)0x04)

#define HY4614_P1_BASE_REG                  ((uint8_t)0x03)
#define HY4614_P1_WEIGHT_REG                ((uint8_t)0x07)
#define HY4614_P1_MISC_REG                  ((uint8_t)0x08)

#define HY4614_P2_BASE_REG                  ((uint8_t)0x09)
#define HY4614_P2_WEIGHT_REG                ((uint8_t)0x0D)
#define HY4614_P2_MISC_REG                  ((uint8_t)0x0E)

#define HY4614_P3_BASE_REG                  ((uint8_t)0x0F)
#define HY4614_P3_WEIGHT_REG                ((uint8_t)0x13)
#define HY4614_P3_MISC_REG                  ((uint8_t)0x14)

#define HY4614_P4_BASE_REG                  ((uint8_t)0x15)
#define HY4614_P4_WEIGHT_REG                ((uint8_t)0x19)
#define HY4614_P4_MISC_REG                  ((uint8_t)0x1A)

#define HY4614_P5_BASE_REG                  ((uint8_t)0x1B)
#define HY4614_P5_WEIGHT_REG                ((uint8_t)0x1F)
#define HY4614_P5_MISC_REG                  ((uint8_t)0x20)
//
//#define HY4614_P6_XH_REG                    ((uint8_t)0x21)
//#define HY4614_P6_XL_REG                    ((uint8_t)0x22)
//#define HY4614_P6_YH_REG                    ((uint8_t)0x23)
//#define HY4614_P6_YL_REG                    ((uint8_t)0x24)
//#define HY4614_P6_WEIGHT_REG                ((uint8_t)0x25)
//#define HY4614_P6_MISC_REG                  ((uint8_t)0x26)
//
//#define HY4614_P7_XH_REG                    ((uint8_t)0x27)
//#define HY4614_P7_XL_REG                    ((uint8_t)0x28)
//#define HY4614_P7_YH_REG                    ((uint8_t)0x29)
//#define HY4614_P7_YL_REG                    ((uint8_t)0x2A)
//#define HY4614_P7_WEIGHT_REG                ((uint8_t)0x2B)
//#define HY4614_P7_MISC_REG                  ((uint8_t)0x2C)
//
//#define HY4614_P8_XH_REG                    ((uint8_t)0x2D)
//#define HY4614_P8_XL_REG                    ((uint8_t)0x2E)
//#define HY4614_P8_YH_REG                    ((uint8_t)0x2F)
//#define HY4614_P8_YL_REG                    ((uint8_t)0x30)
//#define HY4614_P8_WEIGHT_REG                ((uint8_t)0x31)
//#define HY4614_P8_MISC_REG                  ((uint8_t)0x32)
//
//#define HY4614_P9_XH_REG                    ((uint8_t)0x33)
//#define HY4614_P9_XL_REG                    ((uint8_t)0x34)
//#define HY4614_P9_YH_REG                    ((uint8_t)0x35)
//#define HY4614_P9_YL_REG                    ((uint8_t)0x36)
//#define HY4614_P9_WEIGHT_REG                ((uint8_t)0x37)
//#define HY4614_P9_MISC_REG                  ((uint8_t)0x38)
//
//#define HY4614_P10_XH_REG                   ((uint8_t)0x39)
//#define HY4614_P10_XL_REG                   ((uint8_t)0x3A)
//#define HY4614_P10_YH_REG                   ((uint8_t)0x3B)
//#define HY4614_P10_YL_REG                   ((uint8_t)0x3C)
//#define HY4614_P10_WEIGHT_REG               ((uint8_t)0x3D)
//#define HY4614_P10_MISC_REG                 ((uint8_t)0x3E)

  /* Threshold for touch detection */
#define HY4614_TH_GROUP_REG                 ((uint8_t)0x80)

  /* Values HY4614_TH_GROUP_REG : threshold related  */
#define HY4614_THRESHOLD_MASK               ((uint8_t)0xFF)
#define HY4614_THRESHOLD_SHIFT              ((uint8_t)0x00)

  /* Filter function coefficients */
#define HY4614_TH_DIFF_REG                  ((uint8_t)0x85)

  /* Control register */
#define HY4614_CTRL_REG                     ((uint8_t)0x86)

  /* Values related to HY4614_CTRL_REG */

  /* Will keep the Active mode when there is no touching */
#define HY4614_CTRL_KEEP_ACTIVE_MODE        ((uint8_t)0x00)

  /* Switching from Active mode to Monitor mode automatically when there is no touching */
#define HY4614_CTRL_KEEP_AUTO_SWITCH_MONITOR_MODE  ((uint8_t)0x01

  /* The time period of switching from Active mode to Monitor mode when there is no touching */
#define HY4614_TIMEENTERMONITOR_REG         ((uint8_t)0x87)

  /* Report rate in Active mode */
#define HY4614_PERIODACTIVE_REG             ((uint8_t)0x88)

  /* Report rate in Monitor mode */
#define HY4614_PERIODMONITOR_REG            ((uint8_t)0x89)

  /* The value of the minimum allowed angle while Rotating gesture mode */
#define HY4614_RADIAN_VALUE_REG             ((uint8_t)0x91)

  /* Maximum offset while Moving Left and Moving Right gesture */
#define HY4614_OFFSET_LEFT_RIGHT_REG        ((uint8_t)0x92)

  /* Maximum offset while Moving Up and Moving Down gesture */
#define HY4614_OFFSET_UP_DOWN_REG           ((uint8_t)0x93)

  /* Minimum distance while Moving Left and Moving Right gesture */
#define HY4614_DISTANCE_LEFT_RIGHT_REG      ((uint8_t)0x94)

  /* Minimum distance while Moving Up and Moving Down gesture */
#define HY4614_DISTANCE_UP_DOWN_REG         ((uint8_t)0x95)

  /* Maximum distance while Zoom In and Zoom Out gesture */
#define HY4614_DISTANCE_ZOOM_REG            ((uint8_t)0x96)

  /* High 8-bit of LIB Version info */
#define HY4614_LIB_VER_H_REG                ((uint8_t)0xA1)

  /* Low 8-bit of LIB Version info */
#define HY4614_LIB_VER_L_REG                ((uint8_t)0xA2)

  /* Chip Selecting */
#define HY4614_CIPHER_REG                   ((uint8_t)0xA3)

  /* Interrupt mode register (used when in interrupt mode) */
#define HY4614_GMODE_REG                    ((uint8_t)0xA4)

#define HY4614_G_MODE_INTERRUPT_MASK        ((uint8_t)0x03)
#define HY4614_G_MODE_INTERRUPT_SHIFT       ((uint8_t)0x00)

  /* Possible values of HY4614_GMODE_REG */
#define HY4614_G_MODE_INTERRUPT_POLLING     ((uint8_t)0x00)
#define HY4614_G_MODE_INTERRUPT_TRIGGER     ((uint8_t)0x01)

  /* Current power mode the HY4614 system is in (R) */
#define HY4614_PWR_MODE_REG                 ((uint8_t)0xA5)

  /* HY4614 firmware version */
#define HY4614_FIRMID_REG                   ((uint8_t)0xA6)

  /* HY4614 Chip identification register */
#define HY4614_CHIP_ID_REG                  ((uint8_t)0xA8)

  /*  Possible values of HY4614_CHIP_ID_REG */
#define HY4614_ID_VALUE                     ((uint8_t)0x57)

  /* Release code version */
#define HY4614_RELEASE_CODE_ID_REG          ((uint8_t)0xAF)

  /* Current operating mode the HY4614 system is in (R) */
#define HY4614_STATE_REG                    ((uint8_t)0xBC)

/**
 * @brief  Initialize the HY4614 communication bus
 *         from MCU to HY4614 : ie I2C channel initialization (if required).
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @retval None
 */
void HY4614_Init(void);

/**
 * @brief  Software Reset the HY4614.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @retval None
 */
void HY4614_Reset(uint16_t DeviceAddr);

/**
 * @brief  Read the HY4614 device ID, pre initialize I2C in case of need to be
 *         able to read the HY4614 device ID, and verify this is a HY4614.
 * @param  DeviceAddr: I2C HY4614 Slave address.
 * @retval The Device ID (two bytes).
 */
uint16_t HY4614_ReadID(uint16_t DeviceAddr);

/**
 * @brief  Configures the touch Screen IC device to start detecting touches
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
 * @retval None.
 */
void HY4614_TS_Start(uint16_t DeviceAddr);

/**
 * @brief  Return if there is touches detected or not.
 *         Try to detect new touches and forget the old ones (reset internal global
 *         variables).
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval : Number of active touches detected (can be 0, 1 or 2).
 */
uint8_t HY4614_TS_DetectTouch(uint16_t DeviceAddr);

/**
 * @brief  Get the touch screen X and Y positions values
 *         Manage multi touch thanks to touch Index global
 *         variable 'HY4614_handle.currActiveTouchIdx'.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  X: Pointer to X position value
 * @param  Y: Pointer to Y position value
 * @retval None.
 */
void HY4614_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);

/**
 * @brief  Configure the HY4614 device to generate IT on given INT pin
 *         connected to MCU as EXTI.
 * @param  DeviceAddr: Device address on communication Bus (Slave I2C address of HY4614).
 * @retval None
 */
void HY4614_TS_EnableIT(uint16_t DeviceAddr);

/**
 * @brief  Configure the HY4614 device to stop generating IT on the given INT pin
 *         connected to MCU as EXTI.
 * @param  DeviceAddr: Device address on communication Bus (Slave I2C address of HY4614).
 * @retval None
 */
void HY4614_TS_DisableIT(uint16_t DeviceAddr);

/**
 * @brief  Clear IT status in HY4614 interrupt status clear registers
 *         Should be called Following an EXTI coming to the MCU.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @retval TS interrupts status
 */
void HY4614_TS_ClearIT (uint16_t DeviceAddr);

/**** NEW FEATURES enabled when Multi-touch support is enabled ****/

#if (TS_MULTI_TOUCH_SUPPORTED == 1)

/**
 * @brief  Get the last touch gesture identification (zoom, move up/down...).
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @param  pGestureId : Pointer to get last touch gesture Identification.
 * @retval None.
 */
void HY4614_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId);

/**
 * @brief  Get the touch detailed informations on touch number 'touchIdx' (0..1)
 *         This touch detailed information contains :
 *         - weight that was applied to this touch
 *         - sub-area of the touch in the touch panel
 *         - event of linked to the touch (press down, lift up, ...)
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @param  touchIdx : Passed index of the touch (0..1) on which we want to get the
 *                    detailed information.
 * @param  pWeight : Pointer to to get the weight information of 'touchIdx'.
 * @param  pArea   : Pointer to to get the sub-area information of 'touchIdx'.
 * @param  pEvent  : Pointer to to get the event information of 'touchIdx'.

 * @retval None.
 */
void HY4614_TS_GetTouchInfo(uint16_t   DeviceAddr,
                            uint32_t   touchIdx,
                            uint32_t * pWeight,
                            uint32_t * pArea,
                            uint32_t * pEvent);

#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __HY4614_H */
