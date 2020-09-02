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


#include "kl.h"
#include "sat.h"

// (~ '(likes (fish _x)) '{(is moose) (likes {(fish cooked) (steak raw)})})
//   = ({(_x cooked)})
// (~ '(likes (steak _x)) '{(is moose) (likes {(fish cooked) (steak raw)})})

int ComparatorSat::_satList(const Head& a, const Head& b, int& headmatch)
{
    // recursively match elements of lists in sequence.
    // allow `a' to be shorter than `b'.
    // eg (a) ~ (a b)
    // (a b) ~ ((a c) b) ; term by term
    // (a b) ~ ((a b) c)

    // NB:
    // (a _x) ~ ((a b) c), with _x = c, due to term by term,
    // rather than _x=b
    
    const List& l1 = (const List&)a;
    const List& l2 = (const List&)b;

    // find `headMatch' which is the match of the heads of the lists
    List::iterator i1(l1);
    List::iterator i2(l2);

    int r = 0;
    bool first = true;
    headmatch = 0;

    // first try term by term sat
    // allow `a' to be shorter than `b'.
    while (i1)
    {
        if (i2)
        {
            r = _sat(*i1, *i2);
            if (first)
            {
                first = false;
                headmatch = r;                
            }
        }
        else 
        {
            // i1 valid > i2 invalid
            // (A B C) > (A B)
            r = 1;
        }
        if (r) break;
        ++i1;
        ++i2;
    }

    if (r)
    {
        // (~ a (first b)), since a ~ (a b)
        List::iterator it(l2);
        if (it)
            r = _sat(a, *it);
    }

    return r;
}

int ComparatorSat::_satSet(const Head& a, const Head& b)
{
    // a ~ S
    // L ~ S

    // eg a ~ { a b c d e .. }
    // eg a ~ { b c d e (a x) .. }
    // eg a ~ { b c d e (u v) ((a x) y) .. }

    // order by (order-primary, whole object)
    // then lists go before nested lists.
    // and sets go after lists.

    int cc = 0;
    
    Tree::iterator ib((const Tree&)b);

    int r = a._type - b._type;

    // use a strict <= match to locate the start of the search
    Match m(a);

    // locate sup(a), which could be a match
    bool hit = ib.find(m);

    //if (ib) LOG3("set matching ", a << " starts at  " << *ib);

    if (hit)
    {
        // will only be a hit if no subs on left, since
        // we assume none in Set.
        r = 0;
    }
    else
    {
        if (a._type == type_list)
        {
            // List ~ Set.
            //
            // `ib' is parked at the first possible match
            // 
            // we must manually search from this point for
            // a subst match WHILE the heads are matched
            // after that, ASSUME no more matches possible.

            for (; ib; ++ib)
            {
                ++cc;

                int rr;
                int headmatch;
                
                SubsTree st;
                if (_subs) push(st);

                const Head& bi = *ib;
                if (bi.isList())
                {
                    // L1 ~ L2
                    rr = _satList(a, bi, headmatch);
                }
                else
                {
                    // L ~ x, x not List.
                    // NB: possible match only if x is Set.

                    rr = _sat(a, bi);                        

                    // if set, need to continue and process ALL trees
                    if (bi.isTree())
                    {
                        headmatch = 0;
                    }
                    else
                    {
                        // otherwise not a match
                        headmatch = rr;
                    }
                }

                if (_subs)
                {
                    pop();
                    if (!rr) subsOK(st);
                }
                        
                if (!rr) r = 0;

                // when heads don't match, we stop
                if (headmatch) break;

                // if we're not collecting subs and have a match
                // no point in continuing search
                if (!_subs && !r) break;
            }
        }
        else
        {
            // a ~ Set. a not list, not set. atomic.
            // also not direct match.
            // leave `r' to be the match of type. 
            //LOG3(a, " ~ " << b << " = " << r);
        }
    }

    return r;
}

int ComparatorSat::_sat(const Head& a, const Head& b)
{
    if (a._type == type_symbol)
    {
        // _v ~ A, symbols match anything

        const Symbol& as = (const Symbol&)a;
        if (as.isVariable())
        {
            // anon matches anything but we don't collect subs
            if (!as.isAnonVariable())
            {
                // _x ~ A with sub (_x A)
                if (_subs) _subs->_s.insert(List(a, b));
            }
            return 0;
        }
    }

    int r = a._type - b._type;
    if (r)
    {
        // terms are different types
        //  x ~ S (x not a set), true if some b in S: x ~ b.
        //  x ~ L (x not a list), if x ~ (first L)
        if (b._type == type_list)
        {
            //  a ~ List, if a ~ (first List)
            //  S ~ List, if S ~ (first List)
            List::iterator it((const List&)b);
            if (it)
                r = _sat(a, *it);
        }
        else if (b._type == type_tree)
        {
            //  a ~ Set
            //  L ~ Set

            // first try plain match
            r = _satSet(a, b);
        }
        
        // else:
        // List ~ a => no match
        // Set ~ a => no match.
    }
    else
    {
        // otherwise types agree

        // L1 ~ L2
        // S1 ~ S2, if for-all x in S1, x ~ S2.
        // a ~ a (atoms)

        switch (a._type)
        {
        case type_list:
            {
                // L1 ~ L2
                int mhead;
                r = _satList(a, b, mhead);
            }
            break;
        case type_tree:
            {
                // S1 ~ S2
                Tree::iterator ia((const Tree&)a);

                // XX
                // need to combine subst here. take the intersection
                // of the substs for each part of the set.
                
                r = 0;
                SubsTree st;
                if (_subs) push(st);
                for (ia.toMin(); ia; ++ia)
                {
                    r = _sat(*ia, b);
                    if (r) break;  // must all match
                }

                if (_subs) 
                {
                    pop();
                    if (!r) subsOK(st);
                }
            }
            break;
        default:
            {
                // a ~ b, atoms, if a == b.
                ComparatorEq co;
                r = co(a, b);
            }
            break;
        }
    }
    return r;    
}

//// ENT

int ComparatorEnt::_entListHeads(List::iterator& i1, List::iterator& i2)
{
    // compare just the heads of two lists, to check (first L1) ~ (first L2).
    // leave iterators on the next element.
    
    bool v1 = i1.valid();
    bool v2 = i2.valid();

    int r = 0;
    if (v1)
    {
        if (v2)
        {
            Entails e;
            push(e);
            r = _ent(*i1, *i2);
            pop();

            ++i1;
            ++i2;
        }
        else 
        {
            // i1 valid > i2 invalid
            // (A B C) > (A B)
            r = 1;
        }
    }
    else if (v2) r = -1; // i1 invalid < i2 valid
    return r;
}

int ComparatorEnt::_entListBodies(List::iterator& i1, List::iterator& i2)
{
    // entail two list bodies by entailing corresponding elements.
    // ∀ x ∈ L1, y ∈ L2 corresponding to x; x ⊢ y.
    // Note, that L1 can be shorter than L2.

    int r = 0;
    while (i1)
    {
        if (i2)
        {
            // when erasing, we do not entail the body
            // we only compare it, since the erase is top-level
            if (entails() && !erase())
            {
                // arrange to modify `i2' during entails
                // NOTE: the l-value is the address of the second match term.
                LValue lv(i2);
                Entails e(lv);
                push(e);
                r = _ent(*i1, *i2);
                assert(r == 0);
                pop();
            }
            else
                r = _ent(*i1, *i2);

            ++i2;
        }
        else 
        {
            // i1 valid > i2 invalid
            // (A B C) > (A B)
            r = 1;

            if (entails() && !erase())
            {
                // append remainder to list
                // NB: `i2' invalid on end.
                LValue lv(i2);
                lv = *i1;
                r = 0;
            }
        }
        if (r) break;
        ++i1;
    }

    // allow L1 to be shorter than L2, since (a b) ~ (a b c).
    return r;
}

int ComparatorEnt::_entLists(const Head& a, const Head& b)
{
    // L1 ⊢ L2.
    // if (first L1) ~ (first L2), then entail corresponding elements

    List::iterator i1((const List&)a);
    List::iterator i2((const List&)b);

    // match heads
    int r = _entListHeads(i1, i2);
                        
    if (!r)
    {
        if (overwrite())
        {
            assert(entails());

            // claim a match regardless of any subsequent terms.
            // since these are to be replaced.
            _entails->_overwriteMatch = true;
            
        }
        else r = _entListBodies(i1, i2);
    }

    return r;
}

int ComparatorEnt::_entSet(const Match& m, const Head& a, const Head& b)
{
    //  x ⊢ Set (x not set)

    Tree::iterator ib((const Tree&)b);

    // use a strict <= match to locate the start of the search
    // locate sup(a), which could be a match
    bool hit = ib.find(m);

    int r = 1;
    if (a._type == type_list)
    {
        // List ⊢ Set.
        // `ib' is parked at the first potential match in Set. 
        // if the potential match is a list with a head match, entail.

        if (ib)
        {
            const Head& bi = *ib;
            if (bi.isList())
            {
                hit = false;

                // if heads match, entail here.
                r = _entLists(a, bi);
                if (!r && entails())
                {
                    if (_entails->_overwriteMatch) hit = true;
                    else if (erase())
                    {
                        // remove whole list from set
                        ib.eraseHere();
                    }
                }
            }
            else
            {
                // (foo bar) ⊢ foo

                /*
                if (hit)
                {
                    // replace, eg foo by (foo bar)
                    LValue lv(ib);
                    lv = a;
                    r = 0;
                }
                */
            }

            if (hit && !erase())
            {
                // overwrite
                LValue lv(ib);
                lv = a;
                r = 0;
            }
        }
    }
    else
    {
        // other types are match only if direct hit.
        if (hit) r = 0;
    }
    return r;
}

int ComparatorEnt::_entFromSet(const Head& a, const Head& b)
{
    // Set ⊢ b, b not a set.

    // eg {a b} ⊢ c = {a b c}
    // eg {a b} ⊢ b = {a b}
    // eg {(a b) c} ⊢ a = {(a b) c}
    // eg {(a b) c} ⊢ (c d) = {(a b) (c d)}

    // XX todo, can we erase a set at once?
    if (erase()) return 1;

    // see if b ~ Set. If so then Set is the result
    Entails e;
    push(e);
    int r = _ent(b, a);
    
    if (!r)
    {
        pop();
        set(a);
    }
    else
    {
        Tree tr;
        Tree::iterator itr(tr);
        LValue lv(itr);
        
        Tree::iterator ia((const Tree&)a);
        bool added = false;
        for (ia.toMin(); ia; ++ia)
        {
            r = _ent(*ia, b);
            itr.toSup();
            if (r)
                lv = *ia;
            else
            {
                lv = b;
                added = true;
            }
        }    

        if (!added)
        {
            itr.toSup();
            lv = b;
        }

        r = 0;
        pop();
        set(tr);
    }
    return r;
}

/* tests:

 LISTS
 (|- '(cat a) '(dog b))
 (|= '(cat a) '(dog b))
        = {(cat a) (dog b)}
 (|- '(cat a) '(cat b)) 
        = (cat {a b})
 (|= '(cat a) '(cat b)) 
        = (cat a)
 (|- '(cat a) '(cat a)) 
        = (cat a)
 (|- '(a b c) '(a b))
        = (a b c)  "use case"
 (|- 'cat '(cat fat))
 (|= 'cat '(cat fat))
        = (cat fat)
 (|- '(cat (like fish)) '(cat (chase mouse)))
 (|= '(cat (like fish)) '(cat (chase mouse)))
        = (cat {(chase mouse) (like fish)})
 (|= '(cat (like fish)) '(cat (like milk) (chase mouse)))
        = (cat (like fish) (chase mouse))
 (|- {a b} 'c)
 (|- '(like {fish chips}) '(like sausage))
        = (like {chips fish sausage})
 (|= '(like {fish chips}) '(like sausage))
         = (like {chips fish})
 (|= '(jim (like {fish chips})) '(jim (like sausage)))
        (jim (like {chips fish}))

 ATOMS
 (|- 'a 'a)
        = a
 (|- 'a 'b)
        = {a b}
 (|= 'a 'b)
        = a

 SET
 (|- '(cat (chase mouse)) {(dog x) (cat y)})
 (|= '(cat (chase mouse)) {(dog x) (cat y)})
        = {(cat {y (chase mouse)}) (dog x)}
 (|- '(cat fat) {dog cat})
 (|= '(cat fat) {dog cat})
        = {dog (cat fat)}
 (|- '(cat fat) {dog fish})
        = {dog fish (cat fat)}
 (|- '(cat (chase mouse)) {(dog x) (cat (chase bird))})
        = {(cat (chase {bird mouse})) (dog x)}
 (|= '(cat (chase mouse)) {(dog x) (cat (chase bird))})
        = {(cat (chase mouse)) (dog x)}

 (|- 'cat {dog})
        = {cat dog}
 (|- 'cat {cat dog})
        = {cat dog}
 (|- 'cat {(cat fat) dog})
 (|= 'cat {(cat fat) dog})
        = {dog (cat fat)}
 */

int ComparatorEnt::_ent(const Head& a, const Head& b)
{
    // L1 ⊢ L2
    // if (first L1) ~ (first L2) then
    // ∀ a ∈ L1, corresponding b ∈ L2, a ⊢ b, [m+1]
    // ELSE { L1 L2 }
    // (cat (chase mouse)) ⊢ (cat (like fish))
    //        => (cat {(chase mouse) (like fish)})  [m1]
    //
    // ((cat fat) (like fish)) ⊢ (cat (chase mouse))
    // NB: (cat fat) !~ cat, 
    //   => {((cat fat) (like fish)) (cat (chase mouse))} [m0]
    
    // L ⊢ a, atom. if (first L) ~ a, then L replaces a. [m1]
    // eg (chase mouse) ⊢ chase => (chase mouse). [m1]
    // 

    // a ⊢ b, atoms, a != b (ie a !~ b) => { a b } [m0]
    // eg (chase mouse) ⊢ (chase bird) = (chase {mouse bird}) [m1]

    // S1 ⊢ S2. ∀ x ∈ S1, x ⊢ S2.
    // x ⊢ Set (x not a set). find closest match. ie best `m'.
    // ∀ y ∈ Set, 

    int r = a._type - b._type;
    if (r)
    {
        // terms are different types
        if (b._type == type_tree)
        {
            //  x ⊢ Set (x not set)

            if (a._type == type_list)
            {
                // List ⊢ Set.
                List::iterator ai((const List&)a);
                if (ai)
                {
                    // suppose a = (foo bar), want to match on (foo)
                    // to find the earliest head match.
                    
                    r = _entSet(Match(List(*ai, 1)), a, b);

                    if (r && entails() && !erase())
                    {
                        // handle case (foo bar) ⊢ {(dog x) foo}
                        r = _entSet(Match(*ai), a, b);
                    }
                }
            }
            else
            {
                //  a ⊢ Set, a non-list non-set. a atomic.
                r = _entSet(Match(a), a, b);

                if (r)
                {
                    // handle case, cat ⊢ {(cat fat) dog}
                    List c(a, 1);
                    r = _entSet(Match(c), c, b);
                }
            }

            if (r && entails() && !erase())
            {
                // did not otherwise entail. default is to add to set.

                Tree::iterator ti((const Tree&)b);
                ti.toSup(); // make invalid (sort of).
                
                LValue lv(ti);
                lv = a;
                r = 0;
             }
        }
        else if (b._type == type_list)
        {
            //  x ⊢ List

            if (a._type == type_tree)
            {
                r = _entFromSet(a, b);
            }
            else
            {
                // a atom.
                //  check a ~ List, if a ~ (first List)

                List::iterator it((const List&)b);
                if (it)
                {
                    Entails e;
                    push(e);
                    r = _ent(a, *it);
                    pop();
                }
            }
        }
        else 
        {
            // x ⊢ b, b atom
            if (a._type == type_list)
            {
                // List ⊢ b
                List::iterator it((const List&)a);
                if (it)
                {
                    Entails e;
                    push(e);
                    r = _ent(*it, b); // if *it ~ b, then *it is atom.
                    pop();
                    
                    if (!r && entails() && !erase()) set(a);
                }
            }
            else if (a._type == type_tree)
            {
                // Set ⊢ b. b atom
                r = _entFromSet(a, b);
            }
        }
    }
    else
    {
        // otherwise types agree
        switch (a._type)
        {
        case type_list:
            // L1 ⊢ L2
            r = _entLists(a, b);

            // `a` overwrites whole list when overwriting
            if (!r && entails())
            {
                if (_entails->_overwriteMatch) set(a);
                else if (erase())
                {
                    List::iterator i((List&)b);
                    if (i)
                    {
                        ++i; // skip first element
                        
                        // empty rest of list
                        while (i.erase()) ; 
                    }
                }
            }
            
            break;
        case type_tree:
            {
                // S1 ⊢ S2
                Tree::iterator ia((const Tree&)a);
                r = 0;
                for (ia.toMin(); ia; ++ia)
                {
                    r = _ent(*ia, b);
                    if (r) break; 
                }
            }
            break;
        default:
            {
                // a ⊢ b, atoms
                ComparatorEq co;
                r = co(a, b);
            }
            break;
        }
    }

    if (r && entails() && !erase())
    {
        Tree tr;
        Tree::iterator it(tr);
        LValue lv(it);

        lv = a;

        it.toSup(); // force insert
        lv = b;
        
        set(tr);
        r = 0;
    }
    
    return r;
}
