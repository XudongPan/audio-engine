
#include "audio_engine/system_wrappers/interface/condition_variable_wrapper.h"

#if defined(_WIN32)
#include <windows.h>
#include "audio_engine/system_wrappers/source/condition_variable_event_win.h"
#include "audio_engine/system_wrappers/source/condition_variable_native_win.h"
#elif defined(VOIP_LINUX) || defined(VOIP_MAC)
#include <pthread.h>
#include "audio_engine/system_wrappers/source/condition_variable_posix.h"
#endif

namespace VoIP {

ConditionVariableWrapper* ConditionVariableWrapper::CreateConditionVariable() {
#if defined(_WIN32)
  // Try to create native condition variable implementation.
  ConditionVariableWrapper* ret_val = ConditionVariableNativeWin::Create();
  if (!ret_val) {
    // Native condition variable implementation does not exist. Create generic
    // condition variable based on events.
    ret_val = new ConditionVariableEventWin();
  }
  return ret_val;
#elif defined(VOIP_LINUX) || defined(VOIP_MAC)
  return ConditionVariablePosix::Create();
#else
  return NULL;
#endif
}

}  // namespace VoIP
