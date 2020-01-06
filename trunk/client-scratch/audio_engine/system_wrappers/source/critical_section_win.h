
#ifndef VOIP_SYSTEM_WRAPPERS_SOURCE_CRITICAL_SECTION_WIN_H_
#define VOIP_SYSTEM_WRAPPERS_SOURCE_CRITICAL_SECTION_WIN_H_

#include <windows.h>
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {

class CriticalSectionWindows : public CriticalSectionWrapper {
 public:
  CriticalSectionWindows();

  virtual ~CriticalSectionWindows();

  virtual void Enter();
  virtual void Leave();

 private:
  CRITICAL_SECTION crit;

  friend class ConditionVariableEventWin;
  friend class ConditionVariableNativeWin;
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_SOURCE_CRITICAL_SECTION_WIN_H_
