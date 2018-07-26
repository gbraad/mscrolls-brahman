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

#include <vector>
#include "apitypes.h"
#include "rect.h"
#include "types.h"
#include "utils.h"

// forward
struct MapInfo;

struct DLLX Map: public API<Map>
{
    Map(Control* c) : API(c), _notifier(0) { _init(); }
    ~Map();
    
    Notifier*           _notifier;

    typedef ListOf<std::string> Path;

    bool        performLayout(int w, int h, int gap);
    bool        updateMapLocation(MapInfo&);
    int         boxCount() const;
    int         width() const;
    int         height() const;
    Rect        box(int i) const;
    string      boxname(int i) const;
    string      boxID(int i) const;
    int         boxItemCount(int i) const;
    bool        boxDark(int i) const;
    bool        boxIndUp(int i) const;
    bool        boxIndDown(int i) const;
    int         linkCount() const;
    Point2      linkFrom(int i) const;
    Point2      linkTo(int i) const;
    bool        linkBi(int i) const;
    bool        linkVisible(int i) const;
    string      currentBox() const;
    int         boxIndexForID(const string&) const;
    uint        currentExits() const;
    bool        pathTo(const string& place, Path&) const;
    string      backimage() const;
    string      mapTitle() const;

    void _init();

    struct Imp;
    Imp*                _imp;
};

