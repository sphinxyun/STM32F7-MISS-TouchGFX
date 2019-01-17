#ifndef IRRIGATION_H_
#define IRRIGATION_H_

#include "global.h"

typedef enum {
	eVsUndervoltage = 0x3,
	eShortToVsAtO1andO2 = 0x5,
	eShortToGndAtO1andVsAtO2 = 0x6,
	eShortToVsAtO2 = 0x7,
	eShortToVsAtO1andGndAtO2 = 0x9,
	eShortToGndAtO1andO2 = 0xA,
	eShortToGndAtO2 = 0xB,
	eOpenLoad = 0xC,
	eShortToVsAtO1 = 0xD,
	eShortToGndAtO1 = 0xE,
	eNoFailure = 0xF
};

void IRRIGATION_Configure(void);

void IRRIGATION_Start(int16_t i16PWM);
void IRRIGATION_UpdateSpeed(int16_t i16PWM);
void IRRIGATION_Stop(void);

uint8_t IRRIGATION_ReadDiagnostics(void);

void IRRIGATION_StartSpeedMonitoring(void);

#endif /* IRRIGATION_H_ */
