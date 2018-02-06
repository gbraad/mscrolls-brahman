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

#include <string>
#include <stdarg.h>

class GrowBufBase
{
public:
    
    GrowBufBase() { _init(); }

    virtual ~GrowBufBase() {}
    
    bool        isEmpty() const { return _size == 0; }
    size_t      size() const { return _size; }

protected:

    void _init()
    {
        _size = 0;
        _space = 0;
    }

    size_t      _size;
    size_t      _space;
};

template<typename T, int Chunk = 512> class GrowBuf: public GrowBufBase
{
    // a growable contiguous array of T

public:

    typedef GrowBufBase         parentT;

    GrowBuf() { _init(); }
    GrowBuf(const GrowBuf& b) { _init(); *this = b; }

    ~GrowBuf() { purge(); }

    void operator=(const GrowBuf& b)
    {
        // donate semantics
        if (this != &b)
        {
            purge();
            
            _size = b._size;
            _space = b._space;
            _v = b._v;

            // reinitialise the doner
            GrowBuf& ob = const_cast<GrowBuf&>(b);
            ob._init();
            ob.parentT::_init();
        }
    }

    void operator=(const char* s)
    {
        purge();
        append(s);
    }
    
    T* add(const T& v)
    {
        grow(1);
        _v[_size] = v;
        return _v + _size++;
    }

    T last() const
    {
        return _size ? _v[_size-1] : T();
    }

    void unadd()
    {
        // back up one.
        if (_size) --_size;
    }

    size_t find(const T& v) const
    {
        for (size_t i = 0; i < _size; ++i)
            if (_v[i] == v) return i;

        return (size_t)-1;
    }

    void append(const T* va, size_t n)
    {
        grow(n);
        memcpy(end(), va, n * sizeof(T));
        _size += n;
    }

    void append(const char* s)
    {
        // for use when T is char
        if (s) append(s, strlen(s));
    }

    void appendf(const char* m, ...)
    {
        // for use when T is char
        va_list args;
        va_start(args, m);
        int n = vsnprintf(0, 0, m, args);
        va_end(args);
        
        char* buf = new char[n + 1];

        va_start(args, m);
        vsprintf(buf, m, args);
        va_end(args);

        append(buf);
        delete [] buf;
    }

    void append(const std::string& s)
    {
        // for use when T is char
        append(s.c_str());
    }

    void prepend(const char* s, size_t n)
    {
        grow(n);
        memmove(start() + n, start(), _size);
        memcpy(start(), s, n);
        _size += n;
    }

    void prepend(const char* s)
    {
        // for use when T is char
        if (s) prepend(s, strlen(s));
    }

    T*        start() const { return _v; }
    T*        end() const { return _v + _size; }

    // act like an ordinary array
    operator T*() { return _v; }
    operator const T*() const { return _v; }

    void purge()
    {
        delete [] _v;
        parentT::_init();
        _init();
    }

    void clear() { purge(); } // convenience

    void resize(size_t s)
    {
        if (s > _space) grow(s - _size);
        _size = s;
    }

    T* donate() 
    {
        if (!_size) return 0;
        T* p = _v; 
        _init();
        parentT::_init();
        return p;
    }

    void consume(T* va, size_t n)
    {
        purge();
        _v = va;
        _size = n;
        _space = n;
    }

    void grow(size_t n)
    {
        // grow to `size' + n
        size_t sz = _size + n;
        if (sz > _space)
        {
            _space += Chunk;
            if (sz > _space)
            {
                sz = (sz + Chunk - 1)/Chunk;
                _space = sz * Chunk;
            }

            T* v2 = new T[_space];
            memcpy(v2, _v, _size * sizeof(T));

            delete [] _v;

            _v = v2;
        }
    }

private:

    void _init()
    {
        _v = 0;
    }

    T*          _v;
};

// convenience 
typedef GrowBuf<char> GrowString;
