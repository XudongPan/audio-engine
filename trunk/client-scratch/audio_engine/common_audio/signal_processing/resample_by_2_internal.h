/*
 * This header file contains some internal resampling functions.
 *
 */

#ifndef VOIP_SPL_RESAMPLE_BY_2_INTERNAL_H_
#define VOIP_SPL_RESAMPLE_BY_2_INTERNAL_H_

#include "audio_engine/include/typedefs.h"

/*******************************************************************
 * resample_by_2_fast.c
 * Functions for internal use in the other resample functions
 ******************************************************************/
void VoipSpl_DownBy2IntToShort(int32_t *in, int32_t len, int16_t *out,
                                 int32_t *state);

void VoipSpl_DownBy2ShortToInt(const int16_t *in, int32_t len,
                                 int32_t *out, int32_t *state);

void VoipSpl_UpBy2ShortToInt(const int16_t *in, int32_t len,
                               int32_t *out, int32_t *state);

void VoipSpl_UpBy2IntToInt(const int32_t *in, int32_t len, int32_t *out,
                             int32_t *state);

void VoipSpl_UpBy2IntToShort(const int32_t *in, int32_t len,
                               int16_t *out, int32_t *state);

void VoipSpl_LPBy2ShortToInt(const int16_t* in, int32_t len,
                               int32_t* out, int32_t* state);

void VoipSpl_LPBy2IntToInt(const int32_t* in, int32_t len, int32_t* out,
                             int32_t* state);

#endif // VOIP_SPL_RESAMPLE_BY_2_INTERNAL_H_
