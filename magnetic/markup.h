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
        return u_isalnum(c);
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

        Mark(int start, int end, IItem item)
            : _start(start), _end(end), _item(item) 
        {
            _valid = true;
        }
    };

    bool markupItems(string& s)
    {
        bool changed = false;
        std::vector<Mark> marks;

        const char* s0;

        s0 = _pos = s.c_str();

        string lastword;
        const char* lastp = 0;
        
        for (;;)
        {
            skipToWord();
            if (!*_pos) break;
            
            const char* p = _pos; // start
            skipWord();

            string word(p, _pos - p);

            // find all items that match this root word (if any)
            IItems items;
            IItem::findItems(word, items);

            // remove any rooms because we only markup items
            IItem::resolveWithoutRooms(items);

            size_t n = items.size();
            if (n > 0)
            {
                bool useadj = false;
                
                if (n > 1 && lastp)
                {
                    // we have multiple items based on the root word
                    // if last word is an adjective, resolve with that
                    IItem::resolveAdjective(items,
                                            IItem::findAdjective(lastword));
                    
                    if (items.size() != n)
                    {
                        // adjective reduced
                        useadj = true;
                    }
                }

                // all items must also be local
                IItem::resolveLocal(items);

                // must be exactly one item and not ambiguous
                if (items.size() == 1)
                {
                    IItem item = items[0];
                    if (useadj)
                    {
                        marks.emplace_back(Mark(lastp - s0, _pos - s0, item));
                    }
                    else
                    {
                        // located a game item
                        marks.emplace_back(Mark(p - s0, _pos - s0, item));
                    }
                }
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
                string rep = "[" + sw + "](do " + mi._item.adjWord() + ")";
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
