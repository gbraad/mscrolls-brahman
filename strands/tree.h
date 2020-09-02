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

#include <functional>
#include "list.h"
#include "match.h"

struct Tree: public List
{
    struct iterator: public iterator_base
    {
        // a node varies between the inital size and the max size
        // this number of key slots of a non-leaf (m), is even.
        // so, non-leaves have m keys and (m+1) subtrees = 2m+1 slots
        // leaves have up to 2m keys = 2m slots.
        // so a non-leaf has a ODD number of slots.
        // leaves have EVEN number of slots.

#ifdef TEST_MODE
        // these are the small debug sizes
        static const uint initLeafSize = 4;
        static const uint maxLeafSize = 8;
        static const uint growNodeSize = 4;
#else
        static const uint initLeafSize = 8;
        static const uint maxLeafSize = 4096;
        static const uint growNodeSize = 256;
#endif

        // nodes have odd number.
        static const uint initNodeSize = initLeafSize + 1;
        static const uint maxNodeSize = maxLeafSize + 1;


        typedef iterator_base parentT;
        typedef Match::Op Op;

        struct UpLink
        {
            UpLink() {}
            UpLink(const UpLink& l)
            {
                _n = l._n;
                _pos = l._pos;
                _up = 0; // do not copy `up'
            }

            Node*               _n;
            unsigned int        _pos;
            UpLink*             _up;
        };
            
        // Constructors
        iterator() { _init(); }
        iterator(const List& l) : parentT(l) { _init(); }
        iterator(const iterator& it) : parentT(it)
        {
            _init();
            _dupStack(it._uStack);            
        }

        ~iterator() { _purge(); }
        
        explicit operator bool() const { return valid(); }

        bool valid() const { return (bool) _n; }
        bool onSup() const { return _pos == _n->size; }

#if 0
        // not used
        bool onInf() const
        {
            // on the min node of the tree
            bool inf = _pos == 0 && isLeaf();
            if (inf)
            {
                // walk up the stack to check we're on the min node at
                // each level
                UpLink* up = _uStack;
                while (up)
                {
                    if (up->_pos)
                    {
                        // not min
                        inf = false;
                        break; 
                    }
                    up = up->_up;
                }
            }
            return inf;
        }
#endif        

        // crude way to signal invalid point.
        void toSup() { if (_n) _pos = _n->size; }

        iterator& operator=(const iterator& it)
        {
            _purge();
            parentT::operator=(it);

            _dupStack(it._uStack);
            return *this;
        }

        iterator& operator++() { _bump(); return *this; }

        // Features
        bool isLeaf() const
        {
            // leaves have an even number of keys.
            // ASSUME valid
            return (_n->space & 1) == 0;
        }

        void toMin()
        {
            // move down to min, from the current position
            _pos = 0;
            if (valid())
                while (!isLeaf()) _push();
        }

        void toMax()
        {
            if (valid())
            {
                for (;;)
                {
                    assert(_n->size);
                    _pos = _n->size - 1;
                    if (isLeaf()) break;
                    _push();
                }
            }
        }

        bool find(const Head& h) 
        {
            // locate from here down
            return find(Match(h));
        }

        bool find(const Match& mop)
        {
            // locate from here down, true if match
            bool res = false;
            if (valid())
            {
                res = locate(mop);
                if (!res)
                {
                    // locate can leave `pos' at sup of node
                    revalidate();
                }
            }
            return res;
        }

        void revalidate()
        {
            // locate can leave `pos' at sup of node
            if (onSup())
            {
                assert(_pos);
                
                // move to next actual position
                --_pos;  // adjust to last valid entry (from sup)
                _bump();
            }
        }

#if 0
        bool intern(const Head& h)
        {
            return _insert(h, &iterator::_opIntern);            
        }
#endif

        bool insert(const Head& h)
        {
            // return true if inserted, false if present already
            using std::placeholders::_1;
            return _insert(h, std::bind(&iterator::_opInsert, this, _1));
        }

#if 0
        bool insertConsume(Node* node)
        {
            // return true if inserted, false if present already
            using std::placeholders::_1;
            return _insert(*(Head*)node,
                           std::bind(&iterator::_opInsertConsume, this, _1));
        }
#endif        

        void insertHere(const Head& h)
        {
            // ASSUME iterator is at correct position!
            if (!valid())
                _initalise();
            
            _opInsertAux(h.ref(), 0);                
        }

        void eraseHere()
        {
            assert(valid());
            _removeAux();
            // leave pos potentially onSup
        }

        Head& swapHere(const Head& h)
        {
            assert(valid());

            // replace the current element with the given `h'ead.
            // `_purgeEnt' should be called on the returned head.

            // NOTE: it is ASSUMED that the replacing element does not
            // disturb the order of the tree.

            Head& here = **this;
            _n->at(_pos) = h.ref();
            return here;
        }

        void reset()
        {
            _purge();
            parentT::reset();
        }

        bool locate(const Match& mop)
        {
            if (isLeaf())
            {
                // range [x)
                unsigned int lo = 0;
                unsigned int hi = _n->size;

                if (hi)
                {
                    while (hi > lo)
                    {
                        _pos = (lo + hi) >> 1;
                        int v = mop(_head(_pos));
                        if (v < 0)
                        {
                            // target < pos
                            hi = _pos;
                        }
                        else if (v > 0)
                        {
                            // target > pos
                            lo = _pos+1;
                        }
                        else
                        {
                            // hit a match!
                            return mop.found(true);
                        }
                    }
                }

                // set position to the min upper bound
                // insert will happen *before* this point
                // NB: position may be off end if no sup
                _pos = hi;
                return mop.found(false);
            }
            else
            {
                // range [x)
                unsigned int m = _n->size >> 1; // number of keys
                unsigned int lo = 0;
                unsigned int hi = m;
                unsigned int p;
                while (hi > lo)
                {
                    p = (lo + hi) >> 1;
                    _pos = (p<<1) + 1; // odd values are keys
                    int v = mop(_head(_pos));
                    if (v < 0)
                    {
                        // h < pos
                        hi = p;
                    }
                    else if (v > 0)
                    {
                        // h > pos
                        lo = p + 1;
                    }
                    else
                    {
                        // hit a match on a non-leaf key
                        return mop.found(true);
                    }
                }

                // `hi' is an upper bound
                // set pos to the subtree left of the upper bound
                // = (2*hi+1)-1. 
                // when we are greater than the max value, this is the
                // rightmost subtree
                _pos = hi << 1;
                
                // move down
                _push();
                assert(_n);
                
                // recurse in subtree
                return locate(mop);
            }
            return false;
        }

#if 0
        bool firstInNode()
        {
            bool res = valid();
            if (res)
            {
                _pos = 0;
                if (!isLeaf())
                    ++_pos;  // non leaves are not empty
            }
            return res;                
        }

        bool nextInNode()
        {
            // move forward within this node and do not go into subnodes
            bool res = valid();
            if (res)
            {
                unsigned int p = _pos + 1;
                if (!isLeaf())
                {
                    ++p;
                    assert(p & 1); // ensure on a key 
                }
                
                res = p < _n->size;
                if (res)
                    _pos = p;
            }
            return res;
        }

        bool toSubNode(bool left = true)
        {
            // move to subnode from the current position
            bool res = valid() && !isLeaf();
            if (res)
            {
                assert(_pos & 1); // ensure we're parked on a key
                if (left)
                    --_pos;
                else
                    ++_pos;
                
                _push();

                // NB: not necessarily a valid value for the subnode
                _pos = 0;
            }
            return res;
        }
#endif


    protected:

        void _initalise()
        {
            // tree is empty, create a leaf
            _l->_n = List::create(initLeafSize);
            _n = _l->_n;
        }

        bool _insert(const Head& h, Op op)
        {
            // always insert from the top
            reset();
            
            if (!valid())
                _initalise();
            
            Match mop(h);
            mop.setOp(op);

            // if fail, then pos is parked on old match
            // if true, then insert at pos
            return locate(mop);
        }

        void _push()
        {
            UpLink* up = new UpLink;
            up->_n = _n;
            up->_pos = _pos;
            up->_up = _uStack;
            _uStack = up;
            _n = _n->at(_pos);
        }

        bool _pop()
        {
            _n = 0;
            UpLink* up = _uStack;
            if (up)
            {
                _n = up->_n;
                _pos = up->_pos;
                _uStack = up->_up;
                delete up;
            }
            return _n != 0;
        }

        void _dumpStack()
        {
            UpLink* up;
            while ((up = _uStack) != 0)
            {
                _uStack = up->_up;
                delete up;
            }
        }

        void _dupStack(UpLink* up)
        {
            // `up' is the bottom of another stack.
            // make a stack copy for us.
            assert(!_uStack);

            UpLink* p = 0;
            while (up)
            {
                UpLink* l = new UpLink(*up);
                if (p) p->_up = l;
                else _uStack = l;
                p = l;
                up = up->_up;
            }
        }

        void _bump()
        {
            // ASSUME valid
            if (++_pos >= _n->size)
            {
                if (_pop()) _bump();
            }
            else
            {
                if (!isLeaf())
                {
                    if (!(_pos & 1))
                    {
                        // even => subnode pointer (not key)
                        // drop down and left
                        _push();
                        toMin();
                    }
                }
            }
        }
        
        bool _grow(uint maxed)
        {
            uint sz = _n->size;
            uint sz2 = u_min(sz + growNodeSize, maxed);
            if (sz2 != sz)
            {
                // can resize
                Node* nn = List::create(sz2);

                // copy node content to new node
                memcpy(&nn->at(0), &_n->at(0), sz*sizeof(Node*));
                nn->size = sz;

                // wire into parent
                if (_uStack)
                    _uStack->_n->at(_uStack->_pos) = nn;
                else
                {
                    // list top node changes
                    _l->_n = nn;
                }

                Node::destroy(_n);
                _n = nn;
                return true;
            }
            return false;
        }

        Node* _split(Node* mn, uint med, uint initSize)
        {
            // split a node at `med' taking [med...sz) to new node
            
            uint r = _n->size - med;

            assert(r <= initSize);

            // create split node
            Node* spln = List::create(initSize);

            // split the keys
            memcpy(&spln->at(0), &_n->at(med), r*sizeof(Node*));
            spln->size = r;

            // clear last slot so that the node can be interpreted as
            // a list node with no link, since now size < space
            _n->size = med;
            _n->link() = 0;

            // wire these into parent
            if (_uStack)
            {
                // unwind
                _pop();
                ++_pos; // move to key, NB: can be at size

                // need to insert m/rn just before pos
                _opInsertAux(mn, spln);
            }
            else
            {
                // we're a leaf child of the root
                // create a new non-leaf node
                Node* nn = List::create(initNodeSize);
                nn->at(0) = _n; // original node at left
                nn->at(1) = mn; // key in middle
                nn->at(2) = spln; // split node on right
                nn->size = 3;
                _l->_n = nn; // become new root
            }
            return spln;
        }

        bool _opInsert(Match* m)
        {
            if (m->_found) return false; // already present!
            _opInsertAux(m->_m.ref(), 0);
            return true;
        }

#if 0        
        bool _opInsertConsume(Match* m)
        {
            // this version assumes that we do not need to make 
            // a ref from the head because it's either an extracted
            // head (already refed), or a node.

            if (m->_found) return false; // already present!
            _opInsertAux((Node*)&m->_m, 0);
            return true;
        }
#endif        

        void _opInsertAux(Node* in, Node* sub)
        {
            uint sz = _n->size;
            Node* on = _n;
            uint op = _pos;

            if (isLeaf())
            {
                // we have an even size.

                assert(sub == 0); // no subtrees in leaves!

                // insert into a leaf `pos' is the upper bound
                if (_n->full() && !_grow(maxLeafSize))
                {
                    // note sz is even
                    uint med = sz/2;
                    
                    // the split size is half original, rounded to even
                    // and, at least, the min size
                    uint ss = u_max((med + 1) & ~1, initLeafSize);
                    
                    if (_pos > med)
                    {
                        // want to insert new key into split node
                        // take all keys [med+1,...sz) to split node
                        // NB: changes `pos'
                        _n = _split(_n->at(med), med+1, ss);
                        
                        // adjust for loss of median
                        --on->size;
                            
                        // now on split node, at pos
                        _pos = op - (med+1);
                    }
                    else if (_pos < med)
                    {
                        // median to split on. 
                        // will be adding to left side, adjust
                        //--med;

                        // take all keys [med+1,...sz) to split node
                        // NB: changes n and pos
                        _split(_n->at(med-1), med, ss);

                        // adjust for loss of median
                        --on->size;

                        // restore original node position
                        // NB: upstack broken
                        _n = on;
                        _pos = op;
                    }
                    else
                    {
                        // split on new key
                        _split(in, med, ss);

                        // no insert needed!
                        return;
                    }
                }

                // now insert into node
                Node*& p = _n->at(_pos);
                memmove(&p + 1, &p, (_n->size-_pos)*sizeof(Node*));

                // insert at pos
                p = in;
                
                // new logical size
                ++_n->size;
            }
            else
            {
                // insert into non-leaf. we have a key and a subtree
                // insert just before _pos
                // we have an odd size.

                assert(sz & 1);
                assert(sub != 0);  // must have a subtree
                
                // we're full if we have no space for the two items
                bool full = sz + 2 > _n->space;

                if (full && !_grow(maxNodeSize))
                {
                    uint med = ((sz>>2)<<1)+1; // median key to split on

                    // the split size is half original, rounded to odd
                    // and, at least, the min size
                    uint ss = u_max((((sz>>1)+1)&~1)+1, initNodeSize);

                    if (_pos > med)
                    {
                        // we are inserting into split right
                        // takes nodes [med+1,...sz)
                        _n = _split(_n->at(med), med+1, ss);
                        
                        // adjust for loss of median
                        --on->size;
                        
                        // want to insert new key into split node
                        // locate on to split node
                        _pos = op - (med+1);
                    }
                    else if (_pos < med)
                    {
                        // adjust to previous key, providing we leave
                        // at least one key remaining
                        med -= 2;
                        
                        assert(med > 1);
                        
                        // median to split on
                        _split(_n->at(med), med+1, ss);

                        // adjust for loss of median
                        --on->size;
                        
                        // restore old position
                        _n = on;
                        _pos = op;
                    }
                    else
                    {
                        // split node with a left subtree at (med-1)
                        Node* spln = _split(in, med-1, ss); 

                        // but replace that subtree with given sub
                        spln->at(0) = sub;
                        
                        // adjust size because we want that original left
                        // subtree
                        ++on->size;

                        // no insert required
                        return;
                    }
                }

                // now insert into node
                Node*& p = _n->at(_pos);
                memmove(&p + 2, &p, (_n->size-_pos)*sizeof(Node*));

                // put in key and subtree at pos
                (&p)[0] = in;
                (&p)[1] = sub;
                
                // adjust size
                _n->size += 2;
            }
        }

        void _pullRight(Node* r)
        {
            assert(r->size);
            assert(_n->size + 1 < _n->space);

            // parent
            assert(_uStack);
            Node* p = _uStack->_n;

            assert(_uStack->_pos < p->size);
            Node*& np = _n->at(_n->size);
            Node*& pp =  p->at(_uStack->_pos);

            // move parent key here
            np = pp;
            ++_n->size;
            
            // move leftmost right node key to parent
            Node*& rp = r->at(0);
            pp = rp;

            memmove(rp, rp + 1, --r->size*sizeof(Node*));
            r->link() = 0;  // restore link slot
        }

        void _pullLeft(Node* l)
        {
            assert(l->size);
            assert(_n->size + 1 < _n->space);

            // parent
            assert(_uStack);
            Node* p = _uStack->_n;

            assert(_uStack->_pos >= 2);
            Node*& np = _n->at(_n->size);
            Node*& pp =  p->at(_uStack->_pos - 2);

            // move parent key here
            np = pp;
            ++_n->size;
            
            // move rightmost left node key to parent
            Node*& lp = l->at(l->size--);
            pp = lp;
            
            l->link() = 0;  // restore link slot
        }

        void _removeAux()
        {
            if (isLeaf())
            {
                _purgeEnt(_head(_pos));

                if (--_n->size)
                {
                    Node*& p = _n->at(_pos);
                    memmove(&p, &p + 1, (_n->size - _pos)*sizeof(Node*));
                    
                    // ensure last slot is zero now size < space
                    _n->link() = 0;
                }
                else
                {
                    // leaf is empty
                    if (_uStack)
                    {
                        Node* r = 0;
                        Node* l = 0;
                        
                        assert(_uStack->_n);

                        // stack `pos` points to key just > 
                        // so pos+1 is right node (if any)
                        uint p = _uStack->_pos + 1;
                        if (p < _uStack->_n->size)
                        {
                            r = _uStack->_n->at(p);
                            assert(r);

                            // should be a leaf
                            assert((r->size & 1) == 0);
                        }
                        if (p >= 4)
                        {
                            // left node. origianal pos-3 if >= 0
                            l = _uStack->_n->at(p - 4);
                            assert(l);
                            assert((l->size & 1) == 0);
                        }

                        assert(r || l);

                        int rz = r ? r->size : 0;
                        int lz = l ? l->size : 0;

                        if (rz > lz)
                        {
                            // pull from right
                            _pullRight(r);
                        }
                        else
                        {
                            // pull from left
                            _pullLeft(l);

                        }
                    }
                    else
                    {
                        // tree is empty
                        assert(_n == _l->_n);
                        Node::destroy(_n);
                        _l->_n = 0;
                        reset();
                    }
                    
                }
            }
            else
            {
                // XXX todo
                assert(false);
            }
        }


#if 0
        bool _opRemove(Match* m)
        {
            bool res = m->_found;
            if (res)
            {
                // was found, so remove it.
                _opRemoveAux();
                return true;
            }
            return res;
        }

        bool _opIntern(Match* m)
        {
            if (m->_found)
            {
                // remove old value
                _opRemoveAux(); 
            }

            // and insert
            _opInsertAux(m->_term.ref(), 0);
            return true;
        }
#endif

        void _init()
        {
            _uStack = 0;
        }

        void _purge() { _dumpStack(); }

    private:
        
        UpLink*        _uStack;
    };

    // Constructors
    Tree() : List(type_tree) {}

    // convert to sbuf
    void toSBuf(SBuf& sb) const
    {
        sb.add('{');
        int c = 0;
        iterator it(*this);
        for (it.toMin(); it; ++it, ++c)
        {
            if (c) sb.add(' ');
            (*it).toSBuf(sb);
        }
        sb.add('}');
    }

    uint size() const
    {
        uint n = 0;
        iterator it(*this);
        for (it.toMin(); it; ++it) ++n;
        return n;
    }

    bool insert(const Head& h)
    {
        // insert single element
        iterator it(*this);
        return it.insert(h);
    }

#if 0
    // was used to collect subs
    bool consume(Tree& tr)
    {
        bool res = true;
        Tree::iterator it(tr);
        Tree::iterator dest(*this);
        for (it.toMin(); it; ++it)
        {
            // get the old head from the source tree, which 
            // could be a temporary. 
            // we take care to maintain its temp status
            Head& h = it.swapHere(List());

            // is it a temp?
            bool temp = !h.owned() && h._refCount == 0;

            // prepare pointer ready to insert without conversion
            Node* node = h._n;
            if (!temp) SET_HEAD(node, &h);
            
            if (!dest.insertConsume(node)) 
                res = false;
        }

        tr.clear();
        return res;
    }
#endif    

#if 0
    bool insert(const Tree& tr)
    {
        // insert tree
        bool res = true;
        iterator it(*this);
        
        iterator i(tr);
        for (i.toMin(); i.valid(); ++i)
        {
            if (!it.insert(*i)) 
                res = false;
        }
        return res;
    }
#endif

#if 0
    bool remove(Head& h)
    {
        iterator it(*this);
        return it.remove(h);
    }
#endif

    int compare(const Tree& t, Comparator& co) const
    {
        //if (subset(t, co)) return -1;
        //if (t.subset(*this, co)) return 1;
        return compareSame(t, co);
    }

    int compareSame(const Tree& li, Comparator& co) const
    {
        // since trees are ordered, we can compare term by term.
        // the final order being any mismatch order.
        iterator i1(*this);
        i1.toMin();

        iterator i2(li);
        i2.toMin();

        int c = 0;
        while (i1.valid())
        {
            if (i2.valid())
                c = co(*i1, *i2);
            else 
            {
                // i1 valid > i2 invalid
                c = 1;
            }
            if (c) break;
            ++i1;
            ++i2;
        }

        if (!c && i2.valid())
        {
            // i1 invalid < i2 valid
            c = -1;
        }
        
        return c;
    }

    bool subset(const Tree& b, Comparator& co) const
    {
        // this tree is a subtree of `b'
        bool res = true;
        Tree::iterator ia((const Tree&)*this);
        Tree::iterator ib(b);
                
        for (ia.toMin(); ia.valid(); ++ia)
        {
            Match sf2(*ia);

            // override comparator
            sf2._co = &co;
            
            ib.reset();
            if (!ib.find(sf2))
            {
                res = false;
                break;
            }
        }
        return res;
    }

    static Tree* create(Group&);

    static Term parse(Stream& s, ParseContext* ctx)
    {
        if (*s == '{')
        {
            // parse a set tree
            ++s;
            Tree t;
            iterator it(t);
            Term ti;
            while ((ti = parseTerm(s, ctx))) it.insert(ti.demote());

            while (u_isspace(*s)) ++s;
            if (*s == '}') ++s;
            return t;
        }
        return Term();
    }
};

