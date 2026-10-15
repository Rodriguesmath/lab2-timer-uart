/******************************************************************************/
/**
 * @file    Bsp.c
 * @brief   Implementacao do BSP para Lab 2 Timer e UART STM32.
 * @author  Rodrigues - Formacao em Sistemas Embarcados Virtus-CC
 * @note    Estilo de codificacao inspirado nas normas do Laboratorio ASSERT (IFPB).
 * @{
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "Bsp.h"
#include <string.h>

/*******************************************************************************
 * DEFINES LOCAIS (fixos, apenas auxiliar para calculos)
 ******************************************************************************/
#define dBSP_PWM_PERIOD_50KHZ             1999
#define dBSP_SAMPLE_PSC_1MHZ              99

/*******************************************************************************
 * CONSTANTES
 ******************************************************************************/

/*******************************************************************************
 * ESTRUTURAS DE DADOS LOCAIS
 ******************************************************************************/
static struct
{
    TIM_HandleTypeDef  htimPwm;
    TIM_HandleTypeDef  htimSample;
    UART_HandleTypeDef huart;
    bool isInitialized;
} bsp;

/*******************************************************************************
 * PROTOTIPOS LOCAIS
 ******************************************************************************/
static void Bsp_SystemClockConfig(void);
static void Bsp_GpioInit(void);
static void Bsp_TimPwmInit(void);
static void Bsp_TimSampleInit(void);
static void Bsp_UartInit(void);

/*******************************************************************************
 * FUNCOES PUBLICAS
 ******************************************************************************/

void Bsp_Init(void)
{
    HAL_Init();
    Bsp_SystemClockConfig();
    Bsp_GpioInit();
    Bsp_TimPwmInit();
    Bsp_TimSampleInit();
    Bsp_UartInit();

    // Inicia geracao do sinal PWM no canal 1
    HAL_TIM_PWM_Start(&bsp.htimPwm, TIM_CHANNEL_1);

    // Inicia a base de tempo por interrupcao do timer de amostragem
    HAL_TIM_Base_Start_IT(&bsp.htimSample);

    bsp.isInitialized = true;
}

void Bsp_LedToggle(void)
{
    HAL_GPIO_TogglePin(dBSP_LED_PORT, dBSP_LED_PIN);
}

void Bsp_PwmSetCompare(u16 pulse)
{
    __HAL_TIM_SET_COMPARE(&bsp.htimPwm, TIM_CHANNEL_1, pulse);
}

u16 Bsp_PwmGetPeriod(void)
{
    return (u16)__HAL_TIM_GET_AUTORELOAD(&bsp.htimPwm);
}

void Bsp_SampleTimerSetPeriod(u16 period)
{
    __HAL_TIM_SET_AUTORELOAD(&bsp.htimSample, period);
    __HAL_TIM_SET_COUNTER(&bsp.htimSample, 0);
}

void Bsp_UartTransmit(const char *pStr)
{
    if(pStr != NULL)
    {
        HAL_UART_Transmit(&bsp.huart, (uint8_t *)pStr, (uint16_t)strlen(pStr), 100);
    }
}

void Bsp_UartStartReceiveIt(u8 *pChar)
{
    HAL_UART_Receive_IT(&bsp.huart, pChar, 1);
}

/*******************************************************************************
 * FUNCOES LOCAIS
 ******************************************************************************/

static void Bsp_SystemClockConfig(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

#if defined(STM32F411xE)
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
#endif
}

static void Bsp_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* LED On-Board */
    HAL_GPIO_WritePin(dBSP_LED_PORT, dBSP_LED_PIN, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = dBSP_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(dBSP_LED_PORT, &GPIO_InitStruct);

    /* PA8: TIM1_CH1 PWM */
    GPIO_InitStruct.Pin = dBSP_PWM_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(dBSP_PWM_PORT, &GPIO_InitStruct);

    /* PA9, PA10: USART1 TX/RX */
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void Bsp_TimPwmInit(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    bsp.htimPwm.Instance = TIM1;
    bsp.htimPwm.Init.Prescaler = 0;
    bsp.htimPwm.Init.CounterMode = TIM_COUNTERMODE_UP;
    bsp.htimPwm.Init.Period = dBSP_PWM_PERIOD_50KHZ; // 100MHz / 2000 = 50 kHz
    bsp.htimPwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    bsp.htimPwm.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&bsp.htimPwm);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = dBSP_PWM_PERIOD_50KHZ / 2;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&bsp.htimPwm, &sConfigOC, TIM_CHANNEL_1);
}

static void Bsp_TimSampleInit(void)
{
    bsp.htimSample.Instance = TIM2;
    bsp.htimSample.Init.Prescaler = dBSP_SAMPLE_PSC_1MHZ;
    bsp.htimSample.Init.CounterMode = TIM_COUNTERMODE_UP;
    bsp.htimSample.Init.Period = 155; // Inicial para 50 Hz
    bsp.htimSample.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    bsp.htimSample.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&bsp.htimSample);

    HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

static void Bsp_UartInit(void)
{
    bsp.huart.Instance = USART1;
    bsp.huart.Init.BaudRate = dBSP_UART_BAUDRATE;
    bsp.huart.Init.WordLength = UART_WORDLENGTH_8B;
    bsp.huart.Init.StopBits = UART_STOPBITS_1;
    bsp.huart.Init.Parity = UART_PARITY_NONE;
    bsp.huart.Init.Mode = UART_MODE_TX_RX;
    bsp.huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    bsp.huart.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&bsp.huart);

    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/** @} DOXYGEN GROUP TAG END OF FILE */
