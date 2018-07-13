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
#include "logged.h"

struct Worker
{
    typedef std::thread thread;
    typedef std::mutex mutex;
    typedef std::condition_variable condition_variable;
    
    thread              _task;
    bool                _running = false;

    // between start and running
    bool                _starting;

    bool                _shutdown = false;

    mutex               _waitLock;
    bool                _ready = false;
    bool                _waiting = false;
    condition_variable  _waiter;

    virtual ~Worker() { stop(); }

    bool start()
    {
        // interval between starting thread and it running
        _starting = true;
        _task = std::thread(&Worker::runTask, this);
        return true;
    }

    void stop()
    {
        if (_running)
        {
            LOG4("worker, ", "stopping");
            _shutdown = true;
        }
        _task.join();
    }

    void waitForSignal()
    {
        // call in task to block on a signal
        std::unique_lock<mutex> lock(_waitLock);
        
        _waiting = true;
        
        while (!_ready && !_shutdown)
            _waiter.wait_for(lock, std::chrono::milliseconds(500));

        _ready = false;
        _waiting = false;
    }

    void signal()
    {
        // call from outside to signal event
        _ready = true;

        // wake up.
        _waiter.notify_one();
    }

    int sync(int timeoutms = 0)
    {
        // call from outside to wait for task to block on signal
        // if true, caller MUST call `release`

        // return -1 if cannot sync
        // return 0 if timeout yield
        // return 1 if sync ok

        int dt = 20; // 20 millisecond chunks
        int cc = 0;
        int ccmax = timeoutms/dt;
        for (;;)
        {
            _waitLock.lock();
            
            if (_waiting && !_ready)
            {
                break;
            }
            else
            {
                _waitLock.unlock();

                if (!_running && !_starting) return -1; 
                if (ccmax && ++cc >= ccmax)
                {
                    LOG3("Worker, timeout ", cc*dt);
                    return 0;
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(dt));
            }
        }
        return 1;
    }

    void release()
    {
        _waitLock.unlock();
    }

    virtual bool workHandler() { return false; }

protected:
    
    void runTask()
    {
        _running = true;
        _starting = false; // we've started now we're running

        do
        {
            _running = !_shutdown && workHandler();
            
        } while (_running);

        LOG4("Worker, ", "finished");

    }
};


    
