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
 *  Copyright (c) Strand Games 2018.
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

#include <assert.h>
#include <string.h>
#include <set>
#include <vector>
#include "logged.h"
#include "cutils.h"
#include "ifwords.h"
#include "growbuf.h"
#include "json.h"

struct WordStat
{
    typedef std::string string;
    
    char        _word[128];
    char*       _wp;

    struct WordInfo
    {
        WordInfo() { _freq = 0; }
        WordInfo(const char* w, int f = 0) : _word(w), _freq(f) {}
        WordInfo(const string& w, int f = 0) : _word(w), _freq(f) {}

        bool operator<(const WordInfo& w) const { return _word < w._word; }
        bool operator==(const WordInfo& w) const { return _word == w._word; }
        bool operator!=(const WordInfo& w) const { return _word != w._word; }

        friend std::ostream& operator<<(std::ostream& os, const WordInfo& wi)
        { return os << wi._word << ":" << wi._freq; }

        int score(int rel) const
        {
            return _freq;
        }

        bool isRelevant(int rel) const
        {
            assert(rel >= _relevance);
            return _relevance && (rel - _relevance < 100);
        }

        bool lessOrEqual(const WordInfo& wi, int rel) const
        {
            bool r1 = isRelevant(rel);
            bool r2 = wi.isRelevant(rel);

            if (r1 && r2)
            {
                return _relevance <= wi._relevance;
            }

            if (r1 == r2)
            {
                // both irrelevant
                return _freq <= wi._freq;
            }

            // irrelevant <= relevant
            // relevant > irrelevant            
            return r2;
        }
        
        string          _word;
        int             _freq;
        int             _relevance = 0;
    };

    typedef std::set<WordInfo> Words;
    typedef std::vector<string> WordList;

    Words       _words;
    Words       _verbs;
    int         _wordCount = 0;
    int         _relevanceNow = 0;

    static const int   _maxWords = 6;   // max to suggest
        
    WordStat() { _init(); }

    friend std::ostream& operator<<(std::ostream& os, const WordStat& ws)
    {
        os << ws._wordCount << " words:\n";
        bool begun = false;
        for (Words::iterator it = ws._words.cbegin();
             it != ws._words.cend(); ++it)
        {
            if (begun) os << ", ";
            begun = true;
            os << *it;
        }
        return os;
    }
        
    void learnWords(char c)
    {
        if (u_isalpha(c)) // allow only alpha in words
        {
            if (_wp - _word < (int)sizeof(_word)-1) *_wp++ = u_tolower(c);
        }
        else
        {
            // end of word
            if (_wp != _word)
            {
                *_wp = 0;

                addWord(_word);

                // for next word
                _wp = _word;
            }
        }
    }

    void reset()
    {
        _words.clear();
        _wordCount = 0;
        _relevanceNow = 1;
    }

    static bool irrelevantWord(const char* w)
    {
        //LOG3("check relevance ", w);
        
        // assume w is lower case
        if (strlen(w) < 3) return true;

        static const char* iwordTable[] =
        {
            "the",
            "and",
            "one",
            "for",
            "this",
            "that",
            "are",
            "you",
        };

        for (int i = 0; i < ASIZE(iwordTable); ++i)
            if (!strcmp(w, iwordTable[i])) return true;
        
        return false;
    }

    void addWord(const char* w)
    {
        // assume w is lower case
        
        WordInfo word(w);

        // insert if not present
        auto ret = _words.emplace(word);

        // inc frequency
        WordInfo& wi = const_cast<WordInfo&>(*ret.first);
        
        ++wi._freq;

        ++_relevanceNow;

        if (ret.second)
        {
            // new word
            
            ++_wordCount;

            if (!irrelevantWord(w)) wi._relevance = _relevanceNow;

            //if ((_wordCount % 100) == 0) LOG3("words: ", *this);
        }
        else
        {
            // update relevance, if not irrelevant
            if (wi._relevance) wi._relevance = _relevanceNow;
        }
    }

    void suggestCompletion(const char* w, WordList& words)
    {
        // w is the partial sentence. for now just use the last word
        size_t sz = strlen(w);
        const char* p = w + sz;

        // find end of last word
        while (p != w && u_isspace(p[-1])) --p;

        // find start of last word
        const char* q = p;
        while (q != w)
        {
            if (u_isspace(q[-1])) break; // found word start
            --q;
            if (!u_isalpha(*q)) return; // word has non-alpha abort
        }

        if (p == q) return; // no last word
        
        string lastWord = toLower(string(q, p - q));

        // backup from start of last word to see if it is also the first word
        while (q != w && u_isspace(q[-1])) --q;

        if (q == w)
        {
            // first word
            //LOG3("suggest verb for ", lastWord << " from " << _verbs.size());
            _suggest(_verbs, lastWord, words);
        }
        else
        {
            //LOG3("suggest word for ", lastWord);
            _suggest(_words, lastWord, words);
        }
    }

    struct WordRec
    {
        const char* word;
        int freq;
    };

    static const WordRec* gameVerbs(int* count) 
    {
        static const WordRec gameVerbTable[] =
        {
            // guild"
            { "look", 10 },
            { "examine", 10 },
            { "get", 10 },
            { "take", 5 },
            { "drop", 10 },
            { "go", 10 },
            { "walk", 1 },
            { "enter", 1 },
            { "run", 1 },
            { "move", 5 },
            { "lift", 3 },
            { "raise", 1 },
            { "lower", 1 },
            { "push", 1 },
            { "pull", 5 },
            { "smell", 1 },
            { "taste", 1 },
            { "eat", 1 },
            { "drink", 5 },
            { "put", 10 },
            { "insert", 5 },
            { "inventory", 10 },
            { "fit", 1 },
            { "empty", 1 },
            { "fill", 1 },
            { "jump", 5 },
            { "throw", 1 },
            { "punch", 2 },
            { "kick", 1 },
            { "fight", 5 },
            { "attack", 1 },
            { "kill", 1 },
            { "break", 1 },
            { "smash", 1 },
            { "remove", 5 },
            { "scrape", 1 },
            { "light", 5 },
            { "unlight", 1 },
            { "extinguish", 1 },
            { "open", 10 },
            { "close", 10 },
            { "twist", 1 },
            { "turn", 1 },
            { "say", 5 },
            { "shout", 1 },
            { "yell", 1 },
            { "call", 1 },
            { "feel", 1 },
            { "touch", 1 },
            { "wear", 10 },
            { "read", 6 },
            { "hit", 1 },
            { "steal", 1 },
            { "bash", 1 },
            { "unlock", 9 },
            { "pick", 2 },
            { "tie", 5 },
            { "fix", 1 },
            { "join", 1 },
            { "untie", 5 },
            { "feed", 1 },
            { "bite", 1 },
            { "cross", 1 },
            { "slide", 1 },
            { "blow", 1 },
            { "press", 3 },
            { "give", 1 },
            { "climb", 5 },
            { "burn", 1 },
            { "ignite", 1 },
            { "exit", 1 },
            { "ask", 1 },
            { "question", 1 },
            { "melt", 1 },
            { "listen", 1 },
            { "search", 4 },
            { "find", 1 },
            { "rub", 1 },
            { "wait", 5 },
            { "wake", 1 },
            { "buy", 1 },
            { "purchase", 2 },
            { "lock", 1 },
            { "tear", 1 },
            { "rip", 1 },
            { "cut", 1 },
            { "knock", 1 },
            { "point", 2 },
            { "shine", 1 },
            { "exits", 1 },
            { "cover", 1 },
            { "uncover", 1 },
            { "sit", 1 },
            { "stand", 1 },
            { "tell", 6 },
            { "offer", 1 },
            { "dismount", 1 },
            { "help", 1 },
            { "laugh", 1 },
            { "show", 1 },
            { "loop", 1 },
            { "dig", 1 },
            { "bet", 2 },
            { "stop", 1 },
            { "play", 3 },
            { "swim", 1 },
            { "flush", 1 },
            { "sleep", 1 },
            { "chew", 1 },
            { "reflect", 1 },
            { "hold", 1 },
            { "place", 2 },
            { "illuminate", 1 },
            { "catch", 1 },
            { "shake", 1 },
            { "roll", 1 },
            { "sing", 1 },
            { "switch", 1 },
            { "borrow", 1 },
            { "deflect", 1 },
            { "stake", 1 },
            { "inspect", 1 },
            { "assist", 1 },
            { "smear", 1 },
            { "pee", 1 },
            { "toss", 1 },
            { "beat", 1 },
            { "drag", 1 },
            { "strum", 1 },
            { "tip", 1 },
            { "unscrew", 1 },
#if 0    
            // pawn
            { "mix", 1 },
            { "hide", 1 },
            { "cast", 1 },
            { "smoke", 1 },
            { "mend", 1 },
            { "lean", 1 },
            { "ride", 1 },
            { "plant", 2 },
            { "split", 1 },
            { "separate", 1 },
            { "kiss", 1 },
            { "vote", 1 },
            { "mount", 1 },
            { "pray", 2 },
#endif            
        };

        *count = ASIZE(gameVerbTable);
        return gameVerbTable;
    }

    void buildWordsJSON(GrowString& buf)
    {
        WordsInfoBuilder b(builderEmitter, &buf);

        b.begin();

        // words:[{} {} ..]
        b.beginWords();
        for (Words::const_iterator it = _words.cbegin();
             it != _words.cend(); ++it)
        {
            const WordInfo& wi = *it;

            // dont save irrelevant words
            if (wi._relevance)
            {
                b.beginWord();
                b.tagInt(wi._word.c_str(), wi._freq);
                b.endWord();
            }
        }
        
        b.endWords();

        b.end();
        buf.add(0);
    }

    bool restoreWords(JSON& js)
    {
        bool r = true;

        reset();

        JSON::ArrayValue jwords(js, BRA_WORDS_WORDS);
        size_t sz = jwords.size();        
        for (size_t i = 0; i < sz; ++i)
        {
            const JSON::Value& ji = jwords.at(i);
            for (JSON::Value::ConstMemberIterator it = ji.MemberBegin();
                 it != ji.MemberEnd(); ++it)
            {
                const char* word = it->name.GetString();
                int freq = it->value.GetInt();

                WordInfo wi(word, freq);

                // saved words were always relevant
                wi._relevance = _relevanceNow;

                auto ret = _words.emplace(wi);
                if (ret.second)
                {
                    ++_wordCount;
                }
                else
                {
                    LOG3("restore duplicate word, ", word);
                }
            }
        }

        //LOG3("restored words: ", _wordCount);
        return r;
    }

protected:

    static void builderEmitter(char c, void* ctx)
    {
        GrowString* buf = (GrowString*)ctx;
        buf->add(c);
    }
    
    void _suggest(const Words& wlist, const string& w, WordList& words)
    {
        // assume w is lower case
        
        words.clear();
        
        WordInfo best[_maxWords];
        
        // words >= stem
        Words::iterator it = wlist.lower_bound(WordInfo(w));

        while (it != wlist.end())
        {
            const WordInfo& wi = *it;
            if (!startsWith(wi._word, w.c_str())) break;

            int i = 0;
            while (i < _maxWords && wi.lessOrEqual(best[i], _relevanceNow)) ++i;
            
            if (i < _maxWords)
            {
                // shift down
                for (int j = _maxWords-1;j>i;--j) best[j] = best[j-1];
                best[i] = wi;
            }

            ++it;
        }

        for (int i = 0; i < _maxWords; ++i)
        {
            const WordInfo& wi = best[i];
            if (!wi._freq) break;

            //LOG3("word suggest ", wi._word << " freq:" << wi._freq << (wi.isRelevant(_relevanceNow) ? " *rel*" : ""));
            words.emplace_back(wi._word);            
        }
    }

private:

    void _initVerbs()
    {
        int nv;
        const WordRec* wr = gameVerbs(&nv);
        for (int i = 0; i < nv; ++i)
        {
            _verbs.emplace(WordInfo(wr->word, wr->freq));
            ++wr;
        }
    }

    void _init()
    {
        _wp = _word;
        _initVerbs();
    }

};
