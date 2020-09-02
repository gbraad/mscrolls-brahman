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

#include "types.h"

struct Node
{
    static Node*        create(uint n)
    {
        Node* node = (Node*)new char[sizeof(Node) + n*sizeof(Node*)];
        node->size = 0;
        node->space = n;
        return node;
    }

    static void         destroy(Node* n) { delete (char*)n; }

    bool                full() const { return size == space; }

    Node*               at(uint i) const { return ((Node**)(this + 1))[i]; }
    Node*&              at(uint i) { return ((Node**)(this + 1))[i]; }

    Node*               link() const { return at(space-1); }
    Node*&              link() { return at(space-1); }

    ushort              size;
    ushort              space;
    
    // it's like we have an array here, but without it declared
    // Node*               _n[];        
};

