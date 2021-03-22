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
#include <functional>
#include <string>
#include "ifi.h"
#include "ifihost.h"
#include "logged.h"
#include "strutils.h"
#include "fd.h"
#include "imtext.h"

#define CHOICE_TEXTXX

#define META_PREFIX "meta_"

struct SDLHandler: public IFIHandler
{
    IFIHost*        _host;
    ChoicesInfo*    _choice = 0;

    typedef std::function<void(const char*)> TextEmitter;
    TextEmitter     _emitter;

    string          _title;
     
    SDLHandler(IFIHost* host) : _host(host) {}
    ~SDLHandler() { delete _choice; }


    void _emit(const string& s)
    {
        assert(_emitter);
        if (!s.empty()) _emitter(s.c_str());
    }

    void _emit(const char* s)
    {
        assert(_emitter);
        _emitter(s);
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

    bool ifiTitleTextResponse(const string& s) override
    {
        // in game title such as location 
        LOG3("got title ", s);
        _title = s;
        return true;
    }

    bool ifiMetaResponse(const string& js) override
    {
        LOG1("Got meta ", js);
        for (JSONWalker jw(js); jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);
            if (!st) break; // bad json

            if (!isObject)
            {
                // throw all the meta data into the handler propset
                var v = jw.collectValue(st);
                if (v) setProp(string(META_PREFIX) + jw._key, v);
            }
        }
        return true;
    }

    string getGameTitle()
    {
        // meta title of game for window header
        var v = _props.find(META_PREFIX IFI_TITLE);
        return v ? v.toString() : string();
    }

    /////

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

    // NB: build with IFI_HANDLE_CHOICE
    bool ifiHandleChoicesInfo(ChoicesInfo* ci) override
    {
        assert(ci);

        // remove any pending
        if (_choice) delete _choice;

        // we keep the donated choices given to us, and delete it later
        _choice = ci;

#ifdef CHOICE_TEXT        
        _emit("\n");

        if (_choice->_header)
        {
            _emit(_choice->_header._text);
            _emit("\n");
        }
        presentChoices();
#endif        
        
        return true;
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

    void pumpfn()
    {
        //static int cc;
        //LOG1("pump! ", ++cc);
        flush();
        const char* s = gui_input_pump();
        if (s)
        {
            // XX
            // should figure out if this is a choice
            // then use the json response in choiceinfo
            // but for now, we only have numbers anyway.
            
            GrowString js;
            buildJSONStart(js);
            buildCmdJSON(s);
            buildJSONEnd();

            // clear any choices after input
            delete _choice; _choice = 0;

            _host->eval(js.start());
        }
    }
};


struct History
{
    typedef std::string string;
    static const int hSize = 10;
    
    string      _h[hSize];
    int         _pos = 0;
    int         _cur;

    void add(const char* s)
    {
        if (!*s) return;
        
        if (_pos >= hSize)
        {
            memcpy(_h, _h+1, sizeof(string));
            --_pos;
        }
        _h[_pos++] = s;
        _cur = _pos;
    }

    const char* up() 
    {
        if (_cur) return _h[--_cur].c_str();
        return 0;
    }

    const char* down()
    {
        if (_cur < hSize) return _h[_cur++].c_str();
        return 0;
    }
    
};

struct StrandCtx
{
    typedef std::string string;
    
    IFI*        ifi;

    const char* configdir = ".";
    const char* datadir = "."; // default;
    const char* story = "story.str";

    IFIHost host;
    SDLHandler h;
    IFI::Pump _p;

    char            _guiInputBuf[256];
    bool            _guiInputReady = false;
    string          _cmdLabel; // text to display
    History         _hist;
    ImText          _mainText;

    StrandCtx() : h(&host)
    {
        Logged initLog;
        Logged::_logLevel = 3; // XXX

        using namespace std::placeholders;  
        _p = std::bind(&SDLHandler::pumpfn, &h);
    }

    const char* getTitle() const { return h._title.c_str(); }

    void setLabel(const string& label)
    {
        _cmdLabel = "\n> ";
        _cmdLabel += label;
    }
    
    void sendCmd(const char* s, const string* label = 0)
    {
        strcpy(_guiInputBuf, s);
        _hist.add(_guiInputBuf);
        setLabel(label ? *label : s);
        _guiInputReady = true;
    }

    const char* yieldCmd(const char** label)
    {
        const char* s = 0;
        if (_guiInputReady)
        {
            s = _guiInputBuf;
            *label = _cmdLabel.c_str();
            _guiInputReady = false;
        }
        return s;
    }

    bool init(SDLHandler::TextEmitter& e)
    {
        ifi = IFI::create(&_p);
        if (!ifi)
        {
            e("Failed to create IFI");
            return false;
        }

        assert(story);
        h.setProp(IFI_CONFIGDIR, configdir);
        h.setProp(IFI_DATADIR, datadir);
        h.setProp(IFI_STORY, story);

        h._emitter = e;

        host.setHandler(&h);
        host.setIFI(ifi);

        // initial json tells back-end various directories & story
        GrowString js;
        h.buildJSONStart(js);
        h.buildInitialJSON();
        h.buildJSONEnd();
    
        // plug the host handler into the client
        ifi->setEmitter(&IFIHost::emitter, &host);
    
        // start the back-end
        static const char* argv[4];
        argv[0] = "moon";
        argv[1] = "-e";
        argv[2] = js.start();
        argv[3] = 0;

        LOG3("IFIConsole, sending boot json, ", js.start());
        ifi->start(3, (char**)argv);

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
        return true;
    }

    void finish()
    {
        // will stop client thread and release engine
        delete ifi;

        // finish any remaining output
        host.drainQueue();

        LOG3("IFIConsole, ", "finished");
    }
};
