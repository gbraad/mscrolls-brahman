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

#include <assert.h>
#include "ifi.h"
#include "growbuf.h"
#include "worker.h"

struct IFIClient: public IFI, public Worker
{
    typedef std::string string;
    typedef bool mainLoopFn(void* ctx);

    emitterFn*          _emitter = 0;
    void*               _emitterCtx = 0;

    mainLoopFn*         _mainLoop = 0;
    void*               _mainCtx = 0;

    GrowString          _inBuffer;
    int                 _inPos = 0;

    GrowString          _outBuffer;

    // Compliance
    virtual void setEmitter(emitterFn* emit, void* ctx) override
    {
        _emitter = emit;
        _emitterCtx = ctx;
    }
    
    virtual bool eval(const char* json) override;
    virtual bool start() override
    {
        return Worker::start();
    }
    
    virtual bool sync(int timeoutms = 0) override
    {
        return Worker::sync(timeoutms);
    }

    virtual void release() override
    {
        Worker::release();
    }

    //////////////////////// Host Helpers
    
    static void makeCommand(GrowString& gs, const char* cmd)
    {
        gs.appendf("{command:\"%s\"}", cmd);
        gs.add(0);
    }

    void setMainLoop(mainLoopFn* mf, void* ctx)
    {
        _mainLoop = mf;
        _mainCtx = ctx;
    }

    bool workHandler() override
    {
        assert(_mainLoop);
        return (*_mainLoop)(_mainCtx);
    }

    bool evalCommand(const string& s)
    {
        if (!sync()) return false;
        
        // fill input buffer
        _inBuffer = s.c_str();
        _inBuffer.add(0); // signal end of input
        _inPos = 0;

        signal();

        release();
        
        return true;
    }

    //////////////////////////// Client Helpers
    
    char getchar()
    {
        char c;
        while (!_shutdown)
        {
            if (_inPos < _inBuffer.size())
            {
                c = _inBuffer.start()[_inPos++];
                break;
            }

            waitForSignal();
        }
        
        return c;
    }

    void flush()
    {
        assert(_emitter);
        if (_outBuffer.size())
        {
            GrowString json;
            json.appendf("{channel:0,text:\"%s\"}", _outBuffer.start());
            json.add(0);
            (*_emitter)(_emitterCtx, json.start());
            _outBuffer.clear();
        }
    }

    void putchar(char c)
    {
        if (c == '\b')
        {
            _outBuffer.unadd();
        }
        else
        {
            _outBuffer.add(c);
        }

        if (!c)
        {
            // flush
            flush();
        }
    }

    void putstring(const char* s)
    {
        while (*s) putchar(*s++);
        putchar(0); // terminate
    }

};








