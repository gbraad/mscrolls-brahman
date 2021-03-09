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

#include <string>
#include "cutils.h"
#include "defs.h"
#include "strutils.h"

namespace ST
{

// forward
struct Term;


struct Traits
{
    typedef std::string string;

    enum MediaType
    {
        m_void = 0,
        m_image,
        m_audio,
        m_video,
        m_file,
        m_animation,
    };

    static int atSimpleName(const char* p0)
    {
        // `[A-Za-z][A-Za-z0-9_]+`
        const char* p = p0;
        int l = 0;
        if (u_isalpha(*p))
        {
            ++p;
            while (u_isalnum(*p) || *p == '_' ) ++p;
            l = p - p0;
        }
        return l;
    }

    static int atName(const char* p0)
    {
        // `[A-Z][A-Z0-9-_]+`
        const char* p = p0;
        int l = 0;
        if (u_isupper(*p))
        {
            ++p;
            // hypen cannot be after first character (eg V-neck)
            while (u_isupper(*p) || u_isdigit(*p)
                   || *p == '_'
                   || (*p == '-' && (p-p0 > 1)))
                ++p;

            l = p - p0;
            if (l < 2) l = 0;
        }
        return l;
    }

    static string firstAlphabeticWord(const char* s)
    {
        // return first word at `s`.
        // here, a "word" is a alphanumeric term starting with a letter
        while (u_isspace(*s)) ++s;
        const char* p = s;
        if (u_isalpha(*p))
        {
            ++p;
            while (u_isalnum(*p)) ++p;
        }
        return string(s, p - s); // empty if no word
    }

    static string firstAlphabeticWord(const string& s)
    { return firstAlphabeticWord(s.c_str()); }

    static string processEscapes(const char* t, int l)
    {
        // also newlines within a single text segment are changed to space
        // multiple spaces are combined.
        string s;
        bool esc = false;
        char last = 0;
        
        while (l > 0)
        {
            --l;
            char c = *t++;
            if (!c) break;
            if (esc)
            {
                esc = false;
                if (c == 'n') c = '\n'; // \n into newline
            }
            else
            {
                if (c == '\\')
                {
                    esc = true;
                    continue;
                }
                else if (c == '\n')
                {
                    if (l && *t == '\n')
                    {
                        // line break is preserved.
                        s += c; // nl
                        ++t;
                        --l;
                    }
                    else
                    {
                        c = ' '; // single nl  changed to space
                    }
                }
            }

            if (c == '\t') c = ' ';  // tabs changed to space

            // remove duplicate space
            if (c == ' ' && last == ' ') c = 0; 

            if (c)
            {
                s += c;
                last = c;
            }
        }
        return s;
    }

    static string addEscapes(const char* t)
    {
        // put escapes back into string 
        string s;
        for (;;)
        {
            char c = *t++;
            if (!c) break;
            if (c == '\n')
            {
                s += '\\';
                s += 'n';
            }
            else if (c == '"' || c == '\\')
            {
                s += '\\';
                s += c;
            }
            else
            {
                s += c;
            }
        }
        return s;
    }

    static string addEscapes(const string& t)
    { return addEscapes(t.c_str()); }

    static bool isUppercase(const char* s)
    {
        // XX ignores utf8
        // true if does not contain lower case letter
        while (*s) { if (u_islower(*s)) return false; ++s; }
        return true;
    }

    static bool isUppercase(const string& s)
    { return isUppercase(s.c_str()); }

    static uint bitIndex(uint t)
    {
        uint i = 0;
        while (t) { ++i; t >>= 1; }
        return i;
    }

    static uint countBits(uint t)
    {
        // number of bits set
        uint i = 0;
        while (t)
        {
            if (t&1) ++i;
            t >>= 1;
        }
        return i;
    }

};


struct ParseBase: public Traits
{
    const char* posStart = 0;
    const char* pos;
    const char* lastdef = 0;
    string _filename;
    int lineno = 0;
    int _debug = 0;

    char _prevc()
    {
        return pos == posStart ? 0 : pos[-1];
    }

    void _skipc()
    {
        // skip any comments
        if (*pos == '/')
        {
            // XX disallow comment after colon ':', to allow http://etc
            if (pos[1] == '/' && _prevc() != ':')
            {
                bool atStart = _prevc() == '\n';
                
                // "//" comment
                // skip to newline, but leave newline in stream
                // unless we started on a line, in case ignore the
                // whole line
                while (*++pos)
                {
                    if (*pos == '\n')
                    {
                        if (atStart)
                        {
                            ++pos; // eat newline as well
                            ++lineno;
                        }
                        break;
                    }
                }
            }
            else if (pos[1] == '*')
            {
                // C comment
                ++pos; // at '*'
                while (*++pos)
                {
                    if (*pos == '\n') ++lineno;
                    else if (*pos == '*' && pos[1] == '/')
                    {
                        pos += 2;
                        break;
                    }
                }
            }
        }
    }

    void _bump()
    {
        if (*pos++ == '\n') ++lineno;
        _skipc();
    }

#define BUMP _bump()
#define AT *pos

#define POS pos
#define GETC _getc()

// only use when we can advance without testing newlines
#define SETPOS(_x) POS = _x
#define SETPOSSTART(_x) posStart = _x

#define PUSHP \
    const char* _apos = pos;    \
    int _aline = lineno;
    
#define POPP \
    pos = _apos;    \
    lineno = _aline
    
#define PUSHSPAN \
    const char* _apos = pos;    
    
#define POPSPAN _popspan(_apos)

    string _popspan(const char* p)
    {
        // take string without comments from [p,pos]
        string s;
        const char* en = POS;
        PUSHP;
        SETPOS(p);
        while (POS != en) s += _getc();
        POPP;
        return s;
    }

    char _getc()
    {
        char c = AT;
        BUMP;
        return c;
    }

    void _advance(const char* p)
    {
        // move one by one, so we can count lines
        while (POS != p) BUMP;
    }

    string _snippet()
    {
        string s;
        const char* p = POS;
        const char* q = p;
        if (lastdef)
        {
            if (q != lastdef && q[-1] == '\n') --q;
        
            while (q != lastdef)
            {
                --q;
                if (*q == '\n')
                {
                    ++q;
                    break;
                }
            }
        }

        if (q != p)
        {
            while (q != p) s += *q++;
            s += '^';
        }

        while (*p && *p != '\n') s += *p++;
        return s;
    }


    void skipws()
    {
        _skipc();
        while (u_isspaceortab(AT)) BUMP;
    }

    void skipblank()
    {
        // skip blank lines
        for (;;)
        {
            PUSHP;
            skipws();
            if (AT != '\n') { POPP; break; }
            BUMP;
        }
    }

    const char* nextNonBlank()
    {
        PUSHP;
        while (u_isspace(AT)) BUMP;
        const char* p = POS;
        POPP;
        return p;
    }

    static bool inTable(const char** table, uint sz, const char* s)
    {
        while (sz)
        {
            if (equalsIgnoreCase(*table, s)) return true;
            --sz;
            ++table;
        }
        return false;
    }

    static bool inTable(const char** table, uint sz, const string& s)
    {
        return inTable(table, sz, s.c_str());
    }


    static MediaType mediaSuffix(const string& s)
    {
        MediaType m = m_void;
        
        static const char* imageSufTab[] =
        {
            ".jpg",
            ".jpeg",
            ".png",
            ".apng",
            ".webp",
            ".bmp",
        };

        static const char* audioSufTab[] =
        {
            ".wav",
            ".ogg",
            ".mp3",
        };

        static const char* videoSufTab[] =
        {
            ".mp4",
            ".mkv",
        };

        static const char* fileSufTab[] =
        {
            ".str",
        };

        static const char* animSufTab[] =
        {
            ".json",
        };

        if (inTable(imageSufTab, ASIZE(imageSufTab), s)) m = m_image;
        else if (inTable(audioSufTab, ASIZE(audioSufTab), s)) m = m_audio;
        else if (inTable(videoSufTab, ASIZE(videoSufTab), s)) m = m_video;
        else if (inTable(fileSufTab, ASIZE(fileSufTab), s)) m = m_file;
        else if (inTable(animSufTab, ASIZE(animSufTab), s)) m = m_animation;
        return m;
    }

    struct PPoint
    {
        const char*     _pos;
        PPoint*         _prev = 0;
        int             _line;

        PPoint(const char* p) : _pos(p) {}
    };

    PPoint*     _pstack = 0;

    void _push()
    {
        PPoint* pp = new PPoint(POS);
        pp->_prev = _pstack;
        pp->_line = lineno;
        _pstack = pp;
    }

    void _pop()
    {
        assert(_pstack);

        PPoint* pp = _pstack;
        _pstack = pp->_prev;

        SETPOS(pp->_pos); // revert
        lineno = pp->_line;
        delete pp;
    }

    void _drop()
    {
        assert(_pstack);

        PPoint* pp = _pstack;
        _pstack = pp->_prev;
        delete pp;
    }

    void _poppush()
    {
        //_pop();
        //_push();

        assert(_pstack);
        SETPOS(_pstack->_pos);
        lineno = _pstack->_line;
    }

    bool atLiteral(const char* w) const
    {
        return startsWithIgnoreCase(POS, w);
    }

    bool parseLiteral(const char* w)
    {
        skipws();
        bool v = atLiteral(w);
        if (v) SETPOS(POS + strlen(w));
        return v;
    }
    
};

#define GETENODE(_n)                    \
enode* en = (enode*)(_n)->_head;        \
assert(en && en->valid())

template<typename T> struct node
{
    typedef std::string string;
    
    uint            _type;
    T*              _head = 0;
    T*              _next = 0;

    node(int t = 0) : _type(t) {}
    node(T* l, int t) : _type(t), _head(l) {}

    virtual ~node() {}

    struct It
    {
        const T*        _n;
        It*             _up;

        It(const T* n) : _n(n), _up(0) { _down(); }

        ~It() { _purge(); }
        
        operator bool() const { return _n != 0; }
        uint type() const { return _n->_type; }

        void _down()
        {
            while (_n && _n->_head)
            {
                It* i = new It;
                i->_n = _n;
                i->_up = _up;

                _n = _n->_head;
                _up = i;
                assert(_n->valid());
            }
        }

        void _purge()
        {
            delete _up; // recursive
            _up = 0;
        }

        void _bump()
        {
            while (_n)
            {
                if (_n->_next)
                {
                    _n = _n->_next;
                    _down();
                    break;
                }
                else
                {
                    It* i = _up;
                    if (i)
                    {
                        _n = i->_n;
                        _up = i->_up;
                        i->_up = 0; // prevent stack delete
                        delete i;

                        // repeat bump
                    }
                    else
                    {
                        _n = 0; // end
                    }
                }
            }
        }

        It& operator++()
        {
            _bump();
            return *this;
        }

    private:
    
        It() {}

    };
};

    

}; // ST

