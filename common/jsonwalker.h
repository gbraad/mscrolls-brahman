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
#include <ctype.h>
#include "var.h"
#include "growbuf.h"

struct JSONWalker
{
    typedef std::string string;
    
    const char*         _json;
    const char*         _pos;

    JSONWalker(const char* json)
    {
        _json = json;
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
        _pos = _json;
        for (;;)
        {
            char c = _get();
            if (!c || c == '{') break;
        }
    }
    
    void _skipSpace()
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

    string getKey()
    {
        string k;
        _skipSpace();
        const char* st = _pos;
        for (;;)
        {
            char c = _get();
            if (!c || isspace(c) || c == ':')
            {
                _unget(c);
                
                // end of key
                k = string(st, _pos - st);

                // eat any spaces
                do
                {
                    c = _get();
                } while (c && c != ':');
                
                break;
            }
        }
        return k;
    }

    void skipValue()
    {
        // leaves pos at character that ended the value
        int level = 1;
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
                if (c == '{') ++level;
                else if (c == '}')
                {
                    if (!--level) break;
                }
                else if (c == ',' && level == 1) break;
            }
        }
        _unget(c);
    }

    var getValue(bool& isObject)
    {
        // does not get objects
        var r;

        isObject = false;
        _skipSpace();
        const char* st = _pos;
        skipValue();

        if (st != _pos)
        {
            char c = *st;
            if (c == '{') isObject = true;
            else if (c == '"')
            {
                ++st;
                // [st, _pos) represents the string without quotes

                // decode string
                GrowString gs;
                bool esc = false;
                while (st != _pos)
                {
                    if (!esc && *st == '\\')
                    {
                        esc = true;
                    }
                    else
                    {
                        esc = false;
                        gs.add(*st);
                    }
                    ++st;
                }
                gs.add(0);
                r = var(gs.start());
            }
            else
            {
                r.parse(&st);
            }
        }
        return r;
    }

    bool next()
    {
        char c;
        for (;;)
        {
            c = _get();
            if (!c) return false;
            if (c == ',') break;
        }
        return true;
    }

    // helpers
    
    static void addString(GrowString& gs, const char* p)
    {
        // encode string
        gs.add('"');
        while (*p)
        {
            if (*p == '"' || *p == '\\') gs.add('\\');
            gs.add(*p++);
        }
        gs.add('"');
    }

};
