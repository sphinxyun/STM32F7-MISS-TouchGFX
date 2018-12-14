/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.10.0 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
  


#include <common/TouchGFXInit.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include <touchgfx/hal/GPIO.hpp>
#include <platform/driver/lcd/LCD16bpp.hpp>
#include <platform/driver/lcd/LCD24bpp.hpp>

#include <STM32F7HAL.hpp>
#include <STM32F7DMA.hpp>

#include <STM32F746GTouchController.hpp>
#include <STM32F7Instrumentation.hpp>

/***********************************************************
 ******   Single buffer in internal RAM              *******
 ***********************************************************
 * On this platform, TouchGFX is able to run using a single
 * frame buffer in internal SRAM, thereby avoiding the need
 * for external SDRAM.
 * This feature was introduced in TouchGFX 4.7.0. To enable it,
 * uncomment the define below. The function touchgfx_init()
 * later in this file will check for this define and configure
 * TouchGFX accordingly.
 * For details on the single buffer strategy, please refer to
 * the knowledge base article "Single vs double buffering in TouchGFX"
 * on our support site.
 */
//#define SINGLE_FRAME_BUFFER_INTERNAL

/***********************************************************
 ******         24 Bits Per Pixel Support            *******
 ***********************************************************
 *
 * The default bit depth of the framebuffer is 16bpp. If you want 24bpp support, define the symbol "USE_BPP" with a value
 * of "24", e.g. "USE_BPP=24". This symbol affects the following:
 *
 * 1. Type of TouchGFX LCD (16bpp vs 24bpp)
 * 2. Bit depth of the framebuffer(s)
 * 3. TFT controller configuration.
 *
 * WARNING: Remember to modify your image formats accordingly in app/config/. Please see the following knowledgebase article
 * for further details on how to choose and configure the appropriate image formats for your application:
 *
 * https://touchgfx.zendesk.com/hc/en-us/articles/206725849
 */

extern "C"
{
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_qspi.h"

#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_rcc_ex.h"
#include "stm32f7xx_hal_tim.h"

#include "../Components/rk043fn48h/rk043fn48h.h"

static void SystemClock_Config(void);
}

TIM_HandleTypeDef htim13;
LTDC_HandleTypeDef hltdc;
DMA2D_HandleTypeDef hdma2d;

#ifdef SINGLE_FRAME_BUFFER_INTERNAL
uint16_t frameBuffer[640 * 480];
static uint32_t frameBuf0 = (uint32_t)& frameBuffer[0];
#else
// Use SDRAM for frame buffers
static uint32_t frameBuf0 = (uint32_t)(0xC0000000);
#endif

/**
  * @brief  Initializes the LTDC MSP.
  * @param  None
  * @retval None
  */
static void LCD_MspInit()
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable the LTDC and DMA2D clocks */
    __HAL_RCC_LTDC_CLK_ENABLE();
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    __HAL_RCC_GPIOK_CLK_ENABLE();
    LCD_DISP_GPIO_CLK_ENABLE();
//    LCD_BL_CTRL_GPIO_CLK_ENABLE();

    /*** LTDC Pins configuration ***/
    /* GPIOE configuration */
    gpio_init_structure.Pin       = GPIO_PIN_4;
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull      = GPIO_NOPULL;
    gpio_init_structure.Speed     = GPIO_SPEED_FAST;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;  
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    /* GPIOG configuration */
    gpio_init_structure.Pin       = GPIO_PIN_12;
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOG, &gpio_init_structure);

    /* GPIOI LTDC alternate configuration */
    gpio_init_structure.Pin       = GPIO_PIN_9 | GPIO_PIN_10 |
                                    GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOI, &gpio_init_structure);

    /* GPIOJ configuration */  
    gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                                    GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                                    GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                                    GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOJ, &gpio_init_structure);  

    /* GPIOK configuration */  
    gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                                    GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOK, &gpio_init_structure);

    /* LCD_DISP GPIO configuration */
    gpio_init_structure.Pin       = LCD_DISP_PIN;     /* LCD_DISP pin has to be manually controlled */
    gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LCD_DISP_GPIO_PORT, &gpio_init_structure);

//    /* LCD_BL_CTRL GPIO configuration */
//    gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;  /* LCD_BL_CTRL pin has to be manually controlled */
//    gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
//    HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
}

extern "C" {
uint32_t LCD_GetXSize(void)
{
    return RK043FN48H_WIDTH;
}

uint32_t LCD_GetYSize(void)
{
    return RK043FN48H_HEIGHT;
}
}

/**
  * @brief  Initializes the LCD layers.
  * @param  LayerIndex: the layer foreground or background. 
  * @param  FB_Address: the layer frame buffer.
  */
static void LCD_LayerDefaultInit(uint16_t LayerIndex, uint32_t FB_Address)
{     
    LTDC_LayerCfgTypeDef Layercfg;

    /* Layer Init */
    Layercfg.WindowX0 = 0;
    Layercfg.WindowX1 = LCD_GetXSize();
    Layercfg.WindowY0 = 0;
    Layercfg.WindowY1 = LCD_GetYSize();
#if USE_BPP == 16
    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
#elif USE_BPP == 24
    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
#else
#error Unknown USE_BPP
#endif
    Layercfg.FBStartAdress = FB_Address;
    Layercfg.Alpha = 255;
    Layercfg.Alpha0 = 0;
    Layercfg.Backcolor.Blue = 0;
    Layercfg.Backcolor.Green = 0;
    Layercfg.Backcolor.Red = 0;
    Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    Layercfg.ImageWidth = LCD_GetXSize();
    Layercfg.ImageHeight = LCD_GetYSize();

    HAL_LTDC_ConfigLayer(&hltdc, &Layercfg, LayerIndex);
}

static uint8_t LCD_Init(void)
{
    RCC_PeriphCLKInitTypeDef periph_clk_init_struct;

    TIM_OC_InitTypeDef sConfigOC;
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();

	/* LCD_DISP GPIO configuration - pin has to be manually controlled */
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

	/* LCD_BL_CTRL GPIO configuration - pin has to be manually controlled */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* LCD_RESET GPIO configuration - pin has to be manually controlled */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/* Assert backlight LCD_BL_CTRL pin */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);

	/* Assert display enable LCD_DISP pin */
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_12, GPIO_PIN_RESET);

	/* Assert LCD RESET */
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET); /* Assert LCD_BL_CTRL pin */
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_RESET); /* Assert LCD_BL_CTRL pin */
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET); /* Assert LCD_BL_CTRL pin */
	HAL_Delay(100);

    LCD_MspInit();

    /* RK043FN48H LCD clock configuration */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
    /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
    periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    periph_clk_init_struct.PLLSAI.PLLSAIN = 384; //192;
    periph_clk_init_struct.PLLSAI.PLLSAIR = 7; //RK043FN48H_FREQUENCY_DIVIDER;
    periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_2; //RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);

    /* The RK043FN48H LCD 480x272 is selected */
    /* Timing Configuration */
//    hltdc.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
//    hltdc.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
//    hltdc.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
//    hltdc.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
//    hltdc.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
//    hltdc.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
//    hltdc.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
//    hltdc.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);

	hltdc.Init.HorizontalSync = 29;
	hltdc.Init.VerticalSync = 2;
	hltdc.Init.AccumulatedHBP = 173;
	hltdc.Init.AccumulatedVBP = 37;
	hltdc.Init.AccumulatedActiveW = 813;
	hltdc.Init.AccumulatedActiveH = 517;
	hltdc.Init.TotalWidth = 829;
	hltdc.Init.TotalHeigh = 527;

    /* Initialize the LCD pixel width and pixel height */
    hltdc.LayerCfg->ImageWidth  = RK043FN48H_WIDTH;
    hltdc.LayerCfg->ImageHeight = RK043FN48H_HEIGHT;

    /* Background value */
    hltdc.Init.Backcolor.Blue = 0;
    hltdc.Init.Backcolor.Green = 0;
    hltdc.Init.Backcolor.Red = 0;

    /* Polarity */
    hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;  
    hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc.Instance = LTDC;

    HAL_LTDC_Init(&hltdc);

    /* Assert display enable LCD_DISP pin */
    HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);

    /* Assert backlight LCD_BL_CTRL pin */
//    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

    /* Configure LTDC layer framebuffer address */
    LCD_LayerDefaultInit(0, frameBuf0);

    /* Enable LTDC */
    __HAL_LTDC_ENABLE(&hltdc);

    /*
	 TIM13 input clock (TIM13CLK) is same as 2x APB1 clock (PCLK1), because:
	  - APB1 Prescaler is set to 4 (check SystemClock_Config),
	  - additional x2 multiplier on APB1 Timer Clocks

	 TIM13CLK = 216 MHz (HCLK) / 4 * 2 = 216 MHz (HCLK) / 2 = 108 MHz

	 We configure PWM Output Frequency to be equal to 20 kHz:
	 108 MHz / 20 kHz = 5400 counts - no prescaller is needed - TIM13CLK will not be prescalled
	 We need (108 MHz / 20 kHz) - 1 = 5400 - 1 = 5399 counts to get proper TIM12 period.

	 And so:
	  -    0 counts =   0% PWM
	  - 2700 counts =  50% PWM
	  - 5400 counts = 100% PWM
	 */

	__HAL_RCC_TIM13_CLK_ENABLE();

	htim13.Instance = TIM13;
	htim13.Init.Prescaler = 0;
	htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim13.Init.Period = 5400 - 1;
	htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
	{
//		_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_TIM_PWM_Init(&htim13) != HAL_OK)
	{
//		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1000;		//switch off backlight - 0% backlight brightness
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if (HAL_TIM_PWM_ConfigChannel(&htim13, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
//		_Error_Handler(__FILE__, __LINE__);
	}

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_TIM13;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1) != HAL_OK) {
//		_Error_Handler(__FILE__, __LINE__);
	}

//	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_12, GPIO_PIN_SET); /* Assert LCD_DISP pin */

    /* Assert LCD_DISP and LCD_BL_CTRL pins */
    HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

    return 0;
}

namespace touchgfx
{
void hw_init()
{
    HAL_Init();

    __HAL_RCC_CRC_CLK_ENABLE();

    SystemClock_Config();

    /* Initialize the QSPI */
    BSP_QSPI_Init();
    BSP_QSPI_MemoryMappedMode();
    HAL_NVIC_DisableIRQ(QUADSPI_IRQn);

#ifndef SINGLE_FRAME_BUFFER_INTERNAL
    BSP_SDRAM_Init();
#endif

    HAL_MPU_Disable();

    /* Configure unused area of QSPI region as strongly ordered.
     * This is *important* to avoid unintentional fetches from illegal
     * addresses due to cache/speculation which would halt the MCU.
     */
    MPU_Region_InitTypeDef MPU_InitStruct;
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x90000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER2;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as WT for QSPI (used 16Mbytes) */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x90000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER3;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Enable D-cache on SDRAM (Write-through) */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0xC0000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Enable the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

    /* Initialize display */
    LCD_Init();

    GPIO::init();

    /* Enable L1 Cache */
    SCB_EnableDCache();
    SCB_EnableICache();

    //Deactivate speculative/cache access to first FMC Bank to save FMC bandwidth
    FMC_Bank1->BTCR[0] = 0x000030D2;
}

STM32F7DMA dma;
STM32F746GTouchController tc;
STM32F7Instrumentation mcuInstr;

#if !defined(USE_BPP) || USE_BPP==16
static LCD16bpp display;
#elif USE_BPP==24
static LCD24bpp display;
#else
#error Unknown USE_BPP
#endif

void touchgfx_init()
{
    uint16_t dispWidth = RK043FN48H_WIDTH;
    uint16_t dispHeight = RK043FN48H_HEIGHT;
#if !defined(USE_BPP) || USE_BPP==16
    HAL& hal = touchgfx_generic_init<STM32F7HAL>(dma, display, tc, dispWidth, dispHeight,
                                                 (uint16_t*)(frameBuf0 + (dispWidth * dispHeight * 2) * 3),
                                                 1024 * 1024, 16);
#ifdef SINGLE_FRAME_BUFFER_INTERNAL
    //setup for single buffering
    hal.setFrameBufferStartAddress((uint16_t*)frameBuf0, 16, false, false);

    // The optimized strategy for single buffering requires the presence of a
    // task delay function.
    hal.registerTaskDelayFunction(&OSWrappers::taskDelay);

    // Enable strategy.
    hal.setFrameRefreshStrategy(HAL::REFRESH_STRATEGY_OPTIM_SINGLE_BUFFER_TFT_CTRL);
#else
    //setup for double buffering.
    hal.setFrameBufferStartAddress((uint16_t*)frameBuf0);
#endif
#elif USE_BPP==24
    HAL& hal = touchgfx_generic_init<STM32F7HAL>(dma, display, tc, dispWidth, dispHeight,
                                                 (uint16_t*)(frameBuf0 + (dispWidth * dispHeight * 3) * 3),
                                                 1024 * 1024, 16);
#ifdef  SINGLE_FRAME_BUFFER_INTERNAL
#error Single frame buffer in internal is only possible in 16bpp due to memory constraints.
#endif
    hal.setFrameBufferStartAddress((uint16_t*)frameBuf0, 24);
#else
#error Unknown USE_BPP
#endif

    hal.setTouchSampleRate(2);
    hal.setFingerSize(1);

    // By default frame rate compensation is off.
    // Enable frame rate compensation to smooth out animations in case there is periodic slow frame rates.
    hal.setFrameRateCompensation(false);

    // This platform can handle simultaneous DMA and TFT accesses to SDRAM, so disable lock to increase performance.
    hal.lockDMAToFrontPorch(false);

    mcuInstr.init();

    //Set MCU instrumentation and Load calculation
    hal.setMCUInstrumentation(&mcuInstr);
    hal.enableMCULoadCalculation(true);
}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_StatusTypeDef ret = HAL_OK;

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;

    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }

    /* Activate the OverDrive to reach the 200 MHz Frequency */
    ret = HAL_PWREx_EnableOverDrive();
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }
}
