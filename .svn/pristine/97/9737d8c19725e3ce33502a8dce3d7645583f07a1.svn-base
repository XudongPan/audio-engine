#ifndef VOIP_COMMON_AUDIO_FIR_FILTER_SSE_H_
#define VOIP_COMMON_AUDIO_FIR_FILTER_SSE_H_

#include "audio_engine/common_audio/fir_filter.h"
#include "audio_engine/system_wrappers/interface/aligned_malloc.h"
#include "audio_engine/system_wrappers/interface/scoped_ptr.h"

namespace webrtc {

class FIRFilterSSE2 : public FIRFilter {
 public:
  FIRFilterSSE2(const float* coefficients,
                size_t coefficients_length,
                size_t max_input_length);

  virtual void Filter(const float* in, size_t length, float* out) OVERRIDE;

 private:
  size_t coefficients_length_;
  size_t state_length_;
  scoped_ptr<float[], AlignedFreeDeleter> coefficients_;
  scoped_ptr<float[], AlignedFreeDeleter> state_;
};

}  // namespace voip

#endif  // VOIP_COMMON_AUDIO_FIR_FILTER_SSE_H_
