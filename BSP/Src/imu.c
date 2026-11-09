#include "imu.h"
#include "usart.h"

static float roll = 0.0f, pitch = 0.0f, yaw = 0.0f, yaw_rate = 0.0f;
static float yaw_zero = 0.0f; // 零点偏置
static uint8_t yaw_zero_flag = 0; // 是否已经记录了初始零点
uint8_t imu_err_code = 0;
static uint8_t buffer[11];
static uint8_t jy901_index = 0;

void IMU_Init(void) {
    // 串口接收初始化在 UART_Init 中已调用
}

float IMU_GetRoll(void)  { return roll; }
float IMU_GetPitch(void) { return pitch; }
float IMU_GetYaw(void)   { return yaw - yaw_zero; } // 相对偏航角
float IMU_GetYawRate(void) { return yaw_rate; }

void IMU_ResetYawZero(void) {
    yaw_zero = yaw;
}

void IMU_Receive_Handler(uint8_t data) {
    // 找帧头
    if (jy901_index == 0 && data != 0x55) return;
    
    buffer[jy901_index++] = data;

    if (jy901_index >= 11) {
        jy901_index = 0; 

        // 校验和
        uint8_t sum = 0;
        for (int i = 0; i < 10; i++) sum += buffer[i];
        
        if (sum == buffer[10]) { 
            imu_err_code = 0;
            
            // 1. 解析角度帧 (0x51)
            if (buffer[1] == 0x51) { 
                int16_t raw_roll  = (buffer[3] << 8) | buffer[2];
                int16_t raw_pitch = (buffer[5] << 8) | buffer[4];
                int16_t raw_yaw   = (buffer[7] << 8) | buffer[6];
                
                float r = (float)raw_roll  / 32768.0f * 180.0f;
                float p = (float)raw_pitch / 32768.0f * 180.0f;
                float y = (float)raw_yaw   / 32768.0f * 180.0f;

                // 上电后记录第一次读到的 Yaw 作为零点
                if (yaw_zero_flag == 0) {
                    yaw_zero = y;
                    yaw_zero_flag = 1;
                }

                roll  = roll  * 0.8f + r * 0.2f;
                pitch = pitch * 0.8f + p * 0.2f;
                yaw   = yaw   * 0.8f + y * 0.2f;
            }
            // 2. 解析角速度帧 (0x52)
            else if (buffer[1] == 0x52) {
                // 0x52 帧的前三个字节是角速度，维特协议中角速度单位通常为 度/秒 (°/s)
                int16_t raw_rate_x = (buffer[3] << 8) | buffer[2];
                int16_t raw_rate_y = (buffer[5] << 8) | buffer[4];
                int16_t raw_rate_z = (buffer[7] << 8) | buffer[6]; // Z轴即为 Yaw 角速度
                
                // 维特智能协议：角速度转换系数通常为 32768/2000 = 16.384
                float rate_z = (float)raw_rate_z / 32768.0f * 2000.0f; 
                
                yaw_rate = yaw_rate * 0.8f + rate_z * 0.2f; // 滤波
            }
        }
    }
}