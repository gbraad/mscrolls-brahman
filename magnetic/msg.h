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

#include <set>
#include <string>
#include <cstdlib>
#include <functional>
#include "cutils.h"
#include "logged.h"

extern "C"
{
#include "defs.h"
};

struct Messages
{
    typedef std::string string;
    
    struct Msg
    {
        int             _index;
        string          _text;

        bool operator<(const Msg& m) const { return _index < m._index; }
    };

    typedef std::set<Msg>               MessageSet;
    typedef std::function<std::string(int, const char*)>    Hook;

    MessageSet          _messages;
    int                 _line;
    Hook                _hook;

    bool start(const char* filename);
    void emitMsg(int n, const char* s, outfn fn);
    void emitChars(const char* s, outfn fn);
    
    int                 _getc(FILE* fp)
    {
        int c = getc(fp);
        if (c == '\r') c = getc(fp);  // ignore
        if (c == '\n') ++_line;
        return c;
    }

    void                _ungetc(char c, FILE* fp)
    {
        if (c == '\n') --_line;
        ungetc(c, fp);
    }

    void                skipSpace(FILE* fp)
    {
        int c;
        while ((c = _getc(fp)) != EOF)
        {
            if (!u_isspace(c)) 
            {
                _ungetc(c, fp);
                break;
            }
        }
    }

    void                nextLine(FILE* fp)
    {
        // skip all until newline
        int c;
        while ((c = _getc(fp)) != EOF)
            if (c == '\n') break;
    }

    bool                readNumber(FILE* fp, int& number)
    {
        bool res = false;
        char buf[16];
        int c;
        size_t i = 0;
        skipSpace(fp);
        while ((c = _getc(fp)) != EOF)
        {
            if (!u_isdigit(c)) 
            {
                _ungetc(c, fp);
                break;
            }
            if (i < sizeof(buf)-1) buf[i++] = c;
        }

        if (i)
        {
            buf[i] = 0;
            number = atoi(buf);
            res = true;
        }
        return res;
    }

    bool readString(FILE* fp, std::string& s)
    {
        // read a quoted string

        bool res = false;
        skipSpace(fp);
        int c;
    
        c = _getc(fp);
        if (c == '"')
        {
            bool esc = false;
            res = true;
            while ((c = _getc(fp)) != EOF)
            {
                if (esc)
                {
                    if (c == 'n') c = '\n';
                    esc = false;
                }
                else
                {
                    if (c == '\\')
                    {
                        esc = true;
                        continue;
                    }
                    else if (c == '"')
                    {
                        // end quote
                        break;
                    }
                }

                s += c;
            }
        }
        return res;
    }


    bool                readFromMsgFile(const char* filename);
    bool                readFromTextFile(const char* filename);

    const Msg* find(int index) const
    {
        const Msg* res = 0;
        Msg target;
        target._index = index;

        MessageSet::const_iterator it = _messages.find(target);
        if (it != _messages.end())
            res = &*it;

        return res;
    }
    
};
