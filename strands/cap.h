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

#include "utf8.h"
#include "logged.h"

#define TAG_CAP "Cap, "

namespace ST
{

struct Capture
{
    typedef std::string string;
    
    struct Elt
    {
        // element is either a term a value or a string.
        // when neither term or v are valid, then string is valid.
        Term*       _term = 0;
        var         _v;
        string      _s;

        Elt() {}
        Elt(Term* t) : _term(t) {}
        Elt(const var& v) : _v(v.copy()) {}
        Elt(const string& s) : _s(s) {}
        Elt(const Elt& e)
        {
            // copy constructor needs to copy v
            _term = e._term;
            _v = e._v.copy();
            _s = e._s;
        }

        bool operator==(const Elt& e) const
        {
            //LOG1(TAG_CAP "compare ", toStringTyped() << " == " << e.toStringTyped());
            if (_term)
            {
                // must be a term match, no stringification
                return _term == e._term;
            }
            else if (_v)
            {
                if (e._v) return _v == e._v;

                // allow stringification of values to match
                // null string never matches anything
                if (e._s.size()) return equalsIgnoreCase(_v.toString(), e._s);
                
                return false;
            }
            else
            {
                assert(_s.size());  // null elt?

                // allow stringification of values to match
                if (e._v) return equalsIgnoreCase(_s, e._v.toString());

                return equalsIgnoreCase(_s, e._s);
            }
        }

        string toStringTyped() const
        {
            string s;
            if (_term) s = "Term";
            else if (_v) s = "var";
            else s = "string";
            s += ':';
            s += toString();
            return s;
        }
        
        string toString(var::Format* f = 0) const
        {
            string s;
            if (_term) s = _term->_name;
            else if (_v) s = _v.toString(f);
            else s = _s;
            return s;
        }

        friend std::ostream& operator<<(std::ostream& os, const Elt& e)
        { return os << e.toString(); }

        var toVar() const
        {
            return toString();
        }

        void fromVar(const var& v)
        {
            // only unpack into a blank Elt
            assert(!_term && !_v && _s.empty());

            if (v)
            {
                const char* s = v.rawString();
                if (s)
                {
                    _term = Term::find(s);
                    if (!_term)
                    {
                        // is just a string!
                        _s = s;
                    }
                }
                else
                {
                    // is a val
                    _v = v.copy();
                    
                }
            }
        }
       
    };

    typedef std::list<Elt>  Elts;

    Elts            _elts;
    bool            _needNewline = false;

    bool empty() const { return _elts.empty(); }
    int size() const { return _elts.size(); }
    void clear()
    {
        _elts.clear();
        _needNewline = false;
    }
    
    operator bool() const { return !empty(); }
    
    static void cats(string& s, const string& s1)
    {
        // append s1 to s and add space if necessary

        // note, some cases cannot be fixed.
        // eg "FOO" FOO -> "foo"foo
        // because we dont know if it's a start or end quote
        // otherwise get, " foo" foo
        // where one is correct and other end wrong.

        if (!s1.empty())
        {
            uint sz = s.size();
            if (sz)
            {
                Utf8 us(s);
                int last = us.lastChar();

                Utf8 us1(s1);
                int first = *us1; // first char
                
                // normally wont end in a space already but it could
                // be escaped in.
                // first char wont be space unless escaped in
                if (!u_isspace(last) && !u_isspace(first))
                {
                    bool spc = false;

                    // open quotes must be preceded by space
                    if (first == Utf8::openQuote) spc = true;
                    else
                    {
                        // or any other open bracket
                        if (first < 0xff && strchr("([{",first)) spc = true;
                    }

                    bool alnumFirst = false;

                    if (!spc)
                    {
                        // end quote or close bracket followed by alphanum (ie non-punc)
                        // count underscores like letters
                        alnumFirst = u_isalnum(first);
                        
                        if (alnumFirst)
                        {
                            if (last == Utf8::closeQuote) spc = true;
                            else if (last < 0xff && strchr(")}]", last)) spc = true;
                        }
                    }
                    
                    if (!spc)
                    {
                        bool albefore = u_isalnum(last);
                                            
                        // preceding punctuation that requires a space following
                        // when we continue with a normal word.
                        bool puncbefore = last < 0xff && strchr(".,?;:!'", last) != 0;
                        if (albefore || puncbefore)
                        {
                            // add a space if next segment starts with non-punctuation
                            // underscore after punct => space
                            spc = alnumFirst || first == '_';

                            // also certain chars count like letters here, eg $40
                            if (first < 0xff && strchr("£$", first) != 0) spc = true;

                            // things like quotes following punctuation must have
                            // space. eg he said, "hello"
                        
                            if (puncbefore && Utf8::isQuote(first)) spc = true;
                        }
                    }
                        
                    if (spc) s += ' ';
                }
            }
            s += s1;
        }
    }
    
    void add(Term* t) { if (t) _elts.emplace_back(Elt(t)); }
    void add(const var& v)  { if (v) _elts.emplace_back(Elt(v)); }
    void add(const string& s)
    {
        if (!s.empty())
        {
            _needNL();
            if (!empty())
            {
                Elt& e = _elts.back();
                if (!e._s.empty())
                {
                    cats(e._s, s);
                    return;
                }
            }

            // otherwise append a new string
            _elts.emplace_back(Elt(s));
        }
    }

    void _addNL()
    {
        if (!empty())
        {
            Elt& e = _elts.back();
            int sz = e._s.size();
            if (sz)
            {
                // add two to make a new paragraph
                if (e._s[sz-1] != '\n') e._s += '\n';
                return;
            }
        }

        string s;
        s += '\n';
        _elts.emplace_back(Elt(s));
    }

    void _needNL()
    {
        if (_needNewline)
        {
            _needNewline = false;
            _addNL();
        }
    }
    
    void add(const char* s)
    {
        if (s && *s) add(string(s));
    }

    void add(char c)
    {
        _needNL();
        if (!empty())
        {
            Elt& e = _elts.back();
            if (!e._s.empty())
            {
                // append to existing string
                e._s += c;
                return;
            }
        }

        string s;
        s += c;
        _elts.emplace_back(Elt(s));

    }

    void add(const Elt& e)
    {
        // use copy constructor to dup var if present
        _elts.push_back(e);
    }

    bool contains(const Elt& e)
    {
        for (auto& ei : _elts)
            if (ei == e) return true;
        return false;
    }

    bool match(const Elt& e) { return contains(e); }

    bool matchSimple(const char* s) const
    {
        // are we just one string?
        bool v = false;
        if (s && *s)
        {
            if (size() == 1) // insist we are just one string
            {
                const Elt& e = _elts.front();
                v = (e._s == s);
            }
        }
        return v;
    }

    var toVar() const
    {
        var v;
        int sz = size();
        if (sz == 1)
        {
            v = _elts.front().toVar();
        }
        else if (sz > 1)
        {
            for (auto& e : _elts) v.append(e.toVar());
        }
        return v;
    }

    void fromVar(const var& v)
    {
        assert(_elts.empty());
        if (v)
        {
            if (v.isList())
            {
                int sz = v.size();
                for (int i = 0; i < sz; ++i)
                {
                    Elt e;
                    e.fromVar(v[i]);
                    _elts.push_back(e);
                }
            }
            else
            {
                Elt e;
                e.fromVar(v);
                _elts.push_back(e);
            }
        }
    }

    string toString() const
    {
        // dump the cap
        string s;
        for (auto& e : _elts)
        {
            if (!s.empty()) s += ", ";
            s += e.toString();
        }
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Capture& c)
    { return os << c.toString(); }

};

}; // ST
