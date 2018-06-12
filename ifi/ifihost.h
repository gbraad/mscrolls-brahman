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

#include <iostream>
#include <string>
#include <deque>
#include <mutex>
#include "jsonwalker.h"
#include "logged.h"
#include "ifischema.h"
#include "ifihandler.h"

struct IFIHandler;

struct IFIHost
{
    typedef std::mutex mutex;
    
    typedef std::string string;
    typedef std::deque<char*>  Queue;

    mutex       _queueLock;
    Queue       _replies;
    IFIHandler* _handler = 0;

    virtual ~IFIHost() {}

    static void emitter(void* ctx, const char* json)
    {
        IFIHost* host = (IFIHost*)ctx;
        assert(host);

        std::lock_guard<mutex> lock(host->_queueLock);
        host->_replies.push_back(strdup(json));
    }
    
    void setHandler(IFIHandler* h)
    {
        _handler = h;
    }

    void drainQueue()
    {
        // NB: client thread can still be running when this is called
        // during a sync timeout

        std::unique_lock<mutex> lock(_queueLock, std::defer_lock);

        for (;;)
        {
            char* json = 0;
            
            lock.lock();
            
            if (!_replies.empty())
            {
                json = _replies.front();
                _replies.pop_front();
            }

            lock.unlock();

            if (!json) break;

            if (_handler)
            {
                _handler->handle(json);
            }

            delete [] json;
        }
    }

    bool sync(IFI* ifi, int timeoutms = 200)
    {
        int v;

        // sync client and drain queue of any replies
        do
        {
            v = ifi->sync(timeoutms);

            // perform work if sync ok or timeout yield
            if (v >= 0) drainQueue();

        } while(!v); // try again while timeout

        return v > 0;
    }
    
};
