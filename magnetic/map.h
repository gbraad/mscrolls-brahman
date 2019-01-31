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

#include <string>
#include <vector>
#include <iostream>

#include "emux.h"
#include "point.h"

struct GameMap
{
    typedef std::string string;
    
    static GameMap theMap;

    struct MapPos
    {
        int room;
        Point2 pos; // grid position
        int level; // map area
        
        MapPos(int r, int xpos, int ypos, int l) :
            room(r), pos(xpos, ypos), level(l) {}

        bool operator < (const MapPos& m) const { return room < m.room; }

        friend std::ostream& operator<<(std::ostream& os, const MapPos& mp)
        { return os << "room: " << mp.room << "; " << mp.pos << "; " << mp.level; }
    };

    size_t size() const { return room_mpos.size(); }
    const MapPos& at(int i) const { return room_mpos[i]; }

    void build_map_poslist(int gameid);

    bool _not_visited(int node, const std::vector<int>& path)
    {
        for(size_t i=0;i<path.size();++i)
        {
            if(path[i]==node)
                return false;
        }
        return true;
    }

    string find_path(int src, int dest);

private:
    
    std::vector <MapPos> room_mpos;

};



