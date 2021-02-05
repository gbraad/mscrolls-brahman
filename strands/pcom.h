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
#include "pbase.h"
#include "porter.h"
#include "keywords.h"

namespace ST
{

struct Word
{
    typedef std::string string;
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
            pos_property = 0x400, // bastardised property grammar
            pos_value = 0x800, // bastardised property grammar value
            pos_query = 0x1000,

            pos_prep_rel = 0x2000, // preposition relativiser
            pos_ID = 0x4000, // is a term ID of an object
            pos_FLOW = 0x8000, // is a term ID of a flow
            pos_doverb = 0x10000, // require direct object
            pos_ioverb = 0x20000, // verb requiring indirect object
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
            "property",
            "value",
            "query",
            "ID",
        };


        // these aren't part of the name
        pos &= pos_mask;
        
        uint i = 0;
        while (pos) { ++i; pos >>= 1; }
        assert(i < ASIZE(pnames));
        return pnames[i];
    }

    string          _text;
    uint            _pos = 0;

    bool isVerb() const { return (_pos & pos_verb) != 0; }
    bool isArticle() const { return (_pos & pos_article) != 0; }
    bool isIOVerb() const { return (_pos & pos_ioverb) != 0; }
    bool isDOVerb() const { return (_pos & pos_doverb) != 0; }
    bool isID() const { return (_pos & pos_ID) != 0; }
    bool isFlow() const { return (_pos & pos_FLOW) != 0; }
    bool isQuery() const { return (_pos & pos_query) != 0; }
    bool isPrep() const { return (_pos & pos_prep) != 0; }

    // is a relativising preposition
    bool isPrepRel() const { return (_pos & pos_prep_rel) != 0; }

    bool isPOSExact(uint pos) const
    {
        return (_pos & pos_mask) == pos;
    }
        
    Word(const string& t) : _text(t) {}
    bool operator<(const Word& w) const { return _text < w._text; }

    void toLower()
    {
        // map to lower
        _text = ::toLower(_text);
    }

    friend std::ostream& operator<<(std::ostream& os, const Word& w)
    { return os << w._text; }
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
        p_adverbs,
        p_verb,  //14
        p_averb,
        p_prepverb,
        p_cs, //17 command sentence
        p_property,
        p_value,
        p_query, //20
        p_qs, // 21 query sentence
    };

    typedef std::list<Term*> Binding;
    

    const Word*     _word = 0;
    Binding*        _binding = 0;

    pnode(const Word* wt, int t = 0) : node(t), _word(wt) {}
    pnode(pnode* l, int t) : node(l, t) {}
    
    ~pnode() override { _purge(); }

    bool isNounPhrase() const { return _type >= p_noun && _type <= p_pronoun; }

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
    

    void _purge()
    {
        delete _binding; _binding = 0;
        //delete _head; _head = 0;
        //delete _next; _next = 0;
    }

};

struct ParseCommand: public ParseBase
{
    typedef std::set<Word> Dictionary;
    typedef pnode::nodeType nodeType;
    
    Dictionary          _dictionary;
    const Word*         _here;
    const Word*         _it;

    ParseCommand() { _init(); }

    const Word* findWord(const string& word) const
    {
        // dictionary words are case sensitive.
        // in the sense that uppercase words are TERM names
        // however, unless they are all uppercase, they should match
        // without case.
        
        assert(word.size());
        
        Word w(word);

        // if not all upper, change to lower for search
        if (!isUppercase(word)) w.toLower();
        
        auto it = _dictionary.find(w);
        return it == _dictionary.cend() ? 0 : &(*it);
    }

    Word& internWord(const string& word)
    {
        assert(word.size());
        
        Word w(word);
        
        // if not all upper, change to lower for search
        if (!isUppercase(word)) w.toLower();
        
        return const_cast<Word&>(*_dictionary.insert(w).first);
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

    void _internStandardWords()
    {
        static const StdWord stdWords[] =
        {
            { "and", Word::pos_conj },
            { ",", Word::pos_conj },
            { "&", Word::pos_conj },
            
            { ".", Word::pos_ender },
            { "except", Word::pos_except },
            { "but", Word::pos_except },

            { "the", Word::pos_article },
            { "a", Word::pos_article },
            { "an", Word::pos_article },
            { "some", Word::pos_article },
            { "any", Word::pos_article },

            { "in", Word::pos_prep | Word::pos_prep_rel },
            { "into", Word::pos_prep },
            { "to", Word::pos_prep },
            { "on", Word::pos_prep | Word::pos_prep_rel },
            { "onto", Word::pos_prep },
            { "under", Word::pos_prep | Word::pos_prep_rel },
            { "behind", Word::pos_prep | Word::pos_prep_rel },
            { "about", Word::pos_prep },
            { "at", Word::pos_prep },
            { "for", Word::pos_prep },

            { "it", Word::pos_pronoun },
            { "him", Word::pos_pronoun },
            { "her", Word::pos_pronoun },
            { "here", Word::pos_pronoun },
            { "there", Word::pos_pronoun },
            { "them", Word::pos_pronoun },
            { "all", Word::pos_pronoun },

            { "put", Word::pos_verb | Word::pos_doverb | Word::pos_ioverb },
            { "set", Word::pos_verb | Word::pos_doverb },
            { "is", Word::pos_verb | Word::pos_doverb | Word::pos_ioverb | Word::pos_query},
            { "are", Word::pos_verb | Word::pos_doverb | Word::pos_ioverb | Word::pos_query},

            { "what", Word::pos_query },
            { "where", Word::pos_query },
        };

        for (int i = 0; i < ASIZE(stdWords); ++i)
        {
            const StdWord& w = stdWords[i];
            internWordType(w._word, w._pos);
        }

        // locate various pronouns so we can test directly
        _here = findWord("here");
        _it = findWord("it");
    }

    string word()
    {
        // get next word
        string s;
        
        skipws();
        PUSHSPAN;

        if (ispunct(AT))
        {
            // punctuation characters appear as individual words
            s = GETC;
        }
        else
        {
            if (u_isalpha(AT)) BUMP;
            while (u_isalnum(AT)) BUMP;
            s = POPSPAN;
        }

        return s;
    }

    pnode* parseType(const string& w, Word::POS pos, uint t = 0) const
    {
        pnode* pn = 0;
        if (w.size())
        {
            const Word* wt = wordType(w, pos);

            DLOG0(_debug > 1, "parsing '" << w << "' as " << Word::posName(pos) << (wt ? " ok" : " fail"));
            
            if (wt) pn = new pnode(wt, t);
        }
        return pn;
    }

    pnode* _parseAndList(Word::POS ps, uint type)
    {
        // a and a and a ...
        // (a  a  a ...)
        pnode* pl;
        uint pc = 0;
        
        _push(); ++pc;
        pl = parseType(word(), ps, type);
        if (pl)
        {
            pnode* plast = pl;
            //_drop();

            for (;;)
            {
                _push(); ++pc;
                string w = word();
                if (wordType(w, Word::pos_conj)) w = word(); // skip "and"
                pnode* p2 = parseType(w, ps, type);
                if (p2)
                {
                    plast->_next = p2;
                    plast = p2;
                    //_drop();
                }
                else
                {
                    _pop(); --pc;
                    break;
                }
            }
            
            // have pc drops to make

            // if the last word is also another type, reject it
            // this prevents conflict between adj and noun where
            // the noun is also an adj. Basically, it cannot be an adj here.
            assert(plast->_word);
            if (!plast->_word->isPOSExact(ps))
            {
                //NB: pl can be plast
                pnode** pp = &pl;
                while (*pp != plast) pp = &(*pp)->_next;
                delete *pp;
                *pp = 0;

                // and pop the last word we've rejected
                assert(pc);
                --pc;
                _pop();
            }

            // drop remainder
            while (pc--) _drop();
            
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
        pnode* pn = parseType(word(), Word::pos_noun, nodeType::p_noun);
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
        // eg "cat fur" where two (or more) nouns are put together
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

                _push();

                // allow ' or 's
                if (AT == '\'')
                {
                    BUMP;
                    if (AT == 's') BUMP;
                }
                else
                {
                    // skip "of" word
                    of = parseLiteral("of");
                }
                
                pnode* p2 = parseANoun();
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

    pnode* parseRNoun()
    {
        // parse noun relative
        // eg the box in the hall

        // #rnoun = (#tnoun (prep #rnoun)))
        
        pnode* pn = parseTNoun();
        if (pn)
        {
            _push();

            string w = word();
            
            //const Word* nt = wordType(w, Word::pos_not);
            //if (nt) w = word();
            // XXX where to put "not"

            pnode* rn = 0;
            pnode* prep = parseType(w, Word::pos_prep, nodeType::p_prep);
            
            if (prep && prep->_word->isPrepRel()) // has to be a relativiser
            {
                // right recursive
                // eg key in the box in the hall
                rn = parseRNoun();
                if (rn)
                {
                    // make node (prep rnoun)
                    prep->_next = rn; 

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
                parseLiteral("not"); // ignore it
                
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
                // talk to, look at, etc
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
    
    static const Word* getVerb(const pnode* pn)
    {
        const pnode* vn = findSubnode(pn, nodeType::p_verb);
        return vn ? vn->_word : 0;
    }

    void parseSet(pnode* ps)
    {
        // ps = (verb (nouns))
        // forms are:
        // set N prop value

        assert(ps);
        assert(ps->_head);
        
        pnode* pn = ps->_head->_next; // nouns
        if (!pn) return;
        
        assert(!pn->_next); // no prep lifted yet.
        
        const Word* verb = getVerb(ps->_head);  // search verb branch
        if (verb && verb->_text == SYM_SET)
        {
            _push();
            
            bool ok = false;

            // new properties are stemmed 
            string prop = wordStem(word());
            if (prop.size())
            {
                // XX for now just accept a single word
                // later we'll allow a flow here
                string w = word();

                if (w.size())
                {
                    //ERR0("adding prop " << prop << ' ' << w);
                        
                    // add properties to dictionary
                    // (prop)
                    pnode* pp =
                        new pnode(&internWordType(prop, Word::pos_property),
                                  nodeType::p_property);

                    // (prop value)
                    pp->_next = new pnode(&internWordType(w, Word::pos_value),
                                          nodeType::p_value);
                    

                    // hook into sentence
                    // (set nouns (prop value))
                    pn->_next = pp;

                    ok = true;
                    _drop();
                }
            }

            if (!ok)
                _pop();
        }
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
                if (verb && verb->isIOVerb())
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
                        assert(pr->_head);
                        assert(pr->_head->_next);
                        assert(pr->_head->_next->_type == nodeType::p_prep);

                        if (dn == pr)
                        {
                            vn->_next = pr->_head;
                        }
                        else
                        {
                            // steal (prep n)
                            dn->_next = pr->_head->_next;
                            pr->_head->_next = 0;

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
                rn = parseType(word(), Word::pos_value, nodeType::p_value);
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
        // what prep N, eg what in hall => all X where, (X in hall).
        // what prop VAL, eg what feels wet => all X where, (X feels wet).

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
        // (#PS verb nouns)
        pnode* ps = parseAVerb();
        if (ps)
        {
            pnode* pn = parseNouns(); 
            if (pn)
            {
                assert(!ps->_next);
                ps->_next = pn;

                // indirect object?
                pnode* io = parseIO();
                if (io) pn->_next = io;
                
                ps = new pnode(ps, nodeType::p_cs);

                if (!io)
                {
                    skipws();
                    if (AT)
                    {
                        // more to parse!
                        parseSet(ps);
                    }
                    
                    bool v = liftParseIO(ps);
                    DLOG0(_debug > 1 && v, "lifted parse");
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
                PERR1("extra text not expected", POS);

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
    
private:

    void _init()
    {
        _internStandardWords();
    }
    
};

}; // ST
