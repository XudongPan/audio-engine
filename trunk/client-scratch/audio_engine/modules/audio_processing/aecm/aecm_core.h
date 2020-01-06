// Performs echo control (suppression) with fft routines in fixed-point.

#ifndef VOIP_MODULES_AUDIO_PROCESSING_AECM_AECM_CORE_H_
#define VOIP_MODULES_AUDIO_PROCESSING_AECM_AECM_CORE_H_

#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"
#include "audio_engine/modules/audio_processing/aecm/aecm_defines.h"
#include "audio_engine/modules/audio_processing/utility/ring_buffer.h"
#include "audio_engine/include/typedefs.h"

#ifdef _MSC_VER  // visual c++
#define ALIGN8_BEG __declspec(align(8))
#define ALIGN8_END
#else  // gcc or icc
#define ALIGN8_BEG
#define ALIGN8_END __attribute__((aligned(8)))
#endif

typedef struct {
    int16_t real;
    int16_t imag;
} complex16_t;

typedef struct {
    int farBufWritePos;
    int farBufReadPos;
    int knownDelay;
    int lastKnownDelay;
    int firstVAD;  // Parameter to control poorly initialized channels

    RingBuffer* farFrameBuf;
    RingBuffer* nearNoisyFrameBuf;
    RingBuffer* nearCleanFrameBuf;
    RingBuffer* outFrameBuf;

    int16_t farBuf[FAR_BUF_LEN];

    int16_t mult;
    uint32_t seed;

    // Delay estimation variables
    void* delay_estimator_farend;
    void* delay_estimator;
    uint16_t currentDelay;
    // Far end history variables
    // TODO(bjornv): Replace |far_history| with ring_buffer.
    uint16_t far_history[PART_LEN1 * MAX_DELAY];
    int far_history_pos;
    int far_q_domains[MAX_DELAY];

    int16_t nlpFlag;
    int16_t fixedDelay;

    uint32_t totCount;

    int16_t dfaCleanQDomain;
    int16_t dfaCleanQDomainOld;
    int16_t dfaNoisyQDomain;
    int16_t dfaNoisyQDomainOld;

    int16_t nearLogEnergy[MAX_BUF_LEN];
    int16_t farLogEnergy;
    int16_t echoAdaptLogEnergy[MAX_BUF_LEN];
    int16_t echoStoredLogEnergy[MAX_BUF_LEN];

    // The extra 16 or 32 bytes in the following buffers are for alignment based
    // Neon code.
    // It's designed this way since the current GCC compiler can't align a
    // buffer in 16 or 32 byte boundaries properly.
    int16_t channelStored_buf[PART_LEN1 + 8];
    int16_t channelAdapt16_buf[PART_LEN1 + 8];
    int32_t channelAdapt32_buf[PART_LEN1 + 8];
    int16_t xBuf_buf[PART_LEN2 + 16];  // farend
    int16_t dBufClean_buf[PART_LEN2 + 16];  // nearend
    int16_t dBufNoisy_buf[PART_LEN2 + 16];  // nearend
    int16_t outBuf_buf[PART_LEN + 8];

    // Pointers to the above buffers
    int16_t *channelStored;
    int16_t *channelAdapt16;
    int32_t *channelAdapt32;
    int16_t *xBuf;
    int16_t *dBufClean;
    int16_t *dBufNoisy;
    int16_t *outBuf;

    int32_t echoFilt[PART_LEN1];
    int16_t nearFilt[PART_LEN1];
    int32_t noiseEst[PART_LEN1];
    int           noiseEstTooLowCtr[PART_LEN1];
    int           noiseEstTooHighCtr[PART_LEN1];
    int16_t noiseEstCtr;
    int16_t cngMode;

    int32_t mseAdaptOld;
    int32_t mseStoredOld;
    int32_t mseThreshold;

    int16_t farEnergyMin;
    int16_t farEnergyMax;
    int16_t farEnergyMaxMin;
    int16_t farEnergyVAD;
    int16_t farEnergyMSE;
    int currentVADValue;
    int16_t vadUpdateCount;

    int16_t startupState;
    int16_t mseChannelCount;
    int16_t supGain;
    int16_t supGainOld;

    int16_t supGainErrParamA;
    int16_t supGainErrParamD;
    int16_t supGainErrParamDiffAB;
    int16_t supGainErrParamDiffBD;

    struct RealFFT* real_fft;

#ifdef AEC_DEBUG
    FILE *farFile;
    FILE *nearFile;
    FILE *outFile;
#endif
} AecmCore_t;

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_CreateCore(...)
//
// Allocates the memory needed by the AECM. The memory needs to be
// initialized separately using the VoIPAecm_InitCore() function.
//
// Input:
//      - aecm          : Instance that should be created
//
// Output:
//      - aecm          : Created instance
//
// Return value         :  0 - Ok
//                        -1 - Error
//
int VoipAecm_CreateCore(AecmCore_t **aecm);

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_InitCore(...)
//
// This function initializes the AECM instant created with
// VoIPAecm_CreateCore(...)
// Input:
//      - aecm          : Pointer to the AECM instance
//      - samplingFreq  : Sampling Frequency
//
// Output:
//      - aecm          : Initialized instance
//
// Return value         :  0 - Ok
//                        -1 - Error
//
int VoipAecm_InitCore(AecmCore_t * const aecm, int samplingFreq);

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_FreeCore(...)
//
// This function releases the memory allocated by VoIPAecm_CreateCore()
// Input:
//      - aecm          : Pointer to the AECM instance
//
// Return value         :  0 - Ok
//                        -1 - Error
//           11001-11016: Error
//
int VoipAecm_FreeCore(AecmCore_t *aecm);

int VoipAecm_Control(AecmCore_t *aecm, int delay, int nlpFlag);

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_InitEchoPathCore(...)
//
// This function resets the echo channel adaptation with the specified channel.
// Input:
//      - aecm          : Pointer to the AECM instance
//      - echo_path     : Pointer to the data that should initialize the echo
//                        path
//
// Output:
//      - aecm          : Initialized instance
//
void VoipAecm_InitEchoPathCore(AecmCore_t* aecm,
                                 const int16_t* echo_path);

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_ProcessFrame(...)
//
// This function processes frames and sends blocks to
// VoIPAecm_ProcessBlock(...)
//
// Inputs:
//      - aecm          : Pointer to the AECM instance
//      - farend        : In buffer containing one frame of echo signal
//      - nearendNoisy  : In buffer containing one frame of nearend+echo signal
//                        without NS
//      - nearendClean  : In buffer containing one frame of nearend+echo signal
//                        with NS
//
// Output:
//      - out           : Out buffer, one frame of nearend signal          :
//
//
int VoipAecm_ProcessFrame(AecmCore_t * aecm, const int16_t * farend,
                            const int16_t * nearendNoisy,
                            const int16_t * nearendClean,
                            int16_t * out);

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_ProcessBlock(...)
//
// This function is called for every block within one frame
// This function is called by VoIPAecm_ProcessFrame(...)
//
// Inputs:
//      - aecm          : Pointer to the AECM instance
//      - farend        : In buffer containing one block of echo signal
//      - nearendNoisy  : In buffer containing one frame of nearend+echo signal
//                        without NS
//      - nearendClean  : In buffer containing one frame of nearend+echo signal
//                        with NS
//
// Output:
//      - out           : Out buffer, one block of nearend signal          :
//
//
int VoipAecm_ProcessBlock(AecmCore_t * aecm, const int16_t * farend,
                            const int16_t * nearendNoisy,
                            const int16_t * noisyClean,
							int16_t * out);

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_BufferFarFrame()
//
// Inserts a frame of data into farend buffer.
//
// Inputs:
//      - aecm          : Pointer to the AECM instance
//      - farend        : In buffer containing one frame of farend signal
//      - farLen        : Length of frame
//
void VoipAecm_BufferFarFrame(AecmCore_t * const aecm,
                               const int16_t * const farend,
                               const int farLen);

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_FetchFarFrame()
//
// Read the farend buffer to account for known delay
//
// Inputs:
//      - aecm          : Pointer to the AECM instance
//      - farend        : In buffer containing one frame of farend signal
//      - farLen        : Length of frame
//      - knownDelay    : known delay
//
void VoipAecm_FetchFarFrame(AecmCore_t * const aecm,
                              int16_t * const farend,
                              const int farLen, const int knownDelay);


// All the functions below are intended to be private

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_UpdateFarHistory()
//
// Moves the pointer to the next entry and inserts |far_spectrum| and
// corresponding Q-domain in its buffer.
//
// Inputs:
//      - self          : Pointer to the delay estimation instance
//      - far_spectrum  : Pointer to the far end spectrum
//      - far_q         : Q-domain of far end spectrum
//
void VoipAecm_UpdateFarHistory(AecmCore_t* self,
                                 uint16_t* far_spectrum,
                                 int far_q);

////////////////////////////////////////////////////////////////////////////////
// VoIPAecm_AlignedFarend()
//
// Returns a pointer to the far end spectrum aligned to current near end
// spectrum. The function VoIP_DelayEstimatorProcessFix(...) should have been
// called before AlignedFarend(...). Otherwise, you get the pointer to the
// previous frame. The memory is only valid until the next call of
// VoIP_DelayEstimatorProcessFix(...).
//
// Inputs:
//      - self              : Pointer to the AECM instance.
//      - delay             : Current delay estimate.
//
// Output:
//      - far_q             : The Q-domain of the aligned far end spectrum
//
// Return value:
//      - far_spectrum      : Pointer to the aligned far end spectrum
//                            NULL - Error
//
const uint16_t* VoipAecm_AlignedFarend(AecmCore_t* self,
                                         int* far_q,
                                         int delay);

///////////////////////////////////////////////////////////////////////////////
// VoIPAecm_CalcSuppressionGain()
//
// This function calculates the suppression gain that is used in the
// Wiener filter.
//
// Inputs:
//      - aecm              : Pointer to the AECM instance.
//
// Return value:
//      - supGain           : Suppression gain with which to scale the noise
//                            level (Q14).
//
int16_t VoipAecm_CalcSuppressionGain(AecmCore_t * const aecm);

///////////////////////////////////////////////////////////////////////////////
// VoIPAecm_CalcEnergies()
//
// This function calculates the log of energies for nearend, farend and
// estimated echoes. There is also an update of energy decision levels,
// i.e. internal VAD.
//
// Inputs:
//      - aecm              : Pointer to the AECM instance.
//      - far_spectrum      : Pointer to farend spectrum.
//      - far_q             : Q-domain of farend spectrum.
//      - nearEner          : Near end energy for current block in
//                            Q(aecm->dfaQDomain).
//
// Output:
//     - echoEst            : Estimated echo in Q(xfa_q+RESOLUTION_CHANNEL16).
//
void VoipAecm_CalcEnergies(AecmCore_t * aecm,
                             const uint16_t* far_spectrum,
                             const int16_t far_q,
                             const uint32_t nearEner,
                             int32_t * echoEst);

///////////////////////////////////////////////////////////////////////////////
// VoIPAecm_CalcStepSize()
//
// This function calculates the step size used in channel estimation
//
// Inputs:
//      - aecm              : Pointer to the AECM instance.
//
// Return value:
//      - mu                : Stepsize in log2(), i.e. number of shifts.
//
int16_t VoipAecm_CalcStepSize(AecmCore_t * const aecm);

///////////////////////////////////////////////////////////////////////////////
// VoIPAecm_UpdateChannel(...)
//
// This function performs channel estimation.
// NLMS and decision on channel storage.
//
// Inputs:
//      - aecm              : Pointer to the AECM instance.
//      - far_spectrum      : Absolute value of the farend signal in Q(far_q)
//      - far_q             : Q-domain of the farend signal
//      - dfa               : Absolute value of the nearend signal
//                            (Q[aecm->dfaQDomain])
//      - mu                : NLMS step size.
// Input/Output:
//      - echoEst           : Estimated echo in Q(far_q+RESOLUTION_CHANNEL16).
//
void VoipAecm_UpdateChannel(AecmCore_t * aecm,
                              const uint16_t* far_spectrum,
                              const int16_t far_q,
                              const uint16_t * const dfa,
                              const int16_t mu,
                              int32_t * echoEst);

extern const int16_t VoipAecm_kCosTable[];
extern const int16_t VoipAecm_kSinTable[];

///////////////////////////////////////////////////////////////////////////////
// Some function pointers, for internal functions shared by ARM NEON and
// generic C code.
//
typedef void (*CalcLinearEnergies)(
    AecmCore_t* aecm,
    const uint16_t* far_spectrum,
    int32_t* echoEst,
    uint32_t* far_energy,
    uint32_t* echo_energy_adapt,
    uint32_t* echo_energy_stored);
extern CalcLinearEnergies VoipAecm_CalcLinearEnergies;

typedef void (*StoreAdaptiveChannel)(
    AecmCore_t* aecm,
    const uint16_t* far_spectrum,
    int32_t* echo_est);
extern StoreAdaptiveChannel VoipAecm_StoreAdaptiveChannel;

typedef void (*ResetAdaptiveChannel)(AecmCore_t* aecm);
extern ResetAdaptiveChannel VoipAecm_ResetAdaptiveChannel;

// For the above function pointers, functions for generic platforms are declared
// and defined as static in file aecm_core.c, while those for ARM Neon platforms
// are declared below and defined in file aecm_core_neon.s.
#if (defined VOIP_DETECT_ARM_NEON) || defined (VOIP_ARCH_ARM_NEON)
void VoipAecm_CalcLinearEnergiesNeon(AecmCore_t* aecm,
                                       const uint16_t* far_spectrum,
                                       int32_t* echo_est,
                                       uint32_t* far_energy,
                                       uint32_t* echo_energy_adapt,
                                       uint32_t* echo_energy_stored);

void VoipAecm_StoreAdaptiveChannelNeon(AecmCore_t* aecm,
                                         const uint16_t* far_spectrum,
                                         int32_t* echo_est);

void VoipAecm_ResetAdaptiveChannelNeon(AecmCore_t* aecm);
#endif

#if defined(MIPS32_LE)
void VoipAecm_CalcLinearEnergies_mips(AecmCore_t* aecm,
                                        const uint16_t* far_spectrum,
                                        int32_t* echo_est,
                                        uint32_t* far_energy,
                                        uint32_t* echo_energy_adapt,
                                        uint32_t* echo_energy_stored);
#if defined(MIPS_DSP_R1_LE)
void VoipAecm_StoreAdaptiveChannel_mips(AecmCore_t* aecm,
                                          const uint16_t* far_spectrum,
                                          int32_t* echo_est);

void VoipAecm_ResetAdaptiveChannel_mips(AecmCore_t* aecm);
#endif
#endif

#endif
