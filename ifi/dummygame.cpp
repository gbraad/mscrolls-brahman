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
 *  contact@strandgames.com
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

// so that we can get it from glue
IFIClient* ifi;

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

        char buf[1024];
        char* s = gets_s(buf, sizeof(buf));
        if (!s || !strcmp(s, "q")) break;

        printf("You said, \"%s\".\n", s);
    }

    return 0;
}



