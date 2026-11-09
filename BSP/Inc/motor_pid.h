#ifndef __MOTOR_PID_H
#define __MOTOR_PID_H
#include "main.h"

void SpeedPID_Enable(uint8_t enable);
uint8_t SpeedPID_IsEnabled(void);
void SpeedPID_EC11_Scan(void);
void SpeedPID_Task(void);
int16_t SpeedPID_GetTarget(void);
int16_t SpeedPID_GetOutput(void);

void Motor_CheckStall(void);
void Motor_ClearStall(void);
uint8_t Motor_IsStall(void);
void SpeedPID_SetTarget(int16_t target);
void PosPID_SetTarget(int16_t target); 

#endif