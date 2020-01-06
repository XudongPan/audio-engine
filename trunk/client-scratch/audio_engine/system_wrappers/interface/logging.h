
#ifndef VOIP_SYSTEM_WRAPPERS_INTERFACE_LOGGING_H_
#define VOIP_SYSTEM_WRAPPERS_INTERFACE_LOGGING_H_

#include <sstream>

namespace VoIP {

//////////////////////////////////////////////////////////////////////

// Note that the non-standard LoggingSeverity aliases exist because they are
// still in broad use.  The meanings of the levels are:
//  LS_SENSITIVE: Information which should only be logged with the consent
//   of the user, due to privacy concerns.
//  LS_VERBOSE: This level is for data which we do not want to appear in the
//   normal debug log, but should appear in diagnostic logs.
//  LS_INFO: Chatty level used in debugging for all sorts of things, the default
//   in debug builds.
//  LS_WARNING: Something that may warrant investigation.
//  LS_ERROR: Something that should not have occurred.
enum LoggingSeverity {
  LS_SENSITIVE, LS_VERBOSE, LS_INFO, LS_WARNING, LS_ERROR
};

class LogMessage {
 public:
  LogMessage(const char* file, int line, LoggingSeverity sev);
  ~LogMessage();

  static bool Loggable(LoggingSeverity sev);
  std::ostream& stream() { return print_stream_; }

 private:
  // The ostream that buffers the formatted message before output
  std::ostringstream print_stream_;

  // The severity level of this message
  LoggingSeverity severity_;
};

//////////////////////////////////////////////////////////////////////
// Macros which automatically disable logging when VOIP_LOGGING == 0
//////////////////////////////////////////////////////////////////////

#ifndef LOG
// The following non-obvious technique for implementation of a
// conditional log stream was stolen from google3/base/logging.h.

// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".

class LogMessageVoidify {
 public:
  LogMessageVoidify() { }
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) { }
};

#if defined(VOIP_RESTRICT_LOGGING)
// This should compile away logs matching the following condition.
#define RESTRICT_LOGGING_PRECONDITION(sev)  \
  sev < VoIP::LS_INFO ? (void) 0 :
#else
#define RESTRICT_LOGGING_PRECONDITION(sev)
#endif

#define LOG_SEVERITY_PRECONDITION(sev) \
  RESTRICT_LOGGING_PRECONDITION(sev) !(VoIP::LogMessage::Loggable(sev)) \
    ? (void) 0 \
    : VoIP::LogMessageVoidify() &

#define LOG(sev) \
  LOG_SEVERITY_PRECONDITION(VoIP::sev) \
    VoIP::LogMessage(__FILE__, __LINE__, VoIP::sev).stream()

// The _V version is for when a variable is passed in.  It doesn't do the
// namespace concatination.
#define LOG_V(sev) \
  LOG_SEVERITY_PRECONDITION(sev) \
    VoIP::LogMessage(__FILE__, __LINE__, sev).stream()

// The _F version prefixes the message with the current function name.
#if (defined(__GNUC__) && defined(_DEBUG)) || defined(WANT_PRETTY_LOG_F)
#define LOG_F(sev) LOG(sev) << __PRETTY_FUNCTION__ << ": "
#else
#define LOG_F(sev) LOG(sev) << __FUNCTION__ << ": "
#endif

#define LOG_API0() LOG_F(LS_VERBOSE)
#define LOG_API1(v1) LOG_API0() << #v1 << "=" << v1
#define LOG_API2(v1, v2) LOG_API1(v1) \
    << ", " << #v2 << "=" << v2
#define LOG_API3(v1, v2, v3) LOG_API2(v1, v2) \
    << ", " << #v3 << "=" << v3

#define LOG_FERR0(sev, func) LOG(sev) << #func << " failed"
#define LOG_FERR1(sev, func, v1) LOG_FERR0(sev, func) \
    << ": " << #v1 << "=" << v1
#define LOG_FERR2(sev, func, v1, v2) LOG_FERR1(sev, func, v1) \
    << ", " << #v2 << "=" << v2
#define LOG_FERR3(sev, func, v1, v2, v3) LOG_FERR2(sev, func, v1, v2) \
    << ", " << #v3 << "=" << v3
#define LOG_FERR4(sev, func, v1, v2, v3, v4) LOG_FERR3(sev, func, v1, v2, v3) \
    << ", " << #v4 << "=" << v4

#endif  // LOG

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_INTERFACE_LOGGING_H_
