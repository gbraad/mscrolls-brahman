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
 *  Copyright (©) Voidware 2016-2017.
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
 *  contact@voidware.com
 *
 */

#pragma once

//GLK engine


#include <QObject>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <setjmp.h>
#include <vector>
#include <ap.h>

#include "ifengine.h"
#include "logged.h"
#include "braschema.h"
#include "growbuf.h"

// forward
struct event_struct;
struct VarSet;

DECLARE_IFENGINE

class IFGLK : public QObject, public IFEngineInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IFENGINE_IID);
    Q_INTERFACES(IFEngineInterface);

    typedef std::thread thread;
    typedef std::mutex mutex;
    typedef std::condition_variable condition_variable;

    struct CommandResult: public CommandResultI
    {
        string  _result;
        const char* toString() const override { return _result.c_str(); }
    };
    
    
    thread              _msTask;
    bool                _running;
    bool                _shutdown;

    bool                _createImageDir;
    bool                _addCommandNewline;

    string              _story;
    string              _configDir;
    string              _dataDir;
    string              _imageDir;
    charEmitFn*         _transcriptEmit;
    void*               _tctx;
    charEmitFn*         _consoleEmit;
    void*               _cctx;
    segmentInfoFn*      _segmentInfoEmit;
    void*               _sictx;

    mutex               _syncLock;
    condition_variable  _waiter;
    bool                _waiting;
    bool                _inputReady;
    
    char                _imageNameBuf[128];

public:

    struct Window
    {
        char*   _buf;
        size_t  _bufsz;
        size_t  _pos;
        string  _name;
        Window* _redirect;

        Window() { _init(); }
        virtual ~Window() { _purge(); }

        void setSize(size_t sz)
        {
            _purge();
            _bufsz = sz + 1;
            _buf = new char[_bufsz];
        }

        void putchar(char c)
        {
            if (!_buf) return;
            
            if (c == '\b')
            {
                if (_pos) _buf[--_pos] = 0;
            }
            else if (_pos < _bufsz-1)
            {
                _buf[_pos++] = c;
                _buf[_pos] = 0;
            }
        }

        void tidyTranscript()
        {
            if (_buf)
            {
                char* p = _buf;
                char* q = _buf + _pos;

                // remove any spaces on end
                while (p != q && q[-1] == ' ') { *--q = 0; --_pos; }

                // remove any prompt on end
                if (p != q && q[-1] == '>')
                {
                    *--q = 0;
                    --_pos;
                    if (p != q && q[-1] == '\n') { *--q = 0; --_pos; }
                }
            }
        }

        void clear()
        {
            if (_buf)
            {
                _pos = 0;
                _buf[0] = 0;
            }
        }

        void _init()
        {
            _buf = 0;
            _bufsz = 0;
            _pos = 0;
            _redirect = 0;
        }

        void _purge()
        {
            delete _buf;
            _init();
        }
    };

    typedef std::vector<AP<Window> > Windows;


    jmp_buf             _jmpExit;
    char*               _inbuf;
    size_t              _inbufsz;
    Windows             _windows;
    Window*             _outputWindow;
    Window*             _inputWindow;
    Window*             _invWindow;

    IFGLK() { _init(); }

    ~IFGLK()
    {
        stop(); 
        
        delete _invWindow;
    }

    // compliance
    bool start(const char* configDir,
               const char* dataDir,
               const char* story,
               charEmitFn* transcriptEmit, void* tctx,
               charEmitFn* consoleEmit, void* cctx,
               segmentInfoFn* sfn, void* sctx) override;

    void emitScene() override 
    {
        LOG4("GLK, ", "emit scene");
        _sync();
        flush();
        _syncLock.unlock();
    }

    static void _emits(charEmitFn* emitter, void* ctx, const char* s)
    {
        while (*s) (*emitter)(ctx, *s++);
    }

    void promptConsole() override 
    {
        static const char normal_prompt[] = "> ";
        _emits(_consoleEmit, _cctx, normal_prompt);
    }

    bool  evalUseXwithY(const string& x, const string& y,
                               CommandResultI* res = 0) override
    {
        LOG3("GLK, use \"", x << "\" with \"" << y << "\"");
        string cmd = "hang " + x + " on " + y;
        return evalCommand(cmd.c_str());
    }

    bool setOptions(const VarSet&) override;

    void addWindow(Window* w)
    {
        // consume w
        _windows.push_back(w);
    }

    void setWindow(Window* w)
    {
        // set both
        _outputWindow = w;
        _inputWindow = w;
    }

    void removeWindow(Window* w)
    {
        // NB: deletes w if found
        for (Windows::iterator it = _windows.begin(); 
             it != _windows.end(); ++it)
        {
            Window* wi = it->get();
            if (wi == w)
            {
                if (wi == _outputWindow) _outputWindow = 0;
                if (wi == _inputWindow) _inputWindow = 0;
                _windows.erase(it);
                break;
            }
        }
    }

    CommandResultI* makeResult() override
    {
        return new CommandResult();
    }

    void clearImage() override
    {
        //if (!_imageNameBuf[0]) strcat(_imageNameBuf, " ");
    }

    void flushImage()
    {
        if (_imageNameBuf[0])
        {
            // there's an image waiting
            (_segmentInfoEmit)(_sictx, BRA_SEGMENT_IMAGE);

            _emits(_transcriptEmit, _tctx, _imageNameBuf);
            (*_transcriptEmit)(_tctx, 0);
            
            (_segmentInfoEmit)(_sictx, 0);

            // clear
            _imageNameBuf[0] = 0;
        }
    }
    
    void flush()
    {
        Window* w = _outputWindow;
        if (w)
        {
            w->tidyTranscript();
        
            if (w->_pos)
            {
                _emits(_transcriptEmit, _tctx, w->_buf);
                w->clear();
            
                // end of section
                (*_transcriptEmit)(_tctx, 0);
            }
        }
        
        flushImage();
    }

    bool _sync()
    {
        // wait until emulator quiet, waiting on input
        for (;;)
        {
            _syncLock.lock();
            
            if (_waiting && !_inputReady)
            {
                break;
            }
            else
            {
                _syncLock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }
        return true;
    }

    bool _evalCommand(const char* cmd, CommandResultI* res = 0);
    bool evalCommandSpecial(const char* cmd, CommandResultI* cres);

    bool evalCommand(const char* cmd,
                     CommandResultI* cres = 0,
                     bool echo = false) override
    {
        bool res = evalCommandSpecial(cmd, cres);
        if (!res)
        {
            _sync();
            res = _evalCommand(cmd, cres);
        }
        return res;
    }

    void setLogLevel(int level)
    {
        if (level >= 0 && level < 100)
        {
            Logged::_logLevel = level;
            LOG2("GLK, log level ", level);
        }
    }

    bool getProductInfo(ProductInfo&) override;

    void stop()
    {
        LOG1("GLK, ", "stopping");
        _shutdown = true;
        _msTask.join();
    }

    void _init()
    {
        Logged initLog;

        _running = false;
        _shutdown = false;
        _waiting = false;
        _inputReady = false;

        _createImageDir = false;
        _addCommandNewline = false;
        
        _inbufsz = 0;
        _imageNameBuf[0] = 0;

        _outputWindow = 0;
        _inputWindow = 0;

        // make a window to hold inventory text
        _invWindow = new Window;
        _invWindow->setSize(1024);
        _invWindow->_name = "inventory";
    }

    void runMsTask();

    void waitForInput()
    {
        std::unique_lock<mutex> lock(_syncLock);
        
        _waiting = true;

        LOG4("GLK, ", "waiting for input");

        while (!_inputReady && !_shutdown)
        {
            _waiter.wait_for(lock,
                             std::chrono::milliseconds(200));
        }

        _inputReady = false;
        _waiting = false;

    }

    // task methods
    void _putchar(char c)
    {
        // NB: ms thread
        Window* w = _outputWindow;

        if (w)
        {
            if (w->_redirect) w = w->_redirect;
            w->putchar(c);
        }
    }

    void _puts(const char* s)
    {
        // NB: ms thread

        Window* w = _outputWindow;
        if (w)
        {
            if (w->_redirect) w = w->_redirect;            
            if (!w->_name.empty())
            {
                LOG4("GLK, emit to ", w->_name << ": " << s);
            }
            while (*s) w->putchar(*s++);
        }
    }

    void bogusInventory(const char* buf, CommandResult* cres);

    void _showpic(int picn, int mode);
    void select(event_struct* ev);

private:

    void _buildProductInfoJSON(GrowString& buf);
};
