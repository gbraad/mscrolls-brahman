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
 *  Copyright (c) Strand Games 2020.
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

#include "tree.h"

struct Group: public List
{
    // Constructors
    
    // Destructor
    // groups are lists, except they destroy all the contents
    //~Group() { _purge(); }

    void fold(SBuf& sb) const
    {
        size_t len = size(); // logical size
        sb.writeInt32(len);

        iterator it(*this); 
        while (it.valid())
        {
            (*it).fold(*this, sb);
            ++it;
        }
    }

    void unfold(SBuf& sb)
    {
        unsigned int len = sb.readInt32();
        
        // NB: memory must be zero, used for fixup chain whilst unfolding
        _n = createz(len);
        _n->size = len;

        unsigned int i;
        for (i = 0; i < len; ++i)
        {
            Head h;
            h.unfold(*this, sb);

            h.setOwned();

            Head* hp = &h;
            
            // if defined, will be a fix chain
            Node** np = (Node**)_n->at(i);
            while (np)
            {
                // fixups
                Node** nn = (Node**)*np; // next
                *np = hp->ref(false);
                hp = GET_HEAD(*np);
                np = nn;
            }
            _n->at(i) = hp->ref(false);
        }
    }

    unsigned int indexOf(Node* n) const
    {
        unsigned int idx = 0;
        for (iterator it(*this); it.valid(); ++it)
        {
            if (it.node() == n)
                return idx;
            
            ++idx;
        }
        return (unsigned int)-1;
    }

};

