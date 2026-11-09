#ifndef __LED_H
#define __LED_H
#include "main.h"

typedef enum {
    LED_MODE_OFF = 0,
    LED_MODE_ON,
    LED_MODE_BLINK
} LED_Mode;

void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
void LED_Task(void);
void LED_Key_K1(void);
uint8_t LED_Get_Mode(void);
void LED_Set_Mode(uint8_t mode);

#endif