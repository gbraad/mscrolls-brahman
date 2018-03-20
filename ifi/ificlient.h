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

    // Compliance
    virtual void setEmitter(charEmitFn* e, void* ctx)
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

        _inBuffer = json;
        _inBufferReady = true;

        for (JSONWalker jw(json); jw.nextKey(); jw.next())
        {
            bool isObject;
            var v = jw.getValue(isObject);

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
        
        return r;
    }
    
    virtual bool start(int argc, char** argv) override
    {
        _argc = argc;
        _argv = argv;
        
        // called by host to start the client (on host thread)
        handleOptions(argc, argv);
        
        Opt::rebuildArgs(_argc, _argv);
        
        return Worker::start();
    }

    virtual int sync(int timeoutms = 0) override
    {
        return Worker::sync(timeoutms);
    }

    virtual void release() override
    {
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
            bool f = c <= 0; // flush
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








