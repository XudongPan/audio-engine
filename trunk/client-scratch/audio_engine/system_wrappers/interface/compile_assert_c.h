#ifndef VOIP_SYSTEM_WRAPPERS_INTERFACE_COMPILE_ASSERT_H_
#define VOIP_SYSTEM_WRAPPERS_INTERFACE_COMPILE_ASSERT_H_

// Only use this for C files. For C++, use compile_assert.h.
//
// Use this macro to verify at compile time that certain restrictions are met.
// The argument is the boolean expression to evaluate.
// Example:
//   COMPILE_ASSERT(sizeof(foo) < 128);
#define COMPILE_ASSERT(expression) switch (0) {case 0: case expression:;}

#endif  // VOIP_SYSTEM_WRAPPERS_INTERFACE_COMPILE_ASSERT_H_
