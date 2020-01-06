
#ifndef VOIP_SYSTEM_WRAPPERS_SOURCE_EVENT_WIN_H_
#define VOIP_SYSTEM_WRAPPERS_SOURCE_EVENT_WIN_H_

#include <windows.h>

#include "audio_engine/system_wrappers/interface/event_wrapper.h"

#include "audio_engine/include/typedefs.h"

namespace VoIP {

class EventWindows : public EventWrapper {
 public:
  EventWindows();
  virtual ~EventWindows();

  virtual EventTypeWrapper Wait(unsigned long max_time);
  virtual bool Set();
  virtual bool Reset();

  virtual bool StartTimer(bool periodic, unsigned long time);
  virtual bool StopTimer();

 private:
  HANDLE  event_;
  uint32_t timerID_;
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_SOURCE_EVENT_WIN_H_
