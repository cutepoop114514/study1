#include "encoder.h"

static int32_t pulse_count = 0;
static uint8_t last_state = 0;

void Encoder_Scan(void) {
    uint8_t a = HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin);
    uint8_t b = HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_Pin);
    uint8_t current_state = (a << 1) | b;
    
    if (last_state != current_state) {
        if ((last_state == 0b00 && current_state == 0b10) ||
            (last_state == 0b10 && current_state == 0b11) ||
            (last_state == 0b11 && current_state == 0b01) ||
            (last_state == 0b01 && current_state == 0b00)) {
            pulse_count++;
        } 
        else if ((last_state == 0b00 && current_state == 0b01) ||
                 (last_state == 0b01 && current_state == 0b11) ||
                 (last_state == 0b11 && current_state == 0b10) ||
                 (last_state == 0b10 && current_state == 0b00)) {
            pulse_count--;
        }
        last_state = current_state;
    }
}

int16_t Encoder_Get(void) {
    int16_t result = (int16_t)pulse_count;
    pulse_count = 0; 
    return result;
}