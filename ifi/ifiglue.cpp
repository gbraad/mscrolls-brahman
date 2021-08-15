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
 *  Copyright (c) Strand Games 2018.
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


#include <assert.h>
#include "ificlient.h"

extern IFIClient* ifi;

extern "C"
{
    
int ifi_getchar()
{
    assert(ifi);
    return ifi->getchar();
}

int ifi_putchar(int c)
{
    assert(ifi);
    return ifi->putchar(c);
}

int ifi_printf(const char* m, ...)
{
    assert(ifi);
    va_list args;
    va_start(args, m);
    int n = ifi->vprintf(m, args);
    va_end(args);
    return n;
}

char* ifi_gets(char* s)
{
    char* p = s;
    int c;

    for (;;)
    {
        c = ifi_getchar();
        if (c <= 0 || c == '\n') break;
        *p++ = c;
    }
    
    *p = 0;
    return c <= 0 && p == s ? 0 : s;
}

char* ifi_gets_s(char* s, size_t sz)
{
    char* p = s;
    int c;

    if (!sz) return 0;
    --sz;

    for (;;)
    {
        c = ifi_getchar();
        if (c <= 0 || c == '\n') break;
        if (p - s < (int)sz) *p++ = c;
    }
    
    *p = 0;
    return c <= 0 && p == s ? 0 : s;
}

char* ifi_fgets(char* s, int n, FILE* fp)
{
    if (fp != stdin) return fgets(s, n, fp);
    if (n > 1)
    {
        // n-1 space for newline
        s = ifi_gets_s(s, n-1);
        if (s)
        {
            // append newline
            size_t sz = strlen(s);
            s[sz++] = '\n';
            s[sz] = 0;
        }
    }
    else
    {
        *s = 0;
    }
    return s;
}

int ifi_puts(const char* s)
{
    while (*s) ifi_putchar(*s++);
    ifi_putchar('\n');
    return 0;
}

int ifi_scanf(const char* f, ...)
{
   assert(0); // no
   return 0;
}

const char* ifi_getRequest()
{
    // C version 
    return ifi->getRequest();
}

void ifi_emitResponse(const char* json)
{
    // C version
    ifi->emitResponse(json);
}

void ifi_pump()
{
    // coop pumper
    assert(ifi);
    if (ifi->_pump) (ifi->_pump)();
}

void ifi_flush()
{
    assert(ifi);
    ifi->flush();
}

} // extern C
