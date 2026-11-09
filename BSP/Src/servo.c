#include "servo.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"

static uint8_t current_angle = 90;

void Servo_Init(void) {
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    Servo_SetAngle(90);
}

void Servo_SetAngle(uint8_t angle) {
    if (angle > 180) angle = 180;
    else if (angle < 0) angle = 0;
    current_angle = angle;
    uint16_t ccr = 500 + (uint16_t)((angle / 180.0) * 2000);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, ccr);
}

uint8_t Servo_GetAngle(void) {
    return current_angle;
}

void Servo_Key_K1(void) {
    Servo_SetAngle(90);
}