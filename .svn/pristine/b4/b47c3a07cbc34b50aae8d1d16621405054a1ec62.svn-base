
#include "audio_engine/system_wrappers/interface/cpu_info.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(VOIP_MAC)
#include <sys/sysctl.h>
#include <sys/types.h>
#else // defined(VOIP_LINUX) or defined(VOIP_ANDROID)
#include <unistd.h>
#endif

#include "audio_engine/system_wrappers/interface/trace.h"

namespace VoIP {

uint32_t CpuInfo::number_of_cores_ = 0;

uint32_t CpuInfo::DetectNumberOfCores() {
  if (!number_of_cores_) {
#if defined(_WIN32)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    number_of_cores_ = static_cast<uint32_t>(si.dwNumberOfProcessors);
    VOIP_TRACE(kTraceStateInfo, kTraceUtility, -1,
                 "Available number of cores:%d", number_of_cores_);

#elif defined(VOIP_LINUX) || defined(VOIP_ANDROID)
    number_of_cores_ = static_cast<uint32_t>(sysconf(_SC_NPROCESSORS_ONLN));
    VOIP_TRACE(kTraceStateInfo, kTraceUtility, -1,
                 "Available number of cores:%d", number_of_cores_);

#elif defined(VOIP_MAC)
    int name[] = {CTL_HW, HW_AVAILCPU};
    int ncpu;
    size_t size = sizeof(ncpu);
    if (0 == sysctl(name, 2, &ncpu, &size, NULL, 0)) {
      number_of_cores_ = static_cast<uint32_t>(ncpu);
      VOIP_TRACE(kTraceStateInfo, kTraceUtility, -1,
                   "Available number of cores:%d", number_of_cores_);
    } else {
      VOIP_TRACE(kTraceError, kTraceUtility, -1,
                   "Failed to get number of cores");
      number_of_cores_ = 1;
    }
#else
    VOIP_TRACE(kTraceWarning, kTraceUtility, -1,
                 "No function to get number of cores");
    number_of_cores_ = 1;
#endif
  }
  return number_of_cores_;
}

}  // namespace VoIP
