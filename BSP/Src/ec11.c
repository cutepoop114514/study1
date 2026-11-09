#include "ec11.h"
#include "tim.h"

int8_t EC11_Scan(void) {
    int16_t cnt = (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    if (cnt > 0) return 1;
    else if (cnt < 0) return -1;
    return 0;
}