/*
 *
 *
 *   ______              _
 *   | ___ \            | |
 *   | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
 *   | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
 *   | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
 *   \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
 *
 *
 *  "The creative principle which lies realized in the whole world"
 *
 *  Copyright (c) Strand Games 2018.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@strandgames.com
 *
 */

#pragma once

#ifndef _MSC_VER
#include <sys/time.h>   // gettimeofday
#else

#define _CRT_RAND_S
#include <stdlib.h>

//#define _USE_32BIT_TIME_T
//#include "tid.h"
#endif

#include "types.h"
#include "logged.h"

#ifdef _WIN32
#include <Shlobj.h> // known folders
#endif

#if defined(_WIN32) && !defined(_MSC_VER)
# define RtlGenRandom SystemFunction036
extern "C"
BOOLEAN NTAPI RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);
#endif

static inline uint64 makeRandomSeed()
{
    uint64 s = 0U;
    
#ifdef _WIN32
#ifdef _MSC_VER

    /* XXXX
    unsigned int r1,r2;
    if (rand_s(&r1) || rand_s(&r2))
    {
        LOG1("WARNING: windows rand_s failed", 0);        
    }
    else
    {
        s = r1;
        s <<= 32;
        s |= r2;
    }
    */
    
#else // !MSC_VER    
    if (!RtlGenRandom((PVOID) &s, (ULONG) sizeof(s)))
    {
        LOG1("WARNING: windows RtlGenRandom failed", 0);
        s = 0U;
    }
#endif // MSC_VER    
#else

    // XX use the time. 
    struct timeval tv;
    gettimeofday(&tv, 0);
    s = tv.tv_usec;
    s <<= 32;
    s |= tv.tv_sec;
    
#endif
    
    return s;
}
