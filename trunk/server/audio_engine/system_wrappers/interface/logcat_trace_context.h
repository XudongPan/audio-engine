/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_SYSTEM_WRAPPERS_INTERFACE_LOGCAT_TRACE_CONTEXT_H_
#define WEBRTC_SYSTEM_WRAPPERS_INTERFACE_LOGCAT_TRACE_CONTEXT_H_

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

#endif  // WEBRTC_SYSTEM_WRAPPERS_INTERFACE_LOGCAT_TRACE_CONTEXT_H_
