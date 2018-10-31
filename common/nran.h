/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2018
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
 *  contact@voidware.com
 */

#pragma once

#include <assert.h>
#include "rand.h"

#define CACHE_SIZE      1023

struct NRan
{
    // Generate "non-random" numbers.

    typedef unsigned int ID;

    struct State
    {
        NRan*           _host;
        ID              _id;
        unsigned int    _n;
        unsigned int    _maxp;
        State*          _next;
        unsigned int*   _stats;

        State(NRan* host, unsigned int n, unsigned int maxp, ID id)
            : _host(host)
        {
            _init(n, maxp, id);
        }

        ~State() { _purge(); }

        unsigned int nran()
        {
            unsigned int v;

            // look for forced omission
            unsigned int i;
            unsigned int imin = 0;
            for (i = 1; i < _n; ++i)
                if (_stats[i] > _stats[imin]) imin = i;
        
            if (_stats[imin] + 1 >= _maxp)
            {
                // forced
                v = imin;
            }
            else
            {
                // otherwise choose randomly
                assert(_host);
                v = _host->_ran->gen32() % _n;
            }

            // adjust stats
            for (i = 0; i < _n; ++i) ++_stats[i];
            _stats[v] = 0;
        
            return v;
        }

        void reset(unsigned int n, unsigned int maxp, ID id)
        {
            assert(_id == id);
            _purge();
            _init(n, maxp, id);
        }

        void _init(unsigned int n, unsigned int maxp, ID id)
        {
            assert(n);
            
            _id = id;
            _n = n;
            _maxp = maxp;
            _next = 0;
            _stats = new unsigned int[_n];
            memset(_stats, 0, sizeof(unsigned int)*_n);
        }

        void warmup()
        {
            // stir the pot
            for (unsigned int i = 0; i < _n; ++i) nran();
        }

        void _purge()
        {
            delete [] _stats;
        }

    };

    // Constructors
    NRan(Ranq1* r)
    {
        _ran = r;
        _init(); 
    }

    ~NRan() { _purge(); }
    
    unsigned int ran(unsigned int n, unsigned int maxp, ID id)
    {
        if (!n) return 0;

        assert(_ran);
        State* s = _intern(n, maxp, id);
        assert(s);
        return s->nran();
    }

private:

    void _init()
    {
        memset(_cache, 0, sizeof(State*)*CACHE_SIZE);
    }

    State* _intern(unsigned int n, unsigned int maxp, ID id)
    {
        unsigned int h = id % CACHE_SIZE;
        State* s = _cache[h];
        while (s && s->_id != id) s = s->_next;
        if (s)
        {
            if (s->_n != n || s->_maxp != maxp)
            {
                // being used with a different system
                s->reset(n, maxp, id);
                s->warmup();
            }
        }
        else
        {
            // add a new state
            s = new State(this, n, maxp, id);
            s->_next = _cache[h];
            _cache[h] = s;

            s->warmup();
        }
        return s;
    }

    void _purge()
    {
        for (int i = 0; i < CACHE_SIZE; ++i)
        {
            State* s = _cache[i];
            while (s)
            {
                State* n = s->_next;
                delete s;
                s = n;
            }
        }
    }
    
    Ranq1*       _ran;
    State*       _cache[CACHE_SIZE];
};
