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

/* Puzzle and PuzzleManager are used to implement game-specific puzzle
 * solving.
 *
 * all game specific code is in puzzle.cpp, which can also be included
 * in the standalone CLI, providing some stub functions are implemented
 * (see puzzle.cpp STANDALONE).
 *
 * puzman.cpp is not included in cli, as it provides the linkage to
 * ifmagnetic.
 *
 */

#pragma once

#include <map>
#include "emux.h"
#include "strutils.h"
#include "utils.h"
#include "growbuf.h"

// forward
struct PuzzleManager;
class IFMagnetic;

struct PuzzleBase
{
    // types and functions for puzzles types
    
    typedef IItem::IItems IItems;
    typedef std::string string;

    PuzzleManager*      _pm;
    bool                _res;
    
    PuzzleBase(PuzzleManager* pm = 0) : _pm(pm)
    {
        _res = true;
    }
    
    static IItem has(const string& s)
    {
        // do you have item `s`
        
        IItems items;

        // parse and resolve by adjectives
        IItem::parse(items, s);

        // must also be local
        IItem::resolveLocal(items);
        return items.size() > 0 ? items[0] : IItem();
    }
    
    static IItem has(const char* s) 
    {
        string s1(s);
        return has(s1); 
    }

    static bool way(int room1, int room2)
    {
        // can you go from room1 to room2
        IItem r1 = IItem::getRoom(room1);
        if (r1)
        {
            int exits[ITEM_MAX_EXITS];
            int n = r1.getExitRooms(exits);
            for (int i = 0; i < n; ++i)
                if (exits[i] == room2) return true;
        }
        return false;
    }

    static bool worn(const char* s) { return IItem(s).isWorn(); }

    static bool connected(IItem a, IItem b) 
    {
        // are two things connected
        IItems items;
        a.getConnectedSystem(items);
        return contains(items, b);
    }

    static bool notConnected(IItem a, IItem b)
    {
        // two things present, but not connected
        return a && b && !connected(a, b);
    }
    
    static bool connected(IItem a, IItem b, IItem c)
    {
        // are three things connected together
        IItems items;
        a.getConnectedSystem(items);
        return contains(items, b) && contains(items, c);
    }

    static int connections(IItem a) 
    {
        // connections from this object is connected set size - 1
        IItems items;
        return a.getConnectedSystem(items) - 1;
    }

    // no checking
    void _action(const char* s);
    void _action(const string& s) { _action(s.c_str()); }

    void action(const char* s)
    {
        // first fail prevents further actions
        if (_res) _action(s);
    }

    void action(const string& s) { action(s.c_str()); }

    void actIsolate(IItem a)
    {
        // make an item to-hand: untie it, remove if garment
        if (!_res) return;

        IItems items;        
        _res = a;
        if (a.getConnectedSystem(items) > 1)
        {
            // break apart by untying tie-ables
            for (size_t i = 0; i < items.size() && _res; ++i)
            {
                IItem ii = items[i];
                if (ii.isTieable()) action(string("untie ") + ii.toString());
            }
        }
        
        if (a.isWorn()) action(string("remove ") + a.toString());
    }

    void actGet(IItem a)
    {
        _res = a;
        if (!a.carried(true)) action(string("get ") + a.toString());
    }

    IItem actIsoGet(IItem a)
    {
        actIsolate(a);
        actGet(a);
        return a;
    }

    void actWear(IItem a)
    {
        actGet(a);
        if (!a.isWorn()) action(string("wear ") + a.toString());
    }

    void actDrop(IItem a, bool force = false)
    {
        _res = a;
        if (_res)
        {
            _res = a.carried(true);
            actIsolate(a);
            
            string cmd;
            if (a.isBrittle()) cmd = "carefully ";
            cmd += "drop " + a.toString();
            if (_res || force) _action(cmd);
        }
    }
};


struct Puzzle: public PuzzleBase
{
    typedef PuzzleBase parentT;
    
    int                 _state;
    string              _name;

    Puzzle(const char* name)
    {
        _state = 0;
        _name = name;
    }

    virtual ~Puzzle() {}
    virtual void action(int act) {}
    virtual void run() = 0;
    void reset() { _state = 0; }
    
    virtual bool precondition() 
    {
        // default
        return has(_name).isExplored();
    }

    virtual bool match(const char* name) const
    {
        return equalsIgnoreCase(_name, name);
    }

    bool state(int mmin, int mmax)
    {
        // mmin must be set, and mmax must not be set
        return (!mmin || (_state & mmin) == mmin) && (_state & mmax) == 0;
    }

    void state(int m)
    {
        _state |= m;
    }

    // emit text to game 
    void text(const char* s, int st = 0);

    // game perform action
    void _action(const char* s, int st = 0)
    {
        parentT::action(s);
        if (_res) state(st);
    }
    
    void _action(const string& s, int st = 0) { return _action(s.c_str(), st); }
};

struct PChooser
{
    struct Seq: public std::vector<const char*>
    {
        void add(const char* c) { push_back(c); }
    };
        
    typedef std::map<IItem, Seq> PState;
    
    PState              _pstate;

    const char* choose(IItem item, const Seq& s)
    {
        const char* choice = 0;
        
        Seq& si = _pstate[item];

        // if the given sequence has an element not already
        // chosen, select that element and add it to the choices made
        for (size_t i = 0; i < s.size(); ++i)
        {
            if (!contains(si, s[i]))
            {
                choice = s[i];
                si.add(choice);
                break;
            }
        }

        if (!choice)
        {
            // all choices are already used.
            // choose oldest (is first), and move that choice to end
            for (Seq::iterator it = si.begin(); it != si.end(); ++it)
            {
                if (contains(s, *it))
                {
                    choice = *it;
                    si.erase(it);
                    si.add(choice); // add back onto end
                    break;
                }
            }
        }
        return choice;
    }

    void clear() { _pstate.clear(); }

 };

struct PuzzleManager: public PuzzleBase
{
    typedef std::list<Puzzle*> Puzzles;

    Puzzles             _puzzles;
    IFMagnetic*         _host;
    bool                _triggerUndo = false;
    PChooser            _pchooser;

    ~PuzzleManager() { purge(_puzzles); }

    void start(IFMagnetic* host);

    Puzzle* find(const char* name) const
    {
        for (Puzzles::const_iterator it = _puzzles.cbegin();
             it != _puzzles.cend(); ++it)
            if ((*it)->match(name)) return *it;
        
        return 0;
    }

    void add(Puzzle* p)
    {
        // we own p
        p->_pm = this;
        _puzzles.push_back(p);
        LOG4("added puzzle: ", p->_name);
    }

    void text(const char* s);
    bool action(const char*);
    bool goroom(int);
    void moveUpdate();
    
    // handle game specific message triggers
    const char* messageHook(int m);

    void reset()
    {
        LOG3("puzzman, ", "reset");
        for (Puzzles::iterator it = _puzzles.begin(); 
             it != _puzzles.end(); 
             ++it) 
            (*it)->reset();

        // erase item choice states
        _pchooser.clear();
    }

    string evalUseXwithYSpecial(IItem xi, IItem yi);

    // like IItem.toString(), but game specific
    string itemToStringSpecial(IItem);

    void applyGameFixes();
    void buildProductInfoJSON(GrowString&, const string& credits);
};

inline void PuzzleBase::_action(const char* s)
{
    _res = _pm->action(s);
}

inline void Puzzle::text(const char* s, int st)
{
    assert(_pm);
    _pm->text(s);
    _pm->text("\n");
    state(st);

    // emitting text marks state as false to signal something was output
    _res = false;
}


      
