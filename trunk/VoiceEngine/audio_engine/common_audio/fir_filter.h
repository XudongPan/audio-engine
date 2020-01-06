#ifndef VOIP_COMMON_AUDIO_FIR_FILTER_H_
#define VOIP_COMMON_AUDIO_FIR_FILTER_H_

#include <string.h>

namespace webrtc {

// Finite Impulse Response filter using floating-point arithmetic.
class FIRFilter {
 public:
  // Creates a filter with the given coefficients. All initial state values will
  // be zeros.
  // The length of the chunks fed to the filter should never be greater than
  // |max_input_length|. This is needed because, when vectorizing it is
  // necessary to concatenate the input after the state, and resizing this array
  // dynamically is expensive.
  static FIRFilter* Create(const float* coefficients,
                           size_t coefficients_length,
                           size_t max_input_length);

  virtual ~FIRFilter() {}

  // Filters the |in| data supplied.
  // |out| must be previously allocated and it must be at least of |length|.
  virtual void Filter(const float* in, size_t length, float* out) = 0;
};

}  // namespace voip

#endif  // VOIP_COMMON_AUDIO_FIR_FILTER_H_
