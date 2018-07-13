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

#include <assert.h>
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "strutils.h"
#include "logged.h"
#include "ap.h"

struct ObjTraits
{
    typedef std::string string;
};

struct Object: public ObjTraits
{

    // symbolic or numeric unique identity
    string  _id;

    // parseable name
    string  _name;

    // optional icon filepath
    string  _icon;

    Object(const string& id) : _id(id) {}

    bool valid() const { return !_id.empty() && !_name.empty(); }

    bool matchWord(const string& s) const
    {
        // if `s` a match for the main word
        return equalsIgnoreCase(s, lastWordOf(_name));
    }

    bool matchAdj(const string& word, const string& adj) const
    {
        bool r = false;
        std::vector<string> words;
        split(words, _name);
        int sz = words.size();
        if (sz && equalsIgnoreCase(words[sz-1], word))
        {
            for (int i = 0; i < sz-1; ++i)
            {
                if (equalsIgnoreCase(adj, words[i]))
                {
                    r = true;
                    break;
                }
            }
        }
        return r;
    }
    
    string adjWord() const
    {
        // first adjective + word
        string a = firstWordOf(_name);
        string w = lastWordOf(_name);
        if (!equalsIgnoreCase(a, w)) w = a + ' ' + w;
        return w;
    }

    bool operator==(const Object& obj) const { return _id == obj._id; }
    bool operator!=(const Object& obj) const { return _id != obj._id; }
    bool operator<(const Object& obj) const { return _id < obj._id; }
};

struct Objects: public std::set<Object>, public ObjTraits
{
    typedef std::set<Object> parentT;
    
    friend std::ostream& operator<<(std::ostream& os, const Objects& o)
    {
        os << '[';
        for (const_iterator it = o.cbegin(); it != o.cend(); ++it)
        {
            const Object& oi = *it;
            os << '(' << oi._id << ',' << oi._name << ')';
        }
        return os << ']';
    }

    const Object* find(const string& id) const
    {
        const Object* r = 0;
        if (!id.empty())
        {
            Object target(id);
            const_iterator it = parentT::find(target);
            if (it != cend()) r = &*it;
        }
        return r;
    }
        
};

struct ObjectList: public std::vector<const Object*>, public ObjTraits
{
    const Objects*      _objects;
    bool                _echo = false;
    
    std::vector<AP<Object> > _pool;

    typedef const Object*       Item;
    
    ObjectList(const Objects& o) : _objects(&o)
    {
        assert(_objects);
    }
    
    bool add(const string& id, bool force = false)
    {
        Item i = _objects->find(id);
        if (i) push_back(i);
        else if (force)
        {
            // we can add an ID even if it doesnt exist
            Object* o = new Object(id);
            _pool.push_back(o);
            push_back(o);
            i = o;
        }
        else
        {
            LOG1("ObjectList, unknown id ", id);
        }
        return i != 0;
    }
};

    
