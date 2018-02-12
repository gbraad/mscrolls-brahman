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

#include <iostream>
#include <string>
#include <deque>
#include "worker.h"
#include "jsonwalker.h"
#include "ifischema.h"

struct IFIHost: public Worker
{
    typedef std::string string;
    typedef std::deque<char*>  Queue;

    mutex       _queueLock;
    Queue       _replies;
    
    void emitterHandler(const char* json)
    {
        {
            std::lock_guard<mutex> lock(_queueLock);
            _replies.push_back(strdup(json));
        }
        
        signal();
    }

    void handleJSON(const char* json)
    {
        //std::cout << "JSON:" << json << std::endl;

        for (JSONWalker jw(json); jw.nextKey(); jw.next())
        {
            bool isObject;
            var v = jw.getValue(isObject);

            if (v)
            {
                if (jw._key == IFI_TEXT) std::cout << v << std::endl;
            }
        }
    }

    void handleQueue()
    {
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

            handleJSON(json);

            delete [] json;
        }
    }

    virtual bool workHandler() override
    {
        do
        {
            waitForSignal();
            handleQueue();
            
        } while (!_shutdown);
        
        return false;
    }
    
};
