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

#include "term.h"
#include "pair.h"
#include "parsectx.h"

struct LRef: public Head
{
    // used temporarily to fold groups

    LRef(uintptr_t idx): Head(type_lref) { _n = (Node*)idx; }

    uintptr_t refIndex() const { return (uintptr_t)_n; }

    void fold(SBuf& sb) const { sb.writeInt32(refIndex()); }  // XX 64 bits?
    void unfold(SBuf& sb) { _n = (Node*)(uintptr_t)sb.readInt32(); }
};

struct List: public Head
{
    static const uint initListSize = 2;
    static const uint growListSize = 16; // NB: power of 2
    static const uint maxListSize = 20;
   
    // common iterator 
    struct iterator_base
    {
        List*   _l;
        Node*   _n;
        uint   _pos; // although size is 16 bit
        HeadTmp _tHead;        

        // Constructors
        iterator_base() {}
        iterator_base(const List& l) : _l((List*)&l) { _init(); }

        // Accessors
        Head&   operator*() { return _head(_pos); }
        Head*   operator->() { return &_head(_pos); }

        // Features
        void    reset() { _init(); }

        // get current node
        Node*   node() { return _n->at(_pos); }

        bool operator==(const iterator_base& i) const
        { return _n == i._n && _pos == i._pos; }
        bool operator!=(const iterator_base& i) const
        { return _n != i._n || _pos != i._pos; }

    protected:

        // get node as head
        Head&   _head(uint i)
        {
            Node*& n = _n->at(i);
            if (IS_HEAD(n)) return *GET_HEAD(n);
            
            _tHead._type = type_list;
            _tHead._flags = 0; // not owned, will be dup'ed if used
            _tHead._refCount = 0;
            _tHead._n = n;
            _tHead._nAddr = &n;
            return _tHead;
        }

        void _init()
        {
            _n = _l->_n;
            _pos = 0;
        }

    };

    /// this is logical value iterator
    struct iterator: public iterator_base
    {
        typedef iterator_base parentT;
        
        iterator(const List& l) : parentT(l) { _init(); }

        // Features
        void operator++() { _bump(); }
        //void operator--() { _unbump(); }

        // list iterators can park on the final element+1 of the final chain
        bool valid() const { return _n && _pos < _n->size; }

        explicit operator bool() const { return valid(); }

        void reset() 
        {
            parentT::reset(); 
            _init();
        }

        void  end()
        {
            // skip to end
            while (valid()) chain();
        }

        typedef Pair<int> Range;

        void _insertN(Node** np, int nh)
        {
            if (!nh) return;
            
            // insert possibly on end. 
            // need current node and previous node. so expect to 
            // be parked on last node at size if on end.

            assert(!_l->_n || _n); // assume not just null iterator.
            
            int size = 0;
            int space = 0;
            bool chained = false;
            int ds = 0;
            
            bool extend = !_n;
            if (!extend)
            {
                size = _n->size;

                // chain takes up 1 slot
                chained = (bool) _getChain(_n);
                if (chained) ds = 1;
                
                // usable space
                space = _n->space - ds;
                
                int needed = size + nh;
                
                if (needed > space)
                {
                    // try to grow
                    needed += ds; 
                    extend = needed > (int)maxListSize;
                        
                    if (!extend)
                    {
                        // can grow, round up to grow size
                        needed = u_min((needed + (growListSize-1)) & ~(growListSize-1), maxListSize);
                        _growFor(needed);

                        // NB: `space' not used hereafter
                    }
                }
            }
            
            if (extend)
            {
                // otherwise extend for `n'
                int basesize = initListSize;
                Node** npaddr = &_l->_n;
                
                if (_n)
                {
                    basesize = growListSize;
                    npaddr = &_n->link();

                    // not already chained, then logical space 
                    // will become one smaller afterwards
                    if (!chained) --space;
                }

                Range rs(_pos, size);
                Range rd = rs + nh;
                Range rd1 = rd.intersect(Range(0, space));
                Range rd2 = rd.maximum(space) - space; 
                int d2 = space - nh;
                int ov = rd2._hi;
                
                if (space < (int)_pos)
                {
                    // must have size == total space and space adjusted
                    // and pos is at end, so we are appending
                    assert(_n && _n->full());
                    assert(!rd1 && !rd2);
                    
                    // arrange to copy the final `space' element
                    // and to fill new nodes from 1 (after new elt)
                    rd2 = Range(0,1);
                    d2 = space;
                    ov = nh + 1;
                }
                
                assert(ov > 0);
               
                // if already chained, ensure we have space on end for
                // link to existing chain
                Node* n = List::create(u_max(basesize, ov + ds));
                n->size = ov;

                // overflow existing nodes into extension
                if (rd2)
                {
                    memcpy(&n->at(rd2._lo),
                           &_n->at(rd2._lo + d2), // source location
                           rd2.size()*sizeof(Node*));
                    if (_n) _n->size -= rd2.size();
                }

                if (rd1)
                {
                    memmove(&_n->at(rd1._lo),
                            &_n->at(rd1._lo - nh), // source location
                            rd1.size()*sizeof(Node*));
                }

                Range ns(_pos, _pos + nh);
                rd1 = ns.intersect(Range(0,space));
                rd2 = ns.maximum(space) - space;

                // insert new nodes in current node
                int r = rd1.size();
                if (r)
                {
                    _n->size += r;
                    while (r)
                    {
                        --r;
                        _n->at(_pos++) =  *np++;
                    }                
                }

                // insert new nodes into new node
                r = rd2.size();
                if (r)
                {
                    // move to new node
                    _prev = _n;
                    _n = n;
                    _pos = rd2._lo;
                    do
                    {
                        _n->at(_pos++) =  *np++;                    
                    } while (--r);
                }

                // if we're extending an already chained node
                // the chain of the new node must point to the old chain node
                if (chained)
                    n->link() = *npaddr;
                
                *npaddr = n;
            }
            else
            {
                uint r = _n->size - _pos;
            
                // shift any remainder up
                if (r)
                    memmove(&_n->at(_pos + nh), &_n->at(_pos), r*sizeof(Node*));

                // insert at pos
                _n->size += nh;
                while (nh)
                {
                    --nh;
                    _n->at(_pos++) =  *np++;
                }
            }
        }

        // used by `consume'
        void _insertDonate(List& lsrc)
        {
            iterator li(lsrc);
            while (li.valid())
            {
                Node* n = li._n;
                _insertN(&n->at(0), n->size);
                li.chain();
            }

            // empty the source without deleting the content
            lsrc._purgeNodes();
        }

        void consume(List& l)
        {
            iterator it(*this);
            it.end();
            it._insertDonate(l);
        }

        const Head& insert(const Head& h)
        {
            // insert at `pos' and move forward
            // return the inserted HEAD.
            Node* np = h.ref();
            _insertN(&np, 1);
            return *GET_HEAD(np);
        }

        void _growFor(uint space)
        {
            assert(_n);
            assert(space > _n->space);
            
            // grow node size and copy elements
            Node* n = List::create(space);
            memcpy(&n->at(0), &_n->at(0), _n->size*sizeof(Node*));
            n->size = _n->size;

            if (_prev)
                _prev->link() = n; // chain to new
            else
                _l->_n = n;
            
            Node::destroy(_n);
            _n = n;
        }

        uint size() const
        {
            // find the remaining number of elements from pos
            uint m = 0;
            if (_n)
            {
                m = _n->size - _pos;
                Node* n = _n;
                for (;;)
                {
                    n = _getChain(n);
                    if (!n) break;
                    m += n->size;
                }
            }
            return m;
        }

        bool replace(const Head& h)
        {
            // replace current element with `h'
            bool res = valid();
            if (res)
            {
                Node* n = h.ref();
                Head& h1 = **this;
                _purgeEnt(h1);
                _n->at(_pos) = n;
            }
            return res;
        }

        bool erase()
        {
            // remove element at current position, park at next elt
            bool res = valid();
            if (res)
            {
                _purgeEnt(_head(_pos));

                --_n->size;
                
                if (!_n->size)
                {
                    // removed all elements
                    eraseNode();
                }
                else 
                {
                    // move remaining elements down
                    memmove(&_n->at(_pos), &_n->at(_pos+1),
                            (_n->size - _pos)*sizeof(Node));
                    
                    if (_pos == _n->size)
                    {    
                        // have removed the last item and we are parked on empty
                        // move to next node
                        chain();
                    }
                }
            }
            return res;
        }

        void skip(uint m)
        {
            while (m && valid())
            {
                uint r = _n->size - _pos;
                if (m < r)
                {
                    _pos += m;
                    break;
                }
                else
                {
                    m -= r;
                    chain();
                }
            }
        }

        void eraseNode()
        {
            // ASSUME valid
            Node* next = _getChain(_n);
            Node::destroy(_n);
            if (_prev)
                _prev->link() = next;
            else
                _l->_n = next;
            _n = next;
            _pos = 0;
        }

        void chain()
        {
            // chain to next node
            // if no next node, park on size on this node (invalid)
            Node* n = _getChain(_n);
            if (n)
            {
                _prev = _n;
                _n = n;
                _pos = 0;
            }
            else
            {
                assert(_n);
                _pos = _n->size; // park at invalid end
            }
        }

    protected:

        static Node* _getChain(const Node* me)
        {
            // when chaining a tree, we don't have links.
            // whenever size < space, the last slot is empty
            // otherwise we check here.
            return me->full() ? 0 : me->link(); // can be null
        }

        void _bump()
        {
            assert(valid());            // ASSUME valid
            if (++_pos >= _n->size)
                chain();
        }

#if 0
        // used by operator--(), but can only work within a 
        // single node, otherwise have to recover `prev'.
        void _unbump()
        {
            // go back one, but only valid after `bump'
            if (_n && _pos)
            {
                --_pos;
            }
            else
            {
                assert(_prev);
                _n = _prev;
                _pos = _n->size;

                assert(_pos);
                --_pos;
            }
        }
#endif
        
    private:

        void _init() { _prev = 0; }
        
        Node*   _prev;
    };


    List(): Head(type_list) { _init(); }
    List(int n) : Head(type_list) { _n = create(n); }
    List(Type t) : Head(t) { _init(); }

    explicit List(const Head& a, int) : Head(type_list)
    {
        // singleton
        _n = create(1);
        _n->at(0) = a.ref();
        ++_n->size;
    }

    explicit List(const Head& a, const Head& b) : Head(type_list)
    {
        // pair
        _n = create(2);
        _n->at(0) = a.ref();
        _n->at(1) = b.ref();
        _n->size = 2;
    }
    
    ~List()
    {
        DBIF(!isEmpty(), "~" << *this);
        clear();
    }

    void clear()
    {
        if (owned()) _purge();
        _init();
    }

    // convert to sbuf
    void toSBuf(SBuf& sb) const
    {
        int cc = 0;
        sb.add('(');
        for (iterator it(*this); it; ++it)
        {
            if (cc++) sb.add(' ');
            (*it).toSBuf(sb);
        }
        sb.add(')');
    }

    void _init()
    {
        _n = 0;
    }

    void _purgeNodes()
    {
        // delete the chain nodes but not the content
        iterator it(*this); 
        while (it.valid()) it.eraseNode();

        // no contents
        _init();
    }

    static void _purgeEnt(Head& h)
    {
        // are morphology if we are a temp iterator 
        // can still be a referenced HeadTmp.
        if (h.owned() || h._refCount > 0)
            h.drop();
        else
        {
            // the HEAD is temp from the iterator. we own the substructure.
            ((List&)h)._purge();
        }
    }

    void _purge()
    {
        // drop the content
        for (iterator it(*this); it.valid(); ++it)
            _purgeEnt(*it);

        // and drop the chain nodes themselves
        _purgeNodes();
    }

    static List* create(Group& g);

    void fold(const Group& g, SBuf& sb) const;
    void unfold(Group& g, SBuf& sb);

    static Node* create(uint s)
    {
        // create space for `s' logical entries
        Node* n = 0;
        if (s > 0)
        {
            n = Node::create(s); 
            n->at(s-1) = 0; // no link
        }
        return n;
    }

    static Node* createz(uint s)
    {
        // create space for `s' logical entries zeroed
        Node* n = create(s);
        if (n) memset(&n->at(0), 0, sizeof(Node*)*s);
        return n;
    }

    void append(const Head& h)
    {
        iterator it(*this);
        it.end();
        it.insert(h);
    }

    void insert(const Head& h, uint at)
    {
        iterator it(*this);
        it.skip(at);
        it.insert(h);
    }

    bool member(const Head& h)
    {
        ComparatorEq co;
        return member(h, co);
    }

    bool member(const Head& h, Comparator& co)
    {
        for (iterator it(*this); it; ++it)
        {
            if (!co(*it, h))
                return true;
        }
        return false;
    }

    uint size() const
    {
        iterator j(*this);
        return j.size();
    }

#if 0
    uint sizeDeep() const
    {
        // recursive size
        uint n = 0;
        for (iterator j(*this); j.valid(); ++j)
        {
            if (j->isListBased())
                n += ((const List&)*j).sizeDeep();
            else
                ++n;
        }
        return n;
    }
#endif

    bool isEmpty() const
    {
        iterator it(*this);
        return !it.valid();
    }

    Node*&       nodeAt(uint i) const
    {
        // raw access of list element
        // ASSUME exists
        iterator it(*this);
        it.skip(i);
        assert(it.valid());
        return _n->at(it._pos);
    }

    static int compare(iterator& i1, iterator& i2, Comparator& co)
    {
        // compare two lists from their iterators
        int c = 0;
        while (i1)
        {
            if (i2)
                c = co(*i1, *i2);
            else 
            {
                // i1 valid > i2 invalid
                // (A B C) > (A B)
                return 1;
            }
            if (c) break;
            ++i1;
            ++i2;
        }

        if (!c && i2)
        {
            // i1 invalid < i2 valid
            // (A B) < (A B C)
            c = -1;
        }
        return c;
    }

    int compare(const List& li, Comparator& co) const
    {
        iterator i1(*this);
        iterator i2(li);
        return compare(i1, i2, co);
    }

    static Term parse(Stream& s, ParseContext* ctx)
    {
        if (*s == '(')
        {
            int openline = s.line();

            ++s;
            List l;
            iterator it(l);
            Term ti;

            // allow list merging
            while ((ti = parseTerm(s, ctx))) it.insert(ti.demote());
            
            while (u_isspace(*s)) ++s;
            if (*s == ')') ++s;
            else
            {
                // if we're missing a close, store the line where
                // we opened it.
                ctx->_missingCloseLine = openline;
            }

            return l;
        }
        return Term();
    }

private:

    // might not need these...
    List(const List& l) : Head(type_list) { _init(); *this = l; }
    
    List& operator=(const List& l)
    {
        // copy by donate semantics
        _purge();
        _n = l._donate();
        return *this;
    }

};

