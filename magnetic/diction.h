/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2016-2017.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@voidware.com
 */

#pragma once

#include <set>
#include <vector>
#include <iostream>
#include <string>
#include "cutils.h"
#include "strutils.h"
#include "logged.h"

struct Diction
{
    // a faster way to lookup words and items from the game

    typedef std::string string;

    struct Word
    {
        const char*     _w;
        int             _id;

        Word(const char* w, int id)
        {
            _w = w;
            _id = id;
        }

        bool operator<(const Word& w) const
        {
            int c = u_stricmp(_w, w._w);
            return c < 0 || (!c && _id < w._id); 
        }
        
        bool same(const Word& w) const { return equalsIgnoreCase(_w, w._w); }

        bool matchesPhrase(const char* s1) const
        {
            const char* pat;
            for(pat = _w; *pat; ++s1, ++pat)
            {
                char c = u_tolower(*pat);
                if (c == '_') c = ' ';
                if (u_tolower(*s1) != c) return false;
            }
            return !*pat;
        }

        bool isPhrase() const
        {
            // phrases have underscores
            return _w && u_strchr(_w, '_') != 0;
        }

        int phraseLength() const
        {
            // number of words in phrase, or 1 if a single word
            int n = 0;
            for (const char* p = _w; *p; ++p) if (*p == '_') ++n;
            return n + 1;
        }

        string firstWord() const
        {
            // if we're a phrase return the first word, else just our word
            const char* p = _w;
            while (*p && *p != '_') ++p;
            return string(_w, p - _w);
        }

        friend std::ostream& operator<<(std::ostream& os, const Word& w)
        { return os << w._w << ':' << w._id; }
        
    };
    
    struct WordOwn: public Word
    {
        // a word where we own the string 
        ~WordOwn() { delete _w; }
        
        WordOwn(const char* w, int id) : Word(u_strdup(w), id) {}
        WordOwn(const char* w, int id, int size)
            : Word(u_strndup(w, size), id) {}
        WordOwn(const WordOwn& w) : Word(w)
        {
            ((Word&)w)._w = 0; // consume 
        }
    };

    typedef std::set<WordOwn> WordOwns;
    typedef std::vector<Word> Words;

    WordOwns       _words;

    bool add(const WordOwn& w)
    {
        bool v = _words.insert(w).second;
        assert(v);
        return v;
    }

    void findMatches(const string& word, Words& words) const
    { return findMatches(word.c_str(), words); }

    void findMatches(const char* word, Words& words) const
    {
        Word t(word, 0);
        WordOwns::const_iterator it = _words.lower_bound((const WordOwn&)t);
        while (it != _words.cend())
        {
            const WordOwn& wi = *it;
            if (!wi.same(t)) break;
            words.push_back(wi);
            ++it;
        }
    }

    void findPhraseMatches(const char* phrase, Words& words) const
    {
        // `phrase` points to the starts of a phrase with words after it
        // eg "lump of coal is here".
        // locate phrase words like "lump of coal"
        // NB: do not find single words like "lump"
        // ONLY PHRASE WORDS MATCH

        const char* e = strchr(phrase, ' ');
        if (e)
        {
            // isolate the first word and locate it
            WordOwn t(phrase, 0, e - phrase);
            WordOwns::const_iterator it = _words.lower_bound(t);

            // if we locate the first word of a phrase, the rest must follow
            while (it != _words.cend())
            {
                const WordOwn& wi = *it;

                // as soon as the initial word changes, we're done
                if (!equalsIgnoreCase(t._w, wi.firstWord())) break;

                if (wi.isPhrase() && wi.matchesPhrase(phrase))
                {
                    words.push_back(wi);
                    //LOG3("######## findPhraseMatch ", wi._w);
                }
                ++it;
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Diction& d)
    {
        for (WordOwns::const_iterator it = d._words.cbegin(); 
             it != d._words.cend(); ++it) os << *it << " ";
        return os;
    }
    
};

