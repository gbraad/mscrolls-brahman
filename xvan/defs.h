
#pragma once

/* try to make a define corresponding to max path length */

#ifdef _WIN32
#define MAXIMUM_PATH _MAX_PATH+1

#else
#include <limits.h>
#define MAXIMUM_PATH PATH_MAX
#define _alloca(a) alloca(a)
#endif


//////////////////////

#ifdef _WIN32

#ifdef __MINGW32__

#include <inttypes.h>

#define _PRId64 PRId64
#define _PRIX64 PRIX64
#define _PRIx64 PRIx64

#elif defined(_MSC_VER)

#define _PRId64 "I64d"
#define _PRIX64 "I64X"
#define _PRIx64 "I64x"

#else
#error unknown WIN32 compiler
#endif

#elif defined(__GNUC__)

#include <inttypes.h>

#ifdef PRIX64

#define _PRId64 PRId64
#define _PRIX64 PRIX64
#define _PRIx64 PRIx64

#else

// missing (eg android build), define our own
# if __WORDSIZE == 64
#define _PRId64 "ld"
#define _PRIX64 "lX"
#define _PRIx64 "lx"
#else
#define _PRId64 "lld"
#define _PRIX64 "llX"
#define _PRIx64 "llx"
#endif

#endif

#else
#error compiler unknown
#endif
