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
 *

Documentation of data returned by do_findprop:

Old floppy disks present...

        The original "noundata" wall-chart!

Byte #0; Noun States:

        ---------------------------------
        | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
        ---------------------------------
          |   |   |   |   |   |   |   Plural
          |   |   |   |   |   |   Dead
          |   |   |   |   |   Broken
          |   |   |   |   Local Contents,
          |   |   |   |     The contents of such a container are `valid' when
          |   |   |   |     it is closed But may not be reachable as far as
          |   |   |   |     process is concerned.
          |   |   |   |     (See Transparent containers & X-Ray verbs).
          |   |   |   Open
          |   |   Closed
          |   Locked
          Lit
           (as a flame)

Byte #1;  Physical Characteristics

        ---------------------------------
        | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
        ---------------------------------
          ------------:   -------------:
            Hardness            Strength

         0,     Void
         1,     Liquid
         2,     A Quantity (requires 'any' such as rice).
         3,     Can be cut with a `sharp' object.
         4,     Can be cut only with a `very sharp' object.
         5,     Normal (may be breakable).
         6-9,   Undefined.
         10,    Brittle; will break if used violently.
                         Can break if `drop'ed if required.
         11,    Tool for breaking other items.
         12,    Flexible, can be tied & used to tie other things.
         13-14, Undefined.
         15,    Unbreakable (and cannot be used for breaking).

Byte #2;  Size & Weight.

        ---------------------------------
        | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
        ---------------------------------
         ------------:   ------------:
                Size           Weight

    The sizes of objects vary coarsely,
    approximates,
    0,      Void.
    1,      like a key or matchbook,
    2,      like a bottle or paperback,
    3       like a bag small box,
    4,  a chair
    5,  a person.
    6-14    not clearly defined. bed ~ 10
    15,     large

        Weight:
        person ~ 5

    If the object is a `room' the sizes mean the following,
    0,  Normal
    1,  Narrow Crevice (must drop all objects).
    2,  confined space (cubicle or booth, only 1 person at a time).
    3,  Low ceiling, no riding vehicles here.
    4-9,    Undefined.
    10, "Whack, you hit your head."
    11, "The `vehicle' stumbles."
    12, "Ridiculous to ride in there".
    13-15,  Undefined message type.

Byte #3;  Shape & Texture

        ---------------------------------
        | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
        ---------------------------------
          :---    |   |   ------------:
      Sharpness   |   |             Texture
                  |   |
    0, Not Sharp  |   |     0,  Void.       7,   Clammy
    1, Blunt      |   |     1,  Smooth      8,   Soft
    2, Sharp      |   |     2,  Rough       9,   Hard
    3, Very Sharp |   |     3,  Hot         10,  Warm
              |   |     4,  Cold        11,  "`npc' doesn't like it"
             Wet  |     5,  Slippery    12,  Thorny
     Wet rooms are    |     6,  Furry       13,  Sticky
    feet in water     |             14,  Ice Cold
     like a stream    |                 15,  Crunchy.
              Movable

Byte #4; Data1


    ---------------------------------
    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    ---------------------------------
          |   |   |   |   |   |   |   Modified Quantity
      |   |   |   |   |   |   |     Something that is really plural but
          |   |   |   |   |   |   |     Singulared by an adjective, such as
          |   |   |   |   |   |   |     'a pair of trousers'
          |   |   |   |   |   |   |
          |   |   |   |   |   |   Description Only (Do not name the item)
      |   |   |   |   |   Short Look, no extras to description.
      |   |   |   |   Money
      |   |   |   Explored
      |   |   Npc's refuse to accept it (Rubbish)
      |   Object is an NPC.
    Lightable
       (lightable & lit  => a flame)
       (!lightable & lit => glowing)



Byte #5; Data2


    ---------------------------------
    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    ---------------------------------
      |   |   |   |   |   |   |   Pseudo
      |   |   |   |   |   |   Initial State Done
      |   |   |   |   |   A Lit Room
      |   |   |   |   Has a surface & can be sat on
      |   |   |   Force Adjective (eg Blue jeans)
      |   |   Force Plural
      |   |     For such things as uncountable nouns, ones taking
      |   |     any/some but not 'are' eg.
      |   |     there IS SOME furniture here.
      |   Has been given to the npc
      |    Who will give it back unless it has value or money.
    Another Noun
      There is another noun in the game with the same name.

The Location
 This is a 32 + 1 bit quantity most of which lives in bytes 6-9. However
 the pseudo bit is in byte #5.
 Format

byte#    6      7       8      9
       --------------------------------------------------
       |HEAD | Secondary Locator | High Byte | Low Byte |
       --------------------------------------------------

 Head
 This determines the relationship between the object and that which
 it is located to.

    ---------------------------------
    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    ---------------------------------
      |   |   |   |   |   |   |   Related
      |   |   |   |   |   |   Hidden, will automatically appear when
      |   |   |   |   |   |       related object is explored.
      |   |   |   |   |   Worn by player
      |   |   |   |   Intrinsic (hands, feet etc)
      |   |   |   |     NB: other npc's are assumed to also have these.
      |   |   |   Inseparable (a permanent feature)
      |   |   Contained
      |   A Room, bytes 8&9 contain the room#.
     Out, The object does not exist at present.

Secondary Locator, Byte #7

    This byte applies only to pseudos.
    Pseudos are items that are effectively related to many rooms
    simultaneously & shouldn't be movable by the player.

    In many cases the player can put things on pseudos & generally
    relate objects to them. In these cases the 'Secondary Locator'
    of such an object contains the room# of the pseudo instance,
    ie. it picks out one of the pseudo occurrences.

Data Field, Bytes 8&9

    These 16 bits can be either a noun#, room# or pointer as follows;

    Case                Description of bytes 8&9

    Item is simply located, Item is in room# 8&9.
    ie, byte #6 == 0

    Item is a room          8&9 contain the room# of the room.

    Item is a pseudo        8&9 point to a zero terminated list
                            of room#'s (possibly read only)

    Item is neither a room nor  8&9 contain the noun# of the object
    a pseudo and not simply     the item is in/on/near etc.
    located

Byte #10; Data3


    ---------------------------------
    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    ---------------------------------
      |   |   |   |   |   --------:
     Clothing Cover   |        Container Volume & flag
      |   |   |   |  Chest          capacity = vol -1
      |   |   |  Head
      |   |  Arms           The Volume must never reduce to zero,
      |  Groin          for otherwise the object is no longer
       Feet & Legs          a container.

       * bit 7 => no article for rooms (put in by Anita)

       GUI BITS
       * bit 6 "gui_show_map" => visible on map (only applies for rooms)

         eg squares of rainbow room in guild. need to show all on map
         before explored. NB: flag set in code not f23 for guild.

       * bit 5 "gui_show_inv" => visible on inventory bar override.

         eg keys in keyring in Jinxter. they are inseparable to suppress 
         description but need to show on inv. but (eg) jeans pocket in pawn 
         does not.

Byte #11, Data4


    ---------------------------------
    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    ---------------------------------
      :-------    :----   ----:   Worn By Npc
      |           |         Wearability
      |           |
      |           |        0,   Can't be worn
      |           |        1,   clothing (normal)
      |           |            For rooms this is no_article
          |           |        2-3, magical protection
      |           |
      |          Combustibility
      |           0,   non-flammable
      |           1,   burns away (like paper)
      |           2,   melts (like wax)
      |           3,   explodes.
      Edibility
       0,   not edible.   4,   edible
       1,   Fatal         5,   tasty
       2,   Poisoned      6,   Very tasty
       3,   become ill.   7,   magical (healing etc.)


Bytes #12,13; Fixtures

    -----------------------------------------------------------------
    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    -----------------------------------------------------------------
      |   |   ------------ Data Field -----------------------------
      |   |
      |   |
      |  mixing liquids (not implemented)
      |
      Fixed permanently, Cannot be untied.


This concludes the 14 bits of data associated with each object.
If the object is an npc, there is an additional 4 bytes of extended data.

Npc Data

Bytes 0&1

    Undefined.


Byte #2

    ---------------------------------
    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    ---------------------------------
      :------------   |   |   |   Sex
     Emotion          |   |   |     0, Female.
       0,  Null           |   |   |     1, Male.
       1,  Happy          |   |   |
       2,  Angry          |   |  Suppress Clothing
       3,  Violent        |  Animal
       4,  Asleep        No Article
       5,  Unconscious
       6,  Peaceful     11, Tired
       7,  Exited       12, Suffering
       8,  Friendly     13, Frightened
       9,  Drunk        14, Cheerful
       10, Sad          15, Mischievous


Byte #3

        ---------------------------------
        | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
        ---------------------------------
      :--------   |   |   |   |   Paused, from scripts & MoveNpc
      Spare.      |   |   |   |
                  |   |   |  Nice, will give you things except
                  |   |   |     clothing, money & valuables.
                  /   |   |
        Covetous     |  Non-pausable,
                      |        Npc is in a rush
                  |
              Quiet,
                 Produce no output from scripts.


**************************************************************************/

#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>
#include <string.h>
#include "cutils.h"
#include "logged.h"
#include "diction.h"
#include "utils.h"
#include "strutils.h"

extern "C"
{
#include "defs.h"
extern type8 prog_format;
int get_sym_value(const char* s);
}

#include "types.h"
#include "timer.h"

struct IItem
{
    typedef std::string         string;

    struct IItems: public std::vector<IItem>
    {
        friend std::ostream& operator<<(std::ostream& os, const IItems& items)
        {
            size_t n = items.size();
            if (n > 0)
            {
                for (size_t i = 0; i < n-1; ++i) os << items[i] << ", ";
                os << items[n-1];
            }
            return os;
        }
    };
    
    Item*               _item;
    static Diction      _nouns;
    static Diction      _adjectives;

    enum Dir 
    {
        dir_n = 0,
        dir_ne,
        dir_e,
        dir_se,
        dir_s,
        dir_sw,
        dir_w,
        dir_nw,
        dir_u,
        dir_d,
        dir_in,
        dir_out,
        dir_count,
    };

    static inline uint _word(uchar* p) { return (((uint)p[0])<<8) | p[1]; }

    static inline void _setWord(uchar* p, int v)
    {
        p[0] = v >> 8;
        p[1] = v;
    }

    static const char* dirName(int dir)
    {
        static const char* dirs[] = { "n", "ne", "e", "se", "s", "sw", "w", "nw", "u", "d" };        
        return (dir >= 0 && dir < dir_count) ? dirs[dir] : "";
    }

    IItem() { _init(); }
    IItem(Item* i)
    {
        _init();
        _item = i;
    }

    IItem(const string& s)
    { 
        _init();
        parse(s);
    }

    IItem(const char* s)
    {
        _init();
        string s1(s);
        parse(s1);
    }

    operator bool() const { return _item != 0; }

    bool operator==(const IItem& i) const { return _item == i._item; }
    bool operator!=(const IItem& i) const { return _item != i._item; }
    bool operator<(const IItem& i) const
    {
        // just sort on pointer
        return _item < i._item;
    }

    void        invalidate() { _item = 0; }

    const char* word() const
    { return _item ? (const char*)_item->word.name : 0; }

    bool wordIs(const char* s) const
    {
        const char* w = word();
        return w && sameWithUnderscore(w, s);
    }

    int adj(int i) const
    {
        return i < ITEM_MAX_ADJ ? _item->adj[i] : 0; 
    }

    bool hasAdj(int id) const
    {
        for (int i = 0; i < ITEM_MAX_ADJ; ++i)
        {
            int ai = _item->adj[i];
            if (!ai) break;
            if (ai == id) return true;
        }
        return false;
    }

    const char* adjString(int i) const
    {
        // get ith adjective, 0 otherwise
        const char* a = 0;
        int ai = adj(i);
        if (ai)
        {
            Word* adj = find_adjective(ai);
            if (adj) a = (const char*)adj->name;
        }
        return a;
    }

    static string toWordCaps(const char* p)
    {
        string s;
        bool first = true;
        while (*p)
        {
            char c = *p++;
            if (c == '_') c = ' '; // eliminate any underscores in names
            if (u_isspace(c)) first = true;
            else
            {
                if (first)
                {
                    c = u_toupper(c);
                    first = false;
                }
            }
            s += c;
        }
        return s;
    }

    bool isInSet(const char * set) const
    {
        int addr = get_sym_value(set);
        assert(addr);
        uchar* itemSet = getcode() + addr;
        for (int i = 0; ; ++i)
        {
            //LOG4("Next item in set: ",_word(itemSet+i*2));
            int iis = _word(itemSet+i*2);
            if (iis==0)
                break;
            if (iis == id())
            {
                LOG4("Item is in set ", set);
                return true;
            }
        }
        return false;

    }

    bool isKey() const
    {
        if (prog_format)
        {
            // is door if in set Denotation_KEYS
            return isInSet("Denotation_KEYS");
        }
        else
        {
            // are we a key?
            // there is no data property for key, so use the word
            const char* w = word();
            return w ? !u_stricmp(w, "key") : false;
        }
    }

    bool isDoor() const
    {
        if (prog_format)
        {
            // is door if in set Denotation_DOORS
            return isInSet("Denotation_DOORS");
        }
        else
        {
            // are we a door?
            return wordIs("door") || wordIs("gate");
        }
    }

    static string toWordCaps(const string& s)
    {
        return toWordCaps(s.c_str());
    }

    string      tnWord() const
    {
        // approximately follow the logic of "p.tn", "the object."
        if (omitArticle()) return adjWordIf();
        else return "the " + adjWordIf();
    }

    string      adjWordIf() const
    {
        // sometimes the root "word" is multiple words (eg rooms)
        // in this case, return that string
        // otherwise, if the root is a single word and we have an adjective
        // add a single adjective.
        
        if (_item)
        {
            const char* w = word();
            if (strchr(w, ' ') || strchr(w, '_')) return toWordCaps(w); // more than one word
            return adjWord();
        }
        return "unknown";
    }

    string      adjWord() const
    {
        // name of item with an adjective (if any)
        int ac = 0;
        string aword;
        const char* a;

        if (_item)
        {
            while ((a = adjString(ac)) != 0)
            {
                if (ac) aword += ' ';
                aword += a;
                ++ac;

                // only use one adjective for now..
                break;
            }

            if (ac) aword += ' ';
            aword += word();
            return toWordCaps(aword);
        }
        return "unknown";
    }

    string      toString() const { return adjWordIf(); }

    friend std::ostream& operator<<(std::ostream& os, const IItem& i)
    { return os << i.toString(); }

    bool pseudo() const
    {
        // are we a pseudo. 
        return _item && _item->data[5] & 1;
    }

    int  id() const { return _item ? _item->id : 0; }
    bool isNPC() const { return _item && _item->data[4] & 0x40; }
    bool isRoom() const { return (loc_head() & 0x40) != 0; }
    bool isExplored() const { return _item && _item->data[4] & 0x10; }
    bool isMoveable() const { return _item && _item->data[3] & 0x10; }
    bool isInseparable() const { return loc_head() & 0x10; }
    bool isLocked() const { return _item && _item->data[0] & 0x40; }
    bool isClosed() const { return _item && _item->data[0] & 0x20; }
    bool isClosedOrLocked() const { return _item && _item->data[0] & 0x60; }
    bool isOpen() const { return _item && _item->data[0] & 0x10; }
    uint volume() const { return _item ? _item->data[10] & 0x7 : 0; }
    void setVolume(int v)
    {
        if (_item)
        {
            if (v > 7) v = 7; // clamp to max
            if (v < 1) v = 1; // clamp to min
            uint t = (_item->data[10] & ~7) | v;
            _item->data[10] = t;
        }
    }
    
    bool isContainer() const { return volume() > 0; }
    uint weight() const { return _item ? _item->data[2] & 0xf : 0; }
    uint size() const { return _item ? _item->data[2] >> 4 : 0; }
    bool isTooHeavy() const
    {
        // zero weights aren't really too heavy, but they're things you
        // can't get.
        int w = weight();
        return w == 0 || w >= 15;
    }
    bool isMoney() const { return _item && _item->data[4] & 0x08; }
    bool isWorn() const { return loc_head() & 0x04; }
    uint hardness() const { return _item ? _item->data[1] >> 4 : 0; }
    bool isHard() const
    {
        // normal kind of hardness, not soft nor flex nor breakable etc
        uint h = hardness();
        return h >= 5 && h < 12;
    }
    bool isContained() const
    {
        // contained, but not a room nor out
        return (loc_head() & 0xE0) == 0x20;
    }
    
    uint strength() const { return _item ? _item->data[1] & 0x0f : 0; }
    uint sharpness() const { return _item ? _item->data[3] >> 4 : 0; }
    uint texture() const { return _item ? _item->data[3] & 0x0f : 0; }
    bool isLiquid() const { return hardness() == 1; }
    bool isTieable() const { return hardness() == 12; }
    bool isBrittle() const { return hardness() == 10; }
    bool isIntrinsic() const { return loc_head() & 0x08; }
    bool isDead() const { return _item && _item->data[0] & 0x02; }
    bool isBroken() const { return _item && _item->data[0] & 0x04; }
    bool isHidden() const { return loc_head() & 0x02; }
    bool isOut() const { return loc_head() & 0x80; }
    bool isWearable() const { return _item && _item->data[11] & 6; }
    bool isEdible() const { return _item && ((uint)(_item->data[11] >> 5) != 0); }
    bool isInitialStateDone() { return _item && _item->data[5] & 0x02; }
    bool isSurface() const { return _item && _item->data[5] & 0x8; }
    bool isBigThing() const
    {
        // object that may or not be gettable, but is big enough to look under
        // NB: can still be inseparable 
        bool ok = !isNPC() || isDead();
        ok = ok && isMoveable();
        if (ok)
        {
            int s = size();
            int w = weight();
            ok = (s >= 8 && s < 15 && w > 5); // && w < 15);
        }
        return ok;
    }
    bool isAnimal() const
    { return isNPC() && _item->npcData && _item->npcData[2] & 4; }
    bool isLocalContents() const { return _item && _item->data[0] & 0x08; }
    bool visibleOnMap() const
    {
        bool v = isExplored();
        // test gui_show_map bit
        if (!v && isRoom()) v = (_item->data[10] & 0x40) != 0;
        return v;
    }

    void setVisibleOnMap(bool v = true)
    {
        if (isRoom())
        {
            if (v) _item->data[10] |= 0x40;
            else _item->data[10] &= ~0x40;
        }
    }

    bool guiShowInv()
    {
        // check show inv override.
        // NB:only applies for non-wearables
        // assume not out
        
        bool v = !isWearable();
        if (v)
        {
            v = (_item->data[10] & 0x20) != 0;
        }
        return v;
    }


    bool omitArticle() const
    {
        bool omit = false;
        if (isNPC())
        {
            if (_item->npcData[2] & 0x08) omit = true;
        }
        else if (isRoom())
        {
            // room omit article flag (not for the pawn)
            if ((_item->data[10] & 0x80)) omit = true;
        }
        return omit;
    }

    IItem container() const
    {
        // if we have a container, return it
        IItem c;
        if (isContained()) c = find_item(loc_data());
        return c;
    }
    

    bool couldGet() const
    {
        // can get, but might already be carried
        return !isRoom() && isMoveable() && !isInseparable() && !isTooHeavy();
    }
    
    bool gettable() const
    {
        // able to get and not already carried
        return couldGet() && !carried(); 
    }

    bool canTieTo() const
    {
        // not exhaustive
        return !isRoom() && !isLiquid() && !isNPC();
    }

    IItem fixedTo() const
    {
        return _item ? find_item(_word(_item->data + 12) & 0x3fff) : 0;
    }

    void setFixedTo(const IItem& f)
    {
        // f can be null to unfix
        if (_item)
        {
            uint t = f.id() & 0x3fff;
            _setWord(_item->data + 12, t);
        }
    }

    void setExplored(bool v = true)
    {
        if (_item)
        {
            if (v) _item->data[4] |= 0x10;
            else _item->data[4] &= ~0x10;
        }
    }
    
    void setMoveable() { if (_item) _item->data[3] |= 0x10; }

    void setNybbleLo(int d, int v)
    {
        if (_item)
        {
            if (v < 0) v = 0;
            else if (v > 15) v = 15;
            _item->data[d] = (_item->data[d] & 0xf0) | v;
        }
    }

    void setNybbleHi(int d, int v)
    {
        if (_item)
        {
            if (v < 0) v = 0;
            else if (v > 15) v = 15;
            _item->data[d] = (_item->data[d] & 0x0f) | (v << 4);
        }
    }
    
    void setWeight(int v) { setNybbleLo(2, v); }
    void setSize(int v) { setNybbleHi(2, v); }
    void setHardness(int v) { setNybbleHi(1, v); }
    void setStrength(int v) { setNybbleLo(1, v); }

    bool isLit() const
    {
        bool r = false;
        if (_item)
        {
            if (isRoom()) r = (_item->data[5] & 0x04) != 0;
            else r = (_item->data[0] & 0x80) != 0;
        }
        return r;
    }

    void setLit()
    {
        if (_item)
        {
            if (isRoom()) _item->data[5] |= 0x04; // not dark
            else _item->data[0] |= 0x80; // alight
        }
    }

    void _fixcont(bool out = true)
    {
        // fix containers a la fixcont.elt
        // NB: does not handle infinite containers.
        IItem ct = container();
        if (ct)
        {
            // current volume of holding container
            int v = ct.volume();
            assert(v > 0);

            // our size
            int sz = size();

            // if we adjusting out, then add volume to container
            // otherwise subtract it.
            if (out)
                v += sz;
            else
                v -= sz;

            ct.setVolume(v);
        }
    }

    void _fixsys()
    {
        // fix connected items a la fixsys.elt
        IItems clist;
        getConnectedSystem(clist);

        // disconnect
        for (IItems::iterator it = clist.begin(); it != clist.end(); ++it)
            it->setFixedTo(IItem());
    }

    bool setOut()
    {
        bool r = _item && !isOut() && !pseudo() && !isRoom();
        if (r)
        {
            _fixsys();
            _fixcont();
            _item->data[6] |= 0x80;
        }
        return r;
    }

    void resetOut()
    {
        if (_item && isOut() && !pseudo() && !isRoom())
        {
            _item->data[6] &= 0x7f;

            // we're going back in now...
            _fixcont(false);
        }
    }

    bool setLocation(int room)
    {
        // to a simple room#
        // NB: room = 0 => player
        bool r = room >= 0 && _item && !pseudo();
        if (r)
        {
            // bring item in if been set "out".
            resetOut();

            // adjust container, if any
            _fixcont();

            // drop from connected items, if any
            _fixsys();
            
            _item->data[6] = 0; // loc head simple room#
            _setWord(&_item->data[8], room);
        }
        return r;
    }

    void setCarried()
    {
        // will handle containers etc.
        setLocation(0);
    }

    bool setRelatedTo(const IItem& to)
    {
        bool r = _item && !pseudo() && !isRoom();
        if (r)
        {
            // bring item in if been set "out".
            resetOut();

            // adjust container, if any
            _fixcont();

            // change to simple "related to"
            _item->data[6] = 1;
            
            _setWord(_item->data + 8, to.id());
        }
        return r;
    }
    
    bool setContainedBy(const IItem& c)
    {
        bool r = _item && !pseudo() && !isRoom() && c.isContainer();
        if (r)
        {
            // bring item in if been set "out".
            resetOut();

            // adjust any container
            _fixcont();

            // change to simple "contained"
            _item->data[6] = 0x20;
            _setWord(_item->data + 8, c.id());

            // adjust new container
            _fixcont(false);
        }
        return r;
    }

    void setBroken(bool v = true)
    {
        if (_item)
        {
            if (v) _item->data[0] |= 0x04;
            else _item->data[0] &= ~0x04;
        }
    }
    
    uint loc_head() const
    {
        // get location header
        return _item ? _item->data[6] : 0;
    }

    uint loc_pseudo() const
    {
        // secondary locator
        return _item ? _item->data[7] : 0;
    }

    uint loc_data() const
    {
        // get location data (meaning defined by header)
        return _word(_item->data + 8);
    }
        
    static void getInventory(IItems& items)
    {
        // get list of things currently carried (including worn)
        // ignores things that are inseparable (eg jeans pocket)
        // ignore intrinsic (eg hands, feet etc).
        // always include things worn, even if inseparable (eg wristband)
        // allow intrinsic edibles (eg chewing gum).
        // allow things carried that have gui_show_inv override
            
        for (size_t i = 1; i <= itemCount; ++i)
        {
            IItem ii(find_item(i));

            bool fail = false;
            bool pass = false;

            // never show anything "out"
            fail = ii.isOut(); 

            if (!fail)
            {
                // always include things worn,
                // even if inseparable (eg wristband)
                pass = ii.isWorn();
            }

            if (!fail && !pass)
            {
                // allow intrinsic edibles (eg chewing gum).
                pass = ii.isIntrinsic() && ii.isEdible();
            }

            if (!fail && !pass)
            {
                // must be on player spine
                fail = !ii.carried(true);
            }

            if (!fail && !pass)
            {
                // if carried and GUI override, then show
                pass = ii.guiShowInv() && ii.isExplored();
                //if (pass) LOG4("Gui show inv: ",ii.toString());
            }

            if (!fail && !pass)
            {
                // added for jinxter?
                // must be on player spine
                fail = !ii.simplyCarried();
                //if (fail) LOG4("Item not carried (2): ",ii.toString());
            }

            // tests below assume item on player spine (ie carried)

            if (!fail && !pass)
            {
                // block carried items that are inseparable or intrinsic
                fail = ii.isInseparable() || ii.isIntrinsic();
            }

            if (!fail)
                items.push_back(ii);
        }
    }

    static void getCarryableItems(IItems& items)
    {
        // all things gettable in the game
            
        for (size_t i = 1; i <= itemCount; ++i)
        {
            IItem ii(find_item(i));
            if (ii.couldGet()) items.push_back(ii);
        }
    }

    void getItemsInRoom(IItems& items,
                        bool onlyExplored = false,
                        bool onlyCarrable = false,
                        bool noLiveNPC = false)
    {
        uint rn = roomNumber();
        if (!rn) return; // bail if not a room
        
        for (size_t i = 1; i <= itemCount; ++i)
        {
            IItem ii(find_item(i));

            // is item simply in the room?
            bool ok = !ii.loc_head() && ii.loc_data() == rn;

            if (ok && onlyExplored)
            {
                // disregard non-explored items
                if (!ii.isExplored()) ok = false;
            }

            if (ok && onlyCarrable)
            {
                // disregard things that aren't gettable
                if (!ii.couldGet()) ok = false;
            }

            if (ok && noLiveNPC)
            {
                // disregard any live NPC
                if (ii.isNPC() && !ii.isDead()) ok = false;
            }
            
            if (ok)
                items.push_back(ii);
        }
    }

    static string getInventoryString() 
    {
        IItems items;
        getInventory(items);
            
        string inv;
        int wtotal = 0;
        for (size_t i = 0; i < items.size(); ++i)
        {
            if (!inv.empty()) inv += ',';
            inv += items[i].adjWord();
            if (!items[i].isWorn())
            {
                int w = items[i].weight();
                if (w) inv += '(' + std::to_string(w) + ')';
                wtotal += w;
            }
        }
        inv += " total=" + std::to_string(wtotal);
        return inv;
    }

    int getConnectedSystem(IItems& items) const
    {
        // find the collection of things fixed to this or this is fixed to
        
        if (!_item) return 0;

        // first follow the link chain
        IItem to = *this;
        do
        {
            if (contains(items, to)) break; // cycle
            items.push_back(to);
            to = fixedTo();
        } while (to);

        // find everything connected to chain
        for (size_t i = 0; i < items.size(); ++i)
        {
            IItem ii = items[i];
            for (size_t j = 1; j <= itemCount; ++j)
            {
                IItem ij(find_item(j));
                if (ij.fixedTo() == ii && !contains(items, ij))
                    items.push_back(ij);
            }
        }
        return items.size();
    }

    uint roomNumber() const { return isRoom() ? loc_data() : 0; }

    int getExitRooms(int* elist) const
    {
        // unique exits as a list of room numbers
        int n = 0;
        int* ep = elist;
        if (_item)
        {
            for (size_t i = 0; i < bytesPerRoom; ++i)
            {
                size_t ei = _item->exitData[i];
                if (ei > 0 && ei <= maxRoom)
                {
                    int j;
                    for (j = 0; j < n; ++j)
                        if (elist[j] == (int)ei) break;
                    
                    // not already present
                    if (j == n)
                    {
                        *ep++ = ei;
                        ++n;
                    }
                }
            }
        }
        return n;
    }

    int getExits(IItem* elist) const
    {
        // exits as a list of objects
        
        int exits[ITEM_MAX_EXITS];
        int n = getExitRooms(exits);
        for (int i = 0; i < n; ++i)
        {
            IItem ri = getRoom(exits[i]);
            assert(ri);
            *elist++ = ri;
        }
        return n;
    }

    IItem getExit(uint direction) const
    {
        IItem ex;
        if (_item)
        {
            if (direction < bytesPerRoom)
            {
                size_t ei = _item->exitData[direction];
                if (ei > 0 && ei <= maxRoom) ex = getRoom(ei);
            }
        }
        return ex;
    }

    void setExit(int direction, int room) 
    {
        if (_item)
        {
            assert(direction < (int)bytesPerRoom);
            _item->exitData[direction] = room;
        }
    }

    string getExitsString() const
    {
        string s;
        IItem exits[ITEM_MAX_EXITS];
        int n = getExits(exits);
        for (int i = 0; i < n; ++i)
        {
            s += exits[i].toString();
            if (i < n-1) s += ',';
        }        
        return s;
    }

    static IItem getRoom(size_t n)
    {
        // object of room `n`
        IItem room;

        if (n > 0 && n <= maxRoom)
        {
            for (size_t i = 1; i <= itemCount; ++i)
            {
                IItem ii(find_item(i));
                assert(ii);

                if (ii.isRoom() && ii.loc_data() == n)
                {
                    room = ii;
                    break;
                }
            }            
        }
        return room;
    }

    IItem locationTop(bool ignoreout = false) const;

    bool reachable(bool xray = false) const
    {
        // reachable by player?
        // used also as noun resolution
        return spine(currentRoom(), xray); 
    }

    bool carried(bool xray = false) const
    {
        return spine(IItem(), xray);
    }

    bool simplyAt(uint n) const
    {
        return !pseudo() && !loc_head() && loc_data() == n;
    }

    bool isRelatedTo(uint n) const
    {
        // not pseudo, not out nor room, but related to n
        return !pseudo() && ((loc_head() & 0xc1) == 1) && loc_data() == n;
    }

    bool simplyCarried() const
    {
        // object is directly on inventory, not inside something carried etc.
        return simplyAt(0);
    }

    static IItem currentRoom() { return IItem::getRoom(get_current_room()); }

    bool parse(const string&);

    static void initDiction();

    static void findItems(const string& word, IItems& items)
    {
        findItems(word.c_str(), items);
    }

    static void findItems(const char* word, IItems& items)
    {
        // use dictionary to lookup base word, then convert to items
        Diction::Words words;
        _nouns.findMatches(word, words);
        for (size_t i = 0; i < words.size(); ++i)
            items.emplace_back(find_item(words[i]._id));
    }

    static int findPhraseItems(const char* phrase, IItems& items)
    {
        // use dictionary to lookup phrase items, then convert to items

        // only return matches if result is unique
        // return number of words of phrase matched

        int nw = 0;
        Diction::Words words;
        _nouns.findPhraseMatches(phrase, words);

        if (words.size() == 1)
        {
            nw = words[0].phraseLength();
            items.emplace_back(find_item(words[0]._id));
        }
        else
        {
            // whenever there are multiple phrase matches, act as
            // if there is no match as this cannot be resolved to a single
            // item
        }
        return nw;
    }

    static int findAdjective(const string& word)
    {
        return findAdjective(word.c_str());
    }

    static int findAdjective(const char* word)
    {
        // NB: some adjectives in the table have spaces,
        // these won't be matched (correct).
        
        // correct because only the collection of words is the "adjective"
        // otherwise you'd get things like "the" being an adjective!

        // however, so that we can lookup these things (eg from puzzles)
        // allow `word` to contain underscores.

        string w(word);
        w = replaceAll(w, '_', ' ');

        Diction::Words words;
        _adjectives.findMatches(w, words);
        return !words.empty() ? words[0]._id : 0;
    }

    static void resolveAdjective(IItems& items, int adj)
    {
        // remove items without `adj`
        if (adj)
        {
            for (IItems::iterator it = items.begin(); it != items.end();)
            {
                if (!it->hasAdj(adj)) it = items.erase(it);
                else ++it;
            }
        }
    }
    
    static void resolveLocal(IItems& items, bool xray = false)
    {
        // remove any non-local items
        // xray will allow items inside closed containers etc.
        for (IItems::iterator it = items.begin(); it != items.end(); )
        {
            if (it->reachable(xray)) ++it; else it = items.erase(it);
        }
    }

    static void resolveWithoutRooms(IItems& items)
    {
        // remove any rooms
        for (IItems::iterator it = items.begin(); it != items.end(); )
        {
            if (!it->isRoom()) ++it; else it = items.erase(it); 
        }
    }
    
    static void parse(IItems& items, const string& phrase);
    static void phraseResolve(IItems& items, const string& phrase);

    void expose() const
    {
        if (!_item) return;
        
        std::cout << "item# " << std::hex <<_item->id << std::dec << " " << toString() << ":\n";

        int head = loc_head();
        int parent = loc_data();

        std::cout << "pseudo: " << (pseudo() ? "yes" : "no") << std::endl;

        std::cout << "location: " << std::hex << head << " " << parent
                  << std::dec << " ";

        if (head)
            std::cout << "parent = " << IItem(find_item(parent)) << std::endl;
        else
            std::cout << "in room " << getRoom(parent) << std::endl;
        
        std::cout << "size: " << size() << std::endl;
        std::cout << "weight: " << weight() << std::endl;
        std::cout << "hardness: " << hardness() << std::endl;
        std::cout << "strength: " << strength() << std::endl;
        std::cout << "moveable: " << (isMoveable() ? "true": "false") << std::endl;
        std::cout << "inseparable: " << (isInseparable() ? "true": "false") << std::endl;

        if (isNPC())
        {
            std::cout << "Is NPC;";
            if (isDead()) std::cout << " DEAD";
            std::cout << std::endl;
        }
    }

    bool spine(IItem y, bool xray = false) const
    { return _item && _spine(*this, y, xray); }

private:

    void _init()
    {
        _item = 0;
    }


    static bool _spine(IItem x, IItem y, bool xray = false);
};


