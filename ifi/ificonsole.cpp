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
#include "ifi.h"
#include "ifihost.h"
#include "logged.h"
#include "opt.h"

struct Handler: public IFIHandler
{
    bool ifiText(const string& s) override
    {
        std::cout << s << std::endl;
        return true;
    }
    bool ifiMovesResponse(int moveCount) override
    {
        std::cout << "current move count " << moveCount << std::endl;
        return true;
    }
};

int main(int argc, char** argv)
{
    Logged initLog;
    
    IFI* ifi = IFI::create();
    if (!ifi)
    {
        std::cout << "Failed to create IFI\n";
        return -1;
    }

    const char* configdir = ".";
    const char* datadir = "."; // default;
    const char* story = 0;
    
    char** args = argv+1;
    for (char**& ap = args; *ap; ++ap)
    {
        char* arg;
        if ((arg = Opt::nextOptArg(ap, "-configdir")) != 0) configdir = arg;
        if ((arg = Opt::nextOptArg(ap, "-datadir")) != 0) datadir = arg;
        if ((arg = Opt::nextOptArg(ap, "-story")) != 0) story = arg;
        if ((arg = Opt::nextOptArg(ap, "-d", true)) != 0)
            Logged::_logLevel = atoi(arg);
    }
    Opt::rebuildArgs(argc, argv);

    if (!story)
    {
        std::cout << "Usage: " << argv[0] << "[-d <level>] [-configdir <path>] [-datadir <path>] -story <storyname>\n";
        return 0;
    }

    // build simple json for startup
    GrowString js;
    js.add('{');
    JSONWalker::addStringValue(js, IFI_CONFIGDIR, configdir);
    JSONWalker::addStringValue(js, IFI_DATADIR, datadir);
    JSONWalker::addStringValue(js, IFI_STORY, story);
    js.add('}');
    js.add(0);


    IFIHost host;

    Handler h;
    host.setHandler(&h);
    
    // duplicate existing argc/argv with making space for more args
    // NB: copy must be deleted later
    argv = Opt::copyArgs(argc, argv, 2);
    LOG2("IFIConsole, passing start json, ", js.start());
    
    Opt::addArg(argc, argv, "-e", js.start());

    // plug the host handler into the client
    using std::placeholders::_1;
    ifi->setEmitter(std::bind(&IFIHost::emitterHandler, &host, _1));
    
    // start the back-end
    ifi->start(argc, argv);

    // perform initial sync to allow game to start
    if (ifi->sync())
    {
        host.drainQueue();
        ifi->release();
    }

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
        ifi->eval(gs.start());

        // needed if output is in same window as input
        if (!ifi->sync()) break;

        host.drainQueue();

        ifi->release();

    }

    Opt::deleteCopyArgs(argv);

    // release
    delete ifi;

    LOG3("IFIConsole, ", "finished");

    return 0;
}
