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
 *  contact@strandgames.com
 */

#pragma once

#include <iostream>
#include <mutex>
#include "sfstream.h"

template<class T> struct LogBase
{
    static int         _logLevel;
};

template<class T> int LogBase<T>::_logLevel = 1;

inline void padspace(std::basic_ostream<char,std::char_traits<char> >& os,
                     int val)
{
    while (val)
    {
        --val;
        os << ' ';
    }
}

template<class T> struct LogFiler: public LogBase<T>
{
    static StdFStream          _stream;

#ifdef LOG_MT
    static std::mutex          _mutex;
#endif

    LogFiler()
    {
#ifndef NDEBUG
        // default we copy to console, except for release build which
        // is not attached to console. instead use "-log" option to write 
        // to file in release mode
        setAlso(&std::cout);
#endif
    }
    
    static void setAlso(std::ostream* os)
    {
        // allow console output to be changed or suppressed (if 0)
        _stream.setAlso(os);
    }

    bool openFile(const char* name)
    {
        // NB: non static method for that initialisation is performed
        // append to any existing log
        return _stream.open(name, "a");
    }
    
};

template<class T> StdFStream LogFiler<T>::_stream;

#ifdef LOG_MT
template<class T> std::mutex LogFiler<T>::_mutex;
#endif

typedef LogFiler<int> Logged;

#define LOGBOOL(_b) (_b ? "true" : "false")

#ifdef LOG_MT
#define LOG(_msg, _x) \
{                                                               \
std::lock_guard<std::mutex> lock(Logged::_mutex);               \
Logged::_stream << _msg << _x << std::endl << std::flush;       \
}

#else
#define LOG(_msg, _x) \
    Logged::_stream << _msg << _x << std::endl << std::flush
#endif // LOG_MT

#define LOGN(_n, _msg, _x) if (Logged::_logLevel >= _n) LOG(_msg, _x)
#define LOG1(_msg, _x) LOGN(1, _msg, _x)

#if defined(LOGGING)

#define LOG2(_msg, _x) LOGN(2, _msg, _x)
#define LOG3(_msg, _x) LOGN(3, _msg, _x)
#define LOG4(_msg, _x) LOGN(4, _msg, _x)
#define LOG5(_msg, _x) LOGN(5, _msg, _x)

#else

#define LOG2(_msg, _x)
#define LOG3(_msg, _x)
#define LOG4(_msg, _x)
#define LOG5(_msg, _x)

#endif

