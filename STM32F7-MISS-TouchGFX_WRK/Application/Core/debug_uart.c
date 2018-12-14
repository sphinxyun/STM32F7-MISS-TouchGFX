#include "debug_uart.h"
#include "stm32f7xx_hal.h"

#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_bus.h"

#include <string.h>



//UART_HandleTypeDef husart1;
//DMA_HandleTypeDef hdma_usart1_tx;
//DMA_HandleTypeDef hdma_usart1_rx;

#ifndef DEBUG_UART_TX_BUFFER_SIZE
#define DEBUG_UART_TX_BUFFER_SIZE 4192
#endif

//#ifndef DEBUG_UART_RX_BUFFER_SIZE
//#define DEBUG_UART_RX_BUFFER_SIZE 128
//#endif

__IO ITStatus DEBUG_UART_TxFinishedFlag  = SET;
uint8_t u8DebugTxBuffer[DEBUG_UART_TX_BUFFER_SIZE];
volatile uint16_t u16DebugTxOut;
uint16_t u16DebugTxIn;

LL_USART_InitTypeDef USART1_InitStruct;
LL_DMA_InitTypeDef DMA_USART1_TX_InitStruct;

#define UART_1_DMA_TX_BUFFER_SIZE		( 2048 )
volatile uint8_t UART1_DMA_TX_Buffer[UART_1_DMA_TX_BUFFER_SIZE];

//uint8_t u8DebugRxBuffer[DEBUG_UART_RX_BUFFER_SIZE];
//uint16_t u16DebugRxOut;
//volatile uint16_t u16DebugRxIn;

//#define RX_DMA_SIZE		512
//uint8_t dma_uart_tx_buffer[2000];
//uint8_t dma_uart_rx_buffer[RX_DMA_SIZE];

void DEBUG_UART_RxBuffOvf(void);
void DEBUG_UART_TxBuffOvf(void);

void DEBUG_UART_TxCpltCallback(UART_HandleTypeDef *husart1);
void DEBUG_UART_ReceiverIdleCallback(UART_HandleTypeDef *husart1);
void DEBUG_UART_RxCpltCallback(UART_HandleTypeDef *husart1);

void DEBUG_UART_Init(void) {
	DEBUG_UART_ResetComm();

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
	USART1_InitStruct.TransferDirection = LL_USART_DIRECTION_TX; //_RX;
	LL_USART_Init(USART1, &USART1_InitStruct);

	LL_USART_Enable(USART1);
//	LL_USART_EnableDMAReq_RX(USART1);
	LL_USART_EnableDMAReq_TX(USART1);

	/* Configure DMA for USART TX */
//	LL_DMA_StructInit(&DMA_USART1_TX_InitStruct);
//	DMA_USART1_TX_InitStruct.Channel = DMA_CHANNEL_4;
//	DMA_USART1_TX_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
//	DMA_USART1_TX_InitStruct.Mode = LL_DMA_MODE_NORMAL;
//	DMA_USART1_TX_InitStruct.Priority = LL_DMA_PRIORITY_LOW;
//	DMA_USART1_TX_InitStruct.MemoryOrM2MDstAddress = (uint32_t) UART1_DMA_TX_Buffer;
//	DMA_USART1_TX_InitStruct.NbData = UART_1_DMA_TX_BUFFER_SIZE;
//	DMA_USART1_TX_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
//	DMA_USART1_TX_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)&USART1->TDR;
//	LL_DMA_Init(DMA2, LL_DMA_STREAM_7, &DMA_USART1_TX_InitStruct);

//	/* USART1 DMA Init */
//	/* USART1_TX Init */
//	hdma_usart1_tx.Instance = DMA2_Stream7;
//	hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
//	hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//	hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//	hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
//	hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//	hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//	hdma_usart1_tx.Init.Mode = DMA_NORMAL;
//	hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
//	hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//	if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK) {
//		_Error_Handler(__FILE__, __LINE__);
//	}

//	/* USART1_RX Init */
//    hdma_usart1_rx.Instance = DMA2_Stream2;
//    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
//	hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//	hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
//	hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
//	hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//	hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//	hdma_usart1_rx.Init.Mode = DMA_NORMAL;
//	hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
//    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//
//	if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK) {
//		_Error_Handler(__FILE__, __LINE__);
//	}

//	LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_7);
//	//	LL_DMA_EnableIT_HT(DMA2, LL_DMA_STREAM_1);
//	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_7);
//
//	/* Enable global DMA stream interrupts */
//	NVIC_SetPriority(DMA2_Stream7_IRQn, 1);
//	NVIC_EnableIRQ(DMA2_Stream7_IRQn);

//	/* DMA interrupt init */
//	/* DMA2_Stream2_IRQn interrupt configuration */
//	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
//
//	/* DMA2_Stream7_IRQn interrupt configuration */
//	HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

//	husart1.Instance = USART1;
//	husart1.TransmitCompleteCallback = DEBUG_UART_TxCpltCallback;
//	husart1.ReceiveCompleteCallback = DEBUG_UART_RxCpltCallback;
//	husart1.ReceiverIdleCallback = DEBUG_UART_ReceiverIdleCallback;
//	husart1.Init.BaudRate = 230400;
//	husart1.Init.WordLength = UART_WORDLENGTH_8B;
//	husart1.Init.StopBits = UART_STOPBITS_1;
//	husart1.Init.Parity = UART_PARITY_NONE;
//	husart1.Init.Mode = UART_MODE_TX_RX;
//	husart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//	husart1.Init.OverSampling = UART_OVERSAMPLING_16;
//	husart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//	husart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//	if (HAL_UART_Init(&husart1) != HAL_OK) {
//		_Error_Handler(__FILE__, __LINE__);
//	}

//	__HAL_LINKDMA(&husart1, hdmatx, hdma_usart1_tx);
//	__HAL_LINKDMA(&husart1, hdmarx, hdma_usart1_rx);

	/* USART1 interrupt Init, to catch the TX complete  */
//	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
//	HAL_NVIC_EnableIRQ(USART1_IRQn);

//	if (HAL_UART_Receive_DMA(&husart1, (uint8_t *)dma_uart_rx_buffer, RX_DMA_SIZE) != HAL_OK) {
//		_Error_Handler(__FILE__, __LINE__);
//	}
}

void DEBUG_UART_ResetComm(void) {
	// Initialise variables
	u16DebugTxIn = 0;
	u16DebugTxOut = 0;
	DEBUG_UART_TxFinishedFlag = SET;

//	u16DebugRxIn = 0;
//	u16DebugRxOut = 0;
}

//void USART1_IRQHandler(void) {
//	HAL_UART_IRQHandler(&husart1);
//}

//void DMA2_Stream2_IRQHandler(void) {
//	HAL_DMA_IRQHandler(husart1.hdmarx);
//}

void DMA2_Stream7_IRQHandler(void) {
//	HAL_DMA_IRQHandler(husart1.hdmatx);

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
				NVIC_SetPriority(DMA2_Stream7_IRQn, 1);
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
//		DEBUG_UART_TxBuffOvf();
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

//bool DEBUG_UART_GetByte(uint8_t* pu8Data) {
//	// See if there is data in the buffer
//	if (u16DebugRxOut == u16DebugRxIn) {
//		return false;
//	}
//	// Fetch data
//	*pu8Data = u8DebugRxBuffer[u16DebugRxOut];
//
//	// Advance pointer
//	GET_NEXT_BUFF_U16_IDX(u16DebugRxOut, DEBUG_UART_RX_BUFFER_SIZE);
//
//	return true;
//}
//
//bool DEBUG_UART_GetData(uint8_t* pu8Data, uint16_t u16MaxLength, uint16_t *pu16Length) {
//	*pu16Length = 0;
//
//	while (u16MaxLength) {
//		// See if there is data in the buffer
//		if (u16DebugRxOut == u16DebugRxIn) {
//			return false;
//		}
//
//		// Fetch data
//		*pu8Data++ = u8DebugRxBuffer[u16DebugRxOut];
//		// Advance pointer
//		GET_NEXT_BUFF_U16_IDX(u16DebugRxOut, DEBUG_UART_RX_BUFFER_SIZE);
//
//		// Next byte
//		*pu16Length = *pu16Length + 1;
//		u16MaxLength--;
//	}
//
//	return true;
//}

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

//void DEBUG_UART_FlushRxBuffer(void) {
//	u16DebugRxOut = u16DebugRxIn = 0;
//}
//
//uint16_t DEBUG_UART_NoOfWaitingRxBytes(void) {
//	if (u16DebugRxIn >= u16DebugRxOut) {
//		return (u16DebugRxIn - u16DebugRxOut);
//	} else {
//		return ((DEBUG_UART_RX_BUFFER_SIZE - u16DebugRxOut) + u16DebugRxIn);
//	}
//}
//
//uint16_t DEBUG_UART_NoOfFreeRxBytes(void) {
//	return DEBUG_UART_RX_BUFFER_SIZE - DEBUG_UART_NoOfWaitingRxBytes() - 1;
//}
//
//bool DEBUG_UART_IsRxBufferFull(void) {
//	uint16_t index = u16DebugRxIn;
//
//	// Calculate next pointer position
//	GET_NEXT_BUFF_U16_IDX(index, DEBUG_UART_RX_BUFFER_SIZE);
//
//	if (index == u16DebugRxOut) {
//		return true;
//	} else {
//		return false;
//	}
//}
//
//bool DEBUG_UART_IsRxBufferEmpty(void) {
//	if (u16DebugRxOut == u16DebugRxIn) {
//		return true;
//	} else {
//		return false;
//	}
//}

//void DEBUG_UART_TxCpltCallback(UART_HandleTypeDef *husart1) {
//	DEBUG_UART_TxFinishedFlag = SET;
//}

//void DEBUG_UART_ReceiverIdleCallback(UART_HandleTypeDef *husart1) {
//	uint16_t rxXferCount = 0;
//
//	if (husart1->hdmarx != NULL) {
//		DMA_HandleTypeDef *hdma = husart1->hdmarx;
//
//		/* Determine how many items of data have been received */
//		rxXferCount = husart1->RxXferSize - __HAL_DMA_GET_COUNTER(hdma);
//
//		HAL_DMA_Abort(husart1->hdmarx);
//
//		/* Check if a transmit process is ongoing or not */
//		if (husart1->gState == HAL_UART_STATE_BUSY_TX_RX) {
//			husart1->gState = HAL_UART_STATE_BUSY_TX;
//		} else {
//			husart1->gState = HAL_UART_STATE_READY;
//		}
//
//		husart1->RxState = HAL_UART_STATE_READY;
//
//		if (HAL_UART_Receive_DMA(husart1, (uint8_t *)dma_uart_rx_buffer, RX_DMA_SIZE) != HAL_OK) {
//			_Error_Handler(__FILE__, __LINE__);
//		}
//	}
//
//	uint16_t index = u16DebugRxIn;
//
//	for (int16_t i = 0; i < rxXferCount; i++) {
//		// Calculate next pointer position
//		GET_NEXT_BUFF_U16_IDX(index, DEBUG_UART_RX_BUFFER_SIZE);
//
//		// Make sure there is space available in buffer.
//		if (index == u16DebugRxOut) {
//			DEBUG_UART_RxBuffOvf();
//
//			return;
//		}
//
//		u8DebugRxBuffer[u16DebugRxIn] = dma_uart_rx_buffer[i];
//
//		u16DebugRxIn = index;
//	}
//}
//
//void DEBUG_UART_RxCpltCallback(UART_HandleTypeDef *husart1) {
//	uint16_t index = u16DebugRxIn;
//
//	for (int16_t i = 0; i < RX_DMA_SIZE; i++) {
//		// Calculate next pointer position
//		GET_NEXT_BUFF_U16_IDX(index, DEBUG_UART_RX_BUFFER_SIZE);
//
//		// Make sure there is space available in buffer.
//		if (index == u16DebugRxOut) {
//			DEBUG_UART_RxBuffOvf();
//
//			return;
//		}
//
//		u8DebugRxBuffer[u16DebugRxIn] = dma_uart_rx_buffer[i];
//
//		u16DebugRxIn = index;
//	}
//}
