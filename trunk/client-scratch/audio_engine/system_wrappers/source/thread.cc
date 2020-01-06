
#include "audio_engine/system_wrappers/interface/thread_wrapper.h"

#if defined(_WIN32)
#include "audio_engine/system_wrappers/source/thread_win.h"
#else
#include "audio_engine/system_wrappers/source/thread_posix.h"
#endif

namespace VoIP {

ThreadWrapper* ThreadWrapper::CreateThread(ThreadRunFunction func,
                                           ThreadObj obj, ThreadPriority prio,
                                           const char* thread_name) {
#if defined(_WIN32)
  return new ThreadWindows(func, obj, prio, thread_name);
#else
  return ThreadPosix::Create(func, obj, prio, thread_name);
#endif
}

bool ThreadWrapper::SetAffinity(const int* processor_numbers,
                                const unsigned int amount_of_processors) {
  return false;
}

}  // namespace VoIP
