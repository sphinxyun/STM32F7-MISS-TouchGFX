#include "irrigation.h"

//#include "stm32f7xx.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_tim.h"

#include "global.h"

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t MotorsTaskId;


TIM_HandleTypeDef    PWM_TimHandle;

TIM_HandleTypeDef    ENCODER_TimHandle;
TIM_HandleTypeDef 	 SPEED_TimerHandle;

volatile uint32_t pulse_count; // Licznik impulsow

TIM_OC_InitTypeDef sPWMConfig;

#define ENCODER_MAX_PULSES		( 2004 - 1 )

//volatile static uint32_t spd = 0;

#define AVG_BUFFER_SIZE	20

int32_t i32SpeedSum;
int32_t i32SpeedBuff[AVG_BUFFER_SIZE];
uint16_t u16SpeedIdx = 0;

volatile static int32_t i32Speed = 0;

void TIM3_IRQHandler(void) {
	HAL_TIM_IRQHandler(&SPEED_TimerHandle);

	BaseType_t xHigherPriorityTaskWoken;
	static int i = 0;

	uint32_t temp = TIM5->CNT;
	uint32_t speed_pulse = (temp >= pulse_count) ? (temp - pulse_count) : (ENCODER_MAX_PULSES+1 - pulse_count + temp);
	pulse_count = temp;

	i32SpeedSum -= i32SpeedBuff[u16SpeedIdx];
	i32SpeedBuff[u16SpeedIdx] = speed_pulse;
	i32SpeedSum += i32SpeedBuff[u16SpeedIdx];

	GET_NEXT_BUFF_U16_IDX(u16SpeedIdx, AVG_BUFFER_SIZE);

	i32Speed = i32SpeedSum;

	if (++i >= 5) {
		i = 0;
		xTaskNotifyFromISR( MotorsTaskId, i32Speed, eSetValueWithOverwrite, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

void Configure_ENCODER(void) {
	GPIO_InitTypeDef   GPIO_InitStruct;
	TIM_Encoder_InitTypeDef sENCConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	ENCODER_TimHandle.Instance = TIM5;
	ENCODER_TimHandle.Init.Prescaler = 0;
	ENCODER_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	ENCODER_TimHandle.Init.Period = ENCODER_MAX_PULSES;
	ENCODER_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	ENCODER_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	__HAL_RCC_TIM5_CLK_ENABLE();

	sENCConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sENCConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sENCConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sENCConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sENCConfig.IC1Filter = 15;
	sENCConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sENCConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sENCConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sENCConfig.IC2Filter = 15;

	__HAL_RCC_GPIOH_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
	if (HAL_TIM_Encoder_Init(&ENCODER_TimHandle, &sENCConfig) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&ENCODER_TimHandle, &sMasterConfig) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
	}
}

void StartSpeedMonitoring(void) {
	__HAL_RCC_TIM3_CLK_ENABLE();

	TIM5->CNT = 0;
	i32SpeedSum = 0;
	for (uint32_t i = 0; i < AVG_BUFFER_SIZE; i++)
		i32SpeedBuff[i] = 0;
	u16SpeedIdx = 0;

	/*
	 TIM3 input clock (TIM3CLK) is same as 2x APB1 clock (PCLK1), because:
	  - APB1 Prescaler is set to 4 (check SystemClock_Config),
	  - additional x2 multiplier on APB1 Timer Clocks

	 TIM3CLK = 216 MHz (HCLK) / 4 * 2 = 216 MHz (HCLK) / 2 = 108 MHz

	 Max. motor RPMs @ 32V (no load) equal to 1170 RPM (~1200 RPM)
	 1200 RPM / 60 s = 20 RPS (revolutions pre second) - timer must be called at least that often
	 to avoid problems with lost revolutions 20 times per second = 1/20 = every 50 ms


	 We configure measurement period to be 20 Hz
	 108 MHz / 54000 = 2000 counts - prescaller equal to 54000 is needed
	 */
	SPEED_TimerHandle.Instance = TIM3;
	SPEED_TimerHandle.Init.Period = 100 - 1;
	SPEED_TimerHandle.Init.Prescaler = 54000;
	SPEED_TimerHandle.Init.ClockDivision = 0;
	SPEED_TimerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	SPEED_TimerHandle.Init.RepetitionCounter = 0;

	if (HAL_TIM_Base_Init(&SPEED_TimerHandle) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_TIM_Encoder_Start(&ENCODER_TimHandle, TIM_CHANNEL_1 | TIM_CHANNEL_2) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
	}

	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */
	if (HAL_TIM_Base_Start_IT(&SPEED_TimerHandle) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
	}

	/*##-2- Configure the NVIC for TIMx ########################################*/
	/* Set the TIMx priority */
	HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);

	/* Enable the TIMx global Interrupt */
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void Configure_PWM(void) {
	GPIO_InitTypeDef   GPIO_InitStruct;
	TIM_MasterConfigTypeDef sMasterConfig;

	/*
	 TIM12 input clock (TIM12CLK) is same as 2x APB1 clock (PCLK1), because:
	  - APB1 Prescaler is set to 4 (check SystemClock_Config),
	  - additional x2 multiplier on APB1 Timer Clocks

	 TIM12CLK = 216 MHz (HCLK) / 4 * 2 = 216 MHz (HCLK) / 2 = 108 MHz

	 We configure PWM Output Frequency to be equal to 20 kHz:
	 108 MHz / 20 kHz = 5400 counts - no prescaller is needed - TIM12CLK will not be prescalled
	 We need (108 MHz / 20 kHz) - 1 = 5400 - 1 = 5399 counts to get proper TIM12 period.

	 And so:
	  -    0 counts =   0% PWM
	  - 2700 counts =  50% PWM
	  - 5400 counts = 100% PWM
	 */

	PWM_TimHandle.Instance = TIM12;
	PWM_TimHandle.Init.Prescaler = 0;
	PWM_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	PWM_TimHandle.Init.Period = 5400 - 1; //PERIOD_VALUE;
	PWM_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	PWM_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	__HAL_RCC_TIM12_CLK_ENABLE();

	//PWM OUTPUT:
	__HAL_RCC_GPIOH_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_TIM12;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	//DISABLE
	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET); //RE

	//DIR
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

	if (HAL_TIM_PWM_Init(&PWM_TimHandle) != HAL_OK) {
		/* Initialization Error */
		//_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&ENCODER_TimHandle, &sMasterConfig) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
	}

	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sPWMConfig.OCMode = TIM_OCMODE_PWM1;
	sPWMConfig.Pulse = 0;
	sPWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	sPWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
	sPWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sPWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sPWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;

	/* Set the pulse value for channel 1 */
	sPWMConfig.Pulse = 0;
	if (HAL_TIM_PWM_ConfigChannel(&PWM_TimHandle, &sPWMConfig, TIM_CHANNEL_1) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
	}

	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&PWM_TimHandle, TIM_CHANNEL_1) != HAL_OK) {
		/* PWM Generation Error */
		//_Error_Handler(__FILE__, __LINE__);
	}
}

void IRRIGATION_Start(uint16_t u16PWM) {
	TIM5->CNT = 0;

	i32SpeedSum = 0;
	for (uint32_t i = 0; i < AVG_BUFFER_SIZE; i++)
		i32SpeedBuff[i] = 0;
	u16SpeedIdx = 0;

	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);

	if (u16PWM <= 5399)
		__HAL_TIM_SET_COMPARE(&PWM_TimHandle, TIM_CHANNEL_1, u16PWM);
}

void IRRIGATION_UpdateSpeed(uint16_t u16PWM) {
	if (u16PWM <= 5399)
		__HAL_TIM_SET_COMPARE(&PWM_TimHandle, TIM_CHANNEL_1, u16PWM);
}

void IRRIGATION_Stop(void) {
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&PWM_TimHandle, TIM_CHANNEL_1, 0);
}
