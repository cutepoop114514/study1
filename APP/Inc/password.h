#ifndef __PASSWORD_H
#define __PASSWORD_H
#include "main.h"

void Password_Init(void);
void Password_Key_K1(void);
void Password_Key_K2(void);
void Password_Key_K3(void);
void Password_Task(void);
void Password_Draw(void);
uint8_t Password_IsUnlocked(void);
void Password_Lock(void);
uint8_t Password_Change(char *old_pwd, char *new_pwd);

#endif