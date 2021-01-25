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

        void includeResolutionScope(pnode* pn)
        {
            while (pn)
            {
                if (pn->_binding) Strandi::concatm(_resolutionScope,
                                                   *pn->_binding);
                includeResolutionScope(pn->_head);
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

    static void _emitterDefault(char c) { putchar(c);}

    void setdebug(int v)
    {
        _pcom._debug = v;
    }

    struct NRan: public var::Blob
    {
        Ranq1*          _ran;
        unsigned int    _n;
        unsigned int    _maxp;
        unsigned int*   _stats;
        
        NRan(Ranq1* r, int n, int maxp) : _ran(r), _n(n), _maxp(maxp)
        {
            _stats = new unsigned int[_n];
            memset(_stats, 0, sizeof(unsigned int)*_n);
            _warmup();
        }

        NRan(const NRan& nr)
        {
            //printf("nran::copy1\n");
            _ran = nr._ran;
            _n = nr._n;
            _maxp = nr._maxp;
            _stats = new unsigned int[_n];
            memcpy(_stats, nr._stats, sizeof(unsigned int)*_n);
        }

        ~NRan()
        {
            delete [] _stats;
        }

        // blob compliance
        Blob* copy() const override { return new NRan(*this); }
        void destroy() override
        {
            //printf("~NRan!\n");
            delete this;
        }

        bool operator==(const Blob& b) const override { return this == &b; }
        string toString() const override { return "blob"; }

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
            return v;
        }
        
        void _warmup()
        {
            // stir the pot
            for (unsigned int i = 0; i < _n; ++i) nran();
        }
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
                        bool v = resolve(c->_parse);
                        if (v)
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
                            v = exec(ei);
                            clearBindings(c->_parse);
                            updateScope();
                        }
                        else
                        {
                            ERR1("flow cannot resolve", textify(c->_parse));
                        }
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
                            if (et->_flags & (Flow::ft_background | Flow::ft_stop))
                            {
                                v = runBackground(t, et->_flags);
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

        string id() const
        {
            return _reactor ? _reactor->id() : _action->id(); 
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
        bool                _ranhead = false;
        string              _headFlow;

        // null choices are commands, if we have any
        Selector*           _cmdChoice = 0;


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
                _emit('\n');
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

                if (!line.size())
                {
                    // blank line accepts single choice by default
                    // unless cmd available.
                    if (c.size() == 1 && !c._cmdChoice)
                    {
                        accept = true;
                        ch = 1;
                    }
                }
                else
                {
                    // choices must be a number at the start
                    // otherwise assume command input
                    if (u_isdigit(line[0]))
                    {
                        ch = std::stoi(line);
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
                        _execReaction(ei, *cc._reactor);
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
                    _printResolutionScope();

                    // special case to examine reactions
                    int cc = 0;
                    for (auto& r : _ctx->_reactions)
                    {
                        assert(r._reactor);
                        _emit('\n');
                        _emit(++cc);
                        _emit(") ");
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
                        if (resolve(ps))
                        {
                            execInfo ei(ps);
                            accept = exec(ei);
                            updateIt(ei);
                        
                            // and run any action after command
                            c._valid = run(c._cmdChoice->_action);
                        }

                        // will also delete bindings
                        delete ps;
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

        if (rtype != Term::t_first && rtype != Term::t_sequence)
        {
            // fallback to random if not all selectors active
            if (nchoices != (int)c._t->_selectors.size())
                rtype = Term::t_random;
        }
        
        int ch = 0;
        const Choice* cp = 0;

        if (nchoices > 1) switch (rtype)
        {
        case Term::t_random:
            ch = _rand.gen32() % nchoices;
            break;
        case Term::t_shuffle:
            {
                NRan* nr;
                const var* v = _state.getfn(c._t->_name, "nran");
                if (v)
                {
                    assert(v->isBlob());
                    nr = (NRan*)v->_b->copy();
                }
                else nr = new NRan(&_rand, nchoices, nchoices);
                        
                ch = nr->nran();
                _state.setfn(c._t->_name, "nran", nr);
            }
            break;
        case Term::t_nonrandom:
            {
                NRan* nr;
                const var* v = _state.getfn(c._t->_name, "nran");
                if (v)
                {
                    assert(v->isBlob());
                    nr = (NRan*)v->_b->copy();
                }
                else nr = new NRan(&_rand, nchoices,
                                   nchoices+(nchoices+2)/3);

                ch = nr->nran();
                _state.setfn(c._t->_name, "nran", nr);
            }
            break;
        case Term::t_sequence:
            {
                int seq = 0;
                
                // the sequence walks the selectors
                const var* v = _state.getfn(c._t->_name, "seq");
                if (v) seq = v->toInt();

                if (seq >= c._t->_selectors.size())
                {
                    // finished, remain on last valid choice
                    for (auto& ci : c._choices) if (ci._selected) cp = &ci;
                    assert(cp);
                }
                else
                {
                    // walk selectors until we find a valid choice
                    do
                    {
                        Selector* s = c._t->_selectors[seq];
                        
                        // find within choices
                        for (auto& ci : c._choices)
                        {
                            if (ci._selected && ci._action == s)
                            {
                                cp = &ci;
                                break;
                            }
                        }

                        if (cp) break;
                        
                        // not found.
                        // choice not available, move to next
                        ++seq;
                        
                    } while (seq < c._t->_selectors.size());

                    // update sequence
                    ++seq;
                    _state.setfn(c._t->_name, "seq", seq);
                }
            }
            break;
        case Term::t_first:
            // ch = 0 selects first selected
            break;
        }

        assert(ch >= 0 && ch < nchoices);

        if (!cp)
        {
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
        bool matching = false;

        // top flow is input for matching generators
        CapRef topflowcap;

        assert(t->isGenerator());
            
        // run top flow and keep result, this becomes
        // an input selector.
        if (t->_topflow)
        {
            matching = true;

            // mask choices in topflow
            // This prevents choices being run and instead
            // returns the previous choice made if any
            _pushcap(~run_choice);
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

            if (matching)
            {
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
                            // feed the topflow to the matcher one at a time
                            for (auto& e : topflowcap->_cap._elts)
                            {
                                // install each individually as lastcap
                                Capture icap;
                                icap.add(e);
                                LastCap::Tmp ltmp(_ctx->_lastCap, &icap);
                        
                                for (auto& ci : choices._choices)
                                {
                                    ci._selected = !ci.specialMatch() &&
                                        ci._select->_cap.match(e);
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
                for (auto s : t->_selectors._selectors)
                {
                    // when automatically choosing we run the selector
                    // flow later
                    if (checkSelectorCond(s))
                    {
                        Choice ch(s);
                        ch._selected = true;
                        choices._choices.emplace_back(ch);
                    }
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
                if (!_runSpecial(t))
                    v = runselect(t);
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

    bool runBackground(Term* t, uint flags)
    {
        if (flags & Flow::ft_background)
        {
            // signal to run background flow, but do nothing at this point
            // do not initially mark as visited.
            _state.set(t->_name, TERM_TICK); // does not add if already
        }
        else if (flags & Flow::ft_stop)
        {
            // remove from background flow
            _state.clear(t->_name, TERM_TICK);
        }
        return true;
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
        // concat without testing destroying l2
        l1.splice(l1.end(), l2);
    }

    static void concatc(TermList& l1, TermList& l2)
    {
        // concat by copying l2
        for (auto t : l2) l1.push_back(t);
    }

    static bool subset(const TermList& a, const TermList& b)
    {
        // is `a` a subset of `b` ?
        for (auto t : a)
            if (!contains(b, t)) return false;
        
        return true;
    }
    
    static int subtypes(const TermList& a, const TermList& b)
    {
        // are elements of a, all ether in b or subtypes of some member of b
        // return 0 if fail
        // return subtype parent distance
        // NB: if a empty, return 0
        
        int maxd = 0;
        for (auto t : a)
        {
            if (contains(b, t)) continue;

            int di = 0;
            for (auto bi : b)
            {
                di = isDistance(t, bi); // is bi a parent of t? 
                if (di) break; // yes
            }

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

    void calculateScope(TermList& tl, Term* p)
    {
        // calculate scope for p (usually player)
        // the scope is:
        // all things in P (subscope)
        // all things P is in recursively.
        // all things in what P is directly in.
        // and P if not already included.
        tl.clear();

        // sub items of p
        subInTerms(tl, p);

        // immediate parent "in"
        TermList p1;
        inTerms(p1, p);

        if (!p1.empty())
        {
            // add all things in parent
            for (auto a : p1)
            {
                TermList tl1;
                subInTerms(tl1, a); // will include p
                concatm(tl, tl1);
            }

            // all initial parents
            concatc(tl, p1);

            // then all remaining parent
            for (auto a : p1) inTermsRec(tl, a);
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
    concatm(*_a->_binding, *pl->_binding);  \
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
        if (w == _pcom._here)
        {
            if (_player)
            {
                tl = new TermList;
                inTerms(*tl, _player);
            }
        }
        else if (w == _pcom._it)
        {
            Term* t = _ctx->getIt();
            if (t)
            {
                tl = new TermList;
                tl->push_back(t);
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
            Term::matchName(*tl, pn, pa);
        }

        if (tl->empty()) { delete tl; tl = 0; }
        return tl;
    }

    bool resolve(pnode* pn)
    {
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
                    if (tl) pn->_binding = tl;
                    else ok = false;
                    
                    // no need to continue
                    down = false;
                }
                break;
            case nodeType::p_noun:
                {
                    assert(!pn->_binding);
                    TermList* tl = resolveNoun(pn, 0);
                    if (tl) pn->_binding = tl;
                    else ok = false;
                }
                break;
            case nodeType::p_pronoun:
                {
                    assert(!pn->_binding);
                    TermList* tl = resolvePronoun(pn);
                    if (tl) pn->_binding = tl;
                    else
                    {
                        ok = false;
                        ERR1("unable to resolve pronoun", pn->_word->_text);
                    }
                }
                break;
            }

            if (!ok)
            {
                ERR1("unresolved:", pn->toStringStruct());
                res = false;
            }
            
            if (down && !resolve(pn->_head)) res = false;

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
                assert(!pn->_binding);
                assert(pn->_head->_binding);

                // XX need to resolve article
                TL_TAKE(pn, pn->_head);
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
        return res;
    }

    struct execInfo
    {
        pnode*      _ps;
        bool        _err = true;
        bool        _output = false;  // emit results of command
        pnode*      _verbn = 0;
        const Word* _verb = 0;
        pnode*      _dobjn = 0;
        TermList*   _dobj = 0;
        pnode*      _prepn = 0;
        const Word* _prep = 0;

        pnode*      _iobjn = 0;
        TermList*   _iobj = 0;
        Term*       _it = 0;

        execInfo(pnode* ps): _ps(ps) {}
    };

   static TermList* nodeBinding(pnode* pn)
   {
       TermList* b = 0;
       
       // find binding
       while (pn)
       {
           b = pn->_binding;
           if (b) break;
           pn = pn->_head;
       }
       
       return b;
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
                ei._verb = ParseCommand::getVerb(pn);
                assert(ei._verb);

                // verb alone is valid.
                v = true;

                pn = ei._dobjn = pn->_next;
                if (pn)
                {
                    // find binding
                    ei._dobj = nodeBinding(pn);

                    // if we have direct object, it must be bound
                    if (!ei._dobj) v = false;

                    if (v)
                    {
                        // ensure dobj list isnt empty
                        assert(!ei._dobj->empty());

                        // next term might not be prep (eg set)
                        pn = pn->_next;
                        if (pn)
                        {
                            v = false;
                            if (pn->_type == nodeType::p_prep)
                            {
                                ei._prepn = pn;
                                ei._prep = pn->_word;
                                assert(ei._prep);

                                pn = ei._iobjn = pn->_next;
                                if (pn)
                                {
                                    ei._iobj = nodeBinding(pn);

                                    // if we have indirect object, must be bound                                    
                                    v = ei._iobj != 0;
                                    
                                    // ensure iobj list isnt empty
                                    assert(!ei._iobj || ei._iobj->size());
                                }
                            }
                            else if (pn->_type == nodeType::p_property)
                            {
                                // not prep, but store in same slots
                                // verb will know
                            
                                ei._prepn = pn;
                                assert(pn->_word);
                                ei._prep = pn->_word;

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
                        ei._dobj = nodeBinding(pn);
                        
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
                            ei._iobj = nodeBinding(pn);
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
                assert(!ei._dobj->empty());
                Term* t = ei._dobj->front();
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
        Term* iobj = ei._iobj->front();
        const string& prop = ei._prep->_text;

        assert(iobj);

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
            if (_state.setfn(t->_name, prop, ei._iobj->front()->_name))
                DLOG3(_pcom._debug, "exec PUT", *t, prop, *ei._iobj->front());
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
        if (_state.setfn(t->_name, ei._prep->_text, ei._iobjn->_word->_text))
            DLOG3(_pcom._debug, "exec SET", *t, *ei._prep, *ei._iobjn->_word);

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
            for (auto t : *ei._dobj)
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
        if (v) for (auto t : *ei._dobj) if (!execSet1(ei, t)) v = false;
        if (v && ei._output) OUTP(EXEC_OK);
        return v;
    }

    bool execQuery(execInfo& ei)
    {
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
                        Term* t = ei._dobj->front();
                        string val = ei._iobj->front()->_name;
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
                
                    // currently neither properties or prep are multivalued.
                    // so setting X in Y, replaces X in Z.
                    // same for properties set X feels wet, replaces X feels dry.
                    //
                    // later we'd like to specify this
                    Term* t = ei._dobj->front();
                    const var* y = _state.getfn(t->_name, ei._prep->_text);

                    if (y)
                    {
                        if (ei._prep->isPrep())
                        {
                            // expect term
                            Term* r = Term::find(y->rawString());
                            OUTP(r);
                            DLOG4(_pcom._debug, "exec QUERY", *t, *ei._prep, "=", textify(r));
                        }
                        else
                        {
                            // we dont expect a term, but it can be!
                            Term* r = Term::find(y->rawString());
                            if (r)
                            {
                                OUTP(r);
                            }
                            else
                            {
                                // add raw value
                                OUTP(y);
                            }
                        
                            DLOG4(_pcom._debug, "exec QUERY", *t, *ei._prep, "=", textify(y));
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
                    val = ei._iobj->front()->_name;
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

    void _execReaction(execInfo& ei, Reaction& r)
    {
        Term* it = 0;

        // bind dobj to "it"
        if (ei._dobj)
        {
            assert(!ei._dobj->empty());
            it = ei._dobj->front();
            _ctx->pushIt(it);
        }
        
        run(r._s->_action);

        if (it) _ctx->popIt();
    }

    bool execReaction(execInfo& ei)
    {
        bool v = false;

        std::vector<Selector*> matches;
        for (auto& r : _ctx->_reactions)
        {
            execInfo rei(r._reactor);
            if (prepareExecInfo(rei) && execValidate(rei))
            {
                assert(ei._verb);
                assert(rei._verb);

                int rank = 0;

                // can be just a verb match
                v = (ei._verb == rei._verb);
                
                if (v)
                {
                    rank = 10000;

                    // both or neither
                    v = (rei._dobj != 0) == (ei._dobj != 0);
                    
                    if (v && rei._dobj)
                    {
                        // insist they match
                        v = subset(*rei._dobj, *ei._dobj);
                        if (v) rank += 1000;
                        else
                        {
                            // are my terms all subtypes of template?
                            int d = subtypes(*ei._dobj, *rei._dobj);
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
                                v = subset(*rei._iobj, *ei._iobj);
                                if (v) rank += 100;
                                else
                                {
                                    // are my terms all subtypes of template?
                                    int d = subtypes(*ei._iobj, *rei._iobj);
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
                    DLOG1(_pcom._debug, "reactor match", textify(rei._ps));
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
                bestrank = r._rank;
                best = &r;
            }
        }

        v = best != 0;
        if (v)
            _execReaction(ei, *best);

        return v;
    }

    void updateIt(execInfo& ei)
    {
        if (ei._it)
        {
            //LOG3("UPDATEIT ", ei._it->_name);
            _ctx->setIt(ei._it);
        }
    }

    bool exec(execInfo& ei)
    {
        bool v = prepareExecInfo(ei) && execValidate(ei);

        if (v) 
        {
            DLOG1(_pcom._debug > 1, "exec", textify(ei._ps));
            if (ei._verb->_text == SYM_PUT) v = execPut(ei);
            else if (ei._verb->_text == SYM_SET) v = execSet(ei);
            else if (ei._verb->isQuery()) v = execQuery(ei);
            else
            {
                // match against current reactions
                v = execReaction(ei);
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
            
            // can eliminate reactors using conditions like a generator
            if (!checkSelectorCond(s)) continue;
            
            pnode* pn = ec->_parse->copy();
            if (resolve(pn))
            {
                _ctx->_reactions.emplace_back(Reaction(this, s, pn));
                
                // objects resolved in reaction are added to
                // resolution scope.
                _ctx->includeResolutionScope(pn);
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
        for (auto p : tl) resolveTermReactions(p);
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
            //_printScope();

            _ctx->resolveScopeReactions();
        }
    }

    void _prepareObjectRuntime()
    {
        // find well known objects
        _player = Term::find(TERM_PLAYER);
        if (!_player) DLOG0(_pcom._debug, "There is no player!");

        _thing = Term::find(TERM_THING);
        if (!_thing) DLOG0(_pcom._debug, "There is no THING!");

        _tick =  Term::find(TERM_TICK);
        assert(_tick);

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
                    // otherwise they care command templates

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
                                //DLOG1(_pcom._debug,"Adding adjective", words[i]);
                                _pcom.internWordType(words[i], Word::pos_adj);
                            }
                        
                            //DLOG1(_pcom._debug,"Adding noun", words[i]);
                            _pcom.internWordType(words[i], Word::pos_noun);

                            // now we've added the words, parse the nounphrase
                            // and store as one of our names.
                            //DLOG1(_pcom._debug,"Parsing name", c._buf);
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





