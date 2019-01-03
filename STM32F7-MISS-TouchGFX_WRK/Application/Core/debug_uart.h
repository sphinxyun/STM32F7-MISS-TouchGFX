#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "global.h"

void DEBUG_UART_Init(void);
void DEBUG_UART_ResetComm(void);

void DEBUG_UART_SysTick(void);

bool DEBUG_UART_SendByte(uint8_t u8Data);
bool DEBUG_UART_SendData(uint8_t *pu8Data, uint16_t u16Length, uint16_t *pu16SendLength);

// TX BUFFER - INFORMATION & SERVICES
void DEBUG_UART_FlushTxBuffer(void);
uint16_t DEBUG_UART_NoOfWaitingTxBytes(void);
uint16_t DEBUG_UART_NoOfFreeTxBytes(void);

bool DEBUG_UART_TxFinished(void);
bool DEBUG_UART_IsTxBufferEmpty(void);
bool DEBUG_UART_IsTxBufferFull(void);

#endif /* DEBUG_UART_H_ */
