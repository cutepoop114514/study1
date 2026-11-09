#include "led.h"

static uint8_t  led_mode = LED_MODE_BLINK;
static uint32_t blink_tick = 0;
static uint8_t  blink_state = 0;

void LED_Init(void) {
    led_mode = LED_MODE_BLINK;
    blink_tick = HAL_GetTick();
    blink_state = 0;
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
}

void LED_On(void)  { HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET); }
void LED_Off(void) { HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);   }
void LED_Toggle(void) { HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin); }

void LED_Key_K1(void) {
    led_mode = (led_mode + 1) % 3;
    if (led_mode == LED_MODE_BLINK) {
        blink_tick = HAL_GetTick();
        blink_state = 0;
    }
}

void LED_Task(void) {
    if (led_mode == LED_MODE_OFF) {
        HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    }
    else if (led_mode == LED_MODE_ON) {
        HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
    }
    else {
        if (HAL_GetTick() - blink_tick >= 500) {
            blink_tick = HAL_GetTick();
            blink_state = !blink_state;
        }
        if (blink_state)
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
        else
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    }
}

uint8_t LED_Get_Mode(void) { return led_mode; }

void LED_Set_Mode(uint8_t mode) {
    if (mode > 2) mode = 0;
    led_mode = mode;
    if (led_mode == LED_MODE_BLINK) {
        blink_tick = HAL_GetTick();
        blink_state = 0;
    }
}