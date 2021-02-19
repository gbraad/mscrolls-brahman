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
            //LOG3("Reaction ID ", s);
            return s;
        }

        Reaction(Strandi* host, Selector* s, pnode* pn)
            : _host(host),_s(s), _reactor(pn) {}
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
    Term*       _errorNocando = 0;
    Term*       _errorNosuch = 0;
    Term*       _errorSyntax = 0;

    jmp_buf     _env_top;
    int         _time = 0;

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
        const Word* _verbPrep = 0;
        pnode*      _dobjn = 0;
        Binding*    _dobj = 0;
        pnode*      _prepn = 0;
        const Word* _prep = 0;
        const Word* _prepmod = 0; // modifier for prep eg also/not

        pnode*      _iobjn = 0;
        Binding*    _iobj = 0;
        Term*       _it = 0;

        // hit manual break during execution
        bool        _break = false;
        bool        _resolving = false;

        // current exec info
        Reaction*   _currentReaction = 0;
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
        
        PronBinding*        _pronouns = 0;

        // currently available
        Reactions           _reactions;
        execInfo*           _currentExec = 0;

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
            for (auto& r : _reactions) delete r._reactor;
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
        int dur = -1;

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

    bool run(Flow& f)
    {
        // return false if we hit a manual return
        bool v = true;
        for (auto e : f._elts)
        {
            switch (e->_type)
            {
            case Flow::t_text:
                {
                    Flow::EltText* et = (Flow::EltText*)e;
                    OUTP(et->_text);
                }
                break;
            case Flow::t_code:
                {
                    Flow::EltCode* ec = (Flow::EltCode*)e;
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
                }
                break;
            case Flow::t_command:
                {
                    Flow::EltCommand* c = (Flow::EltCommand*)e;
                    if (c->_parse)
                    {
                        bool r = resolve(c->_parse);
                        if (r)
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
                            r = exec(ei);

                            // retrieve manual break request from exec
                            if (ei._break) v = false;

                            
                            clearBindings(c->_parse);
                            updateScope();
                        }
                        else
                        {
                            ERR1("flow cannot resolve", textify(c->_parse));
                        }
                    }
                    else
                    {
                        ERR1("Command has no parse ", c->toString());
                    }
                }
                break;
            case Flow::t_term:
                {
                    Flow::EltTerm* et = (Flow::EltTerm*)e;
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
                            v = true;
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
                                v = run(t);
                            }
                                
                            if (!v) break;
                        }
                    }
                    else
                    {
                        ERR1("unbound term", et->_name);
                    }
                }
                break;
            case Flow::t_media:
                {
                    Flow::EltMedia* em = (Flow::EltMedia*)e;
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
                break;
            }
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
        bool                _ranhead = false;
        string              _headFlow;

        // null choices are commands, if we have any
        Selector*           _cmdChoice = 0;
        bool                _matching = false;


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

#ifdef __EMSCRIPTEN__
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
                LOG1(TAG, "request shutdown");
                longjmp(_env_top, 1);
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
#endif    

    void presentChoices(Choices& c)
    {
        // flatten choice text
        for (auto& ci : c._choices)
        {
            if (ci._text.empty())
                ci._text = textify(ci._select->_cap);
        }

        while (c._valid && c._ranhead)
        {
            int ch;
            bool accept = false;
            string line;

            do
            {
#ifdef IFI_BUILD

                // the headflow is put into the choice dialog
                GrowString gs;
                bool v = buildChoiceJSON(gs, c);

                // without choices, emit any headflow into the main text
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
#else
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
#endif  // IFI_BUILD                
                
                line = toLower(_getline());
                
                // choices count from 1
                ch = 0;

                int nchoices = c.size();

                if (!line.size())
                {
                    // blank line accepts single choice by default
                    // unless cmd available.
                    if (nchoices == 1 && !c._cmdChoice)
                    {
                        accept = true;
                        ch = 1;
                    }
                }
                else
                {
                    // choices must be a number at the start
                    // otherwise assume command input
                    if (nchoices > 0 && u_isdigit(line[0]))
                    {
                        // prevent stoi overflow
                        if (line.length() < 10) ch = std::stoi(line);
                        if (ch > 0 && ch <= (int)c.size()) accept = true;
                    }
                    else
                    {
                        if (c._cmdChoice) accept = true;
                    }
                }
            } while (!accept);

            // ch > 0 will be a choice
            // ch == 0 is a cmd.

            if (ch)
            {
                LOG3(TAG "choice made ", (int)ch);

                Choice& cc = c._choices[--ch];
                Selector* s = cc._action;
                
                // mark as chosen.
                if (s->once()) _state.set(cc.id());
                
                // arrange for newline after choice and before subsequent
                // text, ONLY if there is text!
                _out->_cap._needNewline = true;

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
            else
            {
                // handle command
                accept = false;

                assert(c._cmdChoice);

                if (line == "?")  // debugging
                {
                    _printScope();
                    _printResolutionScope();

                    // special case to examine reactions
                    int cc = 0;
                    for (auto& r : _ctx->_reactions)
                    {
                        assert(r._reactor);
                        _emit('\n');
                        _emit(++cc);
                        _emit(") ");
                        _emit(r._s->_host->_name);
                        _emit(": ");
                        _emit(textify(r._reactor));
                    }
                    _emit('\n');
                }
                else
                {
                    // try to parse command and execute
                    pnode* ps = _pcom.parse(line);
                    if (ps)
                    {
                        // create bindings
                        // restrict input to resolution scope
                        if (resolve(ps, true))
                        {
                            execInfo ei(ps);

                            // perform resolution against reactor match
                            ei._resolving = true;
                            
                            accept = exec(ei);
                            if (accept)
                            {
                                updateIt(ei);
                        
                                // and run any action after command
                                c._valid = run(c._cmdChoice->_action);
                            }
                            else
                            {
                                if (_errorNocando)
                                    accept = run(_errorNocando);
                            }
                        }
                        else
                        {
                            // no such object
                            if (_errorNosuch)
                                accept = run(_errorNosuch);
                        }

                        // will also delete bindings
                        delete ps;
                    }
                    else
                    {
                        if (_errorSyntax)
                            accept = run(_errorSyntax);
                    }
                        
                }
            }

            if (accept) break;
        }

        updateScope();
    }

    bool evalENode(enode* e)
    {
        bool v = false;
        assert(e);
        switch (e->_type)
        {
        case enodeType::e_name:
            {
                Term* t = e->_binding;
                if (t)
                {
                    // XX
                    // since we're using the name to test, do we actually need
                    // the binding?
                    v = _state.test(t->_name);
                }
                else
                {
                    LOG1(TAG "WARNING unbound conditional ", e->_name);
                }
            }
            break;
        case enodeType::e_and:
            {
                enode* ei = e->_head;
                while (ei)
                {
                    v = evalENode(ei);
                    if (!v) break;
                    ei = ei->_next;
                }
            }
            break;
        case enodeType::e_or:
            {
                enode* ei = e->_head;
                while (ei)
                {
                    v = evalENode(ei);
                    if (v) break;
                    ei = ei->_next;
                }
            }
            break;
        }

        if (e->_neg) v = !v;
        
        return v;
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
                v = evalENode(ec->_cond);
            }
                
            //LOG3(TAG "conditional ", s->_cond << " = " << v);
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
        for (auto& ci : c._choices)
        {
            if (ci._selected)
            {
                cp = &ci;
                if (!ch) break;
                --ch;
            }
        }
        
        assert(cp);

        // when we're not matching the select flow is part of the output
        if (!cp->_select)
            c._valid = run(cp->_action->_text);

        // but we always run the action
        if (c._valid)
            c._valid = run(cp->_action->_action);

        return true;
    }

    bool runSticky(Term* t)
    {
        // if we're sticky and stuck, run that.
        
        bool res = false;

        if (t->sticky())
        {
            // stuck?
            // topflow is not run when stuck
            const var* v = getTermValue(t);
            if (v)
            {
                // dont run headflow if stuck!
                //run(t->_flow);
                    
                // return the sticky value
                Capture cap;
                cap.fromVar(*v);
                outCap(cap);

                res = true;
            }
        }
        return res;
    }
    
    bool runGenerator(Term* t)
    {
        // return false for stack break
        
        Choices choices(t);
        
        // top flow is input for matching generators
        CapRef topflowcap;

        assert(t->isGenerator());
            
        // run top flow and keep result, this becomes
        // an input selector.
        if (t->_topflow)
        {
            choices._matching = true;

            // mask choices in topflow
            // prevent choices being run and instead
            // returns the previous choice made if any
            // prevent media, emit as string instead.
            int m = ~run_choice & ~run_media;
            _pushcap(m);
            choices._valid = run(t->_topflow);
            
            // NB: result can be empty
            topflowcap = _popcap();
        }

        // install results of topflow as lastcap
        LastCap::Tmp ltmp(_ctx->_lastCap, &topflowcap->_cap);
        
        bool stickpushed = false;
        if (choices._valid)
        {
            if (t->sticky())
            {
                _pushcap();
                stickpushed = true; // remember to pop
            }
            
            // run head flow after any topflow
            // NB: code within headflow can access topflow via lastcap
            choices._valid = run(t->_flow);
        }

        if (choices._valid)
        {
            // if we are matching:
            // one or more matches => pick random match
            // no matches => collect null tags and pick randomly
            // no matches & no null tags => no output.

            if (choices._matching)
            {
                // insist on matching terms being random (for now)
                assert(t->_rtype == Term::t_random);
                
                // run all choice select flows and capture them
                // for later matching
                for (auto s : t->_selectors._selectors)
                {
                    if (checkSelectorCond(s)) // ignore failed conditional
                    {
                        // when matching we must run all selector flows
                        // in order to make match.
                        _pushcap();
                        choices._valid = run(s->_text);
                        Choice ch(_popcap(), s);
                        if (!choices._valid) break;
                        choices._choices.push_back(ch);
                    }
                }

                if (choices._valid)
                {
                    assert(topflowcap);

                    if (!topflowcap->_cap)
                    {
                        // input flow yielded nothing
                        // do we have a null match choice?
                        for (auto& ci : choices._choices)
                            ci._selected = ci._nullmatch;
                        runGeneratorAction(choices);
                    }
                    else
                    {
                        // if we have a setmatch, then feed all in one
                        // ie a match for a whole set.
                        bool setmatch = false;
                        for (auto& ci : choices._choices)
                        {
                            ci._selected = ci._setmatch;
                            if (ci._selected) setmatch = true;
                        }

                        if (setmatch)
                        {
                            // already installed as lastcap
                            //LastCap::Tmp ltmp(_ctx->_lastCap, &topflowcap->_cap);
                            runGeneratorAction(choices);
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
                        
                                for (auto& ci : choices._choices)
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
                                runGeneratorAction(choices);
                                if (!choices._valid) break;
                            }
                        }
                    }
                }
            }
            else
            {
                // not matching, so generating
                for (auto s : t->_selectors._selectors)
                {
                    // when automatically choosing we run the selector
                    // flow later
                    // add all selectors even if fails condition, but
                    // mark as not selected.
                    Choice ch(s);
                    ch._selected = checkSelectorCond(s);
                    choices._choices.emplace_back(ch);
                }
                runGeneratorAction(choices);
            }
        }
        
        if (stickpushed)
        {
            assert(t->sticky());
            
            CapRef cs = _popcap();

            if (choices._valid)
            {
                // save sticky
                setTermValue(t, cs->_cap.toVar());

                // dub onto base cap
                outCap(cs->_cap);
            }
        }

        bool res = choices._valid;

        // NB: lastcap is still in scope for the postflow
        if (res)
            res = run(t->_postflow);

        return res;
    }

    void prepareChoice(Choices& c, Selector* s, Reaction* reactor)
    {
        // a `reactor` is given if this is a resolved command
        // behaving like a choice.
        
        bool show = !s->hidden();

        if (show)
        {
            // if already seen, dont show
            if (s->once())
            {
                string id = reactor ? reactor->id() : s->id();
                if (_state.test(id)) show = false;
            } 
        }

        if (show) show = checkSelectorCond(s);

        if (show)
        {
            if (!c._ranhead)
            {
                // we have at least once choice
                // before running the selector flow, run the
                // term head.
                c._ranhead = true;

                // run headflow before running the first valid choice
                // keep output
                _pushcap();
                c._valid = run(c._t->_flow);
                CapRef ccap = _popcap();
                if (!c._valid) return; // break;
                c._headFlow = textify(ccap->_cap);
            }
                
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

                    /*
                    Flow::EltCommand* ec = s->_text.firstCommand();
                    if (ec)
                    {
                        // if we have a parse use that
                        if (ec->_parse) pn = ec->_parse;
                        
                        else
                        {
                            ctext = ec->_command;
                        }
                    }
                    */

                    // convert the bound reaction into plain text
                    ctext = textifyFancy(pn);
                    //LOG3("creating choice ", ctext << " from " << pn->toStringStruct());
                    if (!ctext.empty())
                        c._choices.emplace_back(Choice(ctext, reactor));

                }
                else
                {
                    // collect choice texts
                    _pushcap();
                    c._valid = run(s->_text);
                    CapRef ccap = _popcap();

                    if (!c._valid) return; // break
                
                    if (ccap->_cap)
                        c._choices.emplace_back(Choice(ccap, s));
                }
            }
            else
            {
                // we have a choice with no text
                // is a command
                c._cmdChoice = s;
            }
        }
    }

    bool runChoice(Term* t)
    {
        // return false for stack break
        
        Choices choices(t);

        assert(t->isChoice());
        
        // choice does not run the headflow (nor tail)
        // if there are no actual valid choices
            
        // TODO: can choices have topflow?
        // one idea is that this can be inherited choices??
            
        for (auto s : t->_selectors._selectors)
            prepareChoice(choices, s, 0);

        if (choices._valid && t->cmdChoices())
        {
            // add in any `aschoice` reactions
            // these are object reactions that appear as choices
            for (auto& r : _ctx->_reactions)
            {
                Selector* s = r._s;
                if (s->aschoice())
                {
                    assert(s->_isReactor);
                    prepareChoice(choices, s, &r);
                }
            }
        }

        // ranhead => choices non-empty or cmdChoice
        if (choices._valid && choices._ranhead)
            presentChoices(choices);
        
        bool res = choices._valid;

        if (res)
        {
            // otherwise the number of choices
            int nc = choices.size();

            // but can be zero if command usage
            // in this case we still want postflow
            if (choices._ranhead) ++nc;
            if (nc) res = run(t->_postflow);
        }
        
        return res;
    }
    
    bool runselect(Term* t)
    {
        // run a term and selectors
        // return false for stack break

        bool res = true;

        // if stuck, no postflow
        if (!runSticky(t))
        {
            if (t->isGenerator()) res = runGenerator(t);
            else if (t->isChoice()) res = runChoice(t);
        }
        
        return res;
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
        else if (t->_name == TERM_TICK)
        {
            v = true;
            _runTick();
        }
        return v;
    }

    bool _run(Term* t)
    {
        // return false for break
        
        bool v = true;
        switch (t->_type)
        {
        case Term::t_generator:
            {
                if (!_runSpecial(t)) v = runselect(t);
            }
            break;
        case Term::t_choice:
            if (_runtypemask & run_choice)
            {
                v =  runselect(t);
            }
            else
            {
                // when not running choices (eg topflow),
                // return last choice text
                const var* v = getTermValue(t);
                if (v) OUTP(v->toString());
            }
            break;
        case Term::t_object:
            break;
        }
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

            //DLOG1(_pcom._debug, "returned to term ", t->_name);

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

    bool restrictOneToScope(TermList* l1)
    {
        if (!l1) return true; // nothing to do

        // reduce list `l1` to just one item.

        // remove any member of `l1` not in resolution scope
        // or a subtype of some member of resolution scope
        int cc = 0;
        for (auto it = l1->begin(); it != l1->end();)
        {
            if (!subtype(*it, _ctx->_resolutionScope)) it = l1->erase(it);
            else
            {
                ++it;
                ++cc;
            }
        }

        // have more than one left?
        if (cc > 1)
        {
            // are any directly in res scope?
            cc = 0;
            for (auto it = l1->begin(); it != l1->end(); ++it)
                if (contains(_ctx->_resolutionScope, *it)) ++cc;

            if (cc >= 1)
            {
                // yes, clip to this
                cliplist(*l1, _ctx->_resolutionScope);
            }

            if (cc > 1)
            {
                // have either zero or >1 directly in res scope
                // try for interactive scope

                cc = 0;
                for (auto it = l1->begin(); it != l1->end(); ++it)
                    if (contains(_ctx->_scope, *it)) ++cc;

                if (cc >= 1)
                {
                    // have that then!
                    cliplist(*l1, _ctx->_scope);
                }
            }
        }

        if (l1->size() > 1)
        {
            LOG3("restrictOneToScope failed to restrict ", textify(*l1));
        }
        
        return !l1->empty();
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

    void subInTerms(TermList& tl, Term* p)
    {
        // all things in p
        Timeline::Strings ss;
        _state.getdsv(PROP_IN, p->_name, ss);
        for (auto i : ss)
        {
            Term* ti = Term::find(*i);
            assert(ti);
            tl.push_back(ti);
        }
    }

    void inTerms(TermList& tl, Term* p)
    {
        // all things Y, p in Y
        Timeline::Vars vs;
        _state.getset(p->_name, PROP_IN, vs);
        for (auto i : vs)
        {
            Term* ti = Term::find(i->rawString());
            if (ti) tl.push_back(ti);
        }
    }

    void inTermsRec(TermList& tl, Term* p)
    {
        // all things Y, p in Y recursive

        TermList t1;
        inTerms(t1, p);

        for (auto i : t1)
        {
            // parent
            if (!contains(tl, i)) tl.push_back(i);;
            
            // and all parents of parent
            inTermsRec(tl, i);
        }
    }

    bool isInRec(Term* a, Term* b)
    {
        // is a in b?

        // XX could be more efficient
        TermList ain;
        inTermsRec(ain, a);
        return contains(ain, b);
    }

    bool isIn(Term* a, Term* b)
    {
        // is a directly in b?
        // XX could be more efficient
        TermList ain;
        inTerms(ain, a);
        return contains(ain, b);
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
            subInTerms(pl, a); // will include p

            for (auto t : pl)
                if (t != p && !contains(tl, t)) tl.push_back(t);
        }
    }

    void calculateScope(TermList& tl, Term* p)
    {
        // calculate scope for p (usually player)
        // the scope is:
        // all things in P (sub-scope)
        // the thing P is in.
        // all things in what P is directly in.
        // and P if not already included.
        tl.clear();

        // sub items of p
        subInTerms(tl, p);

        // immediate parent "in"
        TermList parents;
        inTerms(parents, p); 

        if (!parents.empty())
        {
            // add all things in parent
            for (auto a : parents)
            {
                TermList tl1;
                subInTerms(tl1, a); // will include p
                concatm(tl, tl1);
            }

            // all initial parents
            concatc(tl, parents);

            // all remaining parents?

            /* 
               Actually no!
               I used to think it was natural to include all the parents
               in the interactive scope, but it's a mistake.
               
               Because those parents (of the location) bring in reference
               terms that should not be referenced.

               For example, if the player get into a cupboard, you don't 
               want the room to be in scope, otherwise you can use
               those room reactors. 

               This means the objects in room are not in scope - and they're
               not. At least not in the interactive scope. You might be able
               to see them, ie reference them, but not interact with them.

               Situations like this might benefit from some sort of 
               scope operator we can specify. Needs consideration.
             */
            
            //for (auto a : parents) inTermsRec(tl, a);
        }
        else
        {
            // not in anything!
            tl.push_back(p);
        }
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
            clearBindings(pn->_head);
            pn = pn->_next;
        }
    }

    TermList* resolvePronoun(pnode* pn)
    {
        TermList* tl = 0;
        assert(pn && pn->_type == nodeType::p_pronoun);
        const Word* w = pn->_word;

        // XX for now map him/her -> it. 
        if (w == _pcom._it || *w == PRON_HIM || *w == PRON_HER)
        {
            Term* t = _ctx->getIt();
            if (t)
            {
                tl = new TermList;
                tl->push_back(t);
            }
        }
        else if (*w == PRON_HERE)  // literal
        {
            if (_player)
            {
                tl = new TermList;
                inTerms(*tl, _player);
            }
        }
        else if (*w == PRON_ALL)
        {
            if (_player)
            {
                tl = new TermList;
                calculateAll(*tl, _player);
            }
        }

        if (tl && tl->empty()) { delete tl; tl = 0; }
        return tl;
    }

    TermList* resolveNoun(pnode* pn, pnode* pa)
    {
        // pn = noun
        // pa = (a a a)
        // pa can be null.

        assert(pn && pn->_type == nodeType::p_noun);
        assert(!pa || pa->_type == nodeType::p_adjs);
        assert(pn->_word);
        
        TermList* tl = new TermList;

        if (pn->_word->isFlow())
        {
            // We have a flow term inside a noun phrase
            Term* t = Term::find(pn->_word->_text);
            if (t)
            {
                assert(t->isGenerator());

                // we expect the flow to evaluate to terms
                _pushcap();
                run(t);
                CapRef cs = _popcap();

                if (cs->_cap)
                {
                    for (auto& e : cs->_cap._elts)
                    {
                        if (e._term) tl->push_back(e._term);
                        else
                        {
                            ERR2("WARNING: non-term in parse flow term", pn->_word->_text, textify(cs->_cap));
                            
                        }
                    }
                }
            }
            else
            {
                ERR1("unresolved flow term in parse", pn->_word->_text);
            }
        }
        else
        {
            // match noun with adjectives
            Term::matchName(*tl, pn, pa);
        }

        if (tl->empty()) { delete tl; tl = 0; }
        return tl;
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
                LOG3("Resolving scope: ", st << ", " << textify(b));
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
                    subInTerms(inv, _player);
                    cliplist(b._terms, inv);  // reduce to those carried
                }
                break;
            case Word::scope_here_nothave:
                // interactive scope but not carried
                {
                    assert(_player);
                    TermList inv;
                    subInTerms(inv, _player);

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

    bool resolve(pnode* pn, bool toScope = false)
    {
        // if `toScope` restrict objects to scope.
         return _resolve(pn, toScope);
    }

    bool _resolve(pnode* pn, bool toScope)
    {
        // if `scope` restrict objects to scope.

        bool res = true;
        while (pn)
        {
            bool down = true;
            bool ok = true;

            // on way down
            switch (pn->_type)
            {
            case nodeType::p_anoun:
                {
                    assert(pn->_head);
                    assert(!pn->_binding);
                    TermList* tl = resolveNoun(pn->_head, pn->_head->_next);
                    if (toScope && !restrictOneToScope(tl)) { delete tl;  tl = 0; }

                    if (tl) pn->_binding = new Binding(tl);
                    else ok = false;
                    
                    // no need to continue
                    down = false;
                }
                break;
            case nodeType::p_noun:
                {
                    assert(!pn->_binding);
                    TermList* tl = resolveNoun(pn, 0);
                    if (toScope && !restrictOneToScope(tl)) { delete tl;  tl = 0; }
                    if (tl) pn->_binding = new Binding(tl);
                    else ok = false;

                }
                break;
            case nodeType::p_pronoun:
                {
                    assert(!pn->_binding);
                    TermList* tl = resolvePronoun(pn);
                    if (toScope && !restrictOneToScope(tl)) { delete tl;  tl = 0; }

                    if (tl) pn->_binding = new Binding(tl);
                    else ok = false;
                    
                    if (!ok)
                    {
                        ERR1("unable to resolve pronoun", pn->_word->_text);
                    }
                }
                break;
            }

            if (!ok)
            {
                // not an error if given a resolution scope
                if (!toScope)
                {
                    ERR1("unresolved:", pn->toStringStruct());
                }
                res = false;
            }
            
            if (down && !_resolve(pn->_head, toScope)) res = false;

            // on way up
            switch (pn->_type)
            {
            case nodeType::p_unoun:
                // handle possession
                assert(pn->_head);
                assert(pn->_head->_binding); // should be resolved
                assert(!pn->_binding);

                // elevate resolution of first anoun
                // XX TODO need to resolve against others!!
                TL_TAKE(pn, pn->_head);
                
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
                        if (toScope)
                            res = resolveScope(*ns->_binding, ns->_binding->_scope);
                        
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

            if (!res) break;
            pn = pn->_next;
        }
        return res;
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
                                    ei._iobjn = pn;
                                    assert(pn->_word);
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

                            ei._iobjn = pn;
                            // NB: iobj is blank
                            v = true;
                        }
                    }
                }
            }
        }

        if (v)
        {
            // setup "it" if valid
            if (ei._dobj)
            {
                Term* t = ei._dobj->first();
                if (t->isObject()) ei._it = t;
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
                v = !ei._verb->isIOVerb() || ei._iobj;
                if (!v && ei._err)
                {
                    ERR1("command expect indirect object", textify(ei._ps));
                }
            }
            else if (ei._err)
            {
                ERR1("command expect direct object", textify(ei._ps));
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
        const string& prop = ei._prep->_text;

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

        if (v && prop == PROP_IN)
        {
            // already?
            if (isIn(t, iobj))
            {
                // XX sentence
                if (ei._err) ERR0("Already there");
                if (ei._output) OUTP(EXEC_ALREADY);
                return false;
            }
            
            // prevent in loop!
            v = !isInRec(iobj, t);
        }

        if (v)
        {
            if (multival)
            {
                _state.set(t->_name, prop, ei._iobj->first()->_name);
            }
            else
            {
                v = _state.setfn(t->_name, prop, ei._iobj->first()->_name);
            }

            if (v)
            {
                DLOG3(_pcom._debug, "exec PUT", *t, prop, *ei._iobj->first());
            }
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

        if (multival)
        {
            if (negate)
            {
                _state.clear(t->_name, ei._prep->_text, ei._iobjn->_word->_text);
                DLOG3(_pcom._debug, "exec CLEAR", *t, *ei._prep, *ei._iobjn->_word);
            }
            else
            {
                _state.set(t->_name, ei._prep->_text, ei._iobjn->_word->_text);
                DLOG3(_pcom._debug, "exec SET", *t, *ei._prep, *ei._iobjn->_word);
            }
        }
        else
        {
            if (_state.setfn(t->_name, ei._prep->_text, ei._iobjn->_word->_text))
                DLOG3(_pcom._debug, "exec SET!", *t, *ei._prep, *ei._iobjn->_word);
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

        bool v = ei._prep && ei._iobjn; 
        if (v) for (auto t : ei._dobj->_terms) if (!execSet1(ei, t)) v = false;
        if (v && ei._output) OUTP(EXEC_OK);
        return v;
    }

    bool execQuery(execInfo& ei)
    {
        // XX TODO
        // handle is/does X prop Y
        
        bool v = false;
        if (ei._dobj)
        {
            // what N in, eg what player in
            // what N prop, eg what box feels
            assert(ei._prep);

            // currently only query singleton N
            v = execChkSingleDO(ei);
            if (v)
            {
                if (ei._iobjn)
                {
                    // is player in hall

                    // _iobj is NP
                    assert(ei._iobj);


                    v = execChkSingleIO(ei);
                    if (v)
                    {
                        Term* t = ei._dobj->first();
                        string val = ei._iobj->first()->_name;
                        bool r = _state.test(t->_name, ei._prep->_text, val);
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

                    if (y)
                    {
                        if (ei._prep->isPrep())
                        {
                            // expect term
                            Term* r = Term::find(y->rawString());
                            OUTP(r);
                            LOG4("exec prep QUERY ", *t << ' ' << *ei._prep << " = " << textify(r));
                        }
                        else
                        {
                            // we dont expect a term, but it can be!
                            Term* r = Term::find(y->rawString());
                            if (r)
                            {
                                LOG4("exec QUERY ", *t << ' ' << *ei._prep << " = " << textify(r));
                                OUTP(r);
                            }
                            else
                            {
                                // add raw value
                                LOG4("exec QUERY ", *t << ' ' << *ei._prep << " = (val) " << y->rawString());
                                OUTP(*y);
                            }
                        

                        }
                    }
                }
            }
        }
        else if (ei._iobjn)
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
                val = ei._iobjn->_word->_text;
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

    bool reactionAlreadyExec(Reaction& r)
    {
        // see if `r` is already in exec chain
        execInfo* eip = _ctx->_currentExec;
        while (eip)
        {
            if (eip->_currentReaction == &r) return true;
            eip = eip->_prev;
        }
        return false;
    }

    void execReaction(execInfo& ei, Reaction& r)
    {
        Term* it = 0;

        if (ei._dobj)
        {
            // bind dobj to "it"
            it = ei._dobj->first();
            _ctx->pushIt(it);
        }

        // keep track of the reaction we are running
        assert(!ei._currentReaction);
        ei._currentReaction = &r;
        ei._prev = _ctx->_currentExec;
        _ctx->_currentExec = &ei;
        
        bool v = run(r._s->_action);

        _ctx->_currentExec = ei._prev;
        //ei._currentReaction = 0;

        // record whether we hit a manual break
        if (!v) ei._break = true;

        if (it) _ctx->popIt();
    }

    Reaction*  matchReactionInScope(execInfo& ei)
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
                    LOG3("reactor match ", textify(rei._ps) << " rank " << r._rank << " from " << r._s->_host->_name);
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
                    LOG3("Skipping exiting reaction ", textify(r._reactor));
                }
                else
                {
                    bestrank = r._rank;
                    best = &r;
                }
            }
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
                LOG3("resolve matching reactor, scope ", s);
                v = resolveScope(*ei._dobj, s);
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
            LOG3("exec ", textify(ei._ps));

            // handle built in cases 
            if (*ei._verb == SYM_PUT) v = execPut(ei);
            else if (*ei._verb == SYM_SET) v = execSet(ei);

            // handle queries
            else if (ei._verb->isQuery()) v = execQuery(ei);
            else
            {
                // match against current reactions
                Reaction* mr = matchReactionInScope(ei);
                if (mr)
                {
                    if (ei._resolving)
                    {
                        v = resolveMatchingReactor(ei, *mr);
                    }

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
            
            pnode* pn = ec->_parse->copy();
            if (resolve(pn))
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
        
        if (_player)
        {
            _ctx->purgeReactions();
            calculateScope(_ctx->_scope, _player);
            _ctx->resolveScopeReactions();
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
                for (auto e : t->_flow._elts)
                {
                    assert(e->_type == Flow::t_command);
                    Flow::EltCommand* ec = (Flow::EltCommand*)e;
                    if (ec->_parse)
                    {
                        //DLOG1(_pcom._debug, "resolving", *ec);
                        bool v = resolve(ec->_parse);
                        if (v)
                        {
                            DLOG1(_pcom._debug > 1, "resolved", textify(ec->_parse));
                            execInfo ei(ec->_parse);
                            v = exec(ei);
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

        for (auto e : f._elts)
        {
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
                        DLOG1(_pcom._debug, "parsed", *ec);
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
            pn = pn->_next;
        }
        return s;
    }
};

}; // ST





