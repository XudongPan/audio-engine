#include "audio_engine/modules/audio_processing/ns/include/noise_suppression.h"

#include <stdlib.h>
#include <string.h>

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"
#include "audio_engine/modules/audio_processing/ns/defines.h"
#include "audio_engine/modules/audio_processing/ns/ns_core.h"

int VoipNs_Create(NsHandle** NS_inst) {
  *NS_inst = (NsHandle*) malloc(sizeof(NSinst_t));
  if (*NS_inst != NULL) {
    (*(NSinst_t**)NS_inst)->initFlag = 0;
    return 0;
  } else {
    return -1;
  }

}

int VoipNs_Free(NsHandle* NS_inst) {
  free(NS_inst);
  return 0;
}


int VoipNs_Init(NsHandle* NS_inst, uint32_t fs) {
  return VoipNs_InitCore((NSinst_t*) NS_inst, fs);
}

int VoipNs_set_policy(NsHandle* NS_inst, int mode) {
  return VoipNs_set_policy_core((NSinst_t*) NS_inst, mode);
}


int VoipNs_Process(NsHandle* NS_inst, short* spframe, short* spframe_H,
                     short* outframe, short* outframe_H) {
  return VoipNs_ProcessCore(
      (NSinst_t*) NS_inst, spframe, spframe_H, outframe, outframe_H);
}

float VoipNs_prior_speech_probability(NsHandle* handle) {
  NSinst_t* self = (NSinst_t*) handle;
  if (handle == NULL) {
    return -1;
  }
  if (self->initFlag == 0) {
    return -1;
  }
  return self->priorSpeechProb;
}
