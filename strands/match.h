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

struct Match
{
    typedef std::function<bool(Match*)> Op;
    
    // term to match 
    const Head&         _m;
    ComparatorEq        _coEq;
    Comparator*         _co;
    Op                  _op;
    mutable bool        _found;
    
    Match(const Head& t) : _m(t)
    {
        _found = false;
        _co = &_coEq;
    }

    void setOp(const Op& op) { _op = op; }

    int operator()(const Head& h) const { return (*_co)(_m, h); }

    bool found(bool hit) const
    {
        _found = hit;
        return _op ? _op((Match*)this) : _found;
    }
};
