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

#include <setjmp.h>
#include "ap.h"
#include "rand.h"
#include "defs.h"
#include "strands.h"
#include "keywords.h"
#include "timeline.h"
#include "cap.h"
#include "logged.h"
#include "version.h"
#include "dl.h"

#define GENSTATE_KEY  "_gen"

std::string em_getline(); // emscripten

#undef TAG
#define TAG "Strandi, "

#define OUTP(_x)   _out->_cap.add(_x)

extern bool cmdWaiting;

namespace ST
{

template<class T> struct Stacked
{
    struct Elt
    {
        T           _v;
        Elt*        _prev = 0;
    };

    Elt*        _top = 0;

    ~Stacked()
    {
        while (_top) pop();
    }
    
    void push(const T& v)
    {
        Elt* e = new Elt;
        e->_v = v;
        e->_prev = _top;
        _top = e;
    }

    void pop()
    {
        assert(_top);
        Elt* e = _top;
        _top = e->_prev;
        delete e;
    }

    T& get()
    {
        assert(_top);
        return _top->_v;
    }

    operator bool() const { return _top != 0; }

    struct Tmp
    {
        Stacked& _s;
        Tmp(Stacked& s, const T& v) : _s(s) { _s.push(v); }
        ~Tmp() { _s.pop(); }
    };
};


struct Strandi: public Traits
{
    typedef Term::Terms Terms;
    typedef std::function<void(char)> Emitter;
    typedef Term::TermList TermList;
    typedef pnode::nodeType nodeType;
    typedef enode::nodeType enodeType;
    typedef Timeline::Strings Strings;
    typedef Timeline::Vars Vars;
    typedef Word::scopeType scopeType;

    struct Reaction
    {
        Strandi*        _host;
        Selector*       _s;
        pnode*          _reactor;  // owned resolved copy
        int             _rank = 0;

        string          id() const
        {
            string s = _s->id();
            if (_reactor)
            {
                s += '_';

                // Doesn't really have to be "fancy",
                // but do need to expand bindings.
                s += _host->textifyFancy(_reactor);
            }
            return s;
        }

        Reaction(Strandi* host, Selector* s, pnode* pn)
            : _host(host),_s(s), _reactor(pn) {}

        Reaction(const Reaction& r) { _dub(r); }
        ~Reaction() { delete _reactor; }

    private:

        void _dub(const Reaction& r)
        {
            // move content
            _host = r._host;
            _s = r._s;
            _reactor = r._reactor;
            _rank = r._rank;
            const_cast<Reaction&>(r)._reactor = 0;
        }
    };

    typedef std::list<Reaction> Reactions;

    struct CapStack
    {
        Capture         _cap;
        CapStack*       _prev = 0;
        uint            _rmask = -1;
    };

    typedef AP<CapStack> CapRef;
    typedef std::function<Capture*(const string&, Capture*)> Evaluator;

    enum RunMask
    {
        run_choice = 1,
        run_media = 2,
    };
    
    Terms*      _terms;
    Ranq1       _rand;
    Emitter     _emitter;
    Timeline    _state;
    ParseCommand _pcom;
    bool        _prepared = false;
    uint        _runtypemask = (uint)-1;
    CapStack*   _out = 0;
    Evaluator   _eval;

    // runtime
    Term*       _player = 0;
    Term*       _thing = 0;
    Term*       _tick = 0;
    Term*       _scopeSeed = 0;
    Term*       _errorNocando = 0;
    Term*       _errorNosuch = 0;
    Term*       _errorSyntax = 0;

    jmp_buf     _env_top;
    int         _time = 0;

    //Exec        _exec;


    struct PronBinding
    {
        // bindings for pronouns
        const Word*     _w;
        Term*           _t;
        PronBinding*    _prev = 0;

        PronBinding(const Word* w, Term* t) : _w(w), _t(t) {}
    };

    struct execInfo
    {
        pnode*      _ps;
        bool        _err = true;
        bool        _output = false;  // emit results of command
        pnode*      _verbn = 0;
        const Word* _verb = 0;
        const Word* _verbPrep = 0;  // look at
        pnode*      _dobjn = 0;
        Binding*    _dobj = 0;
        pnode*      _prepn = 0;
        const Word* _prep = 0;
        const Word* _prepmod = 0; // modifier for prep eg also/not

        pnode*      _iobjn = 0;
        Binding*    _iobj = 0;
        enode*      _value = 0; // in place of iobj when prep is property
        Term*       _it = 0;
        Term*       _that = 0;

        // hit manual break during execution
        bool        _break = false;
        bool        _resolving = false;
        bool        _internalOps = true; // allow "put" and "set"

        // current exec info
        Selector*   _currentSelector = 0; // being exec
        execInfo*   _prev = 0; // upchain of execs
        

        execInfo(pnode* ps): _ps(ps) {}
    };


    struct Context
    {
        typedef Stacked<Capture*>  LastCap;
        
        // resolution context
        Strandi*            _host;
        Context*            _prev;
        
        TermList            _scope;
        TermList            _resolutionScope;

        // stack of temp LAST bindings
        LastCap              _lastCap;

        // last generator index selected
        //int                  _lastGen = 0;
        
        PronBinding*        _pronouns = 0;

        // currently available
        Reactions           _reactions;
        execInfo*           _currentExec = 0;
        bool                _scopeChanged = false;

        Context(Strandi* h) : _host(h) {}
        ~Context() { _purge(); }

        void _pushPron(const Word* w, Term* t)
        {
            PronBinding* pb = new PronBinding(w, t);
            pb->_prev = _pronouns;
            _pronouns = pb;
        }

        void _popPron(const Word* w)
        {
            // pop the last binding for word `w`
            PronBinding** pb = &_pronouns;
            while (*pb)
            {
                PronBinding* t = *pb;
                if (t->_w == w)
                {
                    *pb = t->_prev;
                    delete t;
                    return;
                }
                pb = &t->_prev;
            }
            assert(0);
        }

        PronBinding* _getPronBinding(const Word* w) const
        {
            PronBinding* p = _pronouns;
            while (p)
            {
                if (p->_w == w) break;
                p = p->_prev;
            }
            return p;
        }

        Term* _getPron(const Word* w) const
        {
            PronBinding* p = _getPronBinding(w);
            return p ? p->_t : 0;
        }

        void _replacePron(const Word* w, Term* t)
        {
            PronBinding* p = _getPronBinding(w);
            if (p) p->_t = t; // update
            else _pushPron(w, t);
        }

        void pushIt(Term* t) { _pushPron(_host->_pcom._it, t); }
        void popIt() { _popPron(_host->_pcom._it); }
        void setIt(Term* t) { _replacePron(_host->_pcom._it, t); }
        Term* getIt() const { return _getPron(_host->_pcom._it); }

        void pushThat(Term* t) { _pushPron(_host->_pcom._that, t); }
        void popThat() { _popPron(_host->_pcom._that); }
        void setThat(Term* t) { _replacePron(_host->_pcom._that, t); }
        Term* getThat() const { return _getPron(_host->_pcom._that); }

        bool resolveScopeReactions()
        {
            // locate all reactions from objects within scope, including
            // their parents.
            //
            // collect all reactions to objects in scope and resolve them
            // into ctx.reactions
            //
            // also extend `resolutionScope` beyond its initial scope
            // by adding in objects mentioned in reactions.
            //
            // return true if we performed a resolve

            assert(_reactions.empty());
            
            _resolutionScope.clear();
            concatc(_resolutionScope, _scope);
            _host->resolveAllReactions(_scope);
            return true;
        }

        void purgeReactions()
        {
            //LOG1("purging reactions ", _reactions.size());
            _reactions.clear();
        }

        void includeInResolutionScope(pnode* pn)
        {
            // add to resolution scope
            while (pn)
            {
                if (pn->_binding)
                    concatm(_resolutionScope, pn->_binding->_terms);
                
                includeInResolutionScope(pn->_head);
                pn = pn->_next;
            }
        }

    private:

        void _purge() { while (_pronouns) _popPron(_pronouns->_w); }
    };

    typedef Context::LastCap LastCap;

    // handle transient pronoun bindings
    struct BindPron
    {
        Strandi*    _host;
        const Word* _w;
        
        BindPron(Strandi* h, const Word* w, Term* t) : _host(h), _w(w)
        { _host->_ctx->_pushPron(_w, t); }
        ~BindPron() { _host->_ctx->_popPron(_w); } 
    };

    struct BindIt: public BindPron
    {
        BindIt(Strandi* h, Term* t) : BindPron(h, h->_pcom._it, t) {}
    };

    Context*        _ctx = 0;

    void _pushctx()
    {
        Context* ctx = new Context(this);
        ctx->_prev = _ctx;
        _ctx = ctx;
    }

    void _popctx()
    {
        assert(_ctx);

        Context* ctx = _ctx;
        _ctx = ctx->_prev;
        delete ctx;
    }

    static void _emitterDefault(char c)
    {
        if (c) putchar(c);
        if (!c || c == '\n') fflush(stdout);
    }

    void setdebug(int v) { _pcom._debug = v; }

    struct NRan
    {
        Ranq1*          _ran;
        unsigned int    _n;
        unsigned int    _maxp;
        unsigned int    _runs;
        unsigned int*   _stats;
        
        NRan(Ranq1* r, int n, int maxp) : _ran(r), _n(n), _maxp(maxp)
        {
            _runs = 0;
            _stats = new unsigned int[_n];
            memset(_stats, 0, sizeof(unsigned int)*_n);
            _warmup();
        }

        NRan(const NRan& nr)
        {
            _ran = nr._ran;
            _n = nr._n;
            _maxp = nr._maxp;
            _runs = nr._runs;
            _stats = new unsigned int[_n];
            memcpy(_stats, nr._stats, sizeof(unsigned int)*_n);
        }

        ~NRan() { delete [] _stats; }

        unsigned int nran()
        {
            unsigned int v;

            // look for forced omission
            unsigned int i;
            unsigned int imin = 0;
            for (i = 1; i < _n; ++i)
                if (_stats[i] > _stats[imin]) imin = i;
        
            if (_stats[imin] + 1 >= _maxp)
            {
                // forced
                v = imin;
            }
            else
            {
                // otherwise choose randomly
                v = _ran->gen32() % _n;
            }
            
            // adjust stats
            for (i = 0; i < _n; ++i) ++_stats[i];
            _stats[v] = 0;

            // keep counter of trials
            ++_runs;
            
            return v;
        }
        
        void _warmup()
        {
            // stir the pot
            for (unsigned int i = 0; i < _n; ++i) nran();
            
            // clear counter as warmup doesn't count.
            _runs = 0;
        }
    };

    struct GenState : public var::Blob
    {
        Strandi*        _host;
        Term::RType     _type;
        int             _seq = 0;
        int             _n = 0;  // used for seq
        NRan*           _nr = 0;

        GenState(Strandi* host) : _host(host) {}
        GenState(Strandi* host, Term::RType t, int n)
            : _host(host), _type(t), _n(n) { _init(); }

        void _init()
        {
            switch (_type)
            {
            case Term::t_shuffle:
                assert(!_nr);
                _nr = new NRan(&_host->_rand, _n, _n);
                break;
            case Term::t_nonrandom:
                assert(!_nr);
                _nr = new NRan(&_host->_rand, _n, _n+(_n+2)/3);
                break;
            }
        }

        ~GenState() { _purge(); }

        void _purge()
        {
            delete _nr;
            _nr = 0;
        }

        bool reset()
        {
            // return if reset made any difference
            
            bool changed = false;
            
            switch (_type)
            {
            case Term::t_sequence:
                if (_seq)
                {
                    changed = true;
                    _seq = 0;
                }
                break;
            case Term::t_shuffle:
                // always reshuffle
                _purge();
                _init();
                changed = true;
                break;
            case Term::t_nonrandom:
                // do nothing as reset nonrandom is nonrandom
                // unless we don't have a nonrandom, which happens
                // when we're converted
                if (!_nr) _init();
                break;
            }

            return changed;
        }

        int choose()
        {
            int ch = 0;
            switch (_type)
            {
            case Term::t_random:
                assert(_n);
                ch = _host->_rand.gen32() % _n;
                break;
            case Term::t_shuffle:
            case Term::t_nonrandom:
                assert(_nr);
                ch = _nr->nran();
                break;
            case Term::t_sequence:
                ch = _seq++;
                break;
            }
            return ch;
        }

        bool finished()
        {
            bool r = false;
            if (_type == Term::t_shuffle)
            {
                assert(_nr);
                if (!(_nr->_runs % _nr->_maxp)) r = true;
            }
            else if (_type == Term::t_sequence)
            {
                if (_seq >= _n) r = true;
            }
            return r;
        }

        bool convertTo(Term::RType t)
        {
            // return true if conversion took place
            
            bool r = false;

            //LOG1("converting ", Term::rtypeString(_type) << " to " << Term::rtypeString(t));
            
            // only some types can convert
            
            // random never changes
            // nonrandom never changes
            if (_type == Term::t_shuffle)
            {
                switch (t)
                {
                case Term::t_random:
                case Term::t_sequence:
                    assert(!_seq);
                    _type = t;
                    _n = _nr->_maxp;
                    reset();
                    r = true;
                    break;
                case Term::t_shuffle:
                case Term::t_nonrandom:
                    _type = t;
                    assert(_n);
                    reset();
                    r = true;
                    break;
                }
            }
            else if (_type == Term::t_sequence)
            {
                switch (t)
                {
                case Term::t_random:
                case Term::t_sequence:
                    assert(_n);
                    _type = t;
                    reset();
                    r = true;
                    break;
                case Term::t_shuffle:
                case Term::t_nonrandom:
                    _type = t;
                    assert(!_nr);
                    assert(_n);
                    reset();
                    r = true;
                    break;
                }
            }
            return r; // did we convert?
        }

        GenState* copyIf() 
        {
            // duplicate data for types with state
            
            GenState* gs = this;
            
            switch (_type)
            {
            case Term::t_shuffle:
            case Term::t_nonrandom:
            case Term::t_sequence:
                gs = (GenState*)copy();
            }

            return gs;
        }
        
        // blob compliance
        Blob* copy() const override
        {
            GenState* gs = new GenState(_host);
            gs->_type = _type;
            gs->_seq = _seq;
            gs->_n = _n;
            if (_nr) gs->_nr = new NRan(*_nr);
            return gs;
        }
        
        void destroy() override { delete this; }
        bool operator==(const Blob& b) const override { return this == &b; }
        string toString() const override { return "blob"; }
    };
    
    Strandi(Terms* t = 0) : _terms(t) 
    {
        using namespace std::placeholders;  
        _emitter = std::bind(&Strandi::_emitterDefault, _1);

        _pushcap();
        _pushctx();

        // dont forget to call `randomise`
    }

    ~Strandi() { _purge(); }

    void randomise(uint64 s) { _rand.seed(s); }
    void setTerms(Terms* t)
    {
        assert(!_terms);
        _terms = t;
    }
    void setEmitter(Emitter e) { _emitter = e; }
    void setEvaluator(Evaluator e) { _eval = e; }

    void _purge()
    {
        while (_out) _popcap();
        while (_ctx) _popctx();
    }

    // do no output formatting here.
    void _emit(char c) { _emitter(c); }
    void _emit(const char* s) { while (*s) _emit(*s++); }
    void _emit(const string& s) { _emit(s.c_str()); }
    void _emit(int v)
    {
        char buf[16];
        sprintf(buf, "%d", v);
        _emit(buf);
    }

#ifdef IFI_BUILD
    void emitSound(Flow::EltMedia& em)
    {
        GrowString gs;

        int chan = 0;

        // play once by default
        // override with "duration:X in media attri
        int dur = 1;

        // pick up channel if specified
        if (em._attr)
        {
            var chanv = em._attr->find(MEDIA_ATTR_CHAN);
            if (chanv) chan = chanv.toInt();

            var durv = em._attr->find(MEDIA_ATTR_DURATION);
            if (durv) dur = durv.toInt();
        }

        bool stop = em._filename == "null.ogg"; // XX
        if (stop) dur = 0;

        gs.add('{');
        JSONWalker::addKey(gs, IFI_SOUND);
        gs.add('{');
        JSONWalker::addKeyValue(gs, IFI_CHANNEL, chan);
        JSONWalker::addKeyValue(gs, IFI_DURATION, dur);

        if (!stop)
            JSONWalker::addStringValue(gs, IFI_NAME, em._filename);

        gs.add('}');
        gs.add('}');
        gs.add(0);
        ifi->emitResponse(gs.start());
    }

    void emitPicture(Flow::EltMedia& em)
    {
        if (em._attr)
        {
            GrowString gs;
            gs.add('{');
            JSONWalker::addKey(gs, IFI_PICTURE);
            gs.add('{');
            JSONWalker::addKeyValue(gs, IFI_NAME, em._filename);
            for (auto& e : *em._attr)
            {
                JSONWalker::addKeyValue(gs, e.first, e.second);
            }

            gs.add('}');
            gs.add('}');
            gs.add(0);
            LOG3(TAG "emit picture ", gs.start());
            ifi->emitResponse(gs.start());
        }
        else
        {
            ifi->emitSingleResponse(IFI_PICTURE, em._filename);
        }
    }

    void emitAnimation(Flow::EltMedia& em)
    {
        GrowString gs;
        gs.add('{');
        JSONWalker::addKey(gs, IFI_ANIMATE);
        gs.add('{');

        // name of animation.json
        JSONWalker::addKeyValue(gs, IFI_NAME, em._filename);

        bool haveAtlas = false;
        
        if (em._attr)
        {
            for (auto& e : *em._attr)
            {
                if (e.first == IFI_ATLAS) haveAtlas = true;
                JSONWalker::addKeyValue(gs, e.first, e.second);
            }
        }

        if (!haveAtlas)
        {
            // derive from base name
            JSONWalker::addKeyValue(gs, IFI_ATLAS,
                                    changeSuffix(em._filename, ".atlas"));
        }

        gs.add('}');
        gs.add('}');
        gs.add(0);
        LOG3(TAG "emit animation ", gs.start());
        ifi->emitResponse(gs.start());
    }
    
#endif

    struct ResInfo
    {
        pnode*  _pn;
        bool    _toScope;
        bool    _valid = true;  // resolved
        bool    _empty = false; // resolved to nothing?
        bool    _artScope = false;

        bool    resolved() const { return _valid && !_empty; }

        ResInfo(pnode* pn, bool toScope = false)
            : _pn(pn), _toScope(toScope) {}
        
    };

    void run(Flow::EltText* et)
    {
        OUTP(et->_text);
    }

    void run(Flow::EltCode* ec)
    {
        if (_eval)
        {
            Capture* args = _ctx->_lastCap.get();
            Capture* cp = (_eval)(ec->_code, args);
            if (cp)
            {
                outCap(*cp);
                delete cp;
            }
        }
        else
        {
            LOG1(TAG "run code, missing evaluator for ", *ec);
        }
    }

    bool run(Flow::EltCommand* c)
    {
        bool v = true;
        if (c->_parse)
        {
            ResInfo ri(c->_parse);
            ri._artScope = true;
                        
            resolve(ri);
            if (ri.resolved())
            {
                execInfo ei(c->_parse);

                if ((_runtypemask & run_choice) == 0)
                {
                    // XX hacky way
                    // we're in topflow emit result of
                    // command so it can be tested.
                    ei._output = true;
                }
                            
                ei._err = false; // no built in errors
                exec(ei);

                // retrieve manual break request from exec
                if (ei._break)
                {
                    v = false;
                    //LOG1("command had break ", textify(c->_parse));
                }

                clearBindings(c->_parse);
                updateScope();
            }

            if (!ri._valid)
            {
                ERR1("flow cannot resolve", textify(c->_parse));
            }
        }
        else
        {
            ERR1("Command has no parse ", c->toString());
        }
        return v;
    }

    bool run1(Flow::EltTerm* et)
    {
        // handle immediate eval of term element
        // return true if done, otherwise need to run it.
        
        bool done = true;
        
        Term* t = et->_term;
        if (t)
        {
            if (t->isObject())
            {
                // objects are not "run".
                // They just output themselves.
                OUTP(t);
            }
            else
            {
                if (et->_flags & Flow::ft_background)
                {
                    // signal to run background flow, but do nothing at this point
                    // do not initially mark as visited.
                    // set (term TICK)
                    _state.set(t->_name, TERM_TICK); // does not add if already
                }
                else if (et->_flags & Flow::ft_reset)
                {
                    resetTerm(t);
                }
                else
                {
                    // need to run term
                    done = false;
                }
            }
        }
        else
        {
            ERR1("unbound term", et->_name);
        }
        return done;
    }

    bool run(Flow::EltTerm* et)
    {
        bool v = true;
        if (!run1(et))
        {
            Term* t = et->_term;
            v = run(t);
        }
        return v;
    }

    void run(Flow::EltMedia* em)
    {
        if (_runtypemask & run_media)
        {
            // when we run media we send a request
#ifdef IFI_BUILD
            switch (em->_mType)
            {
            case m_image:
                emitPicture(*em);
                break;
            case m_audio:
                emitSound(*em);
                break;
            case m_animation:
                emitAnimation(*em);
                break;
            }
#endif                        
        }
        else
        {
            // otherwise just emit the filename
            OUTP(em->_filename);
        }
    }
    
    bool run(Flow::Elt* e)
    {
        // return false if we hit a manual return
        bool v = true;
        
        switch (e->_type)
        {
        case Flow::t_text:
            run((Flow::EltText*)e);
            break;
        case Flow::t_code:
            run((Flow::EltCode*)e);
            break;
        case Flow::t_command:
            v = run((Flow::EltCommand*)e);
            break;
        case Flow::t_term:
            v = run((Flow::EltTerm*)e);
            break;
        case Flow::t_media:
            run((Flow::EltMedia*)e);
            break;
        }
        return v;
    }

    bool run(Flow& f)
    {
        // return false if we hit a manual return
        bool v = true;
        for (auto i = f._elts.begin(); i != f._elts.end(); ++i) 
        {
            Flow::Elt* e = i;
            v = run(e);
            if (!v) break;
        }
        return v;
    }

    void _pushcap(uint rmask = -1)
    {
        CapStack* cs = new CapStack;
        cs->_prev = _out;
        cs->_rmask = _runtypemask;
        _out = cs;
        _runtypemask &= rmask; // masks combine
    }

    CapRef _popcap()
    {
        assert(_out);

        CapStack* cs = _out;
        _out = cs->_prev;
        _runtypemask = cs->_rmask;
        return CapRef(cs);
    }

    void outCap(const Capture& cap)
    {
        for (auto& e : cap._elts)
        {
            OUTP(e._term);
            OUTP(e._v);
            OUTP(e._s);
        }
    }

    struct Choice
    {
        string          _text; // cache textified select
        CapRef          _select;
        Selector*       _action;
        Reaction*       _reactor = 0;
        bool            _selected = false;
        bool            _nullmatch = false; // match for null list
        bool            _setmatch = false; // match for whole input set
        
        Choice(const CapRef& c, Selector* s) : _select(c), _action(s)
        {
            // enable special choice types
            if (_select)
            {
                const Capture& cap = _select->_cap;
                if (cap)
                {
                    if (cap.matchSimple(MATCH_NULL)) _nullmatch = true;
                    else if (cap.matchSimple(MATCH_THEM)) _setmatch = true;
                }
            }
        }
        
        Choice(Selector* s) : _action(s) {}
        Choice(const string& text, Reaction* r)
            : _text(text), _reactor(r)
        {
            assert(_reactor);
            _action = _reactor->_s;
        }

        bool specialMatch() const { return _nullmatch || _setmatch; }
        string id() const { return _reactor ? _reactor->id() : _action->id(); }
                
    };

    struct Choices
    {
        typedef std::string string;
        typedef std::vector<Choice>  ChoiceList;

        Choices(Term* t) : _t(t) {}

        Term*                _t;
        ChoiceList           _choices;
        
        // building choices
        bool                _valid = true;
        string              _headFlow;

        // null choices are commands, if we have any
        Selector*           _cmdChoice = 0;
        bool                _matching = false;

        string              _line;  // input
        int                 _ch = -1; // choice

        // invalid when a manual break occurs
        operator bool() const { return _valid; }

        bool present() const
        {
            return _valid && (!isEmpty() || _cmdChoice);
        }

        Choice& last()
        {
            assert(!_choices.empty());
            return _choices.back();
        }

        int size() const { return _choices.size(); }
        bool isEmpty() const { return _choices.empty(); }
    };

    void flush()
    {
        // emit the current capture
        string t = textify(_popcap()->_cap);
        _pushcap();
        if (t.size())
        {
            _emit(t);
            _emit((char)0); // force
        }
    }

#if defined(__EMSCRIPTEN__) && !defined(IFI_BUILD)
    string _getline()
    {
        return em_getline();
    }
    
#else    
    void _getline(char* buf, uint sz)
    {
        assert(sz);
        --sz; // space for terminator
        
        while (sz)
        {
            int c = getchar();
            if (!c || c == EOF || c == '\n') break;
            *buf++ = c;
            --sz;
        }
        *buf = 0;
    }

    string _getline()
    {
        char tbuf[256];
        _getline(tbuf, sizeof(tbuf));
        return tbuf;
    }
#endif    

    void setTermValue(Term* t, const var& v)
    {
        _state.setfn(t->_name, "=", v);
    }

    const var* getTermValue(Term* t)
    {
        return _state.getfn(t->_name, "=");
    }

#ifdef IFI_BUILD    
    void yield()
    {
        for (;;)
        {
            const char* r = ifi->getRequest();
            if (r)
            {
                LOG4(TAG "request ", r);

                // will handle all request types and call
                // `ifiCommand` for any command part
                ifih->handle(r);

                // ifiCommand sets this flag if request contained cmd
                if (cmdWaiting)
                {
                    cmdWaiting = false;
                    break; // fall through and handle line
                }
                    
            }
            else
            {
                if (ifi->_coop)
                {
                    assert(ifi->_pump);
                    (ifi->_pump)();
                }
                else
                {
                    LOG1(TAG, "request shutdown");
                    longjmp(_env_top, 1);
                }
            }
        }
    }

    bool buildChoiceJSON(GrowString& gs, Choices& c)
    {
        // return true if choice json has some content to emit
        bool res = false;
        
        gs.add('{');
        JSONWalker::addKey(gs, IFI_CHOICE);

        // always uses the nested syntax
        gs.add('{');

        if (c._headFlow.size())
            JSONWalker::addStringValue(gs, IFI_TEXT, c._headFlow);

        // even if no choices need to request text input
        res = c._cmdChoice != 0; 

        // request or deny text input
        JSONWalker::addBoolValue(gs, IFI_UI_TEXTINPUT, c._cmdChoice != 0);
        
        if (!c.isEmpty())
        {
            res = true;

            JSONWalker::toAdd(gs);
            JSONWalker::addKey(gs, IFI_CHOICE);
            gs.add('[');

            int cc = 0;
            for (auto& ci : c._choices)
            {
                JSONWalker::toAdd(gs);
                gs.add('{');
                JSONWalker::addStringValue(gs, IFI_TEXT, ci._text);

                // Commands within sub-json will not echo
                JSONWalker::toAdd(gs);
                JSONWalker::addKey(gs, IFI_CHOSEN);
                gs.add('{');
                JSONWalker::addKeyValue(gs, IFI_COMMAND, ++cc);
                gs.add('}');
                gs.add('}');
            }                
            gs.add(']');
        }

        gs.add('}'); // end subobject
        gs.add('}'); // end choice
        gs.add(0);

        //LOG3(TAG "choice json ", gs.start());
        return res;
    }
    
    void presentChoicesIFI(Choices& c)
    {
        GrowString gs;
        bool v = buildChoiceJSON(gs, c);
        
        // without choices, emit any headflow into the main text
        // otherwise it gets put into the choice box
        if (c.isEmpty())
        {
            OUTP(c._headFlow);
        }

        // emit the current capture
        flush();

        if (v)
        {
            // emit the choices or request command line
            ifi->emitResponse(gs.start());
        }

        yield();
    }
#endif // IFI_BUILD    

    void presentChoicesConsole(Choices& c)
    {
                // for console mode, we just emit head flow
        // then emit the choices as text
        OUTP(c._headFlow);

        // emit the current capture
        flush();

        _emit('\n');
                
        int cc = 0;
        for (auto& ci : c._choices)
        {
            char buf[16];
            sprintf(buf, "(%d) ", ++cc);
            _emit(buf);
            _emit(ci._text);
            _emit('\n');
        }
    }
    
    bool validateInput(Choices& c)
    {
        // store choice in c.ch
        // 0=> cmd
        // >0 => choice
        // <0 => invalid
        
        bool v = false;

        int nchoices = c.size();

        if (c._line.empty())
        {
            // blank line accepts single choice by default
            // even if cmd available.
            if (nchoices == 1)
            {
                c._ch = 1;  // choice = 1
                v = true; // accept
            }
        }
        else
        {
            // choices must be a number at the start
            // otherwise assume command input
            if (nchoices > 0 && u_isdigit(c._line[0]))
            {
                // prevent stoi overflow
                int ch = 0;
                if (c._line.length() < 10) ch = std::stoi(c._line);
                
                if (ch > 0 &&  ch <= (int)c.size())
                {
                    v = true;
                    c._ch = ch;
                }
                
            }
            else
            {
                if (c._cmdChoice)
                {
                    c._ch = 0; // command choice
                    v = true;
                }
                    
            }
        }
        return v;
    }

    void acceptInput(Choices& c)
    {
        c._line = toLower(_getline());
    }

    bool handleDebugCmd(Choices& c)
    {
        bool done = false;
#ifdef LOGGING        
        if (c._line[0] == '?')  // debugging
        {
            done = true;
            _printScope();
            _printResolutionScope();

            const char* p = c._line.c_str() + 1; // skip "?"
            string dcmd = firstWordOf(p);

            if (dcmd.size())
            {
                bool any = dcmd == "?";
                        
                // special case to examine reactions
                int cc = 0;
                for (auto& r : _ctx->_reactions)
                {
                    assert(r._reactor);
                    const string& host = r._s->_host->_name;
                    if (any || equalsIgnoreCase(dcmd, host))
                    {
                        bool aschoice = r._s->aschoice();
                                
                        _emit('\n');
                        _emit(++cc);
                        _emit(") ");
                        _emit(host);
                        _emit(':');
                        if (aschoice) _emit('=');
                        _emit(' ');
                        _emit(textify(r._reactor));
                    }
                }
                _emit('\n');
            }
        }
#endif  // LOGGING
        return done;
    }

    void requestNL()
    {
        // arrange for newline after choice and before subsequent
        // text, ONLY if there is text!
        _out->_cap._needNewline = true;
    }

    void handleChoice(Choices& c)
    {
        int ch = c._ch;
        
        assert(ch > 0 && ch <= c.size());
        
        LOG3(TAG "choice made ", ch);

        Choice& cc = c._choices[--ch];
        Selector* s = cc._action;
                
        // mark as chosen.
        if (s->once()) _state.set(cc.id());
                
        // arrange for newline after choice and before subsequent
        // text, ONLY if there is text!
        requestNL();

        if (cc._reactor)
        {
            // is a command choice
            assert(cc._reactor->_reactor);
            execInfo ei(cc._reactor->_reactor);
            if (prepareExecInfo(ei) && execValidate(ei))
            {
                execReaction(ei, *cc._reactor);
                updateIt(ei);
            }
        }
        else
        {
            // choices remember their last chosen choice text
            // also to support sticky choices
            setTermValue(c._t, cc._text);
            c._valid = run(s->_action);
        }
    }

    bool handleCmd(Choices& c)
    {
        bool done = false;

        LOG3(TAG "command ", c._line);

        requestNL();
        
        // try to parse command and execute
        pnode* ps = _pcom.parse(c._line);
        if (ps)
        {
            // create bindings
            // restrict input to resolution scope
            ResInfo ri(ps, true);
            resolve(ri);
            if (ri.resolved())
            {
                execInfo ei(ps);

                // perform resolution against reactor match
                ei._resolving = true;
                ei._internalOps = false; // prevent internal "put"
                            
                done = exec(ei);

                if (ei._break)
                {
                    c._valid = false;
                    //LOG1("parse command had break ", line);
                }
                else
                {
                    if (done)
                    {
                        updateIt(ei);
                        
                        // and run any action after command
                        c._valid = run(c._cmdChoice->_action);
                    }
                    else
                    {
                        if (_errorNocando)
                        {
                            c._valid = run(_errorNocando);
                            done = true;
                        }
                    }
                }
            }
            else
            {
                // no such object
                if (_errorNosuch)
                {
                    c._valid = run(_errorNosuch);
                    done = true;
                }
            }

            // will also delete bindings
            delete ps;
        }
        else
        {
            if (_errorSyntax)
            {
                c._valid = run(_errorSyntax);
                done = true;
            }
        }
        return done;
    }
    
    void runChoices(Choices& c)
    {
        // ch > 0 will be a choiceP
        // ch == 0 is a cmd.

        if (c._ch > 0)
        {
            handleChoice(c);
        }
        else
        {
            // handle command
            
            assert(c._cmdChoice);

            if (!handleDebugCmd(c))
            {
                handleCmd(c);
            }
        }
    }

    var _evalEnodeTerm(eNodeCtx* ctx, const char* name)
    {
        assert(name && *name);
        var v = _state.test(name) ? 1 : 0;
        return v;
    }

    var _evalEnodeFunction(eNodeCtx*, enode* e)
    {
        var v;
        assert(e && e->e_name); // lead with a term 
        string a = e->_v.rawString();

        e = e->_next; // arg1
        if (e)
        {
            string b = e->_v.toString();
            b = wordStem(b);
            const var* vp = _state.getfn(a, b);
            if (vp) v = vp->copy();
            LOG3("eval node function ", a << ' ' << b << " = " << v);
        }
        return v;
    }

    var evalEnode(enode* en)
    {
        eNodeCtx ectx(en);
        
        using namespace std::placeholders;  
        ectx._termFn =
            std::bind(&Strandi::_evalEnodeTerm, this, _1, _2);

        ectx._fnFn =
            std::bind(&Strandi::_evalEnodeFunction, this, _1, _2);
        
        return ectx.eval(en);
    }
    
    bool checkSelectorCond(Selector* s)
    {
        bool v = true;
        if (s->_cond)
        {
            // test whether we have visited or not visited nodes
            v = false;

            Flow::Elt* e = s->_cond.firstElt();
            if (e && e->isCond())
            {
                Flow::EltCond* ec = (Flow::EltCond*)e;
                v = evalEnode(ec->_cond).isTrue();
            }
        }

        return v;
    }

    GenState* getGenState(const string& name)
    {
        GenState* gs = 0;
        const var* v = _state.getfn(name, GENSTATE_KEY);
        if (v)
        {
            assert(v->isBlob());
            gs = (GenState*)v->_b;            
        }
        return gs;
    }

    void setGenState(const string& name, GenState* g)
    {
        // consumes `g`
        assert(g);

        // will delete g if not used
        _state.setfn(name, GENSTATE_KEY, g);  // var(blob)
    }

    int choiceForSelector(Choices& c, int seq)
    {
        assert(seq < c._t->_selectors.size());
        
        Selector* s = c._t->_selectors[seq];
                        
        // find choice for `seq` if any
        int i = 0;
        for (auto& ci : c._choices)
        {
            if (ci._selected)
            {
                if (ci._action == s) return i;
                ++i;
            }
        }
        return -1;
    }

    int handleGenTypes(Choices& c, int n)
    {
        // handle types that can have state
        // shuffle, nonrandom, sequence
        //
        // these types can also mutate into random (possibly others)
        // so we can still get here even though we are random.

        int ch;

        int ns = c.size();

        // we should only get here if we have a choice for all selectors
        assert(ns == c._t->_selectors.size());

        GenState* g1 = getGenState(c._t->_name);
        GenState* g2;
        if (g1)
        {
            // copy because the timeline needs to have each state.
            // unless we dont need state anymore (eg random)
            g2 = g1->copyIf();
        }
        else
        {
            // will be the initial state, unless we don't need it
            g2 = new GenState(this, c._t->_rtype, ns);
        }

        if (g2->_type == Term::t_sequence)
        {
            // a sequence will walk through the possibilities and
            // park on the end

            // the state stores the next sequence# to try
            // seq runs through the selectors not the available choices
            // as these can change.
            int seq = 0;
            if (g1) seq = g1->_seq;

            assert(ns > 1); // otherwise wont need state at all

            // find first available choice from `seq`
            
            while (seq < ns)
            {
                ch = choiceForSelector(c, seq);
                if (ch >= 0) break;
                ++seq;
            }

            // seq is the new current choice or seq == ns if none
            if (seq >= ns)
            {
                // find last valid repeat
                int i = 0;
                for (auto& ci : c._choices) if (ci._selected) ch = i++;

                assert(ch >= 0); // because we have at least one available
            }

            if (g1 && g1->finished())
            {
                // were already finished, so no need for new state
                delete g2; g2 = 0;
            }
            else
            {
                g2->_seq = seq; // assign current, might be == ns ie fin
                g2->choose(); // bump for next time
            }
            
        }
        else
        {
            // choose, out of all selectors, but not necessarily selected.
            for (int i = 0; i < ns*2; ++i)   // *2 to allow for non-random
            {
                // keep choosing until get one selected.
                int si = g2->choose();
                ch = choiceForSelector(c, si);
                if (ch >= 0) break;
            }

            if (ch < 0)
            {
                // should not happen, above should find selected eventually
                LOG1("handleGenTypesm, failed to find selector ", c._t->_name);

                // fallback to last valid in emergency
                int i = 0;
                for (auto& ci : c._choices) if (ci._selected) ch = i++;
                assert(ch >= 0);
                
            }
        }

        if (g2 && g2 != g1)
        {
            // shuffle and sequence can change state
            if (g2->finished()) g2->convertTo(c._t->_rtypenext);

            setGenState(c._t->_name, g2); // consumes g2
        }
        return ch;
    }

    bool runGeneratorAction(Choices& c)
    {
        // return true if done something
        
        int nchoices = 0;

        for (auto& ci : c._choices)
            if (ci._selected) ++nchoices;

        if (!nchoices)
        {
            // no matches, then we choose from fallbacks
            // which are those choices with empty flows
            for (auto& ci : c._choices)
            {
                // evaluated flow, but is empty?
                bool v = !ci.specialMatch() &&
                    ci._select && ci._select->_cap.empty();
                
                ci._selected = v;
                if (v) ++nchoices;
            }
        }
        
        // still zero? then nothing to do
        if (!nchoices) return false;

        Term::RType rtype = c._t->_rtype;

        // when we're matching we have some of the selectors as choices
        // and some of those are selected
        // when we're not matching, we have all the selectors as choices
        // but those failing conditional are not selected.
        if (c._matching)
        {
            assert(rtype == Term::t_random);
            rtype = Term::t_random;
        }
        
        int ch = 0;

        if (nchoices > 1) switch (rtype)
        {
        case Term::t_random:
            // this never has state
            ch = _rand.gen32() % nchoices;
            break;
        case Term::t_first:
            // ch = 0 selects first selected
            break;
        default:
            ch = handleGenTypes(c, nchoices);
            break;
        }

        assert(ch >= 0 && ch < nchoices);

        const Choice* cp = 0;

        // find the ch'th selected choice
        int ri = 0;
        for (auto& ci : c._choices)
        {
            if (ci._selected)
            {
                cp = &ci;
                if (!ch) break;
                --ch;
            }
            ++ri;
        }
        
        assert(cp);

        // ri is the raw index, not just those selected
        //_ctx->_lastGen = ri; 

        // when we're not matching the select flow is part of the output
        if (!cp->_select)
            c._valid = run(cp->_action->_text);

        // but we always run the action
        if (c._valid)
            c._valid = run(cp->_action->_action);

        return true;
    }

    bool runGenerator(Choices& c)
    {
        // return false for stack break
        
        // top flow is input for matching generators
        CapRef topflowcap;

        // run top flow and keep result, this becomes
        // an input selector.
        if (c._t->_topflow)
        {
            c._matching = true;

            // mask choices in topflow
            // prevent choices being run and instead
            // returns the previous choice made if any
            // prevent media, emit as string instead.
            int m = ~run_choice & ~run_media;
            _pushcap(m);
            c._valid = run(c._t->_topflow);
            
            // NB: result can be empty
            topflowcap = _popcap();
        }

        // install results of topflow as lastcap
        LastCap::Tmp ltmp(_ctx->_lastCap, &topflowcap->_cap);
        
        bool stickpushed = false;
        if (c)
        {
            if (c._t->sticky())
            {
                _pushcap();
                stickpushed = true; // remember to pop
            }
            
            // run head flow after any topflow
            // NB: code within headflow can access topflow via lastcap
            c._valid = run(c._t->_flow);
        }

        if (c)
        {
            // if we are matching:
            // one or more matches => pick random match
            // no matches => collect null tags and pick randomly
            // no matches & no null tags => no output.

            if (c._matching)
            {
                // insist on matching terms being random (for now)
                assert(c._t->_rtype == Term::t_random);
                
                // run all choice select flows and capture them
                // for later matching
                for (auto s : c._t->_selectors._selectors)
                {
                    if (checkSelectorCond(s)) // ignore failed conditional
                    {
                        // when matching we must run all selector flows
                        // in order to make match.
                        _pushcap();
                        c._valid = run(s->_text);
                        Choice ch(_popcap(), s);
                        if (!c) break;
                        c._choices.push_back(ch);
                    }
                }

                if (c)
                {
                    assert(topflowcap);

                    if (!topflowcap->_cap)
                    {
                        // input flow yielded nothing
                        // do we have a null match choice?
                        for (auto& ci : c._choices)
                            ci._selected = ci._nullmatch;
                        runGeneratorAction(c);
                    }
                    else
                    {
                        // if we have a setmatch, then feed all in one
                        // ie a match for a whole set.
                        bool setmatch = false;
                        for (auto& ci : c._choices)
                        {
                            ci._selected = ci._setmatch;
                            if (ci._selected) setmatch = true;
                        }

                        if (setmatch)
                        {
                            // already installed as lastcap
                            //LastCap::Tmp ltmp(_ctx->_lastCap, &topflowcap->_cap);
                            runGeneratorAction(c);
                        }
                        else
                        {
                            // filter:
                            // feed the topflow to the matcher one at a time
                            for (auto& e : topflowcap->_cap._elts)
                            {
                                // install each individually as lastcap
                                Capture icap;
                                icap.add(e);
                                LastCap::Tmp ltmp(_ctx->_lastCap, &icap);

                                //LOG1("matching topflow elt ", e.toStringTyped());
                        
                                for (auto& ci : c._choices)
                                {
                                    ci._selected = false;
                                    if (!ci.specialMatch())
                                    {
                                        //LOG1("matching topflow elt ", e.toStringTyped() << " to " << ci._select->_cap.toString());
                                        ci._selected = ci._select->_cap.match(e);
                                    }
                                }
                    
                                // run one of the matches choices OR
                                // one of the null flow choices
                                runGeneratorAction(c);
                                if (!c) break;
                            }
                        }
                    }
                }
            }
            else
            {
                // not matching, so generating
                for (auto s : c._t->_selectors._selectors)
                {
                    // when automatically choosing we run the selector
                    // flow later
                    // add all selectors even if fails condition, but
                    // mark as not selected.
                    Choice ch(s);
                    ch._selected = checkSelectorCond(s);
                    c._choices.emplace_back(ch);
                }
                runGeneratorAction(c);
            }
        }
        
        if (stickpushed)
        {
            assert(c._t->sticky());
            
            CapRef cs = _popcap();

            if (c)
            {
                // save sticky
                setTermValue(c._t, cs->_cap.toVar());

                // dub onto base cap
                outCap(cs->_cap);
            }
        }

        bool res = c;

        // NB: lastcap is still in scope for the postflow
        if (res)
            res = run(c._t->_postflow);

        return res;
    }

    bool selectorShown(Selector* s, Reaction* reactor = 0)
    {
        // should we show this selector?
        bool show = true;
        
        // if already seen, dont show
        if (s->once())
        {
            string id = reactor ? reactor->id() : s->id();
            if (_state.test(id)) show = false;
        }

        // check conditional
        if (show) show = checkSelectorCond(s);
        return show;
    }

    bool suppressElevatedChoice(Selector* s)
    {
        bool suppress = !s->_action;
        if (!suppress && s->_action.size() == 1)
        {
            // if we have a single action
            auto e = s->_action.firstElt();
            assert(e);
            if (e->isTerm())
            {
                Term* t1 = ((Flow::EltTerm*)e)->_term;
                assert(t1);  // otherwise not bound
                if (t1->isChoice())
                {
#if 1
                    // simple version that just looks at choice conditions
                    int cc = 0;
                    for (auto s : t1->_selectors._selectors)
                    {
                        // only count non-terminals
                        if (!s->terminal() && selectorShown(s)) ++cc;
                    }

                    if (!cc) suppress = true;
#else
                    // this version actually runs subsequent flow,
                    // then reverts it to see if something happens
                    Choices ch(t1);
                    // revert any state changes whilst inspecting flow
                    Timeline::Mark m = _state.getMark();
                    prepareChoices(ch);
                    _state.clearToMark(m);
                    if (!ch.present()) suppress = true;
#endif                    

                    if (suppress)
                    {
                        LOG3("suppressing elevated choice reactor ", t1->_name);
                    }
                }
            }
        }
        return suppress;
    }
    
    bool prepareChoice(Choices& c, Selector* s, Reaction* reactor)
    {
        // a `reactor` is given if this is a command
        // elevated to choice
        
        bool r = false; // return true if choice was added
        
        if (selectorShown(s, reactor))
        {
            if (s->_text)
            {
                if (s->_isReactor)
                {
                    // this is a command masquerading as a choice

                    string ctext;
                    pnode* pn = 0;

                    assert(reactor);

                    pn = reactor->_reactor;
                    
                    assert(pn);

                    if (!suppressElevatedChoice(s))
                    {
                        // convert the bound reaction into plain text
                        ctext = textifyFancy(pn);
                        //LOG3("creating choice ", ctext << " from " << pn->toStringStruct());
                        if (!ctext.empty())
                        {
                            // capitalise start, if needed.
                            ctext[0] = u_toupper(ctext[0]);
                            c._choices.emplace_back(Choice(ctext, reactor));
                            r = true;
                        }
                    }
                }
                else
                {
                    // collect choice texts
                    _pushcap();
                    c._valid = run(s->_text);
                    CapRef ccap = _popcap();

                    if (c && ccap->_cap)
                    {
                        c._choices.emplace_back(Choice(ccap, s));
                        r = true;
                    }
                }
            }
            else
            {
                // we have a choice with no text
                // is a command
                c._cmdChoice = s;
            }
        }
        return r;
    }

    void prepareChoices(Choices& c)
    {
        Term* t = c._t;
        assert(t);

        // first process all choices that are not fillers
        int terminal = 0;
        for (auto s : t->_selectors._selectors)
        {
            if (s->terminal())
            {
                // how many terminals?
                if (selectorShown(s)) ++terminal;
            }
            else if (!s->filler())
            {
                prepareChoice(c, s, 0);
                if (!c) break;
            }
        }
        
        if (c)
        {
            // add in commands elevated to choice
            if (t->cmdChoices())
            {
                // add in any `aschoice` reactions
                // these are object reactions that appear as choices
                for (auto& r : _ctx->_reactions)
                {
                    Selector* s = r._s;
                    if (s->aschoice())
                    {
                        assert(s->_isReactor);
                        prepareChoice(c, s, &r);
                        if (!c) break;
                    }
                }
            }
        }

        if (c)
        {
            // pad with filler choices?
            for (auto s : t->_selectors._selectors)
            {
                if (t->_idealChoiceCount - (c.size() + terminal) <= 0) break;
                
                if (s->filler())
                {
                    prepareChoice(c, s, 0);
                    if (!c) break;
                }
            }
        }

        if (c && terminal)
        {
            // add terminals
            for (auto s : t->_selectors._selectors)
            {
                if (s->terminal())
                {
                    prepareChoice(c, s, 0);
                    if (!c) break;
                }
            }
        }
        
        // flatten choice text
        for (auto& ci : c._choices)
        {
            if (ci._text.empty())
                ci._text = textify(ci._select->_cap);
        }
    }

    void runChoiceHead(Choices& c)
    {
        // choice does not run the headflow (nor tail)
        // if there are no actual valid choices
        // TODO: can choices have topflow? what does this mean?
        
        // run head flow ONCE we know we have choices
        _pushcap();
        c._valid = run(c._t->_flow);  // collect manual break
        CapRef ccap = _popcap();
        if (c) c._headFlow = textify(ccap->_cap);
    }

    void runChoice2(Choices& c)
    {
        // present input until accepted
        for (;;)
        {
            
#ifdef IFI_BUILD
                presentChoicesIFI(c);
#else
                presentChoicesConsole(c);
#endif

                acceptInput(c);
                if (validateInput(c)) break;
            
        }
    }

    bool runChoice3(Choices& c)
    {
        // return false for stack break
        runChoices(c);
        
        updateScope();

        if (c)
        {
            c._valid = run(c._t->_postflow);
        }
        return c;
    }
    
    struct RuntimeTermStack
    {
        Term*               _term;
        bool                _jmp = false;
        RuntimeTermStack*   _prev;
        RuntimeTermStack(Term* t) : _term(t) {}
    };

    RuntimeTermStack*       _termStack = 0;

    void _runTick()
    {
        ++_time;
        assert(_tick);

        LOG4(TAG "tick ", _time);

        // previous ticks are not retracted. Just keep adding
        // tick values to mark time.
        _state.set(_tick->_name, "=", _time);

        // run background flows

        // find all terms with (term, TICK) except those negated
        Timeline::Strings tags;
        _state.getds(TERM_TICK, tags);

        for (auto s : tags)
        {
            Term* t = Term::find(s->c_str());
            if (t)
            {
                LOG4(TAG "run background ", *s);
                run(t);
            }
        }
        
    }

    bool _runSpecial(Term* t)
    {
        // return true iff handled
        bool v = false;
        
        assert(t->isGenerator());
        
        if (t->_name == TERM_LAST)
        {
            v = true;
            if (_ctx->_lastCap)
                outCap(*_ctx->_lastCap.get());
        }
        else if (t->_name == TERM_IT)
        {
            v = true;
            Term* it = _ctx->getIt();
            if (it)
            {
                OUTP(it);
            }
        }
        else if (t->_name == TERM_THAT)
        {
            v = true;
            Term* it = _ctx->getThat();
            if (it)
            {
                OUTP(it);
            }
        }
        else if (t->_name == TERM_TICK)
        {
            v = true;
            _runTick();
        }
        else if (t->_name == TERM_VERSION)
        {
            v = true;
            OUTP(var(VERSION " " BUILD_VER));
        }
        /*
        else if (t->_name == TERM_LASTGEN)
        {
            v = true;
            OUTP(var(_ctx->_lastGen));
        }
        */
        return v;
    }

    bool _run1(Term* t)
    {
        // return true if done

        // handle terms that can be evaluated immediately
        // such as special terms and sticky
        
        bool done = t->isGenerator() && _runSpecial(t);
        if (!done)
        {
            done = t->isChoice() && !(_runtypemask & run_choice);
            if (done || t->sticky())
            {
                // we're sticky
                // or a choice but not allowed to run
                // if stuck, no postflow
                const var* v = getTermValue(t);
                if (v)
                {
                    // dont run headflow if stuck!
                    // return the sticky value
                    Capture cap;
                    cap.fromVar(*v);
                    outCap(cap);
                    done = true;
                }
                else
                {
                    // sticky but not yet stuck, so not done yet
                    if (t->sticky()) done = false;
                }
            }
        }
        return done;
    }

    bool _run2(Term* t)
    {
        // return false for break
        bool v = true;

        // run a term and selectors
        // return false for stack break
        Choices choices(t);
        if (t->isGenerator())
        {
            v = runGenerator(choices);
        }
        else if (t->isChoice())
        {
            prepareChoices(choices);
            if (choices.present())
            {
                runChoiceHead(choices);
                if (choices.present())
                {
                    runChoice2(choices);
                    v = runChoice3(choices);
                }
            }
        }
        return v;
    }

    bool _run(Term* t)
    {
        // return false for break

        bool v = true;
        if (!_run1(t)) v = _run2(t);
        return v;
    }

    void resetTerm(Term* t)
    {
        // release all state from `t`
        // Does this also reset the term value (eg sticky)??

        //LOG1("Resetting term ", t->_name);
        
        // remove from background flow
        // clear (term TICK) if present
        _state.clear(t->_name, TERM_TICK);

        // clear the visit marker, if present
        _state.clear(t->_name);

        // reset generate state if necessary
        GenState* g1 = getGenState(t->_name);
        if (g1)
        {
            // not all gens have state
            GenState* g2 = g1->copyIf();
            if (g2 != g1)
            {
                if (g2->reset()) setGenState(t->_name, g2);
                else delete g2; // reset did not change state
            }
        }
    }
    
    bool run(Term* t)
    {
        // First see if the term we want to run is already on the
        // runtime stack
        RuntimeTermStack* rt = _termStack;
        while (rt)
        {
            if (rt->_term == t)
            {
                //LOG1("initiate break, returning to ", t->_name);
                
                // yes! initiate a manual return
                // this will cause all callers to return
                rt->_jmp = true; // indicate the jump point

                // signal return from stack
                return false;
                
            }
            rt = rt->_prev;
        }

        // keep a runtime stack of elements
        RuntimeTermStack    rtElt(t);
        rtElt._prev = _termStack;
        _termStack = &rtElt;

        bool v;

        for (;;)
        {
            v = _run(t);
        
            // mark as visited at the end
            _state.set(t->_name);

            if (v) break;

            // we're returning from stack
            // are we at the top?
            if (!rtElt._jmp) break; // no

            // at the top
            // we've been unwinding the stack and want to jump
            // to this term.
            rtElt._jmp = false; // reset

            // emit what we have in case we are in a loop
            flush();
            
            //LOG1("manual break, returned to term ", t->_name);

            //loop back to term
        }

        assert(!rtElt._jmp);

        // pop runtime term stack
        _termStack = rtElt._prev;

        return v;
    }

    //////////////////

    static void concatm(TermList& l1, TermList& l2)
    {
        // put things in l2 not in l1 into l1
        for (auto t : l2)
            if (!contains(l1, t)) l1.push_back(t);
    }

    static void concata(TermList& l1, TermList& l2)
    {
        // concat without testing for duplicates, destroying l2
        l1.splice(l1.end(), l2);
    }

    static void concatc(TermList& l1, TermList& l2)
    {
        // concat by copying l2 not testing for dups
        for (auto t : l2) l1.push_back(t);
    }

    static void cliplist(TermList& l1, TermList& l2)
    {
        // remove items from l1 not in l2
        for (auto it = l1.begin(); it != l1.end();)
        {
            if (!contains(l2, *it)) it = l1.erase(it);
            else ++it;
        }
    }

    static void subtractlist(TermList& l1, TermList& l2)
    {
        // remove items from l1 in l2
        for (auto it = l1.begin(); it != l1.end();)
        {
            if (contains(l2, *it)) it = l1.erase(it);
            else ++it;
        }
    }

    static int partialList(TermList& l1, TermList& l2)
    {
        // how many of l1 are in l2
        int cc = 0;
        for (auto t : l1) if (contains(l2, t)) ++cc;
        return cc;
    }
        

    bool restrictToIdeal(TermList& l1)
    {
        // try to reduce list by relevancy
        // false if reduced non-empty list to empty
        
        if (l1.empty()) return true; // nothing to do
        
        if (partialList(l1, _ctx->_scope))
        {
            // some are in interactive scope
            cliplist(l1, _ctx->_scope);
            LOG4("restrict to ideal interactive: ", textify(l1));
            
        }
        else if (partialList(l1, _ctx->_resolutionScope))
        {
            // some are directly in resolution scope
            cliplist(l1, _ctx->_resolutionScope);
            LOG4("restrict to ideal resolution: ", textify(l1));
        }
        else
        {
            // remove any member of `l1` not in resolution scope
            // or a subtype of some member of resolution scope
            for (auto it = l1.begin(); it != l1.end();)
            {
                if (!subtype(*it, _ctx->_resolutionScope)) it = l1.erase(it);
                else ++it;
            }
        }
        return !l1.empty();
    }

    static bool subset(const TermList& a, const TermList& b)
    {
        // is `a` a subset of `b` ?
        for (auto t : a)
            if (!contains(b, t)) return false;
        
        return true;
    }

    static int subtype(Term* t, const TermList& b)
    {
        // is t in b or a subtype of some member in b
        // return subtype parent distance, 0 if not a subtype
        
        if (contains(b, t)) return 1; // like a parent?

        int di = 0;
        for (auto bi : b)
        {
            di = isDistance(t, bi); // is bi a parent of t? 
            if (di) break; // yes
        }
        
        // if !di, t does not have a parent in b
        // or return parent distance
        return di;
    }
    
    static int subtypes(const TermList& a, const TermList& b)
    {
        // are elements of a, all ether in b or subtypes of some member in b
        // return 0 if fail
        // return subtype parent distance
        // NB: if a empty, return 0
        
        int maxd = 0;
        for (auto t : a)
        {
            int di = subtype(t, b);
            if (!di) return 0; // t does not have a parent in b, fail

            // collect greatest parent distance for all in a
            if (di > maxd) maxd = di;
        }
        
        return maxd;
    }

    void subPropTerms(TermList& tl, Term* p, const string& prop)
    {
        // all X, X prop p
        Timeline::Strings ss;
        _state.getdsv(prop, p->_name, ss);
        for (auto i : ss)
        {
            Term* ti = Term::find(*i);
            assert(ti);
            tl.push_back(ti);
        }
    }

    void subPropTermsRec(TermList& tl, Term* p, const string& prop)
    {
        // All X, X prop p, recursive
        TermList t1;
        subPropTerms(t1, p, prop);
        for (auto i : t1)
        {
            if (!contains(tl, i))
            {
                tl.push_back(i);
                subPropTermsRec(tl, i, prop);
            }
        }
    }

    void subPropTermsSet(TermList& tl, TermList& a, const string& prop)
    {
        // all things X, where there is y in A, (X prop y)
        Timeline::Rel rel;
        _state.getRel(prop, rel);
        for (auto t : a)
        {
            for (auto e : rel)
            {
                if (e->_val == t->_name)
                {
                    Term* ti = Term::find(e->_tag);
                    if (ti && !contains(tl, ti)) tl.push_back(ti);
                }
            }
        }
    }

    Term* propTerm(Term* p, const string& prop)
    {
        // thing Y, p prop Y, assuming prop is a function
        Term* t = 0;
        const var* v = _state.getfn(p->_name, prop);
        if (v && v->isString())
        {
            t = Term::find(v->rawString());            
        }
        return t;
    }

    void propTerms(TermList& tl, Term* p, const string& prop)
    {
        // all things Y, p prop Y
        Timeline::Vars vs;
        _state.getset(p->_name, prop, vs);
        for (auto i : vs)
        {
            if (i->isString())
            {
                Term* ti = Term::find(i->rawString());
                if (ti) tl.push_back(ti);
            }
        }
    }

    /*
    void propTermsSet(TermList& tl, TermList& a, const string& prop)
    {
        // all things Y, where there is x in A, (x prop Y)
        Timeline::Rel rel;
        _state.getRel(prop, rel);
        for (auto e : rel)
        {
            for (auto t : a)  // do we have t in A also in rel
            {
                if (e->_tag == t->_name)
                {
                    if (e->_val.isString())
                    {
                        Term* ti = Term::find(e->_val.rawString());
                        if (ti && !contains(tl, ti)) tl.push_back(ti);
                    }
                    break;
                }
            }
        }
    }
    */
    
    void subInTerms(TermList& tl, Term* p)
    {
        subPropTerms(tl, p, PROP_IN);
    }

    void subOnTerms(TermList& tl, Term* p)
    {
        subPropTerms(tl, p, PROP_ON);
    }

    void subInTermsRec(TermList& tl, Term* p)
    {
        subPropTermsRec(tl, p, PROP_IN);
    }

    Term* inTerm(Term* p)
    {
        // thing Y, p in Y, assuming p is only in ONE thing
        return propTerm(p, PROP_IN);
    }

    void inTerms(TermList& tl, Term* p)
    {
        // all things Y, p in Y
        // use when things can be in multiple things.
        propTerms(tl, p, PROP_IN);
    }

    static bool is(Term* a, Term* p)
    {
        // is p a parent of a
        TermList tl;
        a->getParents(tl);
        return contains(tl, p);
    }

    static int isDistance(Term* a, Term* p) 
    {
        // is p ultimately a parent of a, return distance
        // 0 => not a parent
        // n => n steps away, eg 1 is immediate parent
        
        TermList tl;
        a->getParents(tl);
        if (contains(tl, p)) return 1;

        // recursive
        for (auto p1 : tl)
        {
            int v = isDistance(p1, p);
            if (v) return v + 1;
        }

        return 0;
    }

    static bool iss(Term* a, Term* p)
    {
        // is p ultimately a parent of a
        return isDistance(a, p) > 0;
    }

    bool isPhysical(Term* a)
    {
        if (!a || !_thing) return false; // failsafe
        return iss(a, _thing);
    }

    void calculateAll(TermList& tl, Term* p)
    {
        // `p` is usually player
        // all things directly in location of p except p
        
        tl.clear();

        // immediate parent "in"
        TermList parents;
        inTerms(parents, p); 

        // add all things in parent
        for (auto a : parents)
        {
            TermList pl;

            // NB: all does not include things inside things
            subInTerms(pl, a); // will include p

            for (auto t : pl)
                if (t != p && !contains(tl, t)) tl.push_back(t);
        }
    }

    void calculateScope(TermList& tl, TermList& seed)
    {
        // calculate scope for p (usually player)
        // the scope is:
        // all things (recursively) in the thing P is in.
        // ensure p is included.
        tl.clear();

        for (auto t : seed)
        {
            // add all things in seeds
            subInTermsRec(tl, t); 

            // include seed objects
            if (!contains(tl, t)) tl.push_back(t);
        }

        // ensure player is in scope
        assert(_player);
        if (!contains(tl, _player)) tl.push_back(_player);

        // ensure the location of player is in scope
        Term* loc = inTerm(_player);
        if (loc && !contains(tl, loc)) tl.push_back(loc);

        // add all things "on" things in scope to scope.
        TermList ons;
        subPropTermsSet(ons, tl, PROP_ON);
        concatm(tl, ons);

    }

#define TL_TAKE(_a, _b)                     \
{                                           \
    (_a)->_binding = (_b)->_binding;        \
    (_b)->_binding = 0;                     \
}

#define TL_TAKECONC(_a, _b)                 \
{                                           \
    _a->_binding->merge(*_b->_binding);     \
    delete pl->_binding;                    \
    pl->_binding = 0;                       \
}

    void clearBindings(pnode* pn)
    {
        // delete all bindings in pn and below
        while (pn)
        {
            delete pn->_binding;
            pn->_binding = 0;
            
            if (pn->_type != nodeType::p_value)
                clearBindings(pn->_head);                
            
            pn = pn->_next;
        }
    }

    bool resolvePronoun(pnode* pn, TermList& tl)
    {
        assert(pn && pn->_type == nodeType::p_pronoun);
        const Word* w = pn->_word;

        bool ok = false;

        // XX for now map him/her -> it. 
        if (w == _pcom._it || *w == PRON_HIM || *w == PRON_HER)
        {
            ok = true;
            Term* t = _ctx->getIt();
            if (t) tl.push_back(t);
        }
        else if (w == _pcom._that)
        {
            ok = true;
            Term* t = _ctx->getThat();
            if (t) tl.push_back(t);
        }
        else if (*w == PRON_HERE)  // literal
        {
            assert(_player);
            ok = true;
            inTerms(tl, _player);
        }
        else if (*w == PRON_ALL)
        {
            assert(_player);
            ok = true;
            calculateAll(tl, _player);
        }
        return ok;
    }

    bool resolveNoun(pnode* pn, pnode* pa, TermList& tl)
    {
        // pn = noun
        // pa = (a a a)
        // pa can be null.

        bool v = false;
        assert(pn && pn->_type == nodeType::p_noun);
        assert(!pa || pa->_type == nodeType::p_adjs);
        assert(pn->_word);
        
        if (pn->_word->isFlow())
        {
            // We have a flow term inside a noun phrase
            Term* t = Term::find(pn->_word->_text);
            if (t)
            {
                assert(t->isGenerator());

                v = true; // resolved to flow term

                // we expect the flow to evaluate to terms
                _pushcap();
                run(t);
                CapRef cs = _popcap();

                if (cs->_cap)
                {
                    for (auto& e : cs->_cap._elts)
                    {
                        if (e._term) tl.push_back(e._term);
                        else
                        {
                            ERR2("WARNING: non-term in parse flow term", pn->_word->_text, textify(cs->_cap));
                            
                        }
                    }
                }
            }
            else
            {
                ERR1("unknown flow term in parse", pn->_word->_text);
            }
        }
        else
        {
            // match noun with adjectives
            v = Term::matchName(tl, pn, pa);
        }
        return v;
    }

    bool resolveScope(Binding& b, scopeType st)
    {
        // false if restricting caused list to empty
        
        bool r = true;

        int n = b.size();

        if (n > 0)
        {
            if (st)
            {
                LOG4("Resolving scope: ", st << ", " << textify(b));
            }
        
            switch (st)
            {
            case Word::scope_any_one:
                // pick one!
                if (n > 1)
                    b._terms.erase(++b._terms.begin(), b._terms.end());
                break;
            case Word::scope_the_one:
                // XXX
                break;
            case Word::scope_here:
                // restrict to interactive scope
                cliplist(b._terms, _ctx->_scope);
                break;
            case Word::scope_have:
                {
                    assert(_player);
                    TermList inv;
                    subInTerms(inv, _player); // directly carried
                    cliplist(b._terms, inv);  // reduce to those carried
                }
                break;
            case Word::scope_here_nothave:
                // interactive scope but not carried
                {
                    assert(_player);
                    TermList inv;
                    subInTermsRec(inv, _player);

                    TermList sc;
                    concatc(sc, _ctx->_scope); // copy
                    subtractlist(sc, inv); // remove those carried
                    cliplist(b._terms, sc);  // reduce to this set
                }
                break;
            }

            
            r = !b.empty();
        }
        return r;
    }

    bool resolvePossession(Binding& b, pnode* tail)
    {
        bool v = true;
        int n = b.size();  
        if (n > 0) // have something to resolve?
        {
            while (tail && !b.empty())
            {
                LOG4("resolving from ", textify(b));

                Binding* u = tail->_binding;
                if (u)
                {
                    // restrict b to in or on u.
                    TermList tl;
                    for (auto t : u->_terms)
                    {
                        TermList tin, ton;
                        subInTerms(tin, t);
                        subOnTerms(ton, t);
                        
                        concatm(tl, tin);
                        concatm(tl, ton);
                    }
                    cliplist(b._terms, tl);

                }
                else b.clear();

                LOG4("resolving to ", textify(b));
                                    
                tail = tail->_next;
            }

            v = !b.empty();
        }
        return v;
    }

    void resolve(ResInfo& ri)
    {
        // if `scope` restrict objects to scope.
        pnode* pn = ri._pn;
        while (pn && ri._valid)
        {
            bool down = true;
            bool ok = true;

            TermList tl;
            
            // on way down
            switch (pn->_type)
            {
            case nodeType::p_anoun:
                {
                    assert(pn->_head);
                    assert(!pn->_binding);
                    
                    ok = resolveNoun(pn->_head, pn->_head->_next, tl);
                    if (ri._toScope) ok = restrictToIdeal(tl);

                    if (ok)
                    {
                        if (tl.empty()) ri._empty = true;
                        else pn->_binding = new Binding(tl);
                    }
                    
                    // no need to continue
                    down = false;
                }
                break;
            case nodeType::p_noun:
                {
                    assert(!pn->_binding);
                    
                    ok = resolveNoun(pn, 0, tl);
                    if (ri._toScope) ok = restrictToIdeal(tl);

                    if (ok)
                    {
                        if (tl.empty()) ri._empty = true;
                        else pn->_binding = new Binding(tl);
                    }
                    
                }
                break;
            case nodeType::p_pronoun:
                {
                    assert(!pn->_binding);
                    ok = resolvePronoun(pn, tl);
                    if (ri._toScope)  ok = restrictToIdeal(tl);

                    if (ok)
                    {
                        if (tl.empty()) ri._empty = true;
                        else pn->_binding = new Binding(tl);
                    }
                    else
                    {
                        ERR1("unable to resolve pronoun", pn->_word->_text);
                    }
                }
                break;
            case nodeType::p_value:
                // below this is a enode, so skip it
                down = false;
                break;
            }

            if (!ok)
            {
                ri._valid = false;  // did not resolve
                
                // not an error if given a resolution scope
                if (!ri._toScope)
                {
                    ERR1("unresolved:", pn->toStringStruct());
                }
            }

            if (down && ri._valid)
            {
                pnode* pn1 = ri._pn;
                ri._pn = pn->_head;
                resolve(ri);
                ri._pn = pn1;
            }
            
            // on way up
            switch (pn->_type)
            {
            case nodeType::p_unoun:
                {
                    // handle possession
                    assert(!pn->_binding);

                    pnode* un = pn->_head;
                    assert(un);

                    if (un->_binding)
                    {
                        if (!resolvePossession(*un->_binding, un->_next))
                        {
                            delete un->_binding;
                            un->_binding = 0;
                            ri._empty = true;
                        }

                        TL_TAKE(pn, un);
                    }
                }
                break;
            case nodeType::p_nounlist:
                // combine resolved nouns
                {
                    assert(!pn->_binding);
                    pnode* pl = pn->_head;
                    while (pl)
                    {
                        assert(pl->_binding);

                        if (!pn->_binding)
                        {
                            TL_TAKE(pn, pl);
                        }
                        else TL_TAKECONC(pn, pl);
                        
                        pl = pl->_next;
                    }
                    
                }
                break;
            case nodeType::p_tnoun:
                {
                    // (tn (noun the))
                
                    assert(!pn->_binding);

                    pnode* ns = pn->_head;
                    pnode* art = pn->_head->_next;  // the
                    
                    assert(ns);
                    assert(art && art->_word);

                    if (ns->_binding)
                    {
                        ns->_binding->_scope = art->_word->scopeOf();

                        // do not perform scope reduction in
                        // reactor templates as these are to be resolved
                        // at use-time
                        
                        if (ri._valid)
                        {
                            // resolve when full scope is selected
                            // also if just article scope selected
                            if (ri._toScope || ri._artScope)
                            {
                                if (!resolveScope(*ns->_binding, ns->_binding->_scope))
                                {
                                    // understood, but resolved to empty
                                    delete ns->_binding;
                                    ns->_binding = 0;
                                    ri._empty = true;
                                }
                            }
                        }
                        TL_TAKE(pn, ns);
                    }
                }
                break;
            case nodeType::p_rnoun:
                assert(!pn->_binding);
                assert(pn->_head->_binding);

                // XX totally ignore relphrase!!
                TL_TAKE(pn, pn->_head);
                break;
            case nodeType::p_rnounx:
                assert(!pn->_binding);
                assert(pn->_head->_binding);

                // XX totally ignore exceptphrase
                TL_TAKE(pn, pn->_head);
                break;
            case nodeType::p_cs:
            case nodeType::p_qs:
                break;
            }

            pn = pn->_next;
        }
    }

    bool prepareExecInfo(execInfo& ei)   
    {
        bool v = false;
        
        if (!ei._ps) return false;
        
        if (ei._ps->_type == nodeType::p_cs)
        {
            // command sentence V DOBJ PREP IOBJ

            pnode* pn = ei._verbn = ei._ps->_head;
            if (pn)
            {
                const pnode* vpn = ParseCommand::getVerbNode(pn);
                if (vpn)
                {
                    ei._verb = ParseCommand::getVerb(vpn);

                    if (vpn->_type == nodeType::p_averb)
                    {
                        // XX ignore adverbs for now
                        vpn = vpn->_head;
                        assert(vpn);
                    }

                    if (vpn->_type == nodeType::p_prepverb)
                    {
                        // extract any prepverb such as "look at"
                        vpn = vpn->_head;  
                        assert(vpn);
                        vpn = vpn->_next;   // prep
                        assert(vpn && vpn->_word); // simple word
                        ei._verbPrep = vpn->_word;
                    }
                }
                
                assert(ei._verb);

                // verb alone is valid.
                v = true;

                pn = ei._dobjn = pn->_next;
                if (pn)
                {
                    // find binding
                    ei._dobj = pn->getBinding();
                    
                    // if we have direct object, it must be bound
                    if (!ei._dobj) v = false;

                    if (v)
                    {
                        // ensure dobj list isnt empty
                        assert(!ei._dobj->empty());

                        // next term could be a prep, property or prepmod
                        pn = pn->_next;
                        if (pn)
                        {
                            v = false;
                            pnode* prep;

                            if (pn->_type == nodeType::p_prepmod)
                            {
                                // point to whole (prepmod prep)
                                ei._prepn = pn;
                                
                                pnode* pm = pn->_head;
                                assert(pm && pm->_word);
                                ei._prepmod = pm->_word;
                                
                                // actual prep node (or property)
                                prep = pm->_next;
                            }
                            else
                            {
                                // prep node is simple word
                                prep = ei._prepn = pn;
                            }

                            assert(prep);

                            if (prep->_type == nodeType::p_prep)
                            {
                                ei._prep = prep->_word;
                                assert(ei._prep);

                                pn = ei._iobjn = pn->_next;
                                if (pn)
                                {
                                    ei._iobj = pn->getBinding();
                                    
                                    // if we have indirect object, must be bound                                    
                                    v = ei._iobj != 0;
                                    
                                    // ensure iobj list isnt empty
                                    assert(!ei._iobj || ei._iobj->size());
                                }
                            }
                            else if (prep->_type == nodeType::p_property)
                            {
                                // not prep, but store in same slots
                                // verb will know
                                
                                ei._prep = prep->_word;
                                assert(ei._prep);

                                // same for p_value
                                pn = pn->_next;
                                if (pn && pn->_type == nodeType::p_value)
                                {
                                    // head of value is actually an enode
                                    GETENODE(pn);
                                    ei._value = en;
                                    v = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (ei._ps->_type == nodeType::p_qs)
        {
            // query sentence Q DOBJ PREP NULL or Q NULL PREP IOBJ

            // query goes into verb slot
            pnode* pn = ei._verbn = ei._ps->_head;
            if (pn)
            {
                ei._verb = pn->_word;
                assert(ei._verb);

                pn = pn->_next;
                if (pn)
                {
                    if (pn->isNounPhrase())
                    {
                        // what player in
                        // what box feels
                        
                        ei._dobjn = pn;
                        ei._dobj = pn->getBinding();
                        
                        if (ei._dobj)
                        {
                            assert(!ei._dobj->empty());
                            
                            // expect either prep or property
                            pn = pn->_next;
                            if (pn)
                            {
                                assert(pn->_type == nodeType::p_prep || pn->_type == nodeType::p_property);
                                ei._prepn = pn;
                                ei._prep = pn->_word;
                                assert(ei._prep);
                                v = true;
                            }
                        }
                    }
                    else
                    {
                        if (pn->_type == nodeType::p_prep)
                        {
                            // what in player
                            ei._prepn = pn;
                            ei._prep = pn->_word;
                            assert(ei._prep);

                            // expect a noun, but this in iobj
                            pn = pn->_next;
                            assert(pn && pn->isNounPhrase());

                            ei._iobjn = pn;
                            ei._iobj = pn->getBinding();
                            
                            if (ei._iobj)
                            {
                                assert(ei._iobj->size());
                                v = true;
                            }
                        }
                        else if (pn->_type == nodeType::p_property)
                        {
                            // what feels wet
                            ei._prepn = pn;
                            ei._prep = pn->_word;
                            assert(ei._prep);

                            pn = pn->_next;
                            assert(pn && pn->_type == nodeType::p_value);

                            GETENODE(pn);
                            ei._value = en;
                            
                            // NB: iobj is blank
                            v = true;
                        }
                    }
                }
            }
        }

        if (v)
        {
            assert(!ei._it);
            assert(!ei._that);
            
            // setup "it" if valid
            if (ei._dobj)
            {
                Term* t = ei._dobj->first();
                assert(t);
                if (t->isObject()) ei._it = t;
            }

            if (ei._iobj)
            {
                Term* t = ei._iobj->first();
                assert(t);
                if (t->isObject()) ei._that = t;
            }
        }
        
        return v;
    }

    bool execValidate(execInfo& ei)
    {
        bool v = ei._verb;
        if (v && ei._verb->isVerb())
        {
            v = !ei._verb->isDOVerb() || ei._dobj;
            if (v)
            {
                // ioverb not enforced with verbprep as it changes meaning
                if (ei._verb->isIOVerb() && !ei._verbPrep)
                {
                    if (!ei._iobj)
                    {
                        v = false;
                        ERR1("command expected indirect object", textify(ei._ps));
                    }
                }
            }
            else if (ei._err)
            {
                ERR1("command expected direct object", textify(ei._ps));
            }
        }

        if (!v && ei._output) OUTP(EXEC_SYNTAX);
        
        return v;
    }

    bool execChkSingleIO(execInfo& ei)
    {
        bool v = ei._iobj->size() == 1;
        if (!v)
        {
            if (ei._err)
            {
                ERR1("Cannot ", textify(ei._ps));
            }
            if (ei._output)
            {
                OUTP(EXEC_SYNTAX);
            }
        }
        return v;
    }

    bool execChkSingleDO(execInfo& ei)
    {
        bool v = ei._dobj->size() == 1;
        if (!v)
        {
            if (ei._err)
            {
                ERR1("Cannot ", textify(ei._ps));
            }
            if (ei._output)
            {
                OUTP(EXEC_SYNTAX);
            }
        }
        return v;
    }

    bool execPut1(execInfo& ei, Term* t)
    {
        Term* iobj = ei._iobj->first();
        string prop = ei._prep->_text;

        assert(iobj);

        bool multival = false;
        bool negate = false;
        
        if (ei._prepmod)
        {
            negate = ei._prepmod->isNegative();
            
            // XX assume if got prepmod
            multival = true;
        }

        // put X not in Y
        // XX for now fail, later might use this to remove X
        if (negate) return false;
        
        // can put A in B if A != B and B is not in A.
        bool v = t != iobj;

        if (v)
        {
            // map some props
            if (prop == PROP_INTO) prop = PROP_IN;
            else if (prop == PROP_ONTO) prop = PROP_ON;

            // is it already true?
            Term* t1 = propTerm(t, prop);
            if (t1 == iobj)
            {
                // already directly there?
                // If A is in B is in C, and X is in A, you can still
                // put it in B, although X is indirectly in B.
                if (ei._err) ERR0("Already there");
                if (ei._output) OUTP(EXEC_ALREADY);
                
                //LOG3("PUT already there ", textify(t));
                return false;
            }
            
            // prevent loops!
            TermList allT;
            subPropTermsRec(allT, t, prop);
            v = !contains(allT, iobj);
            if (!v)
            {
                LOG3("PUT, preventing loop with ", *t);
            }
        }

        if (v)
        {
            if (multival)
            {
                _state.set(t->_name, prop, iobj->_name);
            }
            else
            {
                // on => ~in and in => ~on
                // XX this might need to be expanded to all
                // coordinated relations
                if (prop == PROP_IN) _state.clearfn(t->_name, PROP_ON);
                else if (prop == PROP_ON) _state.clearfn(t->_name, PROP_IN);
                
                v = _state.setfn(t->_name, prop, iobj->_name);
            }

            LOG4("exec PUT ", *t << ' ' << prop << ' ' << *iobj << ": " << v);
        }
        else
        {
            if (ei._err)
            {
                // XX need sentence builder
                string e = "Can't put ";
                e += textify(t);
                e += ' ';
                e += prop;
                e += ' ';
                e += textify(iobj);
                e += '.';
                _emit(e);
            }
            if (ei._output) OUTP(EXEC_UNABLE);
        }

        return v;
    }

    bool execSet1(execInfo& ei, Term* t)
    {
        bool multival = false;
        bool negate = false;
        
        if (ei._prepmod)
        {
            negate = ei._prepmod->isNegative();

            // XX assume if got prepmod
            multival = true;
        }

        string val;
            
        if (ei._iobj)
        {
            Term* t = ei._iobj->first();
            assert(t);
            val = t->_name;
        }
        else
        {
            assert(ei._value);
            var ev = evalEnode(ei._value);
            val = ev.toString();
        }

        if (multival)
        {
            if (negate)
            {
                _state.clear(t->_name, ei._prep->_text, val);
                LOG4("exec CLEAR ", *t << ' ' << *ei._prep << ' ' << val);
            }
            else
            {
                _state.set(t->_name, ei._prep->_text, val);
                LOG4("exec SET ", *t << ' ' << *ei._prep << ' ' << val);
            }
        }
        else
        {
            if (_state.setfn(t->_name, ei._prep->_text, val))
                LOG4("exec SET! ", *t << ' ' << *ei._prep << ' ' << val);
        }

        return true;
    }

    bool execPut(execInfo& ei)
    {
        // assume verb is PUT and dobj and iobj are valid
        bool v = execChkSingleIO(ei);

        // verb is successful if at least one op worked.
        if (v)
        {
            // dobj will have non-zero size
            v = false;
            for (auto t : ei._dobj->_terms)
            {
                if (execPut1(ei, t)) v = true;
            }
        }
        if (v && ei._output) OUTP(EXEC_OK);
        return v;
    }

    bool execSet(execInfo& ei)
    {
        // assume verb is SET and dobj
        // needs to check prop val

        bool v = ei._prep;
        if (v) for (auto t : ei._dobj->_terms) if (!execSet1(ei, t)) v = false;
        if (v && ei._output) OUTP(EXEC_OK);
        return v;
    }

    bool execQuery(execInfo& ei)
    {
        // XX TODO
        // handle is/does X prop Y

        //LOG1("execQuery, ", textify(ei._ps));
        
        bool v = false;
        if (ei._dobj)
        {
            // what N in, eg what player in
            // what N prop, eg what box feels

            //LOG1("execQuery dobj, ", textify(ei._ps));

            if (!ei._prep)
            {
                // eg ioLift and not ioverb
                // then query is true
                OUTP(EXEC_TRUE);
                return true;
            }
            
            // currently only query singleton N
            v = execChkSingleDO(ei);
            if (v)
            {
                if (ei._iobj)
                {
                    // is player in hall
                    // _iobj is NP
                    
                    v = execChkSingleIO(ei);
                    if (v)
                    {
                        Term* t = ei._dobj->first();
                        string val = ei._iobj->first()->_name;
                        bool r = _state.test(t->_name, ei._prep->_text, val);
                        //LOG1("exec iobj QUERY ", *t << ' ' << *ei._prep << " " << val << " = " << r);
                        if (r)
                        {
                            OUTP(EXEC_TRUE);
                        }
                        else
                        {
                            OUTP(EXEC_FALSE);                            
                        }
                    }
                }
                else
                {
                    assert(!ei._value);

                    // Wait!
                    // properties and prep can be multivalued!
                    // normally setting X in Y, replaces X in Z
                    // same for properties set X feels wet, replaces X feels dry.
                    // most of the time this is true, but they can be
                    // multivalued. get the value as if we are a function
                    // which will be the last one if multivalued.
                    //
                    // need a way for query to return a set.
                    
                    Term* t = ei._dobj->first();
                    const var* y = _state.getfn(t->_name, ei._prep->_text);
                    
                    //LOG1("execQuery dobj + !obj ", *t << ' ' << *ei._prep << " = " << (y ? y->toString() : "null"));

                    if (y)
                    {
                        if (ei._prep->isPrep())
                        {
                            // expect term
                            Term* r = Term::find(y->rawString());
                            OUTP(r);
                            //LOG1("exec prep QUERY ", *t << ' ' << *ei._prep << " = " << textify(r));
                        }
                        else
                        {
                            // we dont expect a term, but it can be!
                            Term* r = Term::find(y->rawString());
                            if (r)
                            {
                                //LOG1("exec QUERY ", *t << ' ' << *ei._prep << " = " << textify(r));
                                OUTP(r);
                            }
                            else
                            {
                                // add raw value
                                //LOG1("exec QUERY ", *t << ' ' << *ei._prep << " = (val) " << y->rawString());
                                OUTP(*y);
                            }
                        }
                    }
                }
            }
        }
        else if (ei._iobj || ei._value)
        {
            // what in player
            // what feels wet

            assert(ei._prep);

            string val;

            if (ei._prep->isPrep())
            {
                // eg. what in player => terms
                
                // _iobj is NP
                assert(ei._iobj);
                
                v = execChkSingleIO(ei);
                if (v)
                {
                    val = ei._iobj->first()->_name;
                }
            }
            else
            {
                // property, iobj is blank
                assert(ei._value);
                var ev = evalEnode(ei._value);
                val = ev.toString();
                v = true;
            }

            if (v)
            {
                const string& prop = ei._prep->_text;
                Strings tags;
                _state.getdsv(prop, val, tags);
                for (auto s : tags)
                {
                    Term* r = Term::find(*s);
                    OUTP(r);
                    DLOG4(_pcom._debug, "exec QUERY", prop, val, "=", textify(r));
                }
            }
        }
        return v;
    }

    void _pushExec(execInfo& ei)
    {
        ei._prev = _ctx->_currentExec;
        _ctx->_currentExec = &ei;        
    }

    void _popExec()
    {
        assert(_ctx->_currentExec);
        _ctx->_currentExec = _ctx->_currentExec->_prev;
    }

    bool reactionAlreadyExec(Reaction& r)
    {
        // see if `r` is already in exec chain
        execInfo* eip = _ctx->_currentExec;
        while (eip)
        {
            if (eip->_currentSelector == r._s) return true;
            eip = eip->_prev;
        }
        return false;
    }

    void execReaction(execInfo& ei, Reaction& r)
    {
        if (ei._it) _ctx->pushIt(ei._it);
        if (ei._that) _ctx->pushThat(ei._that);

        // keep track of the reaction we are running
        // NB: cannot hang onto reaction as this gets regenerated.
        assert(!ei._currentSelector);
        ei._currentSelector = r._s;

        _pushExec(ei);
        bool v = run(r._s->_action);
        _popExec();

        // record whether we hit a manual break
        if (!v)
        {
            ei._break = true;
            //LOG1("reaction had break ", textify(ei._ps));
        }

        if (ei._that) _ctx->popThat();
        if (ei._it) _ctx->popIt();

    }

    Reaction* matchReactionInScope(execInfo& ei)
    {
        //LOG3("Looking for reactor match for ", textify(ei._ps));

        for (auto& r : _ctx->_reactions) // search scope
        {
            r._rank = 0; // no match
            
            execInfo rei(r._reactor);
            if (prepareExecInfo(rei) && execValidate(rei))
            {
                assert(ei._verb);
                assert(rei._verb);

                int rank = 0;

                // can be just a verb match
                bool v = (ei._verb == rei._verb);
                
                if (v)
                {
                    rank = 10000;

                    if (rei._verbPrep)
                    {
                        // if the reactor specifies at a verb prep
                        // eg "look at X" or "get in X", then prefer a match
                        // by doubling verb rank, fallback will be to match
                        // in absence of better reactor.
                        if (ei._verbPrep == rei._verbPrep) rank *= 2;
                        else rank /= 2; // otherwise penalty
                    }

                    // both or neither
                    v = (rei._dobj != 0) == (ei._dobj != 0);

                    if (v && rei._dobj)
                    {
                        // insist they match
                        v = subset(rei._dobj->_terms, ei._dobj->_terms);
                        if (v) rank += 1000;
                        else
                        {
                            // are my terms all subtypes of template?
                            int d = subtypes(ei._dobj->_terms,
                                             rei._dobj->_terms);
                            if (d)
                            {
                                v = true;
                                
                                // subtypes match, give weaker rank
                                rank += d*10;
                            }
                        }
                    }

                    if (v)
                    {
                        v = (rei._prep != 0) == (ei._prep != 0);

                        if (v && rei._prep)
                        {
                            v = rei._prep == ei._prep;
                            if (v) rank += 100;
                        }

                        if (v)
                        {
                            v = (rei._iobj != 0) == (ei._iobj != 0);

                            if (v && rei._iobj)
                            {
                                v = subset(rei._iobj->_terms, ei._iobj->_terms);
                                if (v) rank += 100;
                                else
                                {
                                    // are my terms all subtypes of template?
                                    int d = subtypes(ei._iobj->_terms,
                                                     rei._iobj->_terms);
                                    if (d)
                                    {
                                        v = true;
                                        
                                        // weaker match
                                        rank += d;
                                    }
                                }
                            }
                        }
                    }
                }

                if (v)
                {
                    // match!
                    assert(rank);
                    r._rank = rank;
                    LOG4("potential reactor ", textify(rei._ps) << " rank " << r._rank << " from " << r._s->_host->_name);
                }
            }
            else
            {
                ERR1("malformed reactor", rei._ps->toStringStruct());
            }
        }

        // find first closest match
        Reaction* best = 0;
        int bestrank = 0;
        for (auto& r : _ctx->_reactions)
        {
            if (r._rank > bestrank)
            {
                // ignore reactions we are currently executing
                // this both avoids loops and allows a reaction
                // to effectively call its parent
                if (reactionAlreadyExec(r))
                {
                    LOG4("Skipping exiting reaction ", textify(r._reactor));
                }
                else
                {
                    bestrank = r._rank;
                    best = &r;
                }
            }
        }

        if (best)
        {
            LOG4("matched reactor ", textify(best->_reactor) << " from " << best->_s->_host->_name);
        }
        
        return best;
    }

    void updateIt(execInfo& ei)
    {
        if (ei._it)
        {
            //LOG3("UPDATEIT ", ei._it->_name);
            _ctx->setIt(ei._it);
        }
    }

    bool resolveMatchingReactor(execInfo& ei, Reaction& r)
    {
        // some reactors have a scoping word
        // apply that here
        // eg "this book", "that book" etc.
        bool v = true;
        if (ei._dobj)
        {
            execInfo rei(r._reactor);

            // should not fail as this has happened during matching process
            v = prepareExecInfo(rei);
            assert(v);
            assert(rei._dobj);

            auto s = rei._dobj->_scope;
            if (s)
            {
                LOG4("resolve matching reactor, dobj scope ", s);
                v = resolveScope(*ei._dobj, s);
            }

            if (v && ei._iobj && rei._iobj)
            {
                s = rei._iobj->_scope;
                if (s)
                {
                    LOG4("resolve matching reactor, iobj scope ", s);
                    v = resolveScope(*ei._iobj, s);
                }
            }
            
        }
        return v;
    }

    bool exec(execInfo& ei)
    {
        // fill out ei 
        bool v = prepareExecInfo(ei) && execValidate(ei);

        if (v) 
        {
            LOG4("exec ", textify(ei._ps));
            
            bool done = false;

            if (ei._internalOps)
            {
                done = true;
                
                // handle built in cases 
                if (*ei._verb == SYM_PUT && !ei._verbPrep)
                {
                    v = execPut(ei);
                    if (v) _ctx->_scopeChanged = true;
                }
                else if (*ei._verb == SYM_SET)
                {
                    v = execSet(ei);
                    if (v) _ctx->_scopeChanged = true;
                }

                // handle queries
                else if (ei._verb->isQuery())
                {
                    v = execQuery(ei);
                    if (!v)
                    {
                        LOG4("exec query failed ", textify(ei._ps));
                    }
                }
                else done = false;
            }

            if (!done)
            {
                // match against current reactions
                Reaction* mr = matchReactionInScope(ei);
                if (mr)
                {
                    if (ei._resolving)
                        v = resolveMatchingReactor(ei, *mr);

                    if (v) execReaction(ei, *mr);
                }
                else v = false;
            }
        }
        return v;
    }

    void resolveReactions(Term* t)
    {
        assert(t->isObject());
        for (auto s : t->_selectors._selectors)
        {
            if (!s->_isReactor) continue; // skip special labels

            Flow::EltCommand* ec = s->_text.firstCommand();

            // reactor is not a command or is not parsed
            if (!ec || !ec->_parse) continue; 
            
            // conditionals can eliminate a reactor
            if (!checkSelectorCond(s)) continue;
            
            pnode* pn = ec->_parse->copy();  // consumed by reaction
            ResInfo ri(pn);
            resolve(ri);
            if (ri.resolved())
            {
                _ctx->_reactions.emplace_back(Reaction(this, s, pn));
                
                // objects resolved in reaction are added to
                // resolution scope.
                _ctx->includeInResolutionScope(pn);
            }
            else
            {
                ERR1("failed to resolve reactor", pn->toStringStruct());
                delete pn;
            }
        }
    }

    void resolveTermReactions(Term* t)
    {
        resolveReactions(t);

        // parent reactions that match child reactions at the same rank
        // will appear in the list but after the child reaction.
        // if this matches at rank, then it will not be chosen as the
        // child reaction is first.
        TermList tl;
        t->getParents(tl);

        // reactions are added in reverse parent order
        // so that secondary parents can be added to override main
        // parents
        for (auto it = tl.crbegin(); it != tl.crend(); ++it)
            resolveTermReactions(*it);
    }

    void resolveAllReactions(const TermList& scope)
    {
        //LOG1("resolving scope reactions ", "");
        for (auto t : scope)
        {
            // within a reaction, "it" corresponds to itself
            // and also within parents
            BindIt bind(this, t);
            resolveTermReactions(t);
        }
    }

    /////////////////////////

    void _printScope()
    {
        _emit("Scope: ");
        _emit(textify(_ctx->_scope));
        _emit('\n');
    }

    void _printResolutionScope()
    {
        _emit("Resolution Scope: ");
        _emit(textify(_ctx->_resolutionScope));
        _emit('\n');
    }

    void updateScope()
    {
        // recalculate the `scope` for the player
        // also resolve all currently available reactions

        // this is called after every choice/command made
        // AND after every command within flows.

        if (!_ctx->_scopeChanged) return; // no need!
        _ctx->_scopeChanged = false;
        
        //LOG1("updating scope", "");
        
        if (_player)
        {
            _ctx->purgeReactions();

            TermList seed;
            if (_scopeSeed)
            {
                int m = ~run_choice & ~run_media;
                _pushcap(m);
                _run(_scopeSeed);
                CapRef cap = _popcap();
                if (cap)
                {
                    for (auto& e : cap->_cap._elts)
                        if (e._term) seed.push_back(e._term);
                    
                    if (seed.empty())
                    {
                        LOG2("seed cap has no terms, ", cap->_cap);
                    }

                }
                else
                {
                    LOG1("seed cap empty ", textify(_scopeSeed));
                }

            }

            if (seed.empty())
            {
                // cannot have empty seed. start with player
                Term* p = inTerm(_player); // location
                if (!p) p = _player;
                seed.push_back(p);
            }
            else
            {
                LOG1("Using scope seed ", textify(seed));
            }
            
            calculateScope(_ctx->_scope, seed);
            _ctx->resolveScopeReactions();

            // if somehow got set
            _ctx->_scopeChanged = false;
        }
    }

    void _prepareObjectRuntime()
    {
        // find well known objects
        _player = Term::find(TERM_PLAYER);
        if (!_player) DLOG0(_pcom._debug, "There is no " TERM_PLAYER);

        _thing = Term::find(TERM_THING);
        if (!_thing) DLOG0(_pcom._debug, "There is no " TERM_THING);

        _tick =  Term::find(TERM_TICK); // automatically added
        assert(_tick);

        // may not exist
        _scopeSeed = Term::find(TERM_SCOPE);

        // optional handler when cannot resolve
        _errorNocando = Term::find(TERM_NOCANDO);
        if (!_errorNocando) DLOG0(_pcom._debug, "Warning, There is no handler " TERM_NOCANDO);

        _errorNosuch =  Term::find(TERM_NOSUCH);
        if (!_errorNosuch) DLOG0(_pcom._debug, "Warning, There is no handler " TERM_NOSUCH);

        _errorSyntax =  Term::find(TERM_SYNTAX);
        if (!_errorSyntax) DLOG0(_pcom._debug, "Warning, There is no handler " TERM_SYNTAX);

        // calculate initial scope
        updateScope();
    }

    void _processObjectProperties()
    {
        // the head flow of object terms contains commands that set up the
        // initial state.
        // These command should be already parsed and now need resolving
        // and execute.
        //
        // NB: these commands need to be resolved against global scope.
        for (auto t : *_terms)
        {
            if (t->isObject())
            {
                BindIt bind(this, t); // "it" refers to the defined object
                for (auto i = t->_flow._elts.begin(); i != t->_flow._elts.end(); ++i)
                {
                    Flow::Elt* e = i;
                    assert(e->_type == Flow::t_command);
                    Flow::EltCommand* ec = (Flow::EltCommand*)e;
                    if (ec->_parse)
                    {
                        //DLOG1(_pcom._debug, "resolving", *ec);
                        ResInfo ri(ec->_parse);
                        resolve(ri);
                        if (ri.resolved())
                        {
                            DLOG1(_pcom._debug > 1, "resolved", textify(ec->_parse));
                            execInfo ei(ec->_parse);
                            exec(ei);
                        }
                        else
                        {
                            ERR1("failed to resolve", textify(ec->_parse));
                        }

                        // throw away resolution bindings after setup.
                        clearBindings(ec->_parse);
                                                
                    }
                }
            }
        }
    }
    
    void prepare()
    {
        if (!_prepared)
        {
            _buildDictionary();
            _prepared = _preparseCommands();
            _processObjectProperties();
            _prepareObjectRuntime();
        }
    }

    string runSingleTerm(const string& tname)
    {
        // use for evaluating a term outside of normal flow
        // for example meta data.
        string s;

        if (_prepared)  // must already be prepared
        {
            Term* t = Term::find(tname);
            if (t)
            {
                _pushcap(~run_media); // do not process media
                run(t);
                s = textify(_popcap()->_cap);
            }
        }
        return s;
    }

    void _runTerm(Term* t)
    {
        if (t && !setjmp(_env_top))
        {
            _pushcap();
            run(t);
            flush();
        }        
    }

    void runTerm(const string& tname)
    {
        // run a term by name, used for subcommands outside of flow
        assert(_prepared);
        _runTerm(Term::find(tname));
    }

    bool start(Term* startTerm)
    {
        bool v = false;

        if (!_terms->empty())
        {
            if (!_prepared) prepare();

            v = _prepared;
            if (v) _runTerm(startTerm);
            else
            {
                ERR0("not prepared");
            }
            
            // appears to need extra flushing right at end?
            _emit('\n');
            flush();
        }
        return v;
    }

    bool preParseFlowCommands(Flow& f, FlowVisitor& fv)
    {
        // all flow commands are parsed syntactically
        // this will check syntax at author-time.

        // pass is 0 or 1
        // first pass we only parse commands that create new dictionary words
        // second pass we keep results

        int pass = fv._pass;
        bool v = true;

        for (auto i = f._elts.begin(); i != f._elts.end(); ++i)
        {
            Flow::Elt* e = i;
            if (e->_type == Flow::t_command)
            {
                Flow::EltCommand* ec = (Flow::EltCommand*)e;
                //DLOG1(_pcom._debug, "parsing", *ec);

                if (!pass)
                {
                    //DLOG1(1, "pass 1 pre-parsing ", *ec);

                    pnode* pv = _pcom.parseVerb(ec->_command.c_str());
                    if (pv)
                    {
                        const Word* verb = _pcom.getVerb(pv);
                        delete pv;
                        
                        //DLOG1(verb && _pcom._debug, "pass 1 verb", verb->_text);
                        if (verb && verb->_text == SYM_SET)
                        {
                            // parse the whole command then throw it.
                            // this will add set properties to the dictionary
                            // in this first pass in case they are used
                            // in other commands for the second pass
                            delete _pcom.parse(ec->_command); 
                        }
                    }
                    else
                    {
                        //DLOG1(1, "failed to pre-parse verb ", *ec);
                    }
                }
                else
                {
                    pnode* pn = _pcom.parse(ec->_command, fv._lineno);
                    if (pn) ec->setParse(pn); //owns
                    else v = false;
                
                    if (v)
                    {
                        LOG4("parsed, ", *ec);
                    }
                    else
                    {
                        ERR1("Failed to parse ", *ec);
                    }
                }
            }
        }
        return v;
    }
    
    bool _preparseCommands()
    {
        using namespace std::placeholders;

        FlowVisitor ff(std::bind(&Strandi::preParseFlowCommands, this, _1, _2));

        // selector text is a command, but not to be parsed
        // eg a special label
        // this only applies to objects!
        ff._skipNonReactors = true;
        
        bool v = true;

        // first pass
        for (auto t: *_terms)
        {
            // visit all flows and parse commands that create dictionary
            // entries.
            if (!t->visit(ff)) v = false;
        }

        // second pass
        // parse all commands and keep result
        // first pass may have created new dictionary entries that we use here.
        ++ff._pass;
        
        for (auto t: *_terms)
        {
            // visit all flows and parse their commands
            if (!t->visit(ff)) v = false;
        }

        return v;
    }

    void _buildDictionary()
    {
        // adds words used in object reactors to dictionary
        // creates initial verb list.
        // also add term IDs
        
        for (auto t: *_terms)
        {
            if (t->isGenerator())
            {
                // flow terms are added as well. 
                _pcom.internWordType(t->_name, Word::pos_noun | Word::pos_FLOW);
            }
        
            if (t->isObject())
            {
                // the term object ID is added as anoun
                _pcom.internWordType(t->_name, Word::pos_noun | Word::pos_ID);
                
                for (auto s : t->_selectors._selectors)
                {
                    // examine the object reactions
                    // some are special keywords
                    // otherwise they are command templates
                    
                    // Don't evaluate the flow, as this must happen when
                    // we parse the templates,
                    // instead just look for the first textual word
                    // as special keywords are always plain
                    string tag = s->_text.firstCommandWord();
                    
                    if (!tag.size()) continue;

                    if (tag == SYM_NAME)
                    {
                        // * name
                        // is followed by a parsable noun
                        // eg "the fat cat"
                        // but, as yet we don't have the words in the dict
                        int i;
                        
                        _pushcap();
                        run(s->_action);
                        string abuf = textify(_popcap()->_cap);

                        std::vector<string> words;
                        split(words, abuf);

                        pnode* pn = 0;

                        if (!words.empty())
                        {
                            for (i = 0; i < (int)words.size()-1; ++i)
                            {
                                // skip any known articles
                                const Word* wi = _pcom.findWord(words[i]);
                                if (wi && wi->isArticle()) continue;

                                // otherwise assume adjective
                                LOG4("Adding adjective ", words[i]);
                                _pcom.internWordType(words[i], Word::pos_adj);
                            }
                        
                            LOG4("Adding noun ", words[i]);
                            _pcom.internWordType(words[i], Word::pos_noun);

                            // now we've added the words, parse the nounphrase
                            // and store as one of our names.
                            LOG4("Parsing name ", abuf.c_str());
                            pn = _pcom.parseNoun(abuf.c_str());
                        }
                        
                        if (pn)
                        {
                            DLOG1(_pcom._debug > 1,"Parsed name", pn->toStringStruct());
                            t->_namenodes.push_back(pn);
                        }
                        else
                        {
                            ERR0(s->_lineno << ": " << t->_name << " '*name' expects a noun phrase, '" << abuf << '\'');
                        }
                    }
                    else if (tag == SYM_LABEL)
                    {
                        // * label
                        // we just remember the label flow as this is
                        // evaluated on demand.
                        if (t->_label)
                        {
                            ERR1("Object cannot have multiple labels", t->_name);
                        }
                        else t->_label = &s->_action;
                    }
                    else
                    {
                        // general reaction template
                        // later we need to parse the template
                        // but first we must add any verb to the dict.
                        _pcom.internWordType(tag, Word::pos_verb);

                        // mark as a generic reactor template
                        s->_isReactor = true;
                    }
                }
            }
        }
    }

    string textify(const Capture& c, bool expandterms = true)
    {
        // if expandterms, then terms are expanded into words
        // otherwise left as term symbols
        
        string s;
        std::list<string> sl;
        for (auto& e : c._elts)
        {
            if (!e._s.empty() || e._v)
            {
                Capture::cats(s, textify(sl));
                sl.clear();

                if (e._v) Capture::cats(s, e._v.toString());
                else Capture::cats(s, e._s);
            }
            else if (e._term)
            {
                string si;
                if (expandterms) si = textify(e._term);
                else si = e._term->_name;
                sl.push_back(si);
            }
        }

        Capture::cats(s, textify(sl));
        return s;
    }

    string textify(const TermList& tl)
    {
        std::list<string> sl;
        for (auto t : tl) sl.push_back(textify(t));
        return textify(sl);
    }

    string textify(const Binding& b)
    {
        return textify(b._terms);
    }

    string textify(const std::list<string>& tl)
    {
        string s;
        int n = tl.size();
        if (n)
        {
            for (auto& t : tl)
            {
                if (s.size())
                {
                    if (n > 1) s += ", ";
                    else s += " and ";
                    s += t;
                }
                else s = t;
                --n;
            }
        }
        return s;
    }

    string textify(const var* v)
    {
        return v ? v->toString() : string();
    }

    string textify(const Vars& vs)
    {
        std::list<string> sl;
        for (auto v : vs)
        {
            string vi = textify(v);
            if (vi.size()) sl.push_back(vi);
        }
        return textify(sl);        
    }
    
    string textify(Term* t)
    {
        string s;
        if (t)
        {
            if (t->_label)
            {
                // term has a given label. this should always be used
                _pushcap();
                run(*t->_label);
                s = textify(_popcap()->_cap);
            }
            
            // no label or label does not generate output
            if (s.empty())
            {
                // try the name nodes
                for (auto pn : t->_namenodes)
                {
                    s = pn->toString();
                    if (s.size()) break;
                }
            }

            if (s.empty())
            {
                // still nothing! fall back to ID
                s = t->_name;
                
            }
        }
        return s;
    }

    bool _textifyExpandBinding(pnode::SHelper& h) 
    {
        // this ought to work for any bound node, ie replace the
        // node with the textified binding.
        // But, there's no need unless we have a pronoun
        // since it should be the same.
        const pnode* pn = h._current;
        bool res = pn->_type == pnode::p_pronoun && pn->_binding;
        
        if (res)
        {
            string s = textify(*pn->_binding);
            if (!h.s.empty()) h.s += ' ';
            h.s += s;
        }
        return res;
    }

    string textifyFancy(pnode* pn)
    {
        using namespace std::placeholders;  
        pnode::SHelper h;
        h._hook = std::bind(&Strandi::_textifyExpandBinding, this, _1);

        pn->toString(&h);
        
        //LOG3(TAG "textifyFancy ", textify(pn) << " ~ " << h.s);
        return h.s;
    }

    string textify(pnode* pn) 
    {
        // similar to `toStringStruct`, but with parse node details.
        
        string s;

        while (pn)
        {
            if (pn->_type == nodeType::p_value)
            {
                GETENODE(pn);
                s += en->toString();
            }
            else
            {
                bool close = false;
                pnode* n = pn;
                if (pn->_word)
                {
                    assert(!pn->_head);
                    s += pn->_word->_text;
                }
                else
                {
                    n = pn->_head;
                    assert(n);

                    close = n->_next != 0;

                    if (close) s += '(';
                    s += textify(n);
                }

                if (n->_binding)
                {
                    // term list
                    s += " [";
                    s += textify(*n->_binding);
                    s += ']';
                }

                if (close) s += ')';

                if (pn->_next) s += ' ';
            }
            pn = pn->_next;
        }
        return s;
    }


   //  EXEC

#if 0

    enum Op
    {
        op_void = 0,
        op_term,
        op_choice,
        op_choice2,
        op_choice3,
        op_pophead,
        op_generate,
        op_finish,

        op_flow,

    };

    struct Oi
    {
        Op      _op;
        void*   _arg;
        void*   _ctx = 0;

        Oi(Op op, void* a) : _op(op), _arg(a) {}
    };

    std::list<Oi>      _istack;

    Oi  ipop()
    {
        assert(!_istack.empty());
        Oi e = _istack.back();
        _istack.pop_back();
        return e;
    }

    void ipush(Oi& e) { _istack.push_back(e); }
    void ipush(Term* t) { _istack.emplace_back(Oi(op_term, t)); }

    void ipushunder(Oi& e)
    {
        Oi a = ipop();
        ipush(e);
        ipush(a);
    }

    void ipush(Flow& f, void* ctx = 0)
    {
        Flow::Elt* f1 = f.firstElt();
        if (f1)
        {
            Oi i(op_flow, f1);
            i._ctx = ctx;
            _istack.push_back(i);
        }
    }

   void ipush(const Oi& e, Op op)
   {
       // use e as template for op
       Oi i = e;
       i._op = op;
       ipush(i);
   }

    void ex()
    {
        if (!_istack.empty())
        {
            Oi e = ipop();
            
            switch (e._op)
            {
            case op_term:
                {
                    Term* t = (Term*)e._arg;
                    if (!_run1(t))
                    {
                        if (t->isChoice())
                        {
                            e._op = op_choice;
                            ipush(e);
                        }
                        else if (t->isGenerator())
                        {
                            e._op = op_generate;
                            ipush(e);
                        }
                    }
                }
                break;
            case op_choice:
                {
                    Term* t = (Term*)e._arg;
                    Choices* c = new Choices(t);
                    prepareChoices(*c);
                    if (c->present())
                    {
                        e._ctx = c;
                        ipush(e, op_finish); // delete c

                        ipush(e, op_choice2); // after headflow
                        
                        // run head flow?
                        if (t->_flow)
                        {
                            // after eval head pop
                            ipush(e, op_pophead);

                            _pushcap();
                            ipush(t->_flow, c);
                        }
                    }
                    else
                    {
                        delete c;
                    }
                }
                break;
            case op_choice2:
                {
                    Choices* c = (Choices*)e._ctx;
                    assert(c);
                    if (c->present())
                    {
                        ipush(e, op_choice3);
                        runChoice2(*c);
                    }
                }
                break;
            case op_choice3:
                {
                    Choices* c = (Choices*)e._ctx;
                    assert(c);

                    runChoices(*c);
                    updateScope();
                    if (*c)
                    {
                        ipush(c->_t->_postflow, c);
                    }
                }
                break;
            case op_pophead:
                {
                    Choices* c = (Choices*)e._ctx;
                    assert(c);
                    CapRef ccap = _popcap();
                    if (*c) c->_headFlow = textify(ccap->_cap);
                }
                break;
            case op_finish:
                {
                    Choices* c = (Choices*)e._ctx;
                    assert(c);
                    delete c;
                }
                break;
            case op_flow:
                {
                    auto f = (Flow::Elt*)e._arg;
                    assert(f);
                    switch (f->_type)
                    {
                    case Flow::t_text:
                        run((Flow::EltText*)f);
                        break;
                    case Flow::t_code:
                        run((Flow::EltCode*)f);
                        break;
                    case Flow::t_command:
                        {
                            bool v = run((Flow::EltCommand*)f); // XXX
                        }
                        break;
                    case Flow::t_term:
                        {
                            auto et = (Flow::EltTerm*)f;
                            assert(et->_term);
                            ipush(et->_term); // XX what about break
                        }
                        break;
                    case Flow::t_media:
                        run((Flow::EltMedia*)f);
                        break;
                    }

                    f = f->next();
                    if (f)
                    {
                        e._arg = f;
                        ipush(e);
                    }
                }
                break;
            }
        }
    }

#endif // 0
    

};

}; // ST





