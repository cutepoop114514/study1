#ifndef __CASCADE_PID_H
#define __CASCADE_PID_H
#include "main.h"

void CascadePID_Enable(uint8_t enable);
uint8_t CascadePID_IsEnabled(void);
void CascadePID_EC11_Scan(void);
void CascadePID_Task(void);

int16_t CascadePID_GetTgtPos(void);
int32_t CascadePID_GetActPos(void);
int16_t CascadePID_GetTgtRPM(void);
int16_t CascadePID_GetActRPM(void);

#endif