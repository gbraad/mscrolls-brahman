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

#include <stdio.h>

#ifdef IFI_BUILD

#include "ificlient.h"

static IFIClient* ifi;

#undef getchar
#undef putchar

#define main IFIMain

// forward
int main(int argc, char** argv);

static int getchar()
{
    return ifi->getchar();
}

static int putchar(int c)
{
    ifi->putchar(c);
    return c;
}

static int fflush(FILE* fp)
{
    ifi->putchar(0);
    return 0;
}

bool IFIStart(IFIClient* client)
{
    ifi = client;

    int argc = 0;
    char* arg1 = 0;
    char** argv = &arg1;
    
    main(argc, argv);
    return false; // over
}

#endif

static char* getline()
{
    static char buf[4096];
    char* p = buf;
    int c;
    
    while ((c = getchar()) != EOF && c != '\n')
        if (p - buf < sizeof(buf)-1) *p++ = c;

    *p = 0;
    return c == EOF && p == buf ? 0 : buf;
}

#ifdef IFI_BUILD
static void prompt() {}

static void handleRequests()
{
    const char* json = ifi->getRequest();
    if (!json) return;

    //std::cout << "handle requests '" << json << "'\n";

    for (JSONWalker jw(json); jw.nextKey(); jw.next())
    {
        jw.skipValue();
        if (jw._key != IFI_COMMAND)
        {
            std::cout << "request, " << jw._key << std::endl;
        }
    }
}

#else
static void prompt()
{
    putchar('>');
    putchar(' ');
    fflush(stdout);
}

static void handleRequests() {}
#endif

int main(int argc, char** argv)
{
    for (;;)
    {
        prompt();

        handleRequests();
        
        char* s = getline();
        if (!s) break;

        while (*s)
        {
            putchar(*s);
            ++s;
        }

        putchar('\n');
    }

    return 0;
}



