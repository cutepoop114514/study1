#include "motor_pid.h"
#include "motor.h"
#include "tb6612.h"
#include "ec11.h"

float Kp = 0.501f, Ki = 0.16f, Kd = 0.00001f;
static int16_t Target = 0;
static float   Out = 0;
static float Error0 = 0, Error1 = 0, ErrorInt = 0;
static uint8_t pid_enable = 0;
static uint32_t stall_tick = 0;
static uint8_t stall_flag = 0;

void SpeedPID_Enable(uint8_t enable) {
    pid_enable = enable;
    if (!enable) {
        ErrorInt = 0;
        Error1 = 0;
        TB6612_Coast();
    }
}

uint8_t SpeedPID_IsEnabled(void) { return pid_enable; }

void SpeedPID_EC11_Scan(void) {
    int8_t dir = EC11_Scan();
    if (dir == 1) {
        if (Target < 100) Target += 5;
    } else if (dir == -1) {
        if (Target > -100) Target -= 5;
    }
}

void SpeedPID_Task(void) {
    if (!pid_enable) return;

    int16_t Actual = Motor_GetRPM();

    Error1 = Error0;
    Error0 = Target - Actual;
    ErrorInt += Error0;

    if (ErrorInt > 1000) ErrorInt = 1000;
    if (ErrorInt < -1000) ErrorInt = -1000;

    Out = Kp * Error0 + Ki * ErrorInt + Kd * (Error0 - Error1);

    if (Out > 100) Out = 100;
    if (Out < -100) Out = -100;

    if(Out!=0) TB6612_SetSpeed((int16_t)Out);
    else TB6612_Coast();
}

int16_t SpeedPID_GetTarget(void) { return Target; }
int16_t SpeedPID_GetOutput(void) { return (int16_t)Out; }

void Motor_CheckStall(void) {
    if (stall_flag) return;
    if (Out > 80 && Motor_GetRPM() < 50 && Motor_GetRPM() > -50) {
        if (stall_tick == 0) stall_tick = HAL_GetTick();
        else if (HAL_GetTick() - stall_tick >= 500) {
            stall_flag = 1;
            SpeedPID_Enable(0);
            TB6612_Coast();
        }
    } else {
        stall_tick = 0;
    }
}
void SpeedPID_SetTarget(int16_t target) {
    Target = target;
}

void Motor_ClearStall(void) { stall_flag = 0; stall_tick = 0; }
uint8_t Motor_IsStall(void) { return stall_flag; }