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

template<class T, int N> struct Tops
{
    T           _v[N];
    int         _size = 0;

    void        add(const T& t)
    {
        int i;
        for (i = 0; i < _size; ++i)
        {
            if (_v[i] == t)
            {
                // already present, bring to front
                memmove(&_v[1], &_v[0], i*sizeof(T));
                _v[0] = t;            
                return;
            }
        }

        if (i < N)
        {
            ++_size;
        }
        else
        {
            // overwrite last
            --i;
        }
        
        memmove(&_v[1], &_v[0], i*sizeof(T));
        _v[0] = t;
    }

    bool        contains(const T& t) const
    {
        for (int i = 0; i < _size; ++i)
            if (_v[i] == t) return true;
        return false;
    }

    std::string toString() const
    {
        string s;
        s += "{";
        s += std::to_string(_size);
        s += ' ';
        for (int i = 0; i < _size; ++i)
        {
            if (i) s += ',';
            s += _v[i]->_name;
        }
        s += "}";
        return s;
    }
    
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
    typedef Timeline::Mark Mark;
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
                // make sure no spaces!
                s += replaceAll(_host->textifyFancy(_reactor, false), ' ', '_');
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
    
    Terms*      _terms;   // not owned
    Ranq1       _rand;
    Emitter     _emitter;
    Timeline    _state;
    ParseCommand _pcom;
    bool        _prepared;
    uint        _runtypemask;
    CapStack*   _out = 0;
    Evaluator   _eval;

    // when you run a single term, it can break
    // this needs to be communicated back to the main stack.
    bool        _runSingleTermBreak = false;

    // runtime
    Term*       _player;
    Term*       _thing;
    Term*       _tick;
    Term*       _undo;
    Term*       _scopeSeed;
    Term*       _errorNocando;
    Term*       _errorNosuch;
    Term*       _errorSyntax;
    int         _objectCount;

    jmp_buf     _env_top;
    int         _time;
    var::Format _vfmt;
    var::Format::BlobParser _genStateParser;  // restore genstates

    void _init()
    {
        _prepared = false;
        _runtypemask = (uint)-1;
        _out = 0;
        _player = 0;
        _thing = 0;
        _tick = 0;
        _undo = 0;
        _scopeSeed = 0;
        _errorNocando = 0;
        _errorNosuch = 0;
        _errorSyntax = 0;
        _time = 0;
        _objectCount = 0;
        
        using namespace std::placeholders;
        _genStateParser = std::bind(&Strandi::genStateParser, this, _1);

        // default to 2 decimal places when printing floats
        _vfmt._prec = 2;
        _vfmt._blobParser = &_genStateParser;
    }

    void resetAll()
    {
        // erase everything

        // clear timeline
        _state.clear();

        // erase dictionary
        _pcom.clear();

        // reset locals
        _init();
        
    }

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
        Term*       _dobji = 0;    // ith dobj when iterating
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
        typedef Stacked<Capture*>               LastCap;
        
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
        bool                _scopeChanged = false;

        // mark objects as visited (explored) during exec
        bool                 _visitObj = false;

        void                markScope() { _scopeChanged = true; }

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
            _host->resolveAllReactions();
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
            _stats = 0;

            // N=0 should only be used to construct dummy NRan
            if (_n > 0)
            {
                _stats = new unsigned int[_n];
                memset(_stats, 0, sizeof(unsigned int)*_n);
                _warmup();
            }
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

        string toString() const
        {
            // emit N MAXP followed by N stats
            string s;
            s += std::to_string(_n);
            s += ' ';
            s += std::to_string(_maxp);
            for (int i = 0; i < _n; ++i)
            {
                s += ' ';
                s += std::to_string(_stats[i]);
            }
            return s;
        }

        bool fromString(const char* s)
        {
            // opposite of tostring
            assert(!_stats);
            assert(!_n);

            bool r = ParseBase::parseUInt(&s, _n)
                && ParseBase::parseUInt(&s, _maxp);

            r = _n > 0 && _maxp > 0;
            if (r)
            {
                // read n integers into stats
                _stats = new unsigned int[_n];
                for (int i = 0; i < _n && r; ++i)
                    r = ParseBase::parseUInt(&s, _stats[i]);
            }
            
            return r;
        }
        
    };

    struct GenState : public var::Blob
    {
        Strandi*        _host;
        Term::RType     _type;
        int             _seq = 0;
        int             _n = 0;  // used for seq & random
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

                // NB: mark as changed even if we're already at seq=0
                // this is because we have logically changed and we want
                // to write to the timeline the initial state
                changed = true;
                _seq = 0;
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

        string toString() const override
        {
            string s;
            char buf[128];
            
            s += '{';
            s += Term::rtypeString(_type);
            switch (_type)
            {
            case Term::t_random:
                s += ' ';
                s += std::to_string(_n);
                break;
            case Term::t_shuffle:
            case Term::t_nonrandom:
                assert(_nr);
                s += ' ';
                s += _nr->toString();
                break;
            case Term::t_sequence:
                sprintf(buf, " %d %d", _n, _seq);
                s += buf;
                break;
            case Term::t_first:
                break;
            case Term::t_all:
                break;
            default:
                s += " unknown";
                break;
            }
            s += '}';
            return s;
        }

        bool _fromStringNR(const char* s)
        {
            // make dummy
            NRan* nr = new NRan(&_host->_rand, 0, 0);

            // restore
            bool r =  nr->fromString(s);
            if (r) _nr = nr;
            else delete nr;

            //LOG1("fromStringNR ", s << " = " << r);
                        
            return r;
        }

        bool fromString(const char* s)
        {
            // opposite of toString
            bool r = *s == '{';
            if (r)
            {
                int l = ParseBase::atSimpleName(++s);
                r = l > 0;
                if (r)
                {
                    string ts(s, l); s += l;
                    ParseBase::_skipws(&s);
                    
                    if (ts == "random")
                    {
                        // N
                        _type = Term::t_random;
                        r = ParseBase::parseInt(&s, _n);
                    }
                    else if (ts == "shuffle")
                    {
                        _type = Term::t_shuffle;
                        r = _fromStringNR(s);
                    }
                    else if (ts == "nonrandom")
                    {
                        _type = Term::t_nonrandom;
                        r = _fromStringNR(s);
                    }
                    else if (ts == "sequence")
                    {
                        // N SEQ
                        _type = Term::t_sequence;
                        r = ParseBase::parseInt(&s, _n)
                            && ParseBase::parseInt(&s, _seq);
                    }
                    else if (ts == "first")
                    {
                        _type = Term::t_first;
                    }
                    else if (ts == "all")
                    {
                        _type = Term::t_all;
                    }
                    else r = false;
                }
            }
            return r;
        }

        friend std::ostream& operator<<(std::ostream& os, const GenState& g)
        { os << g.toString(); return os; }
    };

    var::Blob* genStateParser(const char** sp)
    {
        // this is used as the var format blob parser
        // in order to restore genstates from the timeline
        
        GenState* gs = 0;
        const char* s = *sp;

        if (*s == '{')
        {
            //LOG1("genStateParser ", s);
            
            gs = new GenState(this);
            bool r = gs->fromString(s);
            if (r)
            {
                // to find the end, simple scan for close '}'
                while (*s && *s != '}') ++s;
                if (*s)
                {
                    ++s; // park just after close '}'
                }
                else
                {
                    r = false; // failed
                }
            }

            if (!r)
            {
                // failed
                delete gs;
                gs = 0;
            }
        }

        if (gs) *sp = s;
        return gs;
    }
    
    Strandi(Terms* t = 0) : _terms(t) 
    {
        _init();
        
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
            LOG3(TAG "emit picture ", em._filename);
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

    void emitPrompt(const string& s)
    {
        GrowString gs;
        gs.add('{');

        JSONWalker::addKeyValue(gs, IFI_PROMPT, s.c_str());
        gs.add('}');
        gs.add(0);
        LOG3(TAG "emit prompt ", gs.start());
        ifi->emitResponse(gs.start());
    }
#endif // IFI_BUILD

    enum ArtScopeType
    {
        artscope_none = 0, // do not resolve artscope at all
        artscope_instances, // things that are not classes
        artscope_all,  // everything
    };

    struct ResInfo
    {
        pnode*  _pn;
        bool    _valid = true;  // not valid means resolve failed
        bool    _empty = false; // resolved to nothing?

        // resolve articles, "the", "my", "this" etc
        ArtScopeType _artScope = artscope_none;
        
        // resolve to interactive scope (ideal)
        bool    _toScope = false;

        // remember why scope failed
        bool    _scopeFailed = false;
        bool    _artFailed = false;

        bool    resolved() const { return _valid && !_empty; }

        ResInfo(pnode* pn) : _pn(pn) {}
        
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
            LOG2(TAG "run code, missing evaluator for ", *ec);
        }
    }

    bool run(Flow::EltCommand* c)
    {
        bool v = true;
        if (c->_parse)
        {
            ResInfo ri(c->_parse);

            // enable only article scoping but keep resolution scope global
            // because we are running at author/load time.
            // this means authoring commands must make their words precise
            // (eg adjectives) or use IDs.
            ri._artScope = artscope_all;
                        
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
                bool done = exec(ei);
                
                // retrieve manual break request from exec
                if (ei._break)
                {
                    v = false;
                    //LOG1("command had break ", textify(c->_parse));
                }
                else
                {
                    if (done) updateScope();
                    else
                    {
                        //LOG3(TAG "failed to exec command ", textify(c->_parse));
                        if (_errorNocando)
                        {
                            if (!run(_errorNocando)) v = false; // can break?
                        }
                    }
                }
            }

            if (!ri._valid)
            {
                ERR1("flow cannot resolve", textify(c->_parse));
            }

            clearBindings(c->_parse);
        }
        else
        {
            ERR1("Command has no parse ", c->toString());
        }
        return v;
    }

    bool prerun(Flow::EltTerm* et, bool& dobreak)
    {
        // handle cases of immediate eval of term element
        // return true if done, otherwise need to run term itself.
        
        bool done = true;
        dobreak = false;
        
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
                    setTick(t);
                }
                else if (et->_flags & Flow::ft_reset)
                {
                    resetTerm(t);

                    if (t == _currentTickTerm)
                    {
                        //LOG1("reset term, breaking flow ", t->_name);
                        
                        // indicate we wish to break from flow
                        dobreak = true;
                    }
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
        bool dobreak;
        if (!prerun(et, dobreak))
        {
            v = run(et->_term);
        }
        else
        {
            if (dobreak) v = false; // simulate break but from reset
        }
        return v;
    }

    void run(Flow::EltMedia* em)
    {
        if (_runtypemask & run_media)
        {
            // when we run media we send a request
#ifdef IFI_BUILD

            // flush before emitting picture so that text and pictures
            // are emitted in the correct order. 
            flush();
            
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

    bool runAfterCommand(Flow& f)
    {
        // expect a flow with a command followed by other terms
        // skip the command and run the remainder of the flow
        bool v = true;
        bool begun = false;
        for (auto i = f._elts.begin(); i != f._elts.end(); ++i) 
        {
            Flow::Elt* e = i;
            if (!begun)
            {
                if (e->_type == Flow::t_command) begun = true;
                continue;
            }
                 
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

        // NB: null matching might no longer be needed?
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
            
            // capitalise start, if needed.
            if (!_text.empty())
                _text[0] = u_toupper(_text[0]);
            
            _action = _reactor->_s;
        }

        bool specialMatch() const { return _nullmatch || _setmatch; }
        string id() const { return _reactor ? _reactor->id() : _action->id(); }

        bool isInput() const
        {
            assert(_action);
            return _action->isInput();
        }
                
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

        string              _prompt;
        string              _line;  // input
        int                 _ch; // choice

        // when cmd expects a nounphrase allow "nothing" to break
        bool                _allowNothing = false;

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

        int  choiceCount() const
        {
            // number of selected choices
            int c = 0;
            for (auto& ci : _choices) if (ci._selected) ++c;
            return c;
        }
    };

    void flush()
    {
        // emit the current capture
        string t = textify(_popcap()->_cap);
        _pushcap();
        if (!t.empty())
        {
            _emit(t);
            _emit((char)0); // force
        }
    }


    void setTermValue(Term* t, const var& v)
    {
        _state.setfn(t->_name, "=", v);
    }

    bool setVisited(Term* t)
    {
        return _state.set(t->_name);
    }

    bool getVisited(Term* t)
    {
        return _state.test(t->_name);
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

            if (_runSingleTermBreak)
            {
                //LOG1("IFI yield, single term break", "");
                cmdWaiting = false;
                break;
            }
            
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
                    break; 
                }
            }
            else
            {
                // no response means shutdown unless we're coop
                // in which case we pump and pump tells us to shutdown
                
                bool shutdown = true;
                
                if (ifi->_coop)
                {
                    assert(ifi->_pump);
                    shutdown = !(ifi->_pump)();
                }

                if (shutdown)
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
        
        int nchoices = c.choiceCount(); // those selected

        bool res = (nchoices > 0); // something to do
        
        if (!res)
        {
            // even if no choices need to request text input
            res = c._cmdChoice != 0;
        }

        if (res)
        {
            gs.add('{');
            JSONWalker::addKey(gs, IFI_CHOICE);

            // always uses the nested syntax
            gs.add('{');

            if (c._headFlow.size())
                JSONWalker::addStringValue(gs, IFI_TEXT, c._headFlow);

            // request or deny text input
            JSONWalker::addBoolValue(gs, IFI_UI_TEXTINPUT, c._cmdChoice != 0);
        
            if (nchoices > 0)
            {
                JSONWalker::toAdd(gs);
                JSONWalker::addKey(gs, IFI_CHOICE);
                gs.add('[');

                int cc = 0;
                for (auto& ci : c._choices)
                {
                    if (!ci._selected) continue;  // ignore these
                    
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
        }

        //LOG3(TAG "choice json ", gs.start());
        return res;
    }
    
    void presentChoicesIFI(Choices& c)
    {
        GrowString gs;
        bool v = buildChoiceJSON(gs, c);
        
        // head flow will normally be put in the UI choice box
        // unless it's just text input.
        if (!c.choiceCount())  // no active choices
        {
            OUTP(c._headFlow);
        }

        if (v)
        {
            // emit the current capture before the UI
            flush();

            emitPrompt(c._prompt);

            // emit the choices or request command line
            ifi->emitResponse(gs.start());

            // this will block for non-coop or poll for coop
            yield();
        }
    }
     
#else // !IFI_BUILD  
    
    void presentChoicesConsole(Choices& c)
    {
        // for console mode, we just emit head flow
        // then emit the choices as text
        OUTP(c._headFlow);

        // emit the current capture
        flush();

        _emit('\n');

        int nc = 0;
        for (auto& ci : c._choices)
        {
            if (!ci._selected) continue; // ignore
            
            char buf[16];
            sprintf(buf, "(%d) ", ++nc);
            _emit(buf);
            _emit(ci._text);
            _emit('\n');
        }

        if (c._prompt.empty())
        {
            // default prompt
            if (c._cmdChoice)
                _emit("> ");   // when we can also type a command
            else
                _emit("? ");   // when only choices available
        }
        else
        {
            _emit(c._prompt);
            _emit(' ');
        }
    }
#endif // IFI_BUILD    
    
    
    bool validateInput(Choices& c)
    {
        // store choice in c.ch
        // 0=> cmd
        // >0 => choice
        // <0 => invalid
        
        bool v = false;

        int nchoices = c.choiceCount(); // selected count

        c._ch = -1;  // mark invalid

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
                
                if (ch > 0 && ch <= nchoices)
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

#if defined(__EMSCRIPTEN__) && !defined(IFI_BUILD)
    string _getline()
    {
        extern std::string em_getline(); // emscripten
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

    void acceptInput(Choices& c)
    {
        // Input is converted before parsing.
        // input is converted to lower case to prevent the user
        // from entering terms directly.
        //
        // However, raw terms are used by autolinks and these should
        // not be converted.
        //
        // In general, if we are running a debug command, then we do not
        // convert to lower case.
        // A debug command is a verb starting with underscore "_".
        //

        bool downcase = true;

#ifdef LOGGING
        // debug builds never downcase so we can enter terms directly
        downcase = false;
#endif        
        
        string line = _getline();
        
        if (!line.empty())
        {
            char c1 = line[0];
            if (c1 == '_') downcase = false;
            if (downcase) line = toLower(line);
        }
        c._line = line;
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

            if (!dcmd.empty())
            {
                // "??" lists all reactors
                // ?foo lists only for FOO@
                // ?state lists the timeline state

                if (dcmd == "state")
                {
                    string s1 = _state.toString();
                    _emit(s1);
                    _emit('\n');

                    //Timeline t2;
                    //t2.fromString(s1.c_str(), &_vfmt);
                    //LOG1("restored as: ", t2);
                }
                else
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

                    if (!cc && !any)
                    {
                        Term* t = Term::find(dcmd);
                        if (t && !t->isObject())
                        {
                            string s = t->toString();
                            _emit(s);
                            _emit('\n');
                        }
                    }
                }
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
        assert(c.size() == c.choiceCount()); // will all be selected
        
        LOG3(TAG "choice made ", ch);

        Choice& cc = c._choices[--ch];
        Selector* s = cc._action;
                
        // mark as chosen.
        if (s->once()) _state.set(cc.id());
                
        // arrange for newline after choice and before subsequent
        // text, ONLY if there is text!
		// XX why is this needed?
        //requestNL();

        if (cc._reactor)
        {
            // is a command choice
            assert(cc._reactor->_reactor);
            execInfo ei(cc._reactor->_reactor);
            if (prepareExecInfo(ei) && execValidate(ei))
            {
                setItThat(ei, 0);
                execReaction(ei, cc._reactor->_s);
                updateIt(ei);
                c._valid = !ei._break;
            }
        }
        else
        {
            // choices remember their last chosen choice text when sticky
            if (c._t->sticky())
                setTermValue(c._t, cc._text);

            c._valid = run(s->_action);
        }
    }

    bool handleCmd(Choices& c)
    {
        bool done = false;

        LOG3(TAG "command ", c._line);

        //requestNL();
        
        // try to parse command and execute
        pnode* ps = _pcom.parse(c._line);
        if (ps)
        {
            // create bindings
            ResInfo ri(ps);

            // restrict input to resolution scope
            ri._toScope = true;
            ri._artScope = artscope_all;
            
            resolve(ri);
            if (ri.resolved())
            {
                execInfo ei(ps);

                // perform resolution against reactor match
                ei._resolving = true;
                ei._internalOps = false; // prevent internal "put"

                // if exec changes an object, mark it as explored/visited
                _ctx->_visitObj = true; 
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
                            c._valid = run(_errorNocando);
                    }
                }

                _ctx->_visitObj = false;

            }
            else
            {
                // no such object
                if (_errorNosuch)
                    c._valid = run(_errorNosuch);
            }

            // will also delete bindings
            delete ps;
        }
        else
        {
            //LOG3(TAG "command parse failed ", c._line);
            if (_errorSyntax)
                c._valid = run(_errorSyntax);
        }
        return done;
    }

    Binding* handleNounParse(Choices& c)
    {
        // NB: caller must delete returned binding
        Binding* b = 0;

        LOG3(TAG "noun parse '", c._line << "'");

        // try to parse command and execute
        pnode* ps = _pcom.parseNoun(c._line.c_str());
        if (ps)
        {
            // create bindings
            ResInfo ri(ps);

            // restrict input to resolution scope
            ri._toScope = true;
            ri._artScope = artscope_all;
            
            resolve(ri);
            if (ri.resolved())
            {
                pnode* pn = ps->getBindingNode();
                if (pn)
                {
                    b = pn->_binding;
                    pn->_binding = 0; // detach
                }
            }
            else
            {
                // no such object
                if (_errorNosuch) run(_errorNosuch);
            }


            // will also delete bindings
            delete ps;
        }
        else
        {
            if (_errorSyntax) run(_errorSyntax);
        }
        return b;
    }
    
    bool runChoices(Choices& c)
    {
        // ch > 0 will be a choice
        // ch == 0 is a cmd.
        // return true if handled

        bool r;
        if (c._ch > 0)
        {
            handleChoice(c);
            r = true;
        }
        else
        {
            // handle command
            assert(c._cmdChoice);

            r = handleDebugCmd(c);
            if (!r) r = handleCmd(c);
        }
        return r;
    }

    var _evalEnodeTerm(eNodeCtx* ctx, const char* name)
    {
        // terms in expressions evaluate according to their visit status
        // either as numeric 1 or 0.
        assert(name && *name);

        var v(0);

        bool done = false;
        
        Term* t = Term::find(name);
        if (t)
        {
            // terms can be marked "runConditional" which means they are "run"
            // when encountered in conditional expressions. Such terms are expected
            // to return EXEC_TRUE or EXEC_FALSE
            //
            // Or the term can be pure, where it is run and does not have to
            // be marked as runConditional.
            
            if (t->runConditional() || t->isPure())
            {
                _pushcap();
                bool r = _run(t);
                CapRef capr = _popcap();
                const Capture& cap = capr->_cap;

                done = true;

                //LOG3("Eval node, eval pure term ", name << " = " << cap);
                
                if (cap.matchSimple(EXEC_TRUE))
                {
                    v = 1;
                }
                else if (cap.matchSimple(EXEC_FALSE))
                {
                    // v already 0
                }
                else
                {
                    // pure term should be true or false
                    // treat as false, but emit a warning
                    LOG3("WARNIG Eval node, degenerate runConditional term ", name << " = " << cap);                    
                }
                
                if (!r)
                {
                    // should not get breaks inside pure terms as they
                    // have no side effects. In any case they will be ignored.
                    LOG1("WARNING, break inside conditional runConditional term ", name);
                }
            }
        }
        else
        {
            LOG1("WARNING, cannot locate term ", name);
        }

        if (!done && _state.test(name)) v = 1;
        
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

        var v = ectx.eval(en);

        //LOG1("evaluating conditional ", en->toString() << " = " << v);
        
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
                assert(ec->_cond);
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

            // the state stores the next sequence# to try, we return this one.
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
                LOG1("handleGenTypes, failed to find selector ", c._t->_name);

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
        }

        //LOG1("++genstate ", c._t->_name << " was " << (g1 ? g1->toString() : string("null")) << " to " << *g2 << " currently " << ch);

        if (g2 && g2 != g1)
        {
            setGenState(c._t->_name, g2); // consumes g2
        }
        
        return ch;
    }
    
    bool runGeneratorAction(Choices& c)
    {
        // return true if done something
        
        int nchoices = c.choiceCount(); // those selected

        if (!nchoices && c._matching)
        {
            // no matches, then we choose from fallbacks
            // which are those matches with empty flows
            for (auto& ci : c._choices)
            {
                // evaluated flow, but is empty?
                bool v = !ci.specialMatch() && !ci.isInput() &&
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
        //
        // when we're not matching, we have all the selectors as choices
        // except those failing conditional.
        if (c._matching)
        {
            bool ok = rtype == Term::t_random || rtype == Term::t_first || rtype == Term::t_all;

            if (!ok)
            {
                LOG1("WARNING, matching TERM must be random, first or all ",
                     c._t->_name);

                // fallback to random
                rtype = Term::t_random;
            }
        }
        
        int ch = 0;

        // NB: if "all", then all are selected, but conditionals have
        // not yet been evaluated.

        if (nchoices > 1) switch (rtype)
        {
        case Term::t_random:
            // this never has state
            ch = _rand.gen32() % nchoices;
            break;
        case Term::t_first:
            // ch = 0 selects first selected
            break;
        case Term::t_all:
            break;
        default:
            ch = handleGenTypes(c, nchoices);
            break;
        }

        assert(ch >= 0 && ch < nchoices);

        // run all the selectors!
        bool doall = rtype == Term::t_all;
        
        // find the ch'th selected choice unless all
        for (auto& ci : c._choices)
        {
            if (doall)
            {
                // evaluate conditionals as we go, as these can change
                ci._selected = checkSelectorCond(ci._action);
            }
            
            if (ci._selected)
            {
                if (doall || !ch)
                {
                    // run this choice
                    if (ci.isInput())
                    {
                        assert(!c._cmdChoice);

                        // assign this as the command selector
                        c._cmdChoice = ci._action;
                        c._allowNothing = true;
                        c._valid = runInputSelector(c);
                    }
                    else
                    {
                        // select is defined when we're matching as we've already
                        // run the text flow in order to perform the match.
                        // when we're not matching, the selector flow is part of the output
                        if (!ci._select)
                            c._valid = run(ci._action->_text);

                    }

                    // but we always run the action
                    if (c._valid)
                        c._valid = run(ci._action->_action);

                    if (!doall) break;  // done
                    
                }
                --ch;
            }
        }

        return true;
    }

    bool runInputSelector(Choices& c)
    {
        // return false if break
        // although this shouldn't be the case inside input?

        assert(c._cmdChoice);
        Selector* s = c._cmdChoice;
        
        assert(s);
        assert(s->isInput());

        // mark all selectors as not selected
        // because we are just prompting for text input
        for (auto& ci : c._choices) ci._selected = false;

        // capture any flow to be used as the prompt
        _pushcap();
        bool v = run(s->_text);
        CapRef capr = _popcap();
        if (!v) return false; // break

        // change prompt if defined
        c._prompt = trim(textify(capr->_cap));
        
        // Special handing when sticky
        // If we're sticky, it's because we want to capture the input
        // We might also have some capture already that will need to
        // be flushed because it needs to be seen before input.
        //
        // A sticky term might well have flow that would normally become
        // part of the sticky value. But for input, we dont want it. only
        // the actual input.
        //
        // If sticky, pop and dub any current capture to lower capture.
        // then flush, which will emit if we're level 1

        for (;;)
        {
            if (c._t->sticky())
            {
                CapRef cs = _popcap();

                // dub onto base cap
                outCap(cs->_cap);
            }
        
            // there are no selected choices, only a text input
            // this will flush and get input
            presentChoices(c);

            // If sticky, then need to re-establish capture so sticky term
            // becomes the input.
            if (c._t->sticky()) _pushcap(_runtypemask);
        
            if (s->isExec())
            {
                if (handleDebugCmd(c)) break;

                // will continue to loop if input fails.
                if (handleCmd(c)) break;  
            }
            else if (s->isParse())
            {
                if (c._allowNothing)
                {
                    // test for break input
                    if (c._line == "nothing" || c._line == "none")
                    {
                        // in such case emit string
                        OUTP("nothing");
                        return true;
                    }
                }
                
                // if this does not parse, an error will be emitted
                // and we loop round again
                Binding* b = handleNounParse(c);
                if (b)
                {
                    LOG3("runInputSelector, parsed: ", textify(b->_terms));
                    for (auto t : b->_terms) OUTP(t);
                    delete b;

                    break;
                }
            }
            else
            {
                // string input (will go sticky if pushed)
                OUTP(c._line);
                break;
            }
        }
        return true;
    }

    bool runGenerator(Choices& c)
    {
        // return false for stack break
        
        // top flow is input for matching generators
        CapRef topflowcap;

        // track whether we use the topflow
        bool unusedTopflow = false;

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

            // we have some content in the topflow, as yet unused.
            if (topflowcap->_cap) unusedTopflow = true;
        }

        // install results of topflow as LAST
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
            if (c._t->_flow)
            {
                unusedTopflow = false; // used!
                c._valid = run(c._t->_flow);
            }
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
                    // totally ignore input selectors when matching
                    bool v = !s->isInput();

                    // if we have any selectors, the topflow is used
                    unusedTopflow = false;
                    
                    // ignore failed conditionals
                    v = v && checkSelectorCond(s);
                    
                    if (v)
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

                if (unusedTopflow)
                {
                    // pure term.
                    // this is where we had a non-empty topflow
                    // and no selectors and no head flow
                    // in such case, the term value becomes the topflow
                    //LOG4(TAG "unused topflowcap in ", c._t->_name << " = " << topflowcap->_cap.toString());
                    assert(topflowcap);
                    outCap(topflowcap->_cap);
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
                    //
                    // a selector here could be an input selector
                    // this gets added like the others and handled later.
                    Choice ch(s);

                    ch._selected = 1; 

                    // Mark selected if condition true, but always add
                    // anyway.
                    // if "all", do not evaluate condition here as it
                    // may change as we execute multiple selectors.
                    // conditional is evaluated later.
                    if (c._t->_rtype != Term::t_all)
                        ch._selected = checkSelectorCond(s);
                    
                    c._choices.emplace_back(ch);
                }

                runGeneratorAction(c);
            }
        }
        
        if (stickpushed)
        {
            assert(c._t->sticky());

            // If sticky, collect the capture and set the term value
            // for next time.
            
            CapRef cs = _popcap();

            if (c)
            {
                // save sticky
                setTermValue(c._t, cs->_cap.toVar());

                // also we want to emit the value this time
                // unless we were actually an input.
                // that's because the user has already typed the input
                // and we will otherwise just repeat it.
                //
                // cmdChoice is only set for input generators.
                if (!c._cmdChoice)
                {
                    // dub onto base cap
                    outCap(cs->_cap);
                }
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
        /* try to figure out whether this command if elevated to choice
         * actually does not do anything.
         */
        
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
        // elevated to choice or if we are inheriting from an object.
        
        bool r = false; // return true if choice was added
        
        if (selectorShown(s, reactor))
        {
            if (s->_text)
            {
                if (s->_isReactor)
                {
                    // this is a command masquerading as a choice
                    assert(reactor);
                    
                    pnode* pn = reactor->_reactor;
                    assert(pn);

                    if (!suppressElevatedChoice(s))
                    {
                        string ctext;

                        if (s->_text.size() > 1)
                        {
                            // flow has additional elements than
                            // just command. So this end flow is
                            // used to form the actual menu text
                            // rather than a textified command
                            _pushcap();
                            c._valid = runAfterCommand(s->_text);
                            CapRef ccap = _popcap();

                            if (c && ccap->_cap)
                                ctext = textify(ccap->_cap);
                        }

                        // if no end flow or it didn't generate
                        // textify the command
                        if (ctext.empty())
                        {
                            // convert the bound reaction into plain text
                            ctext = textifyFancy(pn);
                            //LOG3("creating choice ", ctext << " from " << pn->toStringStruct());
                        }

                        if (!ctext.empty())
                        {
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

    bool prepareTermChoice(Choices& c, Selector* s, Term* t)
    {
        // called when we are preparing from a specific term
        // If that term is an object, we want to include all
        // reactors (not just ones elevated).
        bool v = false;
        
        if (t && t->isObject())
        {
            assert(s->_isReactor);

            // find the current reaction corresponding to this selector.
            // There will be one if the object is in scope.
            // Otherwise ignore it and do not add a choice
            for (auto& ri : _ctx->_reactions)
            {
                if (ri._s == s)
                {
                    v = prepareChoice(c, s, &ri);
                    break;
                }
            }
        }
        else
        {
            // non-object version
            v = prepareChoice(c, s, 0);
        }
        return v;
    }

    void prepareTermFillerChoices(Term* t, Choices& c, int& maxfill)
    {
        // pad with filler choices?
        for (auto s : t->_selectors._selectors)
        {
            if (s->filler())
            {
                if (prepareTermChoice(c, s, t)) --maxfill;
                if (!c || maxfill <= 0) break;
            }
        }

        if (c && maxfill > 0)
        {
            for (auto& i : t->_topflow._elts)
            {
                Flow::Elt* e = &i;
                assert(e->_type == Flow::t_term);
                Term* ti = ((Flow::EltTerm*)e)->_term;
                assert(ti); // assume linked

                // we allow choices or objects as parents
                if (!ti->isChoiceOrObject()) continue;

                // add in standard choices from topflow recursive.
                prepareTermFillerChoices(ti, c, maxfill);
                if (!c || maxfill <= 0) break;
            }
        }
    }

    int prepareTermTerminalChoices(Term* t, Choices& c, bool addin)
    {
        // add terminals or just count them
        int tc = 0;
        for (auto s : t->_selectors._selectors)
        {
            if (s->terminal())
            {
                if (addin)
                {
                    if (prepareTermChoice(c, s, t)) ++tc;
                }
                else
                {
                    // otherwise just count them
                    if (selectorShown(s)) ++tc; 
                }
                if (!c) break;
            }
        }

        if (c)
        {
            for (auto& i : t->_topflow._elts)
            {
                Flow::Elt* e = &i;
                assert(e->_type == Flow::t_term);
                Term* ti = ((Flow::EltTerm*)e)->_term;
                assert(ti); // assume linked
            
                // we allow choices or objects as parents
                if (!ti->isChoiceOrObject()) continue;

                // add in standard choices from topflow recursive.
                tc += prepareTermTerminalChoices(ti, c, addin);
                if (!c) break;
            }
        }
        return tc;
    }

    void prepareTermStdChoices(Term* t, Choices& c)
    {
        // standard choices are those that are not terminals nor fillers.
        // Add these and any elevated commands
        for (auto s : t->_selectors._selectors)
        {
            if (!s->terminal() && !s->filler())
            {
                prepareTermChoice(c, s, t);
                if (!c) break;
            }
        }
        
        if (c)
        {
            // recurse to parent choices
            for (auto& i : t->_topflow._elts)
            {
                Flow::Elt* e = &i;
                assert(e->_type == Flow::t_term);
                Term* ti = ((Flow::EltTerm*)e)->_term;
                assert(ti); // assume linked
            
                // we allow choices or objects as parents
                if (!ti->isChoiceOrObject()) continue;
                
                // add in standard choices from topflow recursive.
                prepareTermStdChoices(ti, c);
                if (!c) break;
            }
        }
    }

    void prepareChoices(Choices& c)
    {
        Term* t = c._t;
        assert(t);
        
        prepareTermStdChoices(t, c);
        
        if (c)
        {
            // add in commands elevated to choice?
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

        // how many fillers can we accommodate?
        int maxfill = t->_idealChoiceCount - c.size();
        
        if (maxfill > 0)
        {
            // but need space for terminals
            maxfill -= prepareTermTerminalChoices(t, c, false);
        }

        if (maxfill > 0)
        {
            // accommodate fillers
            prepareTermFillerChoices(t, c, maxfill);
        }

        // finally, add in the terminals
        prepareTermTerminalChoices(t, c, true);
        
        // flatten choice text
        if (c) for (auto& ci : c._choices)
        {

            // the selected field is used mainly for generators
            // to mark the ones to choose from
            // for choices, we dont put the non selected ones on
            // the list, so they are all "selected"
            //
            // but mark selected as the UI builder ignores non-selected
            ci._selected = true;
            
            if (ci._text.empty())
            {
                ci._text = textify(ci._select->_cap, true);
                if (!ci._text.empty())
                {
                    // ensure starts with caps.
                    ci._text[0] = u_toupper(ci._text[0]);
                }
            }
        }
    }

    void runChoiceHead(Choices& c)
    {
        // choice does not run the headflow (nor tail)
        // if there are no actual valid choices

        // run and capture
        _pushcap();
        c._valid = run(c._t->_flow);  // collect manual break
        CapRef ccap = _popcap();
        if (c) c._headFlow = textify(ccap->_cap);
    }

    void presentChoices(Choices& c)
    {
        // present input until accepted
        for (;;)
        {
#ifdef IFI_BUILD
            presentChoicesIFI(c);
#else
            presentChoicesConsole(c);
#endif
            // blocks here in console mode otherwise reads input
            // already buffered for IFI
            if (!_runSingleTermBreak)
                acceptInput(c);

            // while input was been waiting, another command
            // executed a break
            if (_runSingleTermBreak)
            {
                //LOG1("presentChoices got a single Term break in ", c._t->_name);
                _runSingleTermBreak = false;
                c._valid = false; // propagate manual break
                break;
            }
            
            if (validateInput(c)) break;
        }
    }

    bool runChoicesFinal(Choices& c)
    {
        // return true if handled
        
        bool r = runChoices(c);
        if (r)
        {
            updateScope();

            if (c)
                c._valid = run(c._t->_postflow);
        }

        return r;
    }
    
    struct RuntimeTermStack
    {
        Term*               _term;
        bool                _jmp = false;
        RuntimeTermStack*   _prev;
        RuntimeTermStack(Term* t) : _term(t) {}
    };

    RuntimeTermStack*       _termStack = 0;

    // if we're currently running a background term, this is set
    Term*                   _currentTickTerm = 0;

    void _runTick()
    {
        ++_time;
        assert(_tick);

        LOG4(TAG "tick ", _time);

        // previous ticks are not retracted. Just keep adding
        // tick values to mark time.
        _state.setNoTest(_tick->_name, "=", _time); // wont already be true

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

                // remember the currently running background term
                // in case we reset this term!
                Term* ot = _currentTickTerm;
                _currentTickTerm = t;
                bool r = run(t);
                _currentTickTerm = ot;

                if (!r)
                {
                    LOG4("runtick received break, ", t->_name);
                }
            }
        }
    }


#define ONTICK(_m) \
    ((_m).tag() == TERM_TICK && (_m).prop() == "=")

    bool _undoMove()
    {
        bool r = false;

        if (_time > 1)   // cannot undo first more
        {
            Mark m = _state.beginMarkScan();

            // scan back to previous tick where change was made
            bool change = false;

            while (m)
            {
                // if we are parked on a TICK, then this doesn't count
                if (ONTICK(m))
                {
                    if (change) break;
                }
                else
                {
                    // is a property change
                    if (!m.prop().empty()) change = true;
                }
                --m;
            }

            if (m)
            {
                int t1 = m.val().toInt();
                LOG3("undoMove rewinding to time ", t1);
                
                _state.clearToMark(m);
                _time = t1;
                r = true;
                
                // scope needs recalculating
                _ctx->markScope();
            }
        }
        return r;
    }

    bool undoToTerm(const string& termname)
    {
        // rewind until `termname` was asserted.
        // this might not actually be the first assert if it was subsequently
        // denied, then re-asserted. In which case, the re-assert point is
        // located.
        // Then backup to the time tick and undo from there, in order to
        // undo complete moves.

        assert(_tick);
        
        bool r = false;
        LOG3("undoToTerm ", termname);

        Mark m = _state.beginMarkScan();

        // scan back to locate the tag itself
        _state.scanMarkFor(m, termname, string(), var());
        _state.scanMarkFor(m, _tick->_name, "=", var());
        
        if (m)
        {
            int t1 = m._p->_val.toInt();
            
            //LOG1("undoToTerm rewinding to time ", t1 << " from " << termname);
            
            if (t1 > 0)
            {
                LOG3("undoToTerm, rewinding to time ", t1);
                _state.clearToMark(m);
                _time = t1;
                
                r = true;
                
                // scope needs recalculating
                _ctx->markScope();
            }
        }
        else
        {
            LOG1("UndoToTerm, not found, ", termname);
        }
        
        return r;
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
        else if (t->_name == TERM_UNDO)
        {
            v = true;
            if (!_undoMove())
            {
                OUTP("Nothing to undo.");
                //LOG1("Can't undo ", _time);
            }
        }
        else if (t->_name == TERM_VERSION)
        {
            v = true;
            OUTP(var(VERSION " " BUILD_VER));
        }
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

    bool runChoiceOrGenerator(Term* t)
    {
        // return false for manual break
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
                    bool r;
                    do
                    {
                        presentChoices(choices);
                        v = choices;  // manual break already?
                        if (v)
                        {
                            r = runChoicesFinal(choices);
                            v = choices; // check for break
                        }
                        
                    } while (v && !r);
                }
            }
        }
        return v;
    }

    bool _run(Term* t)
    {
        // return false for break

        bool v = true;
        if (!_run1(t)) v = runChoiceOrGenerator(t);
        return v;
    }

    void setTick(Term* t)
    {
        _state.set(t->_name, TERM_TICK); // does not add if already
    }
    
    void resetTick(Term* t)
    {
        // remove from background flow
        // clear (term TICK) if present
        _state.clear(t->_name, TERM_TICK);
    }
    
    void resetTerm(Term* t)
    {
        // release all state from `t`
        // Does this also reset the term value (eg sticky)?

        //LOG1("Resetting term ", t->_name);
        
        // remove from background flow
        // clear (term TICK) if present
        resetTick(t);

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
                if (g2->reset())
                {
                    //LOG1("resetting gen state for ", t->_name << " to " << *g2);
                    setGenState(t->_name, g2);
                }
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
        
            // mark as visited at the end unless object
            // object visited state is insteasd used for "explored".
            
            // NB: still mark even if we are in break
            // because we are like a goto.
            
            if (!t->isObject() && setVisited(t))
            {
                // XX this is a bit annoying
                // reactions can have conditionals on visited terms
                // so when a term is visited, it can change the condition
                // and therefore change the current reaction set.
                //
                // for now we just mark changed if a new term is visited
                // but ideally we could check to see if any conditionals
                // use this term before forcing a recalculation of scope.
                _ctx->markScope();
            }

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
        
    bool restrictToInstanceVsClass(TermList &tl)
    {
        bool r = false;
        int instanceCount = 0;
        int classCount = 0;
        for (auto ti : tl)
            ti->isClass() ? ++classCount : ++instanceCount;

        if (instanceCount > 0 && classCount > 0)
        {
            // drop the classes and leave the instances
            auto it = tl.begin();
            while (it != tl.end())
            {
                if ((*it)->isClass())
                {
                    LOG4("Restrict instance vs class, dropping class ", (*it)->_name);
                    it = tl.erase(it);
                    r = true;
                }
                else ++it;
            }
        }
        return r;
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

        restrictToInstanceVsClass(l1);
        
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
        // all things in P (non-recursive).
        subPropTerms(tl, p, PROP_IN);
    }

    void subOnTerms(TermList& tl, Term* p)
    {
        // all things on P
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
    
    bool XinY(Term* x, Term* y)
    {
        // Is X in Y, recursive
        // true if X is in Y, or X is in something that is in Y (recursive)
        assert(x && y);

        // walk up from x to see if it is in Y
        Term* t = x;
        for (;;)
        {
            t = inTerm(t);
            if (!t) break; 
            if (t == y) return true;
        }

        return false;
    }

    Term* onTerm(Term* p)
    {
        // thing Y, p on Y, assuming p is only on ONE thing
        return propTerm(p, PROP_ON);
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
        // AND all things "on" things determined above.
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

        // Additionally we include things inside parents of things
        // this is so we can create generic parent objects that have
        // things. like people have hands and feet etc.

        // collect all parents of objects so far. make the parents
        // list first, since this has a lot of repeats and then
        // we only have to look at the contents of the combined parent list.
        TermList allparents;
        for (auto t : tl)
        {
            TermList pl;
            t->getParents(pl);
            concatm(allparents, pl);
        }

        // then take all the things IN this parent list that are not
        // already in scope.
        // NOTE: We're not using recursive in here.
        subPropTermsSet(tl, allparents, PROP_IN);

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

#define TL_CLEAR(_a)                    \
{                                       \
    delete (_a)->_binding;              \
    (_a)->_binding = 0;                 \
}

    void clearBindings(pnode* pn)
    {
        // delete all bindings in pn and below
        while (pn)
        {
            TL_CLEAR(pn);
            if (pn->_type != nodeType::p_value) clearBindings(pn->_head); 
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
        else if (*w == PRON_ALL)  // literal
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
                LOG4("Resolving scope; type:", st << ", " << textify(b));
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
            case nodeType::p_anoun: // adj noun
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
                ri._scopeFailed = true;  // log that scoping failed
                
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
                            TL_CLEAR(un);
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
                    // (tn (noun my))
                
                    assert(!pn->_binding);

                    pnode* ns = pn->_head;
                    pnode* art = pn->_head->_next;  // the, a, my etc
                    
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
                            // XX currently toscope => artscope
                            assert(!ri._toScope || ri._artScope);

                            // always?
                            bool ascope = ri._artScope == artscope_all;

                            if (!ascope)
                            {
                                if (ri._artScope == artscope_instances)
                                {
                                    // apply, unless all are classes
                                    ascope = true;
                                    
                                    // do not apply to classes
                                    for (Term* ti : ns->_binding->_terms)
                                    {
                                        if (ti->isClass())
                                        {
                                            ascope = false;
                                            break;
                                        }
                                    }
                                }
                            }
                            
                            if (ascope)
                            {
                                if (!resolveScope(*ns->_binding,
                                                  ns->_binding->_scope))
                                {
                                    // understood, but resolved to empty
                                    TL_CLEAR(ns);
                                    ri._empty = true;

                                    // remember that article failed
                                    ri._artFailed = true;
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

    bool _eiHelpIOBJ(execInfo& ei, pnode* pn)
    {
        // helper to fill out iobj or value
        //
        // pn is at prep phrase
        // prep is actual prep word (possibly inside phrase)
        // expect iobj or value following prep

        bool v = false;

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

        // at the moment should always get a prep
        assert(prep);

        // but otherwise it's failed
        if (!prep) return false; 

        // if our "prep" isnt, then these tests will fail and we're false
        if (prep->_type == nodeType::p_prep)
        {
            ei._prep = prep->_word;
            assert(ei._prep);

            // expect a NP

            pn = ei._iobjn = pn->_next;
            assert(pn && pn->isNounPhrase());
            
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

                // NB: leave iobj unset
            }
        }
        return v;
    }

    bool prepareExecInfo(execInfo& ei)   
    {
        bool v = false;
        
        if (!ei._ps) return false;

        if (ei._ps->isSentence())
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
                            // extract iobj or value
                            v = _eiHelpIOBJ(ei, pn);
                        }
                    }
                }
            }
        }
        else if (ei._ps->isQuery())
        {
            // query sentence

            // (Q prep rn)  eg (what in box)
            // (Q prop value) eg (what feel wet)
            // (Q N prep) eg (what box in)
            // (Q N prop) eg (what box feel)
            //
            // (IS N)
            // (IS VAL)
            // (IS N prep Y)
            // (is N prop V)

            // (Q DOBJ PREP NULL or Q NULL PREP IOBJ)

            // query goes into verb slot
            pnode* pn = ei._verbn = ei._ps->_head;
            if (pn)
            {
                ei._verb = pn->_word;
                assert(ei._verb);

                bool isare = pn->_word->isISARE();

                pn = pn->_next;
                if (pn)
                {
                    if (pn->isNounPhrase())
                    {
                        // what player in
                        // what box feels
                        // is N ...
                        
                        ei._dobjn = pn;
                        ei._dobj = pn->getBinding();
                        
                        if (ei._dobj)
                        {
                            assert(!ei._dobj->empty());
                            
                            // expect either prep or property
                            pn = pn->_next;
                            if (pn)
                            {
                                if (isare)
                                {
                                    // (is N prep Y)
                                    // (is N prop V)
                                    v = _eiHelpIOBJ(ei, pn);    
                                }
                                else
                                {
                                    assert(pn->_type == nodeType::p_prep || pn->_type == nodeType::p_property);
                                    ei._prepn = pn;
                                    ei._prep = pn->_word;
                                    assert(ei._prep);

                                    // (Q N prep)
                                    // (Q N prop)
                                    v = true;
                                }
                            }
                            else
                            {
                                // have only (is N), allowed
                                if (isare) v = true;
                            }
                        }
                    }
                    else if (isare)
                    {
                        // (is VAL)
                        
                        if (pn->_type == nodeType::p_value)
                        {
                            // head of value is actually an enode
                            GETENODE(pn);
                            ei._value = en;
                            v = true;
                            // we have dobj & iobj blank.
                        }
                    }
                    else
                    {
                        // (Q prep N)
                        // (Q prop V)
                        v = _eiHelpIOBJ(ei, pn);
                    }
                }
            }
        }
        
        return v;
    }

    bool execValidate(execInfo& ei)
    {
        bool v = ei._verb;

        if (v)
        {
            if (ei._verb->isVerb())
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
            else v = ei._verb->isQuery();
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

    bool execPutEach(execInfo& ei, Term* t)
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
                LOG2("PUT, preventing loop with ", *t);
            }
        }

        if (v)
        {
            if (multival)
            {
                v = _state.set(t->_name, prop, iobj->_name);
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

    bool execSetEach(execInfo& ei, Term* t)
    {
        bool multival = false;
        bool negate = false;
        
        if (ei._prepmod)
        {
            negate = ei._prepmod->isNegative();

            // XX assume if got prepmod
            multival = true;
        }

        var val;
            
        if (ei._iobj)
        {
            Term* t = ei._iobj->first();
            assert(t);
            val = t->_name;
        }
        else
        {
            assert(ei._value);
            val = evalEnode(ei._value);
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
                if (execPutEach(ei, t))
                {
                    // mark objects visited when they move
                    if (_ctx->_visitObj) setVisited(t);
                    v = true;
                }
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
        if (v) for (auto t : ei._dobj->_terms)
                   if (!execSetEach(ei, t)) v = false;
        if (v && ei._output) OUTP(EXEC_OK);
        return v;
    }

    bool queryXwithY(Term* x, Term* y)
    {
        // Is X with Y.
        // true if X is in Y, or X is in something that is in Y (recursive)
        // or X is on Y
        // or X is on something in Y.

        assert(x && y);

        Term* t = x;
        for (;;)
        {
            bool r = XinY(t, y);
            if (r) return true;

            // t on something?
            t = onTerm(t);
            if (!t) break;  // no
            if (t == y) return true; // X is on Y
        }

        return false;
    }

    bool execQuery(execInfo& ei)
    {
        //LOG1("execQuery, ", textify(ei._ps));

        // what prep N
        // what prop VAL
        // what N prep
        // what N prop

        // is/are
        // is N prep Y
        // is N prop V
        // is N
        // is VAL
        
        bool v = false;

        assert(ei._verb);
        
        bool isare = ei._verb->isISARE();
        
        if (ei._dobj)
        {
            // what N prep, eg what player in
            // what N prop, eg what box feels
            // is N
            // is N prep Y
            // is N prop V

            //LOG1("execQuery dobj, ", textify(ei._ps));
            
            // currently only query singleton N
            v = execChkSingleDO(ei);
            if (v)
            {
                Term* x = ei._dobj->first();
                if (isare)
                {
                    bool r = false;
                    
                    if (!ei._prep)
                    {
                        // what/is N
                        r = true;
                    }
                    else
                    {
                        if (ei._iobj)
                        {
                            // is X prep Y
                            // where Y is NP

                            v = execChkSingleIO(ei);
                            if (v)
                            {
                                Term* y = ei._iobj->first();
                                //LOG1("exec iobj QUERY ", *x << ' ' << *ei._prep << " " << val << " = " << *y);
                            
                                if (ei._prep->_text == PROP_WITH)
                                {
                                    r = queryXwithY(x, y);
                                }
                                else
                                {
                                    r = _state.test(x->_name, ei._prep->_text, y->_name);
                                }
                            }
                        }
                        else if (ei._value)
                        {
                            // is X prop Val

                            // evaluate the expression and test if true
                            var ev = evalEnode(ei._value);
                            r = _state.test(x->_name, ei._prep->_text, ev.toString());
                        }
                    }
                    
                    if (r)
                    {
                        OUTP(EXEC_TRUE);
                    }
                    else
                    {
                        OUTP(EXEC_FALSE);                            
                    }
                }
                else
                {
                    // what X prep
                    // what X prop

                    assert(ei._prep);
                    
                    // Wait!
                    // properties and prep can be multivalued!
                    // normally setting X in Y, replaces X in Z
                    // same for properties set X feels wet, replaces X feels dry.
                    // most of the time this is true, but they can be
                    // multivalued. get the value as if we are a function
                    // which will be the last one if multivalued.
                    //
                    // need a way for query to return a set.
                    
                    const var* y = _state.getfn(x->_name, ei._prep->_text);

                    //LOG1("execQuery dobj + !obj ", *x << ' ' << *ei._prep << " = " << (y ? y->toString(&_vfmt) : "null"));

                    if (y)
                    {
                        if (ei._prep->isPrep())
                        {
                            // expect term
                            Term* r = Term::find(y->rawString());
                            OUTP(r);
                            //LOG1("exec prep QUERY ", *x << ' ' << *ei._prep << " = " << textify(r));
                        }
                        else
                        {
                            // we dont expect a term, but it can be!
                            Term* r = Term::find(y->rawString());
                            if (r)
                            {
                                //LOG1("exec QUERY ", *x << ' ' << *ei._prep << " = " << textify(r));
                                OUTP(r);
                            }
                            else
                            {
                                // add raw value
                                //LOG1("exec QUERY ", *x << ' ' << *ei._prep << " = (val) " << y->rawString());
                                OUTP(*y);
                            }
                        }
                    }
                }
            }
        }
        else if (ei._iobj || ei._value)
        {
            // what prep N
            // what prop VAL
            // is VAL

            if (isare)
            {
                // is VAL ?
                assert(ei._value);
                v = true;
                
                var ev = evalEnode(ei._value);
                bool r = ev.isTrue();
                
                if (r)
                {
                    OUTP(EXEC_TRUE);
                }
                else
                {
                    OUTP(EXEC_FALSE);                            
                }
            }
            else
            {
                // otherwise expect a prep
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

    void execReaction(execInfo& ei, Selector* rs)
    {
        // pass in the reactor's selector `rs` as we dont use the
        // original reactor
        
        if (ei._it) _ctx->pushIt(ei._it);
        if (ei._that) _ctx->pushThat(ei._that);

        // keep track of the reaction we are running
        // NB: cannot hang onto reaction as this gets regenerated.
        //assert(!ei._currentSelector);
        ei._currentSelector = rs;

        // update focus with the object who owns this reactor
        //Term* t = ei._it;
        //if (t) _ctx->_focus.add(t);

        _pushExec(ei);
        bool v = run(rs->_action);
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
        //if (ei._dobji) { LOG4("Looking for reactor match for ", textify(ei._ps) << " for " << ei._dobji->_name); }

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
                    v = (rei._dobj != 0) == (ei._dobji != 0);

                    if (v && rei._dobj)
                    {
                        // insist they match
                        v = contains(rei._dobj->_terms, ei._dobji);
                        if (v) rank += 1000;
                        else
                        {
                            // are my terms all subtypes of template?
                            int d = subtype(ei._dobji, rei._dobj->_terms);
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

    void setItThat(execInfo& ei, Term* it)
    {
        // set "it" and "that" in the execInfo, from itself or given `it`.

        // currently always take "that" from itself.
        if (ei._iobj)
        {
            Term* t = ei._iobj->first();
            assert(t && t->isObject());
            ei._that = t;
        }

        if (!it)
        {
            if (ei._dobj)
            {
                it = ei._dobj->first();
                assert(it && it->isObject());
            }
        }

        if (it) ei._it = it;
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

    struct BindCluster
    {
        Binding         _b;
        Reaction*       _mr;     // note this does not remain valid after exec
        Selector*       _mrs;    // match reactor selector

#ifdef LOGGING        
        std::string     toString(Strandi& host)
        {
            std::string s;
            s = "reactor: ";
            s += host.textify(_mr->_reactor);
            s += " with binding ";
            s += host.textify(_b);
            return s;
        }
#endif        
    };

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
                // internal ops are primitive put and set, not allowed
                // on command line.
                if (*ei._verb == SYM_PUT && !ei._verbPrep)
                {
                    done = true;
                    v = execPut(ei);
                    if (v) _ctx->markScope();
                }
                else if (*ei._verb == SYM_SET)
                {
                    done = true;
                    v = execSet(ei);

                    // Does "set" really change scope??
                    if (v) _ctx->markScope();
                }
            }

            // normally we dont allow queries on the command line
            bool allowQuery = ei._internalOps;

#ifdef LOGGING
            // but we do for debug builds
            allowQuery = true;
#endif            

            if (!done && allowQuery)
            {
                // handle queries
                if (ei._verb->isQuery())
                {
                    done = true;
                    v = execQuery(ei);
                    if (!v)
                    {
                        LOG4("exec query failed ", textify(ei._ps));
                    }
                }
            }

            if (!done)
            {
                // match against current reactions
                if (ei._dobj)
                {
                    // for each dobj, find the best reactor and create
                    // a bind cluster with it and the dobj.
                    std::vector<BindCluster> bcs;
                    
                    BindCluster* bcilast = 0;
                    for (auto ti : ei._dobj->_terms)
                    {
                        ei._dobji = ti;
                        Reaction* mr = matchReactionInScope(ei);
                        if (!mr)
                        {
                            // XX need to know if the term was a multple
                            // match like "key" or "X and Y"
                            // since the latter should fail, the former
                            // should resolve.
                            LOG4("exec, no reactor for ", ti->_name);
                            continue;
                        }

                        Selector* mrs = mr->_s;

                        // if same as last reactor, then we merge the
                        // objects into one binding
                        // NB: compare selectors as reactors are already
                        // specialised.
                        if (bcilast && bcilast->_mrs == mrs)
                        {
                            // it doesn't matter that the specialised
                            // reaction has only one of the dobjs
                            // because the reactor has done it's job now
                            // and the binding list is used to exec.
                            bcilast->_b.add(ti);
                        }
                        else
                        {
                            BindCluster bci;
                                                
                            bci._b.add(ti);
                            bci._mr = mr;
                            bci._mrs = mrs;
                            bcs.push_back(bci);
                            bcilast = &bcs.back();
                        }
                    }

                    int sz = bcs.size();
                    v = sz > 0;

                    if (v)
                    {
                        // now perform resolution on each cluster
                        Binding* oldb = ei._dobj;

                        auto it = bcs.begin();
                        while (it != bcs.end())
                        {
                            ei._dobj = &(*it)._b;
                            
                            if (ei._resolving)
                            {
                                LOG4("exec, resolving cluster ", it->toString(*this));
                                if (!resolveMatchingReactor(ei, *it->_mr))
                                {
                                    it = bcs.erase(it);
                                    --sz;
                                    continue;
                                }
                            }
                            
                            ++it;
                        }

                        v = sz > 0;

                        // now execute each cluster

                        /* XX There is a potential problem here:
                         * if there are multiple items whose actions require
                         * a scope update, then the current reactors become
                         * invalid and it may not be possible to continue
                         * the exec.
                         * Example: get X and Y.
                         * If an action of getting X also causes Y to vanish
                         * the "get Y" reactor won't be in scope anymore.
                         * 
                         * To fix this, the reactors need to be matched again
                         * for each dobj.  TODO.
                         */
                        if (v)
                        {
                            assert(!ei._it);
                            assert(!ei._that);

                            // find overall size
                            int szt = 0;

                            // each binding might be multiple
                            for (int i = 0; i < sz; ++i)
                                szt += bcs[i]._b.size();

                            bool prompt = szt > 1;
                            
                            for (int i = 0; i < sz; ++i)
                            {
                                ei._dobj = &bcs[i]._b;

                                for (auto ti : ei._dobj->_terms)
                                {
                                    setItThat(ei, ti);
                                    
                                    if (prompt)
                                    {
                                        OUTP(ti);
                                        OUTP(": ");
                                    }

                                    //ei._currentSelector = 0;
                                    execReaction(ei, bcs[i]._mrs);

                                    --szt;
                                    if (prompt && szt) OUTP('\n');
                                }
                            }
                        }
                            
                        ei._dobj = oldb;
                    }
                }
                else
                {
                    Reaction* mr = matchReactionInScope(ei);
                    if (mr)
                    {
                        execReaction(ei, mr->_s);
                    }
                    else v= false;
                }
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

            // create a copy so that the original reactor can be
            // a template and we will resolve the copy.
            pnode* pn = ec->_parse->copy();
            ResInfo ri(pn);

            // since we are making copies, apply article scoping.
            // This may cause resolution to fail.
            // the upshot of this is as if the reaction were absent.
            //
            // However, we need to retain reactions defined on articled
            // classes.
            // eg give my GETTABLE to him
            // defined in the base class. So we need to apply scoping
            // only to instances.
            ri._artScope = artscope_instances;
            
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
                if (!ri._artFailed)
                {
                    // not an overt error if failed due to article scoping
                    ERR1("failed to resolve reactor", pn->toStringStruct());
                }
                delete pn;
            }
        }
    }

    static void expandParents(TermList& tl) 
    {
        for (auto t : tl)
        {
            TermList tpl;
            t->getParents(tpl);

            // reactions are added in reverse parent order
            // so that secondary parents can be added to override main
            // parents
            for (auto it = tpl.crbegin(); it != tpl.crend(); ++it)
                if (!contains(tl, *it)) tl.push_back(*it);
        }
    }

    void resolveTermReactions(Term* t)
    {
        TermList tl;
        tl.push_back(t);

        // make a complete list of parents breadth first.
        expandParents(tl);
        for (auto ti : tl) resolveReactions(ti);
    }

    void resolveAllReactions()
    {
        //LOG1("resolving scope reactions ", "");
        for (auto t : _ctx->_scope)
        {
            // within a reaction, "it" corresponds to itself
            // and also within parents
            BindIt bind(this, t);
            resolveTermReactions(t);
        }

        /* consider a second pass where reactors from objects in reference
         * scope are also added BUT only reactors of their root parent.
         *
         * This will bring in synonyms and general handlers of objects
         * mentioned but prevent direct methods on the objects. 
         *
         * It's important that these added reactors are from instances
         * and not classes. Otherwise we get rectors on "thing" etc.
         *
         * examples: try (l/x/look) walls
         */
        for (auto t : _ctx->_resolutionScope)
        {
            if (!t->isClass() && !contains(_ctx->_scope, t))
            {
                // within a reaction, "it" corresponds to itself
                // and also within parents
                Term* rp = t->rootParent();
                if (rp)
                {
                    BindIt bind(this, t);

                    // NB: This can extend `resolutionScope`
                    resolveReactions(rp);
                }
            }
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

#define DEF_SPECIAL_TERM(_var, _name)                           \
_var = Term::find(_name);                                       \
if (!_var && _objectCount) LOG1("Warning, there is no term, ", _name);   

    void _prepareObjectRuntime()
    {
        LOG4("prepare object runtime", "");
        
        // find well known objects
        DEF_SPECIAL_TERM(_player, TERM_PLAYER);
        DEF_SPECIAL_TERM(_thing, TERM_THING);

        _tick =  Term::find(TERM_TICK); // automatically added
        assert(_tick);

        _undo = Term::find(TERM_UNDO);
        assert(_undo);

        // may not exist
        _scopeSeed = Term::find(TERM_SCOPE);

        // optional handler when cannot resolve
        DEF_SPECIAL_TERM(_errorNocando, TERM_NOCANDO);
        DEF_SPECIAL_TERM(_errorNosuch, TERM_NOSUCH);
        DEF_SPECIAL_TERM(_errorSyntax, TERM_SYNTAX);

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
        LOG4("process object properties", "");
        
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

    bool autoLink(string& t)
    {
        bool r = true;
        bool changed = false;
        size_t start = 0;
        size_t sz = t.size();
        string res;
        for (;;)
        {
            size_t p1 = t.find('[', start);
            if (p1 == string::npos) break;
            
            size_t p2 = t.find(']', p1);
            if (p2 == string::npos) break;

            ++p2;  // char after ]

            if (p2 >= sz) break; // nothing following ']'

            // what follows ]
            if (t[p2] == '(') break; // link already defined

            // append up to and including end ]
            res.append(t, start, p2-start);

            string termwords = t.substr(p1+1, p2-p1-2); // adjust for +1

            // if empty, just skip over and ignore
            if (!termwords.empty())
            {
                bool resolved = false;
                
                LOG5("Parsing autolink ", termwords);
                pnode* pn = _pcom.parseNoun(termwords.c_str());
                if (pn)
                {
                    // create bindings
                    ResInfo ri(pn); // no scope restriction
                    resolve(ri);
                    resolved = ri.resolved();

                    if (resolved)
                    {
                        LOG4("Parsed autolink ", termwords << " = " << textify(pn));
                        Binding* b = pn->getBinding();
                        if (b)
                        {
                            int sz = b->size();
                            assert(sz > 0);
                            if (sz > 1)
                            {
                                LOG1("Warning autolink has multiple matches ", textify(pn));
                            }

                            Term* term = b->first();
                            assert(term);

                            res.append("(_be ");
                            res.append(term->_name);
                            res.append(")");
                            
                            changed = true;
                        }
                        else
                        {
                            LOG1("cannot resolve autolink ", termwords << " = " << textify(pn));
                            resolved = false; // not after all
                        }
                    }
                    
                    delete pn;
                }

                if (!resolved)
                {
                    LOG1("Failed to parse autolink; ", termwords);
                    r = false;
                }
            }
            start = p2;
        }

        if (changed)
        {
            // add in any remainder
            res.append(t, start, string::npos);
            
            // update!
            t = res;
        }
        
        return r;
    }
                      

    bool autoLinks(Flow& f, FlowVisitor& fv)
    {
        bool v = true;
        for (auto i = f._elts.begin(); i != f._elts.end(); ++i)
        {
            Flow::Elt* e = i;
            if (e->_type == Flow::t_text)
            {
                Flow::EltText* et = (Flow::EltText*)e;
                if (!autoLink(et->_text)) v = false;
            }
        }
        return v;
    }

    bool prepareAutoLinks(bool err = true)
    {
        // Autolinks are no longer created by searching the raw text
        // for objects. This causes too many false positives
        // instead look for markup of the form:
        //
        // bla bla bla [thing] bla bla
        //
        // and convert to
        //
        // bla bla bla [thing](_be THING) bla bla

        LOG4("prepare auto links", "");
        
        // if err, emit error messages
        using namespace std::placeholders;  
        FlowVisitor ff(std::bind(&Strandi::autoLinks, this, _1, _2));
        ff._err = err; 
        bool v = true;

        // all flows in all terms need to be resolved.
        for (auto t : Term::_allTerms)
            if (!t->visit(ff)) v = false;

        return v;
    }

    void _prepareNames()
    {
        // Objects can have multiple names. These are usually listed
        // in the object and have been collected by buildDictionary.
        // However, object parents can also have names which are
        // attributed to the instance rather than the parent.

        LOG4("Prepare names", "");

        // collect these up and add to the instance.
        for (auto t : Term::_allTerms)
        {
            if (t->isObject())
            {
                TermList tl;
                t->getParents(tl);
                for (auto ti : tl)
                {
                    for (auto pi : ti->_namenodes)
                    {
                        if (!contains(t->_namenodes, pi))
                            t->_namenodes.push_back(pi);
                    }
                }
            }
        }
    }

    void prepare()
    {
        if (!_prepared)
        {
            LOG4("Preparing..", "");
            _buildDictionary();
            _prepared = _preparseCommands();
            _prepared = _prepared && prepareAutoLinks();

            if (_prepared)
            {
                _prepareNames();
                
                _processObjectProperties();
                _prepareObjectRuntime();
            }

            LOG4("Preparing done", "");
        }
    }

    bool runSingleTerm(const string& tname)
    {
        // NB: caller consider flush
        // return false if run issues manual break
        
        bool r = true;

        if (_prepared)  // must already be prepared
        {
            Term* t = Term::find(tname);
            if (t)
            {
                r = run(t);
            }
            else
            {
                LOG1("runSingleTerm not found, ", tname);
            }
        }

        if (!r)
        {
            // initiate a break to the main stack
            LOG1("runSingleTerm, initiate break from ", tname);
            _runSingleTermBreak = true;
        }
        return r;
    }

    string runSingleTermCap(const string& tname)
    {
        // use for evaluating a term outside of normal flow
        // for example meta data.
        // NB: term output is captured and returned, not emitted.

        _pushcap(~run_media); // do not process media
        runSingleTerm(tname);
        return textify(_popcap()->_cap);
    }

    bool start(Term* startTerm)
    {
        bool v = false;

        if (!_terms->empty())
        {
            if (!_prepared) prepare();

            v = _prepared;
            if (v)
            {
                assert(startTerm);

                // create a toplevel jump point for shutdown
                if (!setjmp(_env_top))
                {
                    _pushcap(); // create the top-level output capture
                    run(startTerm);
                    flush();
                } 
            }
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

        for (auto& e : f._elts)
        {
            if (e._type == Flow::t_command)
            {
                Flow::EltCommand* ec = (Flow::EltCommand*)&e;
                //DLOG1(_pcom._debug, "parsing", *ec);

                if (!pass)
                {
                    //LOG4("pass 1 pre-parsing ", *ec);

                    pnode* pv = _pcom.parseVerb(ec->_command.c_str());
                    if (pv)
                    {
                        const Word* verb = ParseCommand::getVerb(pv);
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
                        // NB: queries come here
                        //LOG1("failed to pre-parse verb ", *ec);
                    }
                }
                else
                {
                    pnode* ps = _pcom.parse(ec->_command, fv._lineno);
                    if (ps)
                    {
                        if (ps->isSentence())
                        {
                            // collect usages for global template properties
                            const Word* verb = ParseCommand::getVerb(ps);
                            if (verb) fv.addUsage(verb, ps);
                        }
                        
                        ec->setParse(ps); //owns
                    }
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

        // enable definition mode which simplified ambiguous terms for templates
        _pcom._defMode = true;

        FlowVisitor ff(std::bind(&Strandi::preParseFlowCommands, this, _1, _2));

        // selector text is a command, but not to be parsed
        // eg a special label
        // this only applies to objects!
        ff._skipNonReactors = true;
        
        bool v = true;

        // first pass
        for (auto t: *_terms)
        {
            // visit all flows and parse commands to create dictionary
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

        _pcom._defMode = false;  // back to normal

        // Now process the collected word usages in the visitor.
        for (auto& wu : ff._wordUses)
        {
            bool canio = false; // can accept indirect object
            bool allio = true;  // must have an indirect object

            //LOG1("considering verb properties of ", *wu._verb);

            // ignore verbs already flagged as they will be predefined
            if (!wu._verb->isIOLift()) 
            {
                for (auto pn : wu._uses)
                {
                    // can take IO as found in template
                    if (ParseCommand::getIONode(pn)) canio = true;
                    else allio = false;  // not every case has IO
                }

                if (!canio) allio = false; // must at least one

                // signal that we require an IO, because all cases did so.
                // for example "give" so we can know later to lift and
                // to separate dnouns, eg "give witch socks".
                if (allio)
                {
                    LOG3("Setting Required IO verb ", *wu._verb);
                    const_cast<Word*>(wu._verb)->setIOVerb();
                }
                else if (canio)
                {
                    LOG3("Setting Can IO verb ", *wu._verb);
                    const_cast<Word*>(wu._verb)->setCanIOVerb();
                }
            }
        }

        return v;
    }

    void _buildDictionary()
    {
        // adds words used in object reactors to dictionary
        // creates initial verb list.
        // also add term IDs

        LOG4("Building dictionary", "");
        _pcom.internStandardWords();

        _objectCount = 0;
        
        for (auto t: *_terms)
        {
            if (t->isGenerator())
            {
                // flow terms are added as well. 
                _pcom.internWordType(t->_name, Word::pos_noun | Word::pos_FLOW);
            }
        
            if (t->isObject())
            {
                ++_objectCount;
                
                // the term NAME is added as anoun
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

                        int sz = words.size();
                        if (sz)
                        {
                            for (i = 0; i < (int)sz-1; ++i)
                            {
                                // skip any known articles
                                const Word* wi = _pcom.findWord(words[i]);
                                if (wi && wi->isArticle()) continue;                
                                // otherwise assume adjective
                                // Note: adjectives can be things like
                                // "rupert's"
                                LOG4("Adding adjective ", words[i]);
                                _pcom.internWordType(words[i], Word::pos_adj);
                            }
                        
                            LOG4("Adding noun ", words[i]);
                            _pcom.internWordType(words[i], Word::pos_noun);

                            // now we've added the words, parse the nounphrase
                            // and store as one of our names.
                            LOG4("Parsing name ", abuf);
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

                if (e._v) Capture::cats(s, e._v.toString(&_vfmt));
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
        for (const var* v : vs)
        {
            string vi = textify(v);
            if (!vi.empty()) sl.push_back(vi);
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
#if 0
                    string s1 = pn->toString();
                    if (!s1.empty())
                    {
                        s += '[' + s1 + ']';
                        s += "(_be " + t->_name + ')';
                        break;
                    }
#else
                    s = textifyExploredArticle(pn, t);
                    if (!s.empty()) break;
#endif
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

        string s;
        if (res)
        {
            s = textify(*pn->_binding);
        }
        else if (h._expandTerms)
        {
            if (pn->_binding)
            {
                if (pn->_type == pnode::p_noun)
                {
                    assert(pn->_word);
                    res = pn->_word->isID();
                    if (res) s = textify(*pn->_binding);                    
                }
                else if (pn->_type == pnode::p_tnoun)
                {
                    // eg (my MUSHROOM)
                    // this replaces the whole "the X" phrase.
                    s = textify(*pn->_binding);
                    res = !s.empty();
                }
            }
        }

        if (!s.empty())
        {
            if (!h.s.empty()) h.s += ' ';
            h.s += s;
        }
        return res;
    }

    bool _textifyArticle(pnode::SHelper& h) 
    {
        const pnode* pn = h._current;
        
        bool res = false;
        if (pn->_type == pnode::p_article && h._binding)
        {
            Term* t = h._binding;
            assert(t->isObject());
            bool e = getVisited(t);

            // only overide if we have everthing and the object is
            // explored
            if (e)
            {
                if (!h.s.empty()) h.s += ' ';
                h.s += "the";
                res = true;
            }
        }

        // needed?
        //else if (h._expandTerms) res = _textifyExpandBinding(h);
        
        return res;
    }

    string textifyFancy(pnode* pn, bool expandterms = true)
    {
        using namespace std::placeholders;  
        pnode::SHelper h;
        h._expandTerms = expandterms;
        h._hook = std::bind(&Strandi::_textifyExpandBinding, this, _1);

        pn->toString(&h);
        
        //LOG3(TAG "textifyFancy ", textify(pn) << " ~ " << h.s);
        return h.s;
    }

    string textifyExploredArticle(pnode* pn, Term* t)
    {
        using namespace std::placeholders;  
        pnode::SHelper h;
        h._expandTerms = true;
        h._binding = t;
        h._hook = std::bind(&Strandi::_textifyArticle, this, _1);
        pn->toString(&h);
        return h.s;
    }

    string textify(const pnode* pn) 
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
                const pnode* n = pn;
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

                if (pn->_binding)
                {
                    // term list
                    s += " [";
                    s += textify(*pn->_binding);
                    s += ']';
                }

                if (close) s += ')';

                if (pn->_next) s += ' ';
            }
            pn = pn->_next;
        }
        return s;
    }

    int getTimelineTime()
    {
        int t = 0;
        Mark m = _state.beginMarkScan();
        _state.scanMarkFor(m, TERM_TICK, "=", var());
        if (m)
        {
            t = m.val().toInt();
        }
        return t;
    }

    string timelineSaveState()
    {
        // get the state data to save.
        // this will be the timeline as a string, but only as far
        // as the last tick point, in order to save whole moves.
        string s;

        //LOG1("timelineSaveState, ", _state);

        Mark m = _state.beginMarkScan();
        _state.scanMarkFor(m, _tick->_name, "=", var());
        if (m)
        {
            //LOG1("timelineSaveState only to time:",  m.val().toInt());
            
            // bump to include the tick entry
            ++m;
            if (m) s = _state.toStringEnd(m._p);
        }
        return s;
    }

    bool loadTimelineState(const string& data)
    {
        // pass in the genstate blob parser
        bool r = _state.fromString(data.c_str(), &_vfmt);
        LOG1("restoring data, size:", data.size() << " " << r);
        
        if (r)
        {
            // find the current time
            _time = getTimelineTime();
            if (_time < 1) _time = 1;
            
            if (r)
            {
                // scope needs recalculating
                // we need to do this right now, as immediately after
                // we execute POSTLOAD_OK
                _ctx->markScope();
                updateScope();
            }
        }
        
        return r;
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
                        presentChoices(*c);
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





