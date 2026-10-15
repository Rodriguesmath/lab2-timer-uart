/******************************************************************************/
/**
 * @file    SineGenerator.h
 * @brief   Sintetizador senoidal via PWM com tabela de consulta (Look-Up Table).
 * @author  Rodrigues - Formacao em Sistemas Embarcados Virtus-CC
 * @note    Estilo de codificacao inspirado nas normas do Laboratorio ASSERT (IFPB).
 * @{
 ******************************************************************************/
#ifndef _SINE_GENERATOR_H_
#define _SINE_GENERATOR_H_

/*******************************************************************************
 * INCLUDES NECESSARIOS
 ******************************************************************************/
#include "AssertTypes.h"

/*******************************************************************************
 * CONFIGURACOES
 ******************************************************************************/
/// Quantidade de amostras por periodo da senoide (128 pontos = excelente THD)
#define dSINE_GENERATOR_TABLE_POINTS      128

/// Limites de frequencia em que o filtro passa-baixas RC permanece efetivo
#define dSINE_GENERATOR_MIN_FREQ_HZ       1
#define dSINE_GENERATOR_MAX_FREQ_HZ       300

/*******************************************************************************
 * PROTOTIPOS PUBLICOS
 ******************************************************************************/
void SineGenerator_Init(void);
bool SineGenerator_SetFrequency(u32 freqHz);
u32  SineGenerator_GetFrequency(void);
void SineGenerator_Step(void);

#endif /* _SINE_GENERATOR_H_ */
/** @} DOXYGEN GROUP TAG END OF FILE */
