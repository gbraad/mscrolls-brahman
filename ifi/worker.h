
#pragma once

#include <assert.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

struct Worker
{
    typedef std::thread thread;
    typedef std::mutex mutex;
    typedef std::condition_variable condition_variable;
    
    thread              _task;
    bool                _running = false;
    bool                _starting = false;
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
            _shutdown = true;
            _task.join();
        }
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

    bool sync(int timeoutms = 0)
    {
        // call from outside to wait for task to block on signal
        // if true, caller MUST call `release`

        //TimeReport syncm("sync");
        int cc1 = 0, cc2 = 0;
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

                if (!_running && !_starting) return false;
            
                if (++cc1 >= 25)
                {
                    ++cc2;
                    //LOG3("_sync, too long ", cc2*500);
                    cc1 = 0;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }
        return true;
    }

    void release()
    {
        _waitLock.unlock();
    }

    virtual bool workHandler()
    {
        return false;
    }

protected:
    
    void runTask()
    {
        _running = true;
        _starting = false; // we've started now we're running

        do
        {
            _running = !_shutdown && workHandler();
            
        } while (_running);
    }
};


    
