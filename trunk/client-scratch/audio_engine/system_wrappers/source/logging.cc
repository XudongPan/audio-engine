#include "audio_engine/system_wrappers/interface/logging.h"

#include <string.h>

#include <sstream>

#include "audio_engine/include/common_types.h"
#include "audio_engine/system_wrappers/interface/trace.h"

namespace VoIP {
namespace {

TraceLevel VoipSeverity(LoggingSeverity sev) {
  switch (sev) {
    // TODO(ajm): SENSITIVE doesn't have a corresponding VoIP level.
    case LS_SENSITIVE:  return kTraceInfo;
    case LS_VERBOSE:    return kTraceInfo;
    case LS_INFO:       return kTraceTerseInfo;
    case LS_WARNING:    return kTraceWarning;
    case LS_ERROR:      return kTraceError;
    default:            return kTraceNone;
  }
}

const char* DescribeFile(const char* file) {
  const char* end1 = ::strrchr(file, '/');
  const char* end2 = ::strrchr(file, '\\');
  if (!end1 && !end2)
    return file;
  else
    return (end1 > end2) ? end1 + 1 : end2 + 1;
}

}  // namespace

LogMessage::LogMessage(const char* file, int line, LoggingSeverity sev)
    : severity_(sev) {
  print_stream_ << "(" << DescribeFile(file) << ":" << line << "): ";
}

bool LogMessage::Loggable(LoggingSeverity sev) {
  // |level_filter| is a bitmask, unlike libjingle's minimum severity value.
  return VoipSeverity(sev) & Trace::level_filter() ? true : false;
}

LogMessage::~LogMessage() {
  const std::string& str = print_stream_.str();
  Trace::Add(VoipSeverity(severity_), kTraceUndefined, 0, str.c_str());
}

}  // namespace VoIP
