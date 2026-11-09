#include "password.h"
#include "oled.h"
#include "font.h"
#include "led.h"
#include "servo.h"
#include "tb6612.h"
#include "motor.h"

#define PWD_LEN 5

static uint8_t password[PWD_LEN] = {1, 2, 3, 4, 5};
static uint8_t input[PWD_LEN]    = {0, 0, 0, 0, 0};
static uint8_t pos = 0;
static uint8_t unlocked = 0;
static uint8_t error_flag = 0;
static uint32_t error_tick = 0;

void Password_Init(void) {
    for (int i = 0; i < PWD_LEN; i++) input[i] = 0;
    pos = 0;
    unlocked = 0;
    error_flag = 0;
}

void Password_Key_K1(void) {
    if (error_flag) return;
    pos = (pos + 1) % PWD_LEN;
}

void Password_Key_K2(void) {
    if (error_flag) return;
    input[pos] = (input[pos] + 1) % 10;
}

void Password_Key_K3(void) {
    if (error_flag) return;
    uint8_t ok = 1;
    for (int i = 0; i < PWD_LEN; i++) {
        if (input[i] != password[i]) { ok = 0; break; }
    }
    if (ok) {
        unlocked = 1;
        OLED_NewFrame();
    } else {
        error_flag = 1;
        error_tick = HAL_GetTick();
    }
}

void Password_Task(void) {
    if (error_flag) {
        if (HAL_GetTick() - error_tick >= 2000) {
            error_flag = 0;
            for (int i = 0; i < PWD_LEN; i++) input[i] = 0;
            pos = 0;
        }
    }
}

void Password_Draw(void) {
    char buf[20];
    if (error_flag) {
        OLED_PrintASCIIString(0, 0, "PASSWORD", &afont16x8, OLED_COLOR_NORMAL);
        OLED_PrintASCIIString(0, 16, "ERROR", &afont16x8, OLED_COLOR_NORMAL);
    } else {
        OLED_PrintASCIIString(0, 0, "PASSWORD", &afont16x8, OLED_COLOR_NORMAL);
        char *title = "Input: ";
        for (int i = 0; i < 7; i++) buf[i] = title[i];
        for (int i = 0; i < PWD_LEN; i++) buf[7 + i] = '0' + input[i];
        buf[12] = '\0';
        OLED_PrintASCIIString(0, 16, buf, &afont16x8, OLED_COLOR_NORMAL);
        uint8_t x = 7 * 8 + pos * 8;
        OLED_DrawLine(x, 32, x + 7, 32, OLED_COLOR_NORMAL);
    }
}

uint8_t Password_IsUnlocked(void) {
    return unlocked;
}

void Password_Lock(void) {
    unlocked = 0;
    error_flag = 0;
    pos = 0;
    for (int i = 0; i < PWD_LEN; i++) input[i] = 0;

    LED_Off();
    TB6612_Coast();
    Motor_Init();
    Servo_SetAngle(90);

    OLED_NewFrame();
}

uint8_t Password_Change(char *old_pwd, char *new_pwd) {
    for (int i = 0; i < PWD_LEN; i++) {
        if ((old_pwd[i] - '0') != password[i]) return 0;
    }
    for (int i = 0; i < PWD_LEN; i++) {
        password[i] = new_pwd[i] - '0';
    }
    return 1;
}