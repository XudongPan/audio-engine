
#ifndef VOIP_TYPEDEFS_H_
#define VOIP_TYPEDEFS_H_

// Processor architecture detection.  For more info on what's defined, see:
//   http://msdn.microsoft.com/en-us/library/b0084kay.aspx
//   http://www.agner.org/optimize/calling_conventions.pdf
//   or with gcc, run: "echo | gcc -E -dM -"
#if defined(_M_X64) || defined(__x86_64__)
#define VOIP_ARCH_X86_FAMILY
#define VOIP_ARCH_X86_64
#define VOIP_ARCH_64_BITS
#define VOIP_ARCH_LITTLE_ENDIAN
#elif defined(__aarch64__)
#define VOIP_ARCH_64_BITS
#define VOIP_ARCH_LITTLE_ENDIAN
#elif defined(_M_IX86) || defined(__i386__)
#define VOIP_ARCH_X86_FAMILY
#define VOIP_ARCH_X86
#define VOIP_ARCH_32_BITS
#define VOIP_ARCH_LITTLE_ENDIAN
#elif defined(__ARMEL__)

//#define VOIP_ARCH_ARM_FAMILY
//#define VOIP_ARCH_ARMEL
#define VOIP_ARCH_32_BITS
#define VOIP_ARCH_LITTLE_ENDIAN
#elif defined(__MIPSEL__)
#define VOIP_ARCH_32_BITS
#define VOIP_ARCH_LITTLE_ENDIAN
#elif defined(__pnacl__)
#define VOIP_ARCH_32_BITS
#define VOIP_ARCH_LITTLE_ENDIAN
#else
#error Please add support for your architecture in typedefs.h
#endif

#if !(defined(VOIP_ARCH_LITTLE_ENDIAN) ^ defined(VOIP_ARCH_BIG_ENDIAN))
#error Define either VOIP_ARCH_LITTLE_ENDIAN or VOIP_ARCH_BIG_ENDIAN
#endif

#if (defined(VOIP_ARCH_X86_FAMILY) && !defined(__SSE2__)) ||  \
    (defined(VOIP_ARCH_ARM_V7) && !defined(VOIP_ARCH_ARM_NEON))
#define VOIP_CPU_DETECTION
#endif

#if !defined(_MSC_VER)
#include <stdint.h>
#else
// Define C99 equivalent types, since pre-2010 MSVC doesn't provide stdint.h.
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef __int64             int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned __int64    uint64_t;
#endif

// Borrowed from Chromium's base/compiler_specific.h.
// Annotate a virtual method indicating it must be overriding a virtual
// method in the parent class.
// Use like:
//   virtual void foo() OVERRIDE;
#if defined(_MSC_VER)
#define OVERRIDE override
#elif defined(__clang__)
// Clang defaults to C++03 and warns about using override. Squelch that.
// Intentionally no push/pop here so all users of OVERRIDE ignore the warning
// too. This is like passing -Wno-c++11-extensions, except that GCC won't die
// (because it won't see this pragma).
#pragma clang diagnostic ignored "-Wc++11-extensions"
#define OVERRIDE override
#elif defined(__GNUC__) && __cplusplus >= 201103 && \
    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100) >= 40700
// GCC 4.7 supports explicit virtual overrides when C++11 support is enabled.
#define OVERRIDE override
#else
#define OVERRIDE
#endif
#if !defined(WARN_UNUSED_RESULT)
#if defined(__GNUC__)
#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define WARN_UNUSED_RESULT
#endif
#endif  // WARN_UNUSED_RESULT

#endif  // VOIP_TYPEDEFS_H_
