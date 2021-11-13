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

#include <set>
#include <list>
#include "logged.h"
#include "pbase.h"
#include "pexpr.h"
#include "porter.h"
#include "keywords.h"

#define PRON_IT         "it"
#define PRON_HERE       "here"
#define PRON_HIM        "him"
#define PRON_HER        "her"
#define PRON_ALL        "all"
#define PRON_THAT       "that"

#define ART_A           "a"
#define ART_AN          "an"
#define ART_ANY         "any"
#define ART_THIS        "this"
#define ART_THE         "the"
#define ART_MY          "my"
#define ART_THAT        "yon"

namespace ST
{

struct Word: public Traits
{
    enum POS
        {
            pos_void = 0,
            pos_verb = 1,
            pos_adj = 2,
            pos_prep = 4,
            pos_noun = 8,
            pos_adverb = 16,
            pos_conj = 32,
            pos_article = 64,
            pos_pronoun = 128,
            pos_ender = 256,
            pos_except = 512,
            pos_prepmod = pos_except*2, // "also" and "not".
            pos_property = pos_prepmod*2, // bastardised property grammar
            pos_value = pos_property*2, // holder for enode value expression
            pos_query = pos_value*2,
            pos_prep_rel = pos_query*2, // preposition relativiser
            pos_ID = pos_prep_rel*2, // is a term ID of an object
            pos_FLOW = pos_ID*2, // is a term ID of a flow
            pos_doverb = pos_FLOW*2, // require direct object
            pos_ioverb = pos_doverb*2, // verb requiring indirect object
            pos_iolift= pos_ioverb*2, // IO lifting (as separate from ioverb)
            pos_negate = pos_iolift*2, // not
        };

    // these are additional properties
    static const uint pos_mask = ~(pos_doverb | pos_ioverb | pos_ID | pos_FLOW | pos_prep_rel);

    static const char* posName(uint pos)
    {
        static const char* pnames[] =
        {
            "void",
            "verb",
            "adjective",
            "preposition",
            "noun",
            "adverb",
            "conjunction",
            "article",
            "pronoun",
            "ender",
            "except",
            "prepmod", // also/not
            "property",
            "value",
            "query",
            "preprel",
            "ID",
            "flow",
            "doverb"
            "ioverb",
        };


        // these aren't part of the name
        pos &= pos_mask;
        
        uint i = 0;
        while (pos) { ++i; pos >>= 1; }
        assert(i < ASIZE(pnames));
        return pnames[i];
    }

    enum scopeType
    {
        scope_void = 0, // none
        scope_any_one,
        scope_the_one,
        scope_here,  // 3
        scope_have,
        scope_here_nothave,
    };

    string          _text;
    uint            _pos = 0;

    bool isVerb() const { return (_pos & pos_verb) != 0; }
    bool isNoun() const { return (_pos & pos_noun) != 0; }
    bool isArticle() const { return (_pos & pos_article) != 0; }
    bool isIOVerb() const { return (_pos & pos_ioverb) != 0; }
    bool isIOLift() const { return (_pos & (pos_iolift | pos_ioverb)) != 0; }
    bool isDOVerb() const { return (_pos & pos_doverb) != 0; }
    bool isID() const { return (_pos & pos_ID) != 0; }
    bool isFlow() const { return (_pos & pos_FLOW) != 0; }
    bool isQuery() const { return (_pos & pos_query) != 0; }
    bool isPrep() const { return (_pos & pos_prep) != 0; }

    // is a relativising preposition
    bool isPrepRel() const { return (_pos & pos_prep_rel) != 0; }
    //bool isPOSExact(uint pos) const { return (_pos & pos_mask) == pos; }
    bool isNegative() const { return (_pos & pos_negate) != 0; }
        
    Word(const string& t) : _text(t) {}

    scopeType scopeOf() const
    {
        scopeType st = scope_void;
        
        // words can indicate scope
        if (isArticle())
        {
            if (_text == ART_A || _text == ART_AN || _text == ART_ANY) st = scope_any_one;
            else if (_text == ART_THE) st = scope_the_one;
            else if (_text == ART_THIS) st = scope_here;
            else if (_text == ART_MY) st = scope_have;
            else if (_text == ART_THAT) st = scope_here_nothave; 
        }
        return st;
    }

    int compare(const Word& w) const
    {
        // words compare by their string name
        // some words are TERMS
        bool t1 = isUppercase(_text);
        bool t2 = isUppercase(w._text);

        if (t1)
        {
            if (t2)
            {
                // straight compare
                return _text.compare(w._text);
            }
            return -1; // term < non-term
        }
        else
        {
            if (t2) return 1; // non term > term
            return compareIgnoreCase(_text, w._text);
        }
    }
    
    bool operator==(const Word& w) const { return compare(w) == 0; }
    bool operator==(const char* w) const
    {
        // compare to literal must be exact
        return _text == w;
    }
    
    bool operator<(const Word& w) const { return compare(w) < 0; }

    friend std::ostream& operator<<(std::ostream& os, const Word& w)
    { return os << w._text; }
};

struct Binding
{
    typedef std::list<Term*> BindList;
    typedef Word::scopeType scopeType;

    Binding() {}
    Binding(BindList& tl)
    {
        // consume terms of tl
        _terms.splice(_terms.begin(), tl);
    }

    void merge(Binding& b)
    {
        for (auto t : b._terms)
            if (!contains(_terms, t)) _terms.push_back(t);

        // copy over any scope flags if not already
        if (!_scope) _scope = b._scope;
    }

    Term* first()
    {
        assert(!_terms.empty());
        return _terms.front();
    }

    bool empty() const { return _terms.empty(); }
    int size() const { return _terms.size(); }
    void clear() { _terms.clear(); }

    BindList    _terms;
    scopeType   _scope = Word::scope_void;
};

struct pnode: public node<pnode>
{
    enum nodeType
    {
        p_void = 0,
        p_adjs,
        p_article,
        p_noun, //3
        p_anoun,
        p_unoun,
        p_nounlist,
        p_tnoun,
        p_rnoun,
        p_rnounx,
        p_nouns,
        p_pronoun, // 11
        p_prep,
        p_prepmod,
        p_adverbs,
        p_verb,  //15
        p_averb,
        p_prepverb,
        p_cs, //18 command sentence
        p_property,
        p_value,
        p_query, //21
        p_qs, // 22 query sentence
        p_count,
    };

    
    const Word*     _word = 0;
    Binding*        _binding = 0;

    pnode(const Word* wt, int t = 0) : node(t), _word(wt) {}
    pnode(pnode* l, int t) : node(l, t) {}
    
    ~pnode() override { _purge(); }

    bool isNounPhrase() const { return _type >= p_noun && _type <= p_pronoun; }
    bool valid() const { return _type < p_count; }

    // converting to string uses a helper which optionally can have
    // a hook function to resolve nodes externally.
    
    struct SHelper
    {
        typedef std::function<bool(SHelper&)> SHook;
        
        string           s;
        SHook           _hook;
        const pnode*    _current;
    };

    void _toString(SHelper& h) const { _toString(h, _next); }
    
    void _toString(SHelper& h, pnode* next) const
    {
        //printf("tostring [%d]\n", _type);

        bool done = false;
        h._current = this;

        if (h._hook)
        {
            // if a hook is provided, try that first
            done = (h._hook)(h);
        }

        if (!done)
        {
            if (_word)
            {
                assert(!_head);
                if (h.s.size()) h.s += ' ';
                h.s += _word->_text;
            }
            else
            {
                assert(_head);
                switch (_type)
                {
                case p_anoun:
                    assert(_head->_next);
                    _head->_next->_toString(h, 0); // only one adjective
                    _head->_toString(h, 0);
                    return;
                case p_tnoun:
                    assert(_head->_next);
                    _head->_next->_toString(h);
                    _head->_toString(h, 0);
                    //return;
                    break;
                case p_value:
                    // head is not a pnode!
                    {
                        GETENODE(this);
                        h.s += en->toString();
                    }
                    break;
                default:
                    _head->_toString(h);
                }
            }
        }

        if (next) next->_toString(h);
    }

    
    string toString(SHelper* h = 0) const
    {
        SHelper h1;
        if (!h) h = &h1;
        _toString(*h);
        return h->s;
    }

    string toStringStruct() const
    {
        // version that shows structure.

        if (_type == p_value)
        {
            GETENODE(this);
            return en->toString();
        }

        string s;

        if (_word)
        {
            assert(!_head);
            s = _word->_text;
        }
        else
        {
            assert(_head);
            if (_head->_next) s += '(';
            s += _head->toStringStruct();
            if (_head->_next) s += ')';
        }

        if (_next)
        {
            s += ' ';
            s += _next->toStringStruct();
        }
        
        return s;
    }

    pnode* copy() const
    {
        // deep copy!
        // but not the bindings
        assert(valid());
        
        pnode* pn;
        if (_word)
        {
            pn = new pnode(_word, _type);
        }
        else
        {
            assert(_head);
            pn = new pnode(_head->copy(), _type);
        }
        if (_next)
        {
            pn->_next = _next->copy();
        }

        if (_word) assert(!_head);
        else assert(_head);
        
        return pn;
    }

    Binding* getBinding()
    {
        // find binding
        pnode* pn = this;

        while (pn)
        {
            assert(pn->valid());
            if (pn->_binding) return pn->_binding;
            pn = pn->_head;
        }
        return 0;
    }
    
protected:
    
    void _purge()
    {
        delete _binding; _binding = 0;

        if (_type == p_value)
        {
            GETENODE(this);
            delete en;
        }
        else
        {
            delete _head;
        }
        _head = 0;
        delete _next; _next = 0;
    }

};

struct ParseCommand: public ParseBase
{
    typedef std::set<Word> Dictionary;
    typedef pnode::nodeType nodeType;
    
    Dictionary          _dictionary;
    const Word*         _it;
    const Word*         _that;

    const Word* findWord(const string& word) const
    {
        // dictionary words are case sensitive.
        // in the sense that uppercase words are TERM names
        // however, unless they are all uppercase, they should match
        // without case. this is done in Word.
        
        assert(word.size());
        auto it = _dictionary.find(Word(word));
        return it == _dictionary.cend() ? 0 : &(*it);
    }

    Word& internWord(const string& word)
    {
        assert(word.size());
        return const_cast<Word&>(*_dictionary.insert(Word(word)).first);
    }

    Word& internWordType(const string& word, uint pos)
    {
        Word& w = internWord(word);
        w._pos |= pos;
        return w;
    }

    const Word* wordType(const string& word, Word::POS pos) const
    {
        const Word* w = 0;
        if (word.size())
        {
            w = findWord(word);
            if (w && (w->_pos & pos) == 0) w = 0;
        }
        return w;
    }


    struct StdWord
    {
        const char*  _word;
        uint         _pos;
    };

    void internStandardWords()
    {
        static const StdWord stdWords[] =
        {
            { "and", Word::pos_conj },
            { ",", Word::pos_conj },
            { "&", Word::pos_conj },
            
            { ".", Word::pos_ender },
            { "except", Word::pos_except },
            { "but", Word::pos_except },

            { ART_THE, Word::pos_article },
            { ART_A, Word::pos_article },
            { ART_AN, Word::pos_article },
            { "some", Word::pos_article },
            { ART_ANY, Word::pos_article },

            // not really articles, but works as such
            { ART_THIS, Word::pos_article },
            { ART_THAT, Word::pos_article },
            { ART_MY, Word::pos_article },

            { PROP_IN, Word::pos_prep | Word::pos_prep_rel },
            { PROP_INTO, Word::pos_prep },
            { "to", Word::pos_prep },
            { PROP_ON, Word::pos_prep | Word::pos_prep_rel },
            { PROP_ONTO, Word::pos_prep },
            { "off", Word::pos_prep },  // XX?
            { "under", Word::pos_prep | Word::pos_prep_rel },
            { "behind", Word::pos_prep | Word::pos_prep_rel },
            { "about", Word::pos_prep },
            { "at", Word::pos_prep },
            { "for", Word::pos_prep },
            { "with", Word::pos_prep },
            { "through", Word::pos_prep },
            { "out", Word::pos_prep },

            { PRON_IT, Word::pos_pronoun },
            { PRON_THAT, Word::pos_pronoun },
            { PRON_HIM, Word::pos_pronoun },
            { PRON_HER, Word::pos_pronoun },
            { PRON_HERE, Word::pos_pronoun },
            { "there", Word::pos_pronoun },
            { "them", Word::pos_pronoun },
            { PRON_ALL, Word::pos_pronoun },

            { "put", Word::pos_verb | Word::pos_doverb | Word::pos_ioverb },
            { "set", Word::pos_verb | Word::pos_doverb },
            { "is", Word::pos_verb | Word::pos_doverb | Word::pos_iolift | Word::pos_query},
            { "are", Word::pos_verb | Word::pos_doverb | Word::pos_iolift | Word::pos_query},

            { "what", Word::pos_query },
            { "where", Word::pos_query },
            
            { "not", Word::pos_prepmod | Word::pos_negate },
            { "also", Word::pos_prepmod },
        };

        for (int i = 0; i < ASIZE(stdWords); ++i)
        {
            const StdWord& w = stdWords[i];
            internWordType(w._word, w._pos);
        }

        // locate various pronouns so we can test directly
        _it = findWord("it");
        _that = findWord("that");
    }

    string word()
    {
        // get next word
        string s;
        
        skipws();
        PUSHSPAN;  // does not need corresponding pop

        // expect start with alpha
        bool ok = u_isalpha(AT);

        // allow words to start with underscore for debug commands
        if (!ok) ok = AT == '_';
            
        if (ok)
        {
            BUMP;
            // allow hypens
            while (u_isalnum(AT) || AT == '-') BUMP;
            s = POPSPAN;
        }
        else
        {
            // punctuation characters appear as individual words
            s = GETC;            
        }

        return s;
    }

    string wordadj()
    {
        // get next word, expected to be an adjective
        // here we allow apostrophe in the wordyes
        
        string s;
        
        skipws();
        PUSHSPAN;  // does not need corresponding pop

        // expect start with alpha
        bool ok = u_isalpha(AT);

        // allow words to start with underscore for debug commands
        if (!ok) ok = AT == '_';
            
        if (ok)
        {
            BUMP;
            // allow hypens and apostrophe
            while (u_isalnum(AT) || AT == '-' || AT == '\'') BUMP;
            s = POPSPAN;
        }
        else
        {
            // punctuation characters appear as individual words
            s = GETC;            
        }

        return s;
    }

    pnode* parseType(const string& w, Word::POS pos, uint t = 0) const
    {
        pnode* pn = 0;
        if (w.size())
        {
            const Word* wt = wordType(w, pos);

            if (wt)
            {
                LOG4("parsing '", w << "' as " << Word::posName(pos));
            }
            
            if (wt) pn = new pnode(wt, t);
        }
        return pn;
    }

    pnode* _parseAndList(Word::POS ps, uint type)
    {
        // a and a and a ...
        // (a  a  a ...)
        pnode* pl;
        
        _push();
        pl = parseType(wordadj(), ps, type);
        if (pl)
        {
            pnode* plast = pl;
            _drop();

            for (;;)
            {
                _push();
                string w = wordadj();
                if (wordType(w, Word::pos_conj)) w = wordadj(); // skip "and"
                pnode* p2 = parseType(w, ps, type);
                if (p2)
                {
                    plast->_next = p2;
                    plast = p2;
                    _drop();
                }
                else
                {
                    _pop(); 
                    break;
                }
            }
        }
        else _pop();
        return pl;
    }

    pnode* parseAdjlist()
    {
        // (#adjs a  a  a ...)
        // eg (big old fat)
        return _parseAndList(Word::pos_adj, nodeType::p_adjs);
    }

    pnode* parseANoun()
    {
        // parse a single noun or noun with adjectives
        
        // #pnoun
        // eg cat
        
        // #anoun = (n a ...)
        // eg (cat big old fat)
        
        _push();
        pnode* pl = parseAdjlist();

        _push();
        pnode* pn = parseType(word(), Word::pos_noun, nodeType::p_noun);

        if (pn)
        {
            _drop(); // accept noun
        }
        else
        {
            _pop();  // word that failed to be a noun
            
            // can last adjective also be a noun?
            if (pl)
            {
                int ac = 1;

                // walk to the last adj
                pnode** ap = &pl;
                while ((*ap)->_next)
                {
                    ap = &(*ap)->_next;
                    ++ac;
                }
                
                assert((*ap)->_word);

                // is it also a noun?
                if ((*ap)->_word->isNoun())
                {
                    // yes. promote it

                    pn = *ap; // noun!
                    pn->_type = nodeType::p_noun; // promote type
                    
                    *ap = 0; // remove from adj list

                    // if we only had one adj, pl should now be 0
                    assert(ac != 1 || !pl);
                }
            }
        }

        if (pn)
        {
            if (pl)
            {
                pn->_next = pl;
                pn = new pnode(pn, nodeType::p_anoun);
            }
            _drop();
        }
        else
        {
            delete pl;
            _pop();
        }
        return pn;
    }

    pnode* parseUNoun()
    {
        // parse attributive nouns or possessive syntax
        // eg "cat's fur" where two (or more) nouns are put together
        // this means "fur of cat"
        // also "cat's fur", which is also "fur of cat"
        //
        // the last noun is the main noun (eg fur), so the ordering
        // has to be reversed.
        //
        // except if "of" actually used, keep same order
        
        // #pnoun
        // #anoun
        //
        // #unoun = (#anoun #pnoun #anoun ...)
        // ordered so first is the significant 

        pnode* pl = parseANoun();
        if (pl)
        {
            pnode* plast = pl;
            for (;;)
            {
                bool of = false;
                bool ok = false;

                _push();

                // allow ' or 's
                if (AT == '\'')
                {
                    ok = true;
                    BUMP;
                    if (AT == 's') BUMP;
                    else if (AT != ' ') ok = false; // must be 's or nothing
                }
                else
                {
                    // skip "of" word
                    ok = of = parseLiteral("of");
                }
                
                pnode* p2 = ok ? parseANoun() : 0;
                
                if (p2)
                {
                    if (of)
                    {
                        // if "of" keep same order, append 
                        plast->_next = p2;
                        plast = p2;
                    }
                    else
                    {
                        // otherwise new node becomes head
                        // and last remains the same
                        p2->_next = pl;
                        pl = p2;
                    }
                    
                    _drop();
                }
                else
                {
                    _pop();
                    break;
                }
            }

            // if we are a list, make a single term
            if (pl->_next) pl = new pnode(pl, nodeType::p_unoun);
        }

        return pl;
    }

    pnode* parseNounList()
    {
        // list of nouns
        
        // #unoun
        // #nounlist = (unoun ...)
        
        pnode* pl = parseUNoun();
        if (pl)
        {
            pnode* plast = pl;
            for (;;)
            {
                pnode* p2 = 0;
                _push();
                if (wordType(word(), Word::pos_conj)) // and
                {
                    p2 = parseUNoun();
                    if (p2)
                    {
                        assert(!p2->_next);
                    
                        // same order
                        plast->_next = p2;
                        plast = p2;

                        _drop();
                    }
                }

                if (!p2)
                {
                    _pop();
                    break;
                }
            }

            if (pl->_next) pl = new pnode(pl, nodeType::p_nounlist);
        }
        return pl;
    }

    pnode* parseTNoun()
    {
        // #pronoun
        // #unoun
        // #tnoun = (#unoun the)
        
        _push();
        string w = word();

        // it
        pnode* pn = parseType(w, Word::pos_pronoun, nodeType::p_pronoun);
        if (pn)
        {
            _drop();
        }
        else
        {
            // [the] thing
            pnode* pa = parseType(w, Word::pos_article, nodeType::p_article);

            if (!pa)
                _poppush();

            // the cup and saucer
            pn = parseNounList();
            if (pn)
            {
                if (pa)
                {
                    // make node (noun the) or ((nouns) the)
                    assert(!pn->_next);
                    pn->_next = pa;

                    pn = new pnode(pn, nodeType::p_tnoun);
                }
                
                _drop();
                     
            }
            else
            {
                delete pa;
                _pop();
            }

        }
        return pn;
    }

    pnode* makePrepMod(const Word* prepmod, pnode* prepn)
    {
        // either prep or (#prepmod (prepmod prep))
        pnode* pn = prepn;
        if (prepmod)
        {
            pnode* pm = new pnode(prepmod, 0);  // XX type?
            pm->_next = pn;
            pn = new pnode(pm, nodeType::p_prepmod);
        }
        return pn;
    }

    pnode* parseRNoun()
    {
        // parse noun relative
        // eg the box [not] in the hall

        // #tn
        // #rnoun = (#tnoun prep rnoun)
        // #rnoun = (#tnoun (#prepmod prepmod prep) rnoun)
        
        pnode* pn = parseTNoun();
        if (pn)
        {
            _push();

            string w = word();

            // optional not or also
            // eg X not in Y, X also in Y.
            const Word* prepmod = wordType(w, Word::pos_prepmod);
            if (prepmod) w = word();
            
            pnode* rn = 0;
            pnode* prep = parseType(w, Word::pos_prep, nodeType::p_prep);
            
            if (prep && prep->_word->isPrepRel()) // has to be a relativiser
            {
                // right recursive
                // eg key in the box in the hall
                rn = parseRNoun();
                if (rn)
                {
                    prep = makePrepMod(prepmod, prep);
                    prep->_next = rn; // (prep rnoun)

                    assert(!pn->_next);
                    pn->_next = prep;

                    pn = new pnode(pn, nodeType::p_rnoun);
                    
                    _drop();
                }
            }

            if (!rn)
            {
                delete prep;
                _pop();
            }
        }
        return pn;
    }

    pnode* parseRNounx()
    {
        // parse except
        // eg all except the box
        // eg all but not the box

        // #rnounx = (#rnoun (except #rnounx))
        
        pnode* pn = parseRNoun();
        if (pn)
        {
            _push();
            pnode* p2 = 0;
            pnode* ex = parseType(word(), Word::pos_except);
            if (ex)
            {
                // optional "not"
                parseLiteral("not"); // XXX ignore it
                
                p2 = parseRNounx(); // right recursive

                // (but nouns) 
                ex->_next = p2;                

                assert(!pn->_next);
                pn->_next = ex;

                pn = new pnode(pn, nodeType::p_rnounx);

                _drop();
            }

            if (!p2)
            {
                delete ex;
                _pop();
            }
        }
        return pn;
    }

    pnode* parseNouns()
    {
        // #rnounx
        // #nounlist = (#rnounx ...)
        
        pnode* pl = parseRNounx();
        if (pl)
        {
            pnode* plast = pl;
            for(;;)
            {
                pnode* p2 = 0;
                _push();
                if (wordType(word(), Word::pos_conj))
                {
                    p2 = parseRNounx();
                    if (p2)
                    {
                        assert(!p2->_next);
                        
                        // same order
                        plast->_next = p2;
                        plast = p2;
                        _drop();                    
                    }
                }

                if (!p2)
                {
                    _pop();
                    break;
                }
            }

            if (pl->_next) pl = new pnode(pl, nodeType::p_nounlist);
        }
        return pl;
    }

    pnode* parseAdvlist()
    {
        // (a + a + a ...)
        return _parseAndList(Word::pos_adverb, nodeType::p_adverbs);
    }

    pnode* parseAVerb()
    {
        _push();
        pnode* pl = parseAdvlist();
        pnode* pn = parseType(word(), Word::pos_verb, nodeType::p_verb);
        if (pn)
        {
            _push();
            pnode* prep = parseType(word(), Word::pos_prep, nodeType::p_prep);
            if (prep)
            {
                // talk to, look at, get in, etc
                pn->_next = prep;
                pn = new pnode(pn, nodeType::p_prepverb);
                _drop();
            }
            else _pop();
            
            if (pl)
            {
                pn->_next = pl;
                pn = new pnode(pn, nodeType::p_averb);
            }
            _drop();
        }
        else
        {
            delete pl;
            _pop();
        }
        return pn;
    }

    static const pnode* findSubnode(const pnode* ps, uint type)
    {
        // finds first subnode of type
        const pnode* pn = 0;
        while (ps)
        {
            assert(ps->valid());
            if (ps->_type == type) return ps;
            pn = findSubnode(ps->_head, type);
            if (pn) break;
            ps = ps->_next;
        }
        return pn;
    }

    typedef std::list<pnode**> NodeRefs;

    static void findSubnodes(NodeRefs& nodes, pnode** pn, uint type)
    {
        while (*pn)
        {
            if ((*pn)->_type == type) nodes.push_back(pn);
            findSubnodes(nodes, &(*pn)->_head, type);
            pn = &(*pn)->_next;
        }
    }

    static const pnode* getVerbNode(const pnode* pn)
    {
        const pnode* p0 = pn;
        
        // walk breadth first
        while (pn)
        {
            if (pn->_type == nodeType::p_verb ||
                pn->_type == nodeType::p_prepverb ||
                pn->_type == nodeType::p_averb)
                return pn;
            pn = pn->_next;
        }

        // walk depth
        while (p0)
        {
            assert(p0->_head);
            pn = getVerbNode(p0->_head);
            if (pn) break;
            p0 = p0->_next;
        }
        return pn;
    }
    
    static const Word* getVerb(const pnode* pn)
    {
        // gets the actual word
        const pnode* vn = findSubnode(pn, nodeType::p_verb);
        return vn ? vn->_word : 0;
    }

    enode* parseValueExpr()
    {
        ParseExpr pe;

        // allow simple non-terms words as property values
        // these are wrapped as string.
        pe._allowSimpleNames = true;
        
        enode* en = pe.parse(POS, lineno);
        if (en)
        {
            SETPOS(pe.pos);
            LOG4("parseExpr, ", *en);
        }
        return en;
    }

    static pnode* makeValueNode(enode* e)
    {
        pnode* p = 0;

        if (e)
        {
            // LIES!
            // this is not a pnode at all. but they share a
            // common `node` baseclass whose types do not overlap
            p = new pnode((pnode*)e, nodeType::p_value);
        }
        return p;
    }

    bool parseSet(pnode* ps)
    {
        // ps = (verb (nouns))
        // forms are:
        // set N [not/also] prop value

        // (set nouns prop value)
        // (set nouns (#prepmod prepmod prop) value)

        assert(ps);
        assert(ps->_head);
        
        pnode* pn = ps->_head->_next; // nouns
        if (!pn) return true;
        
        assert(!pn->_next); // no prep lifted yet.

        bool ok = true;
        
        const Word* verb = getVerb(ps->_head);  // search verb branch
        if (verb && verb->_text == SYM_SET)
        {
            _push();
            
            ok = false;
            
            string w = word();

            // optional not/also
            const Word* prepmod = wordType(w, Word::pos_prepmod);
            if (prepmod) w = word();

            string prop;

            // new properties are stemmed

            // properties are not case sensitive
            w = toLower(w);
            
            if (atSimpleName(w.c_str())) prop = wordStem(w);
            else
            {
                PERR1("expected a property word instead of ", w);
                ok = false;
            }
            
            if (!prop.empty())
            {
                enode* en = parseValueExpr();

                if (en)
                {
                    //LOG1("adding prop ", prop << " " << *en);
                    
                    // add properties to dictionary
                    // (prop)
                    pnode* pp =
                        new pnode(&internWordType(prop, Word::pos_property),
                                  nodeType::p_property);

                    pp = makePrepMod(prepmod, pp);
                    pp->_next = makeValueNode(en);
                    
                    // hook into sentence
                    // (set nouns (prop (value X)))
                    pn->_next = pp;

                    ok = true;
                    _drop();
                }
            }

            if (!ok) _pop();
        }
        return ok;
    }


    bool liftParseIO(pnode* ps)
    {
        // preposition lifting on last rnoun phrase.
        // at this state we do not know the semantics, but an IO
        // verb must have an IO object, so we must lift something.

        bool changed = false;
        
        assert(ps);
        if (ps->_type == nodeType::p_cs) // command sentence only
        {
            // head is verb
            // head next is dobj

            pnode* vn = ps->_head;
            assert(vn);
            pnode* dn = vn->_next;
            if (dn && !dn->_next)  // no current prep
            {
                const Word* verb = getVerb(vn);
                if (verb && verb->isIOLift())  // also ioverb
                {
                    NodeRefs nodes;
                    findSubnodes(nodes, &vn->_next, nodeType::p_rnoun);

                    if (!nodes.empty())
                    {
                        pnode** ppr = nodes.back(); // last rnoun
                        pnode* pr = *ppr;

                        // head is some N
                        // head next is (prep rn)
                        assert(pr->_type == nodeType::p_rnoun);
                        assert(pr->_head); // TN or lower

                        // prep or prepmod
                        pnode* prep = pr->_head->_next;
                        assert(prep);

                        assert(prep->_type == nodeType::p_prep ||
                               prep->_type == nodeType::p_prepmod);

                        if (dn == pr)
                        {
                            vn->_next = pr->_head;
                        }
                        else
                        {
                            // steal (prep n)
                            dn->_next = prep;
                            pr->_head->_next = 0;  // &prep

                            // move subnode up to replace position of rnoun
                            *ppr = pr->_head;
                        }
                        
                        pr->_head = 0;
                        pr->_next = 0;
                        delete pr; // remove rnoun node
                        
                        changed = true;
                    }
                }
            }
        }
        return changed;
    }


    pnode* parseQueryX()
    {
        // what prep N, eg what [is] in hall => all X where, (X in hall).
        // what prop VAL, eg what [is] feels wet => all X where, (X feels wet).

        _push();
        string w = word();
        pnode* rn = 0;
        pnode* prep = parseType(w, Word::pos_prep, nodeType::p_prep);
        if (prep)
        {
            rn = parseRNoun();
        }
        else
        {
            string prop = wordStem(w);
            prep = parseType(prop, Word::pos_property, nodeType::p_property);
            if (prep)
            {
                // possibly null
                rn = makeValueNode(parseValueExpr());
            }
        }
        
        if (rn)
        {
            // make node (prep rnoun)
            // make node (prop value)
            prep->_next = rn; 
            _drop();
        }
        else
        {
            delete prep; prep = 0;
            _pop();
        }
        return prep;
    }

    pnode* parseQueryY()
    {
        // what N prep, eg what [is] hall in => all Y where (hall in Y).
        // what N prop, eg what [is] hall feel => all Y where (hall feel Y).

        pnode* pn = parseRNoun();
        if (pn)
        {
            _push();
            string w = word();

            pnode* prep = parseType(w, Word::pos_prep, nodeType::p_prep);
            if (!prep)
            {
                string prop = wordStem(w);
                prep = parseType(prop, Word::pos_property, nodeType::p_property);
            }

            if (prep)
            {
                // make node (N prep)
                // make node (N prop)
                assert(!pn->_next);
                pn->_next = prep;
                _drop();
            }
            else
            {
                delete pn; pn = 0;
                _pop();
            }
        }
        return pn;
    }

    pnode* parseQuery()
    {
        // typeX
        // what prep N, eg what in hall => all X where, (X in hall).
        // what prop VAL, eg what feels wet => all X where, (X feels wet).

        // typeY
        // what N prep, eg what hall in => all Y where (hall in Y).
        // what N prop, eg what hall feel => all Y where (hall feel Y).
        
        // but Y are values not terms.
        
        _push();
        pnode* prep = 0;
        pnode* pn = parseType(word(), Word::pos_query, nodeType::p_query);
        if (pn)
        {
            // allow optional "is", but ignore it.
            _push();
            const Word* wis = wordType(word(), Word::pos_verb);
            if (wis && wis->_text == SYM_IS) _drop();
            else _pop();
                
            prep = parseQueryX();
            if (!prep) prep = parseQueryY();
            if (prep)
            {
                // (what (prep rn))  eg (what in box)
                // (what (prop value)) eg (what feel wet)
                pn->_next = prep;
                pn = new pnode(pn, nodeType::p_qs);
                _drop();
            }
        }

        if (!prep)
        {
            _pop();
            delete pn; pn = 0;
        }
        return pn;
    }

    pnode* parseIO()
    {
        // (prep nouns)
        _push();

        pnode* pn = 0;
        pnode* prep = parseType(word(), Word::pos_prep, nodeType::p_prep);
        if (prep)
        {
            pn = parseNouns();
            if (pn)
            {
                prep->_next = pn;
                pn = prep;
                _drop();;
            }
        }

        if (!pn)
        {
            delete prep;
            _pop();
        }

        return pn;
    }
    
    pnode* parseSentence()
    {
        // (#cs verb)
        // (#PS verb nouns)
        // (#qs ...)
        pnode* ps = parseAVerb();
        if (ps)
        {
            pnode* pn = parseNouns(); 
            if (pn)
            {
                assert(!ps->_next);
                ps->_next = pn;  // (verb nouns)

                // indirect object?
                pnode* io = parseIO(); // (prep nouns)
                if (io) pn->_next = io; // (verb nouns prep nouns)

                // (#cs (verb nouns prep nouns))
                ps = new pnode(ps, nodeType::p_cs);

                if (!io)
                {
                    skipws();
                    if (AT)
                    {
                        // more to parse!
                        if (!parseSet(ps))
                        {
                            LOG1("parse set problem ", POS);
                        }
                    }

                    // lift (verb nouns) -> (verb nouns prep nouns)
                    bool v = liftParseIO(ps);
                    //DLOG0(_debug > 1 && v, "lifted parse");
                }
            }
            else
            {
                // elevate single verb to sentence
                assert(!ps->_next);
                ps = new pnode(ps, nodeType::p_cs);
            }
        }
        else
        {
            ps = parseQuery();
        }
        return ps;
    }

    typedef pnode* (ParseCommand::*pFn)();
    pnode* parseAux(const char* s, int line, pFn pfn, bool all = true)
    {
        // if `all`, expect to parse all of `s` rather than just the
        // first part
        
        SETPOS(s);
        lineno = line;
        lastdef = 0;
        pnode* pn = (this->*pfn)();
        if (pn && all)
        {
            skipws();
            if (AT && AT != '\n')
            {
                // more not expected
                if (line)
                {
                    PERR1("extra text not expected", POS);
                }

                delete pn;
                pn = 0;
            }
        }
        
        if (!pn && line)
        {
            PERR0("command syntax error");
        }
        return pn;
    }

    pnode* parse(const char* s, int line = 0)
    {
        return parseAux(s, line, &ParseCommand::parseSentence);
    }

    pnode* parseNoun(const char* s, int line = 0)
    {
        return parseAux(s, line, &ParseCommand::parseTNoun);
    }

    pnode* parseVerb(const char* s, int line = 0)
    {
        // if we start with a verb, return it.
        return parseAux(s, line, &ParseCommand::parseAVerb, false);
    }

    pnode* parse(const string& s, int line = 0)
    { return parse(s.c_str(), line); }

    void clear()
    {
        _dictionary.clear();
    }

};

}; // ST
