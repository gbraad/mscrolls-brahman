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
#include <stack>
#include "braschema.h"
#include "cutils.h"
#include "gcvt.h"

struct InfoBase
{
    /*
     * `ctx` is not used here, but is added so back-end engines can
     * optionally store data in this structure between calls.
     */

    std::string         _json;
    bool                _changed = false;
    void*               _ctx = 0;
};

struct InfoBuilder
{
    typedef void emitFn(char c, void* ctx);
    typedef std::string string;
    typedef std::stack<int> EmitCounter;

    emitFn*             _emitter;
    void*               _ctx;

    // count the number of objects at each level, so we know
    // to emit commas
    EmitCounter         _emitCounter;

    InfoBuilder(emitFn* e, void* ctx) : _emitter(e), _ctx(ctx) {}

    void _emit(char c) { (*_emitter)(c, _ctx); }
    void _emit(const char* s) { while (*s) _emit(*s++); } 

    void _emit(int v)
    {
        char buf[16];
        sprintf(buf, "%d", v);
        _emit(buf);
    }

    void _emit(bool v)
    {
        _emit(v ? "true" : "false");
    }

    void _emit(double v)
    {
        char buf[32];

#if 1
        Gcvt g;
        g.gcvt(v, 16, buf);
#else
        // Workaround for locales with , as decimal separator
        char* saved_locale = strdup( setlocale(LC_NUMERIC,NULL) ) ;
        setlocale( LC_NUMERIC, "POSIX" ) ;
        sprintf(buf, "%f", v);
        setlocale( LC_NUMERIC, saved_locale ) ;
        free( saved_locale ) ;
#endif
        _emit(buf);
    }

    void emitTag(const char* tag)
    {
        _emit('"');
        _emit(tag);
        _emit('"');
        _emit(':');
    }

    void emitString(const char* s)
    {
        // emit quoted string as value
        _emit('"');

        // XX handle escape chars?
        _emit(s);
        _emit('"');
    }

    // global

    void begin()
    {
        _emit('{'); 
        _emitCounter.push(0);
    }

    void end()
    {
        _emit('}'); 
        _emitCounter.pop();
    }
    
    int count() const
    {
        // current item count at this level
        return _emitCounter.top();
    }

    int incCount() 
    {
        // inc item count at this level.
        // return old count
        int v = _emitCounter.top();
        _emitCounter.pop();
        _emitCounter.push(v + 1);
        return v;
    }

    void commaInc()
    {
        // emit a comma if necessary and inc count
        if (incCount() > 0) _emit(',');
    }

    void beginArray()
    {
        _emit('[');
        _emitCounter.push(0);
    }

    void endArray()
    {
        _emit(']');
        _emitCounter.pop();
    }

    // helpers

    void beginArray(const char* tag)
    {
        commaInc();
        emitTag(tag);
        beginArray();
    }

    void beginObj()
    {
        commaInc();
        begin();
    }

    void tagInt(const char* tag, int v)
    {
        commaInc();
        emitTag(tag);
        _emit(v);
    }

    void tagString(const char* tag, const char* s)
    {
        commaInc();
        emitTag(tag);
        emitString(s);
    }

    void tagString(const char* tag, const string& s)
    { tagString(tag, s.c_str()); }

    void tagBool(const char* tag, bool v)
    {
        commaInc();  
        emitTag(tag);      
        _emit(v);
    }

    void tagDouble(const char* tag, double v)
    {
        commaInc();  
        emitTag(tag);      
        _emit(v);
    }
};
