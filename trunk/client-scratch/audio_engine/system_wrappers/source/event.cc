
#include "audio_engine/system_wrappers/interface/event_wrapper.h"

#if defined(_WIN32)
#include <windows.h>
#include "audio_engine/system_wrappers/source/event_win.h"
#elif defined(VOIP_MAC) && !defined(VOIP_IOS)
#include <ApplicationServices/ApplicationServices.h>
#include <pthread.h>
#include "audio_engine/system_wrappers/source/event_posix.h"
#else
#include <pthread.h>
#include "audio_engine/system_wrappers/source/event_posix.h"
#endif

namespace VoIP {
EventWrapper* EventWrapper::Create() {
#if defined(_WIN32)
  return new EventWindows();
#else
  return EventPosix::Create();
#endif
}
}  // namespace VoIP
