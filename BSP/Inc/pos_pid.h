#ifndef __POS_PID_H
#define __POS_PID_H
#include "main.h"

void PosPID_Enable(uint8_t enable);
uint8_t PosPID_IsEnabled(void);
void PosPID_EC11_Scan(void);
void PosPID_Task(void);
int16_t PosPID_GetTarget(void);
int32_t PosPID_GetActual(void);
int16_t PosPID_GetOutput(void);
void PosPID_ClearPos(void);

#endif