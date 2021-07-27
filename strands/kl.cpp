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
 *  Copyright (c) Strand Games 2020.
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


#include "kl.h"
#include "version.h"
#include "strutils.h"
#include "opt.h"
#include "var.h"


using namespace std;

#ifdef IFI_BUILD

#include "ificlient.h"
#include "ifiglue.h" // include last
#include "ifihandler.h"
#include "strands.h"

struct KLClient: public IFIClient
{
    void coopWork() override;

    ~KLClient()
    {
        LOG3("~KLClient", "");
        ST::Term::clearTerms();
    }

    Ctx*        _ifictx = 0;
};

// so that we can get it from glue and inside strandi and klstrandi
IFIClient* ifi;
IFIHandler* ifih;
bool cmdWaiting;

// our reference to ifi
#define KIFI  ((KLClient*)ifi)

#endif // IFI_BUILD

#include "pstrands.h"
#include "strandi.h"

#undef TAG
#define TAG  "Strands, "

struct OStream: public StdStream
{
    // compliance
    bool _emit(StdStreamBuf* buf) override
    {
        putstring(*buf);
        return true;
    }

    // hooked by ifiglue
    static void putstring(const char* s)
    {
        while (*s) { putchar(*s); ++s; }
        putchar(EOF); // flush, do not use newline!
    }
};

struct CapStdStream: public StdStream
{
    string _buf;
    bool _emit(StdStreamBuf* buf) override
    {
        _buf += (const char*)*buf;
        return true;
    }
};

struct InStream: public Stream
{
    // Compliance
    int __get() override
    {
        return getchar();
    }

    const char* name() const override { return "input"; }

};

#ifdef IFI_BUILD
struct KLHandler: public IFIHandler
{
    // this is the client
    
    typedef IFIHandler  parentT;
    using Strandi = ST::Strandi;
    using ParseStrands = ST::ParseStrands;
    
    KL*                 _kl = 0;
    ParseStrands        _ps;
    string              _configDir;
    string              _dataDir;
    Strandi             _strandi;
    bool                _gameReady = false;
    string              _story;
    
    KLHandler()
    {
        ifih = this;
        _startDone = true;
    }

    ST::Capture* evalKL(const string& buf, ST::Capture* args)
    {
        //LOG1(TAG "eval KL ", buf);
            
        assert(_kl);
        int cc = 0;
            
        ST::Capture* r = new ST::Capture;

        if (args)
        {

            List arglist;
            List::iterator it(arglist);
            LValue lv(it);
            for (auto& e : args->_elts)
            {
                if (e._v)
                {
                    lv = *KL::varToTerm(e._v);
                    ++cc;
                }
                else if (e._term)
                {
                    ParseContext ctx(_kl);
                    Term t = Symbol(e._term->_name.c_str()).find(&ctx);
                    if (t)
                    {
                        lv = *t;
                        ++cc;
                    }
                }
                else
                {
                    // append string if valid
                    if (e._s.size())
                    {
                        lv = Stringt(e._s);
                        ++cc;
                    }
                }
            }

            if (cc)
            {
                ParseContext ctx(_kl);
                Term s = Symbol("IT").intern(&ctx);

                //Tree e;
                //List et(e, _kl->_env._env);
                //Env env(et, _kl->_env._args);
                _kl->_bindvar(s, &arglist, _kl->_env, true, false);
            }
        }

        CapStdStream tout;
        StdStream* old = _kl->setOutput(&tout);
        _kl->loadString(buf.c_str(), _kl->_env); // eval!
        _kl->setOutput(old);
        r->add(tout._buf); // ignore if empty

        //LOG1(TAG "eval KL result ", tout._buf);

#if 0            
        if (v)
        {
            LOG1(TAG "evalKL result ", v->toString());
                     
            // return values are ignored unless a string
            if (v.isString())
            {
                // avoid quotes in standard KL stringification
                r->add(v.asString().c_str());
            }
        }
#endif
        
        if (r->empty())
        {
            delete r;
            r = 0;
        }
        
        return r;
    }

    bool ifiConfigDir(const string& s) override
    {
        LOG3(TAG "configdir ", s);
        _configDir = s;
        _kl->_loadFilePrefix = s;
        _ps._loadFilePrefix = s;
        return true;
    }
    
    bool ifiDataDir(const string& s) override
    {
        LOG3(TAG "datadir",  s);
        _dataDir = s;
        return true;
    }

    void addKLSymbols(ST::Term::Terms& tl)
    {
        for (auto t : tl)
        {
            ParseContext ctx(_kl);
            Symbol(t->_name.c_str()).intern(&ctx);            
        }
    }

    bool ifiStory(const string& story) override
    {
        _story = story;

        // ensure suffix is .stz or .str
        string sfile = story;
        if (suffixOf(sfile) != ".stz")
            sfile = changeSuffix(_story, ".str");
    
        LOG3(TAG "story ", sfile);
        std::vector<string> fv = { sfile };

        bool loaded = _ps.loadFiles(fv, KIFI->_ifictx);

        if (loaded)
        {
            LOG3(TAG "loading story ", sfile);
            if (_ps.validate())
            {
                if (_ps._startTerm)
                {
                    _strandi.setTerms(&ST::Term::_allTerms);
                    //si.setdebug(debug);
                    
                    using namespace std::placeholders;
                    _strandi.setEvaluator(std::bind(&KLHandler::evalKL, this, _1, _2));

                    addKLSymbols(ST::Term::_allTerms);

                    _strandi.prepare();
                    _gameReady = _strandi._prepared;

                    if (_gameReady)
                    {
                        LOG2(TAG "story ready ", sfile);
                    }
                    else
                    {
                        LOG1(TAG "story did not prepare ", sfile);
                    }
                }
            }
            else
            {
                LOG1(TAG "Story does not validate", sfile);
            }
        }
        else
        {
            LOG1(TAG "cannot load story ", sfile);
        }
        return true;
    }

    bool ifiRandomSeed(int64 v) override
    {
        LOG3(TAG "randomseed ", v);
        _strandi.randomise(v);
        return true;
    }

    bool ifiCommand(const string& cmd) override
    {
        LOG4(TAG "ifiCommand ", cmd);
        cmdWaiting = true;
        return true;
    }

    bool ifiSubcommand(const string& cmd) override
    {
        LOG3(TAG "ifi SUB command ", cmd);
        _strandi.runTerm(cmd);
        return true;
    }

    string collectMetaTag(const char* tag)
    {
        string t = "GAME_";
        t += toUpper(tag);
        return _strandi.runSingleTerm(t);
    }
    
    void collectMeta(VarSet& vs, const char* tag)
    {
        string v = collectMetaTag(tag);
        if (v.size()) vs[tag] = v;  // add or override
    }

    void emitMetaObjects()
    {
        GrowString gs;
        gs.add('{');
        JSONWalker::addKey(gs, IFI_OBJECTS);
        gs.add('[');
        int id = 0;
        for (auto t : *_strandi._terms)
        {
            if (t->isObject() && _strandi.isPhysical(t))
            {
                t->_uiID = ++id;  // bind id
                string pw = t->primaryWord();
                if (pw.size())
                {
                    JSONWalker::toAdd(gs);
                    gs.add('{');
                    JSONWalker::addKeyValue(gs, IFI_ID, t->_uiID);
                    JSONWalker::addKeyValue(gs, IFI_NAME, pw);
                    gs.add('}');
                }
            }
        }
        gs.add(']');
        JSONWalker::addStringValue(gs, IFI_USE, "x {1:name}");
        JSONWalker::addStringValue(gs, IFI_COMPASSGO, "go {1:name}");
        gs.add('}');
        gs.add(0);
        LOG3(TAG "emit meta objects ", gs.start());
        ifi->emitResponse(gs.start());
    }

    bool ifiMeta(bool v) override
    {
        bool r = _gameReady;
        if (r)
        {
            LOG3(TAG "sending meta data ", _story);

            VarSet vs;
            vs.add(IFI_UI_SIDEBAR, true);
            vs.add(IFI_UI_TEXTINPUT, true);
            //vs.add(IFI_AUTOLINK, true);
            //vs.add(IFI_UI_UCANVAS, false);
            //vs.add(IFI_UI_COMPASS, false);

            collectMeta(vs, IFI_TITLE);
            collectMeta(vs, IFI_AUTHOR);
            collectMeta(vs, IFI_ORGANISATION);
            collectMeta(vs, IFI_BACKIMAGE);
            collectMeta(vs, IFI_COVERTEXT);
            collectMeta(vs, IFI_CREDITS);

            // get any meta extra
            string mx = collectMetaTag(IFI_META);
            if (mx.size() && mx[0] == ':')
            {
                const char* ss = mx.c_str() + 1;
                VarSet* vs1 = ParseStrands::parseAttributes(0, &ss);
                if (vs1)
                {
                    for (auto& vi : *vs1) vs[vi.first] = vi.second;
                    delete vs1;
                }
            }
            
            GrowString gs;
            gs.add('{');
            JSONWalker::addKey(gs, IFI_META);
            gs.add('{');

            for (auto& ai : vs)
            {
                JSONWalker::toAdd(gs);
                const char* ss = ai.second.rawString();
                if (ss && ss[0] == ':') // object?
                {
                    ++ss;
                    VarSet* vs1 = ParseStrands::parseAttributes(0, &ss);
                    if (vs1)
                    {
                        JSONWalker::addKey(gs, ai.first.c_str());
                        gs.add('{');
                        for (auto& vi : *vs1)
                            JSONWalker::addKeyValue(gs, vi.first, vi.second);

                        gs.add('}');
                        delete vs1;
                    }   
                }
                else
                {
                    JSONWalker::addKeyValue(gs, ai.first, ai.second);
                }
            }

            gs.add('}');
            gs.add('}');
            gs.add(0);
            LOG3(TAG "emit meta json ", gs.start());
            ifi->emitResponse(gs.start());

            // emit the object table
            emitMetaObjects();
        }
        return r;
    }

    bool ifiBeginGame() override
    {
        LOG2(TAG "received begin signal, ", _story);

        if (_gameReady)
        {
            if (!_strandi.start(_ps._startTerm))
            {
                LOG1(TAG "cannot start, ", _story);
            }
        }

        LOG2(TAG "game finished, ", _story);
        
        return true;
    }

    void ifiDefault(const string& key, const var& v) override
    {
        LOG3(TAG "unhandled ", key << " " << v.toString(true));
    }
    
};


IFI* IFI::create(IFI::Ctx* ctx)
{
    LOG3(TAG, "Creating ifi client");

    ifi = new KLClient();

    if (ctx)
    {
        if (ctx->_p)
        {
            // if we want to run in coop mode
            ifi->_coop = true;
            ifi->_pump = ctx->_p;
        }

        KIFI->_ifictx = ctx;
    }

    return ifi;
}

int IFIClient::client_main(int argc, char** argv)
{
  // arrive here, then go to the original main, which is renamed to ifi_main
  // to avoid conflict with ::main in the front end.
  return ::main(argc, argv);
}

void ifiSend(const char* json)
{
    assert(ifi);
    LOG2("IFISend ", json);
    ifi->emitResponse(json);
}

#endif // IFI_BUILD


static void banner()
{
    // otherwise we emit text too early
#ifndef IFI_BUILD
    OStream::putstring("\nKL Version " VERSION ", " BUILD_VER "\n");
    OStream::putstring("Copyright © Strand Games, 2016-21\n\n");
#endif
}


static void prompt(KL& kl)
{
#ifndef IFI_BUILD        
    *kl._out << "> ";
    kl._out->flush();
#endif        
}

struct KLI
{
    KLHandler   _ifiHandler;
    KL          _kl;
    InStream    _ins;
    OStream     _outs;

    KLI()
    {
        _kl.setInput(&_ins);
        _kl.setOutput(&_outs);

        extern void initKLStrandi(KL* host, ST::Strandi* s);
        initKLStrandi(&_kl, &_ifiHandler._strandi);
    }

    bool loadFile(const string& f) { return _kl.loadFile(f, _kl._env); }
    void put(char c)
    {
        if (c) _outs.put(c);
        else _outs.flush();
    }
};

static KLI* kli;

static bool interactive(KLI* kli)
{
    // drop into interactive 

    bool more = true;
    
    if (!ifi->_coop) prompt(kli->_kl);

#ifdef IFI_BUILD
    const char* r = ifi->getRequest();
    if (r)
    {
        LOG3("KL interative, request: ", r);
        kli->_ifiHandler.handle(r);
    }
    else more = false; // shutdown
#else
        
    SBuf sb;
    if (!kli->_kl._in->getline(sb))
    {
        more = false; // EOF or shutdown
    }
    else
    {
        const char* line = sb;
        if (*line) handleCommand(kli->_kl, line);
    }
#endif
    return more;
}


void KLClient::coopWork()
{
    //LOG3("KLCLient, coopwork", "");

    // process all pending input
    while (interactive(kli)) ;
};

int main(int argc, char** argv)
{
    banner();

    Logged initLog;

    const char* initFilename = "init";
    const char* startjson = 0;

    bool ok = true;

    char** args = argv + 1;
    for (char**& ap = args; *ap; ++ap)
    {
        char* arg;

        if ((arg = Opt::nextOptArg(ap, "-i")) != 0) initFilename = arg;
        else if ((arg = Opt::nextOptArg(ap, "-e")) != 0) startjson = arg;
        else if ((arg = Opt::nextOptArg(ap, "-d")) != 0)
        {
            // normally not processed here, but needed for non-IFI build.
            int v = atoi(arg);
            Logged::_logLevel = v;
            LOG2("KL log level ", v);
        }
        else if (**ap && **ap == '-')
        {
            LOG("unrecognised option '", *ap << "'");
            ok = false;
        }
    }

    Opt::rebuildArgs(argc, argv);
     
    if (!ok)
    {
        cout << "Usage: " << argv[0] << " [-d <n>] [-e json] [-i <initfile>] <kl-file> ...\n";
        return -1;
    }

    string initFile;

#ifdef IFI_BUILD
    /*
     * we have to load the initfile before any other processing.
     * The start json may specify the path prefix, but we are not ready
     * to call the handler yet.
     * So we must manually find any path prefix from the json and
     * use it to locate the init file.
     *
     * Once loaded, we are ok to process handler json as normal.
     */
    if (startjson)
    {
        // assume, if we have start json, we are running a GUI.
        for (JSONWalker jw(startjson); jw.nextKey(); jw.next())
        {
            bool isObject;
            var v = jw.getValue(isObject);
            if (v && jw._key == IFI_CONFIGDIR)
            {
                initFile = v.toString();
                initFile += '/';
            }
        }
    }
#endif // IFI_BUILD    

    // form path of init file
    initFile += initFilename;

    if (kli) delete kli;  // only happens when we reload
    kli = new KLI();

    // automatically load initfile, if present
    kli->loadFile(initFile);

#ifdef IFI_BUILD

    using namespace std::placeholders;
    ST::Strandi& si = kli->_ifiHandler._strandi;
    si.setEmitter(std::bind(&KLI::put, kli, _1));
    
    // pass start json to the handler *after* init file has loaded.
    kli->_ifiHandler._kl = &kli->_kl;
    kli->_ifiHandler.handle(startjson);
#endif    
    
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] != '-') kli->loadFile(argv[i]);
    }
    
    //LOG2("Total Objects: ", Head::_count);

#ifdef IFI_BUILD
    assert(ifi);

    // in coop mode, do not drop into interactive
    if (ifi->_coop) return 0;
#endif   
    
    while (interactive(kli)) ;

    return 0;
}



