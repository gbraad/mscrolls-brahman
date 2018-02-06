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
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "ifihelper.h"

struct IFIClient: public IFI
{
    typedef std::string string;
    typedef std::thread thread;
    typedef std::mutex mutex;
    typedef std::condition_variable condition_variable;

    typedef bool mainLoopFn(void* ctx);

    IFIIOHelper         _ifiHelper;
    emitterFn*          _upEmitter = 0;

    thread              _task;
    bool                _running = false;
    bool                _starting = false;
    mainLoopFn*         _mainLoop = 0;
    void*               _mainCtx = 0;

    GrowString          _inBuffer;
    int                 _inPos = 0;
    
    bool                _inputReady = false;
    bool                _waiting = false;
    bool                _shutdown = false;
    mutex               _syncLock;
    condition_variable  _waiter;

    // Compliance
    virtual void setEmitter(emitterFn* emit, void* ctx) override
    {
        bool begin = !_upEmitter;
        _upEmitter = emit;
        if (begin) _ifiHelper.start(this, emit);
    }
    
    virtual bool eval(const char* json) override;
    virtual bool start() override;
    virtual bool sync(int timeoutms = 0) override;
    virtual void release() override;


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

    void runTask()
    {
        _running = true;
        _starting = false; // we've started now we're runnning

        assert(_mainLoop);

        //LOG3("MS ", "running");    

        while (_running)
        {
            _running = !_shutdown && (*_mainLoop)(_mainCtx);
        }
        
        //LOG3("MS ", "shutdown");
    }

    void stop()
    {
        //LOG3("MS ", "stopping");
        _shutdown = true;
        _task.join();
    }
    
    bool evalCommand(const string& s)
    {
        if (!sync()) return false;
        
        // fill input buffer
        _inBuffer = s.c_str();
        _inBuffer.add(0); // signal end of input
        _inPos = 0;

        _inputReady = true;

        // wake it up.
        _waiter.notify_one();

        release();
        
        return true;
    }

    //////////////////////////// Client Helpers
    
    void waitForInput()
    {
        std::unique_lock<mutex> lock(_syncLock);
        
        _waiting = true;
        
        //LOG4("MS, ", "waiting for input");

        while (!_inputReady && !_shutdown)
            _waiter.wait_for(lock, std::chrono::milliseconds(500));

        _inputReady = false;
        _waiting = false;
    }

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

            waitForInput();
        }
        
        return c;
    }
    
};








