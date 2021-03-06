#include "irrigation.h"

#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_tim.h"

#include "debug.h"
#include "global.h"

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t MotorsTaskId;

TIM_HandleTypeDef    PWM_TimHandle;

TIM_HandleTypeDef    ENCODER_TimHandle;
TIM_HandleTypeDef 	 SPEED_TimerHandle;

static inline void Configure_ENCODER(void);
static inline void Configure_PWM(void);
static inline void Configure_DIAGNOSTICS(void);

volatile uint32_t pulse_count; // Licznik impulsow

TIM_OC_InitTypeDef sPWMConfig;

#define ENCODER_MAX_PULSES		( 2004 - 1 )

#define SHRT_BUFFER_LENGTH	10
volatile static int32_t i32ShrtSpeedSum;
volatile static int16_t i16ShrtSpeedBuff[SHRT_BUFFER_LENGTH];
volatile static uint16_t u16ShrtSpeedIdx = 0;

#define MED_BUFFER_LENGTH	25
volatile static int32_t i32MedSpeedSum;
volatile static int16_t i16MedSpeedBuff[MED_BUFFER_LENGTH];
volatile static uint16_t u16MedSpeedIdx = 0;

//volatile static int32_t i32Speed = 0;

#define DISABLE_IRRIGATION_MOTOR_DRIVER		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET)
#define ENABLE_IRRIGATION_MOTOR_DRIVER		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET)

#define IRRIGATION_MOTOR_FORWARD_DIR		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
#define IRRIGATION_MOTOR_BACKWARD_DIR		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)

void TIM3_IRQHandler(void) {
	HAL_TIM_IRQHandler(&SPEED_TimerHandle);

	BaseType_t xHigherPriorityTaskWoken;

	uint32_t temp = TIM5->CNT;

	int32_t speed_pulse = temp - pulse_count;
	if (speed_pulse > 1500) speed_pulse = -(ENCODER_MAX_PULSES + 1 - temp + pulse_count);
	if (speed_pulse < -1500) speed_pulse = ENCODER_MAX_PULSES + 1 - pulse_count + temp;

	pulse_count = temp;

	i32ShrtSpeedSum -= i16ShrtSpeedBuff[u16ShrtSpeedIdx];
	i16ShrtSpeedBuff[u16ShrtSpeedIdx] = speed_pulse;
	i32ShrtSpeedSum += i16ShrtSpeedBuff[u16ShrtSpeedIdx];

	GET_NEXT_BUFF_U16_IDX(u16ShrtSpeedIdx, SHRT_BUFFER_LENGTH);

//	i32MedSpeedSum -= i16MedSpeedBuff[u16MedSpeedIdx];
//	i16MedSpeedBuff[u16MedSpeedIdx] = speed_pulse;
//	i32MedSpeedSum += i16MedSpeedBuff[u16MedSpeedIdx];
//
//	GET_NEXT_BUFF_U16_IDX(u16MedSpeedIdx, MED_BUFFER_LENGTH);

	i32ShrtSpeedSum = speed_pulse;

	AD_SIG_1_TOGGLE;

//	static int i = 0;
//	if (++i >= 2) {
//		i = 0;
		xTaskNotifyFromISR( MotorsTaskId, i32ShrtSpeedSum, eSetValueWithOverwrite, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//	}
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

void IRRIGATION_StartSpeedMonitoring(void) {
	__HAL_RCC_TIM3_CLK_ENABLE();

	TIM5->CNT = 0;
	pulse_count = 0;

	i32ShrtSpeedSum = 0;
	for (uint32_t i = 0; i < SHRT_BUFFER_LENGTH; i++)
		i16ShrtSpeedBuff[i] = 0;
	u16ShrtSpeedIdx = 0;

	i32MedSpeedSum = 0;
	for (uint32_t i = 0; i < MED_BUFFER_LENGTH; i++)
		i16MedSpeedBuff[i] = 0;
	u16MedSpeedIdx = 0;

	/*
	 TIM3 input clock (TIM3CLK) is same as 2x APB1 clock (PCLK1), because:
	  - APB1 Prescaler is set to 4 (check SystemClock_Config),
	  - additional x2 multiplier on APB1 Timer Clocks

	 TIM3CLK = 200 MHz (HCLK) / 4 * 2 = 200 MHz (HCLK) / 2 = 100 MHz

	 Max. motor RPMs @ 32V (no load) equal to 1170 RPM (~1200 RPM)
	 1200 RPM / 60 s = 20 RPS (revolutions pre second) - timer must be called at least that often
	 to avoid problems with lost revolutions 20 times per second = 1/20 = every 50 ms

	 In order to detect direction in which the motor was moving (when 0-2003 transition is made
	 in the reverse direction - for eg. 45 -> 1998) this must be increased x2, so TIM3 is triggered every 25 ms
	 or even higher x4, so TIM3 is triggered every 12,5ms

	 We configure measurement period to be 20 Hz
	 100 MHz / 50000 = 2000 counts - prescaller equal to 54000 is needed
	 50 ms   = 0,050 s  -> 20 times per second, so 2000 counts divided by 100 is 20 times per second
	 25 ms   = 0,025 s  -> 40 times per second, so 2000 counts divided by  50 is 40 times per second
	 12,5 ms = 0,0125 s -> 80 times per second, so 2000 counts divided by  25 is 80 times per second
	 */
	SPEED_TimerHandle.Instance = TIM3;
	SPEED_TimerHandle.Init.Period = 25 - 1;
	SPEED_TimerHandle.Init.Prescaler = 50000;
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

	//DISABLE OUTPUT:
	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	DISABLE_IRRIGATION_MOTOR_DRIVER;

	/*
	 TIM12 input clock (TIM12CLK) is same as 2x APB1 clock (PCLK1), because:
	  - APB1 Prescaler is set to 4 (check SystemClock_Config),
	  - additional x2 multiplier on APB1 Timer Clocks

	 TIM12CLK = 200 MHz (HCLK) / 4 * 2 = 200 MHz (HCLK) / 2 = 100 MHz

	 We configure PWM Output Frequency to be equal to 20 kHz:
	 100 MHz / 20 kHz = 5000 counts - no prescaller is needed - TIM12CLK will not be prescalled
	 We need (100 MHz / 20 kHz) - 1 = 5000 - 1 = 4999 counts to get proper TIM12 period.

	 And so:
	  -    0 counts =   0% PWM
	  - 2500 counts =  50% PWM
	  - 5000 counts = 100% PWM
	 */

	PWM_TimHandle.Instance = TIM12;
	PWM_TimHandle.Init.Prescaler = 0;
	PWM_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	PWM_TimHandle.Init.Period = MAX_IRRIGATION_DRIVE - 1;
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

	//DIRECTION OUTPUT:
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	IRRIGATION_MOTOR_FORWARD_DIR;

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

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

SPI_HandleTypeDef SpiHandle;

#define MOTOR_nSS_SET_LOW		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET)
#define MOTOR_nSS_SET_HIGH		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET)


//void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
//{
//  if(hspi->Instance == SPIx)
//  {
//    /*##-1- Reset peripherals ##################################################*/
//    SPIx_FORCE_RESET();
//    SPIx_RELEASE_RESET();
//
//    /*##-2- Disable peripherals and GPIO Clocks ################################*/
//    /* Configure SPI SCK as alternate function  */
//    HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
//    /* Configure SPI MISO as alternate function  */
//    HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
//    /* Configure SPI MOSI as alternate function  */
//    HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
//  }
//}

void Configure_DIAGNOSTICS(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* SPI nSS (PA8) GPIO pin configuration  */
	MOTOR_nSS_SET_HIGH;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* SPI SCK (PI1) GPIO pin configuration  */
	__HAL_RCC_GPIOI_CLK_ENABLE();

	GPIO_InitStruct.Pin       = GPIO_PIN_1;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

	/* SPI MISO (PB14) & MOSI (PB15) GPIO pin configuration  */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__HAL_RCC_SPI2_CLK_ENABLE();

	/* Set the SPI parameters */
	SpiHandle.Instance               = SPI2;
	SpiHandle.Init.Mode 			 = SPI_MODE_MASTER;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
	SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.CRCPolynomial     = 7;
	SpiHandle.Init.NSS               = SPI_NSS_SOFT;

	if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
	{
		/* Initialization Error */
//		Error_Handler();
	}
}

uint8_t IRRIGATION_ReadDiagnostics(void) {
	uint8_t u8Temp = 0x00;
	uint8_t u8Rx;

	MOTOR_nSS_SET_LOW;
	u8Temp = 0x00;
	HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t *)&u8Temp, (uint8_t *)&u8Rx, 1, 5000);
	MOTOR_nSS_SET_HIGH;

	return u8Rx;
}

void IRRIGATION_Configure(void) {
	Configure_ENCODER();
	Configure_PWM();
	Configure_DIAGNOSTICS();
}

void IRRIGATION_Start(int16_t i16PWM) {
//	TIM5->CNT = 0;
//	pulse_count = 0;
//
//	i32SpeedSum = 0;
//	for (uint32_t i = 0; i < AVG_BUFFER_SIZE; i++)
//		i32SpeedBuff[i] = 0;
//	u16SpeedIdx = 0;

	if (ABS(i16PWM) <= MAX_IRRIGATION_DRIVE) {

		if (i16PWM >= 0)
			IRRIGATION_MOTOR_FORWARD_DIR;
		else
			IRRIGATION_MOTOR_BACKWARD_DIR;

		__HAL_TIM_SET_COMPARE(&PWM_TimHandle, TIM_CHANNEL_1, ABS(i16PWM));
	}

	ENABLE_IRRIGATION_MOTOR_DRIVER;
}

void IRRIGATION_UpdateSpeed(int16_t i16PWM) {
	if (ABS(i16PWM) <= MAX_IRRIGATION_DRIVE) {

		if (i16PWM >= 0)
			IRRIGATION_MOTOR_FORWARD_DIR;
		else
			IRRIGATION_MOTOR_BACKWARD_DIR;

		__HAL_TIM_SET_COMPARE(&PWM_TimHandle, TIM_CHANNEL_1, ABS(i16PWM));
	}
}

void IRRIGATION_Stop(void) {
	DISABLE_IRRIGATION_MOTOR_DRIVER;

	__HAL_TIM_SET_COMPARE(&PWM_TimHandle, TIM_CHANNEL_1, 0);
}
