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

#include <iostream>
#include <string>
#include <ctype.h>
#include "var.h"
#include "growbuf.h"
#include "logged.h"

struct JSONWalker
{
    typedef std::string string;
    
    const char*         _json;
    const char*         _pos;

    // start of subobject, valid after `getValue`
    const char*         _obj;

    // current key
    string              _key;
    bool                _end;
    bool                _error;
    int                 _level;
    int                 _blevel;

    JSONWalker() {}
    JSONWalker(const char* json) { _json = json; begin(); }
    JSONWalker(const string& j) { _json = j.c_str(); begin(); }

    bool ok() const { return _error == false; }

    friend std::ostream& operator<<(std::ostream& os, const JSONWalker& jw)
    {
        return os << string(jw._json, jw._pos - jw._json);
    }

    char _get()
    {
        char c = *_pos;
        if (c) ++_pos;
        return c;
    }

    void _unget(char c)
    {
        if (c) --_pos;
    }

    void _init()
    {
        _level = 0;
        _blevel = 0;
        _end = false;
        _error = false;
        _pos = _json;
        skipSpace();
    }

    void begin()
    {
        _init();
        char c = _get();
        if (c != '{')
        {
            _error = true;
        }
        else
        {
            ++_level;
            skipSpace();
            c = _get();
            if (c == '}')  
            {
                // empty json {}
                --_level;
                _end = true;
            }
            else
            {
                _unget(c);
            }
        }
    }

    void beginArray()
    {
        _init();
        char c = _get();
        if (c != '[')
        {
            _error = true;
        }
        else
        {
            ++_blevel;
            skipSpace();
            c = _get();
            if (c == ']')  
            {
                // empty array []
                --_blevel;
                _end = true;
            }
            else
            {
                _unget(c);
            }
        }
    }
    
    void skipSpace()
    {
        for (;;)
        {
            char c = _get();
            if (!isspace(c))
            {
                _unget(c);
                break;
            }
        }
    }

    void unskipSpace()
    {
        while (_json != _pos)
        {
            if (!isspace(_pos[-1])) break;
            --_pos;
        }
    }

    bool nextKey()
    {
        _key = _getKey();
        if (_error)
        {
            LOG2("Json error ", _json << " at " << _pos);
        }
        return !_key.empty();
    }

    string _getKey()
    {
        // leaves position next char after ':"
        string k;
        if (!_end && !_error)
        {
            skipSpace();
            char c = _get();
            if (c)
            {
                if (c != '"') _error = true;
                else 
                {
                    const char* st = _pos;
                    for (;;)
                    {
                        c = _get();
                        if (!c)
                        {
                            _error = true;
                            break;
                        }
                        else if (c == '"')
                        {
                            // ASSUME we can't escape " inside keys
                            // end of key

                            k = string(st, _pos - st - 1);
                            
                            skipSpace();
                            c = _get();
                            if (c != ':') _error = true;
                            break;
                        }
                    }
                }
            }
        }
        return k;
    }

    void skipValue()
    {
        // skips over one json value, which can be a whole {object} or
        // a whole [array]
        // leaves pos next char after that which ended value
        
        int level0 = _level;
        int blevel0 = _blevel;
        bool inquote = false;
        bool esc = false;
        char c;
        
        while (!_error)
        {
            c = _get();
            if (!c) break;
            if (esc)
            {
                esc = false;
                continue;
            }

            if (c == '\\')
            {
                esc = true;
                continue;
            }
            
            if (c == '"')
            {
                inquote = !inquote;

                // pos at endquote + 1
                if (!inquote && _level == level0) break;
            }                        
            else if (!inquote)
            {
                if (c == '{') ++_level;
                else if (c == '}')
                {
                    --_level;

                    if (_level < level0)
                    {
                        // must have ended
                        _end = true;

                        // [] should be level
                        if (_blevel != blevel0) _error = true;
                        break;
                    }

                    if (_level == level0 && _blevel == blevel0) break;
                }
                else if (c == '[')
                {
                    ++_blevel;
                }
                else if (c == ']')
                {
                    --_blevel;
                    if (_blevel < blevel0)
                    {
                        // ended or error
                        _end = true;
                        if (_level != level0) _error = true;
                        break;
                    }
                    
                    if (_level == level0 && _blevel == blevel0) break;

                }
                else
                {
                    // can leave post at comma + 1
                    if (c == ',' &&
                        _level == level0 && _blevel == blevel0) break;
                }
            }
        }
    }

    const char* checkValue(bool& isObject)
    {
        skipSpace();
        const char* st = _pos;
        skipValue();

        isObject = false;

        if (_error || st == _pos)
        {
            LOG1("Bad JSON at ", st);
            st = 0;
        }
        else
        {
            char c = *st;
            if (c == '{')
            {
                isObject = true;

                // remember the start of the sub-object
                _obj = st;
            }
            else if (c == '[')
            {
                isObject = true;
                _obj = st;
            }
        }
        
        return st;
    }

    var getValue(bool& isObject)
    {
        var v;
        const char* st = checkValue(isObject);
        if (st && !isObject)
            v = collectValue(st);
        
        return v;
    }

    const char* collectStringBounds(const char* st, size_t& size)
    {
        assert(*st == '"');
        
        // pos is end quote + 1
        // [st+1, _pos-1) represents the string without quotes
                
        assert(_pos[-1] == '"');

        ++st;
        size = (_pos - 1) - st;
        return st;
    }

    string collectRawStringValue(const char* st)
    {
        // expect string value
        
        assert(!_error);
        assert(st != _pos);
        assert(*st != '{' && *st != '[');
        
        char c = *st;
        if (c == '"')
        {
            size_t size;
            st = collectStringBounds(st, size);
            return string(st, size);
        }
        return string();
    }

    var collectValue(const char* st)
    {
        // does not get objects
        var r;

        assert(!_error);
        assert(st != _pos);
        assert(*st != '{' && *st != '[');
        
        char c = *st;
        if (c == '"')
        {
            size_t size;
            st = collectStringBounds(st, size);
            
            GrowString gs;
            decodeString(gs, st, size);
            gs.add(0);
            r = var(gs.start());
        }
        else if (_pos - st == 5 && !strncmp(st, "true", 4)) // "true" + 1
        {
            // convert true and false to int
            r = var(1);
        }
        else if (_pos - st == 6 && !strncmp(st, "false", 5))
        {
            r = var(0);
        }
        else
        {
            r.parse(&st);
        }
        return r;
    }

    void next()
    {
        // assume at end char of last value + 1

        if (_end || _error) return;

        assert(_json != _pos);

        // char that ended term
        char c = _pos[-1];
        
        if (c == ',')
        {
            // last term ended with comma, OK
        }
        else if (c == '"' || c == ']' || c == '}')
        {
            // ended of quote or object, now expect comma
            skipSpace();
            char c = _get();
            if (c == '}')
            {
                // must be final }
                _end = true;
                --_level;
                if (_level || _blevel) _error = true;
            }
            else if (c == ']')
            {
                // final ] of array
                _end = true;
                --_blevel;
                if (_level || _blevel) _error = true;
            }
            else if (c != ',')
            {
                _error = true;
            }
        }
        else
        {
            _error = true;
        }
    }

    // helpers

    static void decodeString(GrowString& gs,
                             const char* st,
                             size_t size)
    {
        // NB: does not terminate gs buffer
        bool esc = false;
        const char* ep = st + size;
        while (st != ep)
        {
            char c = *st++;

            if (!esc)
            {
                if (c == '\\')
                {
                    esc = true;
                    continue;
                }
            }
            else
            {
                esc = false;
                if (c == 'n') c = '\n';
            }
            gs.add(c);
        }
    }
    
    static void decodeString(GrowString& gs, const char* p)
    {
        decodeString(gs, p, strlen(p));
    }

    static void encodeString(GrowString& gs, const char* p)
    {
        gs.add('"');
        while (*p)
        {
            char c = *p++;
            if (c == '\n')
            {
                gs.add('\\');
                gs.add('n');
            }
            else
            {
                if (c == '"' || c == '\\') gs.add('\\');
                gs.add(c);
            }
        }
        gs.add('"');
    }

    static void addKey(GrowString& gs, const char* key)
    {
        gs.add('"');
        gs.append(key);
        gs.add('"');
        gs.add(':');
    }

    static void addValue(GrowString& gs, const var& v)
    {
        if (v)
        {
            toAdd(gs);
            if (v.isString()) encodeString(gs, v.toString().c_str());
            else gs.append(v.toString());        
        }
    }

    static void addKeyValue(GrowString& gs, const char* key, const var& v)
    {
        if (v)
        {
            toAdd(gs);
            addKey(gs,key);
            if (v.isString()) encodeString(gs, v.toString().c_str());
            else gs.append(v.toString());
        }
    }
    
    static void addKeyValue(GrowString& gs, const string& key, const var& v)
    {
        addKeyValue(gs, key.c_str(), v);
    }

    static void addStringValue(GrowString& gs,
                               const char* key, const char* v)
    {
        toAdd(gs);
        addKey(gs,key);
        encodeString(gs, v);
    }

    static void addRawStringValue(GrowString& gs,
                                  const char* key, const char* v)
    {
        // do not encode string
        // only use if you _know_ the string is clean
        toAdd(gs);
        addKey(gs,key);
        gs.add('"');
        gs.append(v);
        gs.add('"');
    }

    static void addStringValue(GrowString& gs,
                               const char* key, const string& v)
    {
        addStringValue(gs, key, v.c_str());
    }

    static void addBoolValue(GrowString& gs, const char* key, bool v)
    {
        toAdd(gs);
        addKey(gs,key);
        gs.append(v ? "true" : "false");
    }

    static void addKeyObject(GrowString& gs, const char* key, const char* json)
    {
        // add a json object as value of `key`
        // ASSUME `json` is valid!

        toAdd(gs);
        addKey(gs, key);
        gs.append(json);
    }

    static void addKeyObject(GrowString& gs, const string& key, const char* json)
    {
        addKeyObject(gs, key.c_str(), json);
    }

    
    static void toAdd(GrowString& gs)
    {
        char c = gs.last();
        if (c && c != ',' && c != '{' && c != '[') gs.add(',');
    }


};
