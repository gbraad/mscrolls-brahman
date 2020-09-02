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
#include "tree.h"
#include "parsectx.h"
#include "dl.h"


#define LABEL(__n) ((const char*)(__n))

struct Symbol: public Head
{
    Symbol(const char* s) : Head(type_symbol) { _init(s, strlen(s)); }
    Symbol(const char* s, size_t n) : Head(type_symbol) { _init(s, n); }
    
    ~Symbol()
    {
        if (owned())
        {
            DBIF(_n, "~SYM " << *this);
            _purge(); 
        }
    }

    const char* label() const { return LABEL(_n); }

    bool isVariable() const
    {
        // determine from the name
        // NB: symbols not zero size
        return *LABEL(_n) == '_';
    }
    
    bool isArg() const
    {
        return (isVariable() && u_isdigit(LABEL(_n)[1]));
    }

    int argValue() const
    {
        // ASSUME we are arg symbol
        return atoi(LABEL(_n) + 1);
    }
    
    bool isAnonVariable() const
    {
        // must be "_" exactly
        return isVariable() && LABEL(_n)[1] == 0;
    }

    // convert to sbuf, for string output
    void toSBuf(SBuf& sb) const
    {
        sb.add(LABEL(_n));
    }
    
    int compare(const Symbol& v) const
    {
        // symbols are case insensitive.
        
        const char* a = LABEL(_n);
        const char* b = LABEL(v._n);

        // symbols with identical node must be the same
        if (a == b)
            return 0;

        bool bvar = (*b == '_');

        // arrange variables to be ordered before normal symbols.
        if (*a == '_')
        {
            if (!bvar) return -1;  // var < non-var
        }
        else
            if (bvar) return 1; // non-var > var
        
        return u_stricmp(a, b);
    }
    
    void fold(SBuf& sb) const
    {
        size_t sz = strlen(LABEL(_n));
        sb.writeInt32(sz);
        sb.write(_n, sz);
    }

    void unfold(SBuf& sb) 
    {
        size_t sz = sb.readInt32(); 
        _n = (Node*)new char[sz + 1];
        sb.read(_n, sz);
        ((char*)_n)[sz] = 0;
    }

    Term intern(ParseContext* ctx)
    {
        Tree::iterator it(ctx->symbols().asTree());
        it.insert(*this);

        // if already present, parked on match
        // otherwise, inserted and parked on new element
        Head& h = *it;
        assert(h.isSymbol() && h._refCount > 0);
        return h;
    }

    Term find(ParseContext* ctx)
    {
        Term t;
        Tree::iterator it(ctx->symbols().asTree());
        if (it.find(*this))
        {
            // if already present, parked on match
            Head& h = *it;
            assert(h.isSymbol() && h._refCount > 0);
            t = h;
        }
        return t;
    }
    
    static Term parse(Stream& s, ParseContext* ctx)
    {
        // when a parse context is supplied, we intern symbols 
        
        // collect allowed characters as the symbol
        SBuf sb;

        int c = *s;
        if (c)
        {
            // handle special cases
            if (c == '\'')
            {
                // quote symbol is always treated as a single char symbol
                sb.add(c);
                ++s;
            }
            else if (!u_isdigit(c)) // can't start with a digit
            {
                do 
                {
                    if (notAllowed(c)) break;
                    sb.add(c);
                    ++s;
                } while ((c = *s) != 0);
            }
        }

        Term t;
        if (sb.size())
        {
            t = Symbol(sb, sb.size());
            if (ctx)
                t = t.asSymbol().intern(ctx);
        }
        return t;
    }

    static bool notAllowed(char c)
    {
        // characters that aren't allowed in symbol names
        static const char* non = "\\\"(){}";
        return u_isspace(c) || u_strchr(non, c) != 0;
    }

private:
    
    void _init(const char* s, size_t n) 
    {
        assert(n);
        char* p = new char[n+1];
        memcpy(p, s, n);
        p[n] = 0;
        _n = (Node*)p;
    }

    void _purge()
    {
        delete (char*)_n;
        _n = 0;
    }
};

struct SymPool
{
    // pool for internal symbols
    
    struct Holder: public DL<Holder>
    {
        Holder(const Symbol& s) : _sym(s) {}
        Term    _sym;
    };

    typedef Holder::iterator    iterator;
    
    Holder::List        _pool;

    ~SymPool()
    {
        while (!_pool.empty())
            _pool.first()->removeAndDelete();
    }

    iterator first()
    {
        if (_pool.empty())
        {
            // make the first symbol
            _pool.add(new Holder(Symbol("_0")));
        }
        return _pool.begin();
    }

    iterator next(iterator& it)
    {
        assert(it != _pool.end());
        iterator ij = it; ++ij;
        if (ij == _pool.end())
        {
            // extend
            int v = it->_sym.asSymbol().argValue();
            char buf[16];
            sprintf(buf, "_%d", v + 1);
            _pool.add(new Holder(Symbol(buf)));

            ij = _pool.end();
            
            // park on last
            --ij;
        }
        return ij;
    }
   
};

struct Lambda
{
    static int _argSubAux(Head& h, const Head& target, const Head& sub)
    {
        int n = 0;
        if (h.isList())
        {
            ComparatorEq co;
            for (List::iterator it((List&)h); it; ++it)
            {
                Head& hi = *it;
                if (!co(hi, target))
                {
                    it.replace(sub);
                    ++n;
                }
                else
                    n += _argSubAux(hi, target, sub);
            }
        }
        return n;
    }

    static Term functionDef(List::iterator& ai, int type, SymPool* sympool)
    {
        // (fn vars-list body)
        // (fn var body) ;; var is assigned all args as a list
        // (fn () body)

        assert(sympool);

        if (ai)
        {
            // vars is either a list of vars or a single var
            // that will be bound to the list of args
            Term vars(*ai); ++ai;
            if (ai)
            {
                Term body(*ai); 

                // XX actually, it can be a single term!
                if (!body.isList())
                {
                    LOG2("Warning, Expected list for body, ", body);
                }

                // first pool entry "_0" represents all args
                SymPool::iterator si = sympool->first();
                
                bool ok = false;
                if (vars.isList())
                {
                    ok = true;
                    for (List::iterator vi(vars.asList()); vi; ++vi)
                    {
                        ok = vi->type() == Head::type_symbol;
                        if (!ok) break;
                        
                        si = sympool->next(si);
                        _argSubAux(*body, *vi, *si->_sym);
                    }
                }
                else if (vars.isSymbol())
                {
                    ok = true;
                    
                    // (def args body) 
                    // where `args' is a symbol represents the whole arg list
                    _argSubAux(*body, *vars, *si->_sym);
                }

                if (ok)
                {
                    // we're keeping the body!
                    Head& fn = body->use();

                    // mark as function or macro
                    fn._fnBodyType = body.type();
                    fn._type = type;
                    
                    return fn;
                }
                else
                {
                    LOG1("Bad function args ", vars);
                }
            }
        }
        return Term();
    }

};
    


