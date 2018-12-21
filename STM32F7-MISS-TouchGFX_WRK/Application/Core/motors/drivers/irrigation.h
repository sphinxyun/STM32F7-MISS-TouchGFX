#ifndef MOTORS_H_
#define MOTORS_H_

#include "global.h"

void Configure_ENCODER(void);
void Configure_PWM(void);

void MOTOR_Start(uint16_t u16PWM);
void MOTOR_UpdateSpeed(uint16_t u16PWM);
void MOTOR_Stop(void);

void StartSpeedMonitoring(void);

#endif /* MOTORS_H_ */
