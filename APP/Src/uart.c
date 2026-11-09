#include "uart.h"
#include "usart.h"
#include "vofa.h"
#include "imu.h"
#include "command.h"
#include "led.h"
#include "servo.h"
#include "password.h"
#include <string.h>
#include <stdio.h>

// 定义 DMA 接收缓冲区和 IMU 接收单字节变量
uint8_t rx_dma_buf[128];
uint8_t imu_rx_byte; 

static uint8_t popup_flag = 0;
static uint32_t popup_tick = 0;
static char popup_msg[32] = "";

char* UART_Get_Popup(void) { return popup_msg; }

void UART_SendString(char *str) {
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
}

void UART_Init(void) {
    // USART2 用 DMA 空闲中断接收指令
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_dma_buf, sizeof(rx_dma_buf));
    // USART3 留给 IMU，用普通中断
    HAL_UART_Receive_IT(&huart3, &imu_rx_byte, 1);
}

void UART_Key_K1(void) {
    UART_SendString("[Hello Robot]\r\n");
}

uint8_t UART_Is_Popup(void) { return popup_flag; }

static void UART_ShowPopup(char *msg) {
    popup_flag = 1;
    popup_tick = HAL_GetTick();
    strcpy(popup_msg, msg);
}

// 仅保留 USART3 的接收中断（给 IMU 用）
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        IMU_Receive_Handler(imu_rx_byte);
        HAL_UART_Receive_IT(&huart3, &imu_rx_byte, 1); 
    }
}

// USART2 的 DMA 空闲中断（接收 VOFA 指令）
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART2) {
        if (Size > 0) {
            Command_Write(rx_dma_buf, Size); // 存入你的命令环形队列
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_dma_buf, 128);
    }
}

void UART_Task(void) {
    if (popup_flag && HAL_GetTick() - popup_tick >= 2000) popup_flag = 0;

    uint8_t cmd[64];
    uint8_t len;

    // 从环形队列中读取完整指令
    while ((len = Command_GetCommand(cmd)) > 0) {
        
        // 1. VOFA 调参指令
        if (strncmp((char*)cmd, "SET,", 4) == 0) {
            Vofa_Parse((char*)cmd);
            continue;
        }

        // 2. LED 控制
        if (strcmp((char*)cmd, "[1]") == 0) {
            LED_Set_Mode(1); UART_SendString("OK\r\n"); UART_ShowPopup("LED ON");
        }
        else if (strcmp((char*)cmd, "[2]") == 0) {
            LED_Set_Mode(0); UART_SendString("OK\r\n"); UART_ShowPopup("LED OFF");
        }
        else if (strcmp((char*)cmd, "[3]") == 0) {
            LED_Set_Mode(2); UART_SendString("OK\r\n"); UART_ShowPopup("LED BLINK");
        }
        // 3. 舵机控制
        else if (strcmp((char*)cmd, "[4]") == 0) {
            Servo_SetAngle(0); UART_SendString("OK\r\n"); UART_ShowPopup("SERVO 0");
        }
        else if (strcmp((char*)cmd, "[5]") == 0) {
            Servo_SetAngle(90); UART_SendString("OK\r\n"); UART_ShowPopup("SERVO 90");
        }
        else if (strcmp((char*)cmd, "[6]") == 0) {
            Servo_SetAngle(180); UART_SendString("OK\r\n"); UART_ShowPopup("SERVO 180");
        }
        else if (cmd[0] == '[' && cmd[1] == '7' && cmd[2] == ':') {
            int angle = 0;
            sscanf((char*)cmd, "[7:%d]", &angle);
            if (angle >= 0 && angle <= 180) {
                Servo_SetAngle(angle); UART_SendString("OK\r\n"); UART_ShowPopup("SERVO CUSTOM");
            } else {
                UART_SendString("ERROR\r\n"); UART_ShowPopup("ERR ANGLE");
            }
        }
        // 4. 密码修改
        else if (cmd[0] == '[' && cmd[1] == 'P' && cmd[2] == ':') {
            char old_pwd[6], new_pwd[6];
            if (sscanf((char*)cmd, "[P:%5[^:]:%5[^]]]", old_pwd, new_pwd) == 2) {
                if (Password_Change(old_pwd, new_pwd)) {
                    UART_SendString("OK\r\n"); UART_ShowPopup("PWD CHANGED");
                } else {
                    UART_SendString("ERROR\r\n"); UART_ShowPopup("ERR PWD");
                }
            } else {
                UART_SendString("ERROR\r\n"); UART_ShowPopup("ERR FORMAT");
            }
        }
        else {
            UART_SendString("ERROR\r\n"); UART_ShowPopup("ERR CMD");
        }
    }
}