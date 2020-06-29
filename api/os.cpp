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


/* OS Specific functions */

#ifndef _MSC_VER
#include <sys/time.h>   // gettimeofday
#else
#define _USE_32BIT_TIME_T
#include "tid.h"
#endif

#include "os.h"
#include "defs.h"
#include "strutils.h"
#include "logged.h"

#ifdef _WIN32
#include <Shlobj.h> // known folders
#include "windpi.h"
#endif

typedef std::string string;

#ifdef _WIN32
static HANDLE uniqueInstanceHandle;
#endif

bool createUniqueInstance(const char* name)
{
    bool res = true;
    
#ifdef _WIN32

    const char* mutexPrefix = "com_voidware_brahman";

    char mutexName[256];
    strcpy(mutexName, mutexPrefix);

    if (name)
    {
        // XX assume not too long
        strcat(strcat(mutexName, "_"), name);
    }
    
    uniqueInstanceHandle = CreateMutex( NULL, TRUE, mutexName );

    int e = GetLastError();
    if (e == ERROR_ALREADY_EXISTS)
    {
        // found existing handle, but we don't have ownership
        CloseHandle(uniqueInstanceHandle);
        uniqueInstanceHandle = 0; // no need to release
        LOG3("createUniqueInstance, exists ", mutexName);
        res = false;
    }
    else if (!uniqueInstanceHandle)
    {
        LOG("createUniqueInstance, failed with ", e);
        // NOTE: return true to allow process to run.
    }
    else 
    {
        LOG3("createUniqueInstance, created ", mutexName);
    }
#endif
    
    return res;
}

void destroyUniqueInstance()
{
#ifdef _WIN32
    if (uniqueInstanceHandle)
    {
        LOG3("destroyUniqueInstance, ", "ok");
        ReleaseMutex(uniqueInstanceHandle);
        CloseHandle(uniqueInstanceHandle);
        uniqueInstanceHandle = 0;
    }
#endif
}

#ifdef _WIN32
static int getWinLogicalDPI()
{
    // old way, possibly the same as Qt
    SetProcessDPIAware(); //true
    HDC screen = GetDC(NULL);
    double hPixelsPerInch = GetDeviceCaps(screen,LOGPIXELSX);
    double vPixelsPerInch = GetDeviceCaps(screen,LOGPIXELSY);
    ReleaseDC(NULL, screen);
    return (int)((hPixelsPerInch + vPixelsPerInch) * 0.5);
}
#endif // _WIN32

int getScreenDPI()
{
    // return 0 if not known
    int dpi = 0;
    
#ifdef _WIN32
    float dpif;
    if (getMonitorDPI(0, dpif))
    {
        dpi = (int)(dpif + 0.5f);
    }
    else
    {
        // use old way
        dpi = getWinLogicalDPI();
    }
#endif

    return dpi;
}

bool getlocaltime(struct tm* t)
{
    struct timeval tv;
    bool res = false;

#ifndef _MSC_VER
    res = !gettimeofday(&tv, 0);
#else
    // XX otherwise hijack the clock from the TID
#define MSEC_TO_MJD 946684800000LL
    uint64 tms = TID().create().toMJDMilliseconds();
    tv.tv_sec = (long)((tms + MSEC_TO_MJD)/1000);
    res = true;
#endif

    if (res)
    {
        time_t t2 = tv.tv_sec;
        struct tm* t1 = localtime(&t2);
        if (t1) *t = *t1;
        else res = false;
    }
    return res;
}

