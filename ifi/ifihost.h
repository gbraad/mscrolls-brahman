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

    IFI*        _ifi = 0;
    mutex       _queueLock;
    Queue       _replies;
    IFIHandler* _handler = 0;
    bool        _inSync = false;
    bool        _more;
    bool        _combineReplies = true;

    virtual ~IFIHost() {}

    static void emitter(void* ctx, const char* json)
    {
        IFIHost* host = (IFIHost*)ctx;
        assert(host);

        char* p = u_strdup(json);

        std::lock_guard<mutex> lock(host->_queueLock);
        host->_replies.push_back(p);
    }
    
    void setHandler(IFIHandler* h) { _handler = h; }
    void setIFI(IFI* ifi) { _ifi = ifi; }

    bool isSimpleText(const char* js, string& s)
    {
        // if json `js` is just one text record, return the string
        // NB: does not detect text objects
        int nr = 0;
        int nt = 0;
        for (JSONWalker jw(js); jw.nextKey(); jw.next())
        {
            bool isObject;
            const char* st = jw.checkValue(isObject);
            if (!st) break; // bad json
            ++nr;

            if (nr > 1) break;

            if (!isObject && jw._key == IFI_TEXT)
            {
                // do not decode string
                s = jw.collectRawStringValue(st);
                ++nt;
            }
        }

        // just one text record? 
        return nr == 1 && nt == 1;
    }
    
    void drainQueue()
    {
        // NB: client thread can still be running when this is called
        // during a sync timeout

        std::unique_lock<mutex> lock(_queueLock, std::defer_lock);

        const int repMax = 64;
        char* rep[repMax];
        bool done = false;

        do
        {
            int n = 0;

            lock.lock();
            
            while (n < repMax)
            {
                done = _replies.empty();
                if (done) break;

                rep[n++] = _replies.front();
                _replies.pop_front();
                
            }
            
            lock.unlock();

            // combined text record, if used
            GrowString cjs;
            int len = 0;
                
            if (n > 1 && _combineReplies)
            {
                cjs.append("{\"" IFI_TEXT "\":\"");
                for (int i = 0; i < n; ++i)
                {
                    string t;
                    if (isSimpleText(rep[i], t))
                    {
                        // combine text and discard
                        len += t.size();
                        cjs.append(t);

                        delete [] rep[i];
                        rep[i] = 0;
                    }
                }

                cjs.append("\"}");
                cjs.add(0);
            }

            // handle replies
            for (int i = 0; i < n; ++i)
            {
                char* r = rep[i];
                if (r)
                {
                    if (_handler) _handler->handle(r);
                    delete [] r;
                }
                else
                {
                    // a missing record must have been a text record that
                    // we combined
                    // emit the combined text here, as if it all occured at
                    // the same point
                    if (len > 0)
                    {
                        if (_handler) _handler->handle(cjs.start());
                        len = 0; // only once!
                    }
                }
            }
        } while (!done);
    }

    bool sync(IFI* ifi, int timeoutms = 200)
    {
        int v;

        assert(!_inSync);

        _more = false;

        // sync client and drain queue of any replies
        do
        {
            v = ifi->sync(timeoutms);

            if (v > 0) _inSync = true;

            // perform work if sync ok or timeout yield
            if (v >= 0) drainQueue();

        } while(!v); // try again while timeout

        return v > 0;
    }

    bool sync()
    {
        return sync(_ifi);
    }

    bool release()
    {
        // return true if more to do
        if (_inSync)
        {
            _inSync = false;
            _ifi->release();
            return _more;
        }
        return false;
    }

    bool _syncRelease()
    {
        // sync and release, but only if not already in sync
        if (!_inSync)
        {
            sync();
            return release();
        }
        return false;
    }

    bool syncRelease()
    {
        // sync and release, but only if not already in sync
        // continue until no more.
        bool r = true;
        if (!_inSync)
        {
            for (;;)
            {
                r = sync();
                if (!r) break;
                if (!release()) break;
            }
        }
        return r;
    }

    bool eval(const char* s)
    {
        bool r;

        if (_inSync)
        {
            r = _ifi->eval(s);

            // will need another syncrelease *after* current drain
            // eg "restore/load" from command line
            if (r)
            {
                _more = true;
                //LOG3("eval more needed", "");
            }
        }
        else
        {
            r = sync();
            if (r)
            {
                r = _ifi->eval(s);
                release();
            }
        }
        return r;
    }
    
};
