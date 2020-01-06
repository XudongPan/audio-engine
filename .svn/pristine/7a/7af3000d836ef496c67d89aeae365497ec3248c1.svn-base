
/*
 * Specifies the interface for the AEC core.
 */

#ifndef VOIP_MODULES_AUDIO_PROCESSING_AEC_AEC_CORE_H_
#define VOIP_MODULES_AUDIO_PROCESSING_AEC_AEC_CORE_H_

#include "audio_engine/include/typedefs.h"

#define FRAME_LEN 80
#define PART_LEN 64               // Length of partition
#define PART_LEN1 (PART_LEN + 1)  // Unique fft coefficients
#define PART_LEN2 (PART_LEN * 2)  // Length of partition * 2

// Delay estimator constants, used for logging.
enum {
  kMaxDelayBlocks = 60
};
enum {
  kLookaheadBlocks = 15
};
enum {
  kHistorySizeBlocks = kMaxDelayBlocks + kLookaheadBlocks
};

typedef float complex_t[2];
// For performance reasons, some arrays of complex numbers are replaced by twice
// as long arrays of float, all the real parts followed by all the imaginary
// ones (complex_t[SIZE] -> float[2][SIZE]). This allows SIMD optimizations and
// is better than two arrays (one for the real parts and one for the imaginary
// parts) as this other way would require two pointers instead of one and cause
// extra register spilling. This also allows the offsets to be calculated at
// compile time.

// Metrics
enum {
  kOffsetLevel = -100
};

typedef struct Stats {
  float instant;
  float average;
  float min;
  float max;
  float sum;
  float hisum;
  float himean;
  int counter;
  int hicounter;
} Stats;

typedef struct AecCore AecCore;

int VoipAec_CreateAec(AecCore** aec);
int VoipAec_FreeAec(AecCore* aec);
int VoipAec_InitAec(AecCore* aec, int sampFreq);
void VoipAec_InitAec_SSE2(void);
#if defined(MIPS_FPU_LE)
void VoipAec_InitAec_mips(void);
#endif

void VoipAec_BufferFarendPartition(AecCore* aec, const float* farend);
void VoipAec_ProcessFrame(AecCore* aec,
                            const short* nearend,
                            const short* nearendH,
                            int knownDelay,
                            int16_t* out,
                            int16_t* outH);

// A helper function to call VoIP_MoveReadPtr() for all far-end buffers.
// Returns the number of elements moved, and adjusts |system_delay| by the
// corresponding amount in ms.
int VoipAec_MoveFarReadPtr(AecCore* aec, int elements);

// Calculates the median and standard deviation among the delay estimates
// collected since the last call to this function.
int VoipAec_GetDelayMetricsCore(AecCore* self, int* median, int* std);

// Returns the echo state (1: echo, 0: no echo).
int VoipAec_echo_state(AecCore* self);

// Gets statistics of the echo metrics ERL, ERLE, A_NLP.
void VoipAec_GetEchoStats(AecCore* self,
                            Stats* erl,
                            Stats* erle,
                            Stats* a_nlp);
#ifdef VOIP_AEC_DEBUG_DUMP
void* VoipAec_far_time_buf(AecCore* self);
#endif

// Sets local configuration modes.
void VoipAec_SetConfigCore(AecCore* self,
                             int nlp_mode,
                             int metrics_mode,
                             int delay_logging);

// We now interpret delay correction to mean an extended filter length feature.
// We reuse the delay correction infrastructure to avoid changes through to
// libjingle. See details along with |DelayCorrection| in
// echo_cancellation_impl.h. Non-zero enables, zero disables.
void VoipAec_enable_delay_correction(AecCore* self, int enable);

// Returns non-zero if delay correction is enabled and zero if disabled.
int VoipAec_delay_correction_enabled(AecCore* self);

// Returns the current |system_delay|, i.e., the buffered difference between
// far-end and near-end.
int VoipAec_system_delay(AecCore* self);

// Sets the |system_delay| to |value|.  Note that if the value is changed
// improperly, there can be a performance regression.  So it should be used with
// care.
void VoipAec_SetSystemDelay(AecCore* self, int delay);

#endif  // VOIP_MODULES_AUDIO_PROCESSING_AEC_AEC_CORE_H_
