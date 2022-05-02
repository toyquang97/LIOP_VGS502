/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define HC595_SEL_Pin GPIO_PIN_1
#define HC595_SEL_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_5
#define SCK_GPIO_Port GPIOA
#define LED4_Pin GPIO_PIN_6
#define LED4_GPIO_Port GPIOA
#define SDO_Pin GPIO_PIN_7
#define SDO_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_0
#define R1_GPIO_Port GPIOB
#define R2_Pin GPIO_PIN_1
#define R2_GPIO_Port GPIOB
#define R3_Pin GPIO_PIN_2
#define R3_GPIO_Port GPIOB
#define UK_IN_Pin GPIO_PIN_11
#define UK_IN_GPIO_Port GPIOB
#define DK_IN_Pin GPIO_PIN_12
#define DK_IN_GPIO_Port GPIOB
#define FIRE_IN_Pin GPIO_PIN_13
#define FIRE_IN_GPIO_Port GPIOB
#define LOCK_IN_Pin GPIO_PIN_14
#define LOCK_IN_GPIO_Port GPIOB
#define NODE_ID_Pin GPIO_PIN_15
#define NODE_ID_GPIO_Port GPIOB
#define RESET_LED1_Pin GPIO_PIN_8
#define RESET_LED1_GPIO_Port GPIOA
#define RESET_LED2_Pin GPIO_PIN_9
#define RESET_LED2_GPIO_Port GPIOA
#define R4_Pin GPIO_PIN_3
#define R4_GPIO_Port GPIOB
#define DO_Pin GPIO_PIN_4
#define DO_GPIO_Port GPIOB
#define UO_Pin GPIO_PIN_5
#define UO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define SCK (*(volatile uint32_t *)0x42210194)
#define PIN_A6 (*(volatile uint32_t *)0x42210118)  //i
#define PIN_A7 (*(volatile uint32_t *)0x4221019C)
#define HC959_SEl (*(volatile uint32_t *)0x42210184)  //PA1_O


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
