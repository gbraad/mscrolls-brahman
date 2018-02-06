/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2016-2017.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@voidware.com
 */

#pragma once

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char uchar;

#include <stdint.h>

#if defined(_WIN32)

#ifdef __MINGW32__

// these are missing from mingw64 (currently)
# if __WORDSIZE == 64
#define _PRId64 "ld"
#define _PRIX64 "lX"
#define _PRIx64 "lx"
#else
#define _PRId64 "lld"
#define _PRIX64 "llX"
#define _PRIx64 "llx"
#endif

#elif defined(_MSC_VER)

// this in base member initialisation
#pragma warning(disable: 4355)

// Have our own versions of `PRId64`.
// these are meant to be in inttypes.h, but they're not always (eg mingw)
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

typedef uint64_t uint64;
typedef int64_t int64;


 
