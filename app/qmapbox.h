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
#include <QRect>
#include <QDebug>

#include "qdefs.h"
#include "qcontrol.h"
#include "mapbox.h"
#include "logged.h"

#define QREGISTER_MAPBOX  QREGISTER(QMapBox)

class QMapBox: public QObject, public Map::Notifier
{
    typedef std::string string;

public:

    Q_OBJECT
    Q_PROPERTY(int boxCount READ boxCount NOTIFY boxCountChanged);
    Q_PROPERTY(int width READ width NOTIFY widthChanged);
    Q_PROPERTY(int height READ height NOTIFY heightChanged);
    Q_PROPERTY(int linkCount READ linkCount NOTIFY linkCountChanged);
    Q_PROPERTY(QString currentBox READ currentBox NOTIFY currentBoxChanged);
    Q_PROPERTY(int generation READ generation NOTIFY generationChanged);
    Q_PROPERTY(QString backimage READ backimage NOTIFY backimageChanged);
    Q_PROPERTY(QString mapTitle READ mapTitle NOTIFY mapTitleChanged);

    Q_PROPERTY(bool gon READ gon NOTIFY directionsChanged);
    Q_PROPERTY(bool gone READ gone NOTIFY directionsChanged);
    Q_PROPERTY(bool goe READ goe NOTIFY directionsChanged);
    Q_PROPERTY(bool gose READ gose NOTIFY directionsChanged);
    Q_PROPERTY(bool gos READ gos NOTIFY directionsChanged);
    Q_PROPERTY(bool gosw READ gosw NOTIFY directionsChanged);
    Q_PROPERTY(bool gow READ gow NOTIFY directionsChanged);
    Q_PROPERTY(bool gonw READ gonw NOTIFY directionsChanged);
    Q_PROPERTY(bool goup READ goup NOTIFY directionsChanged);
    Q_PROPERTY(bool godown READ godown NOTIFY directionsChanged);

public:
    
    Map::Ref   _map;
    int        _boxCount;
    int        _width;
    int        _height;
    int        _linkCount;
    string     _currentBox;
    int        _generation;
    uint       _currentExits;
    string     _backimage;
    string     _mapTitle;

    QMapBox()
    {
        _map = QControl::theControl()->_map;
        _map->_notifier = this;

        _boxCount = 0;
        _width = 0;
        _height = 0;
        _linkCount = 0;
        _generation = 0;
        _currentExits = 0;

        // force query of initial data
        changed();
    }

    ~QMapBox() { _map->_notifier = 0; }

    // from notifier
    void changed() override 
    {
        setCurrentBox(_map->currentBox());
        setCurrentExits(_map->currentExits());
        //LOG3("qmapbox, changed ", _currentBox);
        setBackimage(_map->backimage());
        setMapTitle(_map->mapTitle());
    }
    
    int boxCount() const { return _boxCount; }

    bool setBoxCount(int n)
    {
        bool res = (n != _boxCount);
        if (res)
        {
            _boxCount = n;
            LOG4("map, boxcount changed ", _boxCount);
            emit boxCountChanged();
        }
        return res;
    }

    bool setWidth(int v)
    {
        bool res = (v != _width);
        if (res)
        {
            _width = v;
            LOG4("map, width changed ", _width);
            emit widthChanged();
        }
        return res;
    }

    bool setHeight(int v)
    {
        bool res = (v != _height);
        if (res)
        {
            _height = v;
            LOG4("map, height changed ", _height);
            emit heightChanged();
        }
        return res;
    }

    int width() const { return _width; }
    int height() const { return _height; }
    QString currentBox() const { return QSTR(_currentBox); } 
    int generation() const { return _generation; }
    QString backimage() const { return QSTR(_backimage); }
    QString mapTitle() const { return QSTR(_mapTitle); }

    bool setCurrentBox(const string& id)
    {
        bool res = (id != _currentBox);
        if (res)
        {
            _currentBox = id;
            LOG4("map, currentbox changed ", _currentBox);
            emit currentBoxChanged();
        }
        return res;
    }
    
    Q_INVOKABLE QString boxName(int i) const { return QSTR(_map->boxname(i)); }
    Q_INVOKABLE int getBoxX(int i) const { return _map->box(i)._x; }
    Q_INVOKABLE int getBoxY(int i) const { return _map->box(i)._y; }
    Q_INVOKABLE int getBoxWidth(int i) const { return _map->box(i)._w; }
    Q_INVOKABLE int getBoxHeight(int i) const { return _map->box(i)._h; }
    Q_INVOKABLE QString getBoxID(int i) const { return QSTR(_map->boxID(i)); }
    Q_INVOKABLE int getBoxItemCount(int i) const { return _map->boxItemCount(i); }
    Q_INVOKABLE bool getBoxDark(int i) const { return _map->boxDark(i); }
    Q_INVOKABLE bool getBoxIndUp(int i) const { return _map->boxIndUp(i); }
    Q_INVOKABLE bool getBoxIndDown(int i) const { return _map->boxIndDown(i); }

    Q_INVOKABLE QPoint getLinkFrom(int i) const
    {
        Point2 p = _map->linkFrom(i);
        return QPoint(p.x, p.y);
    }

    Q_INVOKABLE QPoint getLinkTo(int i) const
    {
        Point2 p = _map->linkTo(i);
        return QPoint(p.x, p.y);
    }

    Q_INVOKABLE bool getLinkBi(int i) const
    { return _map->linkBi(i); }

    Q_INVOKABLE bool getLinkVisible(int i) const
    { return _map->linkVisible(i); }

    Q_INVOKABLE void performLayout(int w, int h, int gap)
    {
        bool changed = _map->performLayout(w, h, gap);
        if (changed)
        {
            LOG4("qmapbox ", "performLayout changed");

            bool r = setBoxCount(_map->boxCount())
                | setWidth(_map->width())
                | setHeight(_map->height())
                | setLinkCount(_map->linkCount())
                | setCurrentBox(_map->currentBox())
                | setCurrentExits(_map->currentExits())
                | setBackimage(_map->backimage())
                | setMapTitle(_map->mapTitle());

            if (!r)
            {
                // map can change internally without any of the counts
                // changing.
                
                LOG3("performLayout, ", "implied change");
                
                ++_generation;
                emit generationChanged();
            }
        }
    }

    Q_INVOKABLE int getBoxIndexForID(const QString& id) const
    {
        return _map->boxIndexForID(STRQ(id));
    }

    int linkCount() const { return _linkCount; }

    bool setLinkCount(int n)
    {
        bool res = (n != _linkCount);
        if (res)
        {
            _linkCount = n;
            LOG4("map, linkcount changed ", _linkCount);
            emit linkCountChanged();
        }
        return res;
    }

    bool setCurrentExits(uint mask)
    {
        bool res = (mask != _currentExits);
        if (res)
        {
            _currentExits = mask;
            //LOG3("map, currentexits changed ", std::hex << _currentExits << std::dec);
            emit directionsChanged();
        }
        return res;
    }

    bool setBackimage(const string& s)
    {
        bool res = (s != _backimage);
        if (res)
        {
            _backimage = s;
            LOG4("map, backimage changed ", _backimage);
            emit backimageChanged();
        }
        return res;
    }

    bool setMapTitle(const string& s)
    {
        bool res = (s != _mapTitle);
        if (res)
        {
            _mapTitle = s;
            LOG4("map, mapTitle changed ", _mapTitle);
            emit mapTitleChanged();
        }
        return res;
    }

    // directions

    bool gon() const { return _currentExits & 1; }
    bool gone() const { return _currentExits & 2; }
    bool goe() const { return _currentExits & 4; }
    bool gose() const { return _currentExits & 8; }
    bool gos() const { return _currentExits & 16; }
    bool gosw() const { return _currentExits & 32; }
    bool gow() const { return _currentExits & 64; }
    bool gonw() const { return _currentExits & 128; }
    bool goup() const { return _currentExits & 256; }
    bool godown() const { return _currentExits & 512; }

    Q_INVOKABLE bool cango(int dir) const
    {
        // dir = 1 => north
        // dir = 2 => northeast 
        // etc
        return (_currentExits & (1<<dir)) != 0;
    }
    
signals:

    void boxCountChanged();
    void widthChanged();
    void heightChanged();
    void linkCountChanged();
    void currentBoxChanged();
    void generationChanged();
    void directionsChanged();
    void backimageChanged();
    void mapTitleChanged();
    
private:

};

