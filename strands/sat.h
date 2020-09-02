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

#include "tree.h"

struct ComparatorSat: public Comparator, public HeadTraits
{
    ComparatorSat() : _ri(_results) {}

    // list of subst sets
    List                _results;
    List::iterator      _ri; 

    struct SubsTree
    {
        SubsTree*           _prev;
        Tree                _s;
    };

    SubsTree*               _subs = 0;

    int _sat(const Head& a, const Head& b);
    int _satList(const Head& a, const Head& b, int& headmatch);
    int _satSet(const Head& a, const Head& b);

    void push(SubsTree& st)
    {
        st._prev = _subs;
        _subs = &st;
    }

    void pop()
    {
        assert(_subs);
        _subs = _subs->_prev;        
    }

    void subsOK(SubsTree& st)
    {
        // add into results.
        if (!st._s.isEmpty())
        {
            if (_subs->_prev && !_subs->_prev->_s.isEmpty())
            {
                // suppose we have nested subs;
                //   TOP
                //   A   B
                // C  D
                //
                // then when we close C, must add to result (A + C)
                // and when we close D, add (A + D)
                LOG3("Warning: need to combine subs", "");
            }
            
            ComparatorEq co;
            if (!_results.member(st._s, co))
                _ri.insert(st._s);
        }
    }
    
    // compliance
    int operator()(const Head& a, const Head& b)
    {
        return _sat(a, b);
    }
};

struct ComparatorEnt: public Comparator, public HeadTraits
{
    struct Entails
    {
        Entails() {}
        Entails(LValue& lv, int mode = 0) :
            _mode(mode), _lvalue(&lv) {}

        int             _mode = 0;
        LValue*         _lvalue = 0;
        Entails*        _prev = 0;

        bool            _overwriteMatch = false;
    };

    Entails*    _entails;

    ComparatorEnt(Entails& e)
    {
        _entails = 0;
        push(e);
    }

    bool entails() const { return _entails->_lvalue != 0; }
    bool overwrite() const { return _entails->_mode > 0; }
    bool erase() const { return _entails->_mode < 0; }

    void set(const Head& h)
    {
        assert(_entails && _entails->_lvalue);
        *_entails->_lvalue = h;
    }

    void push(Entails& e)
    {
        e._prev = _entails;
        _entails = &e;
    }

    void pop()
    {
        assert(_entails);
        _entails = _entails->_prev;
    }

    // compliance
    int operator()(const Head& a, const Head& b) { return _ent(a, b); }

    int _ent(const Head& a, const Head& b);
    int _entListHeads(List::iterator& i1, List::iterator& i2);
    int _entListBodies(List::iterator& i1, List::iterator& i2);
    int _entLists(const Head& a, const Head& b);
    int _entSet(const Match& m, const Head& a, const Head& b);
    int _entFromSet(const Head& a, const Head& b);
};


