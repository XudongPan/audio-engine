#include "audio_engine/modules/audio_processing/ns/include/noise_suppression_x.h"

#include <stdlib.h>

#include "audio_engine/common_audio/signal_processing/include/real_fft.h"
#include "audio_engine/modules/audio_processing/ns/nsx_core.h"
#include "audio_engine/modules/audio_processing/ns/nsx_defines.h"

int VoipNsx_Create(NsxHandle** nsxInst) {
  NsxInst_t* self = malloc(sizeof(NsxInst_t));
  *nsxInst = (NsxHandle*)self;

  if (self != NULL) {
    WebRtcSpl_Init();
    self->real_fft = NULL;
    self->initFlag = 0;
    return 0;
  } else {
    return -1;
  }

}

int VoipNsx_Free(NsxHandle* nsxInst) {
  WebRtcSpl_FreeRealFFT(((NsxInst_t*)nsxInst)->real_fft);
  free(nsxInst);
  return 0;
}

int VoipNsx_Init(NsxHandle* nsxInst, uint32_t fs) {
  return WebRtcNsx_InitCore((NsxInst_t*)nsxInst, fs);
}

int VoipNsx_set_policy(NsxHandle* nsxInst, int mode) {
  return WebRtcNsx_set_policy_core((NsxInst_t*)nsxInst, mode);
}

int VoipNsx_Process(NsxHandle* nsxInst, short* speechFrame,
                      short* speechFrameHB, short* outFrame,
                      short* outFrameHB) {
  return WebRtcNsx_ProcessCore(
      (NsxInst_t*)nsxInst, speechFrame, speechFrameHB, outFrame, outFrameHB);
}
