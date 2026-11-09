#ifndef __VOFA_H
#define __VOFA_H
#include "main.h"

void Vofa_Parse(char *cmd);
void Vofa_SendWave(int16_t target, int16_t actual, int16_t out, int16_t err);
void Vofa_SendIMU(float roll, float pitch, float yaw);
#endif