
/*
 * This file contains the implementation of functions
 * VoIPSpl_MemSetW16()
 * VoIPSpl_MemSetW32()
 * VoIPSpl_MemCpyReversedOrder()
 * VoIPSpl_CopyFromEndW16()
 * VoIPSpl_ZerosArrayW16()
 * VoIPSpl_ZerosArrayW32()
 * VoIPSpl_OnesArrayW16()
 * VoIPSpl_OnesArrayW32()
 *
 * The description header can be found in signal_processing_library.h
 *
 */

#include <string.h>
#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"


void VoipSpl_MemSetW16(int16_t *ptr, int16_t set_value, int length)
{
    int j;
    int16_t *arrptr = ptr;

    for (j = length; j > 0; j--)
    {
        *arrptr++ = set_value;
    }
}

void VoipSpl_MemSetW32(int32_t *ptr, int32_t set_value, int length)
{
    int j;
    int32_t *arrptr = ptr;

    for (j = length; j > 0; j--)
    {
        *arrptr++ = set_value;
    }
}

void VoipSpl_MemCpyReversedOrder(int16_t* dest, int16_t* source, int length)
{
    int j;
    int16_t* destPtr = dest;
    int16_t* sourcePtr = source;

    for (j = 0; j < length; j++)
    {
        *destPtr-- = *sourcePtr++;
    }
}

int16_t VoipSpl_CopyFromEndW16(const int16_t *vector_in,
                                 int16_t length,
                                 int16_t samples,
                                 int16_t *vector_out)
{
    // Copy the last <samples> of the input vector to vector_out
    VOIP_SPL_MEMCPY_W16(vector_out, &vector_in[length - samples], samples);

    return samples;
}

int16_t VoipSpl_ZerosArrayW16(int16_t *vector, int16_t length)
{
    VoipSpl_MemSetW16(vector, 0, length);
    return length;
}

int16_t VoipSpl_ZerosArrayW32(int32_t *vector, int16_t length)
{
    VoipSpl_MemSetW32(vector, 0, length);
    return length;
}

int16_t VoipSpl_OnesArrayW16(int16_t *vector, int16_t length)
{
    int16_t i;
    int16_t *tmpvec = vector;
    for (i = 0; i < length; i++)
    {
        *tmpvec++ = 1;
    }
    return length;
}

int16_t VoipSpl_OnesArrayW32(int32_t *vector, int16_t length)
{
    int16_t i;
    int32_t *tmpvec = vector;
    for (i = 0; i < length; i++)
    {
        *tmpvec++ = 1;
    }
    return length;
}
