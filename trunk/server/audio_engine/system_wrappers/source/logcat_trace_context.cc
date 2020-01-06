/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "audio_engine/system_wrappers/interface/logcat_trace_context.h"

#include <android/log.h>
#include <assert.h>

#include "audio_engine/system_wrappers/interface/logging.h"

namespace VoIP {

static android_LogPriority AndroidLogPriorityFromWebRtcLogLevel(
    TraceLevel webrtc_level) {
  // NOTE: this mapping is somewhat arbitrary.  StateInfo and Info are mapped
  // to DEBUG because they are highly verbose in VoIP code (which is
  // unfortunate).
  switch (webrtc_level) {
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
      LOG(LS_ERROR) << "Unexpected log level" << webrtc_level;
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
  __android_log_print(AndroidLogPriorityFromWebRtcLogLevel(level),
                      "WEBRTC", "%.*s", length, message);
}

}  // namespace VoIP
