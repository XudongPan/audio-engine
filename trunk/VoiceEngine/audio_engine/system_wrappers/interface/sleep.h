// An OS-independent sleep function.

#ifndef VOIP_SYSTEM_WRAPPERS_INTERFACE_SLEEP_H_
#define VOIP_SYSTEM_WRAPPERS_INTERFACE_SLEEP_H_

namespace VoIP {

// This function sleeps for the specified number of milliseconds.
// It may return early if the thread is woken by some other event,
// such as the delivery of a signal on Unix.
void SleepMs(int msecs);

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_INTERFACE_SLEEP_H_
