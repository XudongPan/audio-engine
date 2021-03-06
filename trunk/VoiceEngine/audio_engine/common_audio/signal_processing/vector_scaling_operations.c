
#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

void VoipSpl_VectorBitShiftW16(int16_t *res, int16_t length,
                                 const int16_t *in, int16_t right_shifts)
{
    int i;

    if (right_shifts > 0)
    {
        for (i = length; i > 0; i--)
        {
            (*res++) = ((*in++) >> right_shifts);
        }
    } else
    {
        for (i = length; i > 0; i--)
        {
            (*res++) = ((*in++) << (-right_shifts));
        }
    }
}

void VoipSpl_VectorBitShiftW32(int32_t *out_vector,
                                 int16_t vector_length,
                                 const int32_t *in_vector,
                                 int16_t right_shifts)
{
    int i;

    if (right_shifts > 0)
    {
        for (i = vector_length; i > 0; i--)
        {
            (*out_vector++) = ((*in_vector++) >> right_shifts);
        }
    } else
    {
        for (i = vector_length; i > 0; i--)
        {
            (*out_vector++) = ((*in_vector++) << (-right_shifts));
        }
    }
}

void VoipSpl_VectorBitShiftW32ToW16(int16_t* out, int length,
                                      const int32_t* in, int right_shifts) {
  int i;
  int32_t tmp_w32;

  if (right_shifts >= 0) {
    for (i = length; i > 0; i--) {
      tmp_w32 = (*in++) >> right_shifts;
      (*out++) = VoipSpl_SatW32ToW16(tmp_w32);
    }
  } else {
    int16_t left_shifts = -right_shifts;
    for (i = length; i > 0; i--) {
      tmp_w32 = (*in++) << left_shifts;
      (*out++) = VoipSpl_SatW32ToW16(tmp_w32);
    }
  }
}

void VoipSpl_ScaleVector(const int16_t *in_vector, int16_t *out_vector,
                           int16_t gain, int16_t in_vector_length,
                           int16_t right_shifts)
{
    // Performs vector operation: out_vector = (gain*in_vector)>>right_shifts
    int i;
    const int16_t *inptr;
    int16_t *outptr;

    inptr = in_vector;
    outptr = out_vector;

    for (i = 0; i < in_vector_length; i++)
    {
        (*outptr++) = (int16_t)VOIP_SPL_MUL_16_16_RSFT(*inptr++, gain, right_shifts);
    }
}

void VoipSpl_ScaleVectorWithSat(const int16_t *in_vector, int16_t *out_vector,
                                 int16_t gain, int16_t in_vector_length,
                                 int16_t right_shifts)
{
    // Performs vector operation: out_vector = (gain*in_vector)>>right_shifts
    int i;
    int32_t tmpW32;
    const int16_t *inptr;
    int16_t *outptr;

    inptr = in_vector;
    outptr = out_vector;

    for (i = 0; i < in_vector_length; i++)
    {
        tmpW32 = VOIP_SPL_MUL_16_16_RSFT(*inptr++, gain, right_shifts);
        (*outptr++) = VoipSpl_SatW32ToW16(tmpW32);
    }
}

void VoipSpl_ScaleAndAddVectors(const int16_t *in1, int16_t gain1, int shift1,
                                  const int16_t *in2, int16_t gain2, int shift2,
                                  int16_t *out, int vector_length)
{
    // Performs vector operation: out = (gain1*in1)>>shift1 + (gain2*in2)>>shift2
    int i;
    const int16_t *in1ptr;
    const int16_t *in2ptr;
    int16_t *outptr;

    in1ptr = in1;
    in2ptr = in2;
    outptr = out;

    for (i = 0; i < vector_length; i++)
    {
        (*outptr++) = (int16_t)VOIP_SPL_MUL_16_16_RSFT(gain1, *in1ptr++, shift1)
                + (int16_t)VOIP_SPL_MUL_16_16_RSFT(gain2, *in2ptr++, shift2);
    }
}

// C version of VoIPSpl_ScaleAndAddVectorsWithRound() for generic platforms.
int VoipSpl_ScaleAndAddVectorsWithRoundC(const int16_t* in_vector1,
                                           int16_t in_vector1_scale,
                                           const int16_t* in_vector2,
                                           int16_t in_vector2_scale,
                                           int right_shifts,
                                           int16_t* out_vector,
                                           int length) {
  int i = 0;
  int round_value = (1 << right_shifts) >> 1;

  if (in_vector1 == NULL || in_vector2 == NULL || out_vector == NULL ||
      length <= 0 || right_shifts < 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    out_vector[i] = (int16_t)((
        VOIP_SPL_MUL_16_16(in_vector1[i], in_vector1_scale)
        + VOIP_SPL_MUL_16_16(in_vector2[i], in_vector2_scale)
        + round_value) >> right_shifts);
  }

  return 0;
}
