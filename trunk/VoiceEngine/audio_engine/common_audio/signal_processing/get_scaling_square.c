/*
 * This file contains the function VoIPSpl_GetScalingSquare().
 * The description header can be found in signal_processing_library.h
 *
 */

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

int VoipSpl_GetScalingSquare(int16_t *in_vector, int in_vector_length, int times)
{
    int nbits = VoipSpl_GetSizeInBits(times);
    int i;
    int16_t smax = -1;
    int16_t sabs;
    int16_t *sptr = in_vector;
    int t;
    int looptimes = in_vector_length;

    for (i = looptimes; i > 0; i--)
    {
        sabs = (*sptr > 0 ? *sptr++ : -*sptr++);
        smax = (sabs > smax ? sabs : smax);
    }
    t = VoipSpl_NormW32(VOIP_SPL_MUL(smax, smax));

    if (smax == 0)
    {
        return 0; // Since norm(0) returns 0
    } else
    {
        return (t > nbits) ? 0 : nbits - t;
    }
}
