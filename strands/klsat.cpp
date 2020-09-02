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


#include <set>
#include "kllib.h"
#include "sat.h"

struct KLSat: public KLLib
{
    typedef KLSat Self;
    typedef Term (Self::*PrimFn)(List::iterator&, Env&);
    typedef PCallFn<PrimFn, Self> PC;

    KLSat(KL* host) : KLLib(host) { _init(); }
    const char* libname() const override { return "klsat"; }
    void destroy(Blob* b) override { delete b; }
    string asString(const Blob* b) const override { return "blob"; }

    Term _primSatFn(List::iterator& ai, Env& env)
    {
        // Satisfaction operator, (~ term KB)
        // finds substitutions that make variables within `term' match KB.
        
        // eg (~ '(cat _x) kb)
        // eg (~ '(_x {(be mammal)}) kb)
        // kb is either a tree of terms of a single term (must be a list)
        
        if (ai)
        {
            // term to match
            Term mt = EVALAI;
            if (ai)
            {
                Term kb = EVALAI;
                ComparatorSat co;
                ComparatorSat::SubsTree st;
                co.push(st);
                int r = co(*mt, *kb);
                if (!r) 
                {
                    co.subsOK(st);

                    // if satisfied but no subst, return 1 to indicate true
                    if (co._results.isEmpty())
                        return Int(1);
                }
                return co._results;
            }
        }
        return Term();
    }

    Term _primEntFn(List::iterator& ai, Env& env)
    {
        // entail operator;
        // (|- term KB)
        // (|- term KB mode)
        // establish `term' in KB.
        //
        // mode = 0 (or missing) => add
        // mode = 1 => overwrite/set
        // mode = -1 => delete
        //
        // eg. we have (cat {(be mammal) (chase mouse)}) ~ KB
        // then we say, (|- (cat {(like fish) (chase mouse)}) KB)
        // we get, (cat {(be mammal) (chase mouse) (like fish)}) ~ KB

        // NOTE: (|- () kb) makes no change.

        // see also satisfaction operator "~".

        // XXX TODO:
        // potentially serious problem with changing elements of set
        // as this changes the `orderTerm` which will change the ordering
        // of any containing set!

        Term kb;
        if (ai)
        {
            // term to establish
            Term t = EVALAI;
            if (ai && !KL::isEmptyList(t))
            {
                kb = EVALAI;

                int mode = 0; // add
                bool ok = true;
                if (ai)
                {
                    // optional mode specifier
                    Term m = EVALAI;
                    ok = KL::expect(m, type_int, "ent mode");
                    if (ok)
                        mode = m.asInt().value();
                }

                if (ok)
                {
                    LValue res;
                    ComparatorEnt::Entails e(res, mode);
                    ComparatorEnt co(e);
                    co(*t, *kb);
                    if (res._value) kb = res._value;
                }
            }
        }
        return kb;
    }
    
    void _init()
    {
        Tree g;

        // data
        DEF_PRIM(Sat, "~");     // satisfy
        DEF_PRIM(Ent, "|-");    // entail

        _host->_env._env = List(g, *_host->_env._env);

    }

};

void initKLSat(KL* host)
{
    LOG3("KLSat", "init");
    static KLSat klsat(host);
}

