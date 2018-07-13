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

#include <QObject>
#include <QString>
#include <QDebug>
#include <QPointF>
#include <assert.h>
#include <vector>

#include "qdefs.h"
#include "logged.h"
#include "point.h"

#define QREGISTER_SHAPE  QREGISTER(QShape)

struct Shape
{
    const char* _name;
    const char* _path;

    typedef std::vector<Point2f> Points;
    Points      _points;
    
    float       _x0;
    float       _y0;
    
    float       _w;
    float       _h;

    Shape()
    { 
        _name = 0;
        _path = 0; 

        _x0 = 0;
        _y0 = 0;
        
        _w = 0;
        _h = 0;
    }

    Shape(const char* name, const char* path) 
        : _name(name), _path(path)
    {
        _parse(_path);
        _updateSize();
    }

    size_t size() const { return _points.size(); }

    Point2f last() const
    {
        Point2f pt;
        size_t n = _points.size();
        if (n > 0) pt = _points[n-1];
        return pt;
    }

    Point2f first() const
    {
        Point2f pt;
        size_t n = _points.size();
        if (n > 0) pt = _points[0];
        return pt;
    }

    static float readv(const char*& s)
    {
        float v = 0;
        const char* p = s;
        while (u_isspace(*p)) ++p;
        const char* q = p;
        if (*q == '-' || *q == '+') ++q; // allow sign
        while (u_isdigit(*q) || *q == '.') ++q;
        if (q != p)
        {
            v = atof(p);
            s = q;
        }
        return v;
    }

    static Point2f readp(const char*& s)
    {
        Point2f pt;
        pt.x = readv(s);
        pt.y = readv(s);
        return pt;
    }

    void _parse(const char* p)
    {
        bool ok = true;
        char lastc = 0;
        while (ok)
        {
            while (u_isspace(*p)) ++p;
            char c = *p++;
            
            if (!c) break; // done
            
            if (u_isdigit(c) || c == '-' || c == '+' || c == '.')
            {
                // continue last code
                c = lastc;
                --p;
            }
            
            switch (c)
            {
            case 'M':
                _points.emplace_back(readp(p));
                break;
            case 'l':
                _points.emplace_back(last() + readp(p));
                break;
            case 'v':
                _points.emplace_back(last() + Point2f(0,readv(p)));
                break;
            case 'h':
                _points.emplace_back(last() + Point2f(readv(p), 0));
                break;
            case 'z':
                _points.emplace_back(first());
                break;
            default:
                ok = false;
            }

            lastc = c;
        }

        //LOG3("parse, ", *this);
    }

    void _updateSize()
    {
        size_t n = size();

        _w = 0;
        _h = 0;
        
        _x0 = 0;
        _y0 = 0;

        if (n > 1)
        {
            Point2f pmin, pmax;
            pmax = pmin = _points[0];
            for (size_t i = 1; i < n; ++i)
            {
                pmin = pmin.min(_points[i]);
                pmax = pmax.max(_points[i]);
            }

            Point2f dp = pmax - pmin;
            _w = dp.x;
            _h = dp.y;

            _x0 = pmin.x;
            _y0 = pmin.y;
        }
    }

    void transform(float a, float b, float c, float d, float e, float f)
    {
        size_t n = size();
        for (size_t i = 0; i < n; ++i)
        {
            Point2f& p = _points[i];
            float x = a*p.x + c*p.y + e;
            float y = b*p.x + d*p.y + f;
            p.x = x;
            p.y = y;
        }
        _updateSize();
    }

    bool pointInsideBox(const Point2f& p) const
    {
        // point inside box?
        return (p.x >= _x0 && p.x < _x0 + _w && 
                p.y >= _y0 && p.y < _y0 + _h);
    }

    friend std::ostream& operator<<(std::ostream& os, const Shape& s)
    {
        os << s._name << ": ";
        for (size_t i = 0; i < s.size(); ++i)
        {
            if (i) os << ", ";
            os << s._points[i];
        }
        return os;
    }
};

class QShape: public QObject
{
public:
    
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(qreal width READ width NOTIFY widthChanged);
    Q_PROPERTY(qreal height READ height NOTIFY heightChanged);
    Q_PROPERTY(int size READ size NOTIFY sizeChanged);

public:

    typedef std::string string;
    static const int maxShapes = 10;
    
    Shape       _shape;

    ~QShape()
    {
        for (int i = 0; i < maxShapes; ++i) delete shapes[i];
    }

    QString name() const
    {
        return _shape._name ? QSTRC(_shape._name) : QString();
    }

    void setName(const QString& sname)
    {
        string s = STRQ(sname);
        _setName(s.c_str(), false);
    }
    
    void _setName(const char* sname, bool force)
    {
        if (sname
            && (force || !_shape._name || u_stricmp(sname, _shape._name)))
        {
            for (int i = 0; i < maxShapes; ++i)
            {
                Shape* si = shapes[i];
                if (!si) break;
                if (!u_stricmp(sname, si->_name))
                {
                    // install new shape
                    _shape = *si;
                    _changed();
                    break;
                }
            }

        }
    }

    void _sizeChanged()
    {
        emit widthChanged();
        emit heightChanged();
    }

    void _changed()
    {
        //LOG3("QShape, ", _shape._name << " selected");
        emit nameChanged();
        emit sizeChanged();
        _sizeChanged();
    }

    qreal width() const { return _shape._w; }
    qreal height() const { return _shape._h; }
    int size() const { return (int)_shape.size(); }

    Shape* shapes[maxShapes] = 
    {
        new Shape("arrow", "M500 100l-100-100v50h-400v100h400v50"),
        new Shape("arrowbi",
              "M0 100l100 100v-50h300v50l100-100-100-100v50h-300v-50"),

        // up triangle, side 1000 from left base corner
        new Shape("triup", "M0 0M1000 0M500-866"),

        // down triangle, side 1000 from left base corner
        new Shape("tridown", "M0 0M1000 0M500 866")
    };


    Q_INVOKABLE void transform(qreal a, qreal b, qreal c, qreal d, 
                               qreal e, qreal f)
    {
        _shape.transform(a, b, c, d, e, f);
        _sizeChanged();
    }

    Q_INVOKABLE QPointF at(int i) const
    {
        QPointF pt;
        if (i < (int)_shape.size())
        {
            const Point2f& pi = _shape._points[i];
            pt = QPointF(pi.x, pi.y);
        }
        return pt;
    }

    Q_INVOKABLE qreal atx(int i) const { return at(i).x(); }
    Q_INVOKABLE qreal aty(int i) const { return at(i).y(); }
    
    Q_INVOKABLE qreal widthNow() const { return _shape._w; }
    Q_INVOKABLE qreal heightNow() const { return _shape._h; }

    Q_INVOKABLE bool isInside(qreal x, qreal y)
    {
        // TODO: should test point inside polygon.
        Point2f p(x, y);
        return _shape.pointInsideBox(p);

    }

    Q_INVOKABLE void reset()
    {
        // reinstall origin shape, before any transform
        _setName(_shape._name, true);
    }

signals:

    void nameChanged();
    void widthChanged();
    void heightChanged();
    void sizeChanged();
};

