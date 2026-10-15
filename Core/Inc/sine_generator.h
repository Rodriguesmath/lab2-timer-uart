/**
  ******************************************************************************
  * @file    sine_generator.h
  * @brief   Gerador de Onda Senoidal por PWM com Look-Up Table
  *          Lab Timer e UART STM32 - Formação Virtus-CC
  ******************************************************************************
  */

#ifndef __SINE_GENERATOR_H__
#define __SINE_GENERATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define SINE_TABLE_POINTS   128  /**< 128 amostras por período (excelente resolução e baixo THD) */
#define SINE_MIN_FREQ_HZ    1    /**< Frequência mínima permitida */
#define SINE_MAX_FREQ_HZ    300  /**< Frequência máxima em que o filtro RC (1.59kHz) permanece efetivo */

/**
 * @brief Inicializa a tabela senoidal e configura os timers
 * @param htim_pwm Ponteiro para o handle do Timer gerador de PWM (ex: htim1)
 * @param pwm_channel Canal do Timer PWM (ex: TIM_CHANNEL_1)
 * @param htim_sample Ponteiro para o handle do Timer base de tempo de amostragem
 */
void SineGen_Init(TIM_HandleTypeDef *htim_pwm, uint32_t pwm_channel, TIM_HandleTypeDef *htim_sample);

/**
 * @brief Ajusta a frequência da senoide de saída
 * @param freq_hz Frequência em Hertz (1 a 300 Hz)
 * @return true se a frequência está dentro dos limites de operação
 */
bool SineGen_SetFrequency(uint32_t freq_hz);

/**
 * @brief Retorna a frequência atual configurada
 */
uint32_t SineGen_GetFrequency(void);

/**
 * @brief Atualiza o ciclo de trabalho (CCR) do PWM com o próximo ponto senoidal
 *        Invocado na interrupção do Timer de amostragem
 */
void SineGen_Step(void);

#ifdef __cplusplus
}
#endif

#endif /* __SINE_GENERATOR_H__ */
