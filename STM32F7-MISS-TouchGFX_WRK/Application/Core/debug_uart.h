#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "global.h"

void DEBUG_UART_Init(void);
void DEBUG_UART_ResetComm(void);

void DEBUG_UART_SysTick(void);

bool DEBUG_UART_SendByte(uint8_t u8Data);
bool DEBUG_UART_SendData(uint8_t *pu8Data, uint16_t u16Length, uint16_t *pu16SendLength);

bool DEBUG_UART_GetByte(uint8_t* pu8Data);
bool DEBUG_UART_GetData(uint8_t* pu8Data, uint16_t u16MaxLength, uint16_t *pu16Length);

// TX BUFFER - INFORMATION & SERVICES
void DEBUG_UART_FlushTxBuffer(void);
uint16_t DEBUG_UART_NoOfWaitingTxBytes(void);
uint16_t DEBUG_UART_NoOfFreeTxBytes(void);

bool DEBUG_UART_TxFinished(void);
bool DEBUG_UART_IsTxBufferEmpty(void);
bool DEBUG_UART_IsTxBufferFull(void);

// RX BUFFER - INFORMATION & SERVICES
void DEBUG_UART_FlushRxBuffer(void);
uint16_t DEBUG_UART_NoOfWaitingRxBytes(void);
uint16_t DEBUG_UART_NoOfFreeRxBytes(void);

bool DEBUG_UART_IsRxBufferEmpty(void);
bool DEBUG_UART_IsRxBufferFull(void);


//void UART_vPutString(char * str);
//void UART_vPutConstString(const char * str);
//
//bool UART_bGetRxByte(uint16_t* pu16Data);
//
//void UART_vPutChar(uint8_t ch);
//uint16_t UART_u16SendData(uint8_t *pu8Data, uint16_t u16Len);
//
//void UART_vCRLF(void);
//void UART_vLF(void);
//
//void UART_vPutNum_i8(int8_t num, uint8_t radix);
//void UART_vPutNum_i16(int16_t num, uint8_t radix);
//void UART_vPutNum_i32(int32_t num, uint8_t radix);
//
//void UART_vPutHEX_u64(uint64_t qword, uint32_t u32Add0x);
//void UART_vPutHEX_u32(uint32_t word, uint32_t u32Add0x);
//void UART_vPutHEX_u24(uint32_t dword, uint32_t u32Add0x);
//void UART_vPutHEX_u16(uint32_t word, uint32_t u32Add0x);
//void UART_vPutHEX_u8(uint32_t byte, uint32_t u32Add0x);

#endif /* DEBUG_UART_H_ */
