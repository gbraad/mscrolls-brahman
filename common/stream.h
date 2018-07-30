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
#include "cutils.h"
#include "types.h"
#include "sbuf.h"
#include "fdbuf.h"

struct Stream
{
    // abstract stream

    // have a very small hold buffer for unget
    static const int maxhold = 16; // power of 2
    
    int         _hold[maxhold];
    int         _holdpos;
    int         _backcount;
    bool        _inLineComment;
    bool        _inBlockComment;
    bool        _inQuote;
    bool        _esc;

    int         _at;
    int         _line;

    // allow # at the start of a line to comment out the line
    bool        _hashLineComments = true;

    // ignore remainder of line following a semi-colon ";"
    bool        _semiColonComments = false;

    // c-style comments
    bool        _cComments = true;


    Stream() { _init(); }
    
    virtual ~Stream() {}

    int operator*() const { return _at; }
    void operator++() { _bump(); }
    void operator--() { _unget(); }
    int  line() const { return _line; }

    bool getline(SBuf& sb)
    {
        // return false if no data and EOF

        sb.clear();
        
        for (;;)
        {
            _bump();
            if (!_at || _at == '\n') break;
            sb.add((char)_at);
        }
        
        return _at || sb.size();
    }

    virtual const char* name() const = 0;

protected:

    virtual int __get() = 0;

    bool inComment() const { return _inLineComment || _inBlockComment; }

    void _unget()
    {
        ++_backcount;

        _holdpos = (_holdpos - 1) & (maxhold - 1);

        // restore last char from hold
        _at = _hold[(_holdpos - 1) & (maxhold - 1)];
    }

    int _get()
    {
        int c;
        if (_backcount)
        {
            --_backcount;
            c = _hold[_holdpos];
        }
        else
        {
            c = __get();
            if (c == '\r') c = __get(); // ignore DOS newlines (if any)
            _hold[_holdpos] = c;
        }

        _holdpos = (_holdpos + 1) & (maxhold-1);
        return c;
    }

    void _bump()
    {
        // set `at` to the next character. 0 at EOF
        
        int c;

        for (;;)
        {
            c = _get();
            if (c == EOF) c = 0;
            if (!c) break;

            if (!inComment())
            {
                if (c == '\\' && _inQuote)
                {
                    // escape in quotes
                    _esc = true;
                }
                else
                {
                    if (c == '"' && !_esc)
                        _inQuote = !_inQuote;
                    
                    _esc = false;
                }
            }

            if (u_isnl(c))
            {
                // count lines (comments,quotes or not)
                ++_line;

                if (_inBlockComment) continue;

                if (!_inQuote && _inLineComment)
                {
                    _inLineComment = false;

                    // ignore this newline when it terminates a comment
                    continue;
                }
            }
            else if (!_inQuote)
            {
                if (!inComment())
                {
                    if (c == '#' && _hashLineComments)
                        _inLineComment = true;
                    else if (c == ';' && _semiColonComments)
                        _inLineComment = true;
                    else if (c == '/' && _cComments)
                    {
                        c = _get();
                        if (c == '*')
                        {
                            _inBlockComment = true;
                        }
                        else if (c == '/')
                        {
                            // c++ // comment
                            _inLineComment = true;
                        }
                        else
                        {
                            // not a c-comment
                            _unget();
                            c = '/';
                        }
                    }
                }
                else if (_inBlockComment && c == '*' && _cComments)
                {
                    c = _get();
                    if (c == '/')
                        _inBlockComment = false;
                    continue;
                }
            }

            if (!inComment()) break;
        }

        _at = c;
    }

    void _init()
    {
        _at = 0;
        _line = 1;
        _holdpos = 0;
        _backcount = 0;
        _inLineComment = false;
        _inBlockComment = false;
        _inQuote = false;
        _esc = false;
    }

};

struct StreamString: public Stream
{
    // make a stream from a string
    
    void open(const char* s)
    {
        _s = (const uchar*)s;
        
        // load first char
        _bump();
    }

    // Compliance
    int __get() override
    {
        return *_s++;
    }

    const char* name() const override { return "string"; }

private:

    const uchar*         _s;
};

struct StreamFile: public Stream
{
    // read a file as a stream

    FD              _fd;
    FDBuf           _fb;
    std::string     _name;    

    bool open(const char* name)
    {
        _fb.close();
        _name = name;

        bool r = _fd.open(name);

        if (r)
        {
            _fb.fd(_fd);
            _bump(); // load first char
        }
        return r;
    }

    bool open(const std::string& name) { return open(name.c_str()); }

    const char* name() const override { return _name.c_str(); }

protected:

    // Compliance
    int __get() override
    {
        return _fb.getc();
    }

};

