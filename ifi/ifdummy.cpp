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

//// dummy game


bool mainloop(void* ctx)
{
    IFIClient* ifi = (IFIClient*)ctx;
    for (;;)
    {
        char c = ifi->getchar();
        if (c) ifi->putchar(c);
        else ifi->putstring(" (end)");
    }
    return true;
}

void setupBack(IFIClient& c)
{
    c.setMainLoop(mainloop, &c);
}

///// dummy front

void setupFront(IFIHost& host, IFI* client)
{
    client->setEmitter(&IFIHost::emitterHandler, &host);
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
        std::cin >> cmd;
        if (cmd == "quit") break;

        GrowString gs;
        IFIClient::makeCommand(gs, cmd.c_str());
        ificlient.eval(gs.start());
    }

    return 0;
}
