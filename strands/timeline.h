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

#include <string>
#include "var.h"
#include "utils.h"

namespace ST
{

struct Timeline
{
    typedef std::string     string;
    typedef std::vector<const string*>  Strings;
    typedef std::vector<const var*> Vars;

    struct Ent
    {
        string          _tag;
        string          _prop;
        var             _val;
        bool            _neg;

        Ent(const string& tag, bool neg) : _tag(tag), _neg(neg) {}
        Ent(const string& tag, const string& prop, bool neg)
            : _tag(tag), _prop(prop), _neg(neg) {}
        Ent(const string& tag, const string& prop, const var& v, bool neg)
            : _tag(tag), _prop(prop), _val(v), _neg(neg) {}
    };

    typedef std::list<Ent>      Ents;
    Ents                        _ents;

    // boolean values

    const Ent* find(const string& tag) const
    {
        // scan backwards to find (tag...) or ~(tag...)
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();
        while (it != en)
        {
            const Ent& e = *it;
            if (e._tag == tag) return &e;
            ++it;
        }
        return 0;
    }

    bool test(const string& tag) const
    {
        // scan backwards to find (tag...) or ~(tag...)
        // true iff (tag...)

        const Ent* e = find(tag);
        return e ? !e->_neg : false;
    }

    void set(const string& tag) { if (!test(tag)) _append(tag); }
    void clear(const string& tag) { if (test(tag)) _append(tag, true); }

    // single property
    
    const Ent* find(const string& tag, const char* prop) const
    {
        // scan backwards to find (tag,prop...) or ~(tag,prop...)
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();
        while (it != en)
        {
            const Ent& e = *it;
            if (e._tag == tag && !strcmp(prop, e._prop.c_str())) return &e;
            ++it;
        }
        return 0;
    }
    
    const Ent* find(const string& tag, const string prop) const
    { return find(tag, prop.c_str()); }

    bool test(const string& tag, const string prop) const
    {
        // scan backwards to find (tag,prop...) or ~(tag,prop...)
        // true iff (tag,prop...)

        const Ent* e = find(tag, prop);
        return e ? !e->_neg : false;
    }

    void set(const string& tag, const string& prop)
    { if (!test(tag, prop)) _append(tag, prop); }
    
    void clear(const string& tag, const string& prop)
    { if (test(tag, prop)) _append(tag, prop, true); }
    

    // multivalues
    
    const Ent* find(const string& tag, const string prop, const var& v) const
    {
        // scan backwards to find (tag,prop,v) or ~(tag,prop,v)
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();
        while (it != en)
        {
            const Ent& e = *it;
            if (e._tag == tag && e._prop == prop && e._val == v) return &e;
            ++it;
        }
        return 0;
    }

    bool test(const string& tag, const string prop, const var& v) const
    {
        const Ent* e = find(tag, prop, v);
        return e ? !e->_neg : false;
    }

    void set(const string& tag, const string& prop, const var& v)
    { if (!test(tag, prop, v)) _append(tag, prop, v); }
    
    void clear(const string& tag, const string& prop, const var& v)
    { if (test(tag, prop, v)) _append(tag, prop, v, true); }

    
    // functional values

    const var* getfn(const string& tag, const char* prop) const
    {
        //Scan backwards to locate (x,rel,Y) or ~(x,rel,Y) for any Y.
        //  if (x,rel,Y) return Y otherwise null.
        const Ent* e = find(tag, prop);
        return (e && !e->_neg) ? &e->_val : 0;
    }

    const var* getfn(const string& tag, const string& prop) const
    { return getfn(tag, prop.c_str()); }
    
    bool setfn(const string& tag, const string& prop, const var& v)
    {
        // scan backwards to find (tag, prop...) or ~(tag, prop...)
        // treating fn(tag,prop)=Y, if no Y or Y != v, then we must
        // clear Y and add v.
        // after this getfn(tag,prop)==v
        //
        // return true if set, false if already true
        
        bool r = true;
        const Ent* e = find(tag, prop);
        if (e)
        {
            if (e->_val == v) r = false;
            else
            {
                _append(tag, prop, e->_val, true); // neg
                _append(tag, prop, v);
            }
        }
        else
        {
            _append(tag, prop, v);
        }
        return r;
    }

    int getset(const string& tag, const string& prop, Vars& all)
    {
        // collect all Y where (tag,prop,Y)
        // not including those negated.
        
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();

        Vars negs;
        
        while (it != en)
        {
            const Ent& e = *it;
            if (e._tag == tag && e._prop == prop)
            {
                if (e._neg)
                {
                    // keep a list of negged props
                    negs.push_back(&e._val);
                }
                else
                {
                    bool ok = true;
                    for (auto i : negs) if (*i == e._val) { ok = false; break; }
                    if (ok)
                    {
                        all.push_back(&e._val);
                    }
                }
            }
            ++it;
        }
        return all.size();
    }

    int getdsv(const string& prop, const var& v, Strings& tags)
    {
        // collect all X where (X,prop,v)
        // not including those negated.
        
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();

        Strings negs;
        
        while (it != en)
        {
            const Ent& e = *it;
            if (e._prop == prop && e._val == v)
            {
                if (e._neg)
                {
                    negs.push_back(&e._tag);
                }
                else
                {
                    bool ok = true;
                    for (auto i : negs) if (*i == e._tag) { ok = false; break; }
                    if (ok)
                    {
                        tags.push_back(&e._tag);
                    }                    
                }
            }
            ++it;
        }
        return tags.size();
    }

    int getds(const string& prop, Strings& tags)
    {
        // collect all X where (X,v)
        // not including those negated.
        
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();

        Strings negs;
        
        while (it != en)
        {
            const Ent& e = *it;
            if (e._prop == prop)
            {
                if (e._neg)
                {
                    negs.push_back(&e._tag);
                }
                else
                {
                    bool ok = true;
                    for (auto i : negs) if (*i == e._tag) { ok = false; break; }
                    if (ok)
                    {
                        tags.push_back(&e._tag);
                    }                    
                }
            }
            ++it;
        }
        return tags.size();
    }

private:

    void _append(const string& tag, bool neg = false)
    {
        // ASSUME we're ok to just append a new element
        _ents.emplace_back(Ent(tag, neg));
    }

    void _append(const string& tag, const string& prop, bool neg = false)
    {
        // ASSUME we're ok to just append a new element
        _ents.emplace_back(Ent(tag, prop, neg));
    }

    void _append(const string& tag, const string& prop, const var& v,
                 bool neg = false)
    {
        // ASSUME we're ok to just append a new element
        _ents.emplace_back(Ent(tag, prop, v.copy(), neg));
    }
    
};

}; // ST