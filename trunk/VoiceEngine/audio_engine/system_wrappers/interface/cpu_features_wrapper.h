#ifndef VOIP_SYSTEM_WRAPPERS_INTERFACE_CPU_FEATURES_WRAPPER_H_
#define VOIP_SYSTEM_WRAPPERS_INTERFACE_CPU_FEATURES_WRAPPER_H_

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "audio_engine/include/typedefs.h"

// List of features in x86.
typedef enum {
  kSSE2,
  kSSE3
} CPUFeature;

// List of features in ARM.
enum {
  kCPUFeatureARMv7       = (1 << 0),
  kCPUFeatureVFPv3       = (1 << 1),
  kCPUFeatureNEON        = (1 << 2),
  kCPUFeatureLDREXSTREX  = (1 << 3)
};

typedef int (*Voip_CPUInfo)(CPUFeature feature);

// Returns true if the CPU supports the feature.
extern Voip_CPUInfo Voip_GetCPUInfo;

// No CPU feature is available => straight C path.
extern Voip_CPUInfo Voip_GetCPUInfoNoASM;

// Return the features in an ARM device.
// It detects the features in the hardware platform, and returns supported
// values in the above enum definition as a bitmask.
extern uint64_t Voip_GetCPUFeaturesARM(void);

#if defined(__cplusplus) || defined(c_plusplus)
}  // extern "C"
#endif

#endif // VOIP_SYSTEM_WRAPPERS_INTERFACE_CPU_FEATURES_WRAPPER_H_
