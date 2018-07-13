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

#include <stdio.h>
#include <string>
#include "cutils.h"
#include "types.h"
#include "sbuf.h"

struct Stream
{
    // abstract stream
    
    virtual ~Stream() {}
    virtual int operator*() const = 0;
    virtual void operator++() = 0;
    virtual void operator--() = 0;
    virtual int line() const { return 1; }
    virtual bool getline(SBuf& sb) = 0;
};

struct StreamString: public Stream
{
    // make a stream from a string
    StreamString(const char* s) : _s(s)  {}
    
    // compliance
    int operator*() const override { return *_s; }
    void operator++() override { ++_s; }
    void operator--() override { --_s; }

    bool getline(SBuf& sb) override
    {
        int c;
        for (;;++_s)
        {
            c = *_s;
            if (!c) break;
            if (c == '\r') continue;
            if (c == '\n') break;
            sb.add(c);
        }
        return c || sb.size();
    }

private:

    const char*         _s;
};

struct StreamFile: public Stream
{
    // make a stream from a file
    
    FILE*       _fp;
    int         _at;
    int         _line;

    // have a very small hold buffer for unget
    static const int maxhold = 16; // power of 2
    int         _hold[maxhold];
    int         _holdpos;
    int         _backcount;
    bool        _inLineComment;
    bool        _inBlockComment;
    bool        _inQuote;
    bool        _esc;

    // allow # at the start of a line to comment out the line
    bool        _hashLineComments = true;

    // ignore remainder of line following a semi-colon ";"
    bool        _semiColonComments = false;

    // c-style comments
    bool        _cComments = true;

    StreamFile(FILE* fp = 0) : _fp(fp) 
    {
        _init();
    }
    
    bool ok() const { return _fp != 0; }    
    bool inComment() const { return _inLineComment || _inBlockComment; }

    // Compliance
    int operator*() const override { return _at; }
    int  line() const override { return _line; }
    void operator++() override { _bump(); }
    void operator--() override { _unget(); }

    bool getline(SBuf& sb) override
    {
        // return false if no data and EOF

        sb.clear();
        if (!ok()) return false;

        for (;;)
        {
            _bump();
            if (!_at || _at == '\n') break;
            if (_at == '\r') continue;
            sb.add((char)_at);
        }

        return _at || sb.size();
    }

protected:

    void _bump()
    {
        // set `at` to the next character. 0 at EOF
        
        int c;

        for (;;)
        {
            c = _get();
            if (c == EOF) break;

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

        if (c == EOF) c = 0;
        _at = c;
    }

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
            c = fgetc(_fp);
            _hold[_holdpos] = c;
        }

        if (c) _holdpos = (_holdpos + 1) & (maxhold-1);
        return c;
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

struct StreamReader: public StreamFile
{
    // read a file as a stream
    
    std::string      _name;
    
    ~StreamReader() { close(); }

    bool open(const std::string& name)
    {
        close();
        _name = name;
        _fp = fopen(_name.c_str(), "r");
        if (ok()) _bump(); // load first char
        return ok();
    }

    bool close()
    {
        if (_fp)
        {
            fclose(_fp);
            _fp = 0;
            return true;
        }
        return false;
    }

};

