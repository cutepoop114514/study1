#ifndef __UART_H
#define __UART_H
#include "main.h"

void UART_Init(void);
void UART_Task(void);
void UART_Key_K1(void);
uint8_t UART_Is_Popup(void);
char* UART_Get_Popup(void);

#endif