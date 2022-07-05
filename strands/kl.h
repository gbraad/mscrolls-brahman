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

#include "sstream.h"
#include "list.h"
#include "symbol.h"
#include "intt.h"
#include "stringt.h"
#include "primitive.h"
#include "thunk.h"
#include "number.h"
#include "streamt.h"
#include "jsonwalker.h"
#include "growbuf.h"
#include "rand.h"
#include "fdz.h"
#include "strutils.h"
#include "nran.h"

#define ADD_PRIM(_name, _sym)   \
       g.insert(List(Symbol(_sym), Primitive(_prim ## _name)))

#define DEF_PRIM(_name, _sym)   \
    static PC _prim ## _name(&Self::_prim ## _name ## Fn, this);  \
    ADD_PRIM(_name, _sym)

#define TYPES(_t, _tbase) (((_t) << 8) | (_tbase))
#define TYPES_LIST      TYPES(type_tree, type_list)

#define EVALOPT_DEFEREVALARGS   1
#define EVALOPT_NOEVALARGS      2 
#define EVALOPT_NOBINDERROR     4
#define EVALOPT_EVALFUNCTIONS   8

#define EVALAI primNoEval() ? Term(*ai) : eval(Term(*ai), env); ++ai

struct LValue
{
    List::iterator*     _li;
    Tree::iterator*     _ti;
    Term                _value;

    LValue() { _li = 0; _ti = 0; }
    LValue(List::iterator& it) : _li(&it) { _ti = 0; }
    LValue(Tree::iterator& it) : _ti(&it) { _li = 0; }
    
    void operator=(const Head& val) { _bind(val); }

    void _bind(const Head& h)
    {
        // prepare object to store
        if (_li || _ti)
        {
            const Head& hu = h.prepuse();

            if (_li)
            {
                if (_li->valid())
                {
                    // replace element at current location
                    assert(!(*_li)->identical(h));
                    _li->replace(hu);
                }
                else
                {
                    assert(_li->_l);

                    // be sure the hosting list can be used.
                    _li->_l->use();
                
                    // otherwise add to list
                    _li->insert(hu);
                }
            }
            else // _ti
            {
                assert(_ti->_l);

                // ensure hosting tree can be used
                _ti->_l->use();

                bool done = false;
                
                if (_ti->valid() && !_ti->onSup())
                {
                    done = (*_ti)->identical(h);

                    if (done)
                    {
                        LOG3("KL warning", " bind to identical");
                    }

                    if (!done)
                    {
                        // erase the current element
                        _ti->eraseHere();
                    }
                }

                if (!done)
                {
                    bool v = _ti->insert(hu);
                    (void)v; // no warning
                    assert(v);
                }
            }
        }
        else
        {
            // just keep it here
            _value = Term(h);
        }
    }
};

struct KL: public HeadTraits
{
    typedef std::string string;
    typedef KL Self;
    typedef Term (Self::*PrimFn)(List::iterator&, Env&);
    typedef PCallFn<PrimFn, Self> PC;

    Stream*     _in = 0;
    StdStream*  _out = 0;
    Term        _symbols;
    Env         _env;
    SymPool     _symPool;

    int         _break = 0;
    bool        _primNoEval;

    // optional path prefix to load file
    string      _loadFilePrefix;

    // random numbers
    Ranq1       _ran;
    NRan        _nran;

    static const int breakBreak = 1;
    static const int breakReturn = 2;

    KL() : _symbols(Tree()), _nran(&_ran)
    {
        _init();

        // reserve symbols
        ParseContext ctx(this);
        Symbol("EV").intern(&ctx);

#ifdef USE_KLSAT
        extern void initKLSat(KL* host);
        initKLSat(this);
#endif
    }

    Stream* setInput(Stream* s)
    {
        Stream* olds = _in;
        _in = s;
        return olds;
    }
    
    StdStream* setOutput(StdStream* s)
    {
        if (_out) _out->flush();
        StdStream* old = _out;
        _out = s;
        return old;
    }

    bool        primNoEval() const { return _primNoEval; }

    string      where() const
    {
        // if we have input stream, describe it
        string s;
        if (_in)
        {
            s += " at ";
            s += _in->name();
            if (_in->_line)
            {
                s += " line ";
                s += std::to_string(_in->_line);
            }
        }
        return s;
    }

    bool loadString(const char* s, Env& env)
    {
        StreamString ss;
        ss._semiColonComments = true;
        ss.open(s);
        return loadStream(ss, env);
    }

    bool loadFile(const string& name, Env& env, int errlevel = 1)
    {
        StreamFile s;
        s._semiColonComments = true;

        string fpath = makePath(_loadFilePrefix, name);

        // first try exact name as file
        if (!FD::existsFile(fpath))
        {
            // otherwise try for binary
            fpath = changeSuffix(fpath, ".klb");
            if (!FD::existsFile(fpath))
                fpath = changeSuffix(fpath, ".kl");                        
        }
        
        bool r = s.open(fpath);
        if (r)
        {
            if (*s > 0x7f)
            {
                // first byte is binary. assume compressed file
                s._fb.ungetc();

                uchar* data = readz(s._fb);
                if (data)
                {
                    r = loadString((const char*)data, env);
                    delete [] data;
                }
                else
                {
                    LOG1("Could not read binary file ", name);
                    r = false;
                }
            }
            else
            {
                r = loadStream(s, env);
            }
        }

        if (r)
        {
            LOG1("Loaded file '", fpath << '\'');
        }
        else 
        {
            LOGN(errlevel, "Failed to load file '", name << '\'');
        }

        return r;
    }

    bool loadStream(Stream& s, Env& env)
    {
        bool r = true;

        Stream* oldInput = setInput(&s);

        ParseContext pctx(this);
        for (;;)
        {
            Term t = parseTerm(s, &pctx);
            if (!t) break;
            eval(t, env);
        }

        if (pctx._missingCloseLine)
        {
            LOG1("warning, missing close bracket in '",
                 s.name() << "' opened at line " << pctx._missingCloseLine);
            r = false;
        }
        else while (*s)
        {
            int c = *s;
            if (c == '(' || c == '{' || c == ')' || c == '}')
            {
                LOG1("warning unexpected content '",
                     (char)c << "' in '" << s.name() << "' line " << s._line);
                r = false;
                break;
            }
            ++s;
        }

        setInput(oldInput);
        
        return r;
    }

    Term _envFindPair(Symbol& sym, const Head& vl, bool deep)
    {
        // (sym value) -> value
        
        assert(vl.isList());

        Term tr;

        List::iterator vi((List&)vl);
        if (vi)
        {
            const Head& var = deep ? vi->primaryTerm() : *vi;
            if (var.isSymbol())
            {
                if (!sym.compare((const Symbol&)var))
                {
                    ++vi;
                    if (vi) tr = *vi;
                    else
                    {
                        // if (sym), block further matches
                        tr = List();
                    }
                }
            }
        }
        return tr;
    }
    
    Term _envFindTree(Symbol& sym, const Head& h)
    {
        // {(a b) (sym value)} -> value
        
        assert(h.isTree());

        Term tr;

        // search this environment
        Tree::iterator it((const Tree&)h);

        // locate '(sym ?) lower bound
        it.find(Match(List(sym, 0)));

        if (it)
        {
            Head& vl = *it;            

            // should we also support trees in env?
            if (vl.isTree()) LOG1("env tree? ", vl);
                
            if (vl.isList())
                tr = _envFindPair(sym, vl, true);
        }
        return tr;
    }
    
    
    Term envFind(Symbol& sym, const Head& h)
    {
        // find value for `sym` in env `h`
        // ({} {} {(a b) (sym value)})  -> value
        // ((a b) {} (sym value)) -> value
        
        Term tr;

        assert(sym.isSymbol());
        if (h.isTree())
        {
            tr = _envFindTree(sym, h);
        }
        else if (h.isList())
        {
            // search chain of environments
            // '( {} {} {} ...)
            for (List::iterator it((List&)h); it; ++it)
            {
                const Head& hi = *it;
                if (hi.isTree())
                    tr = _envFindTree(sym, hi);
                else if (hi.isList())
                {
                    tr = _envFindPair(sym, hi, false);
                    if (tr) break;
                    tr = envFind(sym, hi);
                }

                if (tr) break;
            }
        }
        return tr;
    }

    void        _bindvar(Term& var, const Head* val,
                         Env& env, bool local, bool temp)
    {
        // val == 0 to unbind
        
        const Head* h = val;

        // temps can only be local
        // temp => local
        assert(local || !temp);

        if (h && !temp)
        {
            // here we have an optimisation
            // when we're binding locally, we can sometimes assume the object
            // referenced by the binding is valid for the iteration
            // (ie a temp)
            //
            // eg. (forall 'x '((foo1 1) (foo2 2) (foo3 3)) (print x))
            // 'x will be bound to each '(foo..) but we don't want to
            // "use" those references and make them separately refcounted.
            // UNLESS we're going to keep `x around for later.
            //
            // `forall will unbind `x after the iteration
            h = &h->prepuse();
        }

        bool v;
        if (local)
            v = _bindLocal((const Symbol&)*var, h, *env._env);
        else
            v = _bindList((const Symbol&)*var, h, *env._env, true);
        
        if (val && !v)
        {
            LOG1("bind var failed, ", var << " to " << val);
        }
    }

    bool        _bindLocal(const Symbol& sym, const Head* val,
                           const Head& env)
    {
        // always bind into the most local context
        if (env.isList())
        {
            List& el = (List&)env;
            List::iterator eli(el);
            if (!eli) return false;  // empty
            
            return _bindTree(sym, val, *eli, true);            
        }
        else
        {
            return _bindTree(sym, val, env, true);
        }
    }
    
    bool        _bindList(const Symbol& sym, const Head* val, 
                          const Head& env, bool force)
    {
        // or unbind if val==0
        // env = ({...} ..) or {}

        bool done = false;
        if (env.isList())
        {
            // first check for existing rebinding
            List::iterator eli((const List&)env);
            while (eli)
            {
                done = _bindList(sym, val, *eli, false);
                if (done) break;
                ++eli;
            }
            
            // otherwise add to leaf
            if (!done && force)
            {
                eli.reset();
                if (eli)
                {
                    done = _bindList(sym, val, *eli, force);
                }
            }
        }
        else if (env.isTree())
        {
            done = _bindTree(sym, val, env, force);
        }
        return done;
    }
    
    bool        _bindTree(const Symbol& sym, const Head* val,
                          const Head& env, bool force)
    {
        // or unbind if val==0
        // env = {}

        assert(env.isTree());

        Tree& ct = (Tree&)env;
        
        bool done = false;
        Tree::iterator it(ct);

        bool ok = it.valid();
        if (ok)
        {
            // when we locate, we expect not to be found because we search
            // for '(var), which isn't a pair. This will be below the actual
            // match. Thus locate will be parked on a tree leaf, at the sup
            // of this position. this will be either an existing copy of
            // the variable or the next variable - or the SUP of the node.
            it.locate(Match(List(sym, 0)));

            Tree::iterator ij;
            const Head* vl;

            ok = !it.onSup();
            if (ok)
            {
                // we are on a valid entry to investigate variable
                vl = &*it;
            }
            else
            {
                // make a copy of iterator because the original is on the SUP
                // leave `it` as position to insert if we need to.
                ij = it;

                // bump from SUP to valid (unless > all values). 
                ij.revalidate();

                // if there's another entry, it might be our match or
                // the next entry
                ok = ij.valid();
                if (ok)
                    vl = &*ij;
            }

            if (ok)
            {
                assert(vl->isList());
                List::iterator vi((List&)*vl);
                assert(vi.valid());
                Head& var = *vi;
                assert(var.isSymbol());
                if (!sym.compare((const Symbol&)var))
                {
                    if (val)
                    {
                        ++vi;
                        assert(vi.valid());

                        // replace with new value
                        vi.replace(*val);
                    }
                    else
                    {
                        // unbind by erasing old (sym val) pair
                        if (it.onSup())
                        {
                            // ij was location of match
                            ij.eraseHere();
                        }
                        else
                        {
                            it.eraseHere();
                        }
                    }
                    done = true;
                }
            }
        }
            
        if (val && !done && force)
        {
            // if we didn't insert before, we're still parked at the 
            // right place, after locating the sup.
            it.insertHere(List(sym, *val));
            done = true;
        }
        return done;
    }


    Term        evalFunction(Term& fn, List::iterator it, Env& env, int opt)
    {
        // if we have a function built from an atom, just return it.
        if (fn->_fnBodyType != type_list) return fn;
        
        // build a list of args for the function call
        EnvArgs args(it.size());
        List::iterator ai(args._args);

        bool deferarg = opt & EVALOPT_DEFEREVALARGS;
        bool noeval = opt & EVALOPT_NOEVALARGS;
        
        // defer only this evaluation
        opt &= ~(EVALOPT_DEFEREVALARGS | EVALOPT_NOEVALARGS);

        while (it)
        {
            Term ti(*it);

            if (!noeval)
            {
                if (!deferarg)
                    ti = eval(ti, env, opt);
                else
                {
                    // replace by a thunk? 
                    // create thunks for types that eval.
                    if (ti.isList() || ti.isSymbol())
                        ti = Thunk(ti, env);
                }
            }
                
            ai.insert(*ti);
            ++it;
        }
        
        env.push(&args);
        Term r = evalList(fn.asList(), env, opt);
        env.pop();
        return r;
    }
    
    Term        fetchArg(Symbol& sy, Env& env, int opt)
    {
        // ASSUME arg
        
        Term r;

        bool evalarg = true;
        if (opt & EVALOPT_NOEVALARGS)
        {
            evalarg = false;
        }
        
        int a = sy.argValue();
        if (a > 0)
        {
            assert(env._args);
            List::iterator ai(env._args->_args);
            ai.skip(a-1);
            if (ai.valid())
            {
                r = Term(*ai);
                if (evalarg)
                {
                    if (r.isThunk())
                        r = evalThunk(r.asThunk(), env, opt);
                }
                else
                {
                    if (r.isThunk())
                    {
                        Thunk::Info* i = r.asThunk().info();
                        r = i->_expr;

                        while (r.isSymbol())
                        {
                            Symbol& sy = r.asSymbol();
                            if (sy.isArg())
                            {
                                Env e(env._env, i->_args);
                                r = fetchArg(sy, e, opt | EVALOPT_NOEVALARGS);
                            }
                            else break;
                        }

                        assert(!r.isThunk());
                    }
                }
            }
            else
            {
                // insufficient args
                bool nobinderr = opt & EVALOPT_NOBINDERROR;
                if (!nobinderr)
                    LOG1("Error, insufficient args at ", a);
                r = List();
            }
               
        }
        else
        {
            // index of 0, represents whole list
            // do not eval this

            if (env._args->_argsTerm)
                r = env._args->_argsTerm;
            else
            {
                // promote args list to term
                // NOTE: that after this `args` will be empty
                // functions use either _0 or _1,_2 etc and so will not mix
                r = env._args->_argsTerm = env._args->_args;
            }
        }

        assert(r);
        return r;
    }


    Term        evalSymbol(Symbol& sy, Env& env, int opt)
    {
        Term r;
        if (sy.isArg())
        {
            // can't no eval symbol
            assert((opt & EVALOPT_NOEVALARGS) == 0);
            r = fetchArg(sy, env, opt);
        }
        else
        {
            r = envFind(sy, *env._env);

            if (!r)
            {
                bool nobinderr = opt & EVALOPT_NOBINDERROR;
                if (!nobinderr)
                {
                    LOG1("Error '", sy << "' undefined" << where());
                }
                r = Term(List());
            }
        }
        return r;
    }

    Term        evalTail(Term& t, List::iterator& it, Env& env, int opt)
    {
        // if not primitive, list remains unchanged
        Term r;

        if (t.isPrimitive())
        {
            // prims are coded to eval
            // to prevent eval, set `primNoEval`
            // BUT ONLY FOR ONE EVAL
            _primNoEval = opt & EVALOPT_NOEVALARGS;
            r = t.asPrimitive().call(it, env);
            _primNoEval = false;
            if (!r)
            {
                // RAW null to ()
                r = Term(List());
            }
        }
        else if (t.isFunction())
        {
            r = evalFunction(t, it, env, opt);
            _break &= ~breakReturn;
        }
        else if (t.isMacro())
        {
            r = evalFunction(t, it, env, opt | EVALOPT_DEFEREVALARGS);
            _break &= ~breakReturn;
        }
        else
        {
            // eval remaining terms too unless break
            // but don't clear break, wait for outer loop
            while (it && !_break)
            {
                r = eval(Term(*it), env, opt);
                ++it;
            }
        }
        return r;
    }
    
    Term        evalList(List& terms, Env& env, int opt)
    {
        // list evaluate by evaluating their first element, 
        // if this is a function, apply it to the other arguments
        // 
        // otherwise,
        // evaluate the remaining elements of the list and return the last.
        List::iterator it(terms);
        if (it)
        {
            // evaluate the first entry of the list
            // disable any defer/no eval option for the first arg
            int opt1 = opt & ~(EVALOPT_DEFEREVALARGS | EVALOPT_NOEVALARGS);
            Term t = eval(Term(*it), env, opt1);
            ++it;

            Term v = evalTail(t, it, env, opt);

            // if no tail eval, then first element is value
            if (!v) v = t;
            return v;
        }
        return terms;
    }

    Term        evalThunk(Thunk& th, Env& env, int opt)
    {
        Thunk::Info* i = th.info();

        // evaluate thunk with its own env, but only the args need changing
        Env e(env._env, i->_args);
        return eval(i->_expr, e, opt);
    }

    Term        eval(const Term& term, Env& env, int opt = 0)
    {
        Term t = term;
        do
        {
            switch (t.type())
            {
            case type_symbol:
                t = evalSymbol(t.asSymbol(), env, opt);
                break;
            case type_list:
                t = evalList(t.asList(), env, opt);
                break;
            case type_thunk:
                t = evalThunk(t.asThunk(), env, opt);
                break;
            case type_function:
            case type_macro:
                if (opt & EVALOPT_EVALFUNCTIONS)
                {
                    opt &= ~EVALOPT_EVALFUNCTIONS;
                    List a(*term, 1);
                    t = evalList(a, env, opt);
                }
                break;
            default:
                // otherwise eval to itself
                break;
            }        
        } while (t.isThunk());
        return t;
    }

    static Term varToTerm(const var& v)
    {
        // convert a var to an atomic term
        Term t;
        switch (v._type)
        {
        case var::var_string:
            t = Stringt(v._s);
            break;
        case var::var_double:
            t = Number((Number::valueT)v._d);
            break;
        case var::var_int:
            t = Int((long)v._i);
            break;
            
            // otherwise null
        }
        return t;
    }

    static var termToVar(const Term& t)
    {
        var v;

        // convert atomic term to var.
        // if not atomic (eg list), return null var

        switch (t.type())
        {
        case type_int:
            v = var((int64)t.asInt().value());
            break;
        case type_number:
            v = var((double)t.asNumber().get());
            break;
        case type_string:
            v = var(t.asString().c_str());
            break;
        case type_symbol:
            // symbols go to strings
            v = var(t.asSymbol().label());
            break;
        }
        return v;
    }

    static bool expect(const Term& t, uint type, const char* msg = "")
    {
        if (!t)
        {
            LOG1("Expected ", Head::typeName(type & 0xff) << " for NULL " << msg);
        }
            
        return t && _expect(*t, type, msg); 
    }

    static bool isEmptyList(const Term& t)
    {
        // or set
        return !t || t->isEmptyList();
    }

    static bool isFalse(const Term& t)
    {
        if (!t || isEmptyList(t)) return true;
        if (t.isInt())
        {
            if (((const Int&)*t).value() == 0) return true;
        }
        else if (t.isString())
        {
            if (t.asString().isEmpty()) return true;
        }
        return false;
    }

    static bool isTrue(const Term& t)
    {
        return !isFalse(t);
    }


protected:


    static bool _expect(const Head& h, uint type, const char* msg)
    {
        // allow `type' to have several types packed as bytes
        bool res = false;
        uint t = h.type();

        uint ty = type;
        while (ty && !res)
        {
            res = (t == (ty & 0xff));
            ty >>= 8;
        }
        
        if (!res)
        {
            LOG1("Expected ", Head::typeName(type & 0xff) << " for " << h << " " << msg);
        }
        
        return res;
    }


    Term _primFnFn(List::iterator& ai, Env& env)
    { return Lambda::functionDef(ai, type_function, &_symPool); }

    Term _primDefFn(List::iterator& ai, Env& env)
    { return Lambda::functionDef(ai, type_macro, &_symPool); }

    Term _evalArgs(List::iterator& ai, Env& env)
    {
        uint sz = ai.size();
        List largs(sz);
        List::iterator it(largs);

        while (ai)
        {
            Term t = EVALAI;
            if (t) it.insert(*t);
        }

        return largs;
    }

    int _evalArgsNumeric(Term& args, List::iterator& ai, Env& env)
    {
        args = _evalArgs(ai, env);
        
        // find most general numeric type
        int gtype = type_int;
        for (List::iterator it(args.asList()); it; ++it)
        {
            if (it->isNumeric())
                gtype = u_max(gtype, (int)it->type());
            else
            {
                // bail on non-numeric arg
                expect(Term(*it), gtype);
                gtype = type_null;
                break;
            }
        }

        if (gtype)
        {
            // convert all to `gtype'
            for (List::iterator it(args.asList()); it; ++it)
            {
                if (it->type() != gtype)
                {
                    // must be int (currently only other numeric type)
                    it.replace(Number(((Int&)*it).value()));
                }
            }
        }

        return gtype;
    }

    Term _primAddFn(List::iterator& ai, Env& env)
    {
        Term args;
        int type = _evalArgsNumeric(args, ai, env);
        if (type)
        {
            long v = 0;
            Number vv;
            int cc = 0;
            
            for (List::iterator it(args.asList()); it; ++it)
            {
                if (type == type_number)
                {
                    const Number& ni = (const Number&)*it;
                    if (!cc)
                        vv = ni; // copy
                    else
                        vv += ni;
                    ++cc;
                }
                else // int
                    v += ((Int&)*it).value();
            }
            
            return type == type_number ? Term(vv) : Term(Int(v));
        }
        return Term();
    }

    Term _primSubFn(List::iterator& ai, Env& env)
    {
        Term args;
        int type = _evalArgsNumeric(args, ai, env);
        if (type)
        {
            long v = 0;
            Number vv;
            int cc = 0;
            
            for (List::iterator it(args.asList()); it; ++it)
            {
                if (type == type_number)
                {
                    const Number& ni = (const Number&)*it;
                    if (!cc)
                        vv = ni; // copy
                    else
                        vv -= ni;
                }
                else // int
                {
                    long vi = ((Int&)*it).value();
                    if (!cc)
                        v = vi;
                    else
                        v -= vi;
                }
                ++cc;
            }
            
            return type == type_number ? Term(vv) : Term(Int(v));
        }
        return Term();
    }

    Term _primMulFn(List::iterator& ai, Env& env)
    {
        Term args;
        int type = _evalArgsNumeric(args, ai, env);
        if (type)
        {
            long v = 1;
            Number vv;
            int cc = 0;
            
            for (List::iterator it(args.asList()); it; ++it)
            {
                if (type == type_number)
                {
                    const Number& ni = (Number&)*it;
                    if (!cc)
                        vv = ni; // copy
                    else
                        vv *= ni;
                    ++cc;
                }
                else // int
                    v *= ((Int &)*it).value();
            }
            
            return type == type_number ? Term(vv) : Term(Int(v));
        }
        return Term();
    }

    Term _primDivFn(List::iterator& ai, Env& env)
    {
        Term args;
        int type = _evalArgsNumeric(args, ai, env);
        if (type)
        {
            long v = 0;
            Number vv;
            int cc = 0;
            
            for (List::iterator it(args.asList()); it; ++it)
            {
                if (type == type_number)
                {
                    const Number& ni = (Number&)*it;
                    if (!cc)
                        vv = ni; // copy
                    else
                        vv /= ni;
                }
                else // int
                {
                    long vi = ((Int&)*it).value();
                    if (!cc)
                        v = vi;
                    else
                    {
                        if (!vi)
                        {
                            // divide by zero. promote result to number.
                            if (!v)
                                vv = Number::nan();
                            else if (v > 0)
                                vv = Number::posInf();
                            else
                                vv = Number::negInf();

                            type = type_number;
                            break;
                        }
                        v /= vi;
                    }
                }
                ++cc;
            }
            
            return type == type_number ? Term(vv) : Term(Int(v));
        }
        return Term();
    }

    enum RelOp
    {
        rel_lt,
        rel_le,
        rel_gt,
        rel_ge,
        rel_count,
    };

    static const char* relopName(int relop)
    {
        static const char *reltab[] =
        {
            "<",
            "<=",
            ">"
            ">=",
        };

        assert(relop < ASIZE(reltab));
        return reltab[relop];
    }
    
    typedef bool relfn(long a, long b);

    Term _primRelFn(List::iterator& ai, Env& env, RelOp relop)
    {
        // eg (< a b)
        Term r;
        Term args;
        int type = _evalArgsNumeric(args, ai, env);
        if (type)
        {
            bool ok = false;
            List::iterator it(args.asList());
            if (it)
            {
                ok = true;
                const Head& a = *it;
                ++it;
                if (it)
                {
                    const Head& b = *it;
                    if (type == type_number)
                    {
                        const Number& na = (const Number&)a;
                        const Number& nb = (const Number&)b;

                        switch (relop)
                        {
                        case rel_lt:
                            if (na.get() < nb.get()) r = Int(1);
                            break;
                        case rel_le:
                            if (na.get() <= nb.get()) r = Int(1);
                            break;
                        case rel_gt:
                            if (na.get() > nb.get()) r = Int(1);
                            break;
                        case rel_ge:
                            if (na.get() >= nb.get()) r = Int(1);
                            break;
                        }
                    }
                    else // int
                    {
                        const Int& na = (const Int&)a;
                        const Int& nb = (const Int&)b;

                        switch (relop)
                        {
                        case rel_lt:
                            if (na.value() < nb.value()) r = Int(1);
                            break;
                        case rel_le:
                            if (na.value() <= nb.value()) r = Int(1);
                            break;
                        case rel_gt:
                            if (na.value() > nb.value()) r = Int(1);
                            break;
                        case rel_ge:
                            if (na.value() >= nb.value()) r = Int(1);
                            break;
                        }
                    }
                }
            }

            if (!ok)
            {
                LOG1("expected two parameter fors ", relopName(relop));
            }
        }
        return r;
    }
    
    Term _primLTFn(List::iterator& ai, Env& env)
    { return _primRelFn(ai, env, rel_lt); }
    Term _primLEFn(List::iterator& ai, Env& env)
    { return _primRelFn(ai, env, rel_le); }
    Term _primGTFn(List::iterator& ai, Env& env)
    { return _primRelFn(ai, env, rel_gt); }
    Term _primGEFn(List::iterator& ai, Env& env)
    { return _primRelFn(ai, env, rel_ge); }

    
    Term _primTypeofFn(List::iterator& ai, Env& env)
    {
        Term t;
        if (ai)
        {
            t = EVALAI;
            t = Int(t.type());
        }
        return t;
    }

    Term _setFn(List::iterator& ai, Env& env, bool local)
    {
        // (set var value [env])

        Term r;
        if (ai)
        {
            Term var = EVALAI;
            if (expect(var, type_symbol, "in set"))
            {
                if (ai)
                {
                    r = EVALAI;
                    if (ai)
                    {
                        // make new environment
                        Term t = EVALAI;
                        Env e(t, env._args);

                        // local but not temp (ie we take reference)
                        _bindvar(var, &*r, e, local, false);
                    }
                    else
                    {
                        // not local and not temp, ref taken
                        _bindvar(var, &*r, env, local, false);
                    }
                }
                else
                {
                    // (set foo) to unbind `foo'
                    _bindvar(var, 0, env, local, false);
                }
            }
        }
        return r;
    }

    Term _primSetFn(List::iterator& ai, Env& env)
    {
        // (set var value [env])
        return _setFn(ai, env, true); // creates locally
    }

    Term _primSetuFn(List::iterator& ai, Env& env)
    {
        // (set var value [env])

        // will set global before making new local
        return _setFn(ai, env, false);
    }

    Term _primEqualFn(List::iterator& ai, Env& env)
    {
        // (== t1 t2 ...).
        // return 1 iff t1 == t2 == .. = tn
        // otherwise 0.
        //
        // compare values 

        int res = 1;
        if (ai)
        {
            Term a1 = EVALAI;
            ComparatorEq co;
            while (ai)
            {
                Term t = EVALAI;
                if (co(*a1, *t))
                {
                    res = 0;
                    break;
                }
            }
        }
        return Int(res);
    }

    Term _primNotEqualFn(List::iterator& ai, Env& env)
    {
        if (isTrue(_primEqualFn(ai, env)))
            return List(); // false
        return Int(1); // true
    }

    Term _primNotFn(List::iterator& ai, Env& env)
    {
        // (not 0) = 1
        // (not ()) = 1
        // (not "") = 1
        // else ()

        if (ai)
        {
            Term t = EVALAI;
            if (isTrue(t))
                return List(); // false
        }
        return Int(1);
    }

    Term _primAndFn(List::iterator& ai, Env& env)
    {
        // (and e1 .. en)

        assert(!_break);

        Term t;
        while (ai && !_break)
        {
            t = EVALAI;
            if (isFalse(t)) break;
        }
        return t;
    }

    Term _primOrFn(List::iterator& ai, Env& env)
    {
        // (or e1 .. en)

        assert(!_break);
        
        Term t;
        while (ai && !_break)
        {
            t = EVALAI;
            if (isTrue(t)) break;
        }
        return t;
    }

    Term _primIfFn(List::iterator& ai, Env& env)
    {
        // (if condition e1 e2 ..)
        
        assert(!_break);

        Term r;
        if (ai)
        {
            // condition
            Term c = EVALAI;
            if (isTrue(c) && !_break)
            {
                while (ai)
                {
                    r = eval(Term(*ai), env);
                    if (_break) break;
                    ++ai;
                }
            }
        }
        return r;
    }

    Term _primIfnotFn(List::iterator& ai, Env& env)
    {
        // (ifnot condition e1 e2 ..)
        
        assert(!_break);

        Term r;
        if (ai)
        {
            // condition
            Term c = EVALAI;
            if (isFalse(c) && !_break)
            {
                while (ai)
                {
                    r = eval(Term(*ai), env);
                    if (_break) break;
                    ++ai;
                }
            }
        }
        return r;
    }
    
    Term _primIfElseFn(List::iterator& ai, Env& env)
    {
        // (if condition true false)
        Term r;
        if (ai)
        {
            // condition
            r = EVALAI;
            if (ai && !_break)
            {
                if (isTrue(r)) r = eval(Term(*ai), env);                    
                else
                {
                    ++ai; // skip true expr
                    if (ai) r = eval(Term(*ai), env);
                    else r = Term(); // false
                }
            }
        }
        return r;
    }


    Term _evalFn(List::iterator& ai, Env& env)
    {
        // (eval expr [env])
        Term t;
        if (ai)
        {
            int opt = EVALOPT_EVALFUNCTIONS;
            
            t = EVALAI;
            if (ai)
            {
                // make new environment
                Env e(eval(Term(*ai), env, opt), env._args);
                t = eval(t, e, EVALOPT_NOBINDERROR);
                
                // we are about to destroy an environment which could
                // be the holder of `t'. `t' needs to be made usable.
                t->use();
            }
            else
            {
                //t = eval(t, env, EVALOPT_NOBINDERROR);
                t = eval(t, env, opt);
            }
        }
        return t;
    }

    Term _primEvalFn(List::iterator& ai, Env& env)
    {
        // (eval expr [env])
        return _evalFn(ai, env);
    }

#ifdef DEBUGOUT
    Term _primDEvalFn(List::iterator& ai, Env& env)    
    {
        Head::_showRefs++;
        Term t = _evalFn(ai, env);
        Head::_showRefs--;
        return t;
    }
#endif    

    Term _primEnvFn(List::iterator& ai, Env& env)
    {
        // (env [env])
        if (ai)
        {
            // if we have an arg, set the env
            Term oe = env._env;
            env._env = EVALAI;

            // return the old one
            return oe;
        }
        else
        {
            // no args, return current env
            return env._env;
        }
    }

    Term _primListFn(List::iterator& ai, Env& env)    
    {
        // create a list of elements
        // (list e1 e2 .. en)
        
        List l;
        List::iterator it(l);
        LValue lv(it);
            
        while (ai)
        {
            Term t = EVALAI;
            lv = *t;
        }
        return l;
    }

    Term _primTreeFn(List::iterator& ai, Env& env)    
    {
        // create a tree of elements
        // (tree e1 e2 .. en)
        
        Tree l;
        Tree::iterator it(l);
        LValue lv(it);
            
        while (ai)
        {
            it.toSup(); // force append
            Term t = EVALAI;
            lv = *t;
        }
        return l;
    }

    Term _primAtFn(List::iterator& ai, Env& env)    
    {
        // (at list n)
        // (at list n value)  // change value in a list
        // (at tree n)
        if (ai)
        {
            Term list = EVALAI;
            if (expect(list, TYPES_LIST) && ai)
            {
                Term n = EVALAI;
                if (expect(n, type_int))
                {
                    long nv = (int)n.asInt().value();
                    if (nv >= 0)
                    {
                        Term r;
                        if (list.type() == type_list)
                        {
                            List::iterator it(list.asList());
                            it.skip((uint)nv);

                            if (it)
                            {
                                // optional new value
                                if (ai)
                                {
                                    r = EVALAI;

                                    LValue lv(it);
                                    lv = *r;
                                }
                                else
                                {
                                    r = *it;
                                }
                            }
                        }
                        else // tree
                        {
                            Tree::iterator it(list.asTree());
                            for (it.toMin(); it; ++it)
                            {
                                if (nv <= 0) break;
                                --nv;
                            }
                            if (it) r = *it;
                        }
                        return r;
                    }
                }
            }
        }
        return Term();
    }

    static uint _size(Term t, bool logical = false)
    {
        // (size list)
        // (size "string")
        uint n;
        switch (t.type())
        {
        case type_list:
            n = t.asList().size();
            break;
        case type_tree:
            n = t.asTree().size();
            break;
        case type_string:
            if (logical) n = t.asString().length(); 
            else n = t.asString().size();
            break;
        default:
            n = 1;
        }
        return n;
    }


    Term _primSizeFn(List::iterator& ai, Env& env)    
    {
        // (size list)
        // (size "string")
        Term res;
        if (ai)
        {
            Term t = EVALAI;
            uint n = _size(t, false);
            res = Int(n);
        }
        return res;
    }

    Term _primLengthFn(List::iterator& ai, Env& env)    
    {
        // (length "string")
        Term res;
        if (ai)
        {
            Term t = EVALAI;
            uint n = _size(t, true);
            res = Int(n);
        }
        return res;
    }

    static void _insert(Term list, const Head& val, int at, bool unique = false)
    {
        if (list.type() == type_list)
        {
            // already present?
            if (unique && list.asList().member(val)) return;
            const Head& h = val.prepuse();
            if (at >= 0)
                list.asList().insert(h, at);
            else
            {
                // append to list
                list.asList().append(h);
            }
        }
        else // tree
        {
            list.asTree().insert(val.prepuse());
        }
    }


    Term _primConcAux(List::iterator& ai, Env& env, bool unique)
    {
        // (conc dst-list srclist1 srclist2 ...)
        // NB: this is destructive, changing `dst-list` 
        Term r;
        if (ai)
        {
            r = EVALAI;
            if (expect(r, TYPES_LIST, "conc dest"))
            {
                // when we modify, must be usable otherwise
                // not a temp ref.
                r->use();
                
                while (ai)
                {
                    Term ti = EVALAI;
                    if (ti.isList())
                    {
                        for (List::iterator it(ti.asList()); it; ++it)
                            _insert(r, *it, -1, unique); // append
                    }
                    else if (ti.isTree())
                    {
                        Tree::iterator it(ti.asTree());
                        for (it.toMin(); it; ++it)
                            _insert(r, *it, -1, unique); // append
                    }
                    else
                    {
                        _insert(r, *ti, -1, unique);
                    }
                }
            }
        }
        return r;
    }

    Term _primConcFn(List::iterator& ai, Env& env)
    {
        return _primConcAux(ai, env, false);
    }

    Term _primConcuFn(List::iterator& ai, Env& env)
    {
        return _primConcAux(ai, env, true);
    }

    Term _primInsertAux(List::iterator& ai, Env& env, bool unique)
    {
        // NB: DESTRUCTIVE operation.

        // (insert list val at) -> list
        // (insert list val) -> list, append to list
        // (insert tree val) -> tree or () if insert fails.
        // (insert tree val1 val2 ...) -> tree or () if insert fails.
        if (ai)
        {
            Term list = EVALAI;
            if (expect(list, TYPES_LIST) && ai)
            {
                // when we modify, must be usable otherwise
                // not a temp ref.
                list->use();
                
                if (list.isList())
                {
                    int atpos = -1; // undefined
                    Term val= EVALAI;
                    
                    // optional parameter is position
                    // not applicable for tree insert
                    if (ai)
                    {
                        Term at = EVALAI;
                        if (expect(at, type_int)) atpos = at.asInt().value();
                    }

                    _insert(list, *val, atpos);
                }
                else // tree
                {
                    assert(list.isTree());
                    do
                    {
                        Term val= EVALAI;
                        _insert(list, *val, -1);
                    } while (ai);
                }
                
                return list;
            }
        }
        return Term();
    }

    Term _primInsertFn(List::iterator& ai, Env& env)    
    {
        // insert
        return  _primInsertAux(ai, env, false);
    }

    Term _primInsertuFn(List::iterator& ai, Env& env)    
    {
        // insert if unique
        return  _primInsertAux(ai, env, true);
    }

    void _print(const Term& t, SBuf& sb)
    {
        assert(t);  // can't print a NULL term
        
        t._head->toSBuf(sb);
        *_out << (const char*)sb;
        _out->flush();
    }

    Term _primPrintFn(List::iterator& ai, Env& env)    
    {
        // (print e1 e2 ..)
        Term t;
        int cc = 0;
        while (ai)
        {
            t = EVALAI;
            if (cc++) *_out << ' ';

            SBuf sb;
            _print(t, sb);
        }
        *_out << std::endl;
        return t;
    }


    Term _primLogFn(List::iterator& ai, Env& env)
    {
        // (log level e1 e2 ..)
        // return last expression
        Term t;
        int cc = 0;

        int level = 1;
        
        if (ai)
        {
            t = EVALAI;
            if (!expect(t, type_int, "log")) return t;
            level = ((Int&)*t).value();

            SBuf sb;
            sb._mode = SBuf::mode_noquotes; 
            while (ai)
            {
                t = EVALAI;
                if (cc++) sb.add(' ');

                assert(t);
                t._head->toSBuf(sb);
            }

            LOGN(level, "Log: ", (const char*)sb);

        }
        return t;
    }
    
    static const char* _parsePrintf(const char* f, int& type)
    {
        // f is immediately after '%'
        // format: [-+ ][0-9]*[.][0-9]*[diuxofegsc]

        // NB: format "o" is not octal, it's object!

        // if format ok, return pointer to next character after format
        // set type as follows:
        // 0 => fail
        // 1 => int
        // 2 => float
        // 3 => string
        // 4 => object
        // 5 => %%

        type = 0;

        if (*f == '%')
        {
            // %%
            type = 5;
            ++f;
        }
        else
        {
            if (*f == '-' || *f == '+' || *f == ' ') ++f;
            while (u_isdigit(*f)) ++f;
            if (*f == '.')
            {
                ++f;
                while (u_isdigit(*f)) ++f;
            }
            char c = u_tolower(*f);
            if (c)
            {
                ++f;
                if (strchr("diux", c)) type = 1;
                else if (strchr("deg", c)) type = 2;
                else if (strchr("sc", c)) type = 3;
                else if (c == 'o') type = 4; // object
            }
        }
        
        return f;
    }

    Term _primPrintfFn(List::iterator& ai, Env& env)    
    {
        // (printf "format" arg ...)
        
        Term t;
        if (ai)
        {
            GrowString gs;
            
            t = EVALAI;
            
            // convert the format arg to a string without quotes
            SBuf fsb;
            fsb._mode = SBuf::mode_noquotes;

            // empty format is treated as empty string
            if (!isEmptyList(t))
                t._head->toSBuf(fsb);

            // walk the format string converting remaining args as we go.
            const char* st = fsb;
            const char* p = st;
            
            for (;;)
            {
                p = strchr(p, '%');
                if (!p) break;
                
                // walk over the printf format %..<formatchar>
                const char* fmt = p++;

                int type;
                p = _parsePrintf(p, type);

                if (type)
                {
                    JSONWalker::decodeString(gs, st, fmt - st);

                    if (type == 5)
                    {
                        // %%
                        gs.add('%');
                    }
                    else if (ai)
                    {
                        t = EVALAI;
                        assert(t);

                        string fmts(fmt, p - fmt);

                        switch (type)
                        {
                        case 1:
                            // int
                            {
                                var v = termToVar(t);
                                if (v)
                                {
                                    int64 val = v.toInt();
                                    gs.appendf(fmts.c_str(), (int)val);
                                }
                                else
                                {
                                    LOG1("printf term not integer ", t);
                                }
                            }
                            break;
                        case 2:
                            // float
                            {
                                var v = termToVar(t);
                                if (v)
                                {
                                    double val = v.toDouble();
                                    gs.appendf(fmts.c_str(), val);
                                }
                                else
                                {
                                    LOG1("printf term not float ", t);
                                }
                            }
                            break;
                        case 3:
                            // string
                            // do not print empty lists, ie nulls
                            if (!isEmptyList(t))
                            {
                                SBuf sb;
                                sb._mode = SBuf::mode_noquotes;
                                t._head->toSBuf(sb);
                                JSONWalker::decodeString(gs, sb, sb.size());
                            }
                            break;
                        case 4:
                            // object
                            {
                                SBuf sb;
                                t._head->toSBuf(sb);
                                JSONWalker::decodeString(gs, sb, sb.size());
                            }
                            break;
                        }
                    }

                    st = p;
                }
            }
            
            // handle escapes
            JSONWalker::decodeString(gs, st, strlen(st));

#if 0
            // NO don't emit anything more
            // any remaining args are converted to strings
            while (ai)
            {
                t = EVALAI;
                assert(t);
                fsb.reset();
                t._head->toSBuf(fsb);
                JSONWalker::decodeString(gs, fsb, fsb.size());
            }
#endif            

            gs.add(0);

            *_out << gs.start();

        }
        return t;
    }


    Term _primWhileFn(List::iterator& ai, Env& env)    
    {
        // (while condition e1 e2 .. en)
        // returns `en' or last expression before (break)

        assert(!_break);
        
        Term t;
        if (ai)
        {
            Term cond(*ai); ++ai;
            
            while (isTrue(eval(cond, env)) && !_break)
            {
                for (List::iterator ij(ai); ij; ++ij)
                {
                    t = eval(Term(*ij), env);
                    if (_break) goto whileexit;
                }
            }
        }

    whileexit:;

        _break &= ~breakBreak;
        return t;
    }

    Term _primForallFn(List::iterator& ai, Env& env)    
    {
        // (forall var list e1 e2 .. en)
        // return last expression
        // if list empty, return null

        assert(!_break);

        Term t;
        if (ai)
        {
            Term var = EVALAI;
            if (expect(var, type_symbol))
            {
                Term sequence = EVALAI;
                switch (sequence.type())
                {
                case type_list:
                    for (List::iterator it(sequence.asList()); it; ++it)
                    {
                        // use local binding for iterator symbol
                        _bindvar(var, &*it, env, true, true);
                        for (List::iterator ij(ai); ij; ++ij)
                        {
                            t = eval(Term(*ij), env);
                            if (_break) break;
                        }
                        if (_break) break;
                    }
                    break;
                case type_tree:
                    {
                        Tree::iterator it(sequence.asTree());
                        for (it.toMin(); it; ++it)
                        {
                            // use local binding for iterator symbol
                            _bindvar(var, &*it, env, true, true);
                            for (List::iterator ij(ai); ij; ++ij)
                            {
                                t = eval(Term(*ij), env);
                                if (_break) break;
                            }
                            if (_break) break;
                        }
                    }
                    break;
                default:
                    // use local binding for iterator symbol
                    _bindvar(var, &*sequence, env, true, true);
                    for (List::iterator ij(ai); ij; ++ij)
                    {
                        t = eval(Term(*ij), env);
                        if (_break) break;
                    }
                    break;
                }
                _bindvar(var, 0, env, true, true); // unbind
            }

            // clear any break state
            _break &= ~breakBreak;
            
        }
        return t;
    }

    Term _primBreakFn(List::iterator& ai, Env& env)    
    {
        // break from enclosing while
        // no args
        _break |= breakBreak;
        return Term();
    }

    Term _primReturnFn(List::iterator& ai, Env& env)    
    {
        // return from function 
        // (return <retvalue>)
        
        Term t;
        if (ai)
        {
            t = EVALAI;
        }
        
        _break |= breakReturn;
        return t;
    }

    Term _primReturnIfFn(List::iterator& ai, Env& env)    
    {
        // return from function if <retvalue> is true
        // (returnif <retvalue>)
        
        Term t;
        if (ai)
        {
            t = EVALAI;
            if (isTrue(t))
                _break |= breakReturn;                
        }
        
        return t;
    }

    Term _primQuoteFn(List::iterator& ai, Env& env)    
    {
        Term t;
        if (ai)
        {
            t = *ai;
            if (t.isSymbol())
            {
                Symbol& sy = t.asSymbol();
                if (sy.isArg())
                    t = fetchArg(sy, env, EVALOPT_NOEVALARGS);
            }
        }
        return t;
    }

    Term _primLoadFn(List::iterator& ai, Env& env)    
    {
        // load files
        // (load "filename" ...)

        int cc = 0;
        while (ai)
        {
            Term t = EVALAI;
            if (expect(t, type_string))
            {
                string f = t.asString().c_str();
                if (loadFile(f, env)) ++cc;
                else
                {
                    *_out << "Error loading " << t << std::endl;
                }
            }
        }
        return Int(cc);
    }

    static int buildJSONTermElt(GrowString& js, Term ti)
    {
        int jcount = 0;
        if (ti.type() == type_list)
        {
            // each element is expected to be a pair (key val)
            List::iterator i(ti.asList());
            if (i)
            {
                // the key must be atomic
                var kv = termToVar(i->primaryTerm());
                var vv;
                if (!kv)
                {
                    LOG1("IFISend, expected atomic type ", *i);
                    return 0;
                }

                ++i;
                if (i)
                {
                    Term arg = *i;
                    if (arg.isList() || arg.isTree())
                    {
                        // is a sub object
                        // (key {(subkey1 v1) (subkey2 v2) ..}) 
                        // -> key:{subkey1:v1,subkey2:v2}

                        // (key (k1 k2))
                        // -> key:[k1,k2]
                        GrowString js1;
                        buildJSONTerm(js1, arg);

                        js1.add(0);
                        
                        JSONWalker::addKeyObject(js,
                                                 kv.toString(),
                                                 js1.start());
                        
                        ++jcount;
                    }
                    else
                    {
                        vv = termToVar(arg);
                        if (!vv)
                        {
                            LOG1("IFISend, expected atomic type ", arg);
                            return 0;
                        }
                                
                        // have (kv vv) both atomic
                        JSONWalker::addKeyValue(js,
                                                kv.toString(),
                                                vv);

                        ++jcount;
                    }
                }
            }
        }
        else
        {
            // not actually an error
            // this will happen for any atomic parents in property
            LOG4("Warning: KL ifisend, property element is not list '", ti << "'");
        }

        return jcount;
    }
    
    static int buildJSONTerm(GrowString& js, Term t)
    {
        // NB: does not terminate `js` buf
        int jcount = 0;
        
        if (t.type() == type_list)
        {
            // array
            JSONWalker::toAdd(js);
            js.add('[');
            
            for (List::iterator it(t.asList()); it.valid(); ++it)
                jcount += buildJSONTerm(js, *it);
            
            js.add(']');
        }
        else if (t.type() == type_tree)
        {
            // js object
            JSONWalker::toAdd(js);
            js.add('{');
            Tree::iterator it(t.asTree());
            for (it.toMin(); it; ++it)
                jcount += buildJSONTermElt(js, *it);
            
            js.add('}');
        }
        else if (t.type() == type_blob)
        {
            ; // silently ignore any blobs
        }
        else
        {
            var v = termToVar(t);
            if (v)
            {
                JSONWalker::addValue(js, v);
                ++jcount;
            }
            else
            {
                LOG1("invalid term for ifisend", t);
            }
        }
    
        return jcount;
    }
    
    Term _primIFISendFn(List::iterator& ai, Env& env)    
    {
        // eg (ifisend ((foo 1) (bar "string")))
        // eg (ifisend {(foo 1) (bar "string")})
        // will send the json {"foo":1,"bar":string"}

        int cc = 0;
        
#ifdef IFI_BUILD        
        while (ai)
        {
            Term t = EVALAI;
            GrowString js;
            buildJSONTerm(js, t);

            js.add(0);
            extern void ifiSend(const char*);
            ifiSend(js.start());
            ++cc;
        }
#endif // IFI_BUILD        
        return Int(cc);
    }

    Term _primSplitFn(List::iterator& ai, Env& env)    
    {
        // (split "string" "tokens")
        // (split "string") ;; tokens assumed white space.
        // eg (split "hello world") -> ("hello" "world")

        if (ai)
        {
            Term str = EVALAI;
            if (expect(str, type_string))
            {
                Term tok;
                const char* tokens = " \t\n";
                if (ai)
                {
                    tok = EVALAI;
                    if (expect(tok, type_string)) tokens = tok.asString();
                }
                
                List sl;
                str.asString().split(sl, tokens);
                return sl;
            }
        }
        return Term();
    }

    Term _primSubstFn(List::iterator& ai, Env& env)    
    {
        // substitute strings
        // (subst "string" "pattern" "substitute")
        // (subst "string" "pattern") -> removes pattern

        if (ai)
        {
            Term str = EVALAI;
            if (expect(str, type_string) && ai.valid())
            {
                Term pat = EVALAI;
                if (expect(pat, type_string))
                {
                    Term sub;
                    const char* ssub = 0; // remove
                    
                    if (ai)
                    {
                        sub = EVALAI;
                        if (expect(sub, type_string))
                            ssub = sub.asString();
                    }

                    Stringt res;
                    if (str.asString().subst(res, pat.asString(), ssub))
                        return res;
                    
                    // if no substitutions, return original.
                    return str;
                }
            }            
        }
        return Term();
    }

    Term _primCatFn(List::iterator& ai, Env& env)
    {
        // concatenate strings
        // (cat "str1" "str2")
        // (cat "str1" n)
        // (cat)
        SBuf sb;
        sb._mode = SBuf::mode_noquotes; 
        while (ai)
        {
            Term r = EVALAI;
            if (!isEmptyList(r))
                r->toSBuf(sb);
        }
        return Stringt(sb);
    }

    Term _catsFnAux(List::iterator& ai, Env& env, bool sentence)
    {
        // concatenate strings adding spaces and optionally making a sentence
        // (cats "str1" "str2")
        SBuf sb;
        sb._mode = SBuf::mode_noquotes;
        bool first = sentence;
        while (ai)
        {
            Term r = EVALAI;
            if (isEmptyList(r)) continue;

            // convert term to string
            SBuf sbi;
            sbi._mode = SBuf::mode_noquotes; 
            r->toSBuf(sbi);

            uint sz = sbi.size();
            if (sz)
            {
                char c = sb.last();
                char* ts = sbi;
                
                if (c && !u_isspace(*ts) && !strchr(",;?!.", *ts))
                {
                    // automatically add any missing space between args
                    if (!u_isspace(c)) sb.add(' ');
                }

                if (first)
                {
                    // capitalise first char
                    char* p = ts;

                    // skip initial spaces and quote
                    while (u_isspace(*p) || *p == '"') ++p;

                    // upper case first char if possible
                    if (*p) *p = u_toupper(*p);
                    
                    first = false;
                }
                    
                sb.write(ts, sz);
            }
        }

        if (sentence)
        {
            // automatically add full stop to make a sentence
            uint sz = sb.size();
            if (sz)
            {
                char* p = sb;
                char* q = sb + sz;

                do
                {
                    --q;
                    if (!u_isspace(*q) && *q != '_' && *q != '"')
                    {
                        if (!strchr(".?;!,:", *q))
                        {
                            // did not end with punctuation already
                            sb.insert('.', q - p + 1);
                        }
                        break;
                    }
                } while (p != q);
            }
        }
        
        return Stringt(sb);
    }

    Term _primCatsFn(List::iterator& ai, Env& env)
    {
        return _catsFnAux(ai, env, false);
    }

    Term _primCatssFn(List::iterator& ai, Env& env)
    {
        return _catsFnAux(ai, env, true);
    }

    Term _primAppendFn(List::iterator& ai, Env& env)
    {
        // (append string expr ...)
        // NB: modifies string
        Term r;
        if (ai)
        {
            Term s = EVALAI;
            if (expect(s, type_string))
            {
                while (ai)
                {
                    Term a = EVALAI;
                    SBuf sb;
                    sb._mode = SBuf::mode_noquotes; 
                    a->toSBuf(sb);
                    s.asString().append(sb, sb.size());
                }
                r = s;
            }
        }
        return r;
    }
    
    Term _primCompareFn(List::iterator& ai, Env& env)    
    {
        // (compare term1 term2)
        
        Term res;
        if (ai)
        {
            Term t1 = EVALAI;
            if (ai)
            {
                Term t2 = EVALAI;
                ComparatorEq co;
                int r = co(*t1, *t2);
                res = Int(r);
            }
        }
        return res;
    }

    Term _primStreamFn(List::iterator& ai, Env& env)    
    {
        // create a stream
        // (stream "filename")
        Term res;
        if (ai)
        {
            Term fname = EVALAI;
            if (expect(fname, type_string))
            {
                Streamt st(fname.asString().c_str());
                return st;
            }
        }
        return res;
    }

    Term _primOutputFn(List::iterator& ai, Env& env)    
    {
        // (output <stream>)
        // change the output stream, return the old stream

        Term res;
        if (ai)
        {
            Term st = EVALAI;
            if (expect(st, type_stream))
            {
                StdStream* ns = st.asStream().donate();
                if (ns)
                {
                    res = Streamt(_out);
                    _out = ns;
                }
            }
        }
        return res;
    }

    uint _random(uint n)
    {
        uint v = 0;
        if (n)
        {
            // XX not linear!
            v = _ran.gen32() % n;
        }
        return v;
    }

    uint _nrandom(uint n, uint maxp, uint id)
    {
        return _nran.ran(n, maxp, id);
    }

    Term _primNRandomFn(List::iterator& ai, Env& env)
    {
        // (nrandom n maxp id) => 0 .. (n-1)
        uint v = 0;
        if (ai)
        {
            Term n = EVALAI;
            if (expect(n, type_int, "nrandom"))
            {
                uint nv = n.asInt().value();
                if (ai)
                {
                    Term maxp = EVALAI;
                    if (expect(maxp, type_int, "nrandom"))
                    {
                        uint maxpv = maxp.asInt().value();
                        uint idv = 0;
                        if (ai)
                        {
                            Term id = EVALAI;
                            if (expect(id, type_int, "nrandom"))
                                idv = id.asInt().value();
                        }

                        v = _nrandom(nv, maxpv, idv);
                    }
                }
                else
                {
                    // (nrandom n) -> (random n)
                    v = _random(nv);
                }
            }
        }
        return Int(v);
    }

    Term _primRandomFn(List::iterator& ai, Env& env)
    {
        // (random n) => 0 .. (n-1)
        // (random 0) = 0

        uint v = 0;
        if (ai)
        {
            Term n = EVALAI;
            if (expect(n, type_int, "random"))
            {
                v = _random(n.asInt().value());
            }
        }
        return Int(v);
    }

    Term _primDebugFn(List::iterator& ai, Env& env)    
    {
        // (debug n). set log level

        Term t(Int((long)Logged::_logLevel));  // return original level

        if (ai)
        {
            Term v = EVALAI;
            if (v.isInt())
            {
                long tv = v.asInt().value();
                if (tv >= 0)
                    Logged::_logLevel = (int)tv;
            }
        }
        return t;
    }

    Term _primApplyFn(List::iterator& ai, Env& env)
    {
        // (apply fn (arg1 arg2 ...))

        Term r;
        if (ai)
        {
            Term f = EVALAI;
            if (ai)
            {
                Term l = EVALAI;
                if (expect(l, type_list, "apply"))
                {
                    List::iterator ij(l.asList());

                    // do not eval f if it's a list
                    r = evalTail(f, ij, env, EVALOPT_NOEVALARGS);
                }
            }
        }
        return r;
    }

    Term _primMapFn(List::iterator& ai, Env& env)
    {
        // (map fn (arg1 arg2 ...))
        // (map fn {arg1 arg2 ...})

        assert(!_break);
        
        List l;
        List::iterator it(l);
        LValue lv(it);

        if (ai)
        {
            Term f = EVALAI;
            if (ai)
            {
                Term l = EVALAI;
                if (expect(l, TYPES_LIST, "map"))
                {
                    
                    if (l.isList())
                    {
                        for (List::iterator ij(l.asList()); ij; ++ij)
                        {
                            List arg(*ij, 0);
                            List::iterator argi(arg);
                            Term t = evalTail(f, argi, env, EVALOPT_NOEVALARGS);

                            if (_break) break;
                            
                            // don't accumulate empty lists
                            if (!isEmptyList(t)) lv = *t;
                        }
                    }
                    else if (l.isTree())
                    {
                        Tree::iterator ij(l.asTree());
                        for (ij.toMin(); ij; ++ij)
                        {
                            List arg(*ij, 0);
                            List::iterator argi(arg);
                            Term t = evalTail(f, argi, env, EVALOPT_NOEVALARGS);

                            if (_break) break;
                            
                            // don't accumulate empty lists
                            if (!isEmptyList(t)) lv = *t;
                        }
                    }
                }
            }
        }
        return l;
    }

    bool _isMember(const Head& x, Term& l)
    {
        // (find x list)
        // (find x tree)
        // (find x x)
        
        bool r = false;

        switch (l.type())
        {
        case type_list:
            {
                ComparatorEq co;
                r = l.asList().member(x, co);
            }
            break;
        case type_tree:
            {
                Tree::iterator it(l.asTree());
                r = it.find(x);
            }
            break;
        default:
            {
                ComparatorEq co;
                r = !co(*l, x);
            }
            break;
        }
        return r;
    }

    Term _primFindFn(List::iterator& ai, Env& env)
    {
        // (find x list)
        // (find x tree)
        // (find x x)
        Term r;
        if (ai)
        {
            Term x = EVALAI;
            if (x && ai)
            {
                Term l = EVALAI;
                // NB: do not return x, since could be zero => false
                if (_isMember(*x, l)) r = Int(1);
            }
        }
        return r;
    }

    Term _primSubListFn(List::iterator& ai, Env& env)
    {
        // (sub list1 list2)
        // subtract lists
        Term r;
        if (ai)
        {
            Term list = EVALAI;

            if (expect(list, TYPES_LIST, "sub") && ai)
            {
                Term l2 = EVALAI;
                switch (list.type())
                {
                case type_list:
                    {
                        r = List();
                        
                        for (List::iterator it(list.asList()); it; ++it) 
                        {
                            if (!_isMember(*it, l2)) _insert(r, *it, -1);
                        }
                    }
                    break;
                case type_tree:
                    {
                        r = Tree();

                        Tree::iterator it(list.asTree());
                        for (it.toMin(); it; ++it)
                        {
                            if (!_isMember(*it, l2)) _insert(r, *it, -1);  
                        }
                    }
                    break;
                }
            }
        }
        return r;
    }

    void _init()
    {
        Tree g;

        // XX need better way...
        _ran.seed(clock());

        // functions
        DEF_PRIM(Fn, "fn");
        DEF_PRIM(Def, "def");

        //arithmetic
        DEF_PRIM(Add, "+");
        DEF_PRIM(Sub, "-");
        DEF_PRIM(Mul, "*");
        DEF_PRIM(Div, "/");

        // comparison
        DEF_PRIM(Equal, "==");
        ADD_PRIM(Equal, "="); // XX later will want different equals
        DEF_PRIM(NotEqual, "!=");
        DEF_PRIM(LT, "<");
        DEF_PRIM(LE, "<=");
        DEF_PRIM(GT, ">");
        DEF_PRIM(GE, ">=");
        DEF_PRIM(Compare, "compare");

        // logical
        DEF_PRIM(Not, "not");
        DEF_PRIM(And, "and");
        DEF_PRIM(Or, "or");

        // variables
        DEF_PRIM(Set, "set");
        DEF_PRIM(Setu, "set!");
        DEF_PRIM(Env, "env");

        // lists
        DEF_PRIM(List, "list");
        DEF_PRIM(At, "at");
        DEF_PRIM(Insert, "insert");
        DEF_PRIM(Insertu, "insert!");
        DEF_PRIM(Size, "size");
        DEF_PRIM(Conc, "conc");
        DEF_PRIM(Concu, "conc!");
        DEF_PRIM(Find, "find");
        DEF_PRIM(SubList, "sub");

        // tree
        DEF_PRIM(Tree, "tree");

        // control
        DEF_PRIM(If, "if");
        DEF_PRIM(IfElse, "ife");
        DEF_PRIM(Ifnot, "ifnot");
        DEF_PRIM(Eval, "eval");
        DEF_PRIM(While, "while");
        DEF_PRIM(Forall, "forall");
        DEF_PRIM(Break, "break");
        DEF_PRIM(Return, "return");
        DEF_PRIM(ReturnIf, "returnif");
        DEF_PRIM(Quote, "'");
        DEF_PRIM(Apply, "apply");
        DEF_PRIM(Map, "map");
#ifdef DEBUGOUT
        DEF_PRIM(DEval, "deval"); // debug print
#endif

        // IO
        DEF_PRIM(Print, "print");
        DEF_PRIM(Printf, "printf");
        DEF_PRIM(Log, "log");
        DEF_PRIM(Load, "load");
        DEF_PRIM(IFISend, "ifisend");

        // Tests
        DEF_PRIM(Typeof, "typeof");

        // Strings
        DEF_PRIM(Split, "split");
        DEF_PRIM(Subst, "subst");
        DEF_PRIM(Cat, "cat");
        DEF_PRIM(Cats, "cats");
        DEF_PRIM(Catss, "catss");
        DEF_PRIM(Length, "length");
        DEF_PRIM(Append, "append");

        // Streams
        DEF_PRIM(Stream, "stream");
        DEF_PRIM(Output, "output");

        // Misc
        DEF_PRIM(Random, "random");
        DEF_PRIM(NRandom, "nrandom");
        

        // debug
        DEF_PRIM(Debug, "debug");

        _env._env = g;
    }
};

