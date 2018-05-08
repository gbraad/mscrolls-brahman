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
#include <stdlib.h>


#define DIM(_x)  (sizeof(_x)/sizeof(_x[0]))
#define ASIZE(_x)  ((int)(sizeof(_x)/sizeof(_x[0])))

// define our own set here
#define u__tolower(_c)    ( (_c)-'A'+'a' )
#define u__toupper(_c)    ( (_c)-'a'+'A' )

#define u_isupper(_c) ((_c) >= 'A' && (_c) <= 'Z')
#define u_islower(_c) ((_c) >= 'a' && (_c) <= 'z')
#define u_isalpha(_c) (u_isupper(_c) || u_islower(_c))
#define u_tolower(_c) (u_isupper(_c) ? u__tolower(_c) : (_c))
#define u_toupper(_c) (u_islower(_c) ? u__toupper(_c) : (_c))

#define u_isdigit(_c) ((_c) >= '0' && (_c) <= '9')
#define u_isnl(_c) ((_c) == '\n' || (_c) == '\r')
#define u_isspaceortab(_c) ((_c) == ' ' || (_c) == '\t')
#define u_isspace(_c) ((_c) == ' ' || (_c) == '\t' || u_isnl(_c))
#define u_isalnum(_c) (u_isalpha(_c) || u_isdigit(_c))
#define u_isintchar(_c) ((_c) == '-' || u_isdigit(_c))

static inline int u_isnumchar(int c)
{ return u_isintchar(c) || c == '.' || c == 'e' || c == 'E' || c == '+'; }

#define u_isprint(_c)  (((_c) >= 0x20) && ((_c) <= 127))

#define u_ishex(_c)    (u_isdigit(_c) || ((_c) >= 'A' && (_c) <= 'F') || ((_c) >= 'a' && (_c) <= 'f'))

#define u_hexv(_c) ((int)(u_isdigit(_c) ? (_c) - '0' :  (((_c) >= 'A' && (_c) <= 'F') ? (_c) - 'A' + 10 :  (((_c) >= 'a' && (_c) <= 'f') ? (_c) - 'a' + 10 : 0))))

#define u_hex(_v) ((char)((_v) < 10 ? '0' + (_v) : 'A' - 10 + (_v)))
#define u_isquote(_v) ((_v) == '"' || (_v) == '\'')


static inline int u_stricmp(const char* s1, const char* s2)
{
    int v;
    for (;;) 
    {
        v = u_tolower(*s1) - u_tolower(*s2);
        if (v || !*s1) break;
        ++s1;
        ++s2;
    }
    return v;
}

static inline int u_strnicmp(const char* s1, const char* s2, size_t n)
{
    int v = 0;
    while (n) 
    {
        --n;
        v = u_tolower(*s1) - u_tolower(*s2);
        if (v || !*s1) break;
        ++s1;
        ++s2;
    }
    return v;
}

static inline char* u_strchr(const char* s, int c)
{
    // NB: not actually the same as `strchr' real version will locate zero.
    while (*s) 
    {
        if (*s == c) return (char*)s;  /* XX */
        ++s;
    }
    return 0;
}

static inline char* u_strcpyn(char* dst, const char* src, size_t n)
{
    // strncpy variant that copyies at most `n` chars
    char* p = dst;
    while (n)
    {
        --n;
        if ((*p = *src) == 0) break;
        ++p;
        ++src;
    }
    return dst;    
}

static inline size_t u_strlenz(const char* s)
{
    // strlen that copes also with null
    size_t sz = 0;
    if (s)
        while (*s++) ++sz;
    return sz;
}

static inline int u_strcmpz(const char * s1, const char * s2)
{
    // strcmp that handles null
    if (s1 == s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;
    
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s1 == 0)
            return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}



#ifdef __cplusplus

inline void u_strdub(char*& dst, const char* src)
{
    // strcpy, but leave `dst' at terminating zero
    char* p = dst;
    while ((*p = *src) != 0)
    {
        ++p;
        ++src;
    }
    dst = p;
}

inline char* u_strdup(const char* s)
{
    return strcpy(new char[strlen(s)+1], s);
}

inline char* u_strndup(const char* s, size_t n)
{
    char* p = (char*)memcpy(new char[n+1], s, n);
    p[n] = 0;
    return p;
}

template<typename V> inline V u_abs(V _x) { return _x >= 0 ? _x : -_x; }
template<typename V> inline V u_max(V _a, V _b) { return _a >= _b ? _a : _b; }
template<typename V> inline V u_min(V _a, V _b) { return _a <= _b ? _a : _b; }

template<typename V> inline V u_max(V _a, V _b, V _c) 
{ return u_max(u_max(_a, _b), _c); }

template<typename V> inline V u_min(V _a, V _b, V _c) 
{ return u_min(u_min(_a, _b), _c); }

#else

#define u_abs(_x)  ((_x) >= 0 ? (_x) : -(_x))
#define u_max(_a, _b) ((_a) >= (_b) ? (_a) : (_b))
#define u_min(_a, _b) ((_a) <= (_b) ? (_a) : (_b))

#endif // __cplusplus

