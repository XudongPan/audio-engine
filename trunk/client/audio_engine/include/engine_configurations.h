#ifndef VOIP_ENGINE_CONFIGURATIONS_H_
#define VOIP_ENGINE_CONFIGURATIONS_H_

#include "audio_engine/include/typedefs.h"

// ----------------------------------------------------------------------------
//  [Voice] Codec settings
// ----------------------------------------------------------------------------
 
// PCM16 is useful for testing and incurs only a small binary size cost.
#define VOIP_CODEC_PCM16

// ============================================================================
//                                 Audio Engine
// ============================================================================

// ----------------------------------------------------------------------------
//  Settings for Audio Engine
// ----------------------------------------------------------------------------

#define VOIP_AUDIO_ENGINE_AGC                 // Near-end AGC
#define VOIP_AUDIO_ENGINE_ECHO                // Near-end AEC
#define VOIP_AUDIO_ENGINE_NR                  // Near-end NS
#define VOIP_AOE_EXTERNAL_REC_AND_PLAYOUT

#if !defined(VOIP_ANDROID) && !defined(VOIP_IOS)
//#define VOIP_AUDIO_ENGINE_TYPING_DETECTION    // Typing detection
#endif

// ----------------------------------------------------------------------------
//  Audio Engine sub-APIs
// ----------------------------------------------------------------------------

#define VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
#define VOIP_AUDIO_ENGINE_CODEC_API
#define VOIP_AUDIO_ENGINE_HARDWARE_API
#define VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API



#endif  // VOIP_ENGINE_CONFIGURATIONS_H_
