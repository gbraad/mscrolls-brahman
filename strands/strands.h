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

#include <iostream>
#include <string>
#include <assert.h>
#include <list>
#include <vector>
#include <set>
#include <functional>
#include "utils.h"
#include "varset.h"
#include "pcom.h"
#include "pexpr.h"

namespace ST
{
    
// forward
struct Term;
struct Word;

struct Flow: public Traits
{
    enum Type
    {
        t_void = 0,
        t_text = 1,
        t_code = 2,
        t_command = 4,
        t_term = 8,
        t_media = 16,
        t_cond = 32,
    };
    
    struct Elt
    {
        Type    _type;

        Elt(Type t) : _type(t) {}
        virtual ~Elt() {}

        const char* typeString() const
        {
            static const char* stab[] =
                {
                    "void",
                    "text",
                    "code",
                    "command",
                    "term",
                    "media",
                    "cond",
                };

            return stab[bitIndex(_type)];
        }
        
        virtual string _toString(bool extra) const = 0;
        virtual string _toBaseString() const { return _toString(false); }
        
        string toString() const
        {
            string s = typeString();
            s += ':';
            s += _toString(true);
            return s;
        }

        bool isCond() const { return _type == t_cond; }

        friend std::ostream& operator<<(std::ostream& os, const Elt& e)
        { return os << e.toString(); }
    };

    struct EltText: public Elt
    {
        string      _text;
        EltText(const string& s) : Elt(t_text), _text(s) {}
        EltText(const char* s, int l) : Elt(t_text), _text(s, l) {}
        
        string _toBaseString() const override { return _text; }

        string _toString(bool x) const override
        {
            string s;
            s += '"';
            s += addEscapes(_text);
            s += '"';
            return s;
        }
        
    };

    struct EltCode: public Elt
    {
        string      _code;
        EltCode(const string& s) : Elt(t_code), _code(s) {}

        string _toString(bool x) const override
        {
            string s;
            s += "{";
            s += _code;
            s += "}";
            return s;
        }
    };

    struct EltCommand: public Elt
    {
        string      _command;
        pnode*      _parse = 0;
        int         _lineno = 0;

        EltCommand(const string& s) : Elt(t_command), _command(s) {}
        
        ~EltCommand() { delete _parse; }

        void setParse(pnode* pn)
        {
            // takes ownership
            assert(!_parse);
            _parse = pn;
        }

        string _toString(bool x) const override
        {
            string s;
            s += '\'';
            s += _command;
            s += '\'';
            if (x && _parse)
            {
                s += " [";
                s += _parse->toStringStruct(); // structural
                s += ']';
            }
            return s;
        }
    };

    struct EltMedia: public Elt
    {
        string      _filename;
        MediaType   _mType;

        // optional attributes appended
        VarSet*     _attr = 0;
        
        EltMedia(const string& s, MediaType mt)
            : Elt(t_media), _filename(s), _mType(mt) {}

        ~EltMedia() { delete _attr; }
        
        string _toString(bool x) const override
        {
            string s;
            s += '/';
            s += _filename;
            s += '/';
            return s;
        }
    };
    
    struct EltCond: public Elt
    {
        enode*  _cond;
        
        EltCond(enode* e) : Elt(t_cond), _cond(e) {}
        
        ~EltCond() { delete _cond; }
        
        string _toString(bool x) const override
        {
            assert(_cond);
            return _cond->toString();
        }
    };

    enum FlowTermFlags
    {
        // powers of 2
        ft_none = 0,
        ft_background = 1,
        ft_reset = 2,
    };

    struct EltTerm: public Elt
    {
        string      _name;
        Term*       _term = 0;  // binding
        uint        _flags = 0;

        EltTerm() : Elt(t_term) {}
        EltTerm(const string& name) : Elt(t_term), _name(name) {}

        operator bool() const { return _term != 0; }

        string _toBaseString() const override
        { return _name; }

        string _toString(bool x) const override
        {
            string s = "#";
            s += _toBaseString();
            return s;
        };

    };

    std::list<Elt*>     _elts;

    ~Flow() { _purge(); }
    
    int size() const { return _elts.size(); }
    bool isEmpty() const { return _elts.empty(); }

    operator bool() const { return !isEmpty(); }

    EltCommand* firstCommand() const
    {
        EltCommand* ec = 0;
        if (!isEmpty())
        {
            const Elt* e = _elts.front();
            if (e->_type == t_command) ec = (EltCommand*)e;
        }
        return ec;
    }

    string firstCommandWord() const
    {
        EltCommand* ec = firstCommand();
        return ec ? firstAlphabeticWord(ec->_command) : string();
    }

    string firstString() const
    {
        // if the flow is a simple string return it.
        string s;
        if (!isEmpty())
        {
            const Elt* e = _elts.front();
            if (e->_type == t_text)
            {
                EltText* et = (EltText*)e;
                s = et->_text;
            }
        }
        return s;
    }

    Elt* firstElt() const
    {
        Elt* e = 0;
        if (!_elts.empty()) e = _elts.front();
        return e;
    }

    /*
    EltTerm* firstTermElt() const
    {
        // if the flow starts with a term, return it
        EltTerm* et = 0;
        if (!isEmpty())
        {
            const Elt* e = _elts.front();
            if (e->_type == t_term) et = (EltTerm*)e;
        }
        return et;
    }


    Term* firstTerm() const
    {
        // if the flow starts with a term, return it
        EltTerm* et = firstTermElt();
        return et ? et->_term : 0;
    }
    */
    
    string toString(bool listform = true) const
    {
        string s;

        if (listform) s = "{\n";
        int cc = 0;
        for (auto i : _elts)
        {
            if (!listform && cc++) s += ' ';
            s += i->toString();
            if (listform) s += '\n';
        }

        if (listform) s += '}';
        return s;
    }

    string toBaseString() const
    {
        string s;
        int cc = 0;
        for (auto i : _elts)
        {
            if (cc++) s += ' ';
            s += i->_toBaseString();
        }

        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Flow& f)
    { return os << f.toString(); }

private:

    void _purge() {  for (auto i : _elts) delete i; _elts.clear(); }

};


struct FlowVisitor
{
    typedef std::function<bool(Flow&, FlowVisitor&)> FlowVisit;

    FlowVisit       _f;
    Term*           _hostTerm = 0;
    int             _pass = 0;
    bool            _err = true;
    bool            _skipNonReactors = false;
    int             _lineno = 0;

    bool operator()(Flow& f) { return _f(f, *this); }
    FlowVisitor(FlowVisit f) : _f(f) {}
};


struct Selector: public Traits
{
    enum CFLag
    {
        c_none = 0,
        c_once = 1,   // once chosen becomes hidden
        c_hidden = 2,  
        c_always = 4, // always available
    };

    enum OFlag
    {
        o_normal = 0,
        o_aschoice = 128,
    };

    Term*   _host;
    uint    _flags = 0;
    Flow    _text;
    Flow    _action;
    int     _id = 0; // nth selector
    int     _lineno = 0;
    bool    _isReactor = false;

    // conditional term ref
    Flow      _cond;
    
    bool hidden() const { return (_flags & c_hidden) != 0; }
    bool always() const { return (_flags & c_always) != 0; }
    bool once() const { return (_flags & c_once) != 0; }
    bool aschoice() const { return (_flags & o_aschoice) != 0; }

    Selector(Term* t) : _host(t) {}

    bool        hostIsObject() const;

    string flagsString() const
    {
        string s;
        if (_flags)
        {
            s += "Flags:";
            if (_flags & c_once) s += "once";
            if (_flags & c_hidden) s += "hidden";
            if (_flags & c_always) s += "always";            
        }
        return s;
    }

    string toString() const
    {
        string s = flagsString();

        if (_cond)
        {
            s += " ?";
            s += _cond.toString();
        }
        
        s += '\n';
        if (_text) s += "Flow:" + _text.toString() + '\n';
        if (_action) s += "Action:" + _action.toString() + '\n';
        return s;
    }

    bool visit(FlowVisitor ff)
    {
        bool v = true;

        ff._lineno = _lineno;

        if (hostIsObject())
        {
            if (!ff._skipNonReactors || _isReactor)
            {
                v = ff(_text);
            }
        }
        else
        {
            // always parse non-objects
            v = ff(_text);
        }
        
        if (!ff(_action)) v = false;
        if (!ff(_cond)) v = false;
        return v;
    }

    string id() const;
};

struct Selectors: public Traits
{
    // also has state
    std::vector<Selector*>    _selectors;

    void add(Selector* s) { _selectors.push_back(s); }

    Selector* operator[](int i) { return _selectors[i]; }

    ~Selectors() { _purge(); }

    int size() const { return _selectors.size(); }
    string toString() const
    {
        string s = "{\n";
        for (auto i : _selectors) s += i->toString();
        s += '}';
        return s;
    }

    bool visit(FlowVisitor ff)
    {
        bool v = true;
        for (auto s : _selectors) if (!s->visit(ff)) v = false;
        return v;
    }

private:

   void _purge() { for (auto i: _selectors) delete i; _selectors.clear(); }
};
   
struct Term: public Traits
{
    // terms ordered by ID
    typedef std::set<Term*, bool (*)(const Term*,const Term*)>  Terms;
    typedef std::list<Term*> TermList;
    typedef pnode::nodeType nodeType;
    
    enum Type
    {
        t_generator = 0,
        t_choice,
        t_object,
    };

    const char* typeString() const
    {
        static const char* stab[] =
            {
                "generator",
                "choice",
                "object",
            };
        
        return stab[_type];
    }

    enum RType
    {
        t_void = 0,
        t_random,
        t_shuffle,
        t_nonrandom,
        t_sequence,
        t_first,
    };

    enum Flags
    {
         f_none = 0,
         f_sticky = 1,
         f_cmd_choices = 2,
    };

    static const char* rtypeString(int t) 
    {
        static const char* stab[] =
            {
                "void",
                "random",
                "shuffle",
                "nonrandom",
                "sequence",
                "first",
            };
        
        return stab[t];
    }

    ~Term() { _purge(); }
    
    string      _name;
    Type        _type = t_generator;
    RType       _rtype = t_random;  // initial generator
    RType       _rtypenext = t_void;  // subsequent generator
    int         _flags = 0;

    // body
    Flow        _flow;
    Selectors   _selectors;
    Flow        _postflow;

    //for objects these are the parents
    // for generators this is the input.
    Flow        _topflow;

    // runtime
    typedef std::list<pnode*> NameNodes;

    // parsed term names.
    // we can have multiple names, although the first is primary and used
    // for output.
    NameNodes   _namenodes;
    Flow*       _label = 0; // points to flow in "label" selector, if any
    int         _uiID;  // code sent to UI

    static Terms _allTerms;

#define DEF_FLAG(_n, _f)                                        \
    bool        _n() const { return (_flags & _f) != 0; }       \
    void        _n(bool v)                                      \
    {                                                           \
        if (v) _flags |= _f;                                    \
        else _flags &= ~(_f);                                   \
    }

    DEF_FLAG(sticky, f_sticky);
    DEF_FLAG(cmdChoices, f_cmd_choices);


    static bool compareLess(const Term* a, const Term* b)
    {
        assert(a && b);
        return a->_name < b->_name;
    }

    Term(const string& name) : _name(name) {}

    bool   isGenerator() const { return _type == t_generator; }
    bool   isChoice() const { return _type == t_choice; }
    bool   isObject() const { return _type == t_object; }

    string primaryWord() const
    {
        // return the word which mostly identifies this object.
        // used for ui tagging
        string w;

        // if we have a label and that is a simple string
        // then use that.
        // if it's a flow, then it might change, so don't
        if (_label) w = _label->firstString();
        if (!w.size())
        {
            // use the primary noun
            if (!_namenodes.empty())
            {
                const pnode* pn = _namenodes.front();
                assert(pn);
                const pnode* p1 = ParseCommand::findSubnode(pn, pnode::p_anoun);
                if (p1) w = p1->toString();
                else
                {
                    p1 = ParseCommand::findSubnode(pn, pnode::p_noun);
                    if (p1)
                    {
                        // take just word, as next may be set to article
                        w = p1->_word->_text;
                    }
                }
            }
        }
        return w; // can be empty
    }

    static Term* find(const string& name)
    {
        // find term by ID.
        Term* ft = 0;
        if (name.size())
        {
            Term t(name);
            Terms::iterator it = _allTerms.find(&t);
            if (it != _allTerms.end()) ft = *it;
        }
        return ft;
    }

    static Term* find(const char* name)
    {
        Term* t = 0;
        if (name && *name) t = find(string(name));
        return t;
    }

    static void add(Term* t)
    {
        bool ok = _allTerms.insert(t).second;
        assert(ok);
    }

    static Term* intern(const string& name)
    {
        Term* t = find(name);
        if (!t)
        {
            t = new Term(name);
            add(t);
        }
        return t;
    }

    static Term* intern(const char* name)
    {
        assert(name && *name);
        return intern(string(name));
    }

    static bool compareWordNodes(const pnode* a, const pnode* b)
    {
        assert(a->_word);
        assert(b->_word);
        if (a->_word == b->_word) return true;
        return equalsIgnoreCase(a->_word->_text, b->_word->_text);
    }

    static bool matchName(TermList& tl, const pnode* pn, const pnode* pa)
    {
        // pn = noun
        // pa = (a a a)
        // pa can be null.

        if (pn->_word->isID())
        {
            // reference to ID look this up and ignore pa
            Term* t = find(pn->_word->_text);
            if (t) tl.push_back(t);
            return t != 0;
        }

        for (auto t : _allTerms)
        {
            if (!t->isObject()) continue;

            for (auto n : t->_namenodes)
            {
                // n = p_noun a word
                // n = (#p_anoun (noun adj))
                // n = (#p_unoun ...)
                // n = (#p_tnoun ((noun adj) the))
                

                // is a single noun not a list
                assert(!n->_next);

                bool v = false;

                do
                {
                    switch (n->_type)
                    {
                    case nodeType::p_noun:
                        {
                            // a supplied adj list cannot match an adj free
                            // word
                            if (pa) n = 0;
                            else
                            {
                                v = compareWordNodes(n, pn);
                                n = 0; // break;
                            }
                        }
                        break;
                    case nodeType::p_anoun:
                        {
                            pnode* n1 = n->_head;
                            assert(n1 && n1->_type == nodeType::p_noun);
                            pnode* a1 = n1->_next;

                            // otherwise degenerate anoun
                            assert(a1 && a1->_type == nodeType::p_adjs);

                            // first check for word match
                            v = compareWordNodes(n1, pn);

                            if (v)
                            {
                                // now check adjs
                                // each adj in `pa` must occur in a1
                                // pa can be null
                                const pnode* a = pa;
                                while (a)
                                {
                                    pnode* a2 = a1;
                                    while (a2)
                                    {
                                        if (compareWordNodes(a, a2)) break;
                                        a2 = a2->_next;
                                    }

                                    // a did not occur in a1, no match
                                    if (!a2)
                                    {
                                        v = false;
                                        break;
                                    }
                                
                                    a = a->_next;
                                }
                            }

                            n = 0; // break;
                        }
                        break;
                    case nodeType::p_unoun:
                        ERR0("TODO p_unoun node");
                        n = n->_head; // take primary, ignore others for now
                        break;
                    case nodeType::p_tnoun:
                        // ignore article for now
                        n = n->_head;
                        break;
                    default:
                        ERR1("Illegal noun node", (int)n->_type);
                        n = 0;
                    }
                } while (n);

                if (v) 
                {
                    // a match!
                    tl.push_back(t);
                    break; // no need to check more namenodes
                }
            }
        }
        return !tl.empty();
    }
    
    string _toStringBody() const
    {
        string s;
        if (_flow.size())
        {
            s += "\nFlow:";
            s += _flow.toString();
        }
        if (_selectors.size())
        {
            s += "\nSelectors:";
            s += _selectors.toString();
        }
        if (_postflow.size())
        {
            s += "\nPost:";
            s += _postflow.toString();                
        }
        s += '\n';
        return s;
    }
    
    string toString() const
    {
        string s = "Term:" + _name;
        s += " Type:";
        s += typeString();
        switch (_type)
        {
        case t_generator:
            s += " RType:";
            s += rtypeString(_rtype);
            s += " RTypenext:";
            s += rtypeString(_rtypenext);
            if (sticky()) s += " *sticky*";

            if (_topflow.size())
            {
                s += "\ninput: ";
                s += _topflow.toString(false);
            }
            s += _toStringBody();
            break;
        case t_choice:
            if (sticky()) s += " *sticky*";
            s += _toStringBody();
            break;
        case t_object:
            if (_topflow.size())
            {
                s += " Parents: ";
                s += _topflow.toString(false);
            }
            if (_flow.size())
            {
                s += '\n';
                s += _flow.toString();
            }
            if (_selectors.size())
            {
                s += "\nReactions:";
                s += _selectors.toString();
            }
            s += '\n';
            
            break;
        }
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Term& t)
    { return os << t._name; } 

    void getParents(TermList& tl)
    {
        if (_type == t_object) // only objects have parents
        {
            // flow will be terms
            for (auto e : _topflow._elts)
            {
                assert(e->_type == Flow::t_term);
                Term* ti = ((Flow::EltTerm*)e)->_term;
                assert(ti); // assume linked
                tl.push_back(ti);
            }
        }
    }

    bool visit(FlowVisitor ff)
    {
        ff._hostTerm = this;
        bool v = ff(_topflow);
        if (!ff(_flow)) v = false;
        if (!_selectors.visit(ff)) v = false;
        if (!ff(_postflow)) v = false;
        return v;
    }

private:

    void _purge()
    {
        // we own the nodes
        purge(_namenodes);
    }
};


inline std::string Selector::id() const
{
    // make a virtual ID
    return _host->_name + '_' + std::to_string(_id);
}

inline bool Selector::hostIsObject() const
{
    assert(_host);
    return _host->isObject();
}


}; // ST





