#include "pressure_sensor_thread.h"
#include "stm32f7xx_hal.h"

#include <string.h>

#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_bus.h"

#define DEBUG_PRESSURE_SENSOR_THREAD_LL		1
#define DEBUG_PRESSURE_SENSOR_THREAD_HL		1

static TaskHandle_t PressureSensorHandlerThreadId = 0;

#define POOL_MAX_STRUCTS		5

sCarmenDataPool_t sensorDataPool[ POOL_MAX_STRUCTS ];

QueueHandle_t xIrrigationPressureData = 0;

LL_USART_InitTypeDef USART6_InitStruct;
LL_DMA_InitTypeDef DMA_USART6_RX_InitStruct;

#define DMA_RX_BUFFER_DIV		3
#define DMA_RX_BUFFER_CHK		8
#define DMA_RX_BUFFER_SIZE		(DMA_RX_BUFFER_CHK * 13)
volatile uint8_t UART6_DMA_RX_Buffer_A[DMA_RX_BUFFER_SIZE];
volatile uint8_t UART6_DMA_RX_Buffer_B[DMA_RX_BUFFER_SIZE];

static inline uint16_t CalculateCarmenCRC16(uint16_t crc, const void *c_ptr, size_t len);

static void PressureAnalysis_Thread(void * argument);

void DMA2_Stream1_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TC1(DMA2)) {
    	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
    	DEBUG_SendTextFrame("\n");
//    	DEBUG_SendTextFrame("UART6_DMA_TC   HISR: %x | LISR: %x", DMA2->HISR, DMA2->LISR);
#endif

    	if (LL_DMA_GetCurrentTargetMem(DMA2, LL_DMA_STREAM_1) == LL_DMA_CURRENTTARGETMEM0) {

#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
        	DEBUG_SendTextFrame("      BUFFERS: fill: %x | proc: %x", UART6_DMA_RX_Buffer_A, UART6_DMA_RX_Buffer_B);
#endif

    		// current target buffer A (read buffer B)
    		if (! xTaskNotifyFromISR( PressureSensorHandlerThreadId, (uint32_t)UART6_DMA_RX_Buffer_B, eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken )) {
#if (DEBUG_PRESSURE_SENSOR_THREAD_HL == 1)
    			DEBUG_SendTextFrame("xTaskNotifyFromISR failed!");
#endif
    		}


#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
//    		if (xHigherPriorityTaskWoken)
//    			DEBUG_SendTextFrame("xHigherPriorityTaskWoken");
#endif
    	} else {

#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
        	DEBUG_SendTextFrame("      BUFFERS: fill: %x | proc: %x", UART6_DMA_RX_Buffer_B, UART6_DMA_RX_Buffer_A);
#endif

    		// current target buffer B (read buffer A)
    		if (! xTaskNotifyFromISR( PressureSensorHandlerThreadId, (uint32_t)UART6_DMA_RX_Buffer_A, eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken )) {
#if (DEBUG_PRESSURE_SENSOR_THREAD_HL == 1)
    			DEBUG_SendTextFrame("xTaskNotifyFromISR failed!");
#endif
    		}

#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
//    		if (xHigherPriorityTaskWoken)
//    		    DEBUG_SendTextFrame("xHigherPriorityTaskWoken");
#endif
    	}

    	LL_DMA_ClearFlag_TC1(DMA2);

    	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    } else {
#if (DEBUG_PRESSURE_SENSOR_THREAD_HL == 1)
    	DEBUG_SendTextFrame("UART6_DMA_??? - ERR");
#endif
    }
}

void CARMEN_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	//RESET
	__HAL_RCC_GPIOI_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_RESET); //RESET

	//nCS
	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET); //nCS

	/* Enable all clocks */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6);

	/**USART6 GPIO Configuration
	 PC6     ------> USART6_TX
	 PC7     ------> USART6_RX
	 */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_6, GPIO_AF8_USART6);
	LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);
	LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_VERY_HIGH);

	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_7, GPIO_AF8_USART6);
	LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);
	LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_VERY_HIGH);

	/* Configure USART */
	LL_USART_StructInit(&USART6_InitStruct);
	USART6_InitStruct.BaudRate = 115200;
	USART6_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART6_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART6_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	USART6_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART6_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART6_InitStruct.TransferDirection = LL_USART_DIRECTION_RX;
	LL_USART_Init(USART6, &USART6_InitStruct);

	/* Enable USART and enable interrupt for IDLE line detection */
	LL_USART_Enable(USART6);
	LL_USART_EnableDMAReq_RX(USART6);
//	LL_USART_EnableIT_ERROR(USART6);
//	LL_USART_EnableIT_IDLE(USART6);

	/* Enable USART global interrupts */
//	NVIC_SetPriority(USART6_IRQn, 1);
//	NVIC_EnableIRQ(USART6_IRQn);

	/* Configure DMA for USART RX */
	LL_DMA_StructInit(&DMA_USART6_RX_InitStruct);
	DMA_USART6_RX_InitStruct.Channel = LL_DMA_CHANNEL_5;
	DMA_USART6_RX_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
	DMA_USART6_RX_InitStruct.Mode = LL_DMA_MODE_CIRCULAR;
	DMA_USART6_RX_InitStruct.Priority = LL_DMA_PRIORITY_VERYHIGH;
	DMA_USART6_RX_InitStruct.MemoryOrM2MDstAddress = (uint32_t) UART6_DMA_RX_Buffer_A;
	DMA_USART6_RX_InitStruct.NbData = DMA_RX_BUFFER_SIZE;
	DMA_USART6_RX_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	DMA_USART6_RX_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)&USART6->RDR;
	LL_DMA_Init(DMA2, LL_DMA_STREAM_1, &DMA_USART6_RX_InitStruct);

	LL_DMA_SetMemory1Address(DMA2, LL_DMA_STREAM_1, (uint32_t) UART6_DMA_RX_Buffer_B);
	LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_1, LL_DMA_CURRENTTARGETMEM0);
	LL_DMA_EnableDoubleBufferMode(DMA2, LL_DMA_STREAM_1);

	LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_1);
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_1);

	/* Enable global DMA stream interrupts */
	NVIC_SetPriority(DMA2_Stream1_IRQn, 4);
	NVIC_EnableIRQ(DMA2_Stream1_IRQn);
}

PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Start(void) {
#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
	DEBUG_SendTextFrame("PRESSURE_SENSOR_Start - A");
#endif

	//put sensor in RESET with nCS HIGH
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_SET); //RESET

	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET); //nCS

	if (PressureSensorHandlerThreadId != 0) {
		vTaskResume(PressureSensorHandlerThreadId);
	}

#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
	DEBUG_SendTextFrame("PRESSURE_SENSOR_Start - B");
#endif

	return PRESSURE_SENSOR_ERROR_NONE;
}

PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Stop(void) {
#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
	DEBUG_SendTextFrame("PRESSURE_SENSOR_Stop - A");
#endif

	//put sensor in RESET with nCS HIGH
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_RESET); //RESET
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET); //nCS

	if (PressureSensorHandlerThreadId != 0) {
		vTaskSuspend(PressureSensorHandlerThreadId);
	}

#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
	DEBUG_SendTextFrame("PRESSURE_SENSOR_Stop - B");
#endif

	return PRESSURE_SENSOR_ERROR_NONE;
}

sCarmenDataPool_t *get_sensor_struct(void) {
	// Find the next free struct
	for (int i = 0; i < POOL_MAX_STRUCTS; i++) {
		if (sensorDataPool[i].inUse == false) {
			// Mark struct in use and return it
			sensorDataPool[i].inUse = true;
			return &sensorDataPool[i];
		}
	}

	// All in use
	return NULL;
}

void free_sensor_struct(sCarmenDataPool_t *toRelease) {
	// Find the struct being freed
	for (int i = 0; i < POOL_MAX_STRUCTS; i++) {
		if (toRelease == &sensorDataPool[i]) {
			// Mark struct not in use
			sensorDataPool[i].inUse = false;
			return;
		}
	}
}

PRESSURE_SENSOR_ErrorTypdef PRESSURE_SENSOR_Init(void) {
#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
	DEBUG_SendTextFrame("PRESSURE_SENSOR_Init");
#endif

	for (int i = 0; i < POOL_MAX_STRUCTS; i++) {
		sensorDataPool[i].inUse = false;
	}

	xIrrigationPressureData = xQueueCreate( 2, sizeof( sCarmenDataPool_t* ) );

	CARMEN_Init();

	/* Create actual task */
    xTaskCreate(PressureAnalysis_Thread, "PressureTask",
                1024,
                NULL,
				tskIDLE_PRIORITY + 5,
                &PressureSensorHandlerThreadId);

	PRESSURE_SENSOR_Stop();

	return PRESSURE_SENSOR_ERROR_NONE;
}

static void PressureAnalysis_Thread(void * pvParameters ) {
	( void ) pvParameters;

	sCarmenDataPool_t *sData = get_sensor_struct();

	uint32_t u32ValidFrames = 0;
	uint32_t u32ErrorsSOF = 0;
	uint32_t u32ErrorsCRC = 0;
	uint32_t u32ErrorsUNK = 0;

	uint32_t ulNotifiedValue = 0;

	uint8_t u8CarmenData[13];
	uint8_t u8CarmenLen = 0;

	for (;;) {
		if (xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &ulNotifiedValue, 1000)) {
#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
			DEBUG_SendTextFrame("Pressure - START PROCESSING (%x)", ulNotifiedValue);
#endif

			uint8_t *pRxBuffer = (uint8_t *)ulNotifiedValue;

			uint32_t u32BufferFrmeCnt = 0;
			int32_t i32BufferPressSum = 0;
			int32_t i32BufferTempSum = 0;
			uCarmentStatusReg_t uStatusSum;
			for (int i = 0; i < 3; i++)
				uStatusSum.u8Stat[i] = 0;

			size_t idx = 0;
#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
			while (idx < DMA_RX_BUFFER_SIZE) {
				if (idx % 13 == 0) {
					if (idx / 13 <= 9)
						DEBUG_SendTextFrame("    %d: %t", idx / 13, &pRxBuffer[idx], 13);
					else
						DEBUG_SendTextFrame( "   %d: %t", idx / 13, &pRxBuffer[idx], 13);
				}
				idx++;
			}

			idx = 0;
#endif

			while (idx < DMA_RX_BUFFER_SIZE) {
				if ((idx == 0) && (u8CarmenLen != 0)) {
					memcpy(&u8CarmenData[u8CarmenLen], &pRxBuffer[idx], 13 - u8CarmenLen);
//					idx += (13 - u8CarmenLen);
//					u8CarmenLen = 0;
					pRxBuffer = u8CarmenData;
				}

				if (pRxBuffer[idx] == 0x35) {
					//check if this is a whole frame in this buffer or if it is divided
					//between two buffers (this and next)...
					if (idx + 13 > DMA_RX_BUFFER_SIZE) {
#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
						DEBUG_SendTextFrame("CARMEN FRAME - PARTIAL, only %d bytes in this buffer...", DMA_RX_BUFFER_SIZE - idx);
						//copy data from this buffer - will be joined with remaining bytes in the beginning of next buffer
						memcpy(u8CarmenData, &pRxBuffer[idx], DMA_RX_BUFFER_SIZE - idx);
						u8CarmenLen = DMA_RX_BUFFER_SIZE - idx;
						idx = DMA_RX_BUFFER_SIZE;
#endif
					} else {
						uint16_t *pu16RxedCrc = (uint16_t *)&pRxBuffer[idx + 11];
						uint16_t u16CompCRC = CalculateCarmenCRC16(0xFFFF, &pRxBuffer[idx], 11);

						if (u16CompCRC == *pu16RxedCrc) {
#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
							DEBUG_SendTextFrame("CARMEN FRAME CRC OK!!!");
#endif

							//parse rxed frame:
							int32_t i32Digout_1 = (((uint32_t)pRxBuffer[idx + 3]) << 16) | (((uint32_t)pRxBuffer[idx + 2]) << 8) | (((uint32_t)pRxBuffer[idx + 1]) << 0);
							if (i32Digout_1 & 0x00800000) i32Digout_1 |= 0xFF000000;
							i32BufferPressSum += i32Digout_1;

							int16_t i16Digout_2 = (((uint32_t)pRxBuffer[idx + 5]) << 8) | (((uint32_t)pRxBuffer[idx + 4]) << 0);
							i32BufferTempSum += i16Digout_2;

							//int16_t i16Digout_3 = (((uint32_t)pRxBuffer[idx + 7]) << 8) | (((uint32_t)pRxBuffer[idx + 6]) << 0);

							uCarmentStatusReg_t uStatus;
							memcpy(&uStatus.u8Stat[0], &pRxBuffer[idx + 8], 3);
							for (int i = 0; i < 3; i++)
								uStatusSum.u8Stat[i] |= uStatus.u8Stat[i];

							u32BufferFrmeCnt++;
							u32ValidFrames++;
						} else {
							u32ErrorsCRC++;

#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
							DEBUG_SendTextFrame("CARMEN FRAME CRC ERROR ( idx = %d ):", idx);
							DEBUG_SendTextFrame("  buffer_A: %s", (pRxBuffer == UART6_DMA_RX_Buffer_A) ? "T" : "F");
							DEBUG_SendTextFrame("  buffer_B: %s", (pRxBuffer == UART6_DMA_RX_Buffer_B) ? "T" : "F");

							DEBUG_SendTextFrame("  cmd:      %x", pRxBuffer[idx]);
							DEBUG_SendTextFrame("  payload:  %t", &pRxBuffer[idx+1], 10);
							uint16_t *pu16RxedCrc = (uint16_t *)&pRxBuffer[idx+11];
							DEBUG_SendTextFrame("  crc rxed: %x", *pu16RxedCrc);
							DEBUG_SendTextFrame("  crc comp: %x", u16CompCRC);
#endif
						}

						if ((idx == 0) && (u8CarmenLen != 0)) {
							idx += (13 - u8CarmenLen);
							u8CarmenLen = 0;
						} else {
							idx += 13;
						}

						pRxBuffer = (uint8_t *)ulNotifiedValue;
					}
				} else {
					u32ErrorsSOF++;

					idx++;

//#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
//					DEBUG_SendTextFrame("CARMEN FRAME SOF ERROR!!!");
//#endif
				}
			}

			if (u32BufferFrmeCnt == DMA_RX_BUFFER_CHK) {
				sData->data.fPressureMMHG = ((float)(i32BufferPressSum >> DMA_RX_BUFFER_DIV) / 16777216.0) / 0.25 * 2* 750.06;
				sData->data.fTemperatureC = ((float)(i32BufferTempSum >> DMA_RX_BUFFER_DIV) / 65536.0) / 0.454545 * 100 + 25;
			} else {
				sData->data.fPressureMMHG = ((float)(i32BufferPressSum / (float)u32BufferFrmeCnt) / 16777216.0) / 0.25 * 2* 750.06;
				sData->data.fTemperatureC = ((float)(i32BufferTempSum / (float)u32BufferFrmeCnt) / 65536.0) / 0.454545 * 100 + 25;
			}

			memcpy(&sData->data.uStatus.u8Stat[0], &uStatusSum.u8Stat[0], 3);

			sData->data.sErrorStats.u32Valid = u32ValidFrames;
			sData->data.sErrorStats.u32ErrCRC = u32ErrorsCRC;
			sData->data.sErrorStats.u32ErrSOF = u32ErrorsSOF;
			sData->data.sErrorStats.u32ErrUNK = u32ErrorsUNK;

			xQueueSend( xIrrigationPressureData, ( void * ) &sData, ( TickType_t ) 0 );

			sData = get_sensor_struct();

			if (sData == NULL) {
				DEBUG_SendTextFrame("PressureAnalysis_Thread: get_sensor_struct ERROR");
			}

#if (DEBUG_PRESSURE_SENSOR_THREAD_LL == 1)
			DEBUG_SendTextFrame("Pressure - STOP PROCESSING (%x)", ulNotifiedValue);
			DEBUG_SendTextFrame("           DMA NDTR: %d", DMA2_Stream1->NDTR);
			DEBUG_SendTextFrame("           DMA M0AR: %x", DMA2_Stream1->M0AR);
			DEBUG_SendTextFrame("           DMA M1AR: %x", DMA2_Stream1->M1AR);
			DEBUG_SendTextFrame("           DMA CR  : %x", DMA2_Stream1->CR);
			DEBUG_SendTextFrame("           DMA MEM : %x", LL_DMA_GetCurrentTargetMem(DMA2, LL_DMA_STREAM_1));
#endif
		} else {
#if (DEBUG_PRESSURE_SENSOR_THREAD_HL == 1)
			DEBUG_SendTextFrame("Pressure - NO DATA FROM SENSOR!!!!");
#endif
		}
	}
}

static const uint16_t crctable[256] = {
	0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011, 0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
	0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
	0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
	0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
	0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
	0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
	0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
	0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132, 0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
	0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
	0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
	0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
	0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
	0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
	0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
	0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252, 0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
	0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};

// CRC16_CCIT_ZERO implementation with the following parameterization:
//  - input reflected: no
//  - result reflected: no
//  - polynomial: 0x8005
//  - initial value: 0xFFFF
//  - final XOR value: 0x0
static inline uint16_t CalculateCarmenCRC16(uint16_t crc, const void *c_ptr, size_t len) {
    const uint8_t *c = c_ptr;

    while (len--)
        crc = (crc << 8) ^ crctable[((crc >> 8) ^ *c++)];

    return crc;
}
