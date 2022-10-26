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
#include <stdio.h>
#include <functional>
#include <string>
#include "ifi.h"
#include "ifihost.h"
#include "logged.h"
#include "strutils.h"
#include "fd.h"
#include "imtext.h"
#include "wordstat.h"

#define CHOICE_TEXTXX

#define META_PREFIX "meta_"

struct SDLHandler: public IFIHandler
{
    IFIHost*        _host;
    ChoicesInfo*    _choice = 0;

    typedef std::function<void(const char*)> TextEmitter;
    TextEmitter     _emitter;

    string          _title;

    string          _requestSaveFile;
    string          _requestLoadFile;
     
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

    string prompt() const
    {
        return getProp(IFI_PROMPT).toString();
    }

    void emitPostSave(bool v)
    {
        // send a message to the engine to indicate save worked or not.
        GrowString js;
        buildJSONStart(js);        

        JSONWalker::addBoolValue(js, IFI_POSTSAVE, v);
        buildJSONEnd();

        // send
        _host->eval(js.start());
    }

    void emitPostLoad(bool v)
    {
        // send a message to the engine to indicate load worked or not.
        GrowString js;
        buildJSONStart(js);        

        JSONWalker::addBoolValue(js, IFI_POSTLOAD, v);
        buildJSONEnd();

        // send
        _host->eval(js.start());
    }

    bool ifiSave(const uchar* data, int size, const string& name) override
    {
        // ifiSaveDataResponse is handled in the ifihandler, which
        // calls this to actually perform the save to file.
        // the name is optionally provided in the json.
        
        // any suggested name?
        string f = name;

        // otherwise invent our own
        if (f.empty()) f = _requestSaveFile;

        // ensure we use a save suffix
        f = changeSuffix(f, ".sav");

        //string dataDir = getProp(IFI_DATADIR).toString();
        //string path = makePath(dataDir, changeSuffix(f, ".sav"));

        string path = f;
        LOG1("ifiSave ", path << " size:" << size);
        
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
                LOG1("ifiSave, cannot write to file '", path << "'\n");
            }
        }
        else
        {
            LOG1("ifiSave, can't open save file '", path << "'\n");
        }

        if (r)
        {
            // XX defined in main. This syncs the persistent files
            syncFilesystem();
        }

        emitPostSave(r);

        return r;
    }

    bool ifiLoadData(const string& s) override
    {
        // called directly from requestLoad.
        // we load from file and send it to the back end.
        
        string f = s;

        // otherwise invent our own
        if (f.empty()) f = _requestLoadFile;
        
        // ensure we use a save suffix
        f = changeSuffix(f, ".sav");

        //string dataDir = getProp(IFI_DATADIR).toString();
        //string path = makePath(dataDir, changeSuffix(f, ".sav"));
        string path = f;

        LOG1("Loading ", path);

        FD fd;
        
        bool v = fd.open(path);
        if (v)
        {
            uchar* data = fd.readAll();
            if (data)
            {
                GrowString js;
                buildJSONStart(js);        

                JSONWalker::addStringValue(js, IFI_LOADDATA, (char*)data);
                buildJSONEnd();

                //LOG1("ifiLoadData, ", js.start());

                // send
                _host->eval(js.start());
                _host->syncRelease(); // refresh when not in sync already
                
                delete [] data;
            }
            else
            {
                LOG1("IFI loadGame, error reading '", path << "'");
                v = false;
            }
        }
        else
        {
            LOG2("IFI loadGame, unable to open '", path << "'");
        }

        if (!v)
        {
            // only need to send if failed!
            // if it worked, we have sent the data and that data needs
            // to be restored before we know if it has worked.
            emitPostLoad(v);
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

    bool ifiSoundResponse(const string& js) override
    {
        int chan = 0;
        int dur = -1;
        string fname;

        for (JSONWalker jw(js); jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);
            if (!st) break; // bad json
            
            if (!isObject)
            {
                // throw all the meta data into the handler propset
                var v = jw.collectValue(st);
                if (jw._key == IFI_CHANNEL) chan =  v.toInt();
                else if (jw._key == IFI_DURATION) dur = v.toInt();
                else if (jw._key == IFI_NAME) fname = v.toString();
            }
        }

        LOG1("Got audio ", js << " " << fname);
        if (!fname.empty()) play_audio(fname.c_str());
        return true;
    }

    string getGameTitle()
    {
        // meta title of game for window header
        var v = _props.find(META_PREFIX IFI_TITLE);
        return v ? v.toString() : string();
    }

    bool ifiPictureResponse(const string& js) override
    {
        string fname;

        if (!js.size()) return true; // ignore
        if (js[0] == '{')
        {
            // json
            LOG2("Got picture request ", js);

            for (JSONWalker jw(js); jw.nextKey(); jw.next())
            {
                bool isObject;
                const char* st = jw.checkValue(isObject);
                if (!st) break; // bad json
            
                if (!isObject)
                {
                    // throw all the meta data into the handler propset
                    var v = jw.collectValue(st);
                    //if (jw._key == IFI_CHANNEL) chan =  v.toInt();
                    //else if (jw._key == IFI_DURATION) dur = v.toInt();
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
            LOG1("Got picture name ", fname);
            show_picture(fname);
        }

        return true;
    }

    bool ifiAnimateResponse(const string& js) override
    {
#ifdef USESPINE        
        if (js[0] == '{')
        {
            // json
            LOG1("Got animation request ", js);

            AnimInfo* ai = new AnimInfo();

            for (JSONWalker jw(js); jw.nextKey(); jw.next())
            {
                bool isObject;
                const char* st = jw.checkValue(isObject);
                if (!st) break; // bad json
            
                if (!isObject)
                {
                    // throw all the meta data into the handler propset
                    var v = jw.collectValue(st);
                    if (jw._key == IFI_NAME)
                    {
                        ai->_anim = v.toString();
                        ai->_name = changeSuffix(ai->_anim, 0);
                    }
                    else if (jw._key == IFI_ATLAS) ai->_atlas = v.toString();
                    else if (jw._key == IFI_PLAY) ai->_play = v.toString();
                    else if (jw._key == IFI_LOOP) ai->_loop = v.isTrue();
                    else if (jw._key == IFI_DELAY)
                    {
                        ai->_delay = v.toInt();

                        // even for zero delay, this means append
                        ai->_append = true;
                    }
                    else if (jw._key == IFI_SHOW) ai->_op = AnimState::op_show;
                    else if (jw._key == IFI_HIDE) ai->_op = AnimState::op_hide;
                }
            }

            extern void play_animation(AnimInfo* ai);
            if (ai->validate()) play_animation(ai);  // consumes ai
            else delete ai;
        }
        return true; // handled
#else
        return false;
#endif        
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

#ifdef IFI_HANDLE_CHOICE
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
#endif // IFI_HANDLE_CHOICE    

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
        flush();

        //static int cc; LOG1("pumpfn! ", ++cc);
        
        std::string s = gui_input_pump();
        if (!s.empty())
        {
            bool done = false;

            if (_choice)
            {
                int nc = _choice->size();
                int chosen = 0;
                const char* p = s.c_str();
                while (u_isdigit(*p))
                {
                    chosen = chosen*10 + (*p - '0');
                    ++p;
                }

                if (chosen > 0 && chosen <= nc)
                {
                    //LOG1(">>> pump got choice ", chosen);

                    // have accepted a value
                    ChoiceList::iterator it = _choice->_choices.begin();
                    if (chosen > 1) std::advance(it, chosen-1);

                    ChoiceInfo ci = *it; // copy

                    if (ci._response[0] == '{')
                    {
                        // response is JSON, so just use it.
                
                        LOG4("IFI choice, sending direct response ", ci._response);
                
                        _host->eval(ci._response.c_str());

                        done = true;
                    }
                }
            }

            // clear any choices after input
            delete _choice; _choice = 0;
            
            if (!done)
            {
                GrowString js;
                buildJSONStart(js);
                buildCmdJSON(s.c_str());
                buildJSONEnd();
                
                _host->eval(js.start());
            }
        }
        return true;
    }
};


struct History
{
    typedef std::string string;
    static const int hSize = 10;
    
    string      _h[hSize];
    int         _pos = 0;
    int         _cur = 0;

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
        if (_cur < _pos) return _h[_cur++].c_str();
        return 0;
    }

    void clear()
    {
        _pos = 0;
        _cur = 0;
    }
    
};

struct Transcript
{
    typedef std::string string;

    string      _name;
    size_t      _size;
    int         _error = 0;

    virtual ~Transcript() {}

    virtual bool open(const string& name) = 0;
    virtual void close() = 0;
    virtual bool write(const char* s, size_t sz) = 0;
    virtual bool write(const char c) = 0;
    virtual string getAll() = 0;
    
    bool write(const string& t) { return write(t.c_str(), t.size()); }
    bool write(const char* s) { return write(s, strlen(s)); }
    
};

struct TranscriptFile: public Transcript
{
    FILE*       _fp = 0;

    ~TranscriptFile()
    {
        close();
    }

    bool isOpen() const { return _fp != 0; }

    bool open(const string& name) override
    {
        close();
        _name = name;
        _size = 0;
        _fp = fopen(name.c_str(), "w");
        return _fp != 0;
    }

    void close() override
    {
        if (_fp)
        {
            fclose(_fp);
            _fp = 0;
        }
    }

    bool write(char c) override
    {
        bool r = isOpen() && !_error;
        if (r)
        {
            r = fputc(c, _fp) != EOF;
            if (r) ++_size;
            else _error = 1;
        }
        return r;
    }
    
    bool write(const char* s, size_t sz) override
    {
        bool r = isOpen() && !_error;

        if (r)
        {
            //LOG1("transcript: ", s);
            r = fwrite(s, 1, sz, _fp) == sz;
            if (r) _size += sz;
            else _error = 1;
        }
        return r;
    }

    string getAll() override
    {
        assert(!isOpen());

        string all;
        if (_size)
        {
            FILE* fp = fopen(_name.c_str(), "r");
            if (fp)
            {
                all.resize(_size);
                fread((char*)all.data(), 1, _size, fp);
                fclose(fp);
                
            }
        }
        return all;
    }

    
};

struct StrandCtx
{
    typedef std::string string;
    
    IFI*        ifi = 0;

    const char* configdir = ".";
    const char* datadir = "."; // default;
    const char* story = "story.str"; 

    IFIHost host;
    SDLHandler h;
    IFI::Ctx   ifiCtx;

    char            _guiInputBuf[256];
    bool            _guiInputReady = false;
    string          _cmdLabel; // text to display
    History         _hist;
    ImText          _mainText;
    bool            _loading;

    int             _loadSz;
    char*           _loadData;

    Transcript*     _transcript = 0;
    WordStat        _wstat;

    void resetAll()
    {
        _hist.clear();
        _mainText.clear();

        // drop the client
        if (ifi)
        {
            LOG1("deleting ifi client", "");
            delete ifi;
            ifi = 0;
        }

        h.resetAll();
    }

    static void _fetch_done(const char* name, void* ctx)
    {
        StrandCtx* sctx = (StrandCtx*)ctx;
        sctx->fetchLoaded(name);
    }

    void fetchLoaded(const char* name)
    {
        if (fetcher.ok)
        {
            LOG1("loaded ", name);
            _loadData = fetcher.yield(_loadSz);
        }
        else
        {
            LOG1("FAILED loading ", name);
        }
        _loading = false;
    }

    char* fetchLoad(const char* name, int& sz)
    {
        assert(!_loading);

        while (!fetcher.start(name, _fetch_done, this))
        {
            //LOG1("fetchload waiting pump ", name);
            (ifiCtx._p)(); // pump while waiting to start loading
        }

        _loading = true;
        _loadSz = 0;
        _loadData = 0;

        while (_loading)
        {
            (ifiCtx._p)();
            sz = _loadSz;
        }
        
        return _loadData;
    }

    StrandCtx() : h(&host)
    {
        Logged initLog;
        Logged::_logLevel = 3; // XXX

        using namespace std::placeholders;  
        ifiCtx._p = std::bind(&SDLHandler::pumpfn, &h);
        ifiCtx._loader = std::bind(&StrandCtx::fetchLoad, this, _1, _2);
    }

    const char* getTitle() const { return h._title.c_str(); }

    void setLabel(const string& label)
    {
        _cmdLabel = "\n> ";
        if (!label.empty())
        {
            char first = label[0];
            if (first == '_')
            {
                // debug command, from a click link
                //std::string w = lastWordOf(label);
                //Term* t = Term::find(w);
                //std::strint l = t->textify();

                // XXX
                // ignore special commands
            }
            else
            {
                _cmdLabel += label;
            }
        }
    }
    
    void sendCmd(const char* s, const string* label = 0)
    {
        // command `s' but sometimes we wish to display this instead
        // but a given `label`.
        // For example, choices are entered as numbers, but we want to
        // display the actual choice text.
        
        // mark text as no longer fresh
        _mainText.seenText();
        
        strcpy(_guiInputBuf, s);
        _hist.add(_guiInputBuf);

        // either use the label or the command text
        setLabel(label ? *label : s);
        _guiInputReady = true;
    }

    bool yieldCmd(string& cmd)
    {
        bool r = false;

        // do we have a click command from the text?
        r = !_mainText._clickCommand.empty();

        if (r)
        {
            // yes, in that case send this first
            cmd = _mainText._clickCommand;

            // click command display as command
            setLabel(cmd);

            // taken
            _mainText._clickCommand.clear();
        }
        else if (_guiInputReady)
        {
            // label is already set
            r = true;
            cmd = _guiInputBuf;
            _guiInputReady = false;
        }
        return r;
    }

    bool init(SDLHandler::TextEmitter& e, const char* storyname)
    {
        assert(!ifi);
        
        ifi = IFI::create(&ifiCtx);
        if (!ifi)
        {
            e("Failed to create IFI");
            return false;
        }

        if (storyname && *storyname) story = storyname;
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

    void requestSave(const string& name)
    {
        LOG1("UI requesting save, ", name);

        GrowString js;
        h.buildJSONStart(js);
        JSONWalker::addBoolValue(js, IFI_SAVEDATA, true);
        h.buildJSONEnd();

        // send to back end via host
        host.eval(js.start());
    }

    void requestLoad(const string& name)
    {
        LOG1("UI requesting load, ", name);

        // call our handler directly, this will load and
        // send data to backend
        h.ifiLoadData(name);
        
    }

    void requestSubcommand(const string& term)
    {
        LOG1("UI requesting subcommand, ", term);

        GrowString js;
        h.buildJSONStart(js);
        JSONWalker::addStringValue(js, IFI_SUBCOMMAND, term);
        h.buildJSONEnd();

        // send to back end via host
        host.eval(js.start());
    }

    void enableCommandChoices(bool v)
    {
        LOG1("enable command choices ", v);

        // XX NB: these are terms directly in core.str
        const char* term = v ? "CHOICE_OPT_ON" : "CHOICE_OPT_OFF";
        requestSubcommand(term);
    }

};
