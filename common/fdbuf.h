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

/* buffered portable file descriptor */

#pragma once

#include <assert.h>
#include <string.h>     // memcpy
#include <stdarg.h>
#include "fd.h"
#include "logged.h"

template<int SZ> struct FDBufN: public FDBase
{
    Pos                 _pos;
    FD*                 _fd;

    uchar*              _buf;
    size_t              _r;
    size_t              _bp;
    bool                _modified;  // outstanding writes
    bool                _eof;
    int                 _line;

    struct Elt
    {
        uint            _id;
        uint            _vmajor;
        uint            _vminor;
        Pos             _sizePos;
        uint            _dsize = (uint)-1;

        std::string     _name;
        bool            _failOnId = true;
        bool            _failOnNewerMajor = true;

        // othwerwise allow minor to be newer, but generate warning
        bool            _failOnNewerMinor = false;

        Elt() {}
        Elt(int id, int vmj, int vmi, const std::string& name)
            : _id(id), _vmajor(vmj), _vminor(vmi), _name(name) {}

        bool valid() const { return _dsize != (uint)-1; }
    };

    FDBufN()
    {
        _init();
    }

    explicit FDBufN(FD& f)
    {
        _init();
        fd(f);
    }

    ~FDBufN() { _purge(); } 

    Pos         pos() const { return _pos; }
    int         line() const { return _line; }
    bool        isOpen() const { return _fd && _fd->isOpen(); }

    void        fd(FD& fd) 
    {
        _purge();
        _fd = &fd;
        _buf = new uchar[SZ];
    }

    bool flush()
    {
        return !_modified || _write();
    }

    // compliance
    void close() override
    {
        if (isOpen())
        {
            flush();
            _fd->close();
        }
    }

    // compliance
    bool read(uchar* buf, size_t amt, size_t& nread) override
    {
        nread = 0;
        bool res = true;
        
        while (amt)
        {
            size_t a = amt;
            if (_r < a) a = _r;

            if (!a)
            {
                res = _fill();
                if (!res) break;
                if (!_r)
                {
                    // no more data
                    _eof = true;
                    break;
                }
            }
            else
            {
                memcpy(buf + nread, _buf + _bp, a);
                _bp += a;
                _r -= a;
                amt -= a;
                nread += a;
            }
        }

        _pos += nread;
        return res;
    }

    int getc()
    {
        int c = -1; // EOF
        if (!_eof)
        {
            if (!_r) _fill();
            if (!_r) 
                _eof = true;
            else
            {
                --_r;
                ++_pos;
                c = _buf[_bp++];
                if (c == '\n') ++_line;
            }
        }
        return c;
    }

    int getshort()
    {
        return getc() | getc() << 8;
    }

    int getint()
    {
        return getshort() | getshort() << 16;
    }

    std::string getstring()
    {
        std::string s;
        int c;
        while ((c = getc()) > 0) s += (char)c;
        return s;
    }

    void ungetc()
    {
        // only guaranteed to work once, after a getc
        if (!_eof)
        {
            --_pos;
            ++_r;
            --_bp;
            if (_buf[_bp] == '\n') --_line;
        }
    }

    int peekc()
    {
        int c = -1; // EOF
        if (!_eof)
        {
            if (!_r) _fill();
            if (!_r)
                _eof = true;
            else
                c = _buf[_bp];
        }
        return c;
    }

    bool putc(char c)
    {
        return isOpen() && _putc(c);
    }

    bool putshort(int v)
    {
        // put 16 bit int
        return isOpen() && _putshort(v);
    }
    
    bool putint(int v)
    {
        // put 32 bit int
        return isOpen() && _putshort(v) && _putshort(v >> 16);
    }

    bool putstring(const char* s)
    {
        bool r = isOpen();
        while (r)
        {
            // include zero terminator
            r = _putc(*s);
            if (!*s) break;
            ++s;
        }
        return r;
    }

    bool write(const unsigned char* buf, size_t amt, size_t& nwrote)  override
    {
        bool res = isOpen();
        nwrote = 0;
        
        if (res) while (amt)
        {
            res = _bp < SZ || _write();
            if (!res) break;

            uint a = SZ - _bp;
            if (a > amt) a = amt;
            assert(a > 0);

            _modified = true;
            memcpy(_buf + _bp, buf + nwrote, a);

            amt -= a;
            _bp += a;
            nwrote += a;
        }
        _pos += nwrote;
        return res;
    }

    bool write(const unsigned char* buf, size_t amt)
    {
        size_t nwrote;
        return write(buf, amt, nwrote) && nwrote == amt;
    }

    int lastChar(unsigned int n = 1) const
    {
        return _bp >= n ? _buf[_bp - n] : 0;
    }

    bool seek(Pos pos)
    {
        if (pos == _pos) return true;

        //LOG3("seek to ", pos << " from " << _pos);
        
        bool res = true;
        Pos p1 = _pos - _bp; // logical pos of buffer start
        if (pos >= p1 && pos <= _pos)
        {
            // within current buffer
            _pos = pos;
            
            uint d = _pos - pos;
            _r += d;
            _bp -= d;
        }
        else
        {
            // flush any unsaved data and move to new logical write position
            res = flush() && _fd->seek(pos);
            if (res) _pos = pos;
            _r = 0;
            _bp = 0;
        }
        if (res) _eof = false;
        return res;
    }

    bool writeEltBegin(Elt& e)
    {
        putint(e._id);
        putc(e._vmajor);
        putc(e._vminor);

        e._sizePos = _pos;
        bool r = putint(0); // dummy size

        if (!r)
        {
            LOG1("cannot write element begin ",
                 std::hex << e._id << std::dec << " in " << e._name);
        }
        return r;
    }

    bool writeEltEnd(Elt& e)
    {
        Pos endp = _pos;

        // otherwise when we seek back we lose unwritten data
        flush();
        
        bool res = seek(e._sizePos);
        if (res)
        {
            // calculate data size
            // NOTE: ints are written as 4 bytes whatever
            e._dsize = endp - e._sizePos - 4;

            // overwrite original zero with actual data size
            res = putint(e._dsize);

            // seek back to end
            res = res && seek(endp);

            if (!res)
            {
                LOG1("cannot write element end ",
                     std::hex << e._id << std::dec << " in " << e._name);
            }
            else
            {
                LOG4("write elt ",
                     std::hex << e._id << std::dec << " " << e._dsize << " bytes");
            }
        }

        return res;
    }

    bool readEltBegin(Elt& e)
    {
        e._id = getint();
        e._vmajor = getc();
        e._vminor = getc();
        e._sizePos = _pos;
        e._dsize = getint();
        return e.valid();
    }

    static bool readEltCheck(const Elt& e1, const Elt& e2)
    {
        // check that e1 is of type e2 and handle errors

        bool res = e1.valid();

        if (!res)
        {
            LOG1("cannot read element ",
                 std::hex << e2._id << std::dec << " in " << e2._name);
            return false;
        }
        
        if (e2._failOnId && e1._id != e2._id)
        {
            LOG1("reading element, expected ID ",
                 std::hex << e1._id
                 << " instead of " << e2._id << std::dec << " in " << e2._name);
                res = false;
        }

        bool newer =  e1._vmajor > e2._vmajor ||
            (e1._vmajor == e2._vmajor && e1._vminor > e2._vminor);
        
        if ((e2._failOnNewerMajor && e1._vmajor > e2._vmajor) ||
            (e2._failOnNewerMinor && newer))
        {
            LOG1("element has version ", e1._vmajor << "." << e1._vminor
                 << " newer than "
                 << e2._vmajor << "." << e2._vminor << " in " << e2._name);
            res = false;
        }
        else if (newer)
        {
            LOG3("Warning, element has version ", e1._vmajor << "." << e1._vminor << " newer than " << e2._vmajor << "." << e2._vminor << " in " << e2._name);
        }
        return res;
    }

    bool readEltEnd(Elt& e)
    {
        // seek to end of element
        Pos pos = e._sizePos + e._dsize + 4; // add back size of size
        bool r = seek(pos);
        if (!r)
        {
            LOG1("cannot read element end ",
                 std::hex << e._id << std::dec << " in " << e._name);
        }
        return r;
    }

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
        
        if (!write((const unsigned char*)tbuf, n)) n = 0;
        if (tbuf != buf) delete [] tbuf;
        
        return n;
    }

private:

    void _init()
    {
        _pos = 0;
        _fd = 0;
        _buf = 0;
        _r = 0;
        _bp = 0;
        _modified = false;
        _eof = false;
        _line = 1;
    }

    void _purge()
    {
        flush();
        delete _buf;
        _init();
    }

    bool _fill()
    {
        // ASSUME consecutive filling (ie no seek needed)
        bool res = isOpen();
        if (res)
        {
            _bp = 0;
            res = _fd->read(_buf, SZ, _r);
            if (res && _r) _eof = false;
        }
        return res;
    }

    bool _write()
    {
        // ASSUME open
        assert(_fd);
        bool res = _fd->write(_buf, _bp);
        _bp = 0;
        _modified = false;
        return res;
    }

    bool _putc(char c)
    {
        // ASSUME open
        bool res = _bp < SZ || _write();
        if (res)
        {
            _buf[_bp++] = c;
            _modified = true;
            ++_pos;
        }
        return res;
    }

    bool _putshort(int v)
    {
        // ASSUME OPEn
        return _putc(v) && _putc(v >> 8);
    }
};

// buffer with default size
typedef FDBufN<4096> FDBuf;
