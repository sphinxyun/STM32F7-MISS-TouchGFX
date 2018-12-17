#include "stm32f7xx_it.h"

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_dma2d.h"
#include "stm32f7xx_hal_ltdc.h"

#include "debug_uart.h"

#include <stdbool.h>

#include <cmsis_os.h>

extern SAI_HandleTypeDef             haudio_out_sai;
extern SAI_HandleTypeDef             haudio_in_sai;

extern DMA2D_HandleTypeDef 			 hdma2d;
extern LTDC_HandleTypeDef 			 hltdc;

static volatile int overrunCnt;

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
/* These are volatile to try and prevent the compiler/linker optimising them
away as the variables never actually get used.  If the debugger won't show the
values of the variables, make them global my moving their declaration outside
of this function. */
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

	(void)r0;
	(void)r1;
	(void)r2;
	(void)r3;
	(void)r12;
	(void)lr;
	(void)pc;
	(void)psr;

    /* When the following line is hit, the variables contain the register values. */
    for( ;; );
}

void HardFault_Handler(void) __attribute__( ( naked ) );

void HardFault_Handler(void)
{
	__asm volatile
	    (
	        " tst lr, #4                                                \n"
	        " ite eq                                                    \n"
	        " mrseq r0, msp                                             \n"
	        " mrsne r0, psp                                             \n"
	        " ldr r1, [r0, #24]                                         \n"
	        " ldr r2, handler2_address_const                            \n"
	        " bx r2                                                     \n"
	        " handler2_address_const: .word prvGetRegistersFromStack    \n"
	    );

  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  SysTick interrupt handler
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	HAL_SYSTICK_IRQHandler();
	osSystickHandler();
}

/**
  * @brief  DMA2D interrupt handler
  * @param  None
  * @retval None
  */
void DMA2D_IRQHandler(void)
{
  HAL_DMA2D_IRQHandler(&hdma2d);
}

/**
  * @brief  LTDC interrupt handler
  * @param  None
  * @retval None
  */
void LTDC_IRQHandler(void)
{
  HAL_LTDC_IRQHandler(&hltdc);
}

/**
  * @brief  LTDC_ER interrupt handler
  * @param  None
  * @retval None
  */
void LTDC_ER_IRQHandler(void)
{
  if (LTDC->ISR & 2)
  {
    LTDC->ICR = 2;
    overrunCnt++;
  }
}

///**
//  * @brief  This function handles External lines 9_5 interrupt request.
//  * @param  None
//  * @retval None
//  */
//void EXTI9_5_IRQHandler(void)
//{
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
//}

/**
  * @brief  This function handles DMA2 Stream 5 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream4_IRQHandler(void) {
	HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

/**
  * @brief This function handles DMA2 Stream 7 interrupt request.
  * @param None
  * @retval None
  */
void AUDIO_IN_SAIx_DMAx_IRQHandler(void) {
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}
