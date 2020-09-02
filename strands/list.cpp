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


#include "group.h"

List* List::create(Group& g)
{
    List* l = new List;
    l->incRef();
    g.append(*l);
    l->drop();
    return l;
}

void List::fold(const Group& g, SBuf& sb) const
{
    // find the total length of the list including all chains
    // we write out as one sequence.
    uint len = size();
    sb.writeInt32(len);

    if (len)
    {
        iterator it(*this);
    
        // ALTERNATIVE IDEA:
        // rather than preserve even/odd of single node lists.
        // we could restore them to the same original space.
        // the idea is that single nodes are small.
        
        // in order to preserve the even/odd cases for tree node
        // must store whether a pad of 1 is required.
        char pad = 0;
        
        // could be list. do we have a chain. if so => list
        if (!_n->link())
        {
            // no link.
            // we have a single node list or tree node.
            // preserve the even/odd of the space
            if ((_n->size ^ _n->space) & 1)
            {
                // different even/odd
                pad = 1;
            }
        }
        
        sb.add(pad);

        while (it.valid())
        {
            Head& h = *it;
            
            if (!h.owned() || h._refCount <= 1)
            {
                // if ref == 1 we are the only copy. we can be 
                // owned or non-owned, in both cases fold here.
                // if we are non-owned, then we reference some other
                // list, but fold a copy.
                //
                // if not owned, then we are a temp (ref=0) or we
                // are a shared non-owned head (ref anything)
                
                h.fold(g, sb);
            }
            else
            {
                // shared object, we must emit a local ref
                // expect object to be in the global group table.
                unsigned int idx = g.indexOf(it.node());

                // expect to exist
                assert(idx != (unsigned int)-1);

                // emit a local ref
                LRef lr(idx);
                lr.Head::fold(g, sb);
            }

            ++it;
        }
    }
}

void List::unfold(Group& g, SBuf& sb)
{
    // number of logical elements
    size_t len = sb.readInt32();

    // space to allocate
    size_t sp = len;

    if (sp)
    {
        // need padding?
        if (sb.readByte())
            ++sp;
    }

    // create node array for `len' objects.
    _n = create(sp);

    // we will have exactly this many
    _n->size = len;

    for (unsigned int i = 0; i < len; ++i)
    {
        Head h;
        h.unfold(g, sb);

        // whether or not it was a fake copy, restoring makes a real copy.
        h.setOwned();

        if (h.type() == type_lref)
        {
            unsigned gi = ((LRef*)&h)->refIndex();

            Node*& n = g.nodeAt(gi);
            if (IS_HEAD(n))
            {
                // defined already, take a reference
                _n->at(i) = GET_HEAD(n)->ref();
            }
            else
            {
                Node*& ni = _n->at(i);
                
                ni = n; // link in fix chain
                
                // not defined yet, store fixup chain
                n = (Node*)&ni;
            }
        }
        else
        {
            _n->at(i) = h.ref(); // ref=1, consume
        }
    }
}
