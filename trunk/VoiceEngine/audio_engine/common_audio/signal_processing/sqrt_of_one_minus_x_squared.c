/*
 * This file contains the function VoIPSpl_SqrtOfOneMinusXSquared().
 * The description header can be found in signal_processing_library.h
 *
 */

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

void VoipSpl_SqrtOfOneMinusXSquared(int16_t *xQ15, int vector_length,
                                      int16_t *yQ15)
{
    int32_t sq;
    int m;
    int16_t tmp;

    for (m = 0; m < vector_length; m++)
    {
        tmp = xQ15[m];
        sq = VOIP_SPL_MUL_16_16(tmp, tmp); // x^2 in Q30
        sq = 1073741823 - sq; // 1-x^2, where 1 ~= 0.99999999906 is 1073741823 in Q30
        sq = VoipSpl_Sqrt(sq); // sqrt(1-x^2) in Q15
        yQ15[m] = (int16_t)sq;
    }
}
