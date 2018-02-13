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

struct JSONWalker
{
    typedef std::string string;
    
    const char*         _json;
    const char*         _pos;

    // current key
    string              _key;
    bool                _end;
    bool                _error;
    int                 _level = 0;

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

    void begin()
    {
        _end = false;
        _error = false;
        _pos = _json;

        skipSpace();
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
        // leaves pos next char after that which ended value
        int level0 = _level;
        bool inquote = false;
        bool esc = false;
        char c;
        for (;;)
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
                if (!inquote) break;
            }                        
            else if (!inquote)
            {
                if (c == '{') ++_level;
                else if (c == '}')
                {
                    --_level;
                    if (_level == level0) break;
                    else if (_level < level0)
                    {
                        // must have ended
                        _end = true;
                        break;
                    }
                }
                else if (c == ',' && _level == level0) break;
            }
        }
    }

    var getValue(bool& isObject)
    {
        // does not get objects
        var r;

        isObject = false;
        skipSpace();
        const char* st = _pos;
        skipValue();

        if (st != _pos && !_error)
        {
            char c = *st;
            if (c == '{') isObject = true;
            else if (c == '"')
            {
                // pos is end quote + 1
                // [st+1, _pos-1) represents the string without quotes
                
                assert(_pos[-1] == '"');

                GrowString gs;
                decodeString(gs, st+1, _pos-1);
                r = var(gs.start());
            }
            else
            {
                r.parse(&st);
            }
        }
        return r;
    }

    void next()
    {
        // assume at end char of last value + 1

        if (_end || _error) return;

        assert(_json != _pos);

        const char* ep = _pos - 1;
        if (*ep == ',')
        {
            // last term ended with comma, OK
        }
        else if (*ep == '"')
        {
            // ended of quote, now expect comma
            skipSpace();
            char c = _get();
            if (c == '}')
            {
                // must be final }
                _end = true;
                if (--_level != 0) _error = true;
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
                             const char* ep)
    {
        bool esc = false;
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
        gs.add(0);
    }
    
    static void decodeString(GrowString& gs, const char* p)
    {
        decodeString(gs, p, p + strlen(p));
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

    static void addStringValue(GrowString& gs,
                               const char* key, const char* v)
    {
        _toAdd(gs);
        addKey(gs,key);
        encodeString(gs, v);
    }

    static void addStringValue(GrowString& gs,
                               const char* key, const string& v)
    {
        addStringValue(gs, key, v.c_str());
    }

protected:
    
    static void _toAdd(GrowString& gs)
    {
        char c = gs.last();
        if (c && c != ',' && c != '{') gs.add(',');
    }

};
