#ifndef DEBUG_H_
#define DEBUG_H_

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

void DEBUG_Init(void);
void DEBUG_ResetCommunication(void);

void DEBUG_SendTextHeader(void);

void COMM_ProcessRxedData(void);

void DEBUG_SendDataFrame(const char* cName, const uint8_t u8NoOfRows, const uint8_t u8NoOfCols, const float *pfData);
void DEBUG_SendTextFrame(const char* str, ...);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */
