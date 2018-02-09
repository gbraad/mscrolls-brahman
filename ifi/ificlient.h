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
#include "worker.h"
#include "jsonwalker.h"
#include "ifischema.h"

struct IFIClient: public IFI, public Worker
{
    typedef std::string string;
    typedef bool mainLoopFn(void* ctx);
    typedef std::function<bool(IFIClient*)> MainLoop;
    
    Emitter             _emitter;
    MainLoop            _mainLoop;

    GrowString          _inBuffer;
    bool                _inBufferReady = false;
    
    GrowString          _cmdBuffer;
    int                 _cmdPos = 0;

    GrowString          _outBuffer;
    bool                _madeRequest = false;

    // Compliance
    virtual void setEmitter(const Emitter& e) override
    {
        _emitter = e;
    }
    
    virtual bool eval(const char* json) override
    {
        // on host thread
    
        bool r = true;

        std::cout << "eval: '" << json << "'\n";

        // be sure client is not using input buffers
        if (!sync()) return false;

        _inBuffer = json;
        _inBufferReady = true;

        for (JSONWalker jw(json); jw.nextKey(); jw.next())
        {
            bool isObject;
            var v = jw.getValue(isObject);

            /*
            cout << key << ": ";
            if (v) cout << v;
            else if (isObject) cout << "{}";
            else cout << "NULL";
            cout << endl;
            */

            if (v)
            {
                if (jw._key == IFI_COMMAND)
                {
                    _cmdBuffer = v.toString();
                    _cmdPos = 0;
                }
            }
        }

        signal();
        release();
        
        return r;
    }
    
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

    void setMainLoop(const MainLoop& m) { _mainLoop = m; }

    bool workHandler() override
    {
        // return true to continue
        assert(_mainLoop);
        return _mainLoop(this);
    }


    //////////////////////////// Client Helpers
    
    int getchar()
    {
        int c = 0;
        while (!_shutdown)
        {
            if (_cmdPos < _cmdBuffer.size())
            {
                c = _cmdBuffer.start()[_cmdPos++];
                break;
            }

            // only block is we didn't fill from `getRequest`
            if (_madeRequest) break;
            waitForSignal();
        }

        if (!c)
        {
            c = '\n';
            if (_shutdown) c = EOF;

            // subsequent calls to getchar will block unless getRequest
            _madeRequest = false;
        }
        return c;
    }

    const char* getRequest()
    {
        const char* r = 0;

        _madeRequest = true;
        
        while (!_shutdown)
        {
            if (_inBufferReady) break;
            waitForSignal();
        }

        if (_inBufferReady)
        {
            r = _inBuffer.start();

            // buffer has been used now
            _inBufferReady = false;
        }
        
        return r;
        
    }

    void flush()
    {
        assert(_emitter);
        if (_outBuffer.size())
        {
            GrowString gs;
            gs.append("{" IFI_TEXT ":");
            JSONWalker::addString(gs, _outBuffer.start());
            gs.add('}');
            gs.add(0);
            _emitter(gs.start());
            _outBuffer.clear();
        }
    }

    int putchar(int c)
    {
        if (c == '\b')
        {
            _outBuffer.unadd();
        }
        else if (c == '\n' || c <= 0)
        {
            // newline, 0 or EOF ?
            // flushes imply newline, so no need to add it.

            if (_outBuffer.size())
            {
                _outBuffer.add(0);
                flush();
            }
        }
        else
        {
            _outBuffer.add(c);
        }

        return c;
    }

    void putstring(const char* s)
    {
        while (*s) putchar(*s++);
        putchar(0); // terminate
    }

};








