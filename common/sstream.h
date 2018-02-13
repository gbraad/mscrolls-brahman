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
#include <stdio.h>
#include <stdarg.h>
#include <functional>
#include "sbuf.h"

struct StdStreamBuf: public std::streambuf, public SBuf
{
    typedef std::function<bool(StdStreamBuf*)> Emitter;
    
    // NB: MUST set emitter before used
    
    void emitter(const Emitter& e) { _emitter = e; }

    void _emit()
    {
        (_emitter)(this);
        clear();
    }
    
protected:

    int_type overflow(int_type c)
    {
        if (c != traits_type::eof())
        {
            add((char)c);
            if (c == '\n')
                _emit();
        }
        return c; // traits_type::eof() if failed
    }

    int sync()
    {	
        if (size() > 0) _emit();
        return 0; // -1 if failed
    }

    Emitter     _emitter;
};

class StdStream: public std::ostream
{
public:

    // Constructors
    StdStream() : std::ostream(&_buf) {}
    
    operator const      char*() const { return _buf; }
    size_t              size() const { return _buf.size(); }

    int printf(const char* fmt, ...)
    {
        char buf[4096];
        
        va_list args;
        va_start(args, fmt);
        int n = vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        char* tbuf = buf;
        
        if (n >= (int)sizeof(buf))
        {
            // failed. allocate temp buffer
            tbuf = new char[n+1];
            va_start(args, fmt);
            n = vsnprintf(tbuf, sizeof(buf), fmt, args);
            va_end(args);
        }
        
        write(tbuf, n);
        if (tbuf != buf) delete [] tbuf;
        
        return n;
    }

protected:

    StdStreamBuf                                _buf;

};

class StdOStream: public StdStream
{
public:

    // Constructors
    StdOStream() 
    {
        using std::placeholders::_1;
        _buf.emitter(std::bind(&StdOStream::_emit, this, _1));
    }

protected:

    bool _emit(StdStreamBuf* buf)
    {
        // stdout emitter
        const char* p = *buf;
        while (*p) { putchar(*p); ++p; }
        return true;
    }

};

