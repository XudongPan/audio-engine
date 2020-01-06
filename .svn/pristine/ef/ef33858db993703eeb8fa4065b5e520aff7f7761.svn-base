/*
 * This file contains the implementation of functions
 * VoIPSpl_MaxAbsValueW16C()
 * VoIPSpl_MaxAbsValueW32C()
 * VoIPSpl_MaxValueW16C()
 * VoIPSpl_MaxValueW32C()
 * VoIPSpl_MinValueW16C()
 * VoIPSpl_MinValueW32C()
 * VoIPSpl_MaxAbsIndexW16()
 * VoIPSpl_MaxIndexW16()
 * VoIPSpl_MaxIndexW32()
 * VoIPSpl_MinIndexW16()
 * VoIPSpl_MinIndexW32()
 *
 */

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

#include <stdlib.h>

// TODO(bjorn/kma): Consolidate function pairs (e.g. combine
//   VoIPSpl_MaxAbsValueW16C and VoIPSpl_MaxAbsIndexW16 into a single one.)
// TODO(kma): Move the next six functions into min_max_operations_c.c.

// Maximum absolute value of word16 vector. C version for generic platforms.
int16_t VoipSpl_MaxAbsValueW16C(const int16_t* vector, int length) {
  int i = 0, absolute = 0, maximum = 0;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    absolute = abs((int)vector[i]);

    if (absolute > maximum) {
      maximum = absolute;
    }
  }

  // Guard the case for abs(-32768).
  if (maximum > VOIP_SPL_WORD16_MAX) {
    maximum = VOIP_SPL_WORD16_MAX;
  }

  return (int16_t)maximum;
}

// Maximum absolute value of word32 vector. C version for generic platforms.
int32_t VoipSpl_MaxAbsValueW32C(const int32_t* vector, int length) {
  // Use uint32_t for the local variables, to accommodate the return value
  // of abs(0x80000000), which is 0x80000000.

  uint32_t absolute = 0, maximum = 0;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    absolute = abs((int)vector[i]);
    if (absolute > maximum) {
      maximum = absolute;
    }
  }

  maximum = VOIP_SPL_MIN(maximum, VOIP_SPL_WORD32_MAX);

  return (int32_t)maximum;
}

// Maximum value of word16 vector. C version for generic platforms.
int16_t VoipSpl_MaxValueW16C(const int16_t* vector, int length) {
  int16_t maximum = VOIP_SPL_WORD16_MIN;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return maximum;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] > maximum)
      maximum = vector[i];
  }
  return maximum;
}

// Maximum value of word32 vector. C version for generic platforms.
int32_t VoipSpl_MaxValueW32C(const int32_t* vector, int length) {
  int32_t maximum = VOIP_SPL_WORD32_MIN;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return maximum;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] > maximum)
      maximum = vector[i];
  }
  return maximum;
}

// Minimum value of word16 vector. C version for generic platforms.
int16_t VoipSpl_MinValueW16C(const int16_t* vector, int length) {
  int16_t minimum = VOIP_SPL_WORD16_MAX;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return minimum;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] < minimum)
      minimum = vector[i];
  }
  return minimum;
}

// Minimum value of word32 vector. C version for generic platforms.
int32_t VoipSpl_MinValueW32C(const int32_t* vector, int length) {
  int32_t minimum = VOIP_SPL_WORD32_MAX;
  int i = 0;

  if (vector == NULL || length <= 0) {
    return minimum;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] < minimum)
      minimum = vector[i];
  }
  return minimum;
}

// Index of maximum absolute value in a word16 vector.
int VoipSpl_MaxAbsIndexW16(const int16_t* vector, int length) {
  // Use type int for local variables, to accomodate the value of abs(-32768).

  int i = 0, absolute = 0, maximum = 0, index = 0;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    absolute = abs((int)vector[i]);

    if (absolute > maximum) {
      maximum = absolute;
      index = i;
    }
  }

  return index;
}

// Index of maximum value in a word16 vector.
int VoipSpl_MaxIndexW16(const int16_t* vector, int length) {
  int i = 0, index = 0;
  int16_t maximum = VOIP_SPL_WORD16_MIN;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] > maximum) {
      maximum = vector[i];
      index = i;
    }
  }

  return index;
}

// Index of maximum value in a word32 vector.
int VoipSpl_MaxIndexW32(const int32_t* vector, int length) {
  int i = 0, index = 0;
  int32_t maximum = VOIP_SPL_WORD32_MIN;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] > maximum) {
      maximum = vector[i];
      index = i;
    }
  }

  return index;
}

// Index of minimum value in a word16 vector.
int VoipSpl_MinIndexW16(const int16_t* vector, int length) {
  int i = 0, index = 0;
  int16_t minimum = VOIP_SPL_WORD16_MAX;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] < minimum) {
      minimum = vector[i];
      index = i;
    }
  }

  return index;
}

// Index of minimum value in a word32 vector.
int VoipSpl_MinIndexW32(const int32_t* vector, int length) {
  int i = 0, index = 0;
  int32_t minimum = VOIP_SPL_WORD32_MAX;

  if (vector == NULL || length <= 0) {
    return -1;
  }

  for (i = 0; i < length; i++) {
    if (vector[i] < minimum) {
      minimum = vector[i];
      index = i;
    }
  }

  return index;
}
