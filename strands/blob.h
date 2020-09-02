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

#include "head.h"

// forward
struct KLLib;

struct Blob: public Head
{
    struct Info
    {
        KLLib*      _lib;

        // print the name of lib before blob `toString`
        bool        _annotateString;
        
        void*       _object;
    };
    
    Blob(void* object, KLLib* lib): Head(type_blob)
    {
        *(Info**)&_n = make(object, lib);
    }

    Blob(Info* i) : Head(type_blob)
    {
        *(Info**)&_n = i;        
    }

    ~Blob() { _purge(); }

    static Info* make(void* object, KLLib* lib)
    {
        assert(lib);
        assert(object);

        Info* i = new Info;
        i->_lib = lib;
        i->_object = object;
        i->_annotateString = true;
        return i;
    }

    Info* info() const { return (Info*)_n; }

    bool annotateString() const
    {
        Info* i = info();
        return !i || i->_annotateString;
    }

    void annotateString(bool v)
    {
        Info* i = info();
        if (i) i->_annotateString = v;
    }

    // convert to sbuf, for string output
    void toSBuf(SBuf& sb) const;
    int compare(const Blob& v) const;

private:

    void _purge();

    // dummy
    Blob(const Blob&) {}
    Blob& operator=(const Blob&) { return *this; }

    void info(Info* i)
    {
        assert(!_n);
        *(Info**)&_n = i;
    }

};


