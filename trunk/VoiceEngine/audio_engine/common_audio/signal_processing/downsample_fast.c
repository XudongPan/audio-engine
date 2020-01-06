
#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

// TODO(Bjornv): Change the function parameter order to VoIP code style.
// C version of VoIPSpl_DownsampleFast() for generic platforms.
int VoipSpl_DownsampleFastC(const int16_t* data_in,
                              int data_in_length,
                              int16_t* data_out,
                              int data_out_length,
                              const int16_t* __restrict coefficients,
                              int coefficients_length,
                              int factor,
                              int delay) {
  int i = 0;
  int j = 0;
  int32_t out_s32 = 0;
  int endpos = delay + factor * (data_out_length - 1) + 1;

  // Return error if any of the running conditions doesn't meet.
  if (data_out_length <= 0 || coefficients_length <= 0
                           || data_in_length < endpos) {
    return -1;
  }

  for (i = delay; i < endpos; i += factor) {
    out_s32 = 2048;  // Round value, 0.5 in Q12.

    for (j = 0; j < coefficients_length; j++) {
      out_s32 += coefficients[j] * data_in[i - j];  // Q12.
    }

    out_s32 >>= 12;  // Q0.

    // Saturate and store the output.
    *data_out++ = VoipSpl_SatW32ToW16(out_s32);
  }

  return 0;
}
