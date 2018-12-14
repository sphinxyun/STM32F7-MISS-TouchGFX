#ifndef MOTORS_H_
#define MOTORS_H_

void Configure_ENCODER(void);
void Configure_PWM(void);

void MOTOR_Start(void);
void MOTOR_Stop(void);

void StartSpeedMonitoring(void);

#endif /* MOTORS_H_ */
