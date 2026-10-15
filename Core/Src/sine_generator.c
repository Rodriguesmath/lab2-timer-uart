/**
  ******************************************************************************
  * @file    sine_generator.c
  * @brief   Implementação do Gerador Senoidal via PWM + Tabela Look-Up
  *          Lab Timer e UART STM32 - Formação Virtus-CC
  ******************************************************************************
  */

#include "sine_generator.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static TIM_HandleTypeDef *p_htim_pwm = NULL;
static uint32_t s_pwm_channel = 0;
static TIM_HandleTypeDef *p_htim_sample = NULL;

static uint32_t current_freq_hz = 50; /* 50 Hz padrão inicial */
static uint16_t sine_lut[SINE_TABLE_POINTS];
static volatile uint16_t current_index = 0;

void SineGen_Init(TIM_HandleTypeDef *htim_pwm, uint32_t pwm_channel, TIM_HandleTypeDef *htim_sample) {
    p_htim_pwm = htim_pwm;
    s_pwm_channel = pwm_channel;
    p_htim_sample = htim_sample;

    /* Obtém o valor de período (ARR) do Timer PWM para escalar os pontos da senoide */
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim_pwm);

    /* 
     * Gera a tabela senoidal pré-computada:
     * Duty cycle varia de 5% a 95% do ARR para garantir linearidade nos limites.
     * y = (ARR / 2) + (ARR / 2 * 0.90) * sin(theta)
     */
    for (int i = 0; i < SINE_TABLE_POINTS; i++) {
        float theta = (2.0f * (float)M_PI * (float)i) / (float)SINE_TABLE_POINTS;
        float sin_val = sinf(theta);
        float duty = ((float)arr / 2.0f) + (((float)arr / 2.0f) * 0.90f * sin_val);
        sine_lut[i] = (uint16_t)duty;
    }

    current_index = 0;
    SineGen_SetFrequency(current_freq_hz);

    /* Inicia a geração de PWM */
    HAL_TIM_PWM_Start(p_htim_pwm, s_pwm_channel);

    /* Se houver timer de amostragem dedicado, inicia a base de tempo */
    if (p_htim_sample != NULL) {
        HAL_TIM_Base_Start_IT(p_htim_sample);
    }
}

bool SineGen_SetFrequency(uint32_t freq_hz) {
    if (freq_hz < SINE_MIN_FREQ_HZ || freq_hz > SINE_MAX_FREQ_HZ) {
        return false;
    }

    current_freq_hz = freq_hz;

    /*
     * Frequência da senoide = f_sample / SINE_TABLE_POINTS
     * Logo, f_sample = freq_hz * SINE_TABLE_POINTS
     * Se usarmos o p_htim_sample para gerar a interrupção a f_sample:
     * ARR_sample = (TimerClock / (PSC + 1) / f_sample) - 1
     */
    if (p_htim_sample != NULL) {
        /* Assume clock de timer de 100MHz ou calcula dinamicamente */
        uint32_t timer_clk = SystemCoreClock; /* Em F411 APB1/2 timer clock */
        uint32_t target_sample_rate = freq_hz * SINE_TABLE_POINTS;
        
        uint32_t psc = p_htim_sample->Init.Prescaler;
        uint32_t arr_val = (timer_clk / ((psc + 1) * target_sample_rate)) - 1;

        if (arr_val > 0xFFFF) arr_val = 0xFFFF;
        if (arr_val < 10) arr_val = 10;

        __HAL_TIM_SET_AUTORELOAD(p_htim_sample, arr_val);
        __HAL_TIM_SET_COUNTER(p_htim_sample, 0);
    }

    return true;
}

uint32_t SineGen_GetFrequency(void) {
    return current_freq_hz;
}

void SineGen_Step(void) {
    if (p_htim_pwm == NULL) return;

    /* Atualiza o comparador CCR com o valor correspondente da senoide */
    __HAL_TIM_SET_COMPARE(p_htim_pwm, s_pwm_channel, sine_lut[current_index]);

    current_index++;
    if (current_index >= SINE_TABLE_POINTS) {
        current_index = 0;
    }
}
