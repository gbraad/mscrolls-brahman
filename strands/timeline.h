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

#include <iostream>
#include <string>
#include "var.h"
#include "pbase.h"
#include "utils.h"
#include "logged.h"

namespace ST
{

struct Timeline
{
    typedef std::string string;
    typedef std::vector<const string*>  Strings;
    typedef std::vector<const var*> Vars;

    struct Ent
    {
        string          _tag;
        string          _prop;
        var             _val;
        bool            _neg;

        Ent() {} // dummy
        Ent(const string& tag, bool neg) : _tag(tag), _neg(neg) {}
        Ent(const string& tag, const string& prop, bool neg)
            : _tag(tag), _prop(prop), _neg(neg) {}
        Ent(const string& tag, const string& prop, const var& v, bool neg)
            : _tag(tag), _prop(prop), _val(v), _neg(neg) {}

        bool sameTrio(const Ent& e) const
        {
            // same apart from neg
            return _tag == e._tag && _prop == e._prop && _val == e._val;
        }

        string toString() const
        {
            string s;
            s += '(';
            if (_neg) s += '!';
            s += _tag;
            if (!_prop.empty())
            {
                s += ' ';
                s += _prop;
                
                if (_val)
                {
                    s += ' ';

                    // in theory, the value of a property can be any string
                    // so make sure this is quoted for avoid format problems
                    var::Format vf;
                    vf._quoteStrings = true;
                    s += _val.toString(&vf);
                }
            }
            s += ')';
            return s;
        }

        friend std::ostream& operator<<(std::ostream& os, const Ent& e)
        { return os << e.toString(); }

        bool fromString(const char** sp, var::Format* f = 0)
        {
            // opposite of toString

            const char* s = *sp;
            bool r = *s == '(';
            if (r)
            {
                ++s;
                if (*s == '!') { _neg = true; ++s; }
                else _neg = false;

                // usually the tag is a term name, but it can be
                // an id for a selector
                int l = ParseBase::atSimpleName(s);
                r = l > 0;
                if (r)
                {
                    string t(s, l);
                    s += l;
                    _tag = t;
                    ParseBase::_skipws(&s);

                    // expect a prop or close ')'
                    if (*s != ')')
                    {
                        // expect a prop name, this is any string
                        // terminated by space or close ')'
                        // eg, can be "="
                        const char* p = s;
                        while (*p && *p != ' ' && *p != ')') ++p;
                        r = *p && p != s;
                        if (r)
                        {
                            string pr(s, p-s);
                            s = p;
                            _prop = pr;
                            ParseBase::_skipws(&s);

                            // expect a val or a close ')'
                            // val can be a Blob, in this case genstate
                            // this is handled by a given BlobParser
                            // passed here through the var::Format

                            if (*s != ')')
                            {
                                // expect string values to be quoted
                                // so we can parse them without running away
                                var v;
                                
                                var::Format vf;

                                // copy over from host, eg blobparser
                                if (f) vf = *f;
                                
                                vf._quoteStrings = true;
                                r = v.parse(&s, &vf);

                                if (r)
                                {
                                    _val = v;
                                }
                            }
                        }
                    }
                }

                if (r)
                {
                    // expect close quote
                    while (u_isspace(*s)) ++s;
                    r = *s == ')';

                    // return pointer after close ')'
                    if (r) *sp = ++s; 
                }
            }
            return r;
        }

    };

    typedef std::list<Ent>              Ents;
    typedef std::list<const Ent*>       Rel;
    
    Ents                                _ents;

    void clear()
    {
        // clear entire timeline
        _ents.clear();
    }

    // boolean values
    const Ent* find(const string& tag) const
    {
        // scan backwards to find (tag) or ~(tag)
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();
        while (it != en)
        {
            const Ent& e = *it;
            if (e._tag == tag && e._prop.empty()) return &e;
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

    bool set(const string& tag)
    {
        bool r = !test(tag);
        if (r) _append(tag);
        return r;
    }
    
    bool clear(const string& tag)
    {
        bool r = test(tag);
        if (r) _append(tag, true);
        return r;
    }

    // single property
    
    const Ent* find(const string& tag, const string& prop) const
    {
        // scan backwards to find (tag,prop...) or ~(tag,prop...)
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();
        while (it != en)
        {
            const Ent& e = *it;
            if (e._tag == tag && prop == e._prop) return &e;
            ++it;
        }
        return 0;
    }

    bool test(const string& tag, const string& prop) const
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
    
    const Ent* find(const string& tag, const string& prop, const var& v) const
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

    bool test(const string& tag, const string& prop, const var& v) const
    {
        const Ent* e = find(tag, prop, v);
        return e ? !e->_neg : false;
    }

    bool set(const string& tag, const string& prop, const var& v)
    {
        bool r = !test(tag, prop, v);  // not already true?
        if (r) _append(tag, prop, v);
        return r;
    }

    void setNoTest(const string& tag, const string& prop, const var& v)
    { _append(tag, prop, v); } // when you know not already set
    
    void clear(const string& tag, const string& prop, const var& v)
    { if (test(tag, prop, v)) _append(tag, prop, v, true); }

    
    // functional values

    const var* getfn(const string& tag, const string& prop) const
    {
        //Scan backwards to locate (x,rel,Y) or ~(x,rel,Y) for any Y.
        //  if (x,rel,Y) return Y otherwise null.
        const Ent* e = find(tag, prop);
        return (e && !e->_neg) ? &e->_val : 0;
    }

    bool clearfn(const string& tag, const string& prop) 
    {
        // find (tag, prop, Y)
        // if non-neg assert ~(tag, prop, Y)
        // return true iff neg asserted (ie prop cleared)
        
        bool v = false;
        const Ent* e = find(tag, prop);
        if (e && !e->_neg)
        {
            _append(tag, prop, e->_val, true); // neg
            v = true;
        }
        return v;
    }

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
            if (e->_val == v)
            {
                if (!e->_neg) r = false; // already true
                else
                {
                    // need to re-assert it
                    _append(tag, prop, v);  // copies var                    
                }
            }
            else
            {
                _append(tag, prop, e->_val, true); // neg
                _append(tag, prop, v);  // copies var
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

    void getRel(const string& prop, Rel& rel)
    {
        // get a projection of a whole `prop` relation
      
        Ents::const_reverse_iterator it = _ents.crbegin();
        Ents::const_reverse_iterator en = _ents.crend();

        Rel negs;
        
        while (it != en)
        {
            const Ent& e = *it;
            if (e._prop == prop)
            {
                if (e._neg) negs.push_back(&e);
                else
                {
                    bool ok = true;
                    for (auto i : negs)
                    {
                        if (e.sameTrio(*i)) { ok = false; break; }
                    }
                    if (ok) rel.push_back(&e);
                }
            }
            ++it;
        }
    }

    struct Mark
    {
        Timeline*               _host;
        bool                    _valid = true;

        // should be stable because we delete only the tails
        Ents::iterator          _p;

        operator bool() const { return _valid; }

        Mark& operator--() 
        {
            if (_valid)
            {
                Ents::iterator en = _host->_ents.begin();
                if (_p == en)
                {
                    // cannot reverse
                    _valid = false;
                }
                else --_p;
            }
            return *this;
        }

        Mark& operator++()
        {
            if (_valid)
            {
                Ents::iterator en = _host->_ents.end();
                if (_p != en) ++_p;
                else
                {
                    // too far
                    _valid = false;
                }
            }
            return *this;
        }

        Mark(Timeline* host) : _host(host) {}

        // ASSUME valid
        const string& prop() const { return _p->_prop; }
        const string& tag() const { return _p->_tag; }
        const var& val() const { return _p->_val; }
        
    };

    Mark getMark()
    {
        Mark m(this);
        m._p = std::prev(_ents.end());
        return m;
    }

    void clearToMark(const Mark& m)
    {
        // erase tail of timeline from after this mark.
        if (!m._valid) return;  // mark invalid
        Ents::iterator i = m._p;
        _ents.erase(++i, _ents.end());
    }

    Mark beginMarkScan()
    {
        // create a marker starting on the last timeline entry
        
        Mark m(this);
        
        if (!_ents.empty()) m._p = std::prev(_ents.end());
        else m._valid = false;
        return m;
    }

    void scanMarkFor(Mark& m,
                     const string& tag, const string& prop, const var& v)
    {

        // scan mark back to find (tag,prop,v) or it's denial
        // or not found.
        // if mark is valid, then parked on match.
        
        while (m)
        {
            const Ent& e = *m._p;

            // perform all classes of property match, depending
            // whether `prop` and `v` are also set.
            
            bool match = e._tag == tag;   // tag must match
            if (match)
            {
                match = prop.empty() || prop == e._prop;
                if (match)
                {
                    match = v.isVoid() || e._val == v;
                    if (match)
                    {
                        if (e._neg) m._valid = false;
                        break;
                    }
                }
            }

            --m;
        }
    }

    string toStringEnd(const Ents::const_iterator& en) const
    {
        // convert to string from the start to a given endpoint, `en`.
        // not including that endpoint.
        
        string s;
        Ents::const_iterator it = _ents.begin();
        if (it != en)
        {
            for (;;)
            {
                s += (*it).toString();
                if (++it == en) break;
                s += ' ';
            }
        }
        return s;        
        
    }

    string toString() const
    {
        // convert whole timeline to string
        return toStringEnd(_ents.end());
    }

    bool fromString(const char* s, var::Format* f = 0)
    {
        // trash any current timeline
        _ents.clear();
        bool r = true;

        while (*s == '(')
        {
            Ent e;
            r = e.fromString(&s, f);
            if (r)
            {
                _ents.push_back(e);
                ParseBase::_skipws(&s);
            }
            else
            {
                LOG1("Malformed element ", s);
                break;
            }
        }
        return r;
    }

    friend std::ostream& operator<<(std::ostream& os, const Timeline& t)
    { return os << t.toString(); }

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
