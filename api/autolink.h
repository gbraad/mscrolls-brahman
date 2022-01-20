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

#include <assert.h>
#include <string>
#include <vector>
#include "logged.h"
#include "obj.h"
#include "mdfilter.h"

struct Autolink: public MDFilter
{
    typedef std::string string;
    typedef ObjectList::Item Item;

    uint                _currentExits = 0;
    const Objects*      _scope;
    string              _useCmd;
    string              _goCmd;
    
    struct Mark
    {
        int             _start;
        int             _end;
        Item            _item = 0;
        bool            _valid = true;
        int             _dir = -1;
        
        Mark(int start, int end, Item item)
            : _start(start), _end(end), _item(item)  {}
        
        Mark(int start, int end, int d) : _start(start), _end(end), _dir(d) {}

    };

    struct DirRec
    {
        const char*     _word;
        int             _dir;
    };

    int markupDirection(const string& word)
    {
        int d = -1; // none
        static const DirRec DirRecTab[] =
        {
            { "north", Control::dir_n },
            { "northern", Control::dir_n },
            { "northward", Control::dir_n },
            { "northwards", Control::dir_n },
            
            { "northeast", Control::dir_ne },
            { "NE", Control::dir_ne },
            { "northeastern", Control::dir_ne },
            { "northeastward", Control::dir_ne },
            { "northeastwards", Control::dir_ne },
            
            { "east", Control::dir_e },
            { "eastern", Control::dir_e },
            { "eastward", Control::dir_e },
            { "eastwards", Control::dir_e },

            { "southeast", Control::dir_se },
            { "SE", Control::dir_se },
            { "southeastern", Control::dir_se },
            { "southeastward", Control::dir_se },
            { "southeastwards", Control::dir_se },

            { "south", Control::dir_s },
            { "southern", Control::dir_s },
            { "southward", Control::dir_s },
            { "southwards", Control::dir_s },

            { "southwest", Control::dir_sw },
            { "SW", Control::dir_sw },
            { "southwestern", Control::dir_sw },
            { "southwestward", Control::dir_sw },
            { "southwestwards", Control::dir_sw },
            
            { "west", Control::dir_w },
            { "western", Control::dir_w },
            { "westerly", Control::dir_w },
            { "westward", Control::dir_w },
            { "westwards", Control::dir_w },

            { "northwest", Control::dir_nw },
            { "NW", Control::dir_nw },
            { "northwestern", Control::dir_nw },
            { "northwestward", Control::dir_nw },
            { "northwestwards", Control::dir_nw },

            { "up", Control::dir_u },
            { "upward", Control::dir_u },
            { "upwards", Control::dir_u },

            { "downward", Control::dir_d },
            { "downwards", Control::dir_d },
            { "down", Control::dir_d },
        };

        for (int i = 0; i < ASIZE(DirRecTab); ++i)
        {
            const DirRec* di = DirRecTab + i;

            // if entry uppercase, then must match
            bool matchcase = u_isupper(*di->_word);
            bool ok = !matchcase ? equalsIgnoreCase(word, di->_word) : word == di->_word;
            if (ok)
            {
                uint mask = 1U << di->_dir;
                if (_currentExits & mask)
                {
                    d = di->_dir;
                }
                
                break;
            }
        }
        return d;
    }

    
    string applyItemMarkup(const string& s)
    {
        assert(_scope);

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
            ObjectList items(*_scope);

            int d = markupDirection(word);

            if (d >= 0)
            {
                marks.emplace_back(Mark(p - s0, _pos - s0, d));
            }
            else 
            {
                for (Objects::const_iterator it = _scope->begin();
                     it != _scope->end(); ++it)
                {
                    if (it->matchWord(word)) items.push_back(&*it);
                }
            }
            
            size_t n = items.size();
            if (n > 0)
            {
                bool useadj = false;
                
                if (n > 1 && lastp)
                {
                    // resolve by adjective, remove items not matching
                    for (ObjectList::iterator it = items.begin();
                         it != items.end();)
                    {
                        if (!(*it)->matchAdj(word, lastword))
                            it = items.erase(it);
                        else ++it;
                    }

                    if (items.size() != n)
                    {
                        // adjective reduced
                        useadj = true;
                    }
                }

                // must be exactly one item and not ambiguous
                if (items.size() == 1)
                {
                    Item item = items[0];
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

        string s2;
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
            for (size_t i = 0; i < n; ++i)
            {
                const Mark& mi = marks[i];                
                if (!mi._valid) continue;

                if (mi._start > off)
                    s2 += s.substr(off, mi._start - off);

                bool ok = true;
                
                string sw = s.substr(mi._start, mi._end - mi._start);
                string rep;
                if (mi._dir < 0)
                {
                    if (!_useCmd.empty())
                    {
                        ObjectList ctx(*_scope);
                        ctx.push_back(mi._item);
                        rep = "[" + sw + "](" + applySubs(_useCmd, ctx) + ")";
                    }
                    else
                    {
                        ok = false;
                    }
                }
                else
                {
                    if (!_goCmd.empty())
                    {
                        ObjectList ctx(*_scope);
                        // force direction for ID
                        ctx.add(Control::dirName(mi._dir), true);
                        rep = "[" + sw + "](" + applySubs(_goCmd, ctx) + ")";
                    }
                    else
                    {
                        // fallback
                        rep = "[" + sw + "](go " + Control::dirName(mi._dir) + ")";
                    }
                }

                if (ok)
                {
                    LOG4("markup found ", sw << " -> " << rep);
                
                    s2 += rep;
                    off = mi._end;
                }
            }

            // add any remainder
            s2 += s.substr(off, string::npos);
        }

        return changed ? s2 : s;
    }

    static string applySubs(const string& cmd, const ObjectList& ctx)
    {
        string scmd;
        
        // cmd is any string, but we can substitute embedded codes
        // from the given context.
        // each embeded code is of the form {<number>:<key>}
        // where;
        //
        // number = arg number within ctx, which will be an ID
        // key = "id", replace {} by ID from ctx
        // key = "name", replace {} by name from object table via ID from ctx

        const char* p = cmd.c_str();
        const char* st = p;
        while (*p)
        {
            // scan for next {number:
            if (*p == '{')
            {
                // start of substitute
                const char* pstart = p;
                
                // expect an arg number immediately
                ++p;

                // expect one digit
                uint arg = *p++ - '0'; // only 1 digit
                if (arg == 0 || arg > 9) continue; // not digit

                --arg; // from 0

                if (arg >= ctx.size())
                {
                    LOG1("command subst arg invalid ", arg << " in " << cmd);
                    continue;
                }

                ObjectList::Item item = ctx[arg];
                
                if (*p != ':') continue; // ignore if not well formed
                ++p;
                // q = start of key
                const char* q = p;
                while (*p && *p != '}') ++p;
                if (!*p || p == q) continue; // no key

                // extract key
                string key(q, p - q);

                // add text up to start of sub
                scmd.append(st, pstart - st);
                
                if (key == IFI_ID)
                {
                    // just want the id and no name
                    scmd += item->_id;
                }
                else if (key == IFI_NAME)
                {
                    scmd += item->_name;
                }
                else
                {
                    LOG1("Unknown command key '", key << "'");
                }

                assert(*p == '}');

                // continue text from after sub
                st = p + 1;
            }

            ++p;
        }

        // add any remainder
        scmd.append(st, p - st);

        if (cmd != scmd)
        {
            LOG4("IFI subbed command ", scmd);
        }

        return scmd;
    }
};
