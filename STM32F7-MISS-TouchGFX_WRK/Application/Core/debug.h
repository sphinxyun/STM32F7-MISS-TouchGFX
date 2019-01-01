#ifndef DEBUG_H_
#define DEBUG_H_

#include "global.h"
#include "stm32f7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AD_SIG_0_SET_LOW		HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_RESET)
#define AD_SIG_0_SET_HIGH		HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_SET)
#define AD_SIG_0_TOGGLE			HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_14)

#define AD_SIG_1_SET_LOW		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET)
#define AD_SIG_1_SET_HIGH		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET)
#define AD_SIG_1_TOGGLE			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_3)

#define AD_SIG_2_SET_LOW		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET)
#define AD_SIG_2_SET_HIGH		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET)
#define AD_SIG_2_TOGGLE			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5)

#define AD_SIG_3_SET_LOW		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET)
#define AD_SIG_3_SET_HIGH		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET)
#define AD_SIG_3_TOGGLE			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6)

#define AD_SIG_4_SET_LOW		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define AD_SIG_4_SET_HIGH		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define AD_SIG_4_TOGGLE			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4)

#define LED0_SET_LOW			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_RESET)
#define LED0_SET_HIGH			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_SET)
#define LED0_TOGGLE				HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_9)

#define LED3_SET_LOW			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET)
#define LED3_SET_HIGH			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET)
#define LED3_TOGGLE				HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_12)

void DEBUG_Init(void);
void DEBUG_ResetCommunication(void);

void DEBUG_SendTextHeader(void);

void COMM_ProcessRxedData(void);

void DEBUG_SendDataFrame(const char* cName, const uint8_t u8NoOfRows, const uint8_t u8NoOfCols, const float *pfData);
void DEBUG_SendTextFrame(const char* str, ...);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */
