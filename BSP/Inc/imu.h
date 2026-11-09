#ifndef __IMU_H
#define __IMU_H
#include "main.h"

extern uint8_t imu_err_code;
extern uint8_t imu_rx_byte;

void IMU_Init(void);
float IMU_GetRoll(void);
float IMU_GetPitch(void);
float IMU_GetYaw(void);
float IMU_GetYawRate(void); // 新增：获取 Yaw 角速度
void IMU_ResetYawZero(void); // 新增：重置零点
void IMU_Receive_Handler(uint8_t rx_data);

#endif