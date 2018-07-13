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
#include <vector>
#include <string>
#include "cutils.h"
#include "point.h"

struct Rect
{
    typedef std::string string;
    
    int _x, _y;
    int _w, _h;

    Rect() : _x(0), _y(0), _w(0), _h(0) {}
    Rect(int x) : _x(x), _y(0), _w(0), _h(0) {}
    Rect(int x, int y) : _x(x), _y(y), _w(0), _h(0) {}
    Rect(int x, int y, int w, int h) : _x(x), _y(y), _w(w), _h(h) {}

    Rect(Point2 p1, Point2 p2)
    {
        _x = u_min(p1.x, p2.x);
        _y = u_min(p1.y, p2.y);
        _w = u_max(p1.x, p2.x) - _x;
        _h = u_max(p1.y, p2.y) - _y;
    }

    int right() const { return _x + _w; }
    int bottom() const { return _y + _h; }
    int top() const { return _y; }
    bool valid() const { return _w > 0 && _h > 0; }

    bool operator==(const Rect& r) const
    { return _x == r._x && _y == r._y && _w == r._w && _h == r._h; }
    bool operator!=(const Rect& r) const
    { return _x != r._x || _y != r._y || _w != r._w || _h != r._h; }

    Point2 centre() const
    { return Point2((_x + _x + _w)/2, (_y + _y + _h)/2); }

    Point2 topLeft() const { return Point2(_x, _y); }
    Point2 topRight() const { return Point2(_x + _w, _y); }
    Point2 bottomLeft() const { return Point2(_x, _y + _h); }
    Point2 bottomRight() const { return Point2(_x + _w, _y + _h); }

    Point2 centreTop() const { return Point2((_x + _x + _w)/2, _y); }
    Point2 centreBottom() const { return Point2((_x + _x + _w)/2, _y + _h); }
    Point2 centreLeft() const { return Point2(_x, (_y + _y + _h)/2); }
    Point2 centreRight() const { return Point2(_x + _w, (_y + _y + _h)/2); }

    void moveXYby(int dx, int dy)
    {
        _w -= dx;
        if (_w < 0) _w = 0;
        _h -= dy;
        if (_h < 0) _h = 0;
        _x += dx;
        _y += dy;
    }

    void moveXY(int x, int y)
    {
        // move top left but not bottom right
        moveXYby(x - _x, y - _y);
    }

    void moveBy(int dx, int dy)
    {
        _x += dx;
        _y += dy;
    }

    void moveBy(Point2 dp)
    {
        _x += dp.x;
        _y += dp.y;
    }

    string toString() const
    {
        char buf[256];
        sprintf(buf, "(%d,%d) - (%d,%d) [%d,%d]",
                _x, _y, _x + _w, _y + _h, _w, _h);
        return buf;
    }

    Rect combine(const Rect& r) const
    {
        // union
        if (r.valid())
        {
            if (valid())
            {
                Rect res;
                res._x = u_min(_x, r._x);
                res._y = u_min(_y, r._y);
                res._w = u_max(right(), r.right()) - res._x;
                res._h = u_max(bottom(), r.bottom()) - res._y;
                return res;
            }
            return r;
        }
        return *this;
    }

    bool within(const Rect& r) const
    {
        // is this box inside box `r`?
        if (!valid()) return true;
        if (!r.valid()) return false;
        return _x >= r._x && right() <= r.right() && 
            _y >= r._y && bottom() <= r.bottom();
    }

    bool within(const Point2& p) const
    {
        // is point inside box?
        return p.x >= _x && p.x < _x + _w && p.y >= _y && p.y < _y + _h;
    }


    static const int OUT_LEFT = 1;
    static const int OUT_TOP = 2;
    static const int OUT_RIGHT = 4;
    static const int OUT_BOTTOM = 8;
    
    int outcode(const Point2& p) const
    {
        int out = 0;
        if (_w <= 0) {
            out |= OUT_LEFT | OUT_RIGHT;
        } else if (p.x < _x) {
            out |= OUT_LEFT;
        } else if (p.x > _x + _w) {
            out |= OUT_RIGHT;
        }
        if (_h <= 0) {
            out |= OUT_TOP | OUT_BOTTOM;
        } else if (p.y < _y) {
            out |= OUT_TOP;
        } else if (p.y > _y + _h) {
            out |= OUT_BOTTOM;
        }
        return out;
    }

    bool intersectsLine(Point2 p1, Point2 p2) const
    {
        int out1, out2;
        if ((out2 = outcode(p2)) == 0) return true;
        
        while ((out1 = outcode(p1)) != 0)
        {
            if ((out1 & out2) != 0) return false;
            
            if ((out1 & (OUT_LEFT | OUT_RIGHT)) != 0)
            {
                int x = _x;
                if ((out1 & OUT_RIGHT) != 0) x += _w;

                double g = ((double)(p2.y - p1.y))/(p2.x - p1.x);
                p1.y += (x - p1.x) * g;
                p1.x = x;
            }
            else
            {
                int y = _y;
                if ((out1 & OUT_BOTTOM) != 0) y += _h;

                double g = ((double)(p2.x - p1.x))/(p2.y - p1.y);
                p1.x += (y - p1.y) * g;
                p1.y = y;
            }
        }
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Rect& r)
    {
        return os << r.toString();
    }
};

struct Rects: public std::vector<Rect>
{
    Rect bounds() const
    {
        Rect r;
        size_t n = this->size();
        for (size_t i = 0; i < n; ++i) r = at(i).combine(r);
        return r;
    }

    bool operator==(const Rects& r) const
    {
        size_t n = this->size();
        if (n != r.size()) return false;
        for (size_t i = 0; i < n; ++i)
            if (at(i) != r.at(i)) return false;

        return true;
    }

    bool operator!=(const Rects& r) const { return !(*this == r); }
};

