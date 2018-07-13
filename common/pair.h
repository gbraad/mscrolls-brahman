/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2016-2017.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@voidware.com
 */

#pragma once

#include "cutils.h"

template<class T> struct Pair
{
    Pair() {}
    Pair(const T& a, const T& b) : _lo(a), _hi(b) {}

    bool valid() const { return _hi > _lo; }
    operator bool() const { return valid(); }
    unsigned int size() const { return _hi - _lo; }

    bool intersects(const Pair& p) const
    { return p._hi > _lo && p._lo < _hi; }

    Pair intersect(const Pair& p) const
    {
        T a = u_max(_lo, p._lo);
        T b = u_min(_hi, p._hi);
        if (a > b) a = b;
        return Pair(a, b);
    }

    Pair intersect(const T& a, const T& b) const
    { return intersect(Pair(a,b)); }
    
    Pair operator+(const T& dt) const { return Pair(_lo + dt, _hi + dt); }
    Pair operator-(const T& dt) const { return Pair(_lo - dt, _hi - dt); }

    Pair max(const T& t) const
    {
        // make all values >= t
        return Pair(u_max(_lo, t), u_max(_hi, t));
    }

    T       _lo;
    T       _hi;
};

