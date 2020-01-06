
#ifndef VOIP_SYSTEM_WRAPPERS_SOURCE_CRITICAL_SECTION_POSIX_H_
#define VOIP_SYSTEM_WRAPPERS_SOURCE_CRITICAL_SECTION_POSIX_H_

#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"

#include <pthread.h>

namespace VoIP {

class CriticalSectionPosix : public CriticalSectionWrapper {
 public:
  CriticalSectionPosix();

  virtual ~CriticalSectionPosix();

  virtual void Enter() OVERRIDE;
  virtual void Leave() OVERRIDE;

 private:
  pthread_mutex_t mutex_;
  friend class ConditionVariablePosix;
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_SOURCE_CRITICAL_SECTION_POSIX_H_
