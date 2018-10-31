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

#include <math.h>
#include <future>
#include <mutex>
#include "obj.h"
#include "imptypes.h"
#include "strutils.h"
#include "control.h"
#include "logged.h"
#include "transcripti.h"
#include "consolei.h"
#include "mapboxi.h"
#include "fdz.h"
#include "ap.h"
#include "varset.h"
#include "wordstat.h"
#include "opt.h"
#include "ifi.h"
#include "ifihost.h"
#include "ifroster.h"
#include "ifproduct.h"
#include "autolink.h"


// 1.1.X pre ifi
// 1.2.X post ifi
#define VERSION_STRING  "1.2.7"

struct ControlImpBase
{
    // common base for both IFI and non-IFI

    // so that we can combine all text, eg for recording
    virtual void allEmitterHandler(char c, int type) = 0;
};

struct ImpIFI: public IFIHandler, public ControlImpBase
{
    typedef std::string string;

    IFI*        _ifi = 0;
    IFIHost     _ifiHost;
    Objects     _objects;
    string      _objectsJSON;  // last version
    MapInfo     _mapinfo;
    int         _moveCount = 0;
    bool        _lastSaveOK = false;
    string      _saveFilename;
    string      _lastSaveFilename;
    bool        _coverPageClosed = false;

    // data for the sidebar
    string              _itemsRoster;
    AP<JSON>            _itemsRosterJSON;

    string              _peopleRoster;
    AP<JSON>            _peopleRosterJSON;

    Autolink            _autolink;
    WordStat*           _wordStat = 0;
    
    // main header
    static const int ifi_magic = 0x31494649; // IFI1
    static const int ifi_version_major = 1;
    static const int ifi_version_minor = 0;

    static const int ifi_words_magic = 0x33494649; // IFI3
    static const int ifi_words_version_major = 1;
    static const int ifi_words_version_minor = 0;
    
    // data element
    static const int ifi_data_magic = 0x32494649; // IFI2
    static const int ifi_data_version_major = 1;
    static const int ifi_data_version_minor = 0;


    ImpIFI()
    {
        _ifiHost.setHandler(this);
    }

    ~ImpIFI() { delete _ifi; }
    
    bool loadIFI()
    {
        _ifi = IFI::create();
        if (_ifi)
        {
            _ifiHost.setIFI(_ifi);
            
            // set the emitter 
            _ifi->setEmitter(&IFIHost::emitter, &_ifiHost);
        }
        else
        {
            LOG("Control, ", " failed to load IFI");
        }
        
        return _ifi;
    }

    const Object* findObject(const string& id) const
    {
        const Object* r = _objects.find(id);

        if (!r)
        {
            LOG3("API, findObject, not found '", id << '\'');
        }

        return r;
    }

    void evalObjectsJSON(const string& js)
    {
        if (js != _objectsJSON)
        {
            JSON* json = JSON::fromString(js.c_str());

            if (json && !json->IsArray())
            {
                LOG1("objects json expects array ", js);
                delete json;
                json = 0;
            }
            
            if (json)
            {
                _objectsJSON = js;

                // purge original objects table
                _objects.clear();
                
                size_t sz = json->Size();
                for (size_t i = 0; i < sz; ++i)
                {
                    const JSON::Value& ji = (*json)[i];
                    string id = JSON::toString(JSON::at(ji, IFI_ID));
                    if (!id.empty())
                    {
                        Object obj(id);
                        obj._name = JSON::toString(JSON::at(ji, IFI_NAME));
                        obj._icon = JSON::toString(JSON::at(ji, IFI_ICON));
                        if (obj.valid()) _objects.insert(obj);
                    }
                }

                LOG3("objects table: ", _objects);
            }
            else
            {
                LOG1("malformed object JSON ", js)
            }
        }
    }

    static bool allowEcho(const string& cmd)
    {
        // try to determine whether we should echo this command
        bool r = !startsWith(cmd, "use");
        if (r)
        {
            // letters that occur in normal sentences.
            static const char* allow = "!\"$%&-'?., ";
            const char* p = cmd.c_str();
            while (*p)
            {
                if (!u_isalnum(*p) && !strchr(allow, *p))
                {
                    r = false;
                    break;
                }
                ++p;
            }
        }
        return r;
    }
    
    bool evalCommand(const string& cmd, const ObjectList& ctx)
    {
        // IFI
        bool r = false;
        GrowString js;
        buildJSONStart(js);
        
        string scmd = Autolink::applySubs(cmd, ctx);

        bool echo = ctx._echo && allowEcho(scmd);

        if (echo)
        {
            const char* p = scmd.c_str();
            while (*p)
            {
                // XX need to feed input also to common text channel
                allEmitterHandler(*p, 2);
                ++p;
            }
            allEmitterHandler(0, 2); // flush
        }
        
        if (buildCmdJSON(scmd.c_str()))
        {
            buildJSONEnd();
            r = _ifiHost.eval(js.start());
        }
        return r;
    }

    bool saveGame(const string& filename)
    {
        GrowString js;
        buildJSONStart(js);        

        JSONWalker::addBoolValue(js, IFI_SAVEDATA, true);
        buildJSONEnd();

        _lastSaveOK = false;

        /* not relative to `dataDir` at this stage.
         * path will be added when "savedata" replied
         */
        _saveFilename = filename;

        bool r = _ifiHost.eval(js.start());

        // process the reply immediately
        // the save will happen here when the data is received
        if (r) _ifiHost.syncRelease();

        // dont use this name for saves initiated from back-end
        _saveFilename.clear();

        // did we save?
        return _lastSaveOK;
    }

    bool loadGame(const string& filename)
    {
        // called from Control::loadGame
        // and from ifiLoadData (initiated from engine).
        bool r = false;

        FD fd;
        if (fd.open(filename.c_str()))
        {
            FDBuf fdb(fd);
            FDBuf::Elt elt;

            // load main head element
            FDBuf::Elt eltHead(ifi_magic,
                               ifi_version_major, ifi_version_minor,
                               filename);
            
            fdb.readEltBegin(elt);
            if (!fdb.readEltCheck(elt, eltHead)) return false;
            
            // string version of library when saved
            string libc = fdb.getstring();
            LOG3("Loading save from code version ", libc);
            if (!fdb.readEltEnd(elt)) return false;

            bool dataloaded = false;

            for (;;)
            {
                if (!fdb.readEltBegin(elt)) break;
                
                LOG3("read elt ", std::hex << elt._id << std::dec);

                switch (elt._id)
                {
                case ifi_data_magic:
                    {
                        // load data element
                        FDBuf::Elt eltData(ifi_data_magic,
                                           ifi_data_version_major,
                                           ifi_data_version_minor,
                                           filename);
                        
                        if (!fdb.readEltCheck(elt, eltData)) return false;
                        
                        uchar* data = readz(fdb);
                        if (data)
                        {
                            GrowString js;
                            buildJSONStart(js);        

                            JSONWalker::addRawStringValue(js, IFI_LOADDATA, (char*)data);
                            buildJSONEnd();

                            LOG2("IFI loadGame ", filename);

                            // issue data to game
                            // NOTE that we can be here from inside `ifiLoadData`
                            // which is inside drainQueue from a response.
                            // this means we are already in sync.
                            r = _ifiHost.eval(js.start());
                            if (r) _ifiHost.syncRelease(); // only if not insync
                
                            delete [] data;
                            
                            dataloaded = r;
                        }
                        else
                        {
                            LOG1("IFI loadGame, error reading '", filename << "'");
                        }
                    }
                    break;
                case ifi_words_magic:
                    {
                        // load words element
                        FDBuf::Elt eltWords(ifi_words_magic,
                                            ifi_words_version_major,
                                            ifi_words_version_minor,
                                            filename);
                        
                        if (!fdb.readEltCheck(elt, eltWords)) return false;
                        uchar* data = readz(fdb);
                        if (data)
                        {
                            // assume data is JSON
                            JSON* xjs = JSON::fromString((const char*)data);
                            if (xjs)
                            {
                                _wordStat->restoreWords(*xjs);
                                delete xjs;
                            }

                            delete [] data;
                        }
                    }
                    break;
                default:
                    LOG3("Unknown element ", std::hex << elt._id << std::dec << " in " << filename);
                    break;
                }

                fdb.readEltEnd(elt); // next
            }

            if (r && !dataloaded)
            {
                LOG1("IFI file missing save data ", filename);
                r = false;
            }
        }
        else
        {
            LOG1("IFI loadGame, unable to open '", filename << "'");
        }

        return r;
    }

    bool refreshItemsRoster()
    {
        bool ok = false;
        
        _itemsRosterJSON = JSON::fromString(_itemsRoster.c_str());
        if (_itemsRosterJSON) ok = true;
        else
        {
            LOG1("refreshItemsRoster, bad json '", _itemsRoster << '\'');
        }
        return ok;
    }

    bool refreshPeopleRoster()
    {
        bool ok = false;
        
        _peopleRosterJSON = JSON::fromString(_peopleRoster.c_str());
        if (_peopleRosterJSON) ok = true;
        else
        {
            LOG1("refreshPeopleRoster, bad json '", _peopleRoster << '\'');
        }
        return ok;
    }
    
};
 
struct ImpEngine
{
    typedef std::string string;
    typedef IFEngineInterface::CommandResultI CommandResultI;

    IFEngineInterface*  _be = 0;

    // extra data to be saved with the save game
    GrowString          _saveExtra;
    WordStat*           _wordStat;

    // data for the sidebar
    RosterInfo          _rosterInfo;
    AP<JSON>            _rosterJSON;

    void updateLogLevel()
    {
        // pass on current log level
        if (_be)
        {
            VarSet vs;
            vs[BRA_OPT_LOGLEVEL] = Logged::_logLevel;
            _be->setOptions(vs);
        }
    }

    bool loadEngine(IFEngineInterface* be)
    {
        _be = be;

        // provide callback to engine to request additional save data
        _be->setRequestSaveExtra(_saveWordsRequest, this);
        _be->setRequestLoadExtra(_loadWordsRequest, this);
        
        // pass on current log level
        updateLogLevel();
        return true;
    }

    bool sendOptionToEngine(const string& opt, const var& val)
    {
        // send a single option to the engine
        bool res = _be != 0;
        if (res)
        {
            VarSet vs;
            vs[opt] = val;
            res = _be->setOptions(vs);
        }
        return res;
    }

    static bool _saveWordsRequest(void* ctx, const uchar** data, size_t* amt)
    {
        ImpEngine* self = (ImpEngine*)ctx;
        return self->saveWordsRequest(data, amt);
    }

    static bool _loadWordsRequest(void* ctx, const uchar** data, size_t* amt)
    {
        ImpEngine* self = (ImpEngine*)ctx; 
        return self->loadWordsRequest(*data, *amt);
    }

    bool loadWordsRequest(const uchar* data, size_t size)
    {
        bool r = data && size > 0;
        if (r)
        {
            if (data[0] == '{')
            {
                // assume data is JSON
                JSON* xjs = JSON::fromString((const char*)data);
                if (xjs)
                {
                    _wordStat->restoreWords(*xjs);
                    delete xjs;
                }
                else
                {
                    LOG1("control, load data size ", size << " invalid JSON " << (const char*)data);
                }
            }
            else
            {
                LOG1("control, unknown load data, size: ", size);
            }
        }
        return r;
    }

    bool saveWordsRequest(const uchar** data, size_t* amt)
    {
        // called via the engine. game thread?
        // to request additional save game data (if any).

        if (!data || !amt) return false;

        LOG4("control, ", "updateWordsRequest")

        GrowString gs;
        _wordStat->buildWordsJSON(gs);

        _saveExtra = gs;  // donate semantics

        //LOG3("words ", _saveExtra);

        *data = (const uchar*)_saveExtra.start();
        *amt = _saveExtra.size();
        
        return true;
    }

    // a story can have "story.json" which contains extra data for the UI
    // For example, items and their icons.
    struct ItemInfo
    {
        // additional UI info for items in games
        int             _id;
        string          _iconFile;

        ItemInfo() { _id = 0; }
        ItemInfo(int id) : _id(id) {}

        bool operator==(const ItemInfo& ii) const { return _id == ii._id; }
        bool operator<(const ItemInfo& ii) const { return _id < ii._id; }

        friend std::ostream& operator<<(std::ostream& os, const ItemInfo& ii)
        {
            os << "item " << ii._id;
            if (!ii._iconFile.empty()) os << " icon:" << ii._iconFile;
            return os;
        }
    };

    typedef std::set<ItemInfo>  ItemsInfo;
    ItemsInfo           _itemsInfo;

    const ItemInfo* findItemInfo(int id) const
    {
        ItemInfo t(id);
        ItemsInfo::const_iterator it = _itemsInfo.find(t);
        if (it != _itemsInfo.end()) return &*it;
        return 0;
    }

    void buildItemsInfo(JSON* sj)
    {
        assert(sj);

        JSON::ConstMemberIterator mi = sj->MemberBegin();
        while (mi != sj->MemberEnd())
        {
            const char* name = mi->name.GetString();

            // are we item data?
            if (startsWith(name, "item")) 
            {
                int id = atoi(name + 4);
                
                ItemInfo ii(id);
                
                JSON& item = (JSON&)mi->value;
                ii._iconFile =
                    (const char*)JSON::StringValue(item, BRA_STORY_JSON_ICON);

                //LOG3("json ", ii);

                _itemsInfo.insert(ii);
            }
            ++mi;
        }
    }

    bool refreshRoster()
    {
        // engine interface uses a single rosterinfo
        
        bool changed = false;
        
        if (_rosterInfo._changed)
        {
            _rosterInfo._changed = false;
            
            //LOG3("refresh roster, ", _rosterInfo._json);
            _rosterJSON = JSON::fromString(_rosterInfo._json.c_str());
            if (_rosterJSON)
            {
                changed = true;
            }
            else
            {
                LOG1("refreshRoster, bad json '", _rosterInfo._json << '\'');
            }
        }
        return changed;
    }

};

struct Control::Imp :
    public ImpType<Control>,
    public ImpEngine,
    public ImpIFI,
    public Opt
{
    typedef std::string string;
    
    Imp(Control* host) : ImpType(host) {} // dont call init here

    virtual ~Imp() {}

    struct EmitterBase
    {
        enum Type
        {
            emit_void = 0,
            emit_transcript,
            emit_console,
            emit_segment,
        };

        Type _type;

        EmitterBase() { _type = emit_transcript; }

        static void emit(void* ctx, char c) { ((EmitterBase*)ctx)->call(c); }
        
        virtual ~EmitterBase() {}
        virtual void call(char c) = 0;
    };

    template<class B> struct Emitter: public EmitterBase
    {
        typedef void (B::*F)(char);

        Imp*            _host;
        F               _fn;
        B*              _base;

        Emitter() : _host(0), _fn(0), _base(0) {}
        Emitter(Imp* host, B* b, F f) : _host(host), _fn(f), _base(b) {}

        void send(char c) { (_base->*_fn)(c); }
        void call(char c) override
        {
            send(c);
            _host->allEmitterHandler(c, _type);
        }

        void calls(const char* s) { while (*s) call(*s++); }
    };

    typedef Emitter<Transcript::Imp> TEmit;
    typedef Emitter<Console::Imp> CEmit;

    int                 _userSpaceID;
    
    TEmit               _temit;
    CEmit               _cemit;
    TEmit               _semit;
    string              _recordFilename;
    FD                  _recordFile;
    FDBuf               _recordFileBuf;
    int                 _recordChannel;
    int                 _emitType;
    bool                _typeChanged;
    int                 _recordMetaState; // tracks [hello](world)
    int                 _blevel;
    int                 _recordLast;

    // write console commands also to main text window
    bool                _echoConsoleToTranscript;

    // chars echoed from console to transcript need buffering
    GrowString          _cemitBuf;

    
    WordStat            _wordStat;
    string              _titleText;

    // if set directly, otherwise we infer from location
    bool                _titleTextSet = false;

    
    void _init()
    {
        _userSpaceID = 1; // dummy
        _recordChannel = 0;
        _emitType = 0;
        _typeChanged = false;
        _recordMetaState = 0;
        _blevel = 0;
        _recordLast = 0;
        _echoConsoleToTranscript = false;

        ImpEngine::_wordStat = &_wordStat;
        ImpIFI::_wordStat = &_wordStat;
    }

    void updateEchoConsoleMode()
    {
        bool v = (_host->_prefs->getInt(PREFS_CONSOLEECHO_ENABLED,
                                      DEFAULT_CONSOLEECHO_ENABLE) != 0);
        _echoConsoleToTranscript = v;
    }

    bool startRecording(const string& filename)
    {
        bool res;
        _recordFilename = filename;
        _recordChannel = 0;
        _typeChanged = false;
        _recordMetaState = 0;
        _recordLast = 0;
        _blevel = 0;
        
        res = _recordFile.open(_recordFilename.c_str(),
                               FD::fd_write | FD::fd_create | FD::fd_trunc);
        if (res)
        {
            LOG3("recording transcript file '", _recordFilename << "'");
            _recordFileBuf.fd(_recordFile);
        }
        else
        {
            LOG("Error: Can't open transcript file '", _recordFilename << "'");
        }
        return res;
    }

    std::string formatStyle(const char* text, const char* style)
    {
        string s = "\n\n#[";
        s += trim(text); // no space on ends please.
        s += "](";
        s += style;
        s += ")\n\n";
        return s;
    }

    void allEmitterHandler(char c, int type) override
    {
        // all output is also copied here.
        bool recording = _recordFileBuf.isOpen();
        
        if (_emitType != type)
        {
            _emitType = type;                
            _typeChanged = true;
        }

        if (type == EmitterBase::emit_segment) _recordChannel = (int)c;
        else
        {
            bool ok = _recordChannel < BRA_SEGMENT_NULL; // main window

            if (ok)
            {
                bool flush = false;
                if (_typeChanged && recording)
                {
                    if (_recordFileBuf.lastChar() != '\n')
                    {
                        _recordFileBuf.putc('\n');
                        flush = true;
                    }
                }
                
                _typeChanged = false;

                bool emit = true;

                // deal with "bla bla [hello](world) bla
                // emit hello, but not world and all before and after
                switch (_recordMetaState)
                {
                case 0:
                    if (c == '[') 
                    {
                        _recordMetaState = 1;
                        emit = false;
                    }
                    break;
                case 1:
                    if (c == ']') 
                    {
                        _recordMetaState = 2;
                        _blevel = 0;
                        emit = false;
                    }
                    break;
                case 2:
                    emit = false;
                    if (_recordLast == ']' && c != '(')
                    {
                        // drop out [foo]...
                        _recordMetaState = 0;
                        emit = true;
                    }
                    else if (c == '(') ++_blevel;
                    else if (c == ')' && !--_blevel) _recordMetaState = 3;
                    break;
                case 3:
                    emit = false;
                    if (_recordLast == ')' && c != '{')
                    {
                        // drop out [foo](bar)...
                        _recordMetaState = 0;
                        emit = true;
                    }
                    else if (c == '}') _recordMetaState = 0;
                    break;
                }

                if (emit)
                {
                    _wordStat.learnWords(c);
                    
                    if (recording)
                    {
                        if (!c) flush = true;
                        else
                        {
                            bool sup = false;
                            
                            if (c == '\n')
                            {
                                flush = true;
                                
                                int ch = _recordFileBuf.lastChar();
                                if (ch == '\n' || !ch) sup = true;
                            }

                            if (!sup)
                                ok = _recordFileBuf.putc(c);
                        }
                    }

                    if (_echoConsoleToTranscript &&
                        type == EmitterBase::emit_console)
                    {
                        _cemitBuf.add(c);
                        if (!c)
                        {
                            // flush to transcript
                            _cemitBuf.prepend("> "); // old school prompt
                            string s = formatStyle(_cemitBuf.start(),
                                                   BRA_CONSOLE_ECHO_STYLE);
                            const char* p = s.c_str();

                            for (;;)
                            {
                                _temit.send(*p); // send 0 as well.
                                if (!*p) break; 
                                ++p;
                            }
                            _cemitBuf.clear();
                        }

                    }
                }

                if (recording)
                {
                    if (ok && flush)
                        ok = _recordFileBuf.flush();

                    if (!ok)
                    {
                        LOG("Error: failed to write to transcript file '", 
                            _recordFilename << "'");
                    }
                }

                _recordLast = c;
            }
        }
    }
    
    void startUp(const char* dpath,
                 const char* configDir,
                 int userSpaceID)
    {
        _userSpaceID = userSpaceID;
        
        //  per user path
        if (dpath && *dpath)
        {
            _host->_dataPath = dpath;
            
            // use linux path style always!
            replaceCharsInplace(_host->_dataPath, '\\', '/');
        }
        else
        {
            LOG("Control, Error ", "missing data path");
        }

        LOG3("data path, '", _host->_dataPath << "'");
        
        if (configDir && *configDir)
        {
            _host->_assetsPath = configDir;
            replaceCharsInplace(_host->_assetsPath, '\\', '/');
        }
        else
        {
            LOG("Control Error ", "missing assets path");
        }
            
        LOG3("assets path, '", _host->_assetsPath << '"');
        
        _host->_prefs = Prefs::create(_host);
        _host->_transcript = Transcript::create(_host);
        _host->_map = Map::create(_host);
        _host->_itemsModel = ItemsModel::create(_host);

        _host->_console = Console::create(_host);

        // NB: `_be` will be NULL for IFI.
        _host->_console->_imp->_be = _be;

    }

    bool autolinkEnabled() const
    {
        return propTrue(IFI_META "/" IFI_AUTOLINK);
    }

    // IFI Overrides

    string applyAutolink(const string& s)
    {
        if (autolinkEnabled())
        {
            //LOG4("API, applying autolink, ", s);

            _autolink._useCmd = getProp(IFI_USE).toString();
            _autolink._goCmd = getProp(IFI_COMPASSGO).toString();
            _autolink._currentExits = _mapinfo._currentExits;
            _autolink._scope = &_objects;
            
            return _autolink.applyItemMarkup(s);
        }
        return s;
    }
    
    bool ifiText(const string& s) override
    {
        //LOG4("API text '", s << '"');
        assert(_ifi);
        
        string t = applyAutolink(s);
        const char* p = t.c_str();
        
        while (*p) _temit.call(*p++);
        _temit.call(0); // flush
        return true;
    }

    bool ifiTextFormatted(const TextF& tf) override
    {
        //LOG3("API textF ", tf);

        char id = tf._id;

        if (id == -1)
        {
            // code to clear text
            id = BRA_SEGMENT_CLEAR;
        }
        else
        {
            // use channel# > those originally defined to have special meaning
            if (id) id += 10;
        }

        if (id) _semit.call(id);
        bool r = ifiText(tf._text);
        if (id) _semit.call(0);

        return r;
    }

    bool ifiTitleTextResponse(const string& s) override
    {
        LOG3("API titletext: ", s);

        _titleTextSet = true; // don't update from location

        // host will call local version and also emit update
        _host->titleText(s);

        return true; // done
    }

    bool ifiMetaResponse(const string& js) override
    {
        LOG3("API Meta response ", js);
        _host->metaChanged(js); // -> Qcontrol

        // continue into meta object to collect properties
        return false; 
    }

    bool ifiSoundResponse(const string& js) override
    {
        LOG3("API sound response ", js);
        _host->soundChanged(js); // -> Qcontrol

        return true; // handled
    }

    bool ifiItemsResponse(const string& js) override
    {
        LOG3("API Items response ", js);

        if (_itemsRoster != js)
        {
            _itemsRoster = js;
            if (refreshItemsRoster()) refreshItemsModel();
        }
        
        return true; // done
    }

    bool ifiPeopleResponse(const string& js) override
    {
        LOG3("API People response ", js);

        if (_peopleRoster != js)
        {
            _peopleRoster = js;
            if (refreshPeopleRoster()) refreshItemsModel();
        }

        return true; // done
    }

    bool ifiObjectsResponse(const string& js) override
    {
        // receive object map
        LOG4("API objects response ", js); // table is printed separately
        evalObjectsJSON(js);
        return true; // done
    }

    bool ifiLocationResponse(const string& id) override
    {
        // Called after JSON `mapresponse` goes into mapobj
        // or from top-level
        LOG3("API location response ", id);
        _mapinfo._currentLocation = id;
        if (updateMapLocation(_mapinfo))
        {
            if (!_titleTextSet)
            {
                // if we're not setting the title text directly
                // use the location (if we have it)
                const Object* o = findObject(id);
                if (o)
                    _host->titleText(CapitaliseStartWords(o->_name));
            }
        }
        return true;
    }

    bool ifiExitsResponse(int mask) override
    {
        LOG3("API exits response ", mask);
        _mapinfo._currentExits = mask;
        updateMapLocation(_mapinfo);        
        return true;
    }

    bool ifiLoadData(const string& s) override
    {
        // expect this to be a response from the engine,
        bool r = false;

        string f = s.empty() ? _lastSaveFilename :
            makeDataPath(changeSuffix(s, ".sav"));

        LOG3("API ifiLoadData ", f);

        if (!f.empty())
            r = ImpIFI::loadGame(f);

        return r;
    }

    bool ifiMapResponse(const string& js) override
    {
        bool changed = js != _mapinfo._json;
        if (changed)
        {
            LOG3("API map response ", js);
            _mapinfo._changed = true;
            _mapinfo._json = js;
        }
        return false; // continue to notify subparts (eg location)
    }

    bool ifiPictureResponse(const string& js) override
    {
        // expect either json or just the path of a picture

        if (!js.empty())
        {
            if (js[0] != '{')
            {
                // expect a simple path
                GrowString gs;
                gs.append("{\"" IFI_NAME "\":\"");
                gs.append(js);
                gs.append("\"}");
                gs.add(0);
                LOG3("API picture from path js ", gs.start());
                _host->imageChanged(gs.start());
            }
            else
            {
                LOG3("API picture js ", js);
                _host->imageChanged(js);
            }
            
        }
        return true;
    }

    bool ifiRestartResponse() override
    {
        _wordStat.reset();        
        return true;
    }

    bool ifiMoves(int n) override
    {
        // keep track of the move counter if provided
        _moveCount = n;
        return true;
    }

    string getGameTitle()
    {
        // from IFI meta
        var v = getProp(IFI_META, IFI_TITLE);
        return v ? v.toString() : string();
    }

    string suggestedSaveName()
    {
        static int gen = 0;

        // invent a name for a save.
        string f = getGameTitle();
        if (!f.empty()) f += '_';
        if (!_titleText.empty())
        {
            f += _titleText;
            f += '_';
        }

        if (_moveCount > gen) gen = _moveCount;
        else ++gen;
        
        f += std::to_string(gen);

        // convert all non-alphanumeric to underscore for filename
        GrowString gs;
        const char* p = f.c_str();
        while (*p)
        {
            char c = *p++;
            if (u_isspace(c)) c = '_';
            else
            {
                if (c != '_' && c != '-' && c != '.' && !u_isalnum(c)) 
                    c = 0;
            }
            if (c) gs.add(c);
        }
        gs.add(0);

        return gs.start();
    }

    bool ifiSave(const uchar* data, int size, const string& name) override
    {
        string f;

        if (!_saveFilename.empty())
        {
            // save initiated from UI. this is the filename to use
            f = _saveFilename;
        }
        else
        {
            // any suggested name?
            f = name;
        }

        // otherwise invent our own, but make sure it doesn't exist
        if (f.empty())
        {
            do
            {
                f = suggestedSaveName();
            } while (FD::exists(makeDataPath(changeSuffix(f, ".sav"))));
        }

        string path = makeDataPath(changeSuffix(f, ".sav"));
        
        LOG3("ifiSave ", path);
        
        // remember the last save name, so that "load" without a name
        // can use the previous file
        _lastSaveFilename = path;
        
        FD fd;
        bool r = fd.open(path.c_str(), FD::fd_new);

        if (r)
        {
            FDBuf fdb(fd);

            // create the main header element
            FDBuf::Elt eltHead(ifi_magic,
                               ifi_version_major, ifi_version_minor,
                               path);
            
            r = fdb.writeEltBegin(eltHead);

            // save text version string of library
            fdb.putstring(VERSION_STRING);
            r = r && fdb.writeEltEnd(eltHead);

            if (r)
            {
                // save the words list
                FDBuf::Elt eltWords(ifi_words_magic,
                                   ifi_words_version_major,
                                   ifi_words_version_minor,
                                   path);
                r = fdb.writeEltBegin(eltWords);

                GrowString gs;
                _wordStat.buildWordsJSON(gs);
                
                size_t nwrote;
                r = r && writez(fdb, (unsigned char*)gs.start(),
                                gs.size(), nwrote);
                r = r && fdb.writeEltEnd(eltWords);
            }
            
            if (r)
            {
                // data element
                FDBuf::Elt eltData(ifi_data_magic,
                                   ifi_data_version_major,
                                   ifi_data_version_minor,
                                   path);
                r = fdb.writeEltBegin(eltData);
            
                size_t nwrote;
                r = r && writez(fdb, data, size, nwrote);
                r = r & fdb.writeEltEnd(eltData);
            }

            if (r)
            {
                _lastSaveOK = true;
                LOG3("saved ", path);
            }
        }
        else
        {
            LOG("ifiSave, can't open save file '", path << "'\n");
        }
        
        return r;
    }
    
    /////////////////////////////////

    static string makePath(const string& prefix, const string& name) 
    {
        string path;
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

    string makeConfigPath(const string& name) const
    {
        return makePath(_host->_assetsPath, name);
    }

    string makeDataPath(const string& name) const
    {
        return makePath(_host->_dataPath, name);
    }

    string prefsFilePath() const
    {
        return makeDataPath(PREFS_FILENAME);
    }

    string resolveAsset(const string& path) const
    {
        if (path.empty()) return path; // nothing to resolve
        
        string p = _resolveAsset(path);
        if (p.empty())
        {
            // try with lower case
            p = _resolveAsset(toLower(path));
        }

        if (p.empty())
        {
            LOG1("failed to resolve '", path << "' to either " << makeConfigPath(path) << " or " << makeDataPath(path));
        }
        return p;
    }
  
    string _resolveAsset(const string& path) const
    {
        // returns empty string if does not resolve
        string rp = makeConfigPath(path);
        bool ok = FD::exists(rp);
            
        if (!ok)
        {
            rp = makeDataPath(path);
            ok = FD::exists(rp);
        }

        if (ok)
        {
            // build Qt file URL prefix
            rp = "file:///" + rp;
            LOG4("resolve asset ", path << " -> " << rp);
        }
        else rp.clear();
        return rp;
    }

    void beginGame()
    {
        string storypath;

        if (!_be && !_ifi)
        {
            LOG1("beginGame, ", "NO ENGINE");
            return;
        }

        LOG3("Control beginGame, using ", (_be ? "BE" : "IFI"));

        // get random seed from OS
        int64 seed = makeRandomSeed();

        // and pass it to engine // BE ONLY
        sendOptionToEngine(BRA_OPT_RANDOMSEED, var(seed));
            
        sendOptionToEngine(BRA_OPT_PIXELSCALE,
                           _host->_prefs->getInt(PREFS_IMAGEPIXSCALE_ENABLED,
                                                 DEFAULT_IMAGEPIXSCALE_ENABLE));

        // send initial option set up to engine
        sendOptionToEngine(BRA_OPT_MODERN, 
                           _host->_prefs->getInt(PREFS_MODERN_ENABLED,
                                                 DEFAULT_MODERN_ENABLE));
        
        // retrieve from prefs
        updateEchoConsoleMode();
            
        if (!_host->_storyfile.empty())
        {
            // start game
            storypath = makeConfigPath(_host->_storyfile);

            if (!_recordFilename.empty())
                startRecording(_recordFilename);
            
            // default to same if not set
            if (_host->_musicFile.empty())
                _host->_musicFile = _host->_storyfile;

            if (_be)
            {
                // do we have any story.json
                string sJSONPath = makeConfigPath(changeSuffix(_host->_storyfile, ".json"));

                FD jfile;
                if (jfile.open(sJSONPath.c_str()))
                {
                    LOG2("Found story json, ", sJSONPath);

                    FD::Pos fsize;
                    uchar* jdat = jfile.readAll(&fsize, true);

                    if (jdat)
                    {
                        JSON* sj = JSON::fromString((const char*)jdat);
                        delete [] jdat;
                    
                        if (sj)
                        {
                            buildItemsInfo(sj);
                            delete sj;
                        }
                        else
                        {
                            LOG1("ERROR: story json corrupt ", sJSONPath);
                        }
                    }
                    else
                    {
                        LOG1("ERROR: could not read story json ", sJSONPath);
                    }
                }
                else
                {
                    LOG3("Did not find ", sJSONPath);
                }
            }
        }

        if (!_host->_musicFile.empty())
        {
            // can be a resource
            _host->_musicFile = changeSuffix(_host->_musicFile, ".ogg");
        }
        else
        {
            LOG1("WARNING, no music file", "");
        }

        _temit = TEmit(this, _host->_transcript->_imp,
                       &Transcript::Imp::charEmit);
        _temit._type = EmitterBase::emit_transcript;

        _cemit = CEmit(this, _host->_console->_imp,
                       &Console::Imp::charEmit);
        _cemit._type = EmitterBase::emit_console;
        
        _semit = TEmit(this, _host->_transcript->_imp,
                       &Transcript::Imp::segmentInfo);
        _semit._type = EmitterBase::emit_segment;

        bool r = false;

        assert(_host->_prefs);
        
        if (_ifi)
        {
            // old engine interface pushes lines at a time
            // IFI can push any text, so transcript must update
            // when it sees newlines
            _host->_transcript->_refreshOnNewline = true;
            
            setProp(IFI_CONFIGDIR, _host->_assetsPath);
            setProp(IFI_DATADIR, _host->_dataPath);
            setProp(IFI_STORY, _host->_storyfile);

            // build initial json for startup
            GrowString js;
            buildJSONStart(js);
            buildInitialJSON();
            buildJSONEnd();

            // construct `main` like args for IFI startup.
            // pass in the current log level *and* initial json
            int argc = 1;
            char buf[32];
            sprintf(buf, "%d", Logged::_logLevel);
            char** argv = Opt::createArgs(argc+4, "api");
            Opt::addArg(argc, argv, "-e", js.start());            
            Opt::addArg(argc, argv, "-d", buf);
            r = _ifi->start(argc, argv);

            _ifiHost.syncRelease();
            _startDone = true;
            
            Opt::deleteCopyArgs(argv);

            // send prologue json

            // we guarantee the back-end will receive some prologue json
            // *before* any commands. This allows the back-end to get ready
            // after it has been started, but *also* to allow the back-end to
            // hold off from properly starting until the first eval (if necessary).
            buildJSONStart(js);
            buildPrologueJSON();
            buildJSONEnd();
            _ifiHost.eval(js.start());

            postEval(); // ensure prologue handled
        }
        else
        {
            r = _be->start(_host->_assetsPath.c_str(),
                           _host->_dataPath.c_str(),
                            
                            storypath.c_str(),
                            
                            // transcript emit
                            &EmitterBase::emit,
                            (void*)&_temit,

                            // console emit
                            &EmitterBase::emit,
                            (void*)&_cemit,

                            // segment change
                            &EmitterBase::emit,                            
                            (void*)&_semit
                            );
            if (r)
            {
                // initial update of scene
                postEval();
            }
        }
        
        if (!r)
        {
            if (!storypath.size()) storypath = "undefined";
            LOG("unable to start story, ", storypath);
        }
    }

    void coverPageClosed()
    {
        if (_ifi && !_coverPageClosed)
        {
            // only send begin code once!
            _coverPageClosed = true;
            
            // send additional {begin:true}
            const char* js = "{\"" IFI_BEGIN "\":true}";
            LOG4("Sending ifi begin, ", js);
            if (_ifiHost.eval(js)) _ifiHost.syncRelease();
        }
    }

    string currentVersion() const
    {
        // combine the system version with the engine version
        string v = VERSION_STRING;
        if (_be)
        {
            v += '.';
            v += _be->currentVersion();
        }
        else if (_ifi)
        {
            //v += '.';
        }
        else
        {
            LOG1("Warning: ", "No engine version");
        }
        return v;
    }

    int getLogLevel() const
    {
        // will be the one inside this DLL/shared lib.
        return Logged::_logLevel;
    }
    
    void setLogLevel(int level)
    {
        if (level >= 0 && level < 100)
        {
            Logged::_logLevel = level;
            LOG2("setting log level to ", level);

            // pass on if engine loaded (BE only)
            ImpEngine::updateLogLevel();
        }
    }

    bool evalCommand(const string& cmd, bool echo = true)
    {
        return _evalCommandSpecial(cmd, echo) || evalCommandDirect(cmd, echo);
    }

    bool evalClickCommand(const string& cmd)
    {
        // called when a link in the text is clicked issuing a command
        return evalCommandDirect(cmd, true);
    }

    bool refreshCommand()
    {
        bool r = false;
        if (_be)
        {
            r = evalCommandDirect("look", true);
        }
        if (_ifi)
        {
            string cmd = getProp(IFI_REFRESHCMD).toString();
            if (!cmd.empty())
            {
                ObjectList ctx(_objects); 
                ctx._echo = true;
                
                r = ImpIFI::evalCommand(cmd, ctx);
                if (r) postEval();
            }
        }
        return r;
    }

    bool evalUseXwithY(const string& x, const string& y)
    {
        LOG3("eval use ", x << " with " << y);
        bool r = false;
        if (_be)
        {
            r = _be->evalUseXwithY(x, y);
            if (r) postEval();
        }
        if (_ifi)
        {
            string cmd = getProp(IFI_USEWITH).toString();
            if (!cmd.empty())
            {
                ObjectList ctx(_objects); // no echo
                if (ctx.add(x) && ctx.add(y))
                {
                    r = ImpIFI::evalCommand(cmd, ctx);
                    if (r) postEval();
                }
            }
        }
        return r;
    }

    bool menuDrop(const string& x)
    {
        // drag menu item to blank area
        LOG3("menu drop ", x);
        bool r = false;
        
        if (_be)
        {
            evalCommand("domenu item" + x);
            r = true;
        }
        if (_ifi)
        {
            string cmd = getProp(IFI_UNUSE).toString();
            if (!cmd.empty())
            {
                ObjectList ctx(_objects); // no echo
                if (ctx.add(x))
                {
                    r = ImpIFI::evalCommand(cmd, ctx);
                    if (r) postEval();
                }
            }
        }
        return r;
    }

    bool menuClick(const string& x)
    {
        // click on menu item 
        LOG3("menu click ", x);
        bool r = false;
        
        if (_be)
        {
            evalCommand("do item" + x);
            r = true;
        }
        if (_ifi)
        {
            string cmd = getProp(IFI_USE).toString();
            if (!cmd.empty())
            {
                ObjectList ctx(_objects); // no echo
                if (ctx.add(x))
                {
                    r = ImpIFI::evalCommand(cmd, ctx);
                    if (r) postEval();
                }
            }
        }
        return r;
    }

    bool compassDirection(uint dir)
    {
        // called from UI when click on compass
        bool r = false;

        const char* dname = dirName(dir);

        //LOG3("compass direction ", dname);

        if (_be)
        {
            r = evalCommandDirect(dname, true);
        }
        if (_ifi)
        {
            string cmd = getProp(IFI_COMPASSGO).toString();
            if (!cmd.empty())
            {
                ObjectList ctx(_objects);
                ctx._echo = true;
                if (ctx.add(dname, true)) // force direction as id
                {
                    r = ImpIFI::evalCommand(cmd, ctx);
                    if (r) postEval();
                }
            }
        }
        return r;
    }

    bool postEval()
    {
        bool r = true;
        if (_be)
        {
            // update scene
            _be->emitScene();
        }
        
        if (_ifi)
        {
            r = _ifiHost.syncRelease();
        }
        
        // refresh sidebar & map
        if (r) updateAfterCommand();
        return r;
    }

    bool updateMapInfo(MapInfo& mi)
    {
        // called in light mode every turn (non-ifi)
        // called in full mode by performLayout when we update whole map

        bool r = false;
        if (_be)
        {
            // get state from engine
            r = _be->updateMapInfo(mi);
        }

        if (_ifi)
        {
            r = true;

            // only needs to copy from latest version we've been sent
            // `mapinfo` is maintained by IFI
            // `changed` is set when JSON changes rather than just location
            mi._currentLocation = _mapinfo._currentLocation;
            mi._currentExits = _mapinfo._currentExits;
            mi._changed = _mapinfo._changed;

            if (mi._full && _mapinfo._changed)
            {
                mi._json = _mapinfo._json;

                // reset for next time
                _mapinfo._changed = false;
            }
        }
        return r;
    }

    bool updateMapLocation(MapInfo& mi)
    {
        bool changed = _host->_map->updateMapLocation(mi);

        // triggers update of location and exits
        if (changed && _host->_map->_notifier)
            _host->_map->_notifier->changed();

        return changed;
    }

    void refreshItemsModel()
    {
        ItemsModel* items = _host->_itemsModel.get();
        if (items) items->update();
    }
    
    void updateAfterCommand()
    {
        if (_be)
        {
            // only for BE, as IFI will receive updates when changed
            if (_be->updateRosterInfo(_rosterInfo))
            {
                if (refreshRoster()) refreshItemsModel();
            }

            // update light map data
            MapInfo mi;

            if (updateMapInfo(mi))
            {
                //LOG3("updateMap, current location ", mi._currentLocation);
                //LOG3("updateMap, current exits: ", std::hex << mi._currentExits << std::dec);
            
                updateMapLocation(mi);
            }
        }
    }

    string nameFromId(const string& id) const
    {
        string name;
        
        if (_ifi)
        {
            // recover name by default from objects map
            const Object* obj = findObject(id);
            if (obj) name = obj->_name;
        }

        if (name.empty())
        {
            // last resort
            name = id;
        }
        
        return name;
        
    }

    void _evalItemList(ItemsModel::Entries& list, JSON::ArrayValue& jitems)
    {
        list.clear();

        size_t sz = jitems.size();

        LOG3("evalItemsList, updating ", sz);

        for (size_t i = 0; i < sz; ++i)
        {
            const JSON::Value& ji = jitems.at(i);
            string id = JSON::toString(JSON::at(ji, BRA_INV_ID));
            string name = JSON::toString(JSON::at(ji, BRA_INV_NAME));
            bool worn = JSON::toBool(JSON::at(ji, BRA_INV_WORN));

            if (id.empty()) continue;
            if (name.empty()) name = nameFromId(id);
            name = CapitaliseStartWords(name);
            if (worn) name += " (worn)";

            ItemsModel::Entry e(name, id);
            
            if (_ifi)
            {
                e._icon = JSON::toString(JSON::at(ji, IFI_ICON));
            }
            
            list.emplace_back(e);
        }
    }

    void evalItemList(ItemsModel::Entries& list)
    {
        if (_be)
        {
            if (_rosterJSON)
            {
                JSON::ArrayValue jitems(*_rosterJSON, BRA_INV_ITEMS);
                _evalItemList(list, jitems);
            }
        }
        if (_ifi)
        {
            if (_itemsRosterJSON && _itemsRosterJSON->IsArray())
            {
                JSON::ArrayValue a(*_itemsRosterJSON);
                _evalItemList(list, a);
            }
        }
    }

    void evalPeopleList(ItemsModel::Entries& list)
    {
        if (_be)
        {
            if (_rosterJSON)
            {
                JSON::ArrayValue jitems(*_rosterJSON, BRA_INV_PEOPLE);
                _evalItemList(list, jitems);
            }
        }
        if (_ifi)
        {
            if (_peopleRosterJSON && _peopleRosterJSON->IsArray())
            {
                JSON::ArrayValue a(*_peopleRosterJSON);
                _evalItemList(list, a);
            }
        }
    }

    bool getProductInfo()
    {
        bool r = false;
        if (_be)
        {
            ProductInfo pi;
            r = _be->getProductInfo(pi);
            if (r)
            {
                _host->_productInfoJSON = JSON::fromString(pi._json.c_str());
                if (_host->_productInfoJSON)
                {
                    //LOG3("Product Info: ", pi._json);
                }
                else
                {
                    LOG("Bad product info: ", pi._json);
                    r = false;
                }
            }
        }
        return r;
    }


    string undoredo(bool undo) 
    {
        string s = "failed";
        if (_be)
        {
            const char* cmd = undo ? "undo" : "redo";
            CommandResultI* cres = _be->makeResult();
            if (_be->evalCommand(cmd, cres))
            {
                s = cres->toString();

                // refresh on undo/redo
                updateAfterCommand();
            }
            delete cres;
        }
        return s;
    }

    bool saveGame(const string& name)
    {
        bool r = false;
        string fn = changeSuffix(name, ".sav");
        LOG3("saveGame, ", fn);
        if (_be)
        {
            r = _be->saveGame(fn.c_str());
        }
        else if (_ifi)
        {
            r = ImpIFI::saveGame(fn);
        }
        return r;
    }

    bool loadGame(const string& name)
    {
        bool r = false;

        //string fn = makeDataPath(changeSuffix(name, ".sav"));
        string fn = changeSuffix(name, ".sav");
        LOG3("loadGame, ", fn);
        
        if (_be)
        {
            r = _be->loadGame(fn.c_str());

            // refresh after loading game
            updateAfterCommand();
        }
        else if (_ifi)
        {
            r = ImpIFI::loadGame(fn);
        }
        
        return r;
    }

    bool deleteSaveGame(const string& name)
    {
        string p = makeDataPath(name);
        LOG3("deleteSaveGame ", p);
        bool res = FD::existsFile(p.c_str());
        if (res)
        {
            res = FD::remove(p.c_str());
            if (!res)
            {
                LOG1("deleteSaveGame; file ", p << "failed to delete");
            }
        }
        else
        {
            LOG2("deleteSaveGame; file ", p << "not found");
            res = false;
        }
        return res;
    }

    bool restartGame()
    {
        bool r = false;

        _cemitBuf.clear();

        // clear before restart, to get the start words
        _wordStat.reset();
        
        if (_be)
        {
            if (_be) r = _be->restartGame();
            if (r)
            {
                // update UI after restart
                postEval();
            }
        }
        return r;
    }


    string itemIcon(const string& idstr)
    {
        //LOG3("request for item icon ", idstr);
        string iconFile;

        if (_be)
        {
            int id = atoi(idstr.c_str());
            if (id > 0)
            {
                const ItemInfo* ii = findItemInfo(id);
                if (ii && !ii->_iconFile.empty())
                {
                    string prefix = "qrc:///itemicons/";
                    prefix += ii->_iconFile;
                    //LOG3("item ", idstr << " icon " << prefix);
                    return prefix;
                }
            }
        }

        if (_ifi)
        {
            const Object* o = findObject(idstr);
            if (o)
                iconFile = resolveAsset(o->_icon);
        }

        return iconFile;
    }

    void handleOptions(int argc, char** argv)
    {
        Logged initLog;

        bool logfile = true;

#ifdef NDEBUG
        logfile = false;
#endif
    
        for (int i = 1; i < argc; ++i)
        {
            if (argv[i][0] == '-')
            {
                char* arg;

                if ((arg = isOptArg(argv + i, "-d")) != 0)
                    setLogLevel(atoi(arg));
                else if ((arg = isOptArg(argv + i, "-story")) != 0)
                    _host->_storyfile = arg;
                else if ((arg = isOptArg(argv + i, "-music")) != 0)
                    _host->_musicFile = arg; 
                else if ((arg = isOptArg(argv + i, "-engine")) != 0)
                    _host->_engineName = arg;
                else if ((arg = isOptArg(argv + i, "-t")) != 0) 
                    _recordFilename = arg;
                else if (isOpt(argv[i], "-hidpi"))
                    _host->_options._hiDpi = true;
                else if (isOpt(argv[i], "-truedpi")) _host->_options._useAccurateDPI = true;
                else if (isOpt(argv[i], "-log")) logfile = true;
                else if ((arg = isOptArg(argv + i, "-w")) != 0)
                {
                    int w = atoi(arg);
                    if (w > 0 && w < 10000) _host->_uiOptionWidth = atoi(arg);
                }
                else if ((arg = isOptArg(argv + i, "-h")) != 0)
                    _host->_uiOptionHeight = atoi(arg);
                else if ((arg = isOptArg(argv + i, "-fontsize")) != 0)
                    _host->_uiOptionFontSize = atoi(arg);
                else if (isOpt(argv[i], "-noframe")) _host->_uiNoFrame = true;
                else if ((arg = isOptArg(argv + i, "-simfontscale")) != 0)
                {
                    // this option should not be needed. 
                    // For some reason, if the device pixel ratio < 1
                    // the font scaling somehow knows about this and scales up
                    // but `devicePixelRatio()` is still 1 (qt bug?) so we can't 
                    // automatically adjust for it.
                    // to fix this, for example have -simfontscale 0.5
                    double v = atof(arg);
                    if (v > 0 && v < 10)
                        _host->_uiOptionSimFontSizeMultiplier = v;
                    else LOG1("bad simfontscale, '", arg << "'");
                }
            }
        }

#ifdef _WIN32
        if (logfile)
            initLog.openFile("brahman.log");
#endif

    }

    bool evalCommandDirect(const string& cmd, bool echo)
    {
        string c = trim(cmd);
        if (c.empty()) return true; // ignore
        
        bool r = false;
        if (_be)
        {
            // remove any space on ends of command
            // including any newlines
            r = _be->evalCommand(c.c_str(), 0, echo);
        }
        
        if (_ifi)
        {
            ObjectList ctx(_objects);
            ctx._echo = echo;
            r = ImpIFI::evalCommand(c, ctx);
        }

        if (r) r = postEval();
        return r;
    }

    bool titleText(const string& t) 
    {
        // is overriden by qcontrol
        string tcap = CapitaliseStartWords(t);
        bool r = _titleText != tcap;
        if (r)
            _titleText = tcap;
        return r;
    }

private:

    bool _evalCommandSpecial(const string& cmd, bool echo)
    {
        // return true if handled
        bool res = false;
        if (startsWithIgnoreCase(cmd, "_goto "))
        {
            string placeid = firstWordOf(cmd.c_str() + 5);
            if (!placeid.empty())
            {
                Map::Path path;
                if (_host->_map->pathTo(placeid, path) && path.size() > 1)
                {
                    LOG3("goto place ", placeid << "; " << path);

                    // path starts with current location
                    path.erase(path.begin()); // drop start location

                    // pass command "goto 1 2 3 4"
                    evalCommandDirect(string("_goto ") + path.toString(), echo);
                }
                else
                {
                    LOG2("goto place ", placeid << " unreachable");
                }
                res = true;
            }
        }
        return res;
    }

};

void Control::_init()
{
    _itemsModel = 0;
    _engineName = "brahman";
    
    _imp = new Imp(this);
    _imp->_init();
}

Control::~Control() { delete _imp; }

typedef std::string string;

string Control::currentVersion() const { return _imp->currentVersion(); }
void Control::setLogLevel(int level) { _imp->setLogLevel(level); }
int Control::getLogLevel() const { return _imp->getLogLevel(); }
bool Control::evalCommand(const string& cmd) { return _imp->evalCommand(cmd); }
bool Control::evalClickCommand(const string& cmd) { return _imp->evalClickCommand(cmd); }
bool Control::refreshCommand() { return _imp->refreshCommand(); }
bool Control::evalCommandDirect(const string& cmd, bool echo)
{ return _imp->evalCommandDirect(cmd, echo); }
bool Control::evalUseXwithY(const string& x, const string& y)
{ return _imp->evalUseXwithY(x, y); }

bool Control::menuDrop(const string& x) { return _imp->menuDrop(x); }
bool Control::menuClick(const string& x) { return _imp->menuClick(x); }

void Control::evalItemList(ItemsModel::Entries& list)
{ _imp->evalItemList(list); }

void Control::evalPeopleList(ItemsModel::Entries& list)
{ _imp->evalPeopleList(list); }

bool Control::loadEngine(IFEngineInterface* be)
{ return _imp->loadEngine(be); }

bool Control::loadIFI() { return _imp->loadIFI(); }

bool Control::sendOptionToEngine(const string& opt, const var& val)
{ return _imp->sendOptionToEngine(opt, val); }

void Control::startUp(const char* dpath,
                      const char* configDir,
                      int userSpaceID)
{ _imp->startUp(dpath, configDir, userSpaceID); }

void Control::beginGame() { _imp->beginGame(); }
void Control::coverPageClosed() { _imp->coverPageClosed(); }
bool Control::updateMapInfo(MapInfo& mi) { return _imp->updateMapInfo(mi); }
string Control::undoredo(bool undo) { return _imp->undoredo(undo); }
void Control::postEval() { _imp->postEval(); }

int Control::getScreenDPI() const
{
    // from os.h
    // try to find accurate DPI. This can fail and returns 0 if unknown.
    return ::getScreenDPI(); 
}

string Control::selectWords(const string& s, int start, int end)
{
    // extract certain words from `s` a string of words
    // eg 0,1 => first word
    // eg 0,-1 => all words
    // eg 1,-1 => all but first word
    
    std::vector<string> words;
    split(words, trim(s));

    int n = words.size();
    if (end < 0 || end > n) end = n;

    string res;
    for (int i = start; i < end; ++i)
    {
        if (!res.empty()) res += ' ';
        res += words[i];
    }
    return res;
}

bool Control::saveGame(const string& name) { return _imp->saveGame(name); }
bool Control::loadGame(const string& name) { return _imp->loadGame(name); }
bool Control::restartGame() { return _imp->restartGame(); }
bool Control::getProductInfo() { return _imp->getProductInfo(); }
bool Control::deleteSaveGame(const string& name) { return _imp->deleteSaveGame(name); }

void Control::echoConsoleToTranscript(bool v)
{ _imp->_echoConsoleToTranscript = v; }

void Control::updatePrefDependencies()
{ _imp->updateEchoConsoleMode(); }

void Control::suggestCompletion(const char* w, Words& words)
{_imp->_wordStat.suggestCompletion(w, words); }

string Control::itemIcon(const string& id)
{ return _imp->itemIcon(id); }

void Control::handleOptions(int argc, char** argv)
{ _imp->handleOptions(argc, argv); }

string Control::resolveAsset(const string& path) const
{ return _imp->resolveAsset(path); }

string Control::nameFromId(const string& id) const
{ return _imp->nameFromId(id); }

string Control::titleText() const { return _imp->_titleText; }
bool Control::titleText(const string& s) { return _imp->titleText(s); }

bool Control::compassDirection(uint dir) { return _imp->compassDirection(dir); }

string Control::prefsFilePath() const { return _imp->prefsFilePath(); }
