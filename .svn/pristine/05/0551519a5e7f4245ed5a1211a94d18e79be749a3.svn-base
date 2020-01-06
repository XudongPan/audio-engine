#ifndef VOIP_SYSTEM_WRAPPERS_INTERFACE_LOGCAT_TRACE_CONTEXT_H_
#define VOIP_SYSTEM_WRAPPERS_INTERFACE_LOGCAT_TRACE_CONTEXT_H_

#include "audio_engine/system_wrappers/interface/trace.h"

#ifndef ANDROID
#error This file only makes sense to include on Android!
#endif

namespace VoIP {

// Scoped helper class for directing Traces to Android's logcat facility.  While
// this object lives, Trace output will be sent to logcat.
class LogcatTraceContext : public VoIP::TraceCallback {
 public:
  LogcatTraceContext();
  virtual ~LogcatTraceContext();

  // TraceCallback impl.
  virtual void Print(TraceLevel level, const char* message, int length);
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_INTERFACE_LOGCAT_TRACE_CONTEXT_H_
