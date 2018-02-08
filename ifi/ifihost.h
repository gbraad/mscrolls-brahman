
#pragma once

#include <iostream>
#include <string>
#include <deque>
#include "worker.h"

struct IFIHost: public Worker
{
    typedef std::string string;
    typedef std::deque<string>  Queue;

    mutex       _queueLock;
    Queue       _replies;
    
    static void emitterHandler(void* ctx, const char* json)
    {
        ((IFIHost*)ctx)->_emitterHandler(json);
    }

    void _emitterHandler(const char* json)
    {
        //std::cout << json << std::endl;

        {
            std::lock_guard<mutex> lock(_queueLock);
            _replies.emplace_back(json);
        }
        
        signal();
    }

    void handleJSON(const char* json)
    {
        std::cout << "JSON:" << json << std::endl;
    }

    void handleQueue()
    {
        std::lock_guard<mutex> lock(_queueLock);

        while (!_replies.empty())
        {
            handleJSON(_replies.front().c_str());
            _replies.pop_front();
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
