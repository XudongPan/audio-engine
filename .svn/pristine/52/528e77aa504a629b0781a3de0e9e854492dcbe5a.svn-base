#if defined(_WIN32)
#include <windows.h>
#include "audio_engine/system_wrappers/source/critical_section_win.h"
#else
#include "audio_engine/system_wrappers/source/critical_section_posix.h"
#endif

namespace VoIP {

CriticalSectionWrapper* CriticalSectionWrapper::CreateCriticalSection() {
#ifdef _WIN32
  return new CriticalSectionWindows();
#else
  return new CriticalSectionPosix();
#endif
}

}  // namespace VoIP
