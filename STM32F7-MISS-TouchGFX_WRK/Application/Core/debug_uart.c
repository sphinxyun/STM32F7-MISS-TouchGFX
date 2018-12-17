#include "debug_uart.h"
#include "stm32f7xx_hal.h"

#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_bus.h"

#include <string.h>

#ifndef DEBUG_UART_TX_BUFFER_SIZE
#define DEBUG_UART_TX_BUFFER_SIZE 4192
#endif

__IO ITStatus DEBUG_UART_TxFinishedFlag  = SET;
uint8_t u8DebugTxBuffer[DEBUG_UART_TX_BUFFER_SIZE];
volatile uint16_t u16DebugTxOut;
uint16_t u16DebugTxIn;

LL_USART_InitTypeDef USART1_InitStruct;
LL_DMA_InitTypeDef DMA_USART1_TX_InitStruct;

#define UART_1_DMA_TX_BUFFER_SIZE		( 2048 )
volatile uint8_t UART1_DMA_TX_Buffer[UART_1_DMA_TX_BUFFER_SIZE];

void DEBUG_UART_Init(void) {
	/* Peripheral clock enable */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	/** USART1 GPIO Configuration
	 PA9     ------> USART1_TX
	 PB7     ------> USART1_RX
	 */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, GPIO_AF7_USART1);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_VERY_HIGH);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, GPIO_AF7_USART1);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_VERY_HIGH);

	/* Configure USART */
	LL_USART_StructInit(&USART1_InitStruct);
	USART1_InitStruct.BaudRate = 230400;
	USART1_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART1_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART1_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	USART1_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART1_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART1_InitStruct.TransferDirection = LL_USART_DIRECTION_TX;
	LL_USART_Init(USART1, &USART1_InitStruct);

	DEBUG_UART_ResetComm();

	LL_USART_Enable(USART1);
	LL_USART_EnableDMAReq_TX(USART1);
}

void DEBUG_UART_ResetComm(void) {
	// Initialise variables
	u16DebugTxIn = 0;
	u16DebugTxOut = 0;
	DEBUG_UART_TxFinishedFlag = SET;
}

void DMA2_Stream7_IRQHandler(void) {
	if (LL_DMA_IsActiveFlag_TC7(DMA2)) {
	    LL_DMA_ClearFlag_TC7(DMA2);

		LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_7);
		DEBUG_UART_TxFinishedFlag = SET;
	}
}

void DEBUG_UART_SysTick(void) {
	static uint16_t i16Cnt = 0;

	if (++i16Cnt >= 10) {
		i16Cnt = 0;

		if (DEBUG_UART_TxFinishedFlag == SET) {
			if (!DEBUG_UART_IsTxBufferEmpty()) {
				DEBUG_UART_TxFinishedFlag = RESET;

				uint16_t u16DataCnt = 0;

				for (u16DataCnt = 0; u16DataCnt < UART_1_DMA_TX_BUFFER_SIZE; u16DataCnt++) {
					if (u16DebugTxOut == u16DebugTxIn)
						break;

					UART1_DMA_TX_Buffer[u16DataCnt] = u8DebugTxBuffer[u16DebugTxOut];

					// Advance pointer
					GET_NEXT_BUFF_U16_IDX(u16DebugTxOut, DEBUG_UART_TX_BUFFER_SIZE);
				}

				/* Configure DMA for USART TX */
				LL_DMA_StructInit(&DMA_USART1_TX_InitStruct);
				DMA_USART1_TX_InitStruct.Channel = DMA_CHANNEL_4;
				DMA_USART1_TX_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
				DMA_USART1_TX_InitStruct.Mode = LL_DMA_MODE_NORMAL;
				DMA_USART1_TX_InitStruct.Priority = LL_DMA_PRIORITY_LOW;
				DMA_USART1_TX_InitStruct.MemoryOrM2MDstAddress = (uint32_t) UART1_DMA_TX_Buffer;
				DMA_USART1_TX_InitStruct.NbData = u16DataCnt;
				DMA_USART1_TX_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
				DMA_USART1_TX_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)&USART1->TDR;
				LL_DMA_Init(DMA2, LL_DMA_STREAM_7, &DMA_USART1_TX_InitStruct);

				LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_7);
				//	LL_DMA_EnableIT_HT(DMA2, LL_DMA_STREAM_1);
				LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_7);

				/* Enable global DMA stream interrupts */
				NVIC_SetPriority(DMA2_Stream7_IRQn, 4);
				NVIC_EnableIRQ(DMA2_Stream7_IRQn);
			}
		}
	}
}

bool DEBUG_UART_SendByte(uint8_t u8Data) {
	uint16_t index = u16DebugTxIn;

	// Calculate next pointer position
	GET_NEXT_BUFF_U16_IDX(index, DEBUG_UART_TX_BUFFER_SIZE);

	// Make sure there is space available in buffer
	if (index == u16DebugTxOut) {
		return false;
	}

	// Insert data into buffer
	u8DebugTxBuffer[u16DebugTxIn] = u8Data;

	// Advance pointer
	u16DebugTxIn = index;

	return true;
}

bool DEBUG_UART_SendData(uint8_t *pu8Data, uint16_t u16Length, uint16_t *pu16SendLength) {
	*pu16SendLength = 0;

	while (u16Length) {
		if (!DEBUG_UART_SendByte(*pu8Data++))
			return false;

		*pu16SendLength = *pu16SendLength + 1;
		u16Length--;
	}

	return true;
}

void DEBUG_UART_FlushTxBuffer(void) {
	u16DebugTxOut = u16DebugTxIn = 0;
}

uint16_t DEBUG_UART_NoOfWaitingTxBytes(void) {
	if (u16DebugTxIn >= u16DebugTxOut) {
		return (u16DebugTxIn - u16DebugTxOut);
	} else {
		return ((DEBUG_UART_TX_BUFFER_SIZE - u16DebugTxOut) + u16DebugTxIn);
	}
}

uint16_t DEBUG_UART_NoOfFreeTxBytes(void) {
	return DEBUG_UART_TX_BUFFER_SIZE - DEBUG_UART_NoOfWaitingTxBytes() - 1;
}

bool DEBUG_UART_IsTxBufferFull(void) {
	uint16_t index = u16DebugTxIn;

	// Calculate next pointer position
	GET_NEXT_BUFF_U16_IDX(index, DEBUG_UART_TX_BUFFER_SIZE);

	if (index == u16DebugTxOut) {
		return true;
	} else {
		return false;
	}
}

bool DEBUG_UART_IsTxBufferEmpty(void) {
	if (u16DebugTxOut == u16DebugTxIn) {
		return true;
	} else {
		return false;
	}
}

bool DEBUG_UART_TxFinished(void) {
	if (u16DebugTxOut == u16DebugTxIn) {
		return false;
	}

	return DEBUG_UART_TxFinishedFlag;
}
