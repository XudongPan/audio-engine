// Performs delay estimation on binary converted spectra.
// The return value is  0 - OK and -1 - Error, unless otherwise stated.

#ifndef VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_H_
#define VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_H_

#include "audio_engine/include/typedefs.h"

static const int32_t kMaxBitCountsQ9 = (32 << 9);  // 32 matching bits in Q9.

typedef struct {
  // Pointer to bit counts.
  int* far_bit_counts;
  // Binary history variables.
  uint32_t* binary_far_history;
  int history_size;
} BinaryDelayEstimatorFarend;

typedef struct {
  // Pointer to bit counts.
  int32_t* mean_bit_counts;
  // Array only used locally in ProcessBinarySpectrum() but whose size is
  // determined at run-time.
  int32_t* bit_counts;

  // Binary history variables.
  uint32_t* binary_near_history;
  int near_history_size;

  // Delay estimation variables.
  int32_t minimum_probability;
  int last_delay_probability;

  // Delay memory.
  int last_delay;

  // Robust validation
  int robust_validation_enabled;
  int allowed_offset;
  int last_candidate_delay;
  int compare_delay;
  int candidate_hits;
  float* histogram;
  float last_delay_histogram;

  // For dynamically changing the lookahead when using SoftReset...().
  int lookahead;

  // Far-end binary spectrum history buffer etc.
  BinaryDelayEstimatorFarend* farend;
} BinaryDelayEstimator;

// Releases the memory allocated by
// VoIP_CreateBinaryDelayEstimatorFarend(...).
// Input:
//    - self              : Pointer to the binary delay estimation far-end
//                          instance which is the return value of
//                          VoIP_CreateBinaryDelayEstimatorFarend().
//
void Voip_FreeBinaryDelayEstimatorFarend(BinaryDelayEstimatorFarend* self);

// Allocates the memory needed by the far-end part of the binary delay
// estimation. The memory needs to be initialized separately through
// VoIP_InitBinaryDelayEstimatorFarend(...).
//
// Inputs:
//      - history_size    : Size of the far-end binary spectrum history.
//
// Return value:
//      - BinaryDelayEstimatorFarend*
//                        : Created |handle|. If the memory can't be allocated
//                          or if any of the input parameters are invalid NULL
//                          is returned.
//
BinaryDelayEstimatorFarend* Voip_CreateBinaryDelayEstimatorFarend(
    int history_size);

// Initializes the delay estimation far-end instance created with
// VoIP_CreateBinaryDelayEstimatorFarend(...).
//
// Input:
//    - self              : Pointer to the delay estimation far-end instance.
//
// Output:
//    - self              : Initialized far-end instance.
//
void Voip_InitBinaryDelayEstimatorFarend(BinaryDelayEstimatorFarend* self);

// Soft resets the delay estimation far-end instance created with
// VoIP_CreateBinaryDelayEstimatorFarend(...).
//
// Input:
//    - delay_shift   : The amount of blocks to shift history buffers.
//
void Voip_SoftResetBinaryDelayEstimatorFarend(
    BinaryDelayEstimatorFarend* self, int delay_shift);

// Adds the binary far-end spectrum to the internal far-end history buffer. This
// spectrum is used as reference when calculating the delay using
// VoIP_ProcessBinarySpectrum().
//
// Inputs:
//    - self                  : Pointer to the delay estimation far-end
//                              instance.
//    - binary_far_spectrum   : Far-end binary spectrum.
//
// Output:
//    - self                  : Updated far-end instance.
//
void Voip_AddBinaryFarSpectrum(BinaryDelayEstimatorFarend* self,
                                 uint32_t binary_far_spectrum);

// Releases the memory allocated by VoIP_CreateBinaryDelayEstimator(...).
//
// Note that BinaryDelayEstimator utilizes BinaryDelayEstimatorFarend, but does
// not take ownership of it, hence the BinaryDelayEstimator has to be torn down
// before the far-end.
//
// Input:
//    - self              : Pointer to the binary delay estimation instance
//                          which is the return value of
//                          VoIP_CreateBinaryDelayEstimator().
//
void Voip_FreeBinaryDelayEstimator(BinaryDelayEstimator* self);

// Allocates the memory needed by the binary delay estimation. The memory needs
// to be initialized separately through VoIP_InitBinaryDelayEstimator(...).
//
// See VoIP_CreateDelayEstimator(..) in delay_estimator_wrapper.c for detailed
// description.
BinaryDelayEstimator* Voip_CreateBinaryDelayEstimator(
    BinaryDelayEstimatorFarend* farend, int max_lookahead);

// Initializes the delay estimation instance created with
// VoIP_CreateBinaryDelayEstimator(...).
//
// Input:
//    - self              : Pointer to the delay estimation instance.
//
// Output:
//    - self              : Initialized instance.
//
void Voip_InitBinaryDelayEstimator(BinaryDelayEstimator* self);

// Soft resets the delay estimation instance created with
// VoIP_CreateBinaryDelayEstimator(...).
//
// Input:
//    - delay_shift   : The amount of blocks to shift history buffers.
//
// Return value:
//    - actual_shifts : The actual number of shifts performed.
//
int Voip_SoftResetBinaryDelayEstimator(BinaryDelayEstimator* self,
                                         int delay_shift);

// Estimates and returns the delay between the binary far-end and binary near-
// end spectra. It is assumed the binary far-end spectrum has been added using
// VoIP_AddBinaryFarSpectrum() prior to this call. The value will be offset by
// the lookahead (i.e. the lookahead should be subtracted from the returned
// value).
//
// Inputs:
//    - self                  : Pointer to the delay estimation instance.
//    - binary_near_spectrum  : Near-end binary spectrum of the current block.
//
// Output:
//    - self                  : Updated instance.
//
// Return value:
//    - delay                 :  >= 0 - Calculated delay value.
//                              -2    - Insufficient data for estimation.
//
int Voip_ProcessBinarySpectrum(BinaryDelayEstimator* self,
                                 uint32_t binary_near_spectrum);

// Returns the last calculated delay updated by the function
// VoIP_ProcessBinarySpectrum(...).
//
// Input:
//    - self                  : Pointer to the delay estimation instance.
//
// Return value:
//    - delay                 :  >= 0 - Last calculated delay value
//                              -2    - Insufficient data for estimation.
//
int Voip_binary_last_delay(BinaryDelayEstimator* self);

// Returns the estimation quality of the last calculated delay updated by the
// function VoIP_ProcessBinarySpectrum(...). The estimation quality is a value
// in the interval [0, 1].  The higher the value, the better the quality.
//
// Return value:
//    - delay_quality         :  >= 0 - Estimation quality of last calculated
//                                      delay value.
float Voip_binary_last_delay_quality(BinaryDelayEstimator* self);

// Updates the |mean_value| recursively with a step size of 2^-|factor|. This
// function is used internally in the Binary Delay Estimator as well as the
// Fixed point wrapper.
//
// Inputs:
//    - new_value             : The new value the mean should be updated with.
//    - factor                : The step size, in number of right shifts.
//
// Input/Output:
//    - mean_value            : Pointer to the mean value.
//
void Voip_MeanEstimatorFix(int32_t new_value,
                             int factor,
                             int32_t* mean_value);


#endif  // VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_H_
