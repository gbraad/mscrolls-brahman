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


#include "fd.h"
#include "strutils.h"
#include "varset.h"
#include "pexpr.h"
#include "growbuf.h"

#define SYM_STICKY      '!'

// generator flags
#define SYM_RANDOM '~'
#define SYM_SHUFFLE '&'
#define SYM_NONRAMDOM '#'
#define SYM_SEQUENCE '<'
#define SYM_FIRST '='

// choice flags
#define SYM_CMD_CHOICES '>'

// object types
#define SYM_CHOICE '?'
#define SYM_OBJECT '@'

// choice selector flags
#define SYM_ALWAYS  '+'
#define SYM_FILLER '-'
#define SYM_TERMINAL '!'

// object flags
#define SYM_ASCHOICE '='

// selector flags
#define SYM_CONDITION '?'
#define SYM_NOT '!'

// subflow
#define SYM_SUBFLOW_START '('
#define SYM_SUBFLOW_END ')'

// flow term properties
// these are placed in flow when a term is referenced, not in definition.
#define SYM_BACKGROUND '^'
#define SYM_BACKGROUND_STOP '#'

#define SYM_ATTRIBUTE ':'
#define SYM_ATTR_SEP ','


namespace ST
{

struct ParseStrands: public ParseBase
{
    Term*       _startTerm = 0;
    string      _loadFilePrefix;  // files base dir

    GrowString  _sourceCollector;
    bool        _collectSource = false;
    
    string parseName()
    {
        // `[A-Z][A-Z0-9-_]+`

        int l = atName(POS);
        string s;
        if (l)
        {
            s = string(POS, l);
            SETPOS(POS + l); 
        }
        return s;
    }

    Term::RType parseRType()
    {
        char c = AT;

        Term::RType rt = Term::t_void;

        switch (c)
        {
        case SYM_RANDOM:
            rt = Term::t_random;
            break;
        case SYM_SHUFFLE:
            rt = Term::t_shuffle;
            break;
        case SYM_NONRAMDOM:
            rt = Term::t_nonrandom;
            break;
        case SYM_SEQUENCE:
            rt = Term::t_sequence;
            break;
        case SYM_FIRST:
            rt = Term::t_first;
            break;
        default:
            c = 0;
        }

        if (c)
        {
            BUMP;
            skipws();
        }

        return rt;
    }

    void _addText(Flow& f,
                  const char* st,
                  const char* p,
                  uint oktypes)
    {
        // backup over any space on the end
        // unless it's been escaped in!
        const char* q = p;
        while (q != st)
        {
            --q;
            if (!u_isspace(*q)) break;
            if (q != st && q[-1] == '\\') break; // space was escaped!
            p = q;
        }

        if (p != st)
        {
            //ERR0("add text '" << string(st, p - st) << "'");
            string s = processEscapes(st, p - st);
            if (s.size())
            {
                if (Flow::t_text & oktypes)
                    f._elts.push_back(new Flow::EltText(s));
                else
                {
                    string m = "\"";
                    m += s;
                    m += '"';
                    PERR1("text not allowed", m);
                }
            }
        }
    }

    void _addTerm(Flow& f, const string& s, uint flags)
    {
        assert(s.size());
        auto t = new Flow::EltTerm(s);
        t->_flags = flags;
        f._elts.push_back(t);
    }

    Flow::EltMedia* _addMedia(Flow& f, const string& fname, MediaType mt)
    {
        assert(fname.size());
        //printf("adding file '%s'\n", fname.c_str());
        auto m = new Flow::EltMedia(fname, mt);
        f._elts.push_back(m);
        return m;
    }

    Flow::EltCond* _addCond(Flow& f, enode* e)
    {
        assert(e);
        auto m = new Flow::EltCond(e);
        f._elts.push_back(m);
        return m;
    }

    static uint parseFlowTermFlags(const char* p)
    {
        uint flags = 0;

        for (;;)
        {
            if (*p == SYM_BACKGROUND) flags |= Flow::ft_background;
            else if (*p == SYM_BACKGROUND_STOP) flags |= Flow::ft_reset;
            else break;
            ++p;
        }
        return flags;
    }

    static VarSet* parseAttributes(VarSet* vars, const char** qp)
    {
        // foo:bar,moo:1  etc.
        // bool properties can omit "true", eg foo,bar:1 
        // same as foo:true,bar:1
        const char* p = *qp;
        for (;;)
        {
            const char* ts = p;

            // expect tag to be alphanum or underscore
            if (u_isalpha(*p))
            {
                ++p;
                while (u_isalnum(*p) || *p == '_') ++p;
            }
            
            if (ts == p) break; // none, we're done

            string tag(ts, p - ts);
            var vv;
            
            if (!*p || *p == SYM_ATTR_SEP || *p == '\n')
            {
                // end or comma separator. 
                // Assume tag is bool and value true.
                vv = var(true);
            }
            else
            {
                if (*p != SYM_ATTRIBUTE) break; // bad syntax, reject

                ++p;
                const char* vs = p;

                // value ends with comma or newline or null
                while (*p && *p != SYM_ATTR_SEP && *p != '\n') ++p;
                if (vs == p) break; // no value given, drop out

                string val(vs, p - vs);
                vv.parse(val.c_str(), true);
            }
            
            if (vv)
            {
                if (!vars) vars = new VarSet;
                (*vars)[tag] = vv; // consumes!
            }

            // consume comma, newline or null will stop
            if (*p == SYM_ATTR_SEP) ++p;
            *qp = p; // accept
        }
        
        return vars;
    }

    void addText(Flow& f, const string& t, uint oktypes)
    {
        // break authored text into its component flows;
        // stretches of plain text, term references and media refs.
        const char* q = t.c_str();
        for (;;)
        {
            while (u_isspace(*q)) ++q;
            if (!*q) break;
            
            const char* p = q;
            int l;
            char lastc = 0;
            uint ftFlags = 0;
            
            while (*p)
            {
                l = atName(p);
                if (l)
                {
                    if (lastc == '\\')
                    {
                        // escaped term, skip this name and treat as text
                        p += l;
                        continue;
                    }

                    ftFlags = parseFlowTermFlags(p + l);
                    break;
                }
                else if (*p == '.' && p != q)
                {
                    // collect suffix and see if it's a media filename
                    const char* e = p;
                    e++;
                    while (u_isalnum(*e)) ++e;
                    MediaType mt = mediaSuffix(string(p, e - p));
                    if (mt)
                    {
                        // ok, look back from .suf and find the filename
                        const char* s = p;

                        do
                        {
                            if (u_isspace(s[-1])) break;
                            --s;
                        } while (s != q);

                        if (s != p)
                        {
                            string fname = string(s, e - s);
                            
                            _addText(f, q, s, oktypes);
                            Flow::EltMedia* m = _addMedia(f, fname, mt);
                            assert(m);

                            if (*e == SYM_ATTRIBUTE)
                            {
                                ++e;
                                m->_attr = parseAttributes(m->_attr, &e);
                            }

                            q = p = e;
                            assert(!l);
                            break;

                        }
                    }
                }
                
                lastc = *p++;
            }
            
            _addText(f, q, p, oktypes);

            if (l)
            {
                // add term ref
                _addTerm(f, string(p, l), ftFlags);
            }

            // skip over any name
            q = p + l;
            q += countBits(ftFlags);  // account for flag symbols
        }

    }

    void addCode(Flow& f, const string& c, uint oktypes)
    {
        if (c.size())
        {
            if (Flow::t_code & oktypes)
                f._elts.push_back(new Flow::EltCode(c));
            else
            {
                PERR1("code not allowed", c);
            }
        }
    }

    void addCommand(Flow& f, const string& c, uint oktypes)
    {
        if (c.size())
        {
            if (Flow::t_command & oktypes)
            {
                auto ec = new Flow::EltCommand(c);
                ec->_lineno = lineno; // keep track of where we were
                f._elts.push_back(ec);
            }
            else
            {
                string m = "'";
                m += c;
                m += '\'';
                PERR1("command not allowed", m);
            }
        }
    }

    void _parseFlow(Flow& f, Flow::Type startType,
                    int allowednl, uint oktypes)
    {
        // allowednl controls whether newline breaks the flow
        // == 0 => no newlines allowed, newline will break
        // == 1 => blank line will end
        // > 1 => more than blank line with next start a NAME will end
        //
        // (hacky) allowednl can be a termination character for text only!
        //
        // oktypes controls what element types allowed in flow
        // -1 => any
        // otherwise a mask of Flow::Type
        skipws();

        Flow::Type t = startType;
        Flow::Type tn = Flow::t_void;
    
        while (t)
        {
            switch (t)
            {
            case Flow::t_text:
                {
                    char last = '\n';
                    bool skipender = false;

                    bool esc = false;

                    PUSHSPAN;
                    
                    tn = Flow::t_void;
                    while (AT)
                    {
                        if (esc)
                        {
                            last = GETC;
                            esc = false;
                            continue;
                        }

                        if (AT == '\\')
                        {
                            esc = true;
                            BUMP;
                            continue;
                        }
                    
                        // special break condition
                        if (allowednl >= ' ' && AT == allowednl)
                        {
                            skipender = true;
                            break;
                        }
                        
                        // newline can end
                        if (AT == '\n' && allowednl == 0)
                        {
                            skipender = true; // consume nl
                            break;
                        }

                        if (last == '\n')
                        {
                            if (AT == '*')
                            {
                                // start of selector
                                break;
                            }
                            else if (AT == '>')
                            {
                                // start of a command
                                tn = Flow::t_command;
                                break;
                            }
                            else if (AT == '\n')
                            {
                                if (allowednl == 1)
                                {
                                    // blank line ends
                                    skipender = true;
                                    break;
                                }

                                // peek
                                {
                                    PUSHP;
                                    BUMP;

                                    assert(!skipender);

                                    // if next line starts with a NAME
                                    if (Traits::atName(POS))
                                    {
                                        //int l = Traits::atName(POS);
                                        //printf("############## break at '%s'\n", string(POS, l).c_str());
                                        skipender = true;
                                    }

                                    POPP;
                                    if (skipender) break;
                                }
                            }
                        }
                        if (AT == '{')
                        {
                            // start of code
                            tn = Flow::t_code;
                            break;
                        }
                        last = GETC;
                    }
                    string s = POPSPAN;
                    addText(f, s, oktypes);

                    // end character to be consumed (eg # or nl)
                    if (skipender) BUMP;
                }
                break;
            case Flow::t_code:
                {
                    assert(AT == '{');
                    BUMP;
                    skipws();

                    PUSHSPAN;
                    
                    int level = 1;
                    int quote = 0;
                    int esc = 0;
                    while (AT)
                    {
                        if (esc)
                        {
                            esc = 0;
                        }
                        else
                        {
                            if (AT == '\\') esc = 1;
                            else if (AT == '"') quote = !quote;
                            else if (!quote)
                            {
                                if (AT == '{') ++level;
                                else if (AT == '}')
                                {
                                    if (!--level)
                                    {
                                        string c = trim(POPSPAN);
                                        addCode(f, c, oktypes);
                                    
                                        BUMP; // '}'

                                        // eat space and single newline after code
                                        skipws();
                                        if (AT == '\n') BUMP;

                                        // assume text follows code
                                        tn = Flow::t_text;
                                
                                        break;
                                    }
                                }
                            }
                        }
                        BUMP;
                    }
                }
                break;
            case Flow::t_command:
                {
                    // will always be at > unless we're calling
                    // parsecommandflow, in which case it is optional.
                    if (AT == '>')
                    {
                        BUMP;
                        skipws();
                    }
                    
                    PUSHSPAN;

                    // commands must be on a single line
                    while (AT && AT != '\n') BUMP;

                    string c = trim(POPSPAN);
                    addCommand(f, c, oktypes);

                    // eat newline after command
                    if (AT == '\n') BUMP;
                    
                    // assume text follows code
                    tn = Flow::t_text;

                    // don't allow any more flow
                    if (!allowednl) tn = Flow::t_void;
                }
                break;
            }

            t = tn;
        }
    }

    void parseFlow(Flow& f, int allowednl, uint oktypes)
    {
        // parse flow assuming text starts
        _parseFlow(f, Flow::t_text, allowednl, oktypes);
    }

    void parseCommandFlow(Flow& f, int allowednl, uint oktypes)
    {
        // parse flow assuming command starts
        _parseFlow(f, Flow::t_command, allowednl, oktypes);
    }

    bool parseProps(Term* t)
    {
        // [\?@][ ]*[~&>=]{0-2}!?[ ]*\n
        char c = AT;

        switch (c)
        {
        case SYM_CHOICE:
            t->_type = Term::t_choice;
            break;
        case SYM_OBJECT:
            t->_type = Term::t_object;
            break;
        default:
            c = 0;
        }

        if (c)
        {
            BUMP;
            skipws();
        }

        switch (t->_type)
        {
        case Term::t_generator:
            {
                Term::RType ty = parseRType();
                if (ty) t->_rtype = ty;  // default is random
                ty = parseRType();
                if (ty) t->_rtypenext = ty; // default void
            
                if (AT == SYM_STICKY)
                {
                    if (t->_rtype == Term::t_shuffle
                        || t->_rtype == Term::t_nonrandom)
                    {
                        // no point having state for term used once
                        LOG1("sticky term changed to random ", t->_name);
                        t->_rtype = Term::t_random;
                    }
                    if (t->_rtype == Term::t_sequence)
                    {
                        // no need for sequence, same as first match
                        LOG1("sticky term changed to first ", t->_name);
                        t->_rtype = Term::t_first;
                    }
            
                    t->sticky(true);
                    BUMP;
                    skipws();
                }

                if (AT == '>' || AT == '{' || u_isalnum(AT)) 
                {
                    // optional input top flow
                    parseFlow(t->_topflow, 0, -1); // consume nl

                    if (t->_rtype != Term::t_random)
                    {
                        // for now we don't have meaning for filters
                        // with state indicators
                        // insist on them being random
                        
                        LOG1("term with topflow changed to random ", t->_name);
                        t->_rtype = Term::t_random;
                        
                    }
                }
                else if (AT == '\n')
                {
                    BUMP;
                }
                else
                {
                    PERR1("unexpected property", AT);
                    return false;
                }
            }
            break;
        case Term::t_choice:

            for (;;)
            {
                if (AT == SYM_STICKY)
                {
                    t->sticky(true);
                    BUMP;
                    skipws();
                }
                else if (AT == SYM_CMD_CHOICES)
                {
                    // signify reactors marked as choices will be displayed
                    // along with the command line.
                    t->cmdChoices(true);
                    BUMP;
                    skipws();
                }
                else if (AT == '\n')
                {
                    BUMP;
                    break;
                }
                else
                {
                    PERR1("unexpected property", AT);
                    return false;
                }
            }

            break;
        case Term::t_object:
            {
                // object followed by optional list of parent names
                // these all have to be term references
                parseFlow(t->_topflow, 0, Flow::t_term);
            }
            break;
        }

        return true;
    }

    Term* parseTermHead()
    {
        // NAME PROPS
        Term* t = 0;
        string name = parseName();
        if (!name.empty())
        {
            t = new Term(name);
            skipws();

            bool v = parseProps(t);

            if (!v)
            {
                delete t;
                t = 0;
            }
        }
        return t;
    }

    uint parseChoiceFlags(Selector* s)
    {
        uint f = 0;

        for (;;)
        {
            char c = AT;
            if (c == SYM_FILLER) f |= Selector::c_filler;
            else if (c == SYM_ALWAYS) f |= Selector::c_always;
            else if (c == SYM_TERMINAL) f |= Selector::c_terminal;
            else break;

            BUMP;
            skipws();
        }

        if ((f & Selector::c_always) == 0)
        {
            // not always, default to once
            f |= Selector::c_once; // make once by default
        }

        if ((f & Selector::c_terminal) && (f & Selector::c_filler))
        {
            PERR1("selector cannot be both terminal and filler", s->id());
            f &= ~Selector::c_filler;
        }
    
        return f;
    }

    uint parseObjectFlags(Selector* s)
    {
        int f = 0;
        for (;;)
        {
            char c = AT;
            if (c == SYM_ASCHOICE) f |= Selector::o_aschoice;
            else break;

            BUMP;
            skipws();
        }
        return f;
    }

    void parseCondFlow(Selector* s)
    {
        ParseExpr pe;
        enode* en = pe.parse(POS, lineno);
        SETPOS(pe.pos);
        if (en)
        {
            //LOG1("parseCondFlow ", en->toString());
            _addCond(s->_cond, en);
        }
    }

    Selector* parseSelector(Term* host)
    {
        // POS just past "*"
        Selector* s = new Selector(host);
        s->_lineno = lineno;

        uint f = 0;

        if (host->isObject()) f |= parseObjectFlags(s);

        // objects can have choice flags
        f |= parseChoiceFlags(s);

        // so that flags can be in any order
        if (host->isObject()) f |= parseObjectFlags(s);

        s->_flags = f;

        skipws();

        // NB: object reactions can have conditionals just like 
        // everything else.
        if (AT == SYM_CONDITION)
        {
            // selector conditional prefix
            BUMP;
            
            parseCondFlow(s);
            skipws();
        }

        if (AT != '\n')
        {
            // the main selector flow
            // if we are an object, treat the selector as a command
            // otherwise it's just a text flow.
            if (host->isObject())
                parseCommandFlow(s->_text, 0, -1);
            else
                parseFlow(s->_text, 0, -1);
        }
        else
        {
            // flow can be empty
            BUMP; // skip \n
            
            if (host->isObject()) PERR0("Object " << host->_name << " cannot have empty selector");
        }

        // followed by another selector or blank => no action
        if (AT != '*' && AT != '\n')
        {
            parseFlow(s->_action, 1, -1);
        }
        else if (AT == '\n')
        {
            //if no action eat newline
            BUMP;
        }
        
        return s;
    }

    void parseSelectors(Term* t)
    {
        assert(AT == '*');

        int sid = 0;
        while (AT == '*')
        {
            BUMP;
            Selector* s = parseSelector(t);
            s->_id = sid++;  // selector id is nth index
            t->_selectors.add(s);

            const char* p = nextNonBlank();
            if (*p == '*') _advance(p);
        }
    }

    bool parseBody(Term* t)
    {
        uint oktypes = -1;
        
        // can be empty flow
        if (AT != '*')
        {
            // headflow: only commands allowed for objects
            if (t->_type == Term::t_object) oktypes = Flow::t_command;
            parseFlow(t->_flow, 2, oktypes);
        }

        const char* p = nextNonBlank();
        // a selector
        if (*p == '*')
        {
            _advance(p);
            parseSelectors(t);
        }

        // NB: can't have post without selectors
        // NB: objects never have post
        if (t->_type != Term::t_object &&
            AT != '\n' && t->_selectors.size() && !Traits::atName(POS))
        {
            parseFlow(t->_postflow, 2, -1);
        }

        return true;
    }

    Term* parseTermDef()
    {
        lastdef = POS;
        Term* t = parseTermHead();
        if (t)
        {
            bool nobody = (AT == '\n');
            if (nobody)
            {
                // blank line follows head, no body.
                // unless a selector
                
                BUMP;
                
                if (*nextNonBlank() == '*')
                {
                    // allow header blanks before first selector
                    nobody = false;
                }
            }

            if (!nobody)
            {
                if (!parseBody(t))
                {
                    delete t;
                    t = 0;
                }

            }
        }
        return t;
    
    }

    bool linkTerm(Flow::EltTerm* et, FlowVisitor& fv)
    {
        bool v = et->_term != 0;
        if (!v)
        {
            //LOG1("Linking ", *et << " from " << *fv._hostTerm);
            et->_term = Term::find(et->_name);
            if (et->_term) v = true;
            else if (fv._err)
            {
                assert(fv._hostTerm);
                ERR0("missing term '" << et->_name << "'; in term " << fv._hostTerm->toString()); 
            }
        }
        return v;
    }

    bool linkEnode(enode* en, FlowVisitor& fv)
    {
        bool v = true;
        if (en->isTermName())
        {
            en->_binding = Term::find(en->name());
            if (!en->_binding)
            {
                v = false;
                if (fv._err)
                {
                    ERR0("missing cond term '" << *en << '\'');
                }
            }
        }
        return v;
    }

    bool linkCond(Flow::EltCond* et, FlowVisitor& fv)
    {
        assert(et->_cond);
        bool v = true;
        for (enode::It it(et->_cond); it; ++it)
        {
            enode* en = const_cast<enode*>(it._n);
            if (!linkEnode(en, fv)) v = false;
        }

        return v;
    }
    
    bool linkFlow(Flow& f, FlowVisitor& fv)
    {
        bool v = true;
        for (auto i = f._elts.begin(); i != f._elts.end(); ++i)
        {
            Flow::Elt* e = i;
            if (e->_type == Flow::t_term)
            {
                Flow::EltTerm* et = (Flow::EltTerm*)e;
                if (!linkTerm(et, fv)) v = false;
            }
            else if (e->_type == Flow::t_cond)
            {
                Flow::EltCond* et = (Flow::EltCond*)e;
                if (!linkCond(et, fv)) v = false;
            }
        }
        return v;
    }

    bool linkTerms(bool err = true)
    {
        // if err, emit error messages
        using namespace std::placeholders;  
        FlowVisitor ff(std::bind(&ParseStrands::linkFlow, this, _1, _2));
        ff._err = err; 
        bool v = true;

        // all flows in all terms need to be resolved.
        for (auto t : Term::_allTerms)
            if (!t->visit(ff)) v = false;

        return v;
    }

    bool validateTerms()
    {
        bool v = true;
        for (auto t: Term::_allTerms)
        {
            if (t->_type == Term::t_object)
            {
                Term::TermList parents;
                t->getParents(parents);
                for (auto p : parents)
                {
                    if (p->_type != Term::t_object)
                    {
                        PERR0(t->_name << " parent " << p->_name << " not object");
                        v = false;
                    }
                }
            }
            else
            {
                // check all flows contain non-objects
                ;
            }
        }
        return v;
    }

    void collect()
    {
        for (;;)
        {
            _skipc();
            int c = AT;
            if (!c) break;
            _sourceCollector.add(c);
            BUMP;
        }

        // ensure first term of next file is treated as a term
        // need to two blanks + 1 in case file not ending in newline
        _sourceCollector.append("\n\n\n");
    }

    void processString(const char* data)
    {
        SETPOSSTART(data);
        SETPOS(data);

        if (_collectSource)
        {
            collect();
            SETPOS(data);
        }
        
        //_dump(); return;

        for (;;)
        {
            skipblank();

            if (!AT) break;
        
            Term* t = parseTermDef();
            if (t)
            {
                if (!Term::find(t->_name))
                {
                    if (!_startTerm) _startTerm = t;
                    Term::add(t);
                }
                else
                {
                    LOG1("ERROR: Duplicate term ", t->_name);
                }

                //DLOG0(_debug, t->toString());

            }
            else
            {
                PERR0("expected term");
                break;
            }
        }
    }

    bool processFile(const string& filename, bool* wasCompressed = 0)
    {
        lineno = 1;
        _filename = makePath(_loadFilePrefix, filename);

        LOG3("processfile ", _filename);

        FD fd;
        bool r = fd.open(_filename);
        if (r)
        {
            // try reading compressed first
            size_t usize;
            unsigned char* data = readz(fd, &usize);

            if (wasCompressed) *wasCompressed = data != 0;
            
            if (!data)
            {            
                FD::Pos fsize;
                data = fd.readAll(&fsize, true); // remove dos newlines
            }
            
            if (data)
            {
                processString((char*)data);
                delete data;
            }
            else r = false;
        }

        if (!r)
        {
            ERR1("Can't open input file", filename);
        }
        return r;
    }

    bool loadFiles(std::vector<std::string>& files, bool loadGameFiles = true)
    {
        // false if any file fails
        bool v = true;
        bool wasCompressed;

        for (auto& f : files)
        {
            v = processFile(f, &wasCompressed);
            if (!v) break;

            // dont auto load game files if compressed
            // as we assume this is a complete story in one
            if (wasCompressed) loadGameFiles = false;
        }

        if (v)
        {
            Term* t = 0;

            if (loadGameFiles)
            {
                // look for well-known term to list extra files
                t = Term::find(TERM_GAME_FILES);
            }

            if (t)
            {
                // we're too early to run flows, so just go through
                // the headflow and look for file media elements.
                // load these.
                for (auto i = t->_flow._elts.begin(); i != t->_flow._elts.end(); ++i)
                {
                    Flow::Elt* e = i;
                    if (e->_type == Flow::t_media)
                    {
                        Flow::EltMedia* em = (Flow::EltMedia*)e;
                        if (em->_mType == m_file)
                        {
                            // a game source file
                            if (!processFile(em->_filename)) v = false;
                        }
                    }
                }
            }
        }
    
        return v;
    }

    bool validate()
    {
        // ensure special terms are present
        Term::intern(TERM_TICK);
        Term::intern(TERM_LAST);
        Term::intern(TERM_IT);
        Term::intern(TERM_THAT);
        Term::intern(TERM_VERSION);
        //Term::intern(TERM_LASTGEN);
        
        bool v = linkTerms();

        if (v)
        {
            v = validateTerms();
            if (!v)
            {
                ERR0("terms do not validate");
            }
        }
        else
        {
            ERR0("terms do not link");
        }

        return v;
    }
    
};

}; // ST


