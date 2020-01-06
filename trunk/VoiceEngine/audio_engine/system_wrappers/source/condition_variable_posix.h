#ifndef VOIP_SYSTEM_WRAPPERS_SOURCE_CONDITION_VARIABLE_POSIX_H_
#define VOIP_SYSTEM_WRAPPERS_SOURCE_CONDITION_VARIABLE_POSIX_H_

#include <pthread.h>

#include "audio_engine/system_wrappers/interface/condition_variable_wrapper.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {

class ConditionVariablePosix : public ConditionVariableWrapper {
 public:
  static ConditionVariableWrapper* Create();
  virtual ~ConditionVariablePosix();

  virtual void SleepCS(CriticalSectionWrapper& crit_sect) OVERRIDE;
  virtual bool SleepCS(CriticalSectionWrapper& crit_sect,
               unsigned long max_time_in_ms) OVERRIDE;
  virtual void Wake() OVERRIDE;
  virtual void WakeAll() OVERRIDE;

 private:
  ConditionVariablePosix();
  int Construct();

 private:
  pthread_cond_t cond_;
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_SOURCE_CONDITION_VARIABLE_POSIX_H_
