#ifndef VOIP_SYSTEM_WRAPPERS_INTERFACE_RW_LOCK_WRAPPER_H_
#define VOIP_SYSTEM_WRAPPERS_INTERFACE_RW_LOCK_WRAPPER_H_

#include "audio_engine/system_wrappers/interface/thread_annotations.h"

// Note, Windows pre-Vista version of RW locks are not supported natively. For
// these OSs regular critical sections have been used to approximate RW lock
// functionality and will therefore have worse performance.

namespace VoIP {

class LOCKABLE RWLockWrapper {
 public:
  static RWLockWrapper* CreateRWLock();
  virtual ~RWLockWrapper() {}

  virtual void AcquireLockExclusive() EXCLUSIVE_LOCK_FUNCTION() = 0;
  virtual void ReleaseLockExclusive() UNLOCK_FUNCTION() = 0;

  virtual void AcquireLockShared() SHARED_LOCK_FUNCTION() = 0;
  virtual void ReleaseLockShared() UNLOCK_FUNCTION() = 0;
};

// RAII extensions of the RW lock. Prevents Acquire/Release missmatches and
// provides more compact locking syntax.
class SCOPED_LOCKABLE ReadLockScoped {
 public:
  ReadLockScoped(RWLockWrapper& rw_lock) SHARED_LOCK_FUNCTION(rw_lock)
      : rw_lock_(rw_lock) {
    rw_lock_.AcquireLockShared();
  }

  ~ReadLockScoped() UNLOCK_FUNCTION() {
    rw_lock_.ReleaseLockShared();
  }

 private:
  RWLockWrapper& rw_lock_;
};

class SCOPED_LOCKABLE WriteLockScoped {
 public:
  WriteLockScoped(RWLockWrapper& rw_lock) EXCLUSIVE_LOCK_FUNCTION(rw_lock)
      : rw_lock_(rw_lock) {
    rw_lock_.AcquireLockExclusive();
  }

  ~WriteLockScoped() UNLOCK_FUNCTION() {
    rw_lock_.ReleaseLockExclusive();
  }

 private:
  RWLockWrapper& rw_lock_;
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_INTERFACE_RW_LOCK_WRAPPER_H_
