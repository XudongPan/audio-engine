// Parts of this file derived from Chromium's base/cpu.cc.

#include "audio_engine/system_wrappers/interface/cpu_features_wrapper.h"

#if defined(VOIP_ARCH_X86_FAMILY) && defined(_MSC_VER)
#include <intrin.h>
#endif

#include "audio_engine/include/typedefs.h"

// No CPU feature is available => straight C path.
int GetCPUInfoNoASM(CPUFeature feature) {
  (void)feature;
  return 0;
}

#if defined(VOIP_ARCH_X86_FAMILY)
#ifndef _MSC_VER
// Intrinsic for "cpuid".
#if defined(__pic__) && defined(__i386__)
static inline void __cpuid(int cpu_info[4], int info_type) {
  __asm__ volatile(
    "mov %%ebx, %%edi\n"
    "cpuid\n"
    "xchg %%edi, %%ebx\n"
    : "=a"(cpu_info[0]), "=D"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
    : "a"(info_type));
}
#else
static inline void __cpuid(int cpu_info[4], int info_type) {
  __asm__ volatile(
    "cpuid\n"
    : "=a"(cpu_info[0]), "=b"(cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
    : "a"(info_type));
}
#endif
#endif  // _MSC_VER
#endif  // VOIP_ARCH_X86_FAMILY

#if defined(VOIP_ARCH_X86_FAMILY)
// Actual feature detection for x86.
static int GetCPUInfo(CPUFeature feature) {
  int cpu_info[4];
  __cpuid(cpu_info, 1);
  if (feature == kSSE2) {
    return 0 != (cpu_info[3] & 0x04000000);
  }
  if (feature == kSSE3) {
    return 0 != (cpu_info[2] & 0x00000001);
  }
  return 0;
}
#else
// Default to straight C for other platforms.
static int GetCPUInfo(CPUFeature feature) {
  (void)feature;
  return 0;
}
#endif

Voip_CPUInfo Voip_GetCPUInfo = GetCPUInfo;
Voip_CPUInfo Voip_GetCPUInfoNoASM = GetCPUInfoNoASM;
