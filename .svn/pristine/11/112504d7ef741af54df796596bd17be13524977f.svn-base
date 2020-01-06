
// Borrowed from Chromium's src/base/basictypes.h.

#ifndef VOIP_SYSTEM_WRAPPERS_INTERFACE_COMPILE_ASSERT_H_
#define VOIP_SYSTEM_WRAPPERS_INTERFACE_COMPILE_ASSERT_H_

// The COMPILE_ASSERT macro can be used to verify that a compile time
// expression is true. For example, you could use it to verify the
// size of a static array:
//
//   COMPILE_ASSERT(ARRAYSIZE_UNSAFE(content_type_names) == CONTENT_NUM_TYPES,
//                  content_type_names_incorrect_size);
//
// or to make sure a struct is smaller than a certain size:
//
//   COMPILE_ASSERT(sizeof(foo) < 128, foo_too_large);
//
// The second argument to the macro is the name of the variable. If
// the expression is false, most compilers will issue a warning/error
// containing the name of the variable.


#if !defined(COMPILE_ASSERT)
template <bool>
struct CompileAssert {
};

#define COMPILE_ASSERT(expr, msg) \
  typedef CompileAssert<(bool(expr))> msg[bool(expr) ? 1 : -1]
#endif  // COMPILE_ASSERT


#endif  // VOIP_SYSTEM_WRAPPERS_INTERFACE_COMPILE_ASSERT_H_
