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
#include <string>
#include <ctype.h>
#include <vector>
#include "cutils.h"
#include "utf8.h"

inline std::string replaceAll(const std::string& source,
                              const std::string& from,
                              const std::string& to)
{
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append( source, lastPos, findPos - lastPos );
        newString += to;
        lastPos = findPos + from.length();
    }

    newString += source.substr( lastPos );
    return newString;
}

inline std::string replaceAll(const std::string& source,
                              char from,
                              char to)
{

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    findPos = source.find(from, lastPos);
    if (findPos == std::string::npos)
    {
        // string does not have any `from` chars
        return source;
    }

    std::string newString;
    newString.reserve(source.length()); 
    
    do
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + 1;
        findPos = source.find(from, lastPos);
    } while (findPos != std::string::npos);

    newString += source.substr(lastPos) ;
    return newString;
}


inline void replaceCharsInplace(std::string& source, char sc, char dc)
{
    // XX apparently you're not supposed to modify the internals
    // util C++17 ?
    char* p = const_cast<char*>(source.data());
    while (*p)
    {
        if (*p == sc) *p = dc;
        ++p;
    }
}

inline bool startsWith(const char* s1, const char* pat)
{
    for(; *pat && *s1 == *pat; ++s1, ++pat) ;
    return !*pat;
}

inline bool startsWith(const std::string& s, const char* pat)
{ return startsWith(s.c_str(), pat); }

inline bool startsWithIgnoreCase(const char* s1, const char* pat)
{
    // XX consider utf8 lower case conversion
    for(; *pat && u_tolower(*s1) == u_tolower(*pat); ++s1, ++pat) ;
    return !*pat;
}

inline bool startsWithIgnoreCase(const std::string& s, const char* pat)
{ return startsWithIgnoreCase(s.c_str(), pat); }

inline bool endsWith(const char* s, char c)
{
    bool res = false;
    if (s)
    {
        size_t sz = strlen(s);
        res = sz > 0 && s[sz-1] == c;
    }
    return res;
}

inline bool endsWith(const std::string& s, char c)
{
    return endsWith(s.c_str(), c);
}

inline std::string suffixOf(const char* s)
{
    // eg ".dat"
    // find any end of path
    const char* sep = strrchr(s, '/');
    if (!sep) sep = strrchr(s, '\\');

    // any existing suffix must be after path
    const char* p = strrchr(s, '.');
    if (p && (!sep || p > sep)) return p;
    return std::string();
}

inline std::string suffixOf(const std::string& s)
{ return suffixOf(s.c_str()); }

inline std::string changeSuffix(const char* s, const char* suf)
{
    // add or change suffix of `s` to `suf'.
    // eg suf is ".moo"
    // suf 0 to remove

    std::string s2;

    // find any end of path
    const char* sep = strrchr(s, '/');
    if (!sep) sep = strrchr(s, '\\');

    // any existing suffix must be after path
    const char* p = strrchr(s, '.');
    if (p && (!sep || p > sep))
        s2 = std::string(s, p - s); // stem
    else
        s2 = s;

    if (suf) s2 += suf;
    
    return s2;
}


inline std::string changeSuffix(const std::string& s, const char* suf)
{
    return changeSuffix(s.c_str(), suf);
}

inline std::string trim(const std::string& s)
{
    const char* sp = s.c_str();
    const char* ep = sp + s.size();
    while (u_isspace(*sp)) ++sp;
    while (ep > sp && u_isspace(ep[-1])) --ep;
    return std::string(sp, ep - sp);
}

inline void trimInplaceLeft(std::string& s)
{
    const char* sp = s.c_str();
    const char* ep = sp;
    
    while (u_isspace(*ep)) ++ep;
    if (ep != sp) s.erase(0, ep - sp);
}

inline void trimInplaceRight(std::string& s)
{
    const char* sp = s.c_str();
    const char* ep = sp + s.size();
    
    while (ep > sp && u_isspace(ep[-1])) --ep;
    if (*ep) s.erase(ep - sp, std::string::npos);
}

inline void trimInplace(std::string& s)
{
    trimInplaceLeft(s);
    trimInplaceRight(s);
}

inline std::string toUpper(const char* s)
{
    Utf8 us(s);
    return us.toupper();
}

inline std::string toUpper(const std::string& s) { return toUpper(s.c_str()); }
inline std::string toLower(const char* s)
{
    Utf8 us(s);
    return us.tolower();
}

inline std::string toLower(const std::string& s) { return toLower(s.c_str()); }

// XX consider utf8
inline int compareIgnoreCase(const char* s1, const char* s2)
{ return u_stricmp(s1, s2) ; }

inline int compareIgnoreCase(const std::string& s1, const char* s2)
{ return compareIgnoreCase(s1.c_str(), s2); } 

inline int compareIgnoreCase(const char* s1, const std::string& s2)
{ return compareIgnoreCase(s1, s2.c_str()); }

inline int compareIgnoreCase(const std::string& s1, const std::string& s2)
{ return compareIgnoreCase(s1.c_str(), s2.c_str()); }

inline bool equalsIgnoreCase(const char* s1, const char* s2)
{ return compareIgnoreCase(s1, s2) == 0; }

inline bool equalsIgnoreCase(const std::string& s1, const char* s2)
{ return compareIgnoreCase(s1.c_str(), s2) == 0; } 

inline bool equalsIgnoreCase(const char* s1, const std::string& s2)
{ return compareIgnoreCase(s1, s2.c_str()) == 0; }

inline bool equalsIgnoreCase(const std::string& s1, const std::string& s2)
{ return compareIgnoreCase(s1.c_str(), s2.c_str()) == 0; }

inline bool lessIgnoreCase(const std::string& s1, const std::string& s2)
{ return compareIgnoreCase(s1.c_str(), s2.c_str()) < 0; }

inline bool sameWithUnderscore(const char* s1, const char* s2)
{
    // compare two strings ignoring case and allow space to match underscore
    for (;;) 
    {
        char c1 = *s1 == '_' ? ' ' : u_tolower(*s1);
        char c2 = *s2 == '_' ? ' ' : u_tolower(*s2);

        if (c1 != c2) return false;
        if (!c1) break;
        ++s1;
        ++s2;
    }
    return true;
}

inline void split(std::vector<std::string>& list, const char* p, char c = ' ')
{
    while (*p && *p == c) ++p;
    do
    {
        const char* start = p;
        while (*p && *p != c) ++p;
        list.emplace_back(std::string(start, p));
        while (*p && *p == c) ++p;
    } while (*p);
}


inline void split(std::vector<std::string>& list, 
                  const std::string& s, char c = ' ')
{
    split(list, s.c_str(), c);
}

inline std::vector<std::string> split(const std::string& s, char c = ' ')
{
    std::vector<std::string> list;
    split(list, s, c);
    return list;
}

inline std::string unsplit(const std::vector<std::string>& ws,
                           const char* sep, size_t start, size_t n)
{
    std::string res;
    size_t sz = ws.size();
    if (n > sz) n = sz;
    
    if (n > 0)
    {
        size_t i;
        for (i = start; i < n-1; ++i) { res += ws[i]; res += sep; }
        res += ws[i];
    }
    return res;
}

inline std::string unsplit(const std::vector<std::string>& ws,
                           char c, size_t start, size_t n)
{
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    return unsplit(ws, buf, start, n);
}

inline std::string unsplit(const std::vector<std::string>& ws, char c = ' ')
{
    return unsplit(ws, c, 0, ws.size());
}

inline std::string unsplit(const std::vector<std::string>& ws, const char* sep)
{
    return unsplit(ws, sep, 0, ws.size());
}

inline const char* findFirst(const char* s, const char* pat)
{
    // find location of `pat' in `s', or 0 if none.
    // TODO: utf8
    return strstr(s, pat);
}

inline std::string simplifyChars(const char* s)
{
    // eliminate some unnecessary utf8
    static const uint from[] = 
        {
            0x2018, 0x2019,  // '
            0x2013, 0x2014,  // -
            0x201c, 0x201d,  // "
        };
    
    static const uint to[] = 
        {
            '\'', '\'',
            '-', '-',
            '"', '"',
        };

    Utf8 u(s);

    // map special spaces into normal ones 
    return u.map(from, to, ASIZE(from), Utf8::mflag_dropspaces);
}

inline std::string simplifyChars(const std::string& s)
{
    return simplifyChars(s.c_str());
}

inline std::string lastLineOf(const char* s, size_t sz,
                              size_t maxc = 0, // max chars to scan
                              size_t* offset = 0) // optional offset of line
{
    const char* ep = s + sz;

    // skip back any newlines or space on end
    while (ep != s && u_isspace(ep[-1])) --ep;
    
    const char* e = ep;
    while (e != s)
    {
        --e;
        if (ep - e == (int)maxc) break; // reached limit
        if (u_isnl(*e))
        {
            ++e;
            break;
        }
    }

    // either e == s, at start OR e is after a newline
    if (offset) *offset = e - s; // offset of last line in original
    return std::string(e, ep - e);
}

inline std::string lastLineOf(const std::string& s, 
                              size_t maxc = 0, size_t* offset = 0)
{
    return lastLineOf(s.c_str(), s.size(), maxc, offset);
}

inline bool isDigits(const char* p)
{
    while (*p) 
    {
        if (!u_isdigit(*p)) return false;
        ++p;
    }
    return true;
}

inline bool isDigits(const std::string& s) { return isDigits(s.c_str()); }

inline bool isNumeric(const std::string& s)
{
    const char* p = s.c_str();
    bool point = false;
    if (*p == '-') ++p; // allow initial minus
    while (*p) 
    {
        if (*p == '.')
        {
            // allow just one point
            if (point) return false;
            point = true;
        }
        else if (!u_isdigit(*p)) return false;
        ++p;
    }
    return true;
}

inline std::string firstWordOf(const char* s)
{
    while (isspace(*s)) ++s;
    const char* p = s;
    while (*p && !isspace(*p)) ++p;
    return std::string(s, p - s);
}

inline std::string firstWordOf(const std::string& s)
{
    return firstWordOf(s.c_str());
}

inline std::string lastWordOf(const char* s)
{
    const char* p = strrchr(s, ' ');
    if (p) s = p + 1;
    return s;
}

inline std::string lastWordOf(const std::string& s)
{
    return lastWordOf(s.c_str());
}

inline std::string CapitaliseStartWords(const std::string& s)
{
    // capitalise the start of each word
    std::string r;
    const char* p = s.c_str();

    while (*p)
    {
        while (isspace(*p)) r += *p++;
        if (!*p) break;
        r += u_toupper(*p);
        ++p;
        while (*p && !isspace(*p)) r += *p++;
    }
    return r;
}

#if 0
inline void subst(char* s, size_t n, const char* replace)
{
    // replace `n` chars at `s` with `replace`
    char* q = s + n;
    size_t len = strlen(q);
    size_t rlen = strlen(replace);
    memmove(s + rlen, q, len + 1); // +terminator
    memcpy(s, replace, rlen);
}
#endif

