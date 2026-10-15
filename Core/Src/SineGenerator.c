/******************************************************************************/
/**
 * @file    SineGenerator.c
 * @brief   Implementacao do gerador senoidal com tabela Look-Up Table e timer.
 * @author  Rodrigues - Formacao em Sistemas Embarcados Virtus-CC
 * @note    Estilo de codificacao inspirado nas normas do Laboratorio ASSERT (IFPB).
 * @{
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "SineGenerator.h"
#include "Bsp.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*******************************************************************************
 * DEFINES LOCAIS (fixos, apenas auxiliar para calculos)
 ******************************************************************************/
#define dDEFAULT_FREQ_HZ                  50
#define dSAMPLE_CLOCK_HZ                  1000000UL // 1 MHz clock do Timer 2

/*******************************************************************************
 * CONSTANTES
 ******************************************************************************/

/*******************************************************************************
 * ESTRUTURAS DE DADOS LOCAIS
 ******************************************************************************/
static struct
{
    u32 currentFreqHz;
    u16 sineLut[dSINE_GENERATOR_TABLE_POINTS];
    volatile u16 currentIndex;
} sineGenerator;

/*******************************************************************************
 * PROTOTIPOS LOCAIS
 ******************************************************************************/

/*******************************************************************************
 * FUNCOES PUBLICAS
 ******************************************************************************/

void SineGenerator_Init(void)
{
    u16 arr = Bsp_PwmGetPeriod();

    // Gera a tabela senoidal escalada para o periodo ARR (variando entre 5% e 95% do duty)
    for(u16 i = 0; i < dSINE_GENERATOR_TABLE_POINTS; i++)
    {
        float theta = (2.0f * (float)M_PI * (float)i) / (float)dSINE_GENERATOR_TABLE_POINTS;
        float sinVal = sinf(theta);
        float duty = ((float)arr / 2.0f) + (((float)arr / 2.0f) * 0.90f * sinVal);
        sineGenerator.sineLut[i] = (u16)duty;
    }

    sineGenerator.currentIndex = 0;
    SineGenerator_SetFrequency(dDEFAULT_FREQ_HZ);
}

bool SineGenerator_SetFrequency(u32 freqHz)
{
    if((freqHz < dSINE_GENERATOR_MIN_FREQ_HZ) || (freqHz > dSINE_GENERATOR_MAX_FREQ_HZ))
    {
        return false;
    }

    sineGenerator.currentFreqHz = freqHz;

    // Calcula periodo do timer de amostragem
    u32 targetSampleRate = freqHz * dSINE_GENERATOR_TABLE_POINTS;
    u32 arrVal = (dSAMPLE_CLOCK_HZ / targetSampleRate) - 1;

    if(arrVal > 0xFFFF)
    {
        arrVal = 0xFFFF;
    }
    if(arrVal < 10)
    {
        arrVal = 10;
    }

    Bsp_SampleTimerSetPeriod((u16)arrVal);
    return true;
}

u32 SineGenerator_GetFrequency(void)
{
    return sineGenerator.currentFreqHz;
}

void SineGenerator_Step(void)
{
    Bsp_PwmSetCompare(sineGenerator.sineLut[sineGenerator.currentIndex]);

    sineGenerator.currentIndex++;
    if(sineGenerator.currentIndex >= dSINE_GENERATOR_TABLE_POINTS)
    {
        sineGenerator.currentIndex = 0;
    }
}

/*******************************************************************************
 * FUNCOES LOCAIS
 ******************************************************************************/

/** @} DOXYGEN GROUP TAG END OF FILE */
