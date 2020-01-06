/* The global function contained in this file initializes SPL function
 * pointers, currently only for ARM platforms.
 *
 * Some code came from common/rtcd.c in the WebM project.
 */

#include "audio_engine/common_audio/signal_processing/include/real_fft.h"
#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"
#include "audio_engine/system_wrappers/interface/cpu_features_wrapper.h"

/* Declare function pointers. */
MaxAbsValueW16 VoipSpl_MaxAbsValueW16;
MaxAbsValueW32 VoipSpl_MaxAbsValueW32;
MaxValueW16 VoipSpl_MaxValueW16;
MaxValueW32 VoipSpl_MaxValueW32;
MinValueW16 VoipSpl_MinValueW16;
MinValueW32 VoipSpl_MinValueW32;
CrossCorrelation VoipSpl_CrossCorrelation;
DownsampleFast VoipSpl_DownsampleFast;
ScaleAndAddVectorsWithRound VoipSpl_ScaleAndAddVectorsWithRound;
CreateRealFFT VoipSpl_CreateRealFFT;
FreeRealFFT VoipSpl_FreeRealFFT;
RealForwardFFT VoipSpl_RealForwardFFT;
RealInverseFFT VoipSpl_RealInverseFFT;

#if (defined(VOIP_DETECT_ARM_NEON) || !defined(VOIP_ARCH_ARM_NEON)) && \
     !defined(MIPS32_LE)
/* Initialize function pointers to the generic C version. */
static void InitPointersToC() {
  VoipSpl_MaxAbsValueW16 = VoipSpl_MaxAbsValueW16C;
  VoipSpl_MaxAbsValueW32 = VoipSpl_MaxAbsValueW32C;
  VoipSpl_MaxValueW16 = VoipSpl_MaxValueW16C;
  VoipSpl_MaxValueW32 = VoipSpl_MaxValueW32C;
  VoipSpl_MinValueW16 = VoipSpl_MinValueW16C;
  VoipSpl_MinValueW32 = VoipSpl_MinValueW32C;
  VoipSpl_CrossCorrelation = VoipSpl_CrossCorrelationC;
  VoipSpl_DownsampleFast = VoipSpl_DownsampleFastC;
  VoipSpl_ScaleAndAddVectorsWithRound =
      VoipSpl_ScaleAndAddVectorsWithRoundC;
  VoipSpl_CreateRealFFT = VoipSpl_CreateRealFFTC;
  VoipSpl_FreeRealFFT = VoipSpl_FreeRealFFTC;
  VoipSpl_RealForwardFFT = VoipSpl_RealForwardFFTC;
  VoipSpl_RealInverseFFT = VoipSpl_RealInverseFFTC;
}
#endif

#if defined(VOIP_DETECT_ARM_NEON) || defined(VOIP_ARCH_ARM_NEON)
/* Initialize function pointers to the Neon version. */
static void InitPointersToNeon() {
  VoipSpl_MaxAbsValueW16 = VoipSpl_MaxAbsValueW16Neon;
  VoipSpl_MaxAbsValueW32 = VoipSpl_MaxAbsValueW32Neon;
  VoipSpl_MaxValueW16 = VoipSpl_MaxValueW16Neon;
  VoipSpl_MaxValueW32 = VoipSpl_MaxValueW32Neon;
  VoipSpl_MinValueW16 = VoipSpl_MinValueW16Neon;
  VoipSpl_MinValueW32 = VoipSpl_MinValueW32Neon;
  VoipSpl_CrossCorrelation = VoipSpl_CrossCorrelationNeon;
  VoipSpl_DownsampleFast = VoipSpl_DownsampleFastNeon;
  VoipSpl_ScaleAndAddVectorsWithRound =
      VoipSpl_ScaleAndAddVectorsWithRoundNeon;
  VoipSpl_CreateRealFFT = VoipSpl_CreateRealFFTNeon;
  VoipSpl_FreeRealFFT = VoipSpl_FreeRealFFTNeon;
  VoipSpl_RealForwardFFT = VoipSpl_RealForwardFFTNeon;
  VoipSpl_RealInverseFFT = VoipSpl_RealInverseFFTNeon;
}
#endif

#if defined(MIPS32_LE)
/* Initialize function pointers to the MIPS version. */
static void InitPointersToMIPS() {
  VoipSpl_MaxAbsValueW16 = VoipSpl_MaxAbsValueW16_mips;
  VoipSpl_MaxValueW16 = VoipSpl_MaxValueW16_mips;
  VoipSpl_MaxValueW32 = VoipSpl_MaxValueW32_mips;
  VoipSpl_MinValueW16 = VoipSpl_MinValueW16_mips;
  VoipSpl_MinValueW32 = VoipSpl_MinValueW32_mips;
  VoipSpl_CrossCorrelation = VoipSpl_CrossCorrelation_mips;
  VoipSpl_DownsampleFast = VoipSpl_DownsampleFast_mips;
  VoipSpl_CreateRealFFT = VoipSpl_CreateRealFFTC;
  VoipSpl_FreeRealFFT = VoipSpl_FreeRealFFTC;
  VoipSpl_RealForwardFFT = VoipSpl_RealForwardFFTC;
  VoipSpl_RealInverseFFT = VoipSpl_RealInverseFFTC;
#if defined(MIPS_DSP_R1_LE)
  VoipSpl_MaxAbsValueW32 = VoipSpl_MaxAbsValueW32_mips;
  VoipSpl_ScaleAndAddVectorsWithRound =
      VoipSpl_ScaleAndAddVectorsWithRound_mips;
#else
  VoipSpl_MaxAbsValueW32 = VoipSpl_MaxAbsValueW32C;
  VoipSpl_ScaleAndAddVectorsWithRound =
      VoipSpl_ScaleAndAddVectorsWithRoundC;
#endif
}
#endif

static void InitFunctionPointers(void) {
#if defined(VOIP_DETECT_ARM_NEON)
  if ((Voip_GetCPUFeaturesARM() & kCPUFeatureNEON) != 0) {
    InitPointersToNeon();
  } else {
    InitPointersToC();
  }
#elif defined(VOIP_ARCH_ARM_NEON)
  InitPointersToNeon();
#elif defined(MIPS32_LE)
  InitPointersToMIPS();
#else
  InitPointersToC();
#endif  /* VOIP_DETECT_ARM_NEON */
}

#if defined(VOIP_POSIX)
#include <pthread.h>

static void once(void (*func)(void)) {
  static pthread_once_t lock = PTHREAD_ONCE_INIT;
  pthread_once(&lock, func);
}

#elif defined(_WIN32)
#include <windows.h>

static void once(void (*func)(void)) {
  /* Didn't use InitializeCriticalSection() since there's no race-free context
   * in which to execute it.
   *
   * TODO(kma): Change to different implementation (e.g.
   * InterlockedCompareExchangePointer) to avoid issues similar to
   * http://code.google.com/p/webm/issues/detail?id=467.
   */
  static CRITICAL_SECTION lock = {(void *)((size_t)-1), -1, 0, 0, 0, 0};
  static int done = 0;

  EnterCriticalSection(&lock);
  if (!done) {
    func();
    done = 1;
  }
  LeaveCriticalSection(&lock);
}

/* There's no fallback version as an #else block here to ensure thread safety.
 * In case of neither pthread for VOIP_POSIX nor _WIN32 is present, build
 * system should pick it up.
 */
#endif  /* VOIP_POSIX */

void VoipSpl_Init() {
  once(InitFunctionPointers);
}
