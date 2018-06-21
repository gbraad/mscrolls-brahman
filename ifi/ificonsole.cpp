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
#include "strutils.h"
#include "fd.h"

static std::string makePath(const std::string& prefix, const std::string& name) 
{
    std::string path;
    if (!name.empty())
    {
        // windows style or linux style absolute path given
        // then do not apply prefix
        if (name.find(':') != std::string::npos || name.at(0) == '/')
        {
            // if we have a prefix like C: or https:// then
            // assume `name` is an absolute path.
            path = name;
        }
        else
        {
            path = prefix;
            if (!path.empty()) path += '/';
            path += name;
        }

        // enough backslashes! windows files work forwards too!
        replaceCharsInplace(path, '\\', '/');
    }

    return path;
}

struct Handler: public IFIHandler
{
    bool ifiText(const string& s) override
    {
        std::cout << s;
        std::cout.flush();
        return true;
    }
    bool ifiMoves(int moveCount) override
    {
        std::cout << "current move count " << moveCount << std::endl;
        return true;
    }

    bool ifiSave(const uchar* data, int size, const string& name) 
    {
        // any suggested name?
        string f = name;

        // otherwise invent our own
        if (f.empty()) f = "save";

        string dataDir = getProp(IFI_DATADIR).toString();
        string path = changeSuffix(makePath(dataDir, f), ".sav");

        LOG3("ifiSave ", f);

        FD fd;
        bool r = fd.open(path.c_str(), FD::fd_new);

        if (r)
        {
            r = fd.write(data, size);
            if (r)
            {
                LOG3("saved ", path);
            }
            else
            {
                LOG("ifiSave, cannot write to file '", path << "'\n");
            }
        }
        else
        {
            LOG("ifiSave, can't open save file '", path << "'\n");
        }
        
        return r;
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
    const char* story = "story"; // stub default
    
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

    Handler h;

    h.setProp(IFI_CONFIGDIR, configdir);
    h.setProp(IFI_DATADIR, datadir);
    h.setProp(IFI_STORY, story);

    IFIHost host;
    host.setHandler(&h);

    // initial json tells back-end various directories & story
    GrowString js;
    h.buildJSONStart(js);
    h.buildInitialJSON();
    h.buildJSONEnd();
    
    // duplicate existing argc/argv with making space for more args
    // NB: copy must be deleted later
    argv = Opt::copyArgs(argc, argv, 2);
    LOG2("IFIConsole, passing start json, ", js.start());
    
    Opt::addArg(argc, argv, "-e", js.start());

    // plug the host handler into the client
    ifi->setEmitter(&IFIHost::emitter, &host);
    
    // start the back-end
    ifi->start(argc, argv);

    // perform initial sync to allow game to start
    if (host.sync(ifi)) ifi->release();
    else
    {
        LOG1("IFIConsole, client not running; ", js.start());
        delete ifi;
        return -1;
    }

    // we guarantee the back-end will receive some prologue json
    // *before* any commands. This allows the back-end to get ready
    // after it has been started, but *also* to allow the back-end to
    // hold off from properly starting until the first eval (if necessary).
    h.buildJSONStart(js);
    h.buildPrologueJSON();
    h.buildJSONEnd();

    if (host.sync(ifi))
    {
        ifi->eval(js.start());
        ifi->release();

        // allow it to process
        if (host.sync(ifi)) ifi->release();
    }

    for (;;)
    {
        std::string cmd;
        std::cout << h.getProp(IFI_PROMPT); std::cout.flush();
        std::getline(std::cin, cmd);
        if (cmd == "quit") break;

        if (cmd.empty()) continue; // blank line

        GrowString js;
        h.buildJSONStart(js);
        if (h.buildCmdJSON(cmd.c_str()))
        {
            h.buildJSONEnd();
            
            if (!host.sync(ifi)) break;
            ifi->eval(js.start());
            ifi->release();

            // needed if output is in same window as input
            if (!host.sync(ifi)) break;        
            ifi->release();
        }
    }

    Opt::deleteCopyArgs(argv);

    // will stop client thread and release engine
    delete ifi;

    // finish any remaining output
    host.drainQueue();

    LOG3("IFIConsole, ", "finished");

    return 0;
}
