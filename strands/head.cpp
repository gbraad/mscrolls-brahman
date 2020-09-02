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


#include "term.h"
#include "intt.h"
#include "stringt.h"
#include "symbol.h"
#include "group.h"
#include "primitive.h"
#include "thunk.h"
#include "number.h"
#include "streamt.h"
#include "blob.h"

uint Head::_count;
int Head::_showRefs;

// Head functions are in place of virtual methods

bool Head::isEmptyList() const
{
    // or set
    bool r = false;
    if (_type == type_list || _type == type_tree)
        r = ((List*)this)->isEmpty();
    
    return r;
}

void Head::_toSBuf(int type, SBuf& sb) const
{
    switch (type)
    {
    case type_list:
        ((const List*)this)->toSBuf(sb);
        break;
    case type_int:
        ((const Int*)this)->toSBuf(sb);
        break;
    case type_number:
        ((const Number*)this)->toSBuf(sb);
        break;
    case type_string:
        ((const Stringt*)this)->toSBuf(sb);
        break;
    case type_symbol:
        ((const Symbol*)this)->toSBuf(sb);        
        break;
    case type_tree:
        ((const Tree*)this)->toSBuf(sb);
        break;
    case type_stream:
        ((const Streamt*)this)->toSBuf(sb);
        break;
    case type_primitive:
        ((const Primitive*)this)->toSBuf(sb);
        break;
    case type_function:
    case type_macro:
        {
            sb.add("<< ");
            _toSBuf(_fnBodyType, sb);
            sb.add(" >>");
        }
        break;
    case type_thunk:
        ((const Thunk*)this)->toSBuf(sb);        
        break;
    case type_blob:
        ((const Blob*)this)->toSBuf(sb);        
        break;
    default:
        sb.add("NULL");
    }
}

void Head::toSBuf(SBuf& sb) const
{
    if (marked()) 
    {
        sb.add("...");
        return;
    }

#ifdef DEBUGOUT
    if (Logged::_logLevel >= 5 || _showRefs)
    {
        if (_refCount > 0)
        {
            if (owned()) sb.add('^'); // indicate owned
            else sb.add('#'); // indicate referenced headtmp
            sb.addLongString((long)_refCount);
        }
    }
#endif
    
    setMark();
    _toSBuf(_type, sb);
    clearMark();
}

const Head& Head::orderTerm() const
{
    if (isList())
    {
        List::iterator it((const List&)*this);
        if (it)
            return it->orderTerm();
    }
    if (isTree())
    {
        Tree::iterator it((const Tree&)*this);
        it.toMax();
        if (it)
            return it->orderTerm();
    }
    return *this;
}

const Head& Head::primaryTerm() const
{
    // return first element of a list or least element of a set
    // or otherwise itself.
    if (isList())
    {
        List::iterator it((const List&)*this);
        if (it)
            return it->primaryTerm();
    }
    if (isTree())
    {
        Tree::iterator it((const Tree&)*this);
        it.toMin();
        if (it)
            return it->primaryTerm();
    }
    return *this;
}

int Head::compare(const Head& h1, const Head& h2, Comparator& co)
{

    // '(a {a b} b (b) {b} c}
    // a < {a b} < b < (b) < {b} < c

    // '{a (a a) (a b) (a c) {a b} b (b a)}
    // a < (a a) < (a b) < (a c) < b < (b a) < {a b}
    
    // (a b) < (a (b)) < (a c)
    
    
    const Head& p1 = h1.orderTerm();
    const Head& p2 = h2.orderTerm();

    int d = _compare(p1, p2, co);

    // if same as primary, then we're done
    if (!d && (&p1 != &h1 || &p2 != &h2))
    {
        d = _compare(h1, h2, co);
    }
    return d;
}

int Head::_compare(const Head& h1, const Head& h2, Comparator& co)
{
    int t1 = h1._type;
    int t2 = h2._type;

    // map variable symbols onto their own type for comparison
    if (t1 == type_symbol && ((const Symbol&)h1).isVariable())
        t1 = type_variable;

    if (t2 == type_symbol && ((const Symbol&)h2).isVariable())
        t2 = type_variable;

    // order by reverse type
    int dt = t2 - t1;
    if (dt) return dt;

    int v = 0;
    switch (h1._type)
    {
    case type_list:
        v = ((const List&)h1).compare((const List&)h2, co);
       break;
    case type_int:
        v = ((const Int&)h1).compare((const Int &)h2);
        break;
    case type_number:
        v = ((const Number&)h1).compare((const Number&)h2);
        break;
    case type_string:
        v = ((const Stringt&)h1).compare((const Stringt&)h2);
        break;
    case type_symbol:
    case type_variable:
        // XX: should this just compare pointers?
        v = ((const Symbol&)h1).compare((const Symbol&)h2);        
        break;
    case type_tree:
        v = ((const Tree&)h1).compare((const Tree&)h2, co);        
        break;
    case type_stream:
        v = ((const Streamt&)h1).compare((const Streamt&)h2);        
        break;
    case type_primitive:
        v = ((const Primitive&)h1).compare((const Primitive&)h2);        
        break;
    case type_function:
    case type_macro:
        // only the same if functions identical
        v = (int)(&h1 - &h2);
        break;
    case type_thunk:
        // any thunks should have been evaluated 
        assert(false);
        break;
    case type_blob:
        v = ((const Blob&)h1).compare((const Blob&)h2);        
        break;
    case type_lref:
        v = 0;
        break;
    }
    return v;
}

void Head::_destroy(int type)
{
    // only valid for objects on the heap
    switch (type)
    {
    case type_list:
        delete (List*)this;
        break;
    case type_int:
        delete (Int*)this;
        break;
    case type_number:
        delete (Number*)this;
        break;
    case type_string:
        delete (Stringt*)this;
        break;
    case type_symbol:
    case type_variable:
        delete (Symbol*)this;
        break;
    case type_tree:
        delete (Tree*)this;
        break;
    case type_stream:
        delete (Streamt*)this;
        break;
    case type_primitive:
        delete (Primitive*)this;
        break;
    case type_function:
    case type_macro:
        _destroy(_fnBodyType);
        break;
    case type_thunk:
        delete (Thunk*)this;
        break;
    case type_blob:
        delete (Blob*)this;
        break;
    case type_lref:
        assert(false);
        break;
    }
}

void Head::fold(const Group& g, SBuf& sb) const
{
    // fold the type
    sb.writeShort(_type);
    sb.writeShort(_flags);
    
    switch (_type)
    {
    case type_list:
        ((const List*)this)->fold(g, sb);
        break;
    case type_int:
        ((const Int*)this)->fold(sb);
        break;
    case type_number:
        ((const Number*)this)->fold(sb);
        break;
    case type_string:
        ((const Stringt*)this)->fold(sb);
        break;
    case type_symbol:
        ((const Symbol*)this)->fold(sb);
        break;
    case type_tree:
        ((const Tree*)this)->fold(g, sb);
        break;
    case type_stream:
    case type_primitive:
    case type_function:
    case type_macro:
    case type_thunk:
    case type_blob:
        // XX to do
        assert(false);
        break;
    case type_lref:
        ((const LRef*)this)->fold(sb);
        break;
    }    
}

void Head::unfold(Group& g, SBuf& sb)
{
    // unfold the type
    _type = sb.readShort();
    _flags = sb.readShort();
    _refCount = 0;
    _n = 0;
    
    switch (_type)
    {
    case type_list:
        ((List*)this)->unfold(g, sb);
        break;
    case type_int:
        ((Int*)this)->unfold(sb);
        break;
    case type_number:
        ((Number*)this)->unfold(sb);
        break;
    case type_string:
        ((Stringt*)this)->unfold(sb);
        break;
    case type_symbol:
        ((Symbol*)this)->unfold(sb);
        break;
    case type_tree:
        ((Tree*)this)->unfold(g, sb);
        break;
    case type_stream:
    case type_primitive:
    case type_function:
    case type_macro:
    case type_thunk:
    case type_blob:
        // XXX to do
        assert(false);
        break;
    case type_lref:
        ((LRef*)this)->unfold(sb);
        break;
    }    
}

void Head::_useAux(int depth)
{
    // if !owned && refcount == 0, we are morphology, so recurse
    // if !owned && refcount > 0, we are a temp ref, so use
    // if owned, then simply recurse to find any inside
    if (!owned() && _refCount)
    {
#ifdef DEBUGOUT
        if (depth > 0)
        {
            // not sure this happens??
            std::cerr << "useAux, depth " << depth << std::endl;
        }
#endif 
        ((HeadTmp*)this)->use();
    }
    
#ifdef DEBUGOUT
    bool alist = (_type == type_list || _type == type_tree);
    if (!alist)
    {
        if (_type != type_macro || _type != type_function)
        {
            alist = _fnBodyType == type_list;
        }
    }

    if (alist)
    {
        for (List::iterator it((List&)*this); it; ++it)
            (*it)._useAux(depth + 1);
    }
#endif
}

Term parseTerm(Stream& s, ParseContext* ctx)
{
    while (u_isspace(*s)) ++s;
    Term t = List::parse(s, ctx);

    /* here we create functions from (fn () ...) at parse time.
     * We don't actually need to do this, since they will be created
     * once the primitive `fn` is eval.
     *
     * BUT, we would need to fix the term (fn (a) body)) so that 
     * <body> replace (fn...). This could be done (probably should)
     * However, creating at parse time is a good idea to save time later
     */
    
    if (t && ctx && ctx->sympool())
    {
        List::iterator it(t.asList());
        if (it)
        {
            // XX TODO: "def" as well?
            
            if (it->isSymbol())
            {
                const char* symname = ((Symbol&)*it).label();
                assert(symname);

                if (!strcmp(symname, "fn"))
                {
                    ++it;
                    Term tf = Lambda::functionDef(it,
                                                  Head::type_function,
                                                  ctx->sympool());
                    
                    // if it worked, replace original term, otherwise leave it.
                    if (tf) t = tf;
                }
                else if (!strcmp(symname, "EV"))
                {
                    // eval during parse
                    ++it;
                    if (it)
                    {
                        t = ctx->eval(*it);
                    }
                }
            }
        }
    }
    
    if (!t) t = Tree::parse(s, ctx);
    if (!t) t = Stringt::parse(s);
    if (!t) t = Int::parse(s);
    if (!t) t = Number::parse(s);
    if (!t)
    {
        t = Symbol::parse(s, ctx);
        if (t)
        {
            // handle quote abbreviation: '(1 2) etc.
            if (!strcmp(t.asSymbol().label(), "'"))
            {
                // single quote symbol
                Term u = parseTerm(s, ctx);
                if (u)
                    t = List(*t, u.demote());
            }
        }
    }
    return t;
}

