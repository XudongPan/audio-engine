#ifndef VOIP_MODULES_AUDIO_PROCESSING_AEC_ECHO_CANCELLATION_INTERNAL_H_
#define VOIP_MODULES_AUDIO_PROCESSING_AEC_ECHO_CANCELLATION_INTERNAL_H_

#include "audio_engine/modules/audio_processing/aec/aec_core.h"
#include "audio_engine/modules/audio_processing/utility/ring_buffer.h"

typedef struct {
  int delayCtr;
  int sampFreq;
  int splitSampFreq;
  int scSampFreq;
  float sampFactor;  // scSampRate / sampFreq
  short skewMode;
  int bufSizeStart;
  int knownDelay;
  int rate_factor;

  short initFlag;  // indicates if AEC has been initialized

  // Variables used for averaging far end buffer size
  short counter;
  int sum;
  short firstVal;
  short checkBufSizeCtr;

  // Variables used for delay shifts
  short msInSndCardBuf;
  short filtDelay;  // Filtered delay estimate.
  int timeForDelayChange;
  int startup_phase;
  int checkBuffSize;
  short lastDelayDiff;

#ifdef VOIP_AEC_DEBUG_DUMP
  RingBuffer* far_pre_buf_s16;  // Time domain far-end pre-buffer in int16_t.
  FILE* bufFile;
  FILE* delayFile;
  FILE* skewFile;
#endif

  // Structures
  void* resampler;

  int skewFrCtr;
  int resample;  // if the skew is small enough we don't resample
  int highSkewCtr;
  float skew;

  RingBuffer* far_pre_buf;  // Time domain far-end pre-buffer.

  int lastError;

  int farend_started;

  AecCore* aec;
} aecpc_t;

#endif  // VOIP_MODULES_AUDIO_PROCESSING_AEC_ECHO_CANCELLATION_INTERNAL_H_
