#include "motor.h"
#include "tb6612.h"
#include "encoder.h"
#include "ec11.h"

static int16_t duty = 0;
static uint8_t running = 0;
static int16_t current_rpm = 0;
static int32_t total_pulses = 0;
void Motor_Init(void) {
    duty = 0;
    running = 0;
    current_rpm = 0;
    total_pulses = 0;
    TB6612_Coast();
}

void Motor_Task(void) {
    int16_t pulses = Encoder_Get(); 
    int32_t temp = (int32_t)pulses * 227; 
    int16_t new_rpm = temp / 1000; 
    static float filtered_rpm = 0;
    filtered_rpm = (filtered_rpm * 0.8f) + (new_rpm * 0.2f);
    current_rpm = (int16_t)filtered_rpm;
    total_pulses += pulses;
}

void Motor_Key_K1(void) {
    running = !running;
    if (running) TB6612_SetSpeed((int16_t)duty);
    else TB6612_Coast();
}

void Motor_EC11_Scan(void) {
    int8_t dir = EC11_Scan();
    if (dir == 1) {
        if (duty < 100) duty += 1;
    } else if (dir == -1) {
        if (duty > 0) duty -= 1;
    }
    if (running) TB6612_SetSpeed(duty);
}

int16_t Motor_GetRPM(void) { return current_rpm; }
int16_t Motor_GetDuty(void) { return duty; }
int32_t Motor_GetTotalPulses(void) { return total_pulses; }
void Motor_ResetTotalPulses(void) { total_pulses = 0; }