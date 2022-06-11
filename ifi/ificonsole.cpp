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

struct ConHandler: public IFIHandler
{
    IFIHost*        _host;
    ChoicesInfo*    _choice = 0;
    bool            _quit = false; 
     
    ConHandler(IFIHost* host) : _host(host) {}
    ~ConHandler() { delete _choice; }

    void _emit(const string& s)
    {
        if (!s.empty())
        {
            std::cout << s;
            std::cout.flush();
        }
    }

    bool ifiText(const string& s) override
    {
        _emit(s);
        return true;
    }
    
    bool ifiMoves(int moveCount) override
    {
        std::cout << "current move count " << moveCount << std::endl;
        return true;
    }

    bool ifiSave(const uchar* data, int size, const string& name) override
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

    void presentChoices()
    {
        if (_choice)
        {
            int cc = 0;
            for (auto& i : _choice->_choices)
            {
                string s = "(";
                ++cc;
                s += std::to_string(cc);
                s += ") ";
                s += i._text._text;
                s += '\n';
                _emit(s);
            }
        }
    }

    bool ifiPictureResponse(const string& js) override
    {
        string fname;

        if (!js.size()) return true; // ignore
        if (js[0] == '{')
        {
            // json
            LOG3("Got picture request ", js);

            for (JSONWalker jw(js); jw.nextKey(); jw.next())
            {
                bool isObject;
                const char* st = jw.checkValue(isObject);
                if (!st) break; // bad json
            
                if (!isObject)
                {
                    // throw all the meta data into the handler propset
                    var v = jw.collectValue(st);
                    if (jw._key == IFI_NAME) fname = v.toString();
                }
            }
        }
        else
        {
            // assume simple file name
            fname = js;
        }

        if (fname.size())
        {
            LOG1("\n[show picture ", fname << "]\n");
        }

        return true;
    }

#ifdef IFI_HANDLE_CHOICE    
    bool ifiHandleChoicesInfo(ChoicesInfo* ci) override
    {
        assert(ci);

        if (_choice)
        {
            //LOG1("IFI, old choice still present! ", *_choice);
            delete _choice;
        }

        // we keep the donated choices given to us, and delete it later
        _choice = ci;
        
        //LOG1("Handler for choices ", *_choice);

        _emit("\n");

        if (_choice->_header)
        {
            _emit(_choice->_header._text);
            _emit("\n");
        }

        presentChoices();
        
        return true;
    }
#endif // IFI_HANDLE_CHOICE    

#ifdef IFI_CONSOLE_CHOICE
    /* this code is needed when you want the console host to 
     * process the choice responses. Sometimes this is done in the engine
     */
    int handleInput(const string& line)
    {
        // return < 0 if done
        // return == 0 if not accepted
        // return > 0 if ok
        
        bool accept = false;
        int  chosen;
        
        string cmd = trim(line); // remove bogus whitespace
        
        if (_choice)
        {
            int nc = _choice->size();
            assert(nc);

            if (cmd.empty())
            {
                // blank line.
                if (nc == 1)
                {
                    // accept single choice
                    chosen = 1;
                    accept = true;
                }
            }

            if (!accept)
            {
                // extract a number
                chosen = 0;
                const char* p = cmd.c_str();
                while (u_isdigit(*p))
                {
                    chosen = chosen*10 + (*p - '0');
                    ++p;
                }

                if (!*p)
                {
                    // input was a single number on its own
                    // accept this as a choice response
                    if (chosen > 0 && chosen <= nc) accept = true;
                }
            }

            if (!accept)
            {
                // if we allow text input and this is not a choice
                // and also not blank, then accept it as cmd
                if (_choice->_textinput && !cmd.empty())
                {
                    accept = true;

                    // no longer a choice input
                    delete _choice; _choice = 0;
                }
            }
        }
        else
        {
            accept = !cmd.empty();
        }

        if (!accept) return 0; 

        bool done = false;
        bool handled = false;

        if (_choice)
        {
            // have accepted a value
            ChoiceList::iterator it = _choice->_choices.begin();
            if (chosen > 1) std::advance(it, chosen-1);

            ChoiceInfo ci = *it; // copy

            // and delete old choices
            delete _choice; _choice = 0;
            
            if (ci._response[0] == '{')
            {
                // response is JSON, so just use it.
                
                handled = true;
                LOG4("IFI choice, sending direct response ", ci._response);
                
                _host->eval(ci._response.c_str());
                do
                {
                    done = !_host->sync();
                } while (!done && _host->release());
            }
            else
            {
                // response is plain text, process as command
                cmd = ci._response;
            }
        }
        
        if (!handled && !done)
        {
            GrowString js;
            buildJSONStart(js);
            if (buildCmdJSON(cmd.c_str()))
            {
                buildJSONEnd();
                _host->eval(js.start());

                do
                {
                    done = !_host->sync();
                } while (!done && _host->release());
            }
        }

        if (done) return -1;
        return 1;
    }
#endif // IFI_CONSOLE_CHOICE

    void sendInput()
    {
        bool done = false;
        do
        {
            // wait for input
            std::string cmd;
            std::cout << getProp(IFI_PROMPT); std::cout.flush();
            std::getline(std::cin, cmd);
            done = true;

            if (cmd == "quit") _quit = true;
            else
            {
#ifdef IFI_CONSOLE_CHOICE
                int v = handleInput(cmd);
                if (!v) done = false; // get more input
#else
                GrowString js;
                buildJSONStart(js);
                buildCmdJSON(cmd.c_str());
                buildJSONEnd();
                _host->eval(js.start());
#endif
            }
        } while (!done);
    }

    bool flush()
    {
        // wait for input
        bool done;

        for (;;)
        {
            done = !_host->sync();
            if (done) break;
            _host->release();
            break;
        }
        return done;
    }

    bool pumpfn()
    {
        //LOG4("console, ", "Pump");
        flush();
        sendInput();
        return !_quit;
    }
    
};


static const char* configdir = ".";
static const char* datadir = "."; // default;
static const char* story = "story.str"; // default

static bool coop = false;

int start(int argc, char** argv)
{
    LOG1("IFIConsole start, ", story);
    
    IFIHost host;
    ConHandler h(&host);

    IFI::Ctx ifiCtx;

    using namespace std::placeholders;
    ifiCtx._p = std::bind(&ConHandler::pumpfn, &h);

    IFI* ifi = IFI::create(coop ? &ifiCtx : 0);
    if (!ifi)
    {
        LOG1("Console, ", "Failed to create IFI");
        return -1;
    }

    assert(story);
    h.setProp(IFI_CONFIGDIR, configdir);
    h.setProp(IFI_DATADIR, datadir);
    h.setProp(IFI_STORY, story);

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
    LOG3("IFIConsole, starting back-end", "");
    ifi->start(argc, argv);

    // perform initial sync to allow game to start
    if (!host.sync())
    {
        LOG1("IFIConsole, client not running; ", js.start());
    }
    else
    {
        host.release();
        h._startDone = true;
        
        // we guarantee the back-end will receive some prologue json
        // *before* any commands. This allows the back-end to get ready
        // after it has been started, but *also* to allow the back-end to
        // hold off from properly starting until the first eval (if necessary).
        h.buildJSONStart(js);
        h.buildPrologueJSON();
        h.buildJSONEnd();

        LOG3("IFIConsole, sending start json, ", js.start());

        if (host.eval(js.start()))
        {
            // allow it to process
            host.syncRelease();
        }

        /* some games wait until they've received a {begin:true} code */
        const char* beginjs = "{\"" IFI_BEGIN "\":true}";
        LOG3("Sending ifi begin, ", beginjs);
        if (host.eval(beginjs)) host.syncRelease();

        while (!h._quit)
        {
            h.sendInput();
            if (h.flush()) break;
        }

        // finish any remaining output
        host.drainQueue();
    }

    // will stop client thread and release engine
    delete ifi;

    Opt::deleteCopyArgs(argv);

    LOG3("IFIConsole, finished ", story);

    return 0;
}

int main(int argc, char** argv)
{
    Logged initLog;
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
        if (Opt::nextOpt(ap, "-coop"))
        {
            coop = true;
            LOG1("Running in COOP mode", "");
        }
    }

    if (help)
    {
        std::cout << "Usage: " << argv[0] << "[-d <level>] [-configdir <path>] [-datadir <path>] -story <storyname>\n";
        return 0;
    }

    Opt::rebuildArgs(argc, argv);

    int r;
    std::string astory;

    for (;;)
    {
        r = start(argc, argv);
        if (r) break;

        std::cout << "Next story: "; 
        std::cin >> astory;
        LOG1("IFIConsole, new story, ", astory);
        astory = trim(astory);
        if (astory.empty() || astory == "quit") break;
        story = astory.c_str();
    }

    LOG3("IFIConsole, ", "finished");
    return r;
}
