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
 *  Copyright (©) Voidware 2016-2017.
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
 *  contact@voidware.com
 *
 */

#pragma once

#include <string>
#include <vector>
#include "strutils.h"
#include "logged.h"

struct Markup
{
    typedef std::string string;
    typedef IItem::IItems IItems;

    static bool wordChar(char c)
    {
        return u_isalnum(c) || u_ishyphen(c);
    }

    void skipMarkup()
    {
        // skip "[foo](bar)"
        if (*_pos == '[')
        {
            while (*++_pos)
            {
                if (*_pos == ']')
                {
                    if (*++_pos == '(')
                    {
                        while (*++_pos)
                        {
                            if (*_pos == ')')
                            {
                                ++_pos;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    void advance()
    {
        // advance position, but skip existing markup as if it is not
        // present in the input.
        ++_pos;
        skipMarkup();
    }

    void skipSpace()
    {
        while (u_isspace(*_pos)) advance();
    }

    void skipWord()
    {
        while (wordChar(*_pos)) ++_pos;
    }

    void skipToWord()
    {
        // skip markup, spaces and punctuation until we start a word
        for (;;)
        {
            skipSpace();
            if (!*_pos || wordChar(*_pos)) break;
            advance();
        }
    }
    
    struct Mark
    {
        int      _start;
        int      _end;
        IItem    _item;
        bool     _valid;
        int      _dir;
        
        Mark(int start, int end, IItem item)
            : _start(start), _end(end), _item(item) 
        {
            _dir = -1;
            _valid = true;
        }

        Mark(int start, int end, IItem::Dir d)
            : _start(start), _end(end), _dir(d)
        {
            _valid = true;
        }
    };

    struct DirRec
    {
        const char*     _word;
        IItem::Dir      _dir;
    };

    int markupDirection(const string& word, IItem croom)
    {
        int d = -1; // none
        static const DirRec DirRecTab[] =
        {
            { "north", IItem::dir_n },
            { "northern", IItem::dir_n },
            { "northward", IItem::dir_n },
            { "northwards", IItem::dir_n },
            
            { "northeast", IItem::dir_ne },
            { "NE", IItem::dir_ne },
            { "northeastern", IItem::dir_ne },
            { "northeastward", IItem::dir_ne },
            { "northeastwards", IItem::dir_ne },
            
            { "east", IItem::dir_e },
            { "eastern", IItem::dir_e },
            { "eastward", IItem::dir_e },
            { "eastwards", IItem::dir_e },

            { "southeast", IItem::dir_se },
            { "SE", IItem::dir_se },
            { "southeastern", IItem::dir_se },
            { "southeastward", IItem::dir_se },
            { "southeastwards", IItem::dir_se },

            { "south", IItem::dir_s },
            { "southern", IItem::dir_s },
            { "southward", IItem::dir_s },
            { "southwards", IItem::dir_s },

            { "southwest", IItem::dir_sw },
            { "SW", IItem::dir_sw },
            { "southwestern", IItem::dir_sw },
            { "southwestward", IItem::dir_sw },
            { "southwestwards", IItem::dir_sw },
            
            { "west", IItem::dir_w },
            { "western", IItem::dir_w },
            { "westerly", IItem::dir_w },
            { "westward", IItem::dir_w },
            { "westwards", IItem::dir_w },

            { "northwest", IItem::dir_nw },
            { "NW", IItem::dir_nw },
            { "northwestern", IItem::dir_nw },
            { "northwestward", IItem::dir_nw },
            { "northwestwards", IItem::dir_nw },

            { "up", IItem::dir_u },
            { "upward", IItem::dir_u },
            { "upwards", IItem::dir_u },

            { "downward", IItem::dir_d },
            { "downwards", IItem::dir_d },
            { "down", IItem::dir_d },
        };

        for (size_t i = 0; i < DIM(DirRecTab); ++i)
        {
            const DirRec* di = DirRecTab + i;

            // if entry uppercase, then must match
            bool matchcase = u_isupper(*di->_word);
            bool ok = !matchcase ? equalsIgnoreCase(word, di->_word) : word == di->_word;
            if (ok)
            {
                if (croom.getExit(di->_dir)) d = di->_dir;
                break;
            }
        }
        return d;
    }

    bool markupItems(string& s)
    {
        bool changed = false;
        std::vector<Mark> marks;

        const char* s0;
        s0 = _pos = s.c_str();

        IItem croom = IItem::currentRoom();

        string lastword;
        const char* lastp = 0;
        string seclastword;
        const char* seclastp = 0;
        
        for (;;)
        {
            skipToWord();
            if (!*_pos) break;
            
            const char* p = _pos; // start
            skipWord();

            string word(p, _pos - p);
            IItems items;

            // look for direction words like "east", "northwards" etc.
            int d = markupDirection(word, croom);
            int nw = 0;

            if (d >= 0)
            {
                marks.emplace_back(Mark(p - s0, _pos - s0, IItem::Dir(d)));
            }
            else
            {
                // resolve partials:
                // p = "lump of coal is here ..."
                // try to locate prefix "lump of coal"
                nw = IItem::findPhraseItems(p,items);

                if (!nw)
                {
                    // find all items that match this root word (if any)
                    IItem::findItems(word, items);
                }

                // remove any rooms because we only markup items
                IItem::resolveWithoutRooms(items);

                // all items must also be local
                IItem::resolveLocal(items);
            }

            size_t n = items.size();
            if (n > 0)
            {
                int useadj = 0;
                
                if (n > 1 && lastp)
                {
                    assert(nw == 0); // not phrase
                    
                    // we have multiple items based on the root word
                    // if last word is an adjective, resolve with that
                    IItem::resolveAdjective(items,
                                            IItem::findAdjective(lastword));
                    
                    if (items.size() != n)
                    {
                        // adjective reduced
                        useadj = 1;
                    }
                    n = items.size();
                    // ... but not enough, try second adjective
                    if (n > 1 && seclastp)
                    {
                        IItem::resolveAdjective(items,
                                                IItem::findAdjective(seclastword));
                        if (items.size() != n)
                        {
                            // adjective reduced again
                            useadj = 2;
                        }
                    }
                }

                // must be exactly one item and not ambiguous
                if (items.size() == 1)
                {
                    IItem item = items[0];
                    if (useadj == 2)
                    {
                        marks.emplace_back(Mark(seclastp - s0, _pos - s0, item));
                    }
                    else if (useadj == 1)
                    {
                        marks.emplace_back(Mark(lastp - s0, _pos - s0, item));
                    }
                    else
                    {
                        if (nw)
                        {
                            // advance over rest of phrase match
                            while (--nw)
                            {
                                skipToWord();
                                assert(*_pos);
                                skipWord();
                            }
                        }
                        
                        // located a game item
                        marks.emplace_back(Mark(p - s0, _pos - s0, item));
                    }
                }
            }
            if (lastp)
            {
                seclastp = lastp;
                seclastword = lastword;
            }
            lastword = word;
            lastp = p;
        }

        size_t n = marks.size();
        if (n)
        {
            changed = true;
            
            // make subst
            Mark* mlast = &marks[0];
            for (size_t i = 1; i < n; ++i)
            {
                Mark& mi = marks[i];
                if (mi._start <= mlast->_start)
                {
                    // supercedes last
                    mlast->_valid = false;
                    
                }
                mlast = &mi;
            }

            int off = 0;
            string s2;
            for (size_t i = 0; i < n; ++i)
            {
                const Mark& mi = marks[i];                
                if (!mi._valid) continue;

                if (mi._start > off)
                    s2 += s.substr(off, mi._start - off);

                string sw = s.substr(mi._start, mi._end - mi._start);
                string rep;
                if (mi._dir < 0)
                {
                    rep = "[" + sw + "](do " + mi._item.adjWordIf() + ")";
                }
                else
                {
                    // changed to allow paddling in Jinxter via click, needs testing
                    //rep = "[" + sw + "](go " + IItem::dirName(mi._dir) + ")";
                    rep = "[" + sw + "](" + IItem::dirName(mi._dir) + ")";
                }
                LOG4("markup found ", sw << " -> " << rep);
                
                s2 += rep;
                off = mi._end;
            }

            // add any remainder
            s2 += s.substr(off, string::npos);
            s = s2;
        }
        return changed;
    }

    bool markupTextBuffer(char* buf)
    {
        string t = buf;
        bool changed = markupItems(t);
        if (changed)
            strcpy(buf, t.c_str());

        return changed;
    }

    const char*         _pos;
};
