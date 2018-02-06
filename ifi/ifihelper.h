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

#include "ifi.h"
#include "growbuf.h"

struct IFIIOHelper: public IFITraits
{
    typedef void charEmitFn(void*, char, int chan);

    IFI*                _ifi = 0;

    // upstream emitter
    emitterFn*          _ifiEmitter = 0;
    
    GrowString          _textBuf;
    int                 _currentChan = 0;

    char*               _result = 0;
    bool                _wantResult = false;

    ~IFIIOHelper() { _purge(); }
    
    static void         emitChar(void* ctx, char c, int chan)
    {
        ((IFIIOHelper*)ctx)->_emitChar(c, chan);
    }

    static void emitProxy(void* ctx, const char* json)
    {
        ((IFIIOHelper*)ctx)->_emitProxy(json);
    }

    bool start(IFI* host, emitterFn* uiEmit)
    {
        _ifi = host;
        _ifiEmitter = uiEmit;
        _ifi->setEmitter(emitProxy, this);
        return true;
    }

    char* evalWithResult(const char* json)
    {
        if (!_ifi->sync()) return 0; // no sync

        // signal capture result
        _purge();
        _wantResult = true;
        
        _ifi->eval(json);

        // donate to caller
        char* r = _result;
        _result = 0;

    }

protected:

    void _purge()
    {
        delete [] _result;
        _result = 0;
    }

    void _emitProxy(const char* json)
    {
        if (_wantResult)
        {
            // capture
            _wantResult = false;
            _result = strdup(json);
        }
        
        (*_ifiEmitter)(_ifi, json);
    }

    void _flush()
    {
        if (_textBuf.size())
        {
            if (_ifiEmitter)
            {
                _textBuf.add(0);
            
                GrowString json;
                json.appendf("{channel:%d,text:\"%s\"}",
                             _currentChan,
                             _textBuf.start());

                (*_ifiEmitter)(_ifi, json.start());
            }

            _textBuf.clear();
        }
    }

    void _emitChar(char c, int chan)
    {
        if (chan != _currentChan)
        {
            _flush();
            _currentChan = chan;
        }

        if (!c) _flush();
        else
        {
            _textBuf.add(c);
        }
    }
};

