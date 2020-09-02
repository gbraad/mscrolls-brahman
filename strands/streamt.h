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
 *  Copyright (c) Strand Games 2020.
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

#include "term.h"
#include "sfstream.h"
#include "logged.h"

struct Streamt: public Head
{
    Streamt() : Head(type_stream) 
    {
        _n = 0;
    }

    Streamt(const char* filename) : Head(type_stream) 
    {
        _n = 0;
        if (filename)
        {
            StdFStream* fs = new StdFStream();
            fs->open(filename);
            ss(fs);
        }
        else
        {
            ss(new StdOStream);
        }
    }

    explicit Streamt(StdStream* ss)
    {
        consume(ss);
    }

    ~Streamt() { _purge(); }
    
    bool        ok() const
    {
        StdStream* s = ss();
        return s && s->ok();
    }

    StdStream&  operator*() const
    {
        assert(ok());
        return *ss();
    }

    void        flush()
    {
        assert(ok());
        ss()->flush(); 
    }

    int compare(const Streamt& v) const
    {
        StdStream* s1 = ss();
        StdStream* s2 = v.ss();
        return (uintptr_t)s1 - (uintptr_t)s2;
    }

    StdStream* donate()
    {
        StdStream* s = ss();
        _n = 0;
        return s;
    }

    void consume(StdStream* s)
    {
        assert(!_n);
        ss(s);
    }

    // convert to sbuf, for string output
    void toSBuf(SBuf& sb) const
    {
        //Info* i = info();
        sb.add("<Stream: ");
        if (ok()) sb.add("open");
        else sb.add("closed");
        sb.add(">");
    }

    void fold(SBuf& sb) const {}
    void unfold(SBuf& sb)  {}

    static Term parse(Stream& s)
    {
        return Term();
    }

protected:

    StdStream* ss() const { return (StdStream*)_n; }

    void ss(StdStream* s)
    {
        assert(!_n);
        *(StdStream**)&_n = s;
    }

    /*
      Info* info() const { return (Info*)_n; }

      void info(Info* i)
      {
      assert(!_n);
      *(Info**)&_n = i; 
      }
    */

    void _purge()
    {
        delete ss();
        _n = 0;
    }

};


