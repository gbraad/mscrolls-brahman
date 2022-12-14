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


#include <list>
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
#define SYM_ALL '$'
#define SYM_RUN_CONDITIONAL '%'

// choice flags
#define SYM_CMD_CHOICES '>'

// object types
#define SYM_CHOICE '?'
#define SYM_OBJECT '@'

// choice selector flags
#define SYM_ALWAYS  '+'
#define SYM_FILLER '-'
#define SYM_TERMINAL '!'

#define SYM_COMMAND '>'

// object flags
#define SYM_ASCHOICE '='

// input selector flag
#define SYM_INPUT '<'

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

#define SYM_FLOWBREAK '#'

#ifdef IFI_BUILD
typedef IFI::Ctx IFICtx;
#else
typedef void* IFICtx;
#endif

#define ATSELECTOR  (AT == '*' && AT1 != '*')
#define SELECTOR(_p) ((_p)[0] == '*' && (_p)[1] != '*')

namespace ST
{

struct ParseStrands: public ParseBase
{
    Term*       _startTerm = 0;
    string      _loadFilePrefix;  // files base dir

    GrowString  _sourceCollector;
    bool        _collectSource = false;

    struct VoiceInfo
    {
        string          _speech;
        string          _actor;
        string          _filename;
    };

    typedef std::list<VoiceInfo>   VoiceSet;

    VoiceSet           _voiceSet;
    bool               _inQuote = false;
    bool               _collectVoices = false;
    
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
        case SYM_ALL:
            rt = Term::t_all;
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

    void collectVoice(const Flow::EltMedia& m)
    {
        if (m._attr)
        {
            var actor = m._attr->find(MEDIA_ATTR_VOICE);
            var speech = m._attr->find(MEDIA_ATTR_SPEECH);

            // if no actor, then this isn't a voice tag
            if (actor && speech)
            {
                VoiceInfo vi;
                vi._speech = speech.toString();
                vi._actor = actor.toString();
                vi._filename = m._filename;

                // check for duplicate filenames
                for (auto& v : _voiceSet)
                {
                    if (v._filename == vi._filename)
                    {
                        LOG1("WARNING: collect Voice, duplicte filename ", v._filename);
                        break;
                    }
                }
                
                _voiceSet.push_back(vi);
            }
        }
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
            string s = processEscapes(st, p - st, _inQuote);
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

    void _addTerm(Flow& f, const string& s, uint flags, enode* refcond)
    {
        assert(s.size());
        auto t = new Flow::EltTerm(s);
        t->_flags = flags;
        t->_cond = refcond;

        if (refcond)
        {
            //LOG1("conditional reference ", t->toString());
        }
        
        f._elts.push_back(t);
    }

    Flow::EltMedia* _addMedia(Flow& f, const string& fname, MediaType mt)
    {
        assert(fname.size());
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
                bool inQuote = false;
                while (*p)
                {
                    if (*p == '"')
                    {
                        inQuote = !inQuote;
                    }
                    else
                    {
                        if (!inQuote && *p == SYM_ATTR_SEP) break;
                        if (*p == '\n') break;
                    }
                    ++p;
                }
                
                if (vs == p) break; // no value given, drop out

                string val(vs, p - vs);

                var::Format vf;
                vf._mapBool = true;

                const char* vp = val.c_str();
                vv.parse(&vp, &vf);
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

        // assume we are not in quotes at the start of any text span
        _inQuote = false;

        // XX bit hacky
        // assume that term-only flows can have conditionals
        bool allowcond = (oktypes == Flow::t_term);

        const char* q = t.c_str();
        for (;;)
        {
            _skipws(&q);
            if (!*q) break;
            
            const char* p = q;
            char lastc = 0;
            uint ftFlags = 0;

            // optional term ref conditional
            enode* refcond = 0;

            const char* refstart = 0;
            const char* refend = 0;
            string refname;
            
            while (*p)
            {
                // support conditionals before term references.
                // must have the syntax:
                // ?FOO BAR or ?(FOO) BAR
                // no space after the "?"
                
                if (allowcond)
                {
                    // conditionals all start with ?
                    bool havec = (*p == '?');

                    // determine if we have a conditional
                    if (havec)
                    {
                        // then they can be an expression (...)
                        // or a term ?FOO
                        // or a neg term ?!FOO
                        // both types must follow immediately without ws
                        const char* p1 = p + 1;
                        if (*p1 != '(')
                        {
                            if (*p1 == '!') ++p1; // neg term?
                            if (!atName(p1))
                                havec = false; // not a condition
                        }
                    }

                    if (havec)
                    {
                        // in case somehow we already had one?
                        // escapes?
                        delete refcond; refcond = 0;
                        
                        PUSHP;
                        SETPOS(p + 1);

                        refcond = parseCond();
                        if (refcond)
                        {
                            refstart = p; // at '?'

                            // park after conditional, before name
                            p = POS;
                            _skipws(&p);
                        }
                        else
                        {
                            LOG1("malformed term pre-condition, ", p);
                        }
                        POPP;
                    }
                }
                
                int l = atName(p);
                if (l)
                {
                    if (lastc == '\\')
                    {
                        // escaped term, skip this name and treat as text
                        p += l;
                        refstart = 0;
                        refend = 0;
                        continue;
                    }

                    refname = string(p, l);
                    refend = p + l;
                    ftFlags = parseFlowTermFlags(refend);
                    refend += countBits(ftFlags);  // account for flag symbols

                    // now add text range q->p and the name.
                    break;
                }
                else if (*p == '.' && p != q)
                {
                    // collect suffix and see if it's a media filename
                    // media elements can have attributes,
                    // foo.jpg:wibble:1,wobble:2,whatever:something ...
                    // these are collected and stored in a varset
                    
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

                        // ignore ![](foo) and [](foo) markdowns
                        if (s != p && *s != '!' && *s != '[')
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

                            if (_collectVoices) collectVoice(*m);
                            
                            q = p = e;
                            assert(!refend);
                            break;

                        }
                    }
                }
                
                lastc = *p++;
            }

            // add section of text before name
            // will be up to refstart or p
            if (refstart) p = refstart;
            _addText(f, q, p, oktypes);

            q = p;
            
            // now add any name
            if (refend)
            {
                // add term ref
                // consumes refcond
                assert(!refname.empty());
                _addTerm(f,  refname, ftFlags, refcond);
                refcond = 0;
                q = refend;
            }
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
                            if (ATSELECTOR)
                            {
                                // start of selector
                                break;
                            }
                            else if (AT == SYM_COMMAND)
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

                                        // don't allow any more flow
                                        if (!allowednl) tn = Flow::t_void;
                                
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
                    if (AT == SYM_COMMAND)
                    {
                        BUMP;
                        skipws();
                    }
                    
                    PUSHSPAN;

                    // commands must be on a single line
                    // but can be ended by a flow break '#'
                    char c;
                    for (;;)
                    {
                        c = AT;
                        if (!c || c == '\n' || c == SYM_FLOWBREAK) break;
                        BUMP;
                    }
                    
                    addCommand(f, trim(POPSPAN), oktypes);

                    // eat char after command
                    if (c) BUMP;
                    
                    // assume text follows code
                    tn = Flow::t_text;

                    // don't allow any more flow
                    // except for flow-breaks which can add further text
                    if (!allowednl && c != SYM_FLOWBREAK) tn = Flow::t_void;
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

        // get the type indicator
        char c = AT;

        // type is generator by default
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

                for (;;)
                {
                    // [!>]

                    bool used = false;
                    
                    if (AT == SYM_STICKY)
                    {
                        used = true;
                        if (t->_rtype == Term::t_shuffle
                            || t->_rtype == Term::t_nonrandom)
                        {
                            // no point having state for term used once
                            LOG1("sticky term changed to random ", t->_name);
                            t->_rtype = Term::t_random;
                        }
                        if (t->_rtype == Term::t_sequence ||
                            t->_rtype == Term::t_all)
                        {
                            // no need for sequence, same as first match
                            LOG1("sticky term changed to first ", t->_name);
                            t->_rtype = Term::t_first;
                        }
            
                        t->sticky(true);
                    }
                    else if (AT == SYM_RUN_CONDITIONAL)
                    {
                        used = true;

                        // when term is referenced in a conditional, we run the
                        // term, rather than testing for visit.
                        // Such terms are expected to return EXEC_TRUE or EXEC_FALSE
                        t->runConditional(true);
                    }

                    if (!used) break;
                    
                    BUMP;
                    skipws();
                }

                if (AT == SYM_COMMAND || AT == '{' || u_isalnum(AT)) 
                {
                    // optional input top flow
                    // can be text, code or command
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

            // ? !>
            for (;;)
            {
                bool used = false;
                
                if (AT == SYM_STICKY)
                {
                    t->sticky(true);
                    used = true;
                }
                else if (AT == SYM_CMD_CHOICES)
                {
                    // signify reactors marked as choices will be elevated
                    // to choices.  eg FOO?>
                    t->cmdChoices(true);
                    used = true;
                }

                if (!used) break;
                
                BUMP;
                skipws();
            }

            if (u_isalnum(AT) || AT == '?')
            {
                // topflow on choice are inherited choices
                // including ?conditional
                parseFlow(t->_topflow, 0, Flow::t_term);
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
            if (!v)  { delete t; t = 0; }
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
        uint f = 0;
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

    uint parseInputFlags(Selector* s)
    {
        uint f = 0;
        uint fi = Selector::i_input;
        for (;;)
        {
            char c = AT;
            if (c == SYM_INPUT)
            {
                f |= fi;
                fi <<= 1;  // NB: assume input flags in power2 sequence.
                if (fi > Selector::i_exec) break;
            }
            else break;

            BUMP;
            skipws();
        }
        return f;
    }

    enode* parseCond()
    {
        ParseExpr pe;
        enode* en = pe.parse(POS, lineno);
        SETPOS(pe.pos);
        return en;
    }

    void parseCondFlow(Flow& f)
    {
        enode* en = parseCond();
        if (en)
        {
            //LOG1("parseCondFlow ", en->toString());
            _addCond(f, en);
        }
    }

    Selector* parseSelector(Term* host)
    {
        // POS just past "*"
        // *=[+-!][<][?]
        Selector* s = new Selector(host);
        s->_lineno = lineno;

        uint f = 0;

        if (host->isObject()) f |= parseObjectFlags(s);

        // objects can have choice flags
        f |= parseChoiceFlags(s);

        // so that flags can be in any order
        if (host->isObject()) f |= parseObjectFlags(s);
        
        f |= parseInputFlags(s);

        s->_flags = f;

        skipws();

        // NB: object reactions can have conditionals just like 
        // everything else.
        if (AT == SYM_CONDITION)
        {
            // selector conditional prefix
            BUMP;
            
            parseCondFlow(s->_cond);
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
        if (!ATSELECTOR && AT != '\n')
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
        assert(ATSELECTOR);

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
        if (!ATSELECTOR)
        {
            // headflow: only commands allowed for objects
            if (t->_type == Term::t_object) oktypes = Flow::t_command;
            parseFlow(t->_flow, 2, oktypes);
        }

        const char* p = nextNonBlank();
        // a selector

        if (SELECTOR(p))
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

                const char* p = nextNonBlank();
                if (SELECTOR(p))
                {
                    // allow header blanks before first selector
                    nobody = false;
                }
            }

            if (!nobody)
            {
                if (!parseBody(t)) { delete t; t = 0; }
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
                t->getStaticParents(parents);
                for (auto p : parents)
                {
                    if (p->_type == Term::t_object)
                    {
                        // only leaves are instances
                        p->isClass(true);
                    }
                    else
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

                    if (_dump)
                    {
                        LOG("", t->toString());
                    }
                }
                else
                {
                    LOG1("ERROR: Duplicate term ", t->_name);
                    delete t;
                }
            }
            else
            {
                PERR0("expected term");
                break;
            }
        }
    }

    char* loadFile(const string& filename,
                   IFICtx* ctx = 0,                   
                   bool* wasCompressed = 0)
    {
        unsigned char* data = 0;
        
        lineno = 1;
        _filename = makePath(_loadFilePrefix, filename);

        LOG3("processfile ", _filename);

        FD fd;
        if (fd.open(_filename))
        {
            // try reading compressed first
            size_t usize;
            data = readz(fd, &usize);

            if (data)
            {
                FD::Pos sz = usize;
                FD::removeDOSLines(data, sz);
                if (wasCompressed) *wasCompressed = true;
            }
            else
            {            
                FD::Pos fsize;
                data = fd.readAll(&fsize, true); // remove dos newlines
            }
        }

#ifdef IFI_BUILD
        if (!data && ctx)
        {
            if (ctx->_loader)
            {
                int sz;
                LOG2("Using fallback ifi loader for ", _filename);
                data = (unsigned char*)ctx->_loader(_filename.c_str(), sz);
                if (data)
                {
                    FD::Pos size = sz;
                    FD::removeDOSLines(data, size);
                }
            }
        }
#endif // IFI_BUILD

        if (!data)
        {
            ERR1("Can't open input file", filename);
        }
        return (char*)data;
    }

    bool loadFiles(std::vector<std::string>& files,
                   IFICtx* ctx = 0,
                   bool loadGameFiles = true)
    {
        // false if any file fails
        bool v = true;
        bool wasCompressed = false;

        for (auto& f : files)
        {
            char* data = loadFile(f, ctx, &wasCompressed);
            if (!data) break;
            
            processString(data);
            delete data;
            
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
                            char* data = loadFile(em->_filename, ctx);
                            if (data)
                            {
                                processString(data);
                                delete data;
                            }
                            else v = false;
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
        Term::intern(TERM_UNDO);
        Term::intern(TERM_LAST);
        Term::intern(TERM_IT);
        Term::intern(TERM_THAT);
        Term::intern(TERM_VERSION);
        
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


