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
#include <string>
#include <vector>
#include "ap.h"
#include "tformat.h"
#include "growbuf.h"
#include "logged.h"

struct TextPageHTML
{
    static void styleHTML(GrowString& gs, int linkColor = 0)
    {
        HTMLStyleBegin(gs);
        if (linkColor) HTMLStyleSetLinkColor(gs, linkColor);
        HTMLStyleEnd(gs);
    }

    static void HTMLStyleBegin(GrowString& gs)
    {
        gs.append("<style type=\"text/css\">\n");
    }

    static void HTMLStyleSetLinkColor(GrowString& gs, int rgb)
    {
        gs.appendf("a {\ncolor: #%06X;\n}\n", rgb);
    }

    static void HTMLStyleSetClassColor(GrowString& gs,
                                        const char* classname,
                                        int rgb)
    {
        gs.appendf(".%s {\ncolor: #%06X;\n}\n", classname, rgb);
    }

    static void HTMLStyleEnd(GrowString& gs)
    {
        gs.append("</style>\n");
    }

};

struct TextPage: public TextPageHTML
{
    typedef std::string string;

    struct Segment
    {
        // one segment of text.
        Segment(int id = 0) : _id(id) {}

        size_t size() const { return _text.size(); }
        
        int             _id;
        bool            _mark = false;
        string          _text;
    };

    typedef std::vector<AP<Segment> > Segments;

    TextPage() { _init(); }

    string toString() const
    {
        string s;
        for (size_t i = 0; i < _segments.size(); ++i)
            s += _segments[i]->_text;
        return s;
    }

    void toStringHTML(TextFormat& tf, GrowString& gs) const
    {
        for (size_t i = 0; i < _segments.size(); ++i)
        {
            tf._width = _width;

            // this will be the default unless specified by {width="100"}
            tf._widthForImages = _width*7/10;

            tf.setPlain(_segments[i]->_text);
            gs.append(tf._text);
        }
    }


    bool append(const char* t, int id = 0)
    {
        // return if need refresh
        bool r = false;
        if (t && *t)
        {
            if (_inprogress)
            {
                Segment* s = _last();
                assert(s);
                if (s->_id == id) s->_text += t;            
                else
                {
                    // new id means new segment
                    _inprogress = false;
                }
            }
        
            if (!_inprogress)
            {
                _append(t, id);
                _inprogress = true;
            }

            checkEndSegment();
            finish();

            r = true;
        }
        return r;
    }

    bool appendSegment(const string& t, int id)
    {
        return appendSegment(t.c_str(), id);
    }

    bool appendSegment(const char* t, int id)
    {
        // can also change or delete a segment
        
        bool changed = false;
        
        // append or replace a segment
        _inprogress = false;

        // are we blanks? this indicates the removal of a segment,
        // when a non-zero id is present
        bool blank = true;
        const char* p = t;
        while (*p) 
        {
            if (!u_isspace(*p))
            {
                blank = false;
                break;
            }
            ++p;
        }

        if (id)
        {
            // have a match?
            size_t n = _segments.size();
            if (n)
            {
                // work backward from last segment looking for id
                do {
                    Segment* si = _segments[--n].get();
                    if (si->_id == id)
                    {
                        // found
                        //LOG3("textpage, replacing existing segment ", si->_text << " -> " << t);
                        // remove this segment
                        _segments.erase(_segments.begin() + n);
                        changed = true;
                        break;
                    }
                } while (n);
            }
        }

        if (!blank)  // no append of blanks
        {
            // append text as new segment
            _append(t, id);
            changed = true;
        }
            
        if (finish()) changed = true;
        return changed;
    }

    void checkEndSegment()
    {
        // segments end with two newlines
        if (_inprogress)
        {
            int sz = _segments.size();
            if (sz)
            {
                const string& t = _segments[sz-1]->_text;
                sz = t.size();
                if (sz >= 2)
                {
                    if (t[sz-1] == '\n' && t[sz-2] == '\n')
                    {
                        // drop out of segment
                        _inprogress = false;
                    }
                }
            }
        }
    }

    bool finish()
    {
        // end segment
        //_inprogress = false;
        return _trim();
    }

    size_t segmentCount() const { return _segments.size(); }
    Segment* operator[](int i)
    {
        assert(i < (int)_segments.size());
        return _segments[i].get();
    }
    
    size_t size() const
    {
        size_t sz = 0;
        for (size_t i = 0; i < _segments.size(); ++i)
            sz += _segments[i]->size();
        return sz;
    }

#if 0
    bool replaceText(const char* text)
    {
        // true if some text appended
        bool res = false;
        
        bool done = false;

        //LOG3("replaceText, ", toString() << " -> " << text);

        _inprogress = false;

        const char* p = text;
        for (size_t i = 0; !done && i < _segments.size(); ++i)
        {
            Segment* si = _segments[i].get();
            const char* q = si->_text.c_str();
            for (;;)
            {
                if (!*p)
                {
                    if (*q)
                    {
                        // text ended, but some more text within this segment
                        
                        // truncate remainder of segment
                        size_t n = q - si->_text.c_str();
                        si->_text.erase(n, std::string::npos);
                    }

                    // else both ended at same time

                    // remove any remaining segments
                    _segments.resize(i + 1);
                    done = true;
                    break;
                }

                if (!*q)
                {
                    // end of this segment and more text.
                    break;
                }

                if (*p != *q)
                {
                    // mismatch

                    // truncate remainder of segment
                    size_t n = q - si->_text.c_str();
                    si->_text.erase(n, std::string::npos);

                    // remove any remaining segments
                    _segments.resize(i + 1);
                    break;
                }

                ++p;
                ++q;
            }
        }

        if (!done)
        {
            // append any remaining text as new segment
            if (*p) res = true;
            append(p);
        }

        finish();
        return res; // did we append?
    }

    void mergeSegments(int i, int j)
    {
        // combine segments from [i,j) 
        // allow j < 0 to mean all following i
        
        int n = (int)_segments.size();
        if (j < 0 || j > n) j = n;
        if (i < 0) i = 0;

        if (i+1 < j)
        {
            //LOG3("mergeSegments, ", i << " to " << j << " n = " << n);

            Segment* si = _segments[i].get();
            for (int k = i+1; k < j; ++k)
                si->_text += _segments[k]->_text;
            
            _segments.erase(_segments.begin()+i+1, _segments.begin()+j);
        }
    }
#endif    

    int lineForPos(int pos) const
    {
        int line = 0;
        int tpos = 0;
        for (size_t i = 0; i < _segments.size(); ++i)
        {
            const string& si = _segments[i]->_text;
            const char* p = si.c_str();
            while (*p)
            {
                if (tpos == pos) return line;
                if (*p++ == '\n') ++line;
                ++tpos;
            }
        }
        return -1; // not found
    }

    string nthLine(int n) const
    {
        GrowString gs;
        int line = 0;
        for (size_t i = 0; i < _segments.size() && line <= n; ++i)
        {
            const string& si = _segments[i]->_text;
            const char* p = si.c_str();
            while (*p)
            {
                if (*p == '\n')
                {
                    if (++line > n) break;
                }
                else
                {
                    if (line == n) gs.add(*p);
                }
                ++p;
            }
        }
        gs.add(0);
        return gs.start();
    }

    // set limit on text to keep
    void maxSize(size_t v) { _maxSize = v; }

private:

    void _append(const char* t, int id)
    {
        Segment* s = new Segment;
        s->_text = t;
        s->_id = id;
        _segments.push_back(s);
    }

    bool _trim()
    {
        // reduce text to not exceed maxsize (if defined)
        
        bool changed = false;
        if (_maxSize > 0)
        {
            // leave at least 1 segment, regardless
            size_t sz = size();
            size_t n = _segments.size();
            
            while (n > 1 && sz > _maxSize)
            {
                //LOG3("textpage, trim text from size ", sz);

                --n;
                for (size_t i = 0; i < n; ++i) _segments[i] = _segments[i+1];
                _segments.resize(n);
                sz = size();
                changed = true;
            }

            //if (changed) { LOG3("textpage, text size ", sz); }
        }
        return changed;
    }

    Segment* _last() const
    {
        assert(_segments.size() > 0);
        return _segments[_segments.size() - 1].get();
    }

    void _init()
    {
        _inprogress = false;
        _maxSize = 0;
        _width = 0;
    }

    Segments    _segments;
    bool        _inprogress;
    size_t      _maxSize;

public:
    int         _width;

};
