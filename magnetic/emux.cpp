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

#include <iostream>
#include <assert.h>

#include "map.h"
#include "sym.h"

#ifdef DISASM
#include "disasm.h"
#endif

Diction IItem::_nouns;
Diction IItem::_adjectives;

// the prog symbol table
Symbols syms;

void IItem::parse(IItems& items, const string& phrase)
{
    std::vector<string> words;

    // first try a full resolve for names with spaces and underscores
    phraseResolve(items,phrase);
    if (items.size() == 1)
    {
        LOG3("Exact resolve found for ", phrase);
        return;
    }
    
    split(words, trim(phrase));
    
    size_t n = words.size();
    if (n > 0)
    {
        findItems(words[n-1], items);

        //LOG4("IItem::parse1 \"", words[n-1] << "\"-> " << items << ", " << items.size() << " items");

        if (items.size() > 1)
        {
            // foreach item, check it has this adjective, if given
            for (size_t i = 0; i < n-1; ++i)
                resolveAdjective(items, findAdjective(words[i]));
        }
    }
}

void IItem::phraseResolve(IItems& items, const string& phrase)
{
    // find an "underscore" phrase in dictionary
    if (phrase.find(' ') != string::npos || phrase.find('_') != string::npos)
    {
        // replace any spaces with underscore for dictionary match
        string p = replaceAll(toLower(phrase), ' ', '_');
        
        // find complete phrase
        findItems(p, items);
    }
}

bool IItem::parse(const string& phrase)
{
    // find best match for phrase.
    // will resolve first by adjective, then by location if multiple matches
    // Note: result may still be non-local
    
    IItems items;
    
    // parse root word resolving by any given adjectives
    // also checks for exact resolve if phrase has spaces or underscore
    parse(items, phrase);
    
    int n = items.size();
    if (n > 0)
    {
        IItem best = items[0];

        // reduce to local items
        resolveLocal(items);
        
        n = items.size();
        if (n > 1)
        {
            // more than one locally. 
            LOG3("emux, warning multiple item matches for ", phrase); 
        }
        if (n >= 1)
        {
            best = items[0];
        }

        _item = best._item;
    }

    if (!_item)
    {
        LOG3("emux, failed to resolve item \"", phrase << "\"");
    }
    
    return n >= 1;
}

IItem IItem::locationTop(bool ignoreout) const
{
    IItem top;
    if (*this)
    {
        uint head = loc_head();
        uint parent = loc_data();

        // out
        if (!ignoreout && (head & 0x80)) return top;

        // pseudos have no top (even though they do have rooms)
        if (pseudo()) return top;
        
        if (!head)
        {
            if (!parent)
            {
                // carried, use the player's location
                top = currentRoom();
            }
            else
            {
                // parent is a room#
                top = getRoom(parent);
            }
        }
        else 
        {
            if (head & 0x40)
            {
                // is room, just return itself
                top = *this;
            }
            else
            {
                // intrinsic or worn by player
                // or related to player
                if ((head & 0x0c) || !parent)
                {
                    // then where the player is
                    top = currentRoom();
                }
                else
                {
                    IItem p(find_item(parent));
                    if (p)
                        top = p.locationTop();
                }
            }
        }
    }
    return top;
}

bool IItem::_spine(IItem x, IItem y, bool xray)
{
    // the spine of X includes y
    // player is null y.
    // if xray, find hidden things and things in closed containers
        
    //LOG3("spine ", x << " to " << y);

    if (x)
    {
        uint head = x.loc_head();
        uint parent = x.loc_data();

        if (head & 0x80) return false; // reject if out
        
        if (x == y) return true;

        if (x.pseudo())
        {
            const uchar* pp = get_psudat();
            if (pp)
            {
                pp += parent; // parent is data offset
                while (*pp)
                {
                    IItem ri = IItem::getRoom(*pp);
                    //LOG3("pseudo ", x << " at " << ri << " target " << y);
                    if (ri && ri == y) return true;
                    ++pp;
                }
            }
            else
            {
                //LOG2("spine, no pseudat! ", x);
            }
            return false;
        }
        
        if (!head)
        {
            IItem room;
                
            if (!parent)
            {
                // simply located on player
                if (!y) return true;

                // otherwise continue to the player's location
                room = currentRoom();
            }
            else
            {
                // parent is a room#
                room = getRoom(parent);
            }
                
            // object from room#
            return _spine(room, y, xray);
        }
        else if ((head & 0xc0) == 0) // ignore out or room
        {
            // not here if hidden (unless xray)
            if (xray || (head & 0x02) == 0) 
            {
                if (!y)
                {
                    // intrinsic or worn by player
                    if (head & 0x0c) return true;
                }

                if (parent)
                {
                    IItem p(find_item(parent));
                    assert(p);

                    bool inClosed = false;

                    if (!xray)
                    {
                        inClosed = x.isContained() && p.isClosed() && !p.isLocalContents();
                    }
                    
                    // if we're in a closed container, stop the spine
                    if (!inClosed)
                    {
                        bool r = _spine(p, y, xray);
                        if (r)
                        {
                            // if parented to a pseudo, we can have a secondary locator
                            // this means it is only in one of the rooms of the pseudo.
                            if (p.pseudo())
                            {
                                uint ps = x.loc_pseudo();
                                if (ps && y.isRoom())
                                {
                                    r = ps == y.roomNumber();
                                }
                            }
                            
                        }
                        return r;
                    }

                }
                else
                {
                    if (!y) return true; // related to player
                    return _spine(currentRoom(), y, xray);
                }
            }
        }
    }
    return false;
}

void IItem::initDiction()
{
    // add nouns
    for (size_t i = 0; i < itemCount; ++i)
    {
        Item* ii = items + i;

        // some dictionary "words" have spaces (or underscore)
        // like "jerry lee lewis".
        // keep last segment of underscored word

        const char* w = (const char*)ii->word.name;
        const char* w1 = strrchr(w, ' ');
        if (w1) w = w1 + 1; // word after last space
        _nouns.add(Diction::WordOwn(w, ii->id));
    }

    // add adjectives
    for (size_t i = 0; i < adjectiveCount; ++i)    
    {
        Word* ai = adjectives + i;
        _adjectives.add(Diction::WordOwn((const char*)ai->name, i + 1));
    }

    //std::cout << _nouns << std::endl;
    //std::cout << _adjectives << std::endl;
    
}

extern "C"
{
    
int emu_word_subst(int gameid, int section, Word* w, const char* p, int len)
{
    if (gameid != 1) return 0;
    
    /* what is going on here?
     *
     * Turns out that the adjectives for some game items are dummy strings
     * for example the "lead lumps" are "S9 lumps" and S9 really is an adjective
     * But "lead lumps" works because lead is a synonym for S9.
     *
     * The plan was to consult the synonym table and automatically replace these
     * bogus words. For instance, there are S1 through S9.
     * BUT access to the adjective table is not provided for version 0.
     * Otherwise it could be done.
     *
     * So, the fallback (which is not very clean) is to just substitute
     * these S* values for their appropriate words in our emux dictionary.
     *
     * This is done here, driven by the table below.
     */

    static const char* subTable[] =
        {
            "S9", "lead",  // lead lumps
            //"long", "long grain",
            0,
        };

    if (!len) return 0; // bail

    const char** si;

    for (si = subTable; *si; si += 2)
    {
        int i;
        const char* q = p;
        const char* s = *si;
        
        for (i = 0; i < len-1; ++i)
        {
            if (!*s || *s != *q) break;
            ++s;
            ++q;
        }

        // input string is high bit terminated
        if ((*q & 0x7f) == *s)
        {
            // match
            w->name = (type8*)strdup(si[1]);

            LOG3("word subst: ", *si << " -> " <<  w->name << " section " << section);
            return 1;
        }
    }
    
    return 0;
}

void init_items()
{
    // called from emuc when items initially constructed.
    // NB: engine thread

    // initialise the map grid
    GameMap::theMap.build_map_poslist(get_game());

    // build a dictionary of nouns from the items list
    IItem::initDiction();
}

void init_syms(FILE* fp)
{
    int c = syms.load_syms(fp);
    if (c)
    {
        LOG4("MS, loaded symbols ", (int)syms.size());
    }
    else
    {
        LOG2("MS, WARNING: prog symbols not present", "");
    }
}

int get_sym_value(const char* s)
{
    int addr = syms.get_value(s);
    if (!addr) 
    {
        LOG3("MS WARNING: unable to locate symbol, ", s);
    }
    return addr;
}
    
}; // "C"

using namespace std;

static void show_exits_and_entrances(IItem room, bool exits, bool entrance)
{
    if (room)
    {
        cout << room << "(" << room.loc_data() << ") :\n";
        if (exits)
        {
            cout << "exits:\n";
            for (int j = 0; j < IItem::dir_count; ++j)
            {
                IItem ej = room.getExit(j);
                if (ej)
                {
                    cout << "\t" << IItem::dirName(j) << " " << ej << "(" << ej.loc_data() << ")" << endl;
                }
            }

#if 0
            cout << "\traw exit data: ";
            for (int j = 0; j < IItem::dir_count; ++j)
            {
                unsigned int v = room._item->exitData[j];
                cout << std::hex << v << " ";
            }
            cout << std::dec << endl;
#endif
            
        }
        if (entrance)
        {
            cout << "entrances:\n";
            for (size_t i = 1; i <= maxRoom; ++i)
            {
                IItem ri = IItem::getRoom(i);
                if (!ri) continue;
                for (int j = 0; j < IItem::dir_count; ++j)                
                {
                    IItem ej = ri.getExit(j);
                    if (ej == room)
                    {
                        cout << "\t" << IItem::dirName(j) << " from " << ri << "(" << ri.loc_data() << ")" << endl;                        
                    }
                }
            }
        }
    }
}

#if defined(LOGGING) && !defined(NDEBUG) && !defined(DEBUG)
#define DEBUG
#endif

int handle_test_commands(const char* cmd)
{
    int res = 0;

    if ((res = !strncmp("seed", cmd, 4) != 0))
    {
        ms_seed(atoi(cmd+5));
        return 1;
    }
    
#ifdef DEBUG
    if ((res = !strcmp(cmd, "inv")) != 0)   // #inv
    {
        cout << IItem::getInventoryString() << endl;
    }
    else if ((res = !strcmp(cmd, "exits")) != 0)   // #exits
    {
        for (size_t i = 1; i <= maxRoom; ++i)
            show_exits_and_entrances(IItem::getRoom(i), true, false);
    }
    else if ((res = !strcmp(cmd, "map")) != 0)   // #map
    {
        for (size_t i = 1; i <= maxRoom; ++i)
        {
            IItem ri = IItem::getRoom(i);
            if (!ri) continue;

            int room_no = ri.loc_data();

            if (ri.isExplored())
                cout << ri << " at " << GameMap::theMap.at(room_no-1) << "\n";
        }
        IItem ri = IItem::currentRoom();
        if (ri)
        {
            cout << "You are here: " << ri << "(" << ri.loc_data() << ") \n";
            show_exits_and_entrances(ri, true, true);
        }
        else
            cout << "location unknown!\n";
    }
    else if ((res = !strncmp("go ", cmd, 3)) != 0)  // #go
    {
        // teleport to location
        const char* place = cmd + 3;

        IItem where;

        // can be word or number
        int rn = atoi(place);
        if (rn > 0) where = IItem::getRoom(rn);
        else where = IItem(place); // try to parse location
        
        if (where)
        {
            int r = where.roomNumber();
            if (r)
            {
                set_current_room(r);
                where.setExplored();
                where.setLit();
                cout << " teleported to " << where << endl;
            }
            else cout << where << " is not a room!\n";
        }
        else cout << "unknown location '" <<  place << "'\n";
    }
    else if ((res = !strncmp("goto", cmd, 4) != 0))
    {
        int target = atoi(cmd+5);
        cout << GameMap::theMap.find_path(get_current_room(),target);
    }
    else if ((res = !strcmp("gettables", cmd) != 0))
    {
        IItem::IItems items;
        IItem::getCarryableItems(items);
        cout << items << "\n";
    }
#ifdef DISASM
    else if ((res = !strncmp("dasm", cmd, 4) != 0))
    {
        DisAssembly* d = reinterpret_cast<DisAssembly*> (disassem);
        if (strlen(cmd)>5)
            d->dump_dasm(cmd+5);
        else
            d->dump_dasm(0);
    }
    else if ((res = !strncmp("dgaps", cmd, 5) != 0))
    {
        DisAssembly* d = reinterpret_cast<DisAssembly*> (disassem);
        d->get_absaddrlist();
        d->find_unusedtargets();

        if (strlen(cmd)>6)
            dasm_fillgaps(d->getUTArray(),d->getUTNum(),cmd+6);
        else
            dasm_fillgaps(d->getUTArray(),d->getUTNum(),0);
    }
#endif 
#endif // debug    

    return res;
}


