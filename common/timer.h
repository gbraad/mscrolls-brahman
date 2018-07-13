/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2017.
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

#include <iostream>

#if defined(TIMERS) && (defined(_WIN32) || defined(Q_OS_LINUX))

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#include "types.h"
#include "logged.h"

struct Timer
{
  
#ifdef _WIN32
    double		_scale;
    int64		_start;
#else
    struct timespec     _start;
#endif

    Timer() { _init(); }

    void report(std::ostream& os)
    {
        uint tms = (now()*1000 + 0.5);
        os << "time " << tms << "ms\n";
    }

#ifdef _WIN32
    
    bool started() const { return _start != 0; }

    double now()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        return (li.QuadPart - _start)*_scale;
    }

    void start()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        _start = li.QuadPart;
    }

    void _init()
    {
	LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        _scale = 1.0 / double(li.QuadPart);
        _start = 0;
    }
#else
    // Linux

    bool started() const { return _start.tv_sec != 0 || _start.tv_nsec != 0; }

    double now()
    {
	struct timespec ts, last;
        clock_gettime(CLOCK_REALTIME, &last);
        ts.tv_sec = last.tv_sec - _start.tv_sec;
        ts.tv_nsec = last.tv_nsec - _start.tv_nsec;
        if (ts.tv_nsec < 0)
        {
            ts.tv_sec--;
            ts.tv_nsec += 1000000000;
        }
        return ts.tv_sec + ts.tv_nsec / 1000000000.0;
    }

    void start()
    {
	clock_gettime(CLOCK_REALTIME, &_start);
    }

    void _init()
    {
        memset(&_start, 0, sizeof _start);
    }
#endif

};

struct TimeReport
{
    Timer       _t;
    const char* _name;
    int         _level;

    TimeReport(const char* name, int level = 4) : _name(name), _level(level)
    {
        _t.start();
    }

    bool        willReport() const
    { 
        return Logged::_logLevel >= _level;
    }

    ~TimeReport()
    {
        if (willReport())
        {
            std::cerr << _name << ' ';
            _t.report(std::cerr);
        }
    }
};

#else // debug

struct Timer {};
struct TimeReport 
{
    TimeReport(const char*, int level = 2) {}
};

#endif
