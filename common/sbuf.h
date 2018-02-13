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

#include <stdio.h>
#include <string.h>

/// very simple string buffer
struct SBuf 
{
    // most are small, but use bigger growsize when not enough
    static const int initSize = 256;
    static const int growSize = 8192;  // must be power of 2

    enum Mode
    {
        mode_normal = 0,
        mode_noquotes = 1,
    };

    // Constructors
    SBuf()
    {
        _init();
        _grow(initSize); 
        _buf[_pos] = 0;
    }

    // Destructor
    ~SBuf() { _purge(); }

    operator const      char*() const { return _buf; }
    size_t              size() const { return _pos; }
    char                last() const { return _buf[_pos-1]; }
    bool                truncated() const { return _truncated; }

    bool        modeNoQuotes() const { return (_mode & mode_noquotes) != 0; }

    void reset() { _pos = 0; }
    void setMaxSize(unsigned int n) { _maxSpace = n; }

    void add(char c)
    {
        if (_spaceFor(2)) // space for terminator
        {
            _buf[_pos++] = c;
            _buf[_pos] = 0;
        }
    }

    void add(const char* s)
    {
        // will terminate
        write(s, strlen(s));
    }

    void addLongString(long v)
    {
        char buf[64];
        sprintf(buf, "%ld", v);
        add(buf);
    }

    void addFloatString(float f)
    {
        char buf[32];
        sprintf(buf, "%f", f);
        add(buf);
    }

    unsigned char readByte()
    {
        unsigned char c = 0;
        if (_pos < _space)
            c = (unsigned char)_buf[_pos++];
        return c;
    }

    unsigned int readShort()
    {
        // write LSB, MSB

        unsigned short v;
        read(&v, 2); // XXX ENDIAN!
        return v;
    }

    unsigned int readInt32()
    {
        unsigned int v;
        read(&v, 4); // XXX ENDIAN!
        return v;
    }

    void writeShort(unsigned short v)
    {
        // write LSB, MSB
        
        // XXX ENDIAN!
        write(&v, 2);
    }

    void writeInt32(unsigned int v)
    {
        // XXX ENDIAN!
        write(&v, 4);
    }

    void write(const void* p, size_t n)
    {
        if (!_spaceFor(n+1))
        {
            // copy what we can
            n = _space - _pos - 1;
        }

        memcpy(_buf + _pos, p, n);
        _pos += n;

        // maintain terminator
        _buf[_pos] = 0;
    }

    void read(void* p, size_t n)
    {
        size_t r = _space - _pos;
        if (n <= r)
        {
            memcpy(p, _buf + _pos, n);
            _pos += n;
        }
        else memset(p, 0, n);
    }

    void clear()
    {
        _purge();
        _grow(initSize);
        _buf[_pos] = 0;
    }

    char* donate()
    {
        char* p = _buf;
        _init();
        return p;
    }

private:

    void _purge()
    {
        delete _buf;
        _init();
    }

    void _init()
    {
        _buf = 0;
        _space = 0;
        _pos = 0;
        _maxSpace = 0;  // unlimited
        _truncated = false;
        _mode = 0;
    }

    bool _spaceFor(size_t n)
    {
        size_t r = _space - _pos;
        bool ok = r >= n;
        if (!ok)
        {
            size_t g = n - r;

            // round up to a whole number of growsizes
            g = (g + (growSize - 1)) & ~(growSize - 1);
            
            _grow(g);

            ok = _space - _pos >= n;
            if (!ok) _truncated = true;
        }
        return ok;
    }

    void _grow(size_t n)
    {
        // grow by `n'
        char* p;
        size_t sz;

        sz = _space + n;

        // clamp to max, if defined
        if (_maxSpace && sz > _maxSpace)
            sz = _maxSpace;

        if (sz > _space)
        {
            p = new char[sz];

            if (_buf)
            {
                memcpy(p, _buf, _pos + 1);
                delete [] _buf;
            }
        
            _buf = p;
            _space = sz;
        }
    }

    char*               _buf;
    size_t              _space;
    size_t              _pos;

    // optional limit on size. 0 => unlimited
    size_t              _maxSpace;

    // if max space was hit and content truncated.
    bool                _truncated;

public:
    
    bool                _mode;
};

