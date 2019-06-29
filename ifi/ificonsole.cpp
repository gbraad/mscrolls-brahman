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
#include "ifi.h"
#include "ifihost.h"
#include "logged.h"
#include "opt.h"
#include "strutils.h"
#include "fd.h"

struct Handler: public IFIHandler
{
    IFIHost*        _host;
     
    Handler(IFIHost* host) : _host(host) {}

    bool ifiText(const string& s) override
    {
        if (!s.empty())
        {
            std::cout << s;
            std::cout.flush();
        }
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
        string path = makePath(dataDir, changeSuffix(f, ".sav"));

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

    bool ifiLoadData(const string& s) override
    {
        // NB: this can be a request or a response.
        // when a request: `s` is the data
        // when a response: `s` is optionally, a filename
        // and although a "response" is actually a request to load
        
        string f = s;

        // otherwise invent our own
        if (f.empty()) f = "save";

        string dataDir = getProp(IFI_DATADIR).toString();
        string path = makePath(dataDir, changeSuffix(f, ".sav"));
        
        FD fd;
        if (fd.open(path))
        {
            uchar* data = fd.readAll();
            if (data)
            {
                GrowString js;
                buildJSONStart(js);        

                JSONWalker::addRawStringValue(js, IFI_LOADDATA, (char*)data);
                buildJSONEnd();

                LOG3("Loading ", path);

                // send
                _host->eval(js.start());
                _host->syncRelease(); // refresh when not in sync already
                
                delete [] data;
            }
            else
            {
                LOG1("IFI loadGame, error reading '", path << "'");
            }
        }
        else
        {
            LOG2("IFI loadGame, unable to open '", path << "'");
        }
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
    bool help = false;
    
    char** args = argv+1;
    for (char**& ap = args; *ap; ++ap)
    {
        char* arg;
        if ((arg = Opt::nextOptArg(ap, "-configdir")) != 0) configdir = arg;
        if ((arg = Opt::nextOptArg(ap, "-datadir")) != 0) datadir = arg;
        if ((arg = Opt::nextOptArg(ap, "-story")) != 0) story = arg;
        if ((arg = Opt::nextOptArg(ap, "-d", true)) != 0)
            Logged::_logLevel = atoi(arg);
        if (Opt::nextOpt(ap, "-h")) help = true;
    }
    Opt::rebuildArgs(argc, argv);

    if (help)
    {
        std::cout << "Usage: " << argv[0] << "[-d <level>] [-configdir <path>] [-datadir <path>] -story <storyname>\n";
        return 0;
    }

    IFIHost host;
    Handler h(&host);

    h.setProp(IFI_CONFIGDIR, configdir);
    h.setProp(IFI_DATADIR, datadir);
    if (story) h.setProp(IFI_STORY, story);

    host.setHandler(&h);
    host.setIFI(ifi);

    // initial json tells back-end various directories & story
    GrowString js;
    h.buildJSONStart(js);
    h.buildInitialJSON();
    h.buildJSONEnd();
    
    // duplicate existing argc/argv with making space for more args
    // NB: copy must be deleted later
    argv = Opt::copyArgs(argc, argv, 2);
    LOG3("IFIConsole, passing start json, ", js.start());
    Opt::addArg(argc, argv, "-e", js.start());

    // plug the host handler into the client
    ifi->setEmitter(&IFIHost::emitter, &host);
    
    // start the back-end
    ifi->start(argc, argv);

    // perform initial sync to allow game to start
    if (!host.sync())
    {
        LOG1("IFIConsole, client not running; ", js.start());
        delete ifi;
        return -1;
    }
    
    host.release();
    h._startDone = true;

    // we guarantee the back-end will receive some prologue json
    // *before* any commands. This allows the back-end to get ready
    // after it has been started, but *also* to allow the back-end to
    // hold off from properly starting until the first eval (if necessary).
    h.buildJSONStart(js);
    h.buildPrologueJSON();
    h.buildJSONEnd();

    if (host.eval(js.start()))
    {
        // allow it to process
        host.syncRelease();
    }

    /* some games wait until they've received a {begin:true} code */
    const char* beginjs = "{\"" IFI_BEGIN "\":true}";
    LOG4("Sending ifi begin, ", beginjs);
    if (host.eval(beginjs)) host.syncRelease();
    
    bool done = false;

    while (!done)
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
            
            host.eval(js.start());

            do
            {
                done = !host.sync();
            } while (!done && host.release());
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
