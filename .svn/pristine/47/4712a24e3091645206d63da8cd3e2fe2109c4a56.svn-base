#include "audio_engine/system_wrappers/interface/logcat_trace_context.h"

#include <android/log.h>
#include <assert.h>

#include "audio_engine/system_wrappers/interface/logging.h"

namespace VoIP {

static android_LogPriority AndroidLogPriorityFromVoipLogLevel(
    TraceLevel voip_level) {
  // NOTE: this mapping is somewhat arbitrary.  StateInfo and Info are mapped
  // to DEBUG because they are highly verbose in VoIP code (which is
  // unfortunate).
  switch (voip_level) {
    case VoIP::kTraceStateInfo: return ANDROID_LOG_DEBUG;
    case VoIP::kTraceWarning: return ANDROID_LOG_WARN;
    case VoIP::kTraceError: return ANDROID_LOG_ERROR;
    case VoIP::kTraceCritical: return ANDROID_LOG_FATAL;
    case VoIP::kTraceApiCall: return ANDROID_LOG_VERBOSE;
    case VoIP::kTraceModuleCall: return ANDROID_LOG_VERBOSE;
    case VoIP::kTraceMemory: return ANDROID_LOG_VERBOSE;
    case VoIP::kTraceTimer: return ANDROID_LOG_VERBOSE;
    case VoIP::kTraceStream: return ANDROID_LOG_VERBOSE;
    case VoIP::kTraceDebug: return ANDROID_LOG_DEBUG;
    case VoIP::kTraceInfo: return ANDROID_LOG_DEBUG;
    case VoIP::kTraceTerseInfo: return ANDROID_LOG_INFO;
    default:
      LOG(LS_ERROR) << "Unexpected log level" << voip_level;
      return ANDROID_LOG_FATAL;
  }
}

LogcatTraceContext::LogcatTraceContext() {
  VoIP::Trace::CreateTrace();
  if (VoIP::Trace::SetTraceCallback(this) != 0)
    assert(false);
}

LogcatTraceContext::~LogcatTraceContext() {
  if (VoIP::Trace::SetTraceCallback(NULL) != 0)
    assert(false);
  VoIP::Trace::ReturnTrace();
}

void LogcatTraceContext::Print(TraceLevel level,
                               const char* message,
                               int length) {
  __android_log_print(AndroidLogPriorityFromVoipLogLevel(level),
                      "Voip", "%.*s", length, message);
}

}  // namespace VoIP
