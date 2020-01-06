#include "audio_engine/common_audio/vad/include/voip_vad.h"

#include <stdlib.h>
#include <string.h>

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"
#include "audio_engine/common_audio/vad/vad_core.h"
#include "audio_engine/include/typedefs.h"

static const int kInitCheck = 42;
static const int kValidRates[] = { 8000, 16000, 32000, 48000 };
static const size_t kRatesSize = sizeof(kValidRates) / sizeof(*kValidRates);
static const int kMaxFrameLengthMs = 30;

int VoipVad_Create(VadInst** handle) {
  VadInstT* self = NULL;

  if (handle == NULL) {
    return -1;
  }

  *handle = NULL;
  self = (VadInstT*) malloc(sizeof(VadInstT));
  *handle = (VadInst*) self;

  if (self == NULL) {
    return -1;
  }

  VoipSpl_Init();

  self->init_flag = 0;

  return 0;
}

int VoipVad_Free(VadInst* handle) {
  if (handle == NULL) {
    return -1;
  }

  free(handle);

  return 0;
}

// TODO(bjornv): Move VoIPVad_InitCore() code here.
int VoipVad_Init(VadInst* handle) {
  // Initialize the core VAD component.
  return VoipVad_InitCore((VadInstT*) handle);
}

// TODO(bjornv): Move VoIPVad_set_mode_core() code here.
int VoipVad_set_mode(VadInst* handle, int mode) {
  VadInstT* self = (VadInstT*) handle;

  if (handle == NULL) {
    return -1;
  }
  if (self->init_flag != kInitCheck) {
    return -1;
  }

  return VoipVad_set_mode_core(self, mode);
}

int VoipVad_Process(VadInst* handle, int fs, int16_t* audio_frame,
                      int frame_length) {
  int vad = -1;
  VadInstT* self = (VadInstT*) handle;

  if (handle == NULL) {
    return -1;
  }

  if (self->init_flag != kInitCheck) {
    return -1;
  }
  if (audio_frame == NULL) {
    return -1;
  }
  if (VoipVad_ValidRateAndFrameLength(fs, frame_length) != 0) {
    return -1;
  }

  if (fs == 48000) {
      vad = VoipVad_CalcVad48khz(self, audio_frame, frame_length);
  } else if (fs == 32000) {
    vad = VoipVad_CalcVad32khz(self, audio_frame, frame_length);
  } else if (fs == 16000) {
    vad = VoipVad_CalcVad16khz(self, audio_frame, frame_length);
  } else if (fs == 8000) {
    vad = VoipVad_CalcVad8khz(self, audio_frame, frame_length);
  }

  if (vad > 0) {
    vad = 1;
  }
  return vad;
}

int VoipVad_ValidRateAndFrameLength(int rate, int frame_length) {
  int return_value = -1;
  size_t i;
  int valid_length_ms;
  int valid_length;

  // We only allow 10, 20 or 30 ms frames. Loop through valid frame rates and
  // see if we have a matching pair.
  for (i = 0; i < kRatesSize; i++) {
    if (kValidRates[i] == rate) {
      for (valid_length_ms = 10; valid_length_ms <= kMaxFrameLengthMs;
          valid_length_ms += 10) {
        valid_length = (kValidRates[i] / 1000 * valid_length_ms);
        if (frame_length == valid_length) {
          return_value = 0;
          break;
        }
      }
      break;
    }
  }

  return return_value;
}
