
/*
 * This file contains the function VoIPSpl_ReflCoefToLpc().
 * The description header can be found in signal_processing_library.h
 *
 */

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

void VoipSpl_ReflCoefToLpc(const int16_t *k, int use_order, int16_t *a)
{
    int16_t any[VOIP_SPL_MAX_LPC_ORDER + 1];
    int16_t *aptr, *aptr2, *anyptr;
    const int16_t *kptr;
    int m, i;

    kptr = k;
    *a = 4096; // i.e., (Word16_MAX >> 3)+1.
    *any = *a;
    a[1] = VOIP_SPL_RSHIFT_W16((*k), 3);

    for (m = 1; m < use_order; m++)
    {
        kptr++;
        aptr = a;
        aptr++;
        aptr2 = &a[m];
        anyptr = any;
        anyptr++;

        any[m + 1] = VOIP_SPL_RSHIFT_W16((*kptr), 3);
        for (i = 0; i < m; i++)
        {
            *anyptr = (*aptr)
                    + (int16_t)VOIP_SPL_MUL_16_16_RSFT((*aptr2), (*kptr), 15);
            anyptr++;
            aptr++;
            aptr2--;
        }

        aptr = a;
        anyptr = any;
        for (i = 0; i < (m + 2); i++)
        {
            *aptr = *anyptr;
            aptr++;
            anyptr++;
        }
    }
}
