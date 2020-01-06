// Header file including the delay estimator handle used for testing.

#ifndef VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_INTERNAL_H_
#define VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_INTERNAL_H_

#include "audio_engine/modules/audio_processing/utility/delay_estimator.h"
#include "audio_engine/include/typedefs.h"

typedef union {
  float float_;
  int32_t int32_;
} SpectrumType;

typedef struct {
  // Pointers to mean values of spectrum.
  SpectrumType* mean_far_spectrum;
  // |mean_far_spectrum| initialization indicator.
  int far_spectrum_initialized;

  int spectrum_size;

  // Far-end part of binary spectrum based delay estimation.
  BinaryDelayEstimatorFarend* binary_farend;
} DelayEstimatorFarend;

typedef struct {
  // Pointers to mean values of spectrum.
  SpectrumType* mean_near_spectrum;
  // |mean_near_spectrum| initialization indicator.
  int near_spectrum_initialized;

  int spectrum_size;

  // Binary spectrum based delay estimator
  BinaryDelayEstimator* binary_handle;
} DelayEstimator;

#endif  // VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_INTERNAL_H_
