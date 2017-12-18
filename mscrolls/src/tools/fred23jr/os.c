/**
 *
 * Copyright (c) 2017 Voidware Ltd.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code as
 * defined in and that are subject to the Voidware Public Source Licence version
 * 1.0 (the 'Licence'). You may not use this file except in compliance with the
 * Licence or with expressly written permission from Voidware.  Please obtain a
 * copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
 * it before using this file.
 *
 * THIS IS NOT FREE SOFTWARE.
 * 
 * The Original Code and all software distributed under the Licence are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Please see the Licence for the specific language governing rights and 
 * limitations under the Licence.
 *
 * contact@strandgames.com
 *
 */

#include "os.h"

#ifdef _MSC_VER

#include <stdio.h>
#include <io.h>
#include <time.h>

#define unlink  _unlink

#else
#include <unistd.h>
#include <sys/time.h>
#endif


int deleteFile(const char* path)
{
    return unlink(path);
}

char* timenow_string()
{
    char* s;
    
#ifdef _MSC_VER
    time_t clock;
    time(&clock);
    s = ctime(&clock);
#else
    struct timeval clock;
    struct timezone zone;

    gettimeofday(&clock, &zone);
    s = ctime(&clock.tv_sec);
#endif

    return s;
}

