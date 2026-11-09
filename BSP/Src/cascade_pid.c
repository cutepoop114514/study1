#include "cascade_pid.h"
#include "motor.h"
#include "motor_pid.h"
#include "tb6612.h"
#include "ec11.h"
#include <stdlib.h>
#include "vofa.h"
#include "pos_pid.h"
#include "main.h"
#include "stdio.h"
#include "string.h"

static float Kp_pos = 2.0f; 
#define RPM_LIMIT 2000       
#define POS_ERROR_BAND 50    
#define HOLD_TIME_MS 500   

static int16_t TgtPos = 0;
static uint8_t cascade_enable = 0;

static uint32_t hold_tick = 0;
static uint8_t is_hold = 0;

void CascadePID_Enable(uint8_t enable) {
    cascade_enable = enable;
    if (!enable) {
        hold_tick = 0; is_hold = 0;
        SpeedPID_Enable(0); 
        TB6612_Coast();
    }
}
uint8_t CascadePID_IsEnabled(void) { return cascade_enable; }

void CascadePID_EC11_Scan(void) {
    int8_t dir = EC11_Scan();
    if (dir == 1) TgtPos += 50;
    else if (dir == -1) TgtPos -= 50;
}

int16_t CascadePID_GetTgtPos(void) { return TgtPos; }
int32_t CascadePID_GetActPos(void) { return Motor_GetTotalPulses(); }
int16_t CascadePID_GetTgtRPM(void) { return SpeedPID_GetTarget(); }
int16_t CascadePID_GetActRPM(void) { return Motor_GetRPM(); }

void CascadePID_Task(void) {
    if (!cascade_enable) return;
    int32_t ActPos = Motor_GetTotalPulses();
    int32_t pos_error = TgtPos - ActPos;
    if (abs(pos_error) <= POS_ERROR_BAND) {
        if (hold_tick == 0) {
            hold_tick = HAL_GetTick();
        } else if (HAL_GetTick() - hold_tick >= HOLD_TIME_MS) {
            is_hold = 1;
    } else {
        hold_tick = 0;
        is_hold = 0;
    }
    int16_t tgt_rpm = 0;
    if (is_hold) {
        tgt_rpm = 0;
        SpeedPID_Enable(0); 
        SpeedPID_Enable(1); 
    } else {
        float calc_rpm = Kp_pos * (float)pos_error;
        if (calc_rpm > RPM_LIMIT) calc_rpm = RPM_LIMIT;
        if (calc_rpm < -RPM_LIMIT) calc_rpm = -RPM_LIMIT;
        tgt_rpm = (int16_t)calc_rpm;
    }
    SpeedPID_SetTarget(tgt_rpm);
    SpeedPID_Task();
    }
}