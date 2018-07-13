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
#include <string.h>

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
#include "ifiglue.h"
#include "ifihandler.h"

// so that we can get it from glue
IFIClient* ifi;
static int moveCount;

struct Handler: public IFIHandler
{
    bool ifiCommand(const string&) override { return true; }
    bool ifiMoves(int n) override
    {
        if (n)
        {
            ifi->emitSingleResponse(IFI_MOVES, var(moveCount));
        }
        return true;
    }
        
};

Handler ifiHandler;

IFI* IFI::create()
{
    ifi = new IFIClient();
    return ifi;
}

int IFIClient::client_main(int argc, char** argv)
{
  // arrive here, then go to the original main, which is renamed to ifi_main
  // to avoid conflict with ::main in the front end.
  return ::main(argc, argv);
}

#endif // IFI_BUILD

/* The remainder of this file can compile standalone and run as a console
 * program.
 */

static void prompt()
{
#ifndef IFI_BUILD    
    // only need to prompt in the standalone case, because
    // when connected to the IFI, the front end may have the input
    // in a totally different window, which may (or may not) prompt
    // the user in its own way.
    putchar('>');
    putchar(' ');
    fflush(stdout);
#endif
}

int main(int argc, char** argv)
{
    // OK, not much of a game :-)

#ifdef IFI_BUILD
    // handle any initial JSON
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-e") && i < argc-1)
            ifiHandler.handle(argv[++i]);
    }
#endif

    for (;;)
    {
#ifdef IFI_BUILD

        // here is one difference. If we want to handle JSON requests
        // as well as regular "commands", there has to be a handler somewhere.
        ifiHandler.handle(ifi->getRequest());
#endif

        prompt();

        ++moveCount;

        char buf[1024];
        char* s = gets_s(buf, sizeof(buf));
        if (!s || !strcmp(s, "q")) break;
        if (*s)
        {
            printf("You said, \"%s\".\n", s);
        }
    }

    return 0;
}



