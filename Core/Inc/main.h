/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOC
#define KEY_EXTI_IRQn EXTI0_IRQn
#define R_Light_Pin GPIO_PIN_1
#define R_Light_GPIO_Port GPIOA
#define WIFIConect_TX_Pin GPIO_PIN_2
#define WIFIConect_TX_GPIO_Port GPIOA
#define WIFIConect_RX_Pin GPIO_PIN_3
#define WIFIConect_RX_GPIO_Port GPIOA
#define B_Light_Pin GPIO_PIN_6
#define B_Light_GPIO_Port GPIOA
#define G_Light_Pin GPIO_PIN_7
#define G_Light_GPIO_Port GPIOA
#define H7Conect_TX_Pin GPIO_PIN_10
#define H7Conect_TX_GPIO_Port GPIOB
#define H7Conect_RX_Pin GPIO_PIN_11
#define H7Conect_RX_GPIO_Port GPIOB
#define TIM_motor1_Pin GPIO_PIN_6
#define TIM_motor1_GPIO_Port GPIOC
#define TIM_motor2_Pin GPIO_PIN_8
#define TIM_motor2_GPIO_Port GPIOC
#define DAPConect_TX_Pin GPIO_PIN_9
#define DAPConect_TX_GPIO_Port GPIOA
#define DAPConect_RX_Pin GPIO_PIN_10
#define DAPConect_RX_GPIO_Port GPIOA
#define TIM_motor3_Pin GPIO_PIN_6
#define TIM_motor3_GPIO_Port GPIOB
#define TIM_motor4_Pin GPIO_PIN_8
#define TIM_motor4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

typedef void (*LoopCallBack)(void);

typedef struct LoopFuncT{
    void (*LoopCallBack)(void);
    uint8_t funcName[50];
}LoopFunc_t;
void unRegisterLoopCallBack(uint8_t * funcName);
void registerLoopCallBack(LoopFunc_t loopF);

/**
 * 设置PWM占空比
 * @param htim    : 定时器指针
 * @param Channel : 定时器通道
 *                  TIM_CHANNEL_1
 *                  TIM_CHANNEL_2
 *                  TIM_CHANNEL_3
 *                  TIM_CHANNEL_4
 * @param Pulse   : 脉宽
 */
inline void setPWMPulse(TIM_HandleTypeDef *htim,uint32_t Channel, uint32_t Pulse)
{
    switch (Channel) {
        case TIM_CHANNEL_1:
            htim->Instance->CCR1 = Pulse;
            break;
        case TIM_CHANNEL_2:
            htim->Instance->CCR2 = Pulse;
            break;
        case TIM_CHANNEL_3:
            htim->Instance->CCR3 = Pulse;
            break;
        case TIM_CHANNEL_4:
            htim->Instance->CCR4 = Pulse;
            break;

    }

}
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
