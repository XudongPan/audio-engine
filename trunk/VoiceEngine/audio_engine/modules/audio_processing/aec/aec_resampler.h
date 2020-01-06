#ifndef VOIP_MODULES_AUDIO_PROCESSING_AEC_AEC_RESAMPLER_H_
#define VOIP_MODULES_AUDIO_PROCESSING_AEC_AEC_RESAMPLER_H_

#include "audio_engine/modules/audio_processing/aec/aec_core.h"

enum {
  kResamplingDelay = 1
};
enum {
  kResamplerBufferSize = FRAME_LEN * 4
};

// Unless otherwise specified, functions return 0 on success and -1 on error
int VoipAec_CreateResampler(void** resampInst);
int VoipAec_InitResampler(void* resampInst, int deviceSampleRateHz);
int VoipAec_FreeResampler(void* resampInst);

// Estimates skew from raw measurement.
int VoipAec_GetSkew(void* resampInst, int rawSkew, float* skewEst);

// Resamples input using linear interpolation.
void VoipAec_ResampleLinear(void* resampInst,
                              const short* inspeech,
                              int size,
                              float skew,
                              short* outspeech,
                              int* size_out);

#endif  // VOIP_MODULES_AUDIO_PROCESSING_AEC_AEC_RESAMPLER_H_
