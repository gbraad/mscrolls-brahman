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

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

#ifdef IFI_IMPORT
#define DLL DLLIMPORT
#else
#define DLL DLLEXPORT
#endif
#endif // _WIN32

struct DLL IFIClient: public IFI, public Worker
{
    typedef std::string string;
    
    Emitter             _emitter;

    GrowString          _inBuffer;
    bool                _inBufferReady = false;
    
    GrowString          _cmdBuffer;
    int                 _cmdPos = 0;

    GrowString          _outBuffer;
    bool                _madeRequest = false;

    static IFIClient*   _theIFI;

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
        // called by host to start the client (on host thread)
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

    /////////////////////// Client thread

    bool workHandler() override
    {
        // on client thread
        
        _theIFI = this;

        int argc = 0;
        char* arg1 = 0;
        char** argv = &arg1;

        // 0 => continue, else stop
        return main(argc, argv) ? false : true;
    }

    virtual int main(int argc, char** argv);

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
            JSONWalker::encodeString(gs, _outBuffer.start());
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
    }

    void printf(const char* m, ...)
    {
        // buffer used for small strings
        static char buf[256];
        
        va_list args;
        va_start(args, m);
        int n = vsnprintf(0, 0, m, args);
        va_end(args);

        // if we fit in small buffer, use it, otherwise allocate
        char* tbuf = n < sizeof(buf) ? buf : new char[n+1];

        va_start(args, m);
        vsprintf(tbuf, m, args);
        va_end(args);

        putstring(tbuf);

        // if we allocated
        if (n >= sizeof(buf)) delete [] tbuf;
    }
};








