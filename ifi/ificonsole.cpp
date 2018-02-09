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


#include <iostream>
#include <string>
#include "ificlient.h"
#include "ifihost.h"


void setupBack(IFIClient& c)
{
    extern bool IFIStart(IFIClient* client);
    c.setMainLoop(IFIStart);
}

void setupFront(IFIHost& host, IFI* client)
{
    using std::placeholders::_1;
    client->setEmitter(std::bind(&IFIHost::emitterHandler, &host, _1));
    client->start();

    host.start();
}

int main()
{
    IFIClient ificlient;
    setupBack(ificlient);

    IFIHost host;
    setupFront(host, &ificlient);

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
                std::cout << "malformed JSON in input: '" << jw << "'\n";
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
        ificlient.eval(gs.start());

        // needed if output is in same window as input
        if (ificlient.sync()) ificlient.release();
    }

    return 0;
}
