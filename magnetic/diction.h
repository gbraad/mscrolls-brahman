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
        
        bool same(const Word& w) const
        {
            return !u_stricmp(_w, w._w);
        }

        friend std::ostream& operator<<(std::ostream& os, const Word& w)
        { return os << w._w << ':' << w._id; }
        
    };
    
    struct WordOwn: public Word
    {
        // a word where we own the string 
        
        ~WordOwn() { delete _w; }
        
        WordOwn(const char* w, int id) : Word(u_strdup(w), id) {}
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

    int findMatches(const string& word, Words& words) const
    { return findMatches(word.c_str(), words); }

    int findMatches(const char* word, Words& words) const
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
        return words.size();
    }

    friend std::ostream& operator<<(std::ostream& os, const Diction& d)
    {
        for (WordOwns::const_iterator it = d._words.cbegin(); 
             it != d._words.cend(); ++it)
        {
            os << *it << " ";
        }
        return os;
    }
    
};

