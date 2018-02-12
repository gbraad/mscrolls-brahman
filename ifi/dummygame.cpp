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

/* Here we have a dummy back-end "game". All it does is echo back what
 * it has received.
 *
 * The code here can compile on it's own, and will make a (simple) console
 * program. However, if IFI_BUILD is defined, it will attach to IFI
 *
 * The first part under IFI_BUILD is glue to attach to IFIClient.
 *
 */

#ifdef IFI_BUILD

#include "ificlient.h"

static IFIClient* ifi;

// do some (somewhat unreliable) tricks to hijack IO into IFI 
#undef getchar
#undef putchar

#define getchar ifi->getchar
#define putchar ifi->putchar
#define printf ifi->printf

// need to move the original "main" out of the way, since there will be
// other main somewhere else when linking with IFI.
#define main IFIMain

// forward
int main(int argc, char** argv);

bool IFIStart(IFIClient* client)
{
    // Called by IFI at the start.
    // We're given the IFIClient object
    ifi = client;

    // call our original main. Fake args here, in case it looks at them.
    int argc = 0;
    char* arg1 = 0;
    char** argv = &arg1;

    main(argc, argv);
    return false; // over
}

static void handleRequests()
{
    // This optional function exists to hook whole JSON from IFI
    // we walk the terms, but ignore IFI_COMMAND, since that also goes to
    // `getchar` and is handled elsewhere.
    
    const char* json = ifi->getRequest();
    if (!json) return;

    for (JSONWalker jw(json); jw.nextKey(); jw.next())
    {
        jw.skipValue();
        if (jw._key != IFI_COMMAND)
        {
            std::cout << "request, " << jw._key << std::endl;
        }
    }
}

#endif // IFI_BUILD

/* The remainder of this file can compile standalone and run as a console
 * program.
 */

static char* getline()
{
    // a somewhat pedestrian getline
    static char buf[4096];
    char* p = buf;
    int c;
    
    while ((c = getchar()) != EOF && c != '\n')
        if (p - buf < sizeof(buf)-1) *p++ = c;

    *p = 0;
    return c == EOF && p == buf ? 0 : buf;
}

int main(int argc, char** argv)
{
    // OK, not much of a game :-)
    for (;;)
    {
#ifdef IFI_BUILD

        // here is one difference. If we want to handle JSON requests
        // as well as regular "commands", there has to be a handler somewhere.
        handleRequests();
#else

        // we also only need to prompt in the standalone case, because
        // when connected to the IFI, the front end may have the input
        // in a totally different window, which may (or may not) prompt
        // the user in its own way.
        putchar('>');
        putchar(' ');
        fflush(stdout);
#endif        

        char* s = getline();
        if (!s) break;

        printf("You said; \"%s\"\n", s);
    }

    return 0;
}



