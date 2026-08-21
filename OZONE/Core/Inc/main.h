/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

  /* Private includes ----------------------------------------------------------*/
  /* USER CODE BEGIN Includes */

  /* USER CODE END Includes */

  /* Exported types ------------------------------------------------------------*/
  /* USER CODE BEGIN ET */

  /* USER CODE END ET */

  /* Exported constants --------------------------------------------------------*/
  /* USER CODE BEGIN EC */

#define BQ25792_ADDR_WRITE 0xD6
#define BQ25792_ADDR_READ 0xD7

  /* USER CODE END EC */

  /* Exported macro ------------------------------------------------------------*/
  /* USER CODE BEGIN EM */

  // Макросы для управления пинами через LL

#define SetPin(PinName) LL_GPIO_SetOutputPin(PinName##_GPIO_Port, PinName##_Pin)
#define ResetPin(PinName) LL_GPIO_ResetOutputPin(PinName##_GPIO_Port, PinName##_Pin)
#define TogglePin(PinName) LL_GPIO_TogglePin(PinName##_GPIO_Port, PinName##_Pin)
#define ReadPin(PinName) LL_GPIO_IsOutputPinSet(PinName##_GPIO_Port, PinName##_Pin)

  /* USER CODE END EM */

  /* Exported functions prototypes ---------------------------------------------*/
  void Error_Handler(void);

  /* CubeMX-периферия: сделана extern, чтобы можно было переинициализировать
   * I2C1/USART2 из bq_compat.c (bus recovery) и boot.c (shutdown OPI). */
  void MX_I2C1_Init(void);
  void MX_USART2_UART_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EN_8Vsd_Pin LL_GPIO_PIN_0
#define EN_8Vsd_GPIO_Port GPIOA
#define EN_VPA_Pin LL_GPIO_PIN_1
#define EN_VPA_GPIO_Port GPIOA
#define PAen_Pin LL_GPIO_PIN_4
#define PAen_GPIO_Port GPIOA
#define OneW_Pin LL_GPIO_PIN_5
#define OneW_GPIO_Port GPIOA
#define EN_5Vpi_Pin LL_GPIO_PIN_6
#define EN_5Vpi_GPIO_Port GPIOA
#define PI_RCRY_Pin LL_GPIO_PIN_7
#define PI_RCRY_GPIO_Port GPIOA
#define EN_FAN_Pin LL_GPIO_PIN_0
#define EN_FAN_GPIO_Port GPIOB
#define BST_Dis_Pin LL_GPIO_PIN_1
#define BST_Dis_GPIO_Port GPIOB
#define EN_HEAT_Pin LL_GPIO_PIN_2
#define EN_HEAT_GPIO_Port GPIOB
#define UDE_Pin LL_GPIO_PIN_8
#define UDE_GPIO_Port GPIOA
#define PI_Boot_Pin LL_GPIO_PIN_11
#define PI_Boot_GPIO_Port GPIOA
#define PI_RST_Pin LL_GPIO_PIN_12
#define PI_RST_GPIO_Port GPIOA
#define INT_Pin LL_GPIO_PIN_5
#define INT_GPIO_Port GPIOB
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0 ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority, \
                                                         4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1 ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority, \
                                                         3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2 ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority, \
                                                         2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3 ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority, \
                                                         1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4 ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority, \
                                                         0 bit  for subpriority */
#endif

  /* USER CODE BEGIN Private defines */

  /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
