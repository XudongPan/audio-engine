
/*
 * This file contains the function VoIPSpl_Energy().
 * The description header can be found in signal_processing_library.h
 *
 */

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

int32_t VoipSpl_Energy(int16_t* vector, int vector_length, int* scale_factor)
{
    int32_t en = 0;
    int i;
    int scaling = VoipSpl_GetScalingSquare(vector, vector_length, vector_length);
    int looptimes = vector_length;
    int16_t *vectorptr = vector;

    for (i = 0; i < looptimes; i++)
    {
        en += VOIP_SPL_MUL_16_16_RSFT(*vectorptr, *vectorptr, scaling);
        vectorptr++;
    }
    *scale_factor = scaling;

    return en;
}
