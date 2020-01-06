#ifndef VOIP_SYSTEM_WRAPPERS_SOURCE_TRACE_POSIX_H_
#define VOIP_SYSTEM_WRAPPERS_SOURCE_TRACE_POSIX_H_

#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/source/trace_impl.h"

namespace VoIP {

class TracePosix : public TraceImpl {
 public:
  TracePosix();
  virtual ~TracePosix();

  // This method can be called on several different threads different from
  // the creating thread.
  virtual int32_t AddTime(char* trace_message, const TraceLevel level) const
      OVERRIDE;

  virtual int32_t AddBuildInfo(char* trace_message) const OVERRIDE;
  virtual int32_t AddDateTimeInfo(char* trace_message) const OVERRIDE;

 private:
  volatile mutable uint32_t  prev_api_tick_count_;
  volatile mutable uint32_t  prev_tick_count_;

  CriticalSectionWrapper& crit_sect_;
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_SOURCE_TRACE_POSIX_H_
