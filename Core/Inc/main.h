/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header do Lab Timer e UART STM32 - Inversor Senoidal
  *                   Virtus-CC - Curso 2
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined(STM32F411xE) || defined(STM32F401xC)
  #include "stm32f4xx_hal.h"
#elif defined(STM32H723xx)
  #include "stm32h7xx_hal.h"
#elif defined(STM32F767xx)
  #include "stm32f7xx_hal.h"
#else
  #include "stm32f4xx_hal.h"
#endif

void Error_Handler(void);

/* Definições de Hardware */
#define PWM_OUT_Pin             GPIO_PIN_8
#define PWM_OUT_GPIO_Port         GPIOA

/* LED de Status */
#if defined(STM32F411xE)
  #define LED_Pin                 GPIO_PIN_13
  #define LED_GPIO_Port           GPIOC
#elif defined(STM32F767xx)
  #define LED_Pin                 GPIO_PIN_0
  #define LED_GPIO_Port           GPIOB
#else
  #define LED_Pin                 GPIO_PIN_13
  #define LED_GPIO_Port           GPIOC
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
