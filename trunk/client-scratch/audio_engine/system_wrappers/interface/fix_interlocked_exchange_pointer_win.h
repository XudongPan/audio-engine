// Various inline functions and macros to fix compilation of 32 bit target
// on MSVC with /Wp64 flag enabled.

// The original code can be found here:
// http://src.chromium.org/svn/trunk/src/base/fix_wp64.h

#ifndef VOIP_SYSTEM_WRAPPERS_SOURCE_FIX_INTERLOCKED_EXCHANGE_POINTER_WINDOWS_H_
#define VOIP_SYSTEM_WRAPPERS_SOURCE_FIX_INTERLOCKED_EXCHANGE_POINTER_WINDOWS_H_

#include <windows.h>

// Platform SDK fixes when building with /Wp64 for a 32 bits target.
#if !defined(_WIN64) && defined(_Wp64)

#ifdef InterlockedExchangePointer
#undef InterlockedExchangePointer
// The problem is that the macro provided for InterlockedExchangePointer() is
// doing a (LONG) C-style cast that triggers invariably the warning C4312 when
// building on 32 bits.
inline void* InterlockedExchangePointer(void* volatile* target, void* value) {
  return reinterpret_cast<void*>(static_cast<LONG_PTR>(InterlockedExchange(
      reinterpret_cast<volatile LONG*>(target),
      static_cast<LONG>(reinterpret_cast<LONG_PTR>(value)))));
}
#endif  // #ifdef InterlockedExchangePointer

#endif // #if !defined(_WIN64) && defined(_Wp64)

#endif // VOIP_SYSTEM_WRAPPERS_SOURCE_FIX_INTERLOCKED_EXCHANGE_POINTER_WINDOWS_H_
