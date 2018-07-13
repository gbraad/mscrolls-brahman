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

#include <iostream>
#include <vector>
#include <assert.h>

#include "types.h"
#include "cutils.h"

struct POS
{
    // part of speech
    static const int punct = 1;
};

struct Tokeniser
{
    typedef std::string string;

    struct Word
    {
        Word() {}
        Word(Tokeniser* host, const char* s, size_t n, int pOS = 0)
            : _text(s, n), _POS(pOS)
        {
            // remember offset in original text
            _offset = s - host->_baseString;
        }                                        
        
        string          _text;
        uint            _offset;
        int             _POS;

    };
        
    typedef std::vector<Word> Tokens;

    Tokens              _tokens;
    const char*         _baseString;

    size_t      size() const { return _tokens.size(); }
    const char* operator[](int i) const { return _tokens[i]._text.c_str(); }
    const Word&  word(int i) const { return _tokens[i]; }
    Word& word(int i) { return _tokens[i]; }

    bool        isPOS(size_t i, int pOS) const
    {
        assert(i < _tokens.size());
        return _tokens[i]._POS == pOS;
    }

    void clear()
    {
        _tokens.clear();
        _baseString = 0;
    }

    void tokenise(const string& s) { tokenise(s.c_str()); }
    void tokenise(const char* s)
    {
        clear();
        _baseString = s;
        _tokeniseSimple(s);
    }

    string toString() const
    {
        string s;
        for (size_t i = 0; i < _tokens.size(); ++i)
        {
            if (i) s += '|';
            s += _tokens[i]._text;
        }
        return s;
    }

    int lastWord() const
    {
        // return index of last non-punct, -1 if none
        
        size_t n = size();
        while (n > 0)
        {
            if (_tokens[n-1]._POS == POS::punct) --n;
            else break;
        }
        --n;
        return (int)n;
    }

    string consolidate() const
    {
        // turn back into a string
        string s;
        if (size() > 0)
        {
            size_t i;
            for (i = 0; i < size(); ++i)
            {
                if (!s.empty() && _tokens[i]._POS != POS::punct)
                    s += ' ';

                s += _tokens[i]._text;
            }
        }
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Tokeniser t)
    { return os << t.toString(); }


private:

    void _tokeniseSimple(const char* s)
    {
        // simply break up on white space and deal with straightforward
        // punctuation cases.
        while (*s)
        {
            while (u_isspace(*s)) ++s;
            if (!*s) break;
            const char* st = s++;
            while (*s && !u_isspace(*s)) ++s;
            _handleWord(st, s - st);
        }
    }

    void _handleWord(const char* s, size_t n)
    {
        // one token separated by whitespace

        // handle punctuation at the start
        while (!u_isalnum(*s))
        {
            // allow these to stay with the word eg $100
            // NB: clang doesnt like pound sign 0xa3
            if (strchr("$-+#@.", *s) || ((int)(*s)) == 0xa3)
                break;
            
            // otherwise separate token
            _addToken(s, 1, POS::punct);
            ++s;
            if (!--n) break;
        }

        if (n)
        {
            // find end of word
            const char* ep = s + n;
            const char* e = ep;

            while (e > s && !u_isalnum(e[-1])) --e;

            // e is first non-alphanum, or e == s if all non-alnum

            if (e > s)
            {
                // add middle bit (which may have internal punctuation)
                _handleMiddle(s, e - s);
                s = e;
            }
            
            // any remainder is all punct. each becomes separate token
            while (s < ep)
                _addToken(s++, 1, POS::punct);
        }
    }

    void _handleMiddle(const char* s, size_t sz)
    {
        // one word fragment.
        // many cases we want to leave as one token.
        // eg 11.00   1.23  etc.
        // some punctuation however are always separators.

        static const char* seps = ",/:;";

        const char* es = s + sz;
        const char* sp = s;
        while (sp != es)
        {
            if (strchr(seps, *sp))
            {
                // separate
                if (sp != s) _handleAlphaNum(s, sp - s);
                _addToken(sp, 1, POS::punct);
                s = sp + 1;
            }
            ++sp;
        }
        if (sp != s) _handleAlphaNum(s, sp - s);
    }

    void _handleAlphaNum(const char* s, size_t sz)
    {
        const char* ep = s + sz;
        const char* st = s;

        for (;;)
        {
            const char* s1 = st + 1;
            if (s1 == ep) break;
            if ((u_isalpha(*st) && u_isdigit(*s1)) ||
                (u_isalpha(*s1) && u_isdigit(*st)))
            {
                // change over
                _addToken(s, s1 - s);
                s = s1;
            }
            ++st;
        }
        _addToken(s, ep - s);
    }

    void _addToken(const char* s, size_t n, int pOS = 0)
    {
        _tokens.emplace_back(Word(this, s, n, pOS));
    }

};

