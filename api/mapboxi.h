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
#include <assert.h>
#include "imptypes.h"
#include "control.h"
#include "mapbox.h"
#include "logged.h"
#include "rect.h"
#include "cutils.h"
#include "strutils.h"
#include "graph.h"
#include "ifmap.h"
#include "json.h"

struct Map::Imp : public ImpType<Map>
{
    Imp(Map* host) : ImpType(host) { _init(); }

    typedef Graph<string>       MapG;
    typedef Map::Path           Path;

    MapG                _mg;
    Rect                _bounds;
    string              _currentPlace; // id
    uint                _currentExits;

    // optional map background
    string              _backimage;
    
    MapInfo             _mi;
    
    struct Place: public MapG::Node
    {
        typedef MapG::Node parentT;

        Place(const string& id) : parentT(id) {}
        
        string          _name;
        Rect            _pos;
        int             _itemCount = 0;
        bool            _dark = false;
        bool            _indUp = false;
        bool            _indDown = false;
        
    };

    struct Link: public MapG::Connection
    {
        typedef MapG::Connection parentT;
        Link(Place* p1, Place* p2) : parentT(p1, p2) {}

        Point2  _fromPt;
        Point2  _toPt;

        bool valid() const { return _fromPt && _toPt; }

        void invalidate()
        {
            _fromPt = Point2();
            _toPt = Point2();
        }
    };

    typedef std::vector<Link*> Links;

    Place& _place(int i) { return *(Place*)_mg.at(i); }
    const Place& _place(int i) const { return *(const Place*)_mg.at(i); }
    
    Link& _link(int i) { return *(Link*)_mg.con(i); }
    const Link& _link(int i) const { return *(const Link*)_mg.con(i); }

    static bool linkPresent(MapG& mg, Link& l)
    {
        // if there's a link with the same in and out points
        for (size_t i = 0; i < mg.csize(); ++i)
        {
            Link* li = (Link*)mg.con(i);
            if (li->_fromPt == l._fromPt && li->_toPt == l._toPt) return true;
        }
        return false;
    }

    void makeLayout(MapG& mg, int w, int h, int gap, JSON& js)
    {
        // rebuild map graph (mg) from JSON map description
        // arrange layout as boxes (w, h) separated by gap.
        // these are relative units that are later scaled by the UI
        
        JSON::ArrayValue jplaces(js, BRA_MAP_PLACES);
        int sz = jplaces.size();

        // erase any preview map graph
        mg.clear();

        // add the places
        for (int i = 0; i < sz; ++i)
        {
            const JSON::Value& ji = jplaces.at(i);
            int x = JSON::toInt(JSON::at(ji, BRA_MAP_GRIDX));
            int y = JSON::toInt(JSON::at(ji, BRA_MAP_GRIDY));
            string id = JSON::toString(JSON::at(ji, BRA_MAP_ID));
            string name = JSON::toString(JSON::at(ji, BRA_MAP_NAME));
            int itemCount = JSON::toInt(JSON::at(ji, BRA_MAP_ITEMCOUNT));

            if (id.empty())
            {
                LOG1("warning: map entry with no ID ", name);
                continue;
            }

            if (name.empty())
                name = _host->_control->nameFromId(id);

            name = CapitaliseStartWords(name);
            
            // false if absent
            bool dark = JSON::toBool(JSON::at(ji, BRA_MAP_DARK));

            // up/down indicators?
            bool indUp = JSON::toBool(JSON::at(ji, BRA_MAP_IND_UP));
            bool indDown = JSON::toBool(JSON::at(ji, BRA_MAP_IND_DOWN));

            x = x*(w + gap);
            y = y*(h + gap);

            Place* p = new Place(id);
            p->_pos = Rect(x, y, w, h);
            p->_name = name;
            p->_itemCount = itemCount;
            p->_dark = dark;
            p->_indUp = indUp;
            p->_indDown = indDown;

#if defined(LOGGING) && !defined(NDEBUG)
            // room# to name 
            if (Logged::_logLevel > 3)
            {
                p->_name += " (";
                p->_name += id;
                p->_name += ')';
            }
#endif
            mg.add(p);
        }

        Links uglies;

        // go through the links.
        // calculate the start and end point for each link
        for (int i = 0; i < sz; ++i)
        {
            const JSON::Value& ji = jplaces.at(i);
            const JSON::Value& links = JSON::at(ji, BRA_MAP_EXITS);

            // no exit specification
            if (!links.IsArray()) continue;
            
            string id = JSON::toString(JSON::at(ji, BRA_MAP_ID));
            Place* p1 = (Place*)mg.findNode(id);
            if (!p1) continue; // eg missing id

            Rect pos1 = p1->_pos;
            Point2 p1c = pos1.centre();

            for (JSON::Value::ConstValueIterator it = links.Begin();
                 it != links.End(); ++it)
            {
                string li = JSON::toString(*it);
                Place* p2 = (Place*)mg.findNode(li);

                if (!p2)
                {
                    // link destination not found, for example,
                    // if destination not in the known set.
                    continue;
                }

                Rect pos2 = p2->_pos;
                Point2 p2c = pos2.centre();
                
                Link* l = new Link(p1, p2);
                
                bool rightof = p2c.x > p1c.x;
                bool leftof = p2c.x < p1c.x;
                bool xof = !leftof && !rightof;

                bool topof = p2c.y < p1c.y;
                bool bottomof = p2c.y > p1c.y;
                bool yof = !topof && !bottomof;
                
                if (rightof && yof)
                {
                    // A -> B
                    l->_fromPt = pos1.centreRight();
                    l->_toPt = pos2.centreLeft();
                }
                if (leftof && yof)
                {
                    // B <- A
                    l->_fromPt = pos1.centreLeft();                    
                    l->_toPt = pos2.centreRight();
                }
                if (topof && xof)
                {
                    // B
                    // ^
                    // A
                    l->_fromPt = pos1.centreTop();
                    l->_toPt = pos2.centreBottom();
                }
                if (bottomof && xof)
                {
                    // A
                    // v
                    // B
                    l->_fromPt = pos1.centreBottom();
                    l->_toPt = pos2.centreTop();
                }
                if (rightof && topof)
                {
                    //    B
                    //   /
                    //  A
                    l->_fromPt = pos1.topRight();
                    l->_toPt = pos2.bottomLeft();
                }
                if (rightof && bottomof)
                {
                    /*
                       A
                        \
                         B
                    */
                    l->_fromPt = pos1.bottomRight();
                    l->_toPt = pos2.topLeft();                    
                }
                if (leftof && topof)
                {
                    /*
                       B
                        \
                         A
                    */
                    l->_fromPt = pos1.topLeft();
                    l->_toPt = pos2.bottomRight();
                }  
                if (leftof && bottomof)
                {
                    //    A
                    //   /
                    //  B
                    l->_fromPt = pos1.bottomLeft();
                    l->_toPt = pos2.topRight();
                }

                // sometimes multiple exits go to the same place.  
                if (linkPresent(mg, *l))
                {
                    delete l;
                    continue;
                }
                
                Point2 dp = (l->_toPt - l->_fromPt).abs();
                float grad = dp.x ? (float)dp.y/dp.x : 0;

                // some links look ugly
                // horizontal and vertical are good
                // but low or high slopes are bad
                // grads of 3:1 or 1:3 or worse should be removed if pos
                bool ugly = grad != 0 && (grad < 0.3f || grad > 3.5f);

                bool ok = true;

                // see if the link crosses any other boxes
                for (size_t j = 0; j < mg.size(); ++j)
                {
                    const Place* pj = (const Place*)mg.at(j);

                    // ignore the start and end of the link itself
                    if (p1->_id == pj->_id || p2->_id == pj->_id) continue;

                    if (pj->_pos.intersectsLine(l->_fromPt, l->_toPt))
                    {
                        LOG4("map line crosses box from location, ", p1->_id << "->" << p2->_id);
                        ok = false;
                        break;
                    }
                }

                if (ok && !ugly)
                {
                    //LOG3("adding link, ", p1->_id << "->" << p2->_id);
                    mg.add(l);
                }
                else
                {
                    // rejected links (that cross boxes) are invalidated
                    // and added to ugly, to be sorted later.
                    if (!ok) l->invalidate();
                    
                    // in theory we are ok, but we might look poor
                    // if we are the only link, then we have to be drawn
                    // otherwise it does no harm to hide the ugly connections

                    // keep these to one side.
                    // later make a second pass to allow some back in
                    uglies.push_back(l);

                    if (ugly)
                    {
                        //Point2 dp = (l->_toPt - l->_fromPt).abs();
                        //float grad = dp.x ? (float)dp.y/dp.x : 0;
                        //LOG4("ugly link, ", p1->_id << "->" << p2->_id << " grad: " << grad);
                    }
                }
            }
        }

        // deal with the ugly (and rejected) connections
        Links::iterator it = uglies.begin();
        while (it != uglies.end())
        {
            Link* li = *it;
            if (mg.isConnected(li->_from, li->_to))
            {
                // don't need it
                delete li;
            }
            else
            {
                LOG4("adding ugly/rejected link, ", li->_from->_id << "->" << li->_to->_id);
                mg.add(li); //  owned
            }
            it = uglies.erase(it);
        }
        
        // for A->B and B->A, there will be two links
        // replace this by a single bi-directional links
        size_t cn = mg.csize();
        for (size_t i = 0; i < cn; ++i)
        {
            Link* li = (Link*)mg.con(i);

            // back-link?
            Link* c = (Link*)mg.findConnection(li->_to, li->_from);

            if (c)
            {
                // connection must be later (ie > i), so we can just
                // remove it and adjust loop count
                mg.remove(c);
                --cn;

                // upgrade to bi-directional
                li->_bi = true;
            }
        }
    }

    JSON* updateWholeMap(MapInfo& mi)
    {
        JSON* js = 0;
        
        // request full map data
        mi._full = true;
        
        // if false, we have no map info from the engine
        bool v = _host->_control->updateMapInfo(mi);

        if (v)
        {
            if (mi._changed)
            {
                // build live JSON from string
                js = JSON::fromString(mi._json.c_str());

                if (!js)
                {
                    LOG1("updateWholemap, bad json '", mi._json << "'");
                }
            }
        }

        return js;
    }

    bool performLayout(int w, int h, int gap)
    {
        JSON* js = updateWholeMap(_mi);

        // update our idea of where the player is
        bool changed = updateMapLocation(_mi);
        
        if (js)
        {
            changed = _mi._changed;
            if (changed)
            {
                // rebuild graph
                makeLayout(_mg, w, h, gap, *js);

                //LOG3("API, map changed rebuild JSON ", _mg.size())
                    
                // recalculate overall bounds
                Rect r;
                for (size_t i = 0; i < _mg.size(); ++i)
                    r = _place(i)._pos.combine(r);            
                
                if (r._x || r._y)
                {
                    Point2 dp(-r._x, -r._y);
                    
                    for (size_t i = 0; i < _mg.size(); ++i)
                        _place(i)._pos.moveBy(dp);

                    for (int i = 0; i < linkCount(); ++i)
                    {
                        Link& li = _link(i);
                        li._fromPt += dp;
                        li._toPt += dp;
                    }
                    
                    // new bounds is zero aligned
                    r = Rect(0, 0, r._w, r._h);
                }
                
                _bounds = r;
            
                // back image?
                string bg = JSON::StringValue(*js, BRA_MAP_BACKIMAGE).toString();
                _backimage = _host->_control->resolveAsset(bg);
            }

            delete js;
        }
        return changed;
    }

    int boxCount() const { return (int)_mg.size(); }
    int linkCount() const { return _mg.csize(); }
    int width() const { return _bounds._w; }
    int height() const { return _bounds._h; }
    
    string boxname(int i) const 
    { return i < boxCount() ? _place(i)._name : string(); } 

    Rect box(int i) const
    { return i < boxCount() ? _place(i)._pos : Rect(); }
    
    string boxID(int i) const
    { return i < boxCount() ? _place(i)._id : string(); }

    int boxItemCount(int i) const
    { return i < boxCount() ? _place(i)._itemCount : 0; }

    bool boxDark(int i) const
    { return i < boxCount() ? _place(i)._dark : false; }        

    bool boxIndUp(int i) const
    { return i < boxCount() ? _place(i)._indUp : false; }        

    bool boxIndDown(int i) const
    { return i < boxCount() ? _place(i)._indDown : false; }        
    
    Point2 linkFrom(int i) const
    { return i < linkCount() ? _link(i)._fromPt : Point2(); }

    Point2 linkTo(int i) const
    { return i < linkCount() ? _link(i)._toPt : Point2(); }

    bool linkBi(int i) const
    { return i < linkCount() ? _link(i)._bi : false; }

    bool linkVisible(int i) const
    { return i < linkCount() ? _link(i).valid() : false; }

    int boxIndexForID(const string& id) const
    {
        // find the box index for the box with the given id
        int n = boxCount();
        for (int i = 0; i < n; ++i) if (_place(i)._id == id) return i;
        return -1;
    }

    bool updateMapLocation(MapInfo& mi)
    {
        bool changed = false;

        if (_currentPlace != mi._currentLocation)
        {
            changed = true;
            _currentPlace = mi._currentLocation;
        }

        if (_currentExits != mi._currentExits)
        {
            changed = true;
            _currentExits = mi._currentExits;
        }
        
        return changed;
    }

    bool pathTo(const string& place, Path& p) const
    {
        MapG::Path ppath;

        Place* from = (Place*)_mg.findNode(_currentPlace);
        Place* to = (Place*)_mg.findNode(place);
        bool res = _mg.shortestPath(from, to, ppath);
        if (res)
        {
            for (size_t i = 0; i < ppath.size(); ++i)
                p.push_back(ppath[i]->_id);
        }
        return res;
    }

    void _init() 
    {
        _currentExits = 0;
    }
};





