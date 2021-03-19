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
#include <stdarg.h>
#include "ifi.h"
#include "worker.h"
#include "jsonwalker.h"
#include "ifischema.h"
#include "logged.h"
#include "opt.h"

struct IFIClient: public IFI, public Worker
{
    typedef std::string string;
    
    charEmitFn*         _emitter;
    void*               _eCtx;

    GrowString          _inBuffer;
    bool                _inBufferReady = false;
    
    GrowString          _cmdBuffer;
    uint                _cmdPos = 0;

    GrowString          _outBuffer;
    bool                _madeRequest = false;

    int                 _argc;
    char**              _argv;

    bool                _coop = false;
    Pump                _pump; // only if coop

    // Compliance
    void setEmitter(charEmitFn* e, void* ctx)  override
    {
        _emitter = e;
        _eCtx = ctx;
    }

    void emitResponse(const char* json)
    {
        assert(_emitter && json && *json);
        LOG4("response, ", json);
        (*_emitter)(_eCtx, json);
    }

    void emitSingleResponse(const char* key, const char* val)
    {
        GrowString gs;
        gs.add('{');
        JSONWalker::addStringValue(gs, key, val);
        gs.add('}');
        gs.add(0);
        emitResponse(gs.start());
    }

    void emitSingleResponse(const char* key, const var& val)
    {
        GrowString gs;
        gs.add('{');
        JSONWalker::addKeyValue(gs, key, val);
        gs.add('}');
        gs.add(0);
        emitResponse(gs.start());        
    }
    
    virtual bool eval(const char* json) override
    {
        // on host thread
        // NB: expect to be in SYNC state
    
        bool r = true;

        LOG4("eval: '", json << "'\n");

        // this will copy the input json
        _inBuffer = json;
        _inBufferReady = true;

        for (JSONWalker jw(json); jw.nextKey(); jw.next())
        {
            // we don't need to decode values, just look for
            // any command key and decode just THAT value.

            bool isObject;
            const char* st = jw.checkValue(isObject);
            
            if (st && !isObject && jw._key == IFI_COMMAND)
            {
                var v = jw.collectValue(st);
                _cmdBuffer = v.toString();
                _cmdPos = 0;
            }
        }

        if (!_coop) signal();
        
        return r;
    }
    
    virtual bool start(int argc, char** argv) override
    {
        _argc = argc;
        _argv = argv;
        
        // called by host to start the client (on host thread)
        handleOptions(argc, argv);
        
        Opt::rebuildArgs(_argc, _argv);

        // in coop mode, we do not start the client thread
        if (_coop)
        {
            // call client main immediately
            // this would normally be called first thing
            // on the worker thread.
            client_main(_argc, _argv);
            return true;
        }
        return Worker::start("IFIClient");
    }

    virtual void coopWork() {}

    virtual int sync(int timeoutms = 0) override
    {
        if (_coop)
        {
            // do client work here
            coopWork();
            return 1;  // signal done
        }
        else
        {
            return Worker::sync(timeoutms);
        }
    }

    virtual void release() override
    {
        if (!_coop)
            Worker::release();
    }

    void setLogLevel(int level)
    {
        if (level >= 0 && level < 100)
        {  
            Logged::_logLevel = level;
            LOG2("IFIClient, setting log level to ", level);
        }
    }

    void handleOptions(int argc, char** argv)
    {
        Logged initLog;
        
        for (char**& ap = ++argv; *ap; ++ap)
        {
            char* arg;
            if ((arg = Opt::nextOptArg(ap, "-d")) != 0)
                setLogLevel(atoi(arg));
        }
    }

    /////////////////////// Client thread

    bool workHandler() override
    {
        // on client thread

        // once main returns (eg shutdown), we're done
        client_main(_argc, _argv);
        return false; 
    }

    virtual int client_main(int argc, char** argv);

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

            // only block if we didn't fill from `getRequest`
            if (_madeRequest) break;
            if (_coop) break; // do not block
            waitForSignal();
        }

        if (!c)
        {
            if (!_coop) c = '\n';  // !c remains for coop
            if (_shutdown) c = EOF;

            // subsequent calls to getchar will block unless getRequest
            _madeRequest = false;
        }
        return c;
    }

    const char* getRequest()
    {
        // returns pointer to our `inBuffer (when ready).
        const char* r = 0;

        _madeRequest = true;

        // coop version does not block, but can return 0
        if (!_coop)
        {
            while (!_inBufferReady && !_shutdown)
            {
                waitForSignal();
            }
        }

        if (_shutdown) return 0;
        
        assert(_coop || _inBufferReady);

        if (_inBufferReady)
        {
            r = _inBuffer.start();

            // buffer has been used now
            _inBufferReady = false;
        }

        assert(_coop || r);
        
        return r;
    }

    void flush()
    {
        assert(_emitter);
        if (_outBuffer.size())
        {
            emitSingleResponse(IFI_TEXT, _outBuffer.start());
            _outBuffer.clear();
        }
    }

    int putchar(int c)
    {
        if (c == '\b')
        {
            _outBuffer.unadd();
        }
        else
        {
            bool f = c == 0 || c == -1; // flush
            if (!f)
            {
                f = (c == '\n');
                _outBuffer.add(c);
            }

            if (f && _outBuffer.size())
            {
                _outBuffer.add(0);
                flush();
            }
            
        }
        
        return c;
    }

    void putstring(const char* s)
    {
        while (*s) putchar(*s++);
    }

    int vprintf(const char* m, va_list args)
    {
        // buffer used for small strings
        static char buf[256];
        
        va_list a0;
        va_copy(a0, args);
        int n = vsnprintf(buf, sizeof(buf), m, a0);
        va_end(a0);

        char* tbuf = buf;

        if (n >= (int)sizeof(buf))
        {
            // didn't fit
            tbuf = new char[n+1];
            vsprintf(tbuf, m, args);
        }

        putstring(tbuf);

        // if we allocated
        if (tbuf != buf) delete [] tbuf;

        return n;
    }

    int printf(const char* m, ...)
    {
        // buffer used for small strings
        
        va_list args;
        va_start(args, m);
        int n = vprintf(m, args);
        va_end(args);
        return n;
    }
};








