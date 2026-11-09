/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "key.h"
#include "ec11.h"
#include "encoder.h"
#include "tb6612.h"
#include "password.h"
#include "servo.h"
#include "uart.h"
#include "motor.h"
#include "motor_pid.h"
#include "draw.h"
#include "oled.h"
#include "font.h"
#include <stdio.h>
#include <string.h>
#include "vofa.h"
#include "pos_pid.h"
#include "cascade_pid.h"
#include "imu.h"
#include "Amadeus_Logo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    SYS_LOCK = 0,    
    SYS_LED,
    SYS_SERVO,         
    SYS_UART,          
    SYS_MOTOR,         
    SYS_SPEED_PID,    
    SYS_POS_PID,        
    SYS_CASCADE_PID,
    SYS_IMU    
} SystemState;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
SystemState g_state = SYS_LOCK;
static uint8_t last_k1 = 1, last_k2 = 1, last_k3 = 1;
static uint32_t key_tick = 0;
static uint32_t oled_tick = 0;
static uint32_t key_lock_tick = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Key_Scan(void) {
    if (HAL_GetTick() - key_lock_tick < 200) return;
    
    if (HAL_GetTick() - key_tick < 10) return;
    key_tick = HAL_GetTick();

    uint8_t k1 = Key_Get_K1();
    uint8_t k2 = Key_Get_K2();
    uint8_t k3 = Key_Get_K3();

    if (last_k1 == 1 && k1 == 0) {
        if (g_state == SYS_LOCK) Password_Key_K1();
        else if (g_state == SYS_LED) LED_Key_K1();
        else if (g_state == SYS_SERVO) Servo_Key_K1();
        else if (g_state == SYS_UART) UART_Key_K1();
        else if (g_state == SYS_MOTOR) Motor_Key_K1();
        else if (g_state == SYS_SPEED_PID) {if (Motor_IsStall()) Motor_ClearStall();else SpeedPID_Enable(!SpeedPID_IsEnabled());}
        else if (g_state == SYS_POS_PID) {PosPID_ClearPos();}
        else if (g_state == SYS_CASCADE_PID) {CascadePID_Enable(!CascadePID_IsEnabled());}
    }

    static uint32_t k2_press_tick = 0;
    static uint8_t  k2_long_triggered = 0;
    if (k2 == 0) {
        if (k2_press_tick == 0) k2_press_tick = HAL_GetTick();
        else if (!k2_long_triggered && (HAL_GetTick() - k2_press_tick >= 2000)) {
            if (g_state == SYS_MOTOR) {
                g_state = SYS_SPEED_PID; SpeedPID_Enable(0);
            } else if (g_state == SYS_SPEED_PID || g_state == SYS_POS_PID) {
                g_state = SYS_MOTOR; Motor_Init();
            }
            key_lock_tick = HAL_GetTick(); 
            k2_long_triggered = 1;
        }
    }else{
        if (k2_press_tick > 0 && !k2_long_triggered) {
            if (g_state == SYS_LOCK) Password_Key_K2();
            else if (g_state == SYS_LED) { g_state = SYS_SERVO; key_lock_tick = HAL_GetTick(); }
            else if (g_state == SYS_SERVO) { g_state = SYS_UART; key_lock_tick = HAL_GetTick(); }
            else if (g_state == SYS_UART) { g_state = SYS_MOTOR; Motor_Init(); key_lock_tick = HAL_GetTick(); }
            else if (g_state == SYS_MOTOR) { g_state = SYS_SPEED_PID; SpeedPID_Enable(0); key_lock_tick = HAL_GetTick(); }
            else if (g_state == SYS_SPEED_PID) { g_state = SYS_POS_PID; PosPID_Enable(0); key_lock_tick = HAL_GetTick(); }
            else if (g_state == SYS_POS_PID) { g_state = SYS_CASCADE_PID; Motor_Init(); key_lock_tick = HAL_GetTick(); }
            else if (g_state == SYS_CASCADE_PID) { g_state = SYS_IMU; Motor_Init(); key_lock_tick = HAL_GetTick(); }
            else if (g_state == SYS_IMU) { g_state = SYS_LED; key_lock_tick = HAL_GetTick(); }
        }
        k2_press_tick = 0;
        k2_long_triggered = 0;
    }

    if (last_k3 == 1 && k3 == 0) {
        if (g_state == SYS_LOCK) {
            Password_Key_K3();
        } else {
            Password_Lock();
            g_state = SYS_LOCK;
            key_lock_tick = HAL_GetTick(); 
        }
    }

    last_k1 = k1; last_k2 = k2; last_k3 = k3;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(20);
  OLED_Init();
  OLED_NewFrame();     
  OLED_DrawImage(0, 0, &Amadeus_Logo, OLED_COLOR_NORMAL); 
  OLED_ShowFrame();    
  HAL_Delay(2000);     
  OLED_NewFrame();
  OLED_ShowFrame();
  Password_Init();
  LED_Init();
  Servo_Init();
  UART_Init();
  TB6612_Init();
  Motor_Init();
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  __HAL_TIM_SET_COUNTER(&htim1, 0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    Encoder_Scan();
    Key_Scan();
    Password_Task();
    UART_Task();
    LED_Task();
    
    if (Password_IsUnlocked() && g_state == SYS_LOCK) {
        g_state = SYS_LED;
    }
    if (g_state == SYS_LED) {
        LED_Task();
    }
    else if (g_state == SYS_SERVO) {
        int8_t dir = EC11_Scan();
        if (dir == 1) Servo_SetAngle(Servo_GetAngle() + 5);
        else if (dir == -1) Servo_SetAngle(Servo_GetAngle() - 5);
    }
    else if (g_state == SYS_MOTOR) {
        Motor_EC11_Scan();
    }
    else if (g_state == SYS_SPEED_PID) {
        SpeedPID_EC11_Scan();
    }
    else if (g_state == SYS_POS_PID) {
        PosPID_EC11_Scan();
    }
    else if (g_state == SYS_CASCADE_PID) {
        CascadePID_EC11_Scan();
    }

    if (HAL_GetTick() - oled_tick >= 200) {
        oled_tick = HAL_GetTick();
        if (g_state == SYS_MOTOR) {
            Motor_Task();
        } else if (g_state == SYS_SPEED_PID) {
            Motor_Task();
            SpeedPID_Task();
            Motor_CheckStall();
            Vofa_SendWave(SpeedPID_GetTarget(),Motor_GetRPM(), SpeedPID_GetOutput(), SpeedPID_GetTarget() - Motor_GetRPM());
        }else if (g_state == SYS_POS_PID) {
            Motor_Task();
            PosPID_Task(); 
            Motor_CheckStall();
        } else if (g_state == SYS_CASCADE_PID) {
            Motor_Task();
            CascadePID_Task();
            Motor_CheckStall();
            Vofa_SendWave(CascadePID_GetTgtPos(), (int16_t)CascadePID_GetActPos(), CascadePID_GetTgtRPM(), CascadePID_GetActRPM());
        }else if (g_state == SYS_IMU) {
             IMU_Init();
            static uint32_t imu_tick = 0;
            if (g_state == SYS_IMU && (HAL_GetTick() - imu_tick >= 100)) {
                imu_tick = HAL_GetTick();
                Vofa_SendIMU(IMU_GetRoll(), IMU_GetPitch(), IMU_GetYaw());
            }  
        } 

    Draw_UI();
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
   } 
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
