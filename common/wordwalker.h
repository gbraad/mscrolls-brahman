#pragma once

#include <string>
#include <stdlib.h>
#include "cutils.h"

struct WordWalker
{
    typedef std::string string;
    
    const char*     _s;
    const char*     _pos;

    // if true separates punctuation into "words"
    // otherwise words are strictly separated by spaces
    bool  _separatePunct = false;

    WordWalker(const char* s) { _init(s); }
    WordWalker(const string& s) { _init(s.c_str()); }

    string nextWord() 
    {
        while (u_isspace(*_pos)) ++_pos;
        const char* p = _pos;

        if (!*p) return string();  // done

        if (_separatePunct)
        {
            static const char* allowedWordChars = "-_";

            // words are [a-zA-Z0-9_-]+
            while (u_strchr(allowedWordChars, *p) || u_isalnum(*p)) ++p;

            // force at least one char
            if (p == _pos) ++p;
        }
        else
        {
            // end of word is the next space (or EOF)
            while (*p && !u_isspace(*p)) ++p;
        }
        
        const char* st = _pos;
        _pos = p;
        return string(st, _pos - st);
    }

    void reset()
    {
        _pos = _s;
    }

private:

    void _init(const char* s)
    {
        _s = s;
        reset();
    }
    
};















