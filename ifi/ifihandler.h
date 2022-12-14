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


#pragma once

#include <string>
#include <vector>
#include "jsonwalker.h"
#include "logged.h"
#include "ifischema.h"
#include "varset.h"
#include "osrandom.h"
#include "strutils.h"

#define TAG "IFIHandler, "

/* IFI_HANDLE_CHOICE is defined (elsewhere) to include code here
 * that breaks down the choice JSON. This isn't needed for the GUI,
 * which consumed the JSON directly, but is needed for cli.
 */

struct IFIHandler
{
    typedef std::string string;
    
    VarSet              _props;

    // valid whilst using builders
    GrowString*         _js = 0;

    // for checking responses before game has been initialised
    bool                _startDone = false;

    struct TextF
    {
        string          _text;
        int             _id = 0;

        bool             valid() const { return !_text.empty(); }
        operator bool() const { return valid(); }

        friend std::ostream& operator<<(std::ostream& os, const TextF& t)
        { return os << "{\"" << t._text << "\", id=" << t._id << '}'; }

        void clear() { _text.clear(); }
    };

    
    struct ChoiceInfo
    {
        TextF           _text;
        string          _response;

        friend std::ostream& operator<<(std::ostream& os, const ChoiceInfo& ci)
        {
            return os << "{ choice=" << ci._text << ", response=" << ci._response << '}';
        }
    };

    typedef std::vector<ChoiceInfo> ChoiceList;
    
    struct ChoicesInfo
    {
        TextF       _header;
        ChoiceList  _choices;
        bool        _textinput = false;
        int         _selected = -1;

        int size() const { return _choices.size(); }

        friend std::ostream& operator<<(std::ostream& os, const ChoicesInfo& ci)
        {
            os << "{ header=" << ci._header << ", textinput=" << ci._textinput << '\n';
            for (auto& i : ci._choices)
            {
                os << i << '\n';
            }
            os << "}\n";
            return os;
        }
    };

#ifdef IFI_HANDLE_CHOICE
    struct HandlerCtx
    {
        ChoicesInfo*     _choices = 0;

        void init()
        {
            if (!_choices) _choices = new ChoicesInfo;
        }

        void clear()
        {
            if (_choices)
            {
                delete _choices;
                _choices = 0;
            }
        }

        ChoicesInfo* donate()
        {
            ChoicesInfo* ci = _choices;
            _choices = 0;
            return ci;
        }

        ~HandlerCtx() { clear(); }

        friend std::ostream& operator<<(std::ostream& os, const HandlerCtx& h)
        {
            if (h._choices) os << *h._choices;
            else os << "null";
            return os;
        }
    };

    HandlerCtx          _hctx;
#endif // IFI_HANDLE_CHOICE

    IFIHandler()
    {
        setProp(IFI_PROMPT, "> ");
        setProp(IFI_UNUSE, IFI_UNUSE_DEFAULT);
        setProp(IFI_USEWITH, IFI_USEWITH_DEFAULT);
        setProp(IFI_COMPASSGO, IFI_COMPASSGO_DEFAULT);
        setProp(IFI_USE, IFI_USE_DEFAULT);
        setProp(IFI_REFRESHCMD, IFI_REFRESHCMD_DEFAULT);
    }

    static string extendPrefix(const string& prefix, const char* key)
    {
        return prefix.size() ? prefix + '/' + key : key;
    }

    static string extendPrefix(const string& prefix, const string& key)
    {
        return prefix.size() ? prefix + '/' + key : key;        
    }
    
    void setProp(const string& key, const var& v)
    {
        LOG4(TAG "set ", key << " = " << v);
        _props[key] = v.copy();
    }

    static bool atObj(JSONWalker& jw, string& subjs, const string* prefix = 0)
    {
        if (*jw._obj == '{')
        {
            subjs = string(jw._obj, jw._pos - jw._obj);
            if (jw._pos[-1] == '}') return true;
            if (!prefix) prefix = &jw._key;
            LOG1(TAG "malformed json object; ", prefix << ":" << subjs);
        }
        return false;
    }

    static bool atList(JSONWalker& jw, string& subjs, const string* prefix = 0)
    {
        if (*jw._obj == '[')
        {
            subjs = string(jw._obj, jw._pos - jw._obj);
            if (jw._pos[-1] == ']') return true;
            if (!prefix) prefix = &jw._key;
            LOG1(TAG "malformed json list; ", prefix << ":" << subjs);
        }
        return false;
    }

    void handleAux(const char* json, const string& prefix)
    {
        for (JSONWalker jw(json); jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);

            if (!st) break; // bad json

            string p = extendPrefix(prefix, jw._key);

            if (!_startDone)
            {
                LOG3(TAG "WARNING, IFI received response before started: ", json);
            }

            if (!isObject)
            {
                var v = jw.collectValue(st);
                
                if (v)
                {
                    if (p != IFI_TEXT) setProp(p, v);
                    ifiKey(p, v);
                }
                else
                {
                    LOG(TAG "Bad IFI ", jw._key);
                    LOG3(TAG "in '", json << "'");
                }
            }
            else // object
            {
                // `obj` is start of object, ie "{
                // `pos` is end of object + 1, ie one past "}"

                bool r = false;
                string subjs;
                
                if (atObj(jw, subjs, &p))
                {
                    if (p == IFI_META)
                        r = ifiMetaResponsePrep(subjs);
                    else if (p == IFI_MAP)
                        r = ifiMapResponse(subjs);
                    else if (p == IFI_PICTURE) // picture as object
                        r = ifiPictureResponse(subjs);
                    else if (p == IFI_ANIMATE) 
                        r = ifiAnimateResponse(subjs);
                    else if (p == IFI_SAVEDATA)
                        r = ifiSaveDataResponse(subjs);
                    else if (p == IFI_TEXT)
                        r = ifiTextFormattedResponse(subjs);
                    else if (p == IFI_SOUND)
                        r = ifiSoundResponse(subjs);
                    else if (p == IFI_CHOICE) 
                        r = ifiChoiceGeneralResponse(subjs);
                        
                    if (!r)
                        handleAux(jw._obj, p);
                }
                else if (atList(jw, subjs, &p))
                {
                    string subjs(jw._obj, jw._pos - jw._obj);
                    
                    if (p == IFI_OBJECTS)
                        r = ifiObjectsResponse(subjs);
                    else if (p == IFI_ITEMS)
                        r = ifiItemsResponse(subjs);
                    else if (p == IFI_PEOPLE)
                        r = ifiPeopleResponse(subjs);
                    else if (p == IFI_MAP "/" IFI_PLACES)
                        r = ifiMapPlacesResponse(subjs);
                    else if (p == IFI_CHOICE)
                        r = ifiChoiceListResponse(subjs);
                }
                else
                {
                    LOG1("IFI expected object; ", p << ":" << subjs);
                }
            }
        }
    }
    
    void handle(const char* json)
    {
        if (!json || !*json) return;

        LOG5("IFIHandler::handle ", json);

        ifiBegin();
        handleAux(json, string());
        ifiEnd();
    }

    virtual void ifiBegin() {}
    virtual void ifiEnd() {}

    static bool isTrue(var v)
    {
        // schema uses string "true" to mean true
        if (v.isString()) return v.toString() == "true";
        return v.isTrue();
    }

    static bool isBool(const var& v)
    {
        // some terms can be bool or something else
        // such as "picture:true"

        if (v.isString())
        {
            // are we a bool masquerading as a string?
            string vs = v.toString();
            if (vs == "true" || vs == "false") return true;
        }
        else
        {
            // non-strings are interpreted as bool
            return true;
        }
        
        return false;
    }
    
    virtual void ifiKey(const string& key, const var& v)
    {
        bool r = false;

        if (key == IFI_COMMAND) r = ifiCommand(v.toString());
        else if (key == IFI_SUBCOMMAND) r = ifiSubcommand(v.toString());
        else if (key == IFI_CONFIGDIR) r = ifiConfigDir(v.toString());
        else if (key == IFI_DATADIR) r = ifiDataDir(v.toString());
        else if (key == IFI_STORY) r = ifiStory(v.toString());
        else if (key == IFI_TEXT) r = ifiText(v.toString());
        else if (key == IFI_BEGIN)
        {
            r = ifiBeginGame();
            if (!r)
            {
                LOG2("IFI, Warning ifiBeginGame returned ", r);
            }
        }
        else if (key == IFI_MOVES)
        {
            int n;
            if (v.isNumber()) n = v.toInt();
            else n = isTrue(v) ? 1 : 0;
            r = ifiMoves(n);
        }
        else if (key == IFI_PROMPT) r = ifiPromptResponse(v.toString());
        else if (key == IFI_RANDOMSEED) r = ifiRandomSeed(v.toInt());
        else if (key == IFI_MAP) // requester
            r = ifiMap(isTrue(v));
        else if (key == IFI_META)
            r = ifiMeta(isTrue(v)); // requester
        else if (key == IFI_LOCATION || key == IFI_MAP "/" IFI_LOCATION)
        {
            r = ifiLocationResponse(v.toString());
        }
        else if (key == IFI_EXITS) r = ifiExitsResponse(v.toInt());
        else if (key == IFI_TITLE) // top-level
            r = ifiTitleTextResponse(v.toString());
        else if (key == IFI_PICTURE)
        {
            if (isBool(v))
            {
                // we are a request
                r = ifiPicture(isTrue(v));
            }
            else
            {
                r = ifiPictureResponse(v.toString());
            }
        }
        else if (key == IFI_SAVEDATA)
        {
            // should be a request. response should be saveobj
            // savedata:true
            if (isBool(v) && isTrue(v))
            {
                // is a request to provide save data
                r = ifiSaveData();
            }
            else
            {
                LOG3("IFI savedata request ignored :", v);
            }
        }
        else if (key == IFI_LOADDATA) r = ifiLoadData(v.toString());
        else if (key == IFI_RESTART)
        {
            // ignore argument
            r = ifiRestartResponse();
        }
        else if (key == IFI_POSTSAVE) { r = ifiPostSave(isTrue(v)); }
        else if (key == IFI_POSTLOAD) { r = ifiPostLoad(isTrue(v)); }

        if (!r) ifiDefault(key, v);
    }
    
    virtual bool ifiCommand(const string&) { return false; }
    virtual bool ifiSubcommand(const string&) { return false; }
    virtual bool ifiConfigDir(const string&)
    {
        LOG2("IFI Warning, no ifiConfigDir handler", "");
        return false;
    }
    virtual bool ifiDataDir(const string&) { return false; }
    virtual bool ifiStory(const string&) { return false; }
    virtual bool ifiText(const string&) { return false; }
    virtual bool ifiTextFormatted(const TextF&) { return false; }
    virtual bool ifiLocationResponse(const string& id) { return false; }
    virtual bool ifiExitsResponse(int mask) { return false; }
    virtual bool ifiTitleTextResponse(const string&) { return false; }
    virtual bool ifiPicture(bool v) { return false; }
    virtual bool ifiPictureResponse(const string&) { return false; }
    virtual bool ifiAnimateResponse(const string&) { return false; }
    virtual bool ifiRestartResponse() { return false; }
    virtual bool ifiBeginGame() { return false; }
    virtual bool ifiSaveData()
    {
        // request
        LOG3("IFI save data request not implemented", "");
        return true;
    }

    virtual bool ifiSave(const uchar* data, int size, const string& name)
    {
        // called with data to save and optional file name
        LOG3("ifiSave not implemented ", name);
        return false;
    }

    virtual bool ifiSaveDataResponse(const string& js)
    {
        // called from the handler, this can be overridden or
        // let it call `ifiSave`
        
        string filename;
        string data;

        for (JSONWalker jw(js.c_str()); jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);

            if (!st) break; // bad json

            if (!isObject)
            {
                if (jw._key == IFI_NAME)
                {
                    filename = jw.collectValue(st).toString();
                }
                else if (jw._key == IFI_DATA)
                {
                    // need to decode strings
                    var d = jw.collectValue(st);
                    data = d.toString();
                }
                else
                {
                    LOG2("IFI, unknown key in savedata obj: ", jw._key);
                }
            }
        }

        if (data.empty())
        {
            LOG1("ifiSaveDataResponse, missing data key in ", js);
        }
        else
        {
            ifiSave((uchar*)data.c_str(), data.size(), filename);
        }

        return true;
    }

    virtual bool ifiLoadData(const string& s)
    {
        // NB: this can be a request or a response.
        // when a request: `s` is the data
        // when a response: `s` is optionally, a filename
        // and although a "response" is actually a request to load
        
        LOG3("IFI loaddata not implemented ", s);
        return true;
    }
    
    // request & response
    virtual bool ifiMoves(int n) { return false; } 

    // these return true since they're optional
    virtual bool ifiPromptResponse(const string& p) { return true; }
    virtual bool ifiRandomSeed(int64) { return true; }
    virtual bool ifiMap(bool) { return true; }
    virtual bool ifiMeta(bool) { return true; }
    virtual bool ifiMetaResponse(const string& js) { return false; }
    virtual bool ifiObjectsResponse(const string& js) { return false; }
    virtual bool ifiItemsResponse(const string& js) { return false; }
    virtual bool ifiPeopleResponse(const string& js) { return false; }
    virtual bool ifiMapResponse(const string& js) { return false; }
    virtual bool ifiMapPlacesResponse(const string& js) { return false; }
    virtual bool ifiSoundResponse(const string& js) { return false; }


    // these are optional
    virtual bool ifiPostSave(bool v) { return true; }
    virtual bool ifiPostLoad(bool v) { return true; }

    bool ifiMetaResponsePrep(const string& js)
    {
        // add some elements to the meta:
        // for example the credits have to include open source attributions.

        GrowString js2;
        js2.add('{');

        static const char* extracredits = 
                "<h2>Brahman GUI</h2>"
                "<p>More info at <a href=\"https://strandgames.com/\">Strand Games</a></p>"
            ;
            
        for (JSONWalker jw(js); jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);
            if (!st) break;
            
            if (jw._key == IFI_CREDITS)
            {
                string v = jw.collectValue(st).toString();
                v += extracredits;
                JSONWalker::addStringValue(js2, jw._key.c_str(), v);
            }
            else
            {
                string v(st, jw._pos-st);
                JSONWalker::addKeyObject(js2, jw._key, v.c_str());
            }
        }

        js2.add('}');
        js2.add(0);

        return ifiMetaResponse(js2.start());
    }
    
    virtual bool ifiTextFormattedResponse(const string& js)
    {
        TextF textF;
        bool r = _makeTextF(js, textF);

        if (r)
        {
            r = ifiTextFormatted(textF);
            
            if (!r)
            {
                // fallback to non-formatted
                r = ifiText(textF._text);
            }
        }
        return r;
    }

    virtual bool ifiChoiceGeneralResponse(const string& js)
    {
#ifdef IFI_HANDLE_CHOICE
        // choice:{text:"heading",ui_textinput:true,choices:[{choiceobj}...]}
        // choice:{text:{textobj},choices:[{choiceobj}...]}

        // extract the top-level choice properties, then call
        // the choice list response to extract the choice list itself
        
        bool r = true;

        _hctx.init(); 

        for (JSONWalker jw(js); r && jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);
            if (!st) break; // bad json

            string subjs;
                                
            if (jw._key == IFI_TEXT) // heading
            {
                TextF& textF = _hctx._choices->_header;
                
                // can be just a string or a text object
                if (isObject) r = atObj(jw, subjs) && _makeTextF(subjs, textF);
                else textF._text = jw.collectValue(st).toString();

                // ensure header text is trimmed
                textF._text = trim(textF._text);
            }
            else if (jw._key == IFI_UI_TEXTINPUT)
            {
                bool v = jw.collectValue(st).isTrue();
                _hctx._choices->_textinput = v;
            }
            else if (jw._key == IFI_CHOICE)
            {
                // expect list!
                r = atList(jw, subjs) && ifiChoiceListResponse(subjs);
            }
        }
#endif // IFI_HANDLE_CHOICE        
        
        // always return true, saying we're done even if fail
        return true;
    }

#ifdef IFI_HANDLE_CHOICE
    bool addChoice(const string& choiceobj)
    {
        // {text:"whatever",chosen:"whatever"}
        // {text:{textobj},chosen:{"command":1} }
        
        bool r = true;

        LOG5(TAG "choiceobj ", choiceobj);

        ChoiceInfo choice;
        
        for (JSONWalker jw(choiceobj); r && jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);
            if (!st) break; // bad json

            if (jw._key == IFI_TEXT) // heading
            {
                TextF& textF = choice._text;

                // can be just a string or a text object
                string subjs;
                if (isObject) r = atObj(jw, subjs) && _makeTextF(subjs, textF);
                else textF._text = jw.collectValue(st).toString();
            } 
            else if (jw._key == IFI_CHOSEN)
            {
                if (isObject)
                {
                    string subjs;
                    r = atObj(jw, subjs);
                    if (r)
                    {
                        choice._response = subjs;
                    }
                    else break;
                    
                }
                else choice._response = jw.collectValue(st).toString();
            }
        }

        r = r && choice._text;
        
        if (r)
        {
            if (choice._response.empty())
            {
                // use the choice text
                choice._response = choice._text._text;
            }

            _hctx._choices->_choices.push_back(choice);
        }
        else
        {
            LOG3(TAG "malformed choice ", choiceobj);
        }

        return r;
    }

    // override this to handle a broken down version of choices
    virtual bool ifiHandleChoicesInfo(ChoicesInfo*) { return false; }

    void finishChoice()
    {
        LOG5("got choice ", _hctx);

        ChoicesInfo* ci = _hctx.donate();

        // assume handler consumes `ci`
        bool r = ifiHandleChoicesInfo(ci);
        if (!r)
        {
            // didn't handle it. so delete
            delete ci;
        }
    }
#endif // IFI_HANDLE_CHOICE

    virtual bool ifiChoiceListResponse(const string& js)
    {
        // [{choiceobj}...]

        LOG5(TAG "choice list ", js);
        
#ifdef IFI_HANDLE_CHOICE

        _hctx.init();
        JSONWalker jw;
        jw._json = js.c_str();
        jw.beginArray();
        while (!jw._error && !jw._end)
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);
            if (!st) break; // bad json

            if (!isObject)
            {
                LOG3(TAG "Bad choice list ", st);
                continue;
            }

            // expect a list of objects
            string choiceobj;
            if (!atObj(jw, choiceobj)) continue;

            // handle cjoiceobj
            if (!addChoice(choiceobj)) break; // abort if malformed

            jw.next();
        }

        finishChoice();
#endif // IFI_HANDLE_CHOICE
        
        return true; // accept regardless
    }

    virtual void ifiDefault(const string& key, const var& v)
    {
        LOG4("IFI unhandled ", key << " " << v.toString());
    }

    var getProp(const string& key) const
    {
        return _props.find(key);
    }

    bool propTrue(const string& key) const
    {
        return isTrue(getProp(key));
    }

    var getProp(const string& prefix, const string& key) const
    {
        return getProp(extendPrefix(prefix, key));
    }

    void buildJSONStart(GrowString& js)
    {
        js.clear();
        _js = &js;
        _js->add('{');
    }

    void buildInitialJSON()
    {
        // build simple json for startup
        assert(_js);

        // will add those defined
        JSONWalker::addKeyValue(*_js, IFI_CONFIGDIR, getProp(IFI_CONFIGDIR));
        JSONWalker::addKeyValue(*_js, IFI_DATADIR, getProp(IFI_DATADIR));
        JSONWalker::addKeyValue(*_js, IFI_STORY, getProp(IFI_STORY));
    }

    void buildPrologueJSON()
    {
        assert(_js);
        
        // request meta data
        JSONWalker::addBoolValue(*_js, IFI_META, true);

        // request object map
        JSONWalker::addBoolValue(*_js, IFI_OBJECTS, true);

        // signal we want map updates
        JSONWalker::addBoolValue(*_js, IFI_MAP, true);

        // signal we want pictures updates
        JSONWalker::addBoolValue(*_js, IFI_PICTURE, true);

        // signal we want items updates
        JSONWalker::addBoolValue(*_js, IFI_ITEMS, true);

        // signal we want people updates
        JSONWalker::addBoolValue(*_js, IFI_PEOPLE, true);

        // signal we want move count updates
        //JSONWalker::addBoolValue(*_js, IFI_MOVES, true);

        int64 r = makeRandomSeed();
        
        // include random number
        JSONWalker::addKeyValue(*_js, IFI_RANDOMSEED, r);

    }

    GrowString& buildJSONEnd()
    {
        assert(_js);
        GrowString* js = _js;
        js->add('}');
        js->add(0);
        _js = 0;
        return *js;
    }

    void addKeyProp(GrowString& js, const string& prefix, const char* key)
    {
        var v = getProp(prefix, key);
        if (v) JSONWalker::addKeyValue(js, key, v);
    }

    bool buildCmdJSON(const char* cmdp)
    {
        assert(_js);

        bool raw = false;
        

#ifndef NDEBUG
        // incorporate some direct json if start with "{"
        raw = (*cmdp == '{');

        if (raw)
        {
            // find content of json
            const char* st = cmdp + 1;
            const char* ep = 0;
            const char* p = st;
            while (*p)
            {
                if (*p == '}') ep = p;
                ++p;
            }

            if (ep)
            {
                _js->append(st, ep - st);
                LOG3("IFI raw json ", _js->start());
            }
        }
#endif
        
        if (!raw)
        {
            JSONWalker::addStringValue(*_js, IFI_COMMAND, cmdp);
        }

        return true;
    }

    void resetAll()
    {
        _props.clear();
        _startDone = false;
    }

protected:
    
    static bool _makeTextF(const string& js, TextF& textF)
    {
        bool res = false;
        
        for (JSONWalker jw(js); jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);

            if (!st) break; // bad json

            if (!isObject)
            {
                if (jw._key == IFI_TEXT)
                {
                    res = true;
                    textF._text = jw.collectValue(st).toString();
                }
                else if (jw._key == IFI_ID)
                {
                    textF._id = jw.collectValue(st).toInt();
                }
                // ignore others
            }
        }
        return res;
    }
};

#undef TAG

