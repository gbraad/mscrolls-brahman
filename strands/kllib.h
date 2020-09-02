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

#include "kl.h"
#include "blob.h"

struct KLLib: public HeadTraits
{
    typedef std::string string;
    
    KL*     _host;

    KLLib(KL* host) : _host(host) {}
    
    StdStream&  out() { return *_host->_out; }
    bool        primNoEval() const { return _host->_primNoEval; }
    Term        eval(const Term& term, Env& env, int opt = 0)
    {
        return _host->eval(term, env, opt);
    }

    virtual const char* libname() const = 0;
    virtual void destroy(Blob*) = 0;
    virtual string asString(const Blob*) const = 0;
    virtual int compare(const Blob::Info* a, const Blob::Info* b) const
    {
        return (int)((const char*)a->_object - (const char*)b->_object);
    }
        
    bool        check(const Blob& b) const
    {
        // make sure this is blob is ours
        bool r = false;
        Blob::Info* i = b.info();
        if (i)
        {
            assert(i->_lib);
            const char* blibname = i->_lib->libname();
            r = blibname == libname();
        
            if (!r)
            {
                LOG1("Wrong blob type ", blibname << " expected " << libname());
            }
        }
        else
        {
            LOG1("Null Blob ", libname());
        }
        return r;
    }
};

