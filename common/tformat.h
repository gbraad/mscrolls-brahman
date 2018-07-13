/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (�) Voidware 2016-2017.
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

#include <string>
#include <functional>
#include "strutils.h"
#include "html.h"
#include "utf8.h"

struct TextFormat
{
    // convert plain text to and from HTML

    typedef std::string string;
    typedef std::function<string(const string&)> Resolver;

    TextFormat() { _init(); }

    struct Emitter
    {
        TextFormat*     _host;
        
        Emitter(TextFormat* host, string* s) : _host(host), _text(s) 
        {
            _lastChar = 0;
        }

        void operator()(const string& s) { (*this)(s.c_str()); }
        void operator()(const char* s) { (*this)(s, strlen(s)); }
    
        void operator()(const char* s, size_t n)
        {
            if (n)
            {
                _lastChar = s[n-1];
                _text->append(s, n);
            }
        }
        
        void operator()(char c)
        {
            *_text += c;
            _lastChar = c;
        }

        void emitLink(const string& link, 
                      const string* linktext = 0)
        {
            // default the text of a link to the link itself
            if (!linktext) linktext = &link;
            
            // XX need to encode any quotes in the `link' text.
            (*this)("<a href=\"");
            (*this)(link);
            (*this)("\">");
            (*this)(*linktext);
            (*this)("</a>");
        }

        string convertPercent(const char* s) const
        {
            string s2;
            const char* p = strchr(s, '%');
            if (p)
            {
                const char* q = p;
                while (q != s && u_isdigit(q[-1])) --q;
                if (q != p)
                {
                    int v = atoi(q);
                    if (v < 0) v = 0;
                    if (v > 100) v = 100;
                    
                    v = (_host->_width * v)/100;

                    s2 = string(s, q - s);
                    s2 += std::to_string(v);
                    s2.append(p + 1);

                    //LOG3("convert percentage ", s2);
                }
            }
            return s2.empty() ? s : s2;
        }

        void emitImageLink(const string& imagepath,
                           const string& alt,
                           const string& extra)
        {
            // third slash elides file://localhost/
            //(*this)("<p style=\"text-align:center\">&nbsp;");
            (*this)("<p align=\"center\">&nbsp;");

            (*this)("<img src=\"");

            bool resolved = false;

            // if path contains ":", then we have a url header, so don't 
            // add one. Otherwise, add a file header
            if (imagepath.find(':') == string::npos)
            {
                if (_host->_resolver)
                {
                    // use optional resolver to find path
                    string p = (_host->_resolver)(imagepath);
                    resolved = !p.empty();
                    if (resolved) (*this)(p);
                }
                
                if (!resolved)
                    (*this)("file:///");
            }

            if (!resolved) (*this)(imagepath);
            (*this)('"');
            
            if (!alt.empty())
            {
                (*this)(" alt=\"");
                (*this)(alt);
                (*this)('"');
            }
                      
            if (!extra.empty())
            {
                // allow raw style to be emitted
                (*this)(' ');
                (*this)(convertPercent(extra.c_str()));
            }
            else if (_host->_widthForImages)
            {
                char buf[128];
                sprintf(buf, " width=\"%d\"", _host->_widthForImages);
                (*this)(buf);
            }

            (*this)('>');
            (*this)("</p>");
        }

        void emitDiv(const string& style, const string* text = 0)
        {
            (*this)("<div class=\"");
            (*this)(style);
            (*this)("\">");
            if (text) (*this)(*text);
            (*this)("</div>");
        }
        
        string*   _text;
        char      _lastChar;
    };

    static bool looksLikeLinkMD(const char*& str,
                                string& link,
                                string& linktext,
                                string& extra)
    {
        // detect markdown style links:
        // [linktext](link){extra}
        // [linktext]    degenerate case

        const char* s = str;    
        const char* linkp;
        const char* linktextp;
        const char* extrap;
        
        bool ok = *s == '[';
        if (ok)
        {
            // scan ahead to look for format:
            // [text](link)
            linktextp = ++s;

            // scan [text]
            ok = false;
            int level = 1;
            while (*s)
            {
                if (*s == '[') ++level;
                else if (*s == ']' && !--level) { ok = true; break; }
                ++s;
            }

            if (ok)
            {
                // s -> ]
                linktext = string(linktextp, s - linktextp);
                
                //  scan (link)
                ok = *++s == '(';  // must follow "]" immediately
                if (ok)
                {
                    ok = false;
                    linkp = ++s;
                    level = 1;
                    while (*s)
                    {
                        if (*s == '(') ++level;
                        else if (*s == ')' && !--level) { ok = true; break; }
                        ++s;
                    }

                    if (ok)
                    {
                        // s -> )
                        link = string(linkp, s - linkp);
                    }

                    // optional extra, must follow immediately
                    if (*++s == '{')
                    {
                        ok = false;
                        extrap =  ++s;
                        level = 1;
                        while (*s)
                        {
                            if (*s == '{') ++level;
                            else if (*s == '}' && !--level)
                            { ok = true; break; }
                            ++s;
                        }

                        if (ok)
                        {
                            extra = string(extrap, s - extrap);
                            
                            // move to }+1
                            ++s;
                        }
                        else
                        {
                            // we failed to parse extra, but it's optional
                            // so recover from the end of () and treat the 
                            // "{" as content.
                            ok = true; 

                            // will be )+1 = {
                            s = extrap - 1;
                        }
                    }
                }
                else
                {
                    // degenerate case [linktext]...
                    // return true but with no `link` defined allow this
                    // to escape [linktext] as "linktext"
                    link.clear();
                    str = s; // ]+1
                    return true;
                }
            }

            if (ok)
            {
                // will be )+1 or }+1
                str = s;
            }
        }
        return ok;
    }

    static bool looksLikeLink(const char*& str, string& link)
    {
        const char* s = str;

        static const char* allowed = "%-._~:/?#@!$&'*+,;=";

        bool res = false;

        if (startsWithIgnoreCase(s, "http://"))
        {
            res = true;
            s += 7;
        }
        else if (startsWithIgnoreCase(s, "https://"))
        {
            res = true;
            s += 6;
        }

        if (res)
        {

            int level = 0;
            int blevel = 0;

            for (;;)
            {
                char c = *s;

                if (c == '(') ++level;
                else if (c == ')')
                {
                    if (--level < 0) break;
                }
                else if (c == '[') ++blevel;
                else if (c == ']')
                {
                    if (--blevel < 0) break;
                }
                else if (!u_isalnum(c) && !strchr(allowed, c)) break;
                ++s;
            }
            
            link = string(str, s - str);
            str = s; // update end
        }
        return res;
    }

    void setPlain(const string& t)
    {
        setPlain(t.c_str());
    }
    
    void setPlain(const char* s)
    {
        // PLAIN -> HTML
        _text.clear();
        Emitter e(this, &_text);
        _convertToHTML(s, e);
    }

    static void _html_out(char c, Emitter& out)
    {
        // characters that need to be encoded into HTML
        switch (c)
        {
        case '&': out("&amp;"); break;
        case '<': out("&lt;"); break;
        case '>': out("&gt;"); break;
        case '"': out("&quot;"); break;
        case '\'': out("&#39;"); break;
        default:
            out(c);
        }
    }

    static void _html_out(const char* s, Emitter& out)
    {
        while (*s) _html_out(*s++, out);
    }
    

    static bool scanLineFor(const char* s, char c)
    {
        // scan ahead from `s' looking for char `c` until
        // end of line is reached or end of string.
        // if found, return true

        bool r = false;
        while (*s && *s != '\n')
        {
            if (*s == c) { r = true; break; }
            ++s;
        }
        return r;
    }

    void closeList(Emitter& out)
    {
        if (_inList)
        {
            out("</ul>\n");
            out("<p>");
            _inList = false;
        }
    }

    void _convertToHTML(const char* st, Emitter& out)
    {
        bool inItalic = false;
        bool inBold = false;
        const char* s;
        char last = 0;
        bool startofline = true;

        _inList = false;
        _inP = false;


        for (s = st;*s;++s)
        {
            if (!_inP)
            {
                if (!u_isspace(*s))
                {
                    out("<p>");
                    _inP = true;
                }
            }

            if (_inP)
            {
                string link;
                string linktext;
                string extra;
                
                // blank lines end a paragraph
                if (startsWith(s, "\n\n"))
                {
                    if (!_inList)
                    {
                        out("</p>\n");
                    }
                    else
                    {
                        // blank line will drop out of list and
                        // start new para
                        out("</li>");
                        out("</ul>\n");
                        _inList = false;
                    }
                    ++s;
                    _inP = false;
                }
                else if (*s == '\n')
                {
                    if (!_inList)
                    {
                        if (s[1]) out("<br/>\n");
                    }
                    else
                    {
                        out("</li>\n");
                    }
                }
                else if (startofline && *s == '*')
                {
                    // "*" on start of line => list item
                    if (!_inList)
                    {
                        out("</p>\n"); // drop out of current para
                        out("<ul>");
                        _inList = true;
                    }

                    out("<li>");
                }
                else if (_makeLinks && looksLikeLink(s, link))
                {
                    if (startofline) closeList(out);
                    out.emitLink(link);
                    --s;
                }
                else if (_markdownLinks && looksLikeLinkMD(s,
                                                           link,
                                                           linktext, extra))
                {
                    if (startofline) closeList(out);
                    if (link.empty())
                    {
                        // degenerate escape case, just emit `linktext`
                        _html_out(linktext.c_str(), out);
                    }
                    else out.emitLink(link, &linktext);
                    --s;
                }
                else if (_markdownImages && *s == '!')
                {
                    if (startofline) closeList(out);
                    const char* s1 = s + 1;
                    if (looksLikeLinkMD(s1, link, linktext, extra))
                    {
                        // linktext is alt string
                        // link is path to file
                        s = s1 - 1; // -1 for loop inc
                        trimInplace(linktext); 
                        out.emitImageLink(link, linktext, extra);
                    }
                    else out(*s); // !
                }
                else if (_markdownDiv && *s == '#')
                {
                    if (startofline) closeList(out);
                    const char* s1 = s + 1;
                    if (looksLikeLinkMD(s1, link, linktext, extra))
                    {
                        // linktext is text string
                        // link is div style
                        s = s1 - 1; // -1 for loop inc
                        trimInplace(linktext); 
                        out.emitDiv(link, &linktext);
                    }
                    else out(*s); // #
                }
                else if (*s == ' ' && s[1] == ' ')
                {
                    // two spaces convert into non-breaking space
                    // to preserve format.
                    out("&nbsp;");
                }
                else if (*s == '_')
                {
                    if (startofline) closeList(out); 
                    if (!inItalic)
                    {
                        // start of a line or preceded by space
                        bool r = startofline || u_isspace(last);

                        if (!r)
                        {
                            // otherwise not sure. but if closes on same 
                            // line, the we assume italic _ 
                            r = scanLineFor(s + 1, '_');
                        }

                        if (r)
                        {
                            out("<i>");
                            inItalic = true;
                        }
                        else out(*s);
                    }
                    else
                    {
                        out("</i>");
                        inItalic = false;
                    }
                }                    
                else if (*s == '*' && s[1] == '*')
                {
                    if (startofline) closeList(out); 
                    if (!inBold) out("<b>");
                    else out("</b>");
                    ++s;
                    inBold = !inBold;
                }
                else
                {
                    if (startofline) closeList(out);
                    
                    // characters that need to be encoded into HTML
                    _html_out(*s, out);
                }
            }

            last = *s;
            startofline = (last == '\n');
        }

        if (_inP) out("</p>\n");
    }

    void setHTML(const string& t)
    {
        setHTML(t.c_str());
    }

    void setHTML(const char* t)
    {
        // HTML -> PLAIN
        
        //if (st != t) printf("## SIMPLIFY -> '%s'\n", st.c_str());

        _nparser.loadComments(false);
        _nparser.parse(t);

        // convert from HTML (no trim here)
        _convertHTML(_nparser.root());

        // get rid of unnecessary UTF8, then trim any space on ends
        _text = trim(simplifyChars(_text.c_str()));
    }

    static bool isHeader(NodeTag* n)
    {
        return n->_tag == "h1"
            || n->_tag == "h2"
            || n->_tag == "h3"
            || n->_tag == "h4";
    }

    static bool endsWithSpace(const char* s)
    {
        // include utf8 spaces
        Utf8 u(s);
        return u._isspace(u.lastChar());
    }

    void _convertNode(Node* n, bool emitp, Emitter& out)
    {
        if (n->type() == Node::nodeTypeTag)
        {
            NodeTag* tag = (NodeTag*)n;
            bool par = (tag->_tag == "p");
            bool head = false;

            if (par)
            {
                emitp = true;
                
                if (!_text.empty()) 
                    out("\n\n");
            }
            else if (tag->_tag == "a")
            {
                if (emitp) // only inside par
                {
                    string linkref = tag->getAttr("href").toString();
                    if (startsWithIgnoreCase(linkref, "http"))
                    {
                        emitp = false;

                        string linkcontent;
                        Emitter e(this, &linkcontent);
                        
                        // collect content inside link
                        for (Node::Nodes::iterator it = tag->_content.begin();
                             it != tag->_content.end(); ++it)
                            _convertNode(it, true, e);
                        
                        if (equalsIgnoreCase(linkref, linkcontent))
                        {
                            out(linkref);
                        }
                        else
                        {
                            // emit "content (link)"
                            out(linkcontent);
                            bool endspace = endsWithSpace(linkcontent.c_str());

                            // move space from end of content to end of bracket
                            if (!endspace) out(' ');
                            out('(');
                            out(linkref);
                            out(')');
                            if (endspace) out(' ');
                        }
                    }
                    else
                    {
                        // if not a standard link, continue recurse inside
                        
                    }
                }
            }
            else if (tag->_tag == "br")
            {
                out('\n');
            }
            else if (tag->_tag == "li")
            {
                // ensure we start a line
                if (!u_isnl(out._lastChar)) out('\n');
                emitp = true;
            }
            else if ((head = isHeader(tag)) != false)
            {
                emitp = true;
            }
            
            for (Node::Nodes::iterator it = tag->_content.begin();
                 it != tag->_content.end(); ++it)
                _convertNode(it, emitp, out);            
            
            if (head) out("\n\n");
        }
        else
        {
            NodeText* nt = (NodeText*)n;
            if (emitp)
                out(nt->_text);
        }
    }
    
    void _convertHTML(Node* root)
    {
        // convert parsed HTML into plain
        _text.clear();
        Emitter e(this, &_text);
        _convertNode(root, false, e);
    }

    string      _text;
    bool        _makeLinks;
    bool        _markdownLinks;
    bool        _markdownImages;
    bool        _markdownDiv;
    NodeParser  _nparser;
    int         _widthForImages;
    int         _width;
    Resolver    _resolver;
    bool        _inList;
    bool        _inP;

private:

    void _init()
    {
        _makeLinks = true;

        // [foo](bar) -> link
        _markdownLinks = true;

        // ![alt](image file path) 
        _markdownImages = true;

        // #[foo](bar) -> <div class="bar">foo</div>
        // NOTE: this is not markdown!!
        // i added this notation so that we can add styles to text
        _markdownDiv = true;
        
        _widthForImages = 0; // not defined
    }
};
