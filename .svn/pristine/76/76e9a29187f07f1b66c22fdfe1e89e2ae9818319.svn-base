#ifndef VOIP_SYSTEM_WRAPPERS_SOURCE_RW_LOCK_POSIX_H_
#define VOIP_SYSTEM_WRAPPERS_SOURCE_RW_LOCK_POSIX_H_

#include "audio_engine/system_wrappers/interface/rw_lock_wrapper.h"
#include "audio_engine/include/typedefs.h"

#include <pthread.h>

namespace VoIP {

class RWLockPosix : public RWLockWrapper {
 public:
  static RWLockPosix* Create();
  virtual ~RWLockPosix();

  virtual void AcquireLockExclusive() OVERRIDE;
  virtual void ReleaseLockExclusive() OVERRIDE;

  virtual void AcquireLockShared() OVERRIDE;
  virtual void ReleaseLockShared() OVERRIDE;

 private:
  RWLockPosix();
  bool Init();

  pthread_rwlock_t lock_;
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_SOURCE_RW_LOCK_POSIX_H_
