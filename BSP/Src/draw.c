#include "draw.h"
#include "oled.h"
#include "font.h"
#include "password.h"
#include "led.h"
#include "servo.h"
#include "uart.h"
#include "motor.h"
#include "motor_pid.h"
#include <stdio.h>
#include "pos_pid.h"
#include "cascade_pid.h"
#include "imu.h"

extern uint8_t g_state;

void Draw_UI(void) {
    OLED_NewFrame();

    if (g_state == 0) {
        Password_Draw();
    }
    else if (g_state == 1) {
        OLED_PrintASCIIString(0, 0, "LED MODE", &afont16x8, OLED_COLOR_NORMAL);
        char buf[20];
        sprintf(buf, "Mode: %d", LED_Get_Mode());
        OLED_PrintASCIIString(0, 16, buf, &afont16x8, OLED_COLOR_NORMAL);
    }
    else if (g_state == 2) {
        OLED_PrintASCIIString(0, 0, "SERVO", &afont16x8, OLED_COLOR_NORMAL);
        char buf[20];
        sprintf(buf, "Angle: %d", Servo_GetAngle());
        OLED_PrintASCIIString(0, 16, buf, &afont16x8, OLED_COLOR_NORMAL);
    }
    else if (g_state == 3) {
        if (UART_Is_Popup()) {
            OLED_PrintASCIIString(0, 0, "UART MSG:", &afont16x8, OLED_COLOR_NORMAL);
            OLED_PrintASCIIString(0, 16, UART_Get_Popup(), &afont16x8, OLED_COLOR_NORMAL);
        } else {
            OLED_PrintASCIIString(0, 0, "UART", &afont16x8, OLED_COLOR_NORMAL);
            OLED_PrintASCIIString(0, 16, "Waiting...", &afont16x8, OLED_COLOR_NORMAL);
        }
    }
    else if (g_state == 4) {
        OLED_PrintASCIIString(0, 0, "MOTOR OPEN", &afont16x8, OLED_COLOR_NORMAL);
        char buf[20];
        sprintf(buf, "Duty: %d%%", Motor_GetDuty());
        OLED_PrintASCIIString(0, 16, buf, &afont16x8, OLED_COLOR_NORMAL);
        sprintf(buf, "RPM: %d", Motor_GetRPM());
        OLED_PrintASCIIString(0, 32, buf, &afont16x8, OLED_COLOR_NORMAL);
    }
    else if (g_state == 5) {
        if (Motor_IsStall()) {
            OLED_PrintASCIIString(0, 0, "STALL ERROR", &afont16x8, OLED_COLOR_NORMAL);
            OLED_PrintASCIIString(0, 16, "Press K1 to Reset", &afont16x8, OLED_COLOR_NORMAL);
        } else {
            OLED_PrintASCIIString(0, 0, "SPEED PID", &afont16x8, OLED_COLOR_NORMAL);
            char buf[20];
            sprintf(buf, "Set: %d", SpeedPID_GetTarget());
            OLED_PrintASCIIString(0, 16, buf, &afont16x8, OLED_COLOR_NORMAL);
            sprintf(buf, "Now: %d", Motor_GetRPM());
            OLED_PrintASCIIString(0, 32, buf, &afont16x8, OLED_COLOR_NORMAL);
            sprintf(buf, "Out: %d%%", SpeedPID_GetOutput());
            OLED_PrintASCIIString(0, 48, buf, &afont16x8, OLED_COLOR_NORMAL);
        }
    }
    else if (g_state == 6) { 
    OLED_PrintASCIIString(0, 0, "POS PID", &afont16x8, OLED_COLOR_NORMAL);
    char buf[30];
    sprintf(buf, "Tgt: %d", PosPID_GetTarget());
    OLED_PrintASCIIString(0, 16, buf, &afont16x8, OLED_COLOR_NORMAL);
    sprintf(buf, "Act: %ld", (long)PosPID_GetActual());
    OLED_PrintASCIIString(0, 32, buf, &afont16x8, OLED_COLOR_NORMAL);
    sprintf(buf, "Out: %d%%", PosPID_GetOutput());
    OLED_PrintASCIIString(0, 48, buf, &afont16x8, OLED_COLOR_NORMAL);
    }
    else if (g_state == 7) {
    OLED_PrintASCIIString(0, 0, "CASCADE PID", &afont16x8, OLED_COLOR_NORMAL);
    char buf[30];
    sprintf(buf, "TgtPos: %d", CascadePID_GetTgtPos());
    OLED_PrintASCIIString(0, 16, buf, &afont16x8, OLED_COLOR_NORMAL);
    sprintf(buf, "ActPos: %ld", (long)CascadePID_GetActPos());
    OLED_PrintASCIIString(0, 32, buf, &afont16x8, OLED_COLOR_NORMAL);
    sprintf(buf, "TgtRPM: %d", CascadePID_GetTgtRPM());
    OLED_PrintASCIIString(0, 48, buf, &afont16x8, OLED_COLOR_NORMAL);
    }
    else if (g_state == 8) {
    char buf[30];
    int r10 = (int)(IMU_GetRoll() * 10);
    int p10 = (int)(IMU_GetPitch() * 10);
    int y10 = (int)(IMU_GetYaw() * 10);
    sprintf(buf, "R:%d P:%d Y:%d", r10, p10, y10);
    OLED_PrintASCIIString(0, 16, buf, &afont16x8, OLED_COLOR_NORMAL);
    }
    
    OLED_ShowFrame();
}