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
 *  Copyright (c) Strand Games 2020.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@strandgames.com
 *
 */


#pragma once

#include "term.h"
#include "list.h"
#include "utf8.h"

struct Stringt: public Head
{
    Stringt(const char* s, size_t n) : Head(type_string) { _init(s, n); }
    Stringt(const char* s) : Head(type_string) { _init(s, u_strlenz(s)); }
    Stringt(const std::string& s) : Head(type_string)
    { _init(s.c_str(), s.size()); }
    Stringt() : Head(type_string) { _init(0, 0); }

    ~Stringt()
    { 
        if (owned())
        {
            DB("~" << *this);
            _purge();
        }
    }

    size_t size() const { return u_strlenz((const char*)_n); }
    bool isEmpty() const { return !_n; }
    operator const char*() const { return (const char*)_n; }
    const char* c_str() const { return _n ? (const char*)_n : ""; }

    size_t length() const
    {
        // character length (eg utf8)
        size_t n = 0;
        if (_n)
        {
            Utf8 u((const char*)_n);
            n = u.length();
        }
        return n;
    }

    // convert to sbuf, for string output
    void toSBuf(SBuf& sb) const
    {
        bool quotes = !sb.modeNoQuotes();
        if (quotes) sb.add('"');
        if (_n)
            sb.add((const char*)_n);
        if (quotes) sb.add('"');
    }
    
    int compare(const Stringt& v) const
    {
        if (_n == v._n) return 0; // identical node

        if (!_n)
            return -1; // 0 < "X"
        else 
            if (!v._n) return 1;  // "X" > 0

        // NB: handle nulls
        return u_strcmpz((const char*)_n, (const char*)v._n);
    }

    void split(List& list, const char* tokens) const
    {
        // split this string into a list of strings separated by `tokens'
        // `tokens' is a set a individual characters.

        const char* s = *this;
        
        List::iterator it(list);
        it.end();
        
        for (;;)
        {
            while (u_strchr(tokens, *s)) ++s;
            if (!*s) break;

            const char* st = s;
            do
            {
                ++s;
            } while (*s && !u_strchr(tokens, *s));
            
            it.insert(Stringt(st, s - st));
        }
    }

    int subst(Stringt& res, const char* pat, const char* sub) const
    {
        // substitute `pat` for `sub' in this string, putting into `res'ult.
        // does not change this string.

        // return the number of substitutions performed. 
        // IMPORTANT: if no substitutions performed, leave `res' untouched.
        // this allows caller to return original string.

        // if `pat' is empty, insert `sub' at the start of the string.

        size_t plen = pat ? strlen(pat) : 0;
        size_t sublen = sub ? strlen(sub) : 0;

        const char* s = *this;
        const char* st = s;
        int cc = 0;

        SBuf sb;
        int n = (s ? strlen(s) : 0) - plen;
        while (n >= 0)
        {
            if (strncmp(s, pat, plen))
            {
                // no match.
                ++s;
                --n;
            }
            else
            {
                ++cc;
                sb.write(st, s - st);
                sb.write(sub, sublen);

                // an empty pattern performs a single subst at the start.
                if (!plen) break;

                s += plen;
                n -= plen;
                st = s;

            }
        }

        // only copy remainder and finish result string if we have
        // substitutes, otherwise leave `res' alone.
        if (cc)
        {
            // add remainder
            n += (s - st) + plen;
            sb.write(st, n);
            sb.add('\0');
            
            res._purge();
            res._n = (Node*)sb.donate();
        }
        return cc;
    }

    void append(const char* s, int amt)
    {
        if (amt > 0 && s)
        {
            if (isEmpty())
            {
                _init(s, amt);
            }
            else
            {
                int len = size();
                int sz = len + amt;
                char* p = (char*)realloc(_n, sz + 1);
                assert(p);
                memcpy(p + len, s, amt);
                p[sz] = 0;
                _n = (Node*)p;
            }
        }
    }

    void fold(SBuf& sb) const
    {
        size_t sz = size();
        sb.writeInt32(sz);
        sb.write(_n, sz);
    }

    void unfold(SBuf& sb) 
    {
        _n = 0;
        size_t sz = sb.readInt32(); 
        if (sz)
        {
            _n = (Node*)new char[sz + 1];
            sb.read(_n, sz);
            ((char*)_n)[sz] = 0;
        }
    }

    static Term parse(Stream& s)
    {
        SBuf sb;

        if (_parse(s, sb))
        {
            // Can join strings at parse time with "foo"+"bar"
            while (*s == '+')
            {
                ++s; // skip +
                while (u_isspace(*s)) ++s; 
                if (!_parse(s, sb)) break;
            }

            return Stringt(sb, sb.size());
        }

        return Term();
    }

private:

    static bool _parse(Stream& s, SBuf& sb)
    {
        bool r = *s == '"';
        if (r)
        {
            ++s;
            bool escape = false;
            while (*s)
            {
                char c = *s;
                ++s;
                    
                if (escape)
                {
                    escape = false;
                    if (c == 'n') c = '\n';
                }
                else
                {
                    if (c == '\\')
                    {
                        escape = true; 
                        continue;
                    }
                    if (c == '"')
                    {
                        // end of string
                        break;
                    }
                }
                sb.add(c);
            }
        }
        return r;
    }
    
    void _init(const char* s, int n) 
    {
        char* p = 0;
        if (s && n)
        {
            p = (char*)malloc(n+1);
            memcpy(p, s, n);
            p[n] = 0;
        }
        _n = (Node*)p;
    }

    void _purge()
    {
        if (_n) free(_n);
    }
};

