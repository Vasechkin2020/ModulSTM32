/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define Analiz1_Pin GPIO_PIN_13
#define Analiz1_GPIO_Port GPIOC
#define Step_Motor0_Pin GPIO_PIN_1
#define Step_Motor0_GPIO_Port GPIOC
#define Dir_Motor0_Pin GPIO_PIN_2
#define Dir_Motor0_GPIO_Port GPIOC
#define micMotor0_Pin GPIO_PIN_3
#define micMotor0_GPIO_Port GPIOC
#define micMotor0_EXTI_IRQn EXTI2_3_IRQn
#define micMotor1_Pin GPIO_PIN_0
#define micMotor1_GPIO_Port GPIOB
#define micMotor1_EXTI_IRQn EXTI0_1_IRQn
#define Step_Motor1_Pin GPIO_PIN_1
#define Step_Motor1_GPIO_Port GPIOB
#define Dir_Motor1_Pin GPIO_PIN_2
#define Dir_Motor1_GPIO_Port GPIOB
#define Step_Motor2_Pin GPIO_PIN_10
#define Step_Motor2_GPIO_Port GPIOB
#define Dir_Motor2_Pin GPIO_PIN_11
#define Dir_Motor2_GPIO_Port GPIOB
#define micMotor2_Pin GPIO_PIN_12
#define micMotor2_GPIO_Port GPIOB
#define micMotor2_EXTI_IRQn EXTI4_15_IRQn
#define micMotor3_Pin GPIO_PIN_13
#define micMotor3_GPIO_Port GPIOB
#define micMotor3_EXTI_IRQn EXTI4_15_IRQn
#define Dir_Motor3_Pin GPIO_PIN_14
#define Dir_Motor3_GPIO_Port GPIOB
#define Step_Motor3_Pin GPIO_PIN_15
#define Step_Motor3_GPIO_Port GPIOB
#define En_Motor_Pin GPIO_PIN_8
#define En_Motor_GPIO_Port GPIOA
#define Led2_Pin GPIO_PIN_6
#define Led2_GPIO_Port GPIOC
#define Led1_Pin GPIO_PIN_7
#define Led1_GPIO_Port GPIOC
#define Analiz2_Pin GPIO_PIN_10
#define Analiz2_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
