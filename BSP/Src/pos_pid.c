#include "pos_pid.h"
#include "motor.h"
#include "tb6612.h"
#include "ec11.h"
#include <stdlib.h>
#include "vofa.h"
#include "motor_pid.h"
#include "cascade_pid.h"
#include "main.h"

float Kp_pos = 1.5f, Ki_pos = 0.0f, Kd_pos = 0.0f;

static int16_t Target_Pos = 0;  
static float Out_Pos = 0;       
static float Error0_pos = 0, Error1_pos = 0, ErrorInt_pos = 0;
static uint8_t pos_enable = 0;

void PosPID_Enable(uint8_t enable) {
    pos_enable = enable;
    if (!enable) {
        ErrorInt_pos = 0; 
        Error1_pos = 0; 
        TB6612_Coast();
    }
}
uint8_t PosPID_IsEnabled(void) { return pos_enable; }
void PosPID_EC11_Scan(void) {
    int8_t dir = EC11_Scan();
    if (dir == 1) {
        Target_Pos += 50; 
    } else if (dir == -1) {
        Target_Pos -= 50; 
    }
}
void PosPID_Task(void) {
    if (!pos_enable) return;

    int32_t Actual_Pos = Motor_GetTotalPulses();

    Error1_pos = Error0_pos;
    Error0_pos = Target_Pos - Actual_Pos;
    ErrorInt_pos += Error0_pos;
    if (ErrorInt_pos > 10000) ErrorInt_pos = 10000;
    if (ErrorInt_pos < -10000) ErrorInt_pos = -10000;

    Out_Pos = Kp_pos * Error0_pos + Ki_pos * ErrorInt_pos + Kd_pos * (Error0_pos - Error1_pos);

    if (Out_Pos > 100) Out_Pos = 100;
    if (Out_Pos < -100) Out_Pos = -100;

    TB6612_SetSpeed((int16_t)Out_Pos);
}

int16_t PosPID_GetTarget(void) { return Target_Pos; }
int32_t PosPID_GetActual(void) { return Motor_GetTotalPulses(); }
int16_t PosPID_GetOutput(void) { return (int16_t)Out_Pos; }

void PosPID_ClearPos(void) {
    Motor_ResetTotalPulses();
    Target_Pos = 0;
    ErrorInt_pos = 0;
    Error0_pos = 0;
    Error1_pos = 0;
}