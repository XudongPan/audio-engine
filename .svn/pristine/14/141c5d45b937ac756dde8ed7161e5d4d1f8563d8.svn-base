/*
 * This file contains implementations of the iLBC specific functions
 * VoIPSpl_ReverseOrderMultArrayElements()
 * VoIPSpl_ElementwiseVectorMult()
 * VoIPSpl_AddVectorsAndShift()
 * VoIPSpl_AddAffineVectorToVector()
 * VoIPSpl_AffineTransformVector()
 *
 */

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

void VoipSpl_ReverseOrderMultArrayElements(int16_t *out, const int16_t *in,
                                             const int16_t *win,
                                             int16_t vector_length,
                                             int16_t right_shifts)
{
    int i;
    int16_t *outptr = out;
    const int16_t *inptr = in;
    const int16_t *winptr = win;
    for (i = 0; i < vector_length; i++)
    {
        (*outptr++) = (int16_t)VOIP_SPL_MUL_16_16_RSFT(*inptr++,
                                                               *winptr--, right_shifts);
    }
}

void VoipSpl_ElementwiseVectorMult(int16_t *out, const int16_t *in,
                                     const int16_t *win, int16_t vector_length,
                                     int16_t right_shifts)
{
    int i;
    int16_t *outptr = out;
    const int16_t *inptr = in;
    const int16_t *winptr = win;
    for (i = 0; i < vector_length; i++)
    {
        (*outptr++) = (int16_t)VOIP_SPL_MUL_16_16_RSFT(*inptr++,
                                                               *winptr++, right_shifts);
    }
}

void VoipSpl_AddVectorsAndShift(int16_t *out, const int16_t *in1,
                                  const int16_t *in2, int16_t vector_length,
                                  int16_t right_shifts)
{
    int i;
    int16_t *outptr = out;
    const int16_t *in1ptr = in1;
    const int16_t *in2ptr = in2;
    for (i = vector_length; i > 0; i--)
    {
        (*outptr++) = (int16_t)(((*in1ptr++) + (*in2ptr++)) >> right_shifts);
    }
}

void VoipSpl_AddAffineVectorToVector(int16_t *out, int16_t *in,
                                       int16_t gain, int32_t add_constant,
                                       int16_t right_shifts, int vector_length)
{
    int16_t *inPtr;
    int16_t *outPtr;
    int i;

    inPtr = in;
    outPtr = out;
    for (i = 0; i < vector_length; i++)
    {
        (*outPtr++) += (int16_t)((VOIP_SPL_MUL_16_16((*inPtr++), gain)
                + (int32_t)add_constant) >> right_shifts);
    }
}

void VoipSpl_AffineTransformVector(int16_t *out, int16_t *in,
                                     int16_t gain, int32_t add_constant,
                                     int16_t right_shifts, int vector_length)
{
    int16_t *inPtr;
    int16_t *outPtr;
    int i;

    inPtr = in;
    outPtr = out;
    for (i = 0; i < vector_length; i++)
    {
        (*outPtr++) = (int16_t)((VOIP_SPL_MUL_16_16((*inPtr++), gain)
                + (int32_t)add_constant) >> right_shifts);
    }
}
