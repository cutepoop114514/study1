#include "vofa.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern float Kp, Ki, Kd;
void Vofa_Parse(char *cmd) {
    if (strncmp(cmd, "SET,", 4) == 0) {
        char name[10];
        float value;
        if (sscanf(cmd, "SET,%[^,],%f", name, &value) == 2) {
            if (strcmp(name, "kp") == 0) Kp = value;
            else if (strcmp(name, "ki") == 0) Ki = value;
            else if (strcmp(name, "kd") == 0) Kd = value;
        }
    }
}
void Vofa_SendWave(int16_t target, int16_t actual, int16_t out, int16_t err) {
    char buf[64];
    sprintf(buf, "%d,%d,%d,%d\n", target, actual, out, err);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 100);
}
void Vofa_SendIMU(float roll, float pitch, float yaw) {
    char buf[64];
    int32_t r = (int32_t)(roll * 100);
    int32_t p = (int32_t)(pitch * 100);
    int32_t y = (int32_t)(yaw * 100);
    sprintf(buf, "%ld,%ld,%ld\n", r, p, y);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 100);
}