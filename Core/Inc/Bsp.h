/******************************************************************************/
/**
 * @file    Bsp.h
 * @brief   Board Support Package para Lab 2 Timer e UART STM32.
 * @author  Rodrigues - Formacao em Sistemas Embarcados Virtus-CC
 * @note    Estilo de codificacao inspirado nas normas do Laboratorio ASSERT (IFPB).
 * @{
 ******************************************************************************/
#ifndef _BSP_H_
#define _BSP_H_

/*******************************************************************************
 * INCLUDES NECESSARIOS
 ******************************************************************************/
#include "AssertTypes.h"

#if defined(STM32F411xE) || defined(STM32F401xC)
  #include "stm32f4xx_hal.h"
#elif defined(STM32H723xx)
  #include "stm32h7xx_hal.h"
#elif defined(STM32F767xx)
  #include "stm32f7xx_hal.h"
#else
  #include "stm32f4xx_hal.h"
#endif

/*******************************************************************************
 * CONFIGURACOES
 ******************************************************************************/
#define dBSP_UART_BAUDRATE                115200

/*******************************************************************************
 * DEFINES PUBLICOS
 ******************************************************************************/
#define dBSP_PWM_PIN                      GPIO_PIN_8
#define dBSP_PWM_PORT                     GPIOA

#if defined(STM32F411xE)
  #define dBSP_LED_PIN                    GPIO_PIN_13
  #define dBSP_LED_PORT                   GPIOC
#elif defined(STM32F767xx)
  #define dBSP_LED_PIN                    GPIO_PIN_0
  #define dBSP_LED_PORT                   GPIOB
#else
  #define dBSP_LED_PIN                    GPIO_PIN_13
  #define dBSP_LED_PORT                   GPIOC
#endif

/*******************************************************************************
 * PROTOTIPOS PUBLICOS
 ******************************************************************************/
void Bsp_Init(void);
void Bsp_LedToggle(void);
void Bsp_PwmSetCompare(u16 pulse);
u16  Bsp_PwmGetPeriod(void);
void Bsp_SampleTimerSetPeriod(u16 period);
void Bsp_UartTransmit(const char *pStr);
bool Bsp_UartReceiveChar(u8 *pChar);
void Bsp_UartStartReceiveIt(u8 *pChar);

#endif /* _BSP_H_ */
/** @} DOXYGEN GROUP TAG END OF FILE */
