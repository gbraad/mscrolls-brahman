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
 

#include <iostream>
#include <string>
#include "ificlient.h"
#include "ifihost.h"

int main(int argc, char** argv)
{
    IFIClient client;
    IFIHost host;

    // start the host thread
    host.start(argc, argv);

    // pass to client
    client.setLogLevel(Logged::_logLevel);

    // plug the host handler into the client
    using std::placeholders::_1;
    client.setEmitter(std::bind(&IFIHost::emitterHandler, &host, _1));

    // start the back-end
    client.start(argc, argv);

    for (;;)
    {
        std::string cmd;
        std::cout << "> "; std::cout.flush();
        std::getline(std::cin, cmd);
        if (cmd == "quit") break;

        if (cmd.empty()) continue; // blank line

        const char* cmdp = cmd.c_str();

        GrowString gs;

        if (*cmdp == '{')
        {
            // incorporate some direct json
            JSONWalker jw(cmdp);
            for (; jw.nextKey(); jw.next()) jw.skipValue();
            if (!jw.ok())
            {
                LOG1("malformed JSON in input: '", jw << "'\n");
                continue;
            }
            else
            {
                //std::cout << "parsed: '" << jw << "'\n";
                
                // append { ... but NOT closing "}"
                gs.append(jw._json, jw._pos - jw._json - 1);
                jw.skipSpace();
                cmdp = jw._pos; // any remainder is command
            }
        }
        else
        {
            gs.add('{');
        }


        if (*cmdp)
            JSONWalker::addStringValue(gs, IFI_COMMAND, cmdp);
        
        gs.add('}');
        gs.add(0);
        client.eval(gs.start());

        // needed if output is in same window as input
        if (client.sync()) client.release();
    }

    return 0;
}
