#include <assert.h>

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"

// TODO(bjornv): Change the return type to report errors.

void VoipSpl_FilterARFastQ12(const int16_t* data_in,
                               int16_t* data_out,
                               const int16_t* __restrict coefficients,
                               int coefficients_length,
                               int data_length) {
  int i = 0;
  int j = 0;

  assert(data_length > 0);
  assert(coefficients_length > 1);

  for (i = 0; i < data_length; i++) {
    int32_t output = 0;
    int32_t sum = 0;

    for (j = coefficients_length - 1; j > 0; j--) {
      sum += coefficients[j] * data_out[i - j];
    }

    output = coefficients[0] * data_in[i];
    output -= sum;

    // Saturate and store the output.
    output = VOIP_SPL_SAT(134215679, output, -134217728);
    data_out[i] = (int16_t)((output + 2048) >> 12);
  }
}
