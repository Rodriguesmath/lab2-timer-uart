/* USER CODE BEGIN Header */
/******************************************************************************/
/**
 * @file    main.c
 * @brief   Inversor senoidal via PWM filtrado e interface UART.
 * @author  Rodrigues - Formacao em Sistemas Embarcados Virtus-CC
 * @note    Estilo de codificacao inspirado nas normas do Laboratorio ASSERT (IFPB).
 ******************************************************************************/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "Bsp.h"
#include "SineGenerator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private defines -----------------------------------------------------------*/
#define dCMD_BUFFER_SIZE          64

/* Private variables ---------------------------------------------------------*/
static u8 rxChar;
static char cmdBuffer[dCMD_BUFFER_SIZE];
static u8 cmdIndex = 0;

/* Private function prototypes -----------------------------------------------*/
static void Process_UartCommand(char *pCmd);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* Inicializacao do Hardware via BSP */
    Bsp_Init();

    /* USER CODE BEGIN 2 */
    SineGenerator_Init();

    Bsp_UartTransmit("\r\n======================================================\r\n");
    Bsp_UartTransmit("   Virtus-CC - Lab Timer e UART: Inversor Senoidal    \r\n");
    Bsp_UartTransmit("   Ajuste de frequencia por PWM + Filtro Passa-Baixas \r\n");
    Bsp_UartTransmit("======================================================\r\n> ");

    Bsp_UartStartReceiveIt(&rxChar);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1)
    {
        Bsp_LedToggle();
        HAL_Delay(500);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

static void Process_UartCommand(char *pCmd)
{
    char response[128];
    char *pNewline = strpbrk(pCmd, "\r\n");
    if(pNewline != NULL)
    {
        *pNewline = '\0';
    }

    if(strncmp(pCmd, "FREQ ", 5) == 0)
    {
        u32 val = (u32)atoi(pCmd + 5);
        if(SineGenerator_SetFrequency(val) == true)
        {
            snprintf(response, sizeof(response), "\r\n[OK] Frequencia ajustada para %lu Hz.\r\n> ", val);
        }
        else
        {
            snprintf(response, sizeof(response), "\r\n[ERRO] Limite invalido (%d a %d Hz).\r\n> ", 
                     dSINE_GENERATOR_MIN_FREQ_HZ, dSINE_GENERATOR_MAX_FREQ_HZ);
        }
        Bsp_UartTransmit(response);
    }
    else if(strcmp(pCmd, "STATUS") == 0)
    {
        snprintf(response, sizeof(response), "\r\n[STATUS] Frequencia atual: %lu Hz | Filtro RC fc ~1591 Hz\r\n> ", 
                 SineGenerator_GetFrequency());
        Bsp_UartTransmit(response);
    }
    else if(strcmp(pCmd, "HELP") == 0)
    {
        Bsp_UartTransmit("\r\n--- COMANDOS DISPONIVEIS ---\r\n"
                         "FREQ <val> : Ajusta a frequencia em Hz (ex: FREQ 60)\r\n"
                         "STATUS     : Exibe os parametros atuais\r\n"
                         "HELP       : Exibe este menu\r\n> ");
    }
    else if(strlen(pCmd) > 0)
    {
        Bsp_UartTransmit("\r\n[ERRO] Comando nao reconhecido. Digite HELP.\r\n> ");
    }
}

/**
  * @brief Interrupcao de amostragem do Timer
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        SineGenerator_Step();
    }
}

/**
  * @brief Interrupcao de caractere UART
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        Bsp_UartTransmit((const char *)&rxChar);

        if((rxChar == '\r') || (rxChar == '\n'))
        {
            cmdBuffer[cmdIndex] = '\0';
            if(cmdIndex > 0)
            {
                Process_UartCommand(cmdBuffer);
            }
            else
            {
                Bsp_UartTransmit("\r\n> ");
            }
            cmdIndex = 0;
        }
        else if((rxChar == '\b') || (rxChar == 127))
        {
            if(cmdIndex > 0)
            {
                cmdIndex--;
            }
        }
        else
        {
            if(cmdIndex < (dCMD_BUFFER_SIZE - 1))
            {
                cmdBuffer[cmdIndex++] = (char)rxChar;
            }
        }

        Bsp_UartStartReceiveIt(&rxChar);
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while(1)
    {
    }
}
