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

#include <string.h>

struct JTrans
{
    char*       _buf;
    size_t      _sz;
    size_t      _sp;
    size_t      _r;

    static const int chunk = 1024;

    JTrans()
    {
        _buf = 0;
        _sz = 0;
        _sp = 0;
    }

    ~JTrans() { _purge(); }

    void allocate(size_t sz)
    {
        _sz = sz;
        _sp = _sz + 1;
        _buf = new char[_sp];
    }

    void fixJson()
    {
        // repair some common JSON problems
        char* p = _buf;
        bool inquotes = false;
        bool esc = false;
        char* q = p;
        _r = _sz;
        for (;;)
        {
            char c = *p++;
            if (!c) break;
            --_r;
            if (esc)
                esc = false;
            else
            {
                if (c == '\\') esc = true;
                else if (c == '"')
                    inquotes = !inquotes;
                else if (c == '\r')
                {
                    // remove all DOS newlines
                    continue;
                }
                else if (inquotes)
                {
                    if (c == '\n')
                    {
                        // newline -> "\n"
                        q = _makeSpace(p, q);
                        *q++ = '\\';
                        c = 'n';
                    }
                }
            }
            *q++ = c;
        }
        
        _sz = q - _buf;
    }

private:

    void _purge()
    {
        delete _buf;
        _buf = 0;
    }
    
    char* _makeSpace(char*& p, char* q)
    {
        if (p - q <= 1)
        {
            size_t a = q - _buf;
            size_t t = a + _r + 2;
            if (t > _sp)
            {
                _sp = t + chunk;
                char* nbuf = new char[_sp];
                
                memcpy(nbuf, _buf, a);

                q = nbuf + a;
                memcpy(q + 2, p, _r);

                _purge();
                _buf = nbuf;

                p = q + 1;
            }
            else
                memmove(p + 1, p, _r);
            ++p;
        }
        return q;
    }
    
};


