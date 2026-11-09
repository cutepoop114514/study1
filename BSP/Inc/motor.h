#ifndef __MOTOR_H
#define __MOTOR_H
#include "main.h"

void Motor_Init(void);
void Motor_Task(void);
void Motor_EC11_Scan(void);
void Motor_Key_K1(void);
int16_t Motor_GetRPM(void);
int16_t Motor_GetDuty(void);

int32_t Motor_GetTotalPulses(void);
void Motor_ResetTotalPulses(void);
#endif