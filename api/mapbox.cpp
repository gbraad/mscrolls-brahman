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


#include "mapboxi.h"

void Map::_init()
{
    _imp = new Imp(this);
}

Map::~Map() { delete _imp; }

typedef std::string string;

bool Map::performLayout(int w, int h, int gap)
{return _imp->performLayout(w, h, gap); }

int Map::boxCount() const { return _imp->boxCount(); }
int Map::width() const { return _imp->width(); }
int Map::height() const { return _imp->height(); }
Rect Map::box(int i) const { return _imp->box(i); }
string Map::boxname(int i) const { return _imp->boxname(i); }
string Map::boxID(int i) const { return _imp->boxID(i); }
int Map::boxItemCount(int i) const { return _imp->boxItemCount(i); }
bool Map::boxDark(int i) const { return _imp->boxDark(i); }
bool Map::boxIndUp(int i) const { return _imp->boxIndUp(i); }
bool Map::boxIndDown(int i) const { return _imp->boxIndDown(i); }
int Map::linkCount() const { return _imp->linkCount(); }
Point2 Map::linkFrom(int i) const { return _imp->linkFrom(i); }
Point2 Map::linkTo(int i) const { return _imp->linkTo(i); }
bool Map::linkBi(int i) const { return _imp->linkBi(i); }
bool Map::linkVisible(int i) const { return _imp->linkVisible(i); }
string Map::currentBox() const { return _imp->_currentPlace; }
int Map::boxIndexForID(const string& id) const { return _imp->boxIndexForID(id); }
bool Map::updateMapLocation(MapInfo& mi) { return _imp->updateMapLocation(mi); }
uint Map::currentExits() const { return _imp->_currentExits; }
string Map::backimage() const { return _imp->_backimage; }
bool Map::pathTo(const string& place, Map::Path& p) const
{ return _imp->pathTo(place, p); }
string Map::mapTitle() const { return _imp->_mapTitle; }
