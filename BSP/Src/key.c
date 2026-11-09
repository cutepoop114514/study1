#include "key.h"

uint8_t Key_Get_K1(void) {
    return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
}

uint8_t Key_Get_K2(void) {
    return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
}

uint8_t Key_Get_K3(void) {
    return HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin);
}