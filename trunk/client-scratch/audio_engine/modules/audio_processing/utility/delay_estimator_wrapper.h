// Performs delay estimation on block by block basis.
// The return value is  0 - OK and -1 - Error, unless otherwise stated.

#ifndef VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_WRAPPER_H_
#define VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_WRAPPER_H_

#include "audio_engine/include/typedefs.h"

// Releases the memory allocated by VoIP_CreateDelayEstimatorFarend(...)
// Input:
//      - handle        : Pointer to the delay estimation far-end instance.
//
void Voip_FreeDelayEstimatorFarend(void* handle);

// Allocates the memory needed by the far-end part of the delay estimation. The
// memory needs to be initialized separately through
// VoIP_InitDelayEstimatorFarend(...).
//
// Inputs:
//      - spectrum_size : Size of the spectrum used both in far-end and
//                        near-end. Used to allocate memory for spectrum
//                        specific buffers.
//      - history_size  : The far-end history buffer size. Note that the maximum
//                        delay which can be estimated is controlled together
//                        with |lookahead| through
//                        Voip_CreateDelayEstimator().
//
// Return value:
//      - void*         : Created |handle|. If the memory can't be allocated or
//                        if any of the input parameters are invalid NULL is
//                        returned.
//
void* Voip_CreateDelayEstimatorFarend(int spectrum_size, int history_size);

// Initializes the far-end part of the delay estimation instance returned by
// Voip_CreateDelayEstimatorFarend(...)
// Input:
//      - handle        : Pointer to the delay estimation far-end instance.
//
// Output:
//      - handle        : Initialized instance.
//
int Voip_InitDelayEstimatorFarend(void* handle);

// Soft resets the far-end part of the delay estimation instance returned by
// Voip_CreateDelayEstimatorFarend(...).
// Input:
//      - delay_shift   : The amount of blocks to shift history buffers.
//
void Voip_SoftResetDelayEstimatorFarend(void* handle, int delay_shift);

// Adds the far-end spectrum to the far-end history buffer. This spectrum is
// used as reference when calculating the delay using
// VoIP_ProcessSpectrum().
//
// Inputs:
//    - handle          : Pointer to the delay estimation far-end instance.
//    - far_spectrum    : Far-end spectrum.
//    - spectrum_size   : The size of the data arrays (same for both far- and
//                        near-end).
//    - far_q           : The Q-domain of the far-end data.
//
// Output:
//    - handle          : Updated far-end instance.
//
int Voip_AddFarSpectrumFix(void* handle, uint16_t* far_spectrum,
                             int spectrum_size, int far_q);

// See Voip_AddFarSpectrumFix() for description.
int Voip_AddFarSpectrumFloat(void* handle, float* far_spectrum,
                               int spectrum_size);

// Releases the memory allocated by VoIP_CreateDelayEstimator(...)
// Input:
//      - handle        : Pointer to the delay estimation instance.
//
void Voip_FreeDelayEstimator(void* handle);

// Allocates the memory needed by the delay estimation. The memory needs to be
// initialized separately through VoIP_InitDelayEstimator(...).
//
// Inputs:
//      - farend_handle : Pointer to the far-end part of the delay estimation
//                        instance created prior to this call using
//                        VoIP_CreateDelayEstimatorFarend().
//
//                        Note that VoIP_CreateDelayEstimator does not take
//                        ownership of |farend_handle|, which has to be torn
//                        down properly after this instance.
//
//      - max_lookahead : Maximum amount of non-causal lookahead allowed. The
//                        actual amount of lookahead used can be controlled by
//                        VoIP_set_lookahead(...). The default |lookahead| is
//                        set to |max_lookahead| at create time. Use
//                        VoIP_set_lookahead(...) before start if a different
//                        value is desired.
//
//                        Using lookahead can detect cases in which a near-end
//                        signal occurs before the corresponding far-end signal.
//                        It will delay the estimate for the current block by an
//                        equal amount, and the returned values will be offset
//                        by it.
//
//                        A value of zero is the typical no-lookahead case.
//                        This also represents the minimum delay which can be
//                        estimated.
//
//                        Note that the effective range of delay estimates is
//                        [-|lookahead|,... ,|history_size|-|lookahead|)
//                        where |history_size| was set upon creating the far-end
//                        history buffer size.
//
// Return value:
//      - void*         : Created |handle|. If the memory can't be allocated or
//                        if any of the input parameters are invalid NULL is
//                        returned.
//
void* Voip_CreateDelayEstimator(void* farend_handle, int max_lookahead);

// Initializes the delay estimation instance returned by
// VoIP_CreateDelayEstimator(...)
// Input:
//      - handle        : Pointer to the delay estimation instance.
//
// Output:
//      - handle        : Initialized instance.
//
int Voip_InitDelayEstimator(void* handle);

// Soft resets the delay estimation instance returned by
// VoIP_CreateDelayEstimator(...)
// Input:
//      - delay_shift   : The amount of blocks to shift history buffers.
//
// Return value:
//      - actual_shifts : The actual number of shifts performed.
//
int Voip_SoftResetDelayEstimator(void* handle, int delay_shift);

// Sets the amount of |lookahead| to use. Valid values are [0, max_lookahead]
// where |max_lookahead| was set at create time through
// VoIP_CreateDelayEstimator(...).
//
// Input:
//      - lookahead     : The amount of blocks to shift history buffers.
//
// Return value:
//      - new_lookahead : The actual number of shifts performed.
//
int Voip_set_lookahead(void* handle, int lookahead);

// Returns the amount of lookahead we currently use.
int Voip_lookahead(void* handle);

// Sets the |allowed_offset| used in the robust validation scheme.  If the
// delay estimator is used in an echo control component, this parameter is
// related to the filter length.  In principle |allowed_offset| should be set to
// the echo control filter length minus the expected echo duration, i.e., the
// delay offset the echo control can handle without quality regression.  The
// default value, used if not set manually, is zero.  Note that |allowed_offset|
// has to be non-negative.
// Inputs:
//  - handle            : Pointer to the delay estimation instance.
//  - allowed_offset    : The amount of delay offset, measured in partitions,
//                        the echo control filter can handle.
int Voip_set_allowed_offset(void* handle, int allowed_offset);

// Returns the |allowed_offset| in number of partitions.
int Voip_get_allowed_offset(const void* handle);

// TODO(bjornv): Implement this functionality.  Currently, enabling it has no
// impact, hence this is an empty API.
// Enables/Disables a robust validation functionality in the delay estimation.
// This is by default set to disabled at create time.  The state is preserved
// over a reset.
// Inputs:
//      - handle        : Pointer to the delay estimation instance.
//      - enable        : Enable (1) or disable (0) this feature.
int Voip_enable_robust_validation(void* handle, int enable);

// Returns 1 if robust validation is enabled and 0 if disabled.
int Voip_is_robust_validation_enabled(const void* handle);

// Estimates and returns the delay between the far-end and near-end blocks. The
// value will be offset by the lookahead (i.e. the lookahead should be
// subtracted from the returned value).
// Inputs:
//      - handle        : Pointer to the delay estimation instance.
//      - near_spectrum : Pointer to the near-end spectrum data of the current
//                        block.
//      - spectrum_size : The size of the data arrays (same for both far- and
//                        near-end).
//      - near_q        : The Q-domain of the near-end data.
//
// Output:
//      - handle        : Updated instance.
//
// Return value:
//      - delay         :  >= 0 - Calculated delay value.
//                        -1    - Error.
//                        -2    - Insufficient data for estimation.
//
int Voip_DelayEstimatorProcessFix(void* handle,
                                    uint16_t* near_spectrum,
                                    int spectrum_size,
                                    int near_q);

// See VoIP_DelayEstimatorProcessFix() for description.
int Voip_DelayEstimatorProcessFloat(void* handle,
                                      float* near_spectrum,
                                      int spectrum_size);

// Returns the last calculated delay updated by the function
// VoIP_DelayEstimatorProcess(...).
//
// Input:
//      - handle        : Pointer to the delay estimation instance.
//
// Return value:
//      - delay         : >= 0  - Last calculated delay value.
//                        -1    - Error.
//                        -2    - Insufficient data for estimation.
//
int Voip_last_delay(void* handle);

// Returns the estimation quality/probability of the last calculated delay
// updated by the function VoIP_DelayEstimatorProcess(...). The estimation
// quality is a value in the interval [0, 1]. The higher the value, the better
// the quality.
//
// Return value:
//      - delay_quality : >= 0  - Estimation quality of last calculated delay.
float Voip_last_delay_quality(void* handle);

#endif  // VOIP_MODULES_AUDIO_PROCESSING_UTILITY_DELAY_ESTIMATOR_WRAPPER_H_
