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

#include <iostream>
#include "cutils.h"

template<class T> struct Point2T
{
    Point2T() : x(0), y(0) {}
    Point2T(T x, T y) 
    {
        this->x = x;
        this->y = y;
    }

    explicit operator bool() const
    {
        return this->x != 0 || this->y != 0;
    }

    Point2T min(const Point2T& p) const
    {
        return Point2T(u_min(x, p.x), u_min(y, p.y));
    }
    
    Point2T max(const Point2T& p) const
    {
        return Point2T(u_max(x, p.x), u_max(y, p.y));
    }

    Point2T& operator+=(const Point2T& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    Point2T& operator-=(const Point2T& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    friend Point2T operator-(const Point2T& a, const Point2T& b)
    {
        Point2T t(a);
        t -= b;
        return t;
    }

    friend Point2T operator+(const Point2T& a, const Point2T& b)
    {
        Point2T t(a);
        t += b;
        return t;
    }

    Point2T abs() const
    {
        Point2T t;
        t.x = u_abs(x);
        t.y = u_abs(y);
        return t;
    }

    bool operator==(const Point2T& p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point2T& p) const { return x != p.x || y != p.y; }

    friend std::ostream& operator<<(std::ostream& os, const Point2T& p)
    {
        return os << '(' << p.x << ',' << p.y << ')';
    }

    
    T x, y;
};

typedef Point2T<int> Point2;
typedef Point2T<float> Point2f;
