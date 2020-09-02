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

#include "list.h"

// forward
struct Env;

struct PCall
{
    virtual ~PCall() {}
    virtual Term call(List::iterator& args, Env& env) = 0;
};

template<class Fn, class B> struct PCallFn: public PCall
{
    Fn                  _fn;
    B*                  _base;

    PCallFn() {}
    PCallFn(Fn f, B* b) : _fn(f), _base(b)  {}
    
    // compliance
    Term call(List::iterator& args, Env& env) override
    {
        return (_base->*_fn)(args, env);
    }
};

struct Primitive: public Head
{
    Primitive(PCall& pc) : Head(type_primitive)
    {
        _n = (Node*)&pc;
    }

    PCall*     pcall() { return (PCall*)_n; }

    Term call(List::iterator& args, Env& env)
    {
        assert(_n);
        return pcall()->call(args, env);
    }

    // convert to sbuf
    void toSBuf(SBuf& sb) const
    {
        char buf[16];
        sprintf(buf, "<%08X>", (int)(uintptr_t)_n);
        sb.add(buf);
    }

    int compare(const Primitive& v) const
    {
        return (int)(_n - v._n);
    }
    
};
