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

#include "ifinfo.h"

struct MapInfo: public InfoBase
{
    /* filled in by engine `updateMapInfo` 
     *
     * the current known map is returned as a JSON object string.
     * the tags for this json object are in `braschema.h`
     * so that engines can easily build the json object, a helper
     * `MapInfoBuilder` is provided that makes the string from 
     * the set of places and exits.
     *
     * `currentLocation` is the ID of the current room
     * `changed` is set by the engine if it rebuilt the string
     */

    bool                _full = false; // request full map data
    std::string         _currentLocation;
    unsigned int        _currentExits = 0;
};

struct MapInfoBuilder: public InfoBuilder
{
    MapInfoBuilder(emitFn* e, void* ctx) : InfoBuilder(e, ctx) {}

    void mapBackimage(const char* name) { tagString(BRA_MAP_BACKIMAGE, name); }
    void mapBackimage(const string& name) { tagString(BRA_MAP_BACKIMAGE, name); }
    void beginPlaces() { beginArray(BRA_MAP_PLACES); }
    void endPlaces() { endArray(); }
    
    // each place

    void beginPlace() { beginObj(); }
    void endPlace() { end(); }

    void placeID(int id) { tagInt(BRA_MAP_ID, id); }
    void placeName(const char* name) { tagString(BRA_MAP_NAME, name); }
    void placeName(const string& name) { tagString(BRA_MAP_NAME, name); }
    void placeDark(int v) { tagBool(BRA_MAP_DARK, v); }
    void placeIndicatorUp(int v) { tagBool(BRA_MAP_IND_UP, v); }
    void placeIndicatorDown(int v) { tagBool(BRA_MAP_IND_DOWN, v); }
    
    void placeX(int v) { tagInt(BRA_MAP_GRIDX, v); }
    void placeY(int v) { tagInt(BRA_MAP_GRIDY, v); }

    void beginExits() { beginArray(BRA_MAP_EXITS); }
    void endExits() { endArray(); }

    // number of items in room (to show)
    void placeItemCount(int v) { if (v > 0) tagInt(BRA_MAP_ITEMCOUNT, v); }
    
    void exit(int v)
    {
        commaInc();
        _emit(v);
    }
};
