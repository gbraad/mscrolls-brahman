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

#pragma once

#include <string>
#include "jsonwalker.h"
#include "logged.h"
#include "ifischema.h"
#include "varset.h"

struct IFIHandler
{
    typedef std::string string;

    VarSet              _props;

    // valid whilst using builders
    GrowString*         _js = 0;

    IFIHandler()
    {
        setProp(IFI_PROMPT, "> ");
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
        LOG4("IFIHandler, set ", key << " = " << v);
        _props[key] = v.copy();
    }

    void handleAux(const char* json, const string& prefix)
    {
        for (JSONWalker jw(json); jw.nextKey(); jw.next())
        {
            bool isObject;
            var v = jw.getValue(isObject);

            if (jw._error)
            {
                LOG1("IFI, bad json ", json);
                return;
            }

            string p = extendPrefix(prefix, jw._key);
            
            if (v)
            {
                if (p != IFI_TEXT) setProp(p, v);
                ifiKey(p, v);
            }
            else if (isObject)
            {
                bool r = false;
                
                // `obj` is start of object, ie "{
                // `pos` is end of object + 1, ie one past "}"

                if (*jw._obj == '{')
                {
                    assert(jw._pos[-1] == '}');
                    string subjs(jw._obj, jw._pos - jw._obj);
                    //LOG3("IFI subjs ", subjs);

                    if (p == IFI_META)
                    {
                        r = ifiMetaResponse(subjs);
                    }
                    else if (p == IFI_MAP)
                    {
                        r = ifiMapResponse(subjs);
                    }
                    
                    if (!r)
                        handleAux(jw._obj, p);
                }
                else 
                {
                    // array
                    assert(jw._obj[0] == '[' && jw._pos[-1] == ']');
                    
                    string jlist(jw._obj, jw._pos - jw._obj);
                        
                    if (p == IFI_OBJECTS)
                        r = ifiObjectsResponse(jlist);
                    else if (p == IFI_ITEMS)
                        r = ifiItemsResponse(jlist);
                    else if (p == IFI_MAP "/" IFI_PLACES)
                    {
                        r = ifiMapPlacesResponse(jlist);
                    }
                }
            }
            else
            {
                LOG("Bad IFI ", jw._key);
            }
        }
    }
    
    void handle(const char* json)
    {
        if (!json || !*json) return;

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
    
    virtual void ifiKey(const string& key, const var& v)
    {
        bool r = false;

        if (key == IFI_COMMAND) r = ifiCommand(v.toString());
        else if (key == IFI_CONFIGDIR) r = ifiConfigDir(v.toString());
        else if (key == IFI_DATADIR) r = ifiDataDir(v.toString());
        else if (key == IFI_STORY) r = ifiStory(v.toString());
        else if (key == IFI_TEXT) r = ifiText(v.toString());
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
        else if (key == IFI_EXITS && v.isNumber())
        {
            r = ifiExitsResponse(v.toInt());
        }

        if (!r) ifiDefault(key, v);
    }
    
    virtual bool ifiCommand(const string&) { return false; }
    virtual bool ifiConfigDir(const string&) { return false; }
    virtual bool ifiDataDir(const string&) { return false; }
    virtual bool ifiStory(const string&) { return false; }
    virtual bool ifiText(const string&) { return false; }
    virtual bool ifiLocationResponse(const string& id) { return false; }
    virtual bool ifiExitsResponse(int mask) { return false; }

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
    virtual bool ifiMapResponse(const string& js) { return false; }
    virtual bool ifiMapPlacesResponse(const string& js) { return false; }

    virtual void ifiDefault(const string& key, const var& v)
    {
        LOG3("IFI unhandled ", key << " " << v.toString(true));
    }

    var getProp(const string& key) const
    {
        return _props.find(key);
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
        JSONWalker::addBoolValue(*_js, IFI_MOVES, true);

        // include random number
        JSONWalker::addKeyValue(*_js, IFI_RANDOMSEED, var(0));

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

#if 0
    bool metaReplyObj(GrowString& js)
    {
        string prefix = IFI_META;
        var v;
        
        js.add('{');
        addKeyProp(js, prefix, IFI_TITLE);
        addKeyProp(js, prefix, IFI_AUTHOR);
        addKeyProp(js, prefix, IFI_COVERTEXT);
        addKeyProp(js, prefix, IFI_CREDITS);
        addKeyProp(js, prefix, IFI_VERSION);
        addKeyProp(js, prefix, IFI_ANDROID_MARKET);
        addKeyProp(js, prefix, IFI_IOS_MARKET);
        addKeyProp(js, prefix, IFI_BACKIMAGE);
        addKeyProp(js, prefix, IFI_EFFECT);
        js.add('}');
        js.add(0);

        // did we add anything?
        return js.size() > 3;
    }
    
    bool metaReplyJSON(GrowString& js)
    {
        GrowString js1;
        bool r = metaReplyObj(js1);
        if (r)
        {            
            js.add('{');
            JSONWalker::addKeyObject(js, IFI_META, js1.start());
            js.add('}');
            js.add(0);
        }
        return r;
    }
#endif

    bool buildCmdJSON(const char* cmdp)
    {
        assert(_js);

        // incorporate some direct json if start with "{"
        if (*cmdp == '{')
        {

            JSONWalker jw(cmdp);
            for (; jw.nextKey(); jw.next()) jw.skipValue();
            
            if (!jw.ok())
            {
                LOG1("malformed JSON in input: '", jw << "'");
                return false;
            }
            else
            {
                // append content of json (ie not "{" or "}")
                const char* st = jw._json + 1;
                const char* ep = jw._pos - 1;
                if (ep > st) _js->append(st, ep - st);
                jw.skipSpace();
                cmdp = jw._pos; // any remainder is command
            }
        }
        
        if (*cmdp)
            JSONWalker::addStringValue(*_js, IFI_COMMAND, cmdp);
        
        return true;
    }

};



