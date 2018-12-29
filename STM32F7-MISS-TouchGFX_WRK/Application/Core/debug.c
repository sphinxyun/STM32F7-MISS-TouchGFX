#include <string.h>
#include <stdarg.h>
#include "stm32f7xx_hal.h"

#include "global.h"
#include "version.h"
#include "debug_uart.h"
#include "debug.h"

#define DEBUG_COMM_MODULE 		0

#define MAX_PAYLOAD_LENGTH	128

#define START1 	0xAA
#define START2 	0x55

#define BINARY_DEBUG_DATA_COMMAND	'd'
#define TEXT_DEBUG_DATA_COMMAND		't'

#define RPM_PID_DEBUG_DATA_COMMAND	'p'

uint8_t u8FrmCnt;

typedef enum {
	eCommIdle = 0, eCommStart, eCommCmd, eCommFrmCnt, eCommLenHigh, eCommLenLow, eCommPayload, eCommCRC
} eProtocolState_t;

typedef struct {
	uint8_t u8Cmd;
	uint8_t u8FrmCnt;
	uint8_t u8Len_H;
	uint8_t u8Len_L;
	uint8_t u8Payload[MAX_PAYLOAD_LENGTH];
	uint32_t u32CRC;

	uint16_t u16Len;
} eProtocolFrame_t;

static eProtocolState_t eProtocolState = 0;
static eProtocolFrame_t sRxedFrame;

static inline uint32_t reverse(uint32_t x);
static inline uint32_t crc32(const uint8_t *frame, uint16_t length);

void DEBUG_Init(void) {
	DEBUG_UART_Init();
	DEBUG_ResetCommunication();
}

void DEBUG_ResetCommunication(void) {
	DEBUG_UART_ResetComm();

	u8FrmCnt = 0;
	eProtocolState = eCommIdle;
}

void DEBUG_SendTextHeader(void) {
	DEBUG_SendTextFrame("");
	DEBUG_SendTextFrame("----------------------------------");
	DEBUG_SendTextFrame("  LCDPS Test, VER. %s", VER_PRODUCTVERSION_STR);
	DEBUG_SendTextFrame("   %s, %s", __DATE__, __TIME__);
	DEBUG_SendTextFrame("----------------------------------");
	DEBUG_SendTextFrame("   SystemCoreClock: %d [Hz]", SystemCoreClock);
	DEBUG_SendTextFrame("----------------------------------");
	DEBUG_SendTextFrame("");
}

static void DEBUG_ParseFrame(eProtocolFrame_t *sFrame) {
	switch (sFrame->u8Cmd) {
	default:
		DEBUG_SendTextFrame("unknown command");
		break;
	}
}

void DEBUG_ProcessRxedData(void) {
	static uint16_t u16RxedBytesCnt;
	uint8_t u8RxedByte;
	while (UART_bGetRxByte(&u8RxedByte)) {

#if (DEBUG_COMM_MODULE == 1)
		UART_vPutString("rxed: ");
		UART_vPutHEX_u8(u8RxedByte, true);
		UART_vPutString(" -> ");
#endif

		switch (eProtocolState) {
		case eCommIdle:
			if (u8RxedByte == START1)
				eProtocolState = eCommStart;
			break;

		case eCommStart:
			if (u8RxedByte == START2)
				eProtocolState = eCommCmd;
			else
				eProtocolState = eCommIdle;
			break;

		case eCommCmd:
			sRxedFrame.u8Cmd = u8RxedByte;
			eProtocolState = eCommFrmCnt;
			break;

		case eCommFrmCnt:
			sRxedFrame.u8FrmCnt = u8RxedByte;
			eProtocolState = eCommLenHigh;
			break;

		case eCommLenHigh:
			sRxedFrame.u8Len_H = u8RxedByte;
			sRxedFrame.u16Len = u8RxedByte << 8;
			eProtocolState = eCommLenLow;
			break;

		case eCommLenLow:
			sRxedFrame.u8Len_L = u8RxedByte;
			sRxedFrame.u16Len |= u8RxedByte;

			if (sRxedFrame.u16Len <= MAX_PAYLOAD_LENGTH) {
				u16RxedBytesCnt = 0;

//#if (DEBUG_COMM_MODULE == 1)
//				UART_vPutString("\n\nHEADER:");
//				UART_vPutString("\n  cmd:      ");
//				UART_vPutHEX_u8(sRxedFrame.u8Cmd, true);
//				UART_vPutString("\n  len:      ");
//				UART_vPutHEX_u16(sRxedFrame.u16Len, true);
//				UART_vPutString("\n\n");
//#endif

				if (sRxedFrame.u16Len != 0) {
					eProtocolState = eCommPayload;
				} else {
					sRxedFrame.u32CRC = 0;
					eProtocolState = eCommCRC;
				}
			} else {
				//ERROR!!!
				eProtocolState = eCommIdle;
			}
			break;

		case eCommPayload:
			sRxedFrame.u8Payload[u16RxedBytesCnt++] = u8RxedByte;

			if (u16RxedBytesCnt >= sRxedFrame.u16Len) {
				u16RxedBytesCnt = 0;
				sRxedFrame.u32CRC = 0;
				eProtocolState = eCommCRC;
			}
			break;

		case eCommCRC:
			sRxedFrame.u32CRC <<= 8;
			sRxedFrame.u32CRC |= u8RxedByte;
			u16RxedBytesCnt++;

			if (u16RxedBytesCnt >= 4) {
				uint32_t u32CompCRC = crc32((uint8_t *) &sRxedFrame, sRxedFrame.u16Len + 4);

#if (DEBUG_COMM_MODULE == 1)
				UART_vPutString("\n\nFULL FRAME:");
				UART_vPutString("\n  cmd:      ");
				UART_vPutHEX_u8(sRxedFrame.u8Cmd, true);
				UART_vPutString("\n  len:      ");
				UART_vPutHEX_u16(sRxedFrame.u16Len, true);
				if (sRxedFrame.u16Len != 0) {
					UART_vPutString("\n  payload:  ");
					for (uint16_t i = 0; i < sRxedFrame.u16Len; i++) {
						UART_vPutHEX_u8(sRxedFrame.u8Payload[i], false);
						UART_vPutString(" ");
					}
				} else {
					UART_vPutString("\n  payload:  ---");
				}
				UART_vPutString("\n  crc rxed: ");
				UART_vPutHEX_u32(sRxedFrame.u32CRC, true);
				UART_vPutString("\n  crc comp: ");
				UART_vPutHEX_u32(u32CompCRC, true);
				UART_vPutString("\n");
#endif

				if (u32CompCRC == sRxedFrame.u32CRC) {
#if (DEBUG_COMM_MODULE == 1)
					UART_vPutString("CRC OK!!!\n\n");
#endif

					//parse rxed frame:
					DEBUG_ParseFrame(&sRxedFrame);
				} else {
#if (DEBUG_COMM_MODULE == 1)
					UART_vPutString("CRC ERROR!!!\n\n");
#endif
				}

				eProtocolState = eCommIdle;
			}
			break;

		default:
			eProtocolState = eCommIdle;
			break;
		}

#if (DEBUG_COMM_MODULE == 1)
		UART_vPutHEX_u8(eProtocolState, true);
		UART_vPutString("\n");
#endif
	}
}

static inline char *convert_x(uint32_t byte) {
	static char buffer[3] = { 2, '\0', '\0' };

	uint32_t temp;
	byte &= 0x000000FF;

	temp = byte / 16;
	if (temp > 9) {
		buffer[1] = (temp + 0x37);
	} else {
		buffer[1] = (temp + 0x30);
	}

	temp = byte % 16;
	if (temp > 9) {
		buffer[2] = (temp + 0x37);
	} else {
		buffer[2] = (temp + 0x30);
	}

	buffer[0] = 2;

	return ( buffer );
}

static inline char *convert_d(uint32_t num, uint32_t base) {
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50];
	char *ptr;

	//on purpose:
	//50 instead of 49
	ptr = &buffer[50];

	uint32_t u32RepLen = 0;

	do {
		*--ptr = Representation[num % base];
		num /= base;
		u32RepLen++;
	} while (num != 0);

	*--ptr = u32RepLen;

	return (ptr);
}

static inline char* convert_f(float x, uint32_t numDigits) {
	static char buffer[50];

	unsigned char firstplace = 0;
	unsigned char negative;
	unsigned char i, digit;
	double place = 1.0;

	uint32_t u32RepLen = 1;

	// save sign
	negative = (x < 0);
	// convert to absolute value
	x = (x > 0) ? (x) : (-x);

	// find starting digit place
	for (i = 0; i < 15; i++) {
		if ((x / place) < 10.0)
			break;
		else
			place *= 10.0;
	}
	// print polarity character
	if (negative)
		buffer[u32RepLen++] = '-';
	else
		buffer[u32RepLen++] = '+';

	// print digits
	for (i = 0; i < numDigits; i++) {
		digit = (x / place);

		if (digit | firstplace | (place == 1.0)) {
			firstplace = 1;
			buffer[u32RepLen++] = digit + 0x30;
		} else
			buffer[u32RepLen++] = ' ';

		if (place == 1.0) {
			buffer[u32RepLen++] = '.';
		}

		x -= (digit * place);
		place /= 10.0;
	}
	buffer[0] = u32RepLen-1;

	return buffer;
}

void DEBUG_SendTextFrame(const char* str, ...) {
	const char *ptr = str;
	uint16_t idx = 0;
	char temp[768];

	va_list argp;
	va_start(argp, str);

	while (*ptr) {
		if (*ptr != '%') {
			temp[idx] = *ptr;
			idx++;
			ptr++;
			continue;
		}

		switch (*++ptr) {
			case '%': {
				temp[idx] = *ptr; idx++;
				break;
			}

			case 'c': {
				char c = va_arg(argp, signed int);
				temp[idx] = c; idx++;
				break;
			}

			case 's': {
				char* s = va_arg(argp, char *);
				while (*s) {
					temp[idx] = *s; idx++;
					s++;
				}
				break;
			}

			case 'f': {
				float f = va_arg(argp, double);
				char *ndz = convert_f(f, 7);
				memcpy(&temp[idx], &ndz[1], ndz[0]);
				idx += ndz[0];

				break;
			}

			case 'd': {
				int32_t i = va_arg(argp, signed int);
				if (i < 0) {
					i = -i;
					temp[idx] = '-'; idx++;
				}

				char *ndz = convert_d(i, 10);
				memcpy(&temp[idx], &ndz[1], ndz[0]);
				idx += ndz[0];
				break;
			}

			case 'x':	{
				uint32_t x = va_arg(argp, unsigned int);
				temp[idx] = '0'; idx++;
				temp[idx] = 'x'; idx++;
				char *ndz = convert_d(x, 16);
				memcpy(&temp[idx], &ndz[1], ndz[0]);
				idx += ndz[0];
				break;
			}

			case 't': {
				uint8_t* tbl = va_arg(argp, uint8_t *);
				uint32_t len = va_arg(argp, unsigned int);
				for (uint8_t i = 0; i < len; i++) {
					char *ndz = convert_x(tbl[i]);
					memcpy(&temp[idx], &ndz[1], ndz[0]);
					idx += ndz[0];

					if (i < len-1) {
						temp[idx] = ' '; idx++;
					}
				}
				break;
			}
		}

		ptr++;
	}

	va_end(argp);

	uint16_t u16LengthInBytes = idx;

	uint8_t u8Frame[768];
	u8Frame[0] = START1;
	u8Frame[1] = START2;

	u8Frame[2] = TEXT_DEBUG_DATA_COMMAND;

	//frame no
	u8Frame[3] = u8FrmCnt;
	//omit frame number zero in normal frame tx mode - frames with zero are special :-)
	if (++u8FrmCnt == 0) {
		u8FrmCnt = 1;
	}

	u8Frame[4] = (u16LengthInBytes >> 8) & 0x00FF;
	u8Frame[5] = (u16LengthInBytes >> 0) & 0x00FF;

	memcpy(&u8Frame[6], temp, u16LengthInBytes);

	uint32_t crc_result = crc32(&u8Frame[2], 4 + u16LengthInBytes);

	u8Frame[6 + u16LengthInBytes + 0] = (crc_result & 0xff000000) >> 24;
	u8Frame[6 + u16LengthInBytes + 1] = (crc_result & 0x00ff0000) >> 16;
	u8Frame[6 + u16LengthInBytes + 2] = (crc_result & 0x0000ff00) >> 8;
	u8Frame[6 + u16LengthInBytes + 3] = (crc_result & 0x000000ff) >> 0;

	uint16_t u16BytesSent;
	DEBUG_UART_SendData(u8Frame, 2 + 4 + u16LengthInBytes + 4, &u16BytesSent);
}

void DEBUG_SendRpmPidDataFrame(const uint8_t u8NoOfItems, const float *pfData) {
	uint16_t u16LengthInBytes = 1 + 4 * u8NoOfItems;

	uint8_t u8Frame[265];
	u8Frame[0] = START1;
	u8Frame[1] = START2;

	u8Frame[2] = RPM_PID_DEBUG_DATA_COMMAND;

	//frame no
	u8Frame[3] = u8FrmCnt;
	//omit frame number zero in normal frame tx mode - frames with zero are special :-)
	if (++u8FrmCnt == 0) {
		u8FrmCnt = 1;
	}

	u8Frame[4] = (u16LengthInBytes >> 8) & 0x00FF;
	u8Frame[5] = (u16LengthInBytes >> 0) & 0x00FF;

	u8Frame[6] = u8NoOfItems;

	for (int i = 0; i < u8NoOfItems; i++) {
		memcpy(&u8Frame[6 + 1 + i * 4], &pfData[i], 4);
	}

	uint32_t crc_result = crc32(&u8Frame[2], 4 + u16LengthInBytes);

	u8Frame[6 + u16LengthInBytes + 0] = (crc_result & 0xff000000) >> 24;
	u8Frame[6 + u16LengthInBytes + 1] = (crc_result & 0x00ff0000) >> 16;
	u8Frame[6 + u16LengthInBytes + 2] = (crc_result & 0x0000ff00) >> 8;
	u8Frame[6 + u16LengthInBytes + 3] = (crc_result & 0x000000ff) >> 0;

	uint16_t u16BytesSent;
	DEBUG_UART_SendData(u8Frame, 2 + 4 + u16LengthInBytes + 4, &u16BytesSent);
}

void DEBUG_SendDataFrame(const char* cName, const uint8_t u8NoOfRows, const uint8_t u8NoOfCols, const float *pfData) {
	uint16_t u16LengthInBytes = 10 + 2 + 4 * (u8NoOfRows * u8NoOfCols);

	uint8_t u8Frame[265];
	u8Frame[0] = START1;
	u8Frame[1] = START2;

	u8Frame[2] = BINARY_DEBUG_DATA_COMMAND;

	//frame no
	u8Frame[3] = u8FrmCnt;
	//omit frame number zero in normal frame tx mode - frames with zero are special :-)
	if (++u8FrmCnt == 0) {
		u8FrmCnt = 1;
	}

	u8Frame[4] = (u16LengthInBytes >> 8) & 0x00FF;
	u8Frame[5] = (u16LengthInBytes >> 0) & 0x00FF;

	memcpy(&u8Frame[6], cName, 10);
	u8Frame[6 + 10 + 0] = u8NoOfRows;
	u8Frame[6 + 10 + 1] = u8NoOfCols;

	for (int i = 0; i < (u8NoOfRows * u8NoOfCols); i++) {
		memcpy(&u8Frame[6 + 10 + 2 + i * 4], &pfData[i], 4);
	}

	uint32_t crc_result = crc32(&u8Frame[2], 4 + u16LengthInBytes);

	u8Frame[6 + u16LengthInBytes + 0] = (crc_result & 0xff000000) >> 24;
	u8Frame[6 + u16LengthInBytes + 1] = (crc_result & 0x00ff0000) >> 16;
	u8Frame[6 + u16LengthInBytes + 2] = (crc_result & 0x0000ff00) >> 8;
	u8Frame[6 + u16LengthInBytes + 3] = (crc_result & 0x000000ff) >> 0;

	uint16_t u16BytesSent;
	DEBUG_UART_SendData(u8Frame, 2 + 4 + u16LengthInBytes + 4, &u16BytesSent);
}

// Reverses (reflects) bits in a 32-bit word.
static inline uint32_t reverse(uint32_t x) {
	x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
	x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
	x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
	x = (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24);
	return x;
}

static inline uint32_t crc32(const uint8_t *frame, uint16_t length) {
	int i, j;
	uint32_t byte, crc;

	i = 0;
	crc = 0xffffffff;
	while (length != 0) {
		length--;
		byte = frame[i];            	// Get next byte.
		byte = reverse(byte);         	// 32-bit reversal.
		for (j = 0; j <= 7; j++) {    	// Do eight times.
			if ((int) (crc ^ byte) < 0)
				crc = (crc << 1) ^ 0x04C11DB7;
			else
				crc = crc << 1;
			byte = byte << 1;          	// Ready next msg bit.
		}
		i = i + 1;
	}
	return reverse(~crc);
}

////void UART_vPutChar(uint8_t ch) {
////	UART_SendData(&ch, 1);
////}
//
//void UART_vPutString(char * str) {
//	UART_SendData((uint8_t *) str, strlen(str));
//}
//
//void UART_vPutConstString(const char * str) {
//	UART_SendData((uint8_t *) str, strlen(str));
//}
//
//uint16_t UART_u16SendData(uint8_t *pu8Data, uint16_t u16Len) {
////	uint16_t i16TxCnt = 0;
////
////	while (UART_NoOfFreeTxBufferBytes() <= u8Len) {
////		i16TxCnt++;
////	}
//
//	UART_SendData(pu8Data, u16Len);
//
//	return 0; //i16TxCnt;
//}
//
//void UART_vPutHEX_u64(uint64_t qword, uint32_t u32Add0x) {
//	UART_vPutHEX_u32((qword >> 32) & 0x00000000FFFFFFFF, u32Add0x);
//	UART_vPutHEX_u32((qword >> 0) & 0x00000000FFFFFFFF, 0);
//}
//
//void UART_vPutHEX_u32(uint32_t dword, uint32_t u32Add0x) {
//	UART_vPutHEX_u16((dword >> 16) & 0x0000FFFF, u32Add0x);
//	UART_vPutHEX_u16((dword >> 0) & 0x0000FFFF, 0);
//}
//
//void UART_vPutHEX_u24(uint32_t dword, uint32_t u32Add0x) {
//	UART_vPutHEX_u8((dword >> 16) & 0x000000FF, u32Add0x);
//	UART_vPutHEX_u8((dword >> 8) & 0x000000FF, 0);
//	UART_vPutHEX_u8((dword >> 0) & 0x000000FF, 0);
//}
//
//void UART_vPutHEX_u16(uint32_t word, uint32_t u32Add0x) {
//	UART_vPutHEX_u8((word >> 8) & 0x000000FF, u32Add0x);
//	UART_vPutHEX_u8((word >> 0) & 0x000000FF, 0);
//}
//
//void UART_vPutHEX_u8(uint32_t byte, uint32_t u32Add0x) {
//	char HexByte[] = { '0', 'x', '\0', '\0', '\0' };
//
//	uint32_t temp;
//	byte &= 0x000000FF;
//
//	temp = byte / 16;
//	if (temp > 9) {
//		HexByte[2] = (temp + 0x37);
//	} else {
//		HexByte[2] = (temp + 0x30);
//	}
//
//	temp = byte % 16;
//	if (temp > 9) {
//		HexByte[3] = (temp + 0x37);
//	} else {
//		HexByte[3] = (temp + 0x30);
//	}
//
//	if (u32Add0x)
//		UART_vPutString(&HexByte[0]);
//	else
//		UART_vPutString(&HexByte[2]);
//}
//
//void UART_vPutNum_i8(int8_t num, uint8_t radix) {
//	char string[17];
//	itoa(num, string, radix);
//	UART_vPutString(string);
//}
//
//void UART_vPutNum_i16(int16_t num, uint8_t radix) {
//	char string[17];
//	itoa(num, string, radix);
//	UART_vPutString(string);
//}
//
//void UART_vPutNum_i32(int32_t num, uint8_t radix) {
//	char string[17];
//	itoa(num, string, radix);
//	UART_vPutString(string);
//}
//
//void UART_vCRLF(void) {
//	UART_vPutString("\n\r");
//}
//
//void UART_vLF(void) {
//	UART_vPutString("\n");
//}
//
//void UART_vCR(void) {
//	UART_vPutString("\r");
//}
//
//static void __reverse(char* begin, char* end) {
//	char temp;
//
//	while (end > begin) {
//		temp = *end;
//		*end-- = *begin;
//		*begin++ = temp;
//	}
//}
//
//char* itoa(int value, char* buffer, int base) {
//	static const char digits[] = "0123456789abcdef";
//
//	char* buffer_copy = buffer;
//	int32_t sign = 0;
//	int32_t quot, rem;
//
//	if ((base >= 2) && (base <= 16))				// is the base valid?
//			{
//		if (base == 10 && (sign = value) < 0)// negative value and base == 10? store the copy (sign)
//			value = -value;					// make it positive
//
//		do {
//			quot = value / base;			// calculate quotient and remainder
//			rem = value % base;
//			*buffer++ = digits[rem];	// append the remainder to the string
//		} while ((value = quot));	// loop while there is something to convert
//
//		if (sign < 0)							// was the value negative?
//			*buffer++ = '-';					// append the sign
//
//		__reverse(buffer_copy, buffer - 1);		// reverse the string
//	}
//
//	*buffer = '\0';
//	return buffer_copy;
//}
