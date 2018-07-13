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

#include <string>
#include <map>
#include "dl.h"
#include "cutils.h"
#include "var.h"
#include "logged.h"

// forward
struct NodeTag;

struct NodeTraits
{
    static const int flagEmpty = 1;
    static const int flagClose = 2;
    static const int flagXMLDecl = 4;
    static const int flagComment = 8;

    typedef std::string string;
};

struct Node: public DL<Node>, public NodeTraits
{
    static const int nodeTypeText = 1;
    static const int nodeTypeTag = 2;

    typedef DL<Node>::List              Nodes;

    Node() { _init(); }

    virtual ~Node() {}
    virtual int type() const = 0;
    virtual string toString() const = 0;

    NodeTag*    parent() const { return _parent; }
    void        parent(NodeTag* n) { _parent = n; }

protected:

    void _init() { _parent = 0; }

    NodeTag*            _parent;
};

struct NodeText: public Node
{
    NodeText() {}
    NodeText(const string& s) : _text(s) {}

    // Node, compliance
    int type() const override { return nodeTypeText; }
    string toString() const override { return _text; }
    
    string      _text;
};

struct NodeTag: public Node
{
    typedef std::map<string, var> NamedValueSet;

    NodeTag() { _init(); }
    NodeTag(const string& tag) : _tag(tag) { _init(); }

    ~NodeTag() { clear(); }


    bool                isClosing() const { return (_flags & flagClose) != 0; }
    bool                isEmpty() const { return (_flags & flagEmpty) != 0; }
    bool                isXML() const { return (_flags & flagXMLDecl) != 0; }
    bool                isComment() const
                        { return (_flags & flagComment) != 0; }

    void                setFlag(bool v, int flag)
    {
        if (v) _flags |= flag; else _flags &= ~flag;
    }
    
    void                setEmpty(bool v) { setFlag(v, flagEmpty); }

    int                 flags() const { return _flags; }

    void                flags(int v) { _flags = v; }

    // Node, compliance    
    int type() const override { return nodeTypeTag; }

    var getAttr(const char* tag) const
    {
        for (NamedValueSet::const_iterator it = _attr.cbegin();
             it != _attr.cend(); ++it)
        {
            if (equalsIgnoreCase(it->first, tag))
                return it->second.copy();

        }        
        return var();
    }

    // Node, compliance
    string toString() const override
    {
        // can we use <foo/> syntax?
        bool empty = _content.empty() && isEmpty(); 

        string s("<");
        
        if (isXML()) s += '?';

        if (isComment())
            s += "!--";

        s += _tag;
        
        // emit attributes
        for (NamedValueSet::const_iterator it = _attr.cbegin();
             it != _attr.cend(); ++it)
        {
            s += ' ';
            s += it->first;
            s +=  "=\"";
            s += it->second.toString();
            s += '"';
        }

        // emit trailing slash before close if empty
        if (empty)
        {
            if (isXML()) s += '?';
            else if (isComment()) s += "--";
            else s += '/';
        }

        // close of tag
        s += ">";

        bool first = true;
        
        if (!empty)
        {
            for (Nodes::iterator it = _content.begin();
                 it != _content.end(); ++it)
            {
                const Node& ni = *it;
                if (first && ni.type() == nodeTypeTag)
                    s += '\n';
                
                first = false;
                s += ni.toString();
            }
            
            s += "</";
            s += _tag;
            s += '>';
        }

        // do not always put a newline after close
        // for example <b>A</b><b>B</b> renders differently 
        //s += '\n';
        return s;
    }

    void add(Node* n)
    {
        n->parent(this);
        _content.add(n);
    }

    void clear()
    {
        while (!_content.empty())
            delete _content.first();
    }

    void moveChildrenToParent()
    {
        if (_parent)
        {
            while (!_content.empty())
            {
                Node* ci = _content.first();
                ci->remove();
                _parent->add(ci);
            }
        }
    }

    string              _tag;
    Nodes               _content;
    NamedValueSet       _attr;

 protected:

    void _init()
    {
        _flags = 0;
    }

    int                 _flags;
};

struct NodeParser: public NodeTraits
{
    NodeParser() : _basePtr(0) { _init(); }

    ~NodeParser() { _purge(); }

    NodeTag*    root() { return _root; }

    void parse(const string& s)
    {
        parse(s.c_str());
    }

    void parse(const char* s)
    {
        _purge();
        _root = new NodeTag("root");
        
        _basePtr = s;
        const char* p = _basePtr;
        parse(_root, &p);
    }

    void loadComments(bool v) { _loadComments = v; }

    static const char* skipTo(const char* s, char c)
    {
        // advance to `c' or 0
        while (*s && *s != c) ++s;
        return s;
    }

    static const char* skipTo(const char* s, const char* sp)
    {
        // advance to a member of `sp' or 0
        while (*s) 
        {
            if (*skipTo(sp, *s)) return s;
            ++s;
        }
        return s;
    }
    
    static const char* skipIdentifier(const char* s)
    {
        static const char noIdentChar[] = " \t\n\r=<>/\"?";
        while (*s && !*skipTo(noIdentChar, *s)) ++s;
        return s;
    }

    static const char* skipWord(const char* s)
    {
        static const char noWordChar[] = " \t\n\r<>&";

        // initially skip any bad chars
        while (*skipTo(noWordChar, *s)) ++s;

        // then skip until bad
        while (*s && !*skipTo(noWordChar, *s)) ++s;

        return s;
    }
    
    bool ignoreSpaceTag(NodeTag* n)
    {
        static const char* ignoreTags[] =
        {
            "p",
            "br"
        };
        
        for (size_t i = 0; i < DIM(ignoreTags); ++i)
        {
            if (equalsIgnoreCase(n->_tag, ignoreTags[i]))
                return true;
        }
        return false;
    }

    bool parse(NodeTag* root, const char** sptr)
    {
        // parse the content of a node

        bool res = true;
        const char* sp = *sptr;
        NodeTag* lastTag = 0;
        while (*sp)
        {
            
            // ignore space after start of node
            // eg <b> hello

            // space between nodes sometimes matters
            // for example <b>hello</b> <b>world</b>
            // but not, <p>hello</p> <p>world</p>
            
            if (!lastTag || ignoreSpaceTag(lastTag))
            {
                sp = skipSpace(sp);
                if (!*sp) break;
            }
            
            // find end of content between nodes
            const char* cp = skipTo(sp, '<');

            NodeTag* n = 0;
            const char* cp2 = cp;
            if (*cp)
            {
                n = parseNodeTag(&cp2);
                if (n)
                {
                    if (n->isClosing())
                    {
                        // ignore space before end of node
                        // eg foo </b>
                        while (cp > sp && u_isspace(cp[-1])) --cp;
                    }
                }
            }

            if (sp != cp)
            {
                // collect content between nodes
                string c = makeContent(sp, cp);
                if (!c.empty())
                    root->add(new NodeText(c));
            }

            if (!*cp2) break;
            sp = cp2;

            // now we are at the start of a node
            if (n)
            {
                if (n->isClosing())
                {
                    if (equalsIgnoreCase(n->_tag, root->_tag))
                    {
                        // we're closed!
                    }
                    else
                    {
                        // malformed closure

                        // are we a closure for the parent node
                        // eg <A><B><X></X></A>
                        // this means <B> was an empty node
                        //
                        // eg2 <p>hello<br>world</p>
                        //
                        NodeTag* pp = root->parent();
                        if (pp && equalsIgnoreCase(n->_tag, pp->_tag))
                        {
                            // yes, move children up.
                            // eg X becomes child of A.
                            root->moveChildrenToParent();

                            // mark as empty node
                            root->setEmpty(true);
                        }
                        else
                        {
                            // fail
                            LOG1("HTML ERROR,  line",
                                 _lineNum(sp)
                                 << " orphaned close '" << n->_tag << "'");
                            
                            // but don't fail
                            //res = false;
                        }
                    }
                    break; // done
                }
                else if (n->isEmpty())
                {
                    // empty nodes do not have content
                    if (!_loadComments && n->isComment())
                    {
                        delete n;
                        n = 0;
                    }
                    else
                    {
                        root->add(n);                    
                    }
                }
                else
                {
                    // `n' is a new node of root
                    root->add(n);
                    res = parse(n, &sp);
                    if (!res)
                        break;
                }

                if (n) lastTag = n;
            }
            else
            {
                // malformed tag
                res = false;
                break;
            }
        }
        *sptr = sp;
        return res;
    }

    static const char* skipSpace(const char* s)
    {
        while (u_isspace(*s)) ++s;
        return s;
    }

    static const char* skipToOutQuotes(const char* s, char c)
    {
        // advance to c outside quotes or 0
        bool inQuote = false;
        char qc;
        while (*s) 
        {
            if (u_isquote(*s))
            {
                if (!inQuote)
                {
                    inQuote = true;
                    qc = *s;
                }
                else if (qc == *s) // must match original
                    inQuote = false;
            }
            
            if (!inQuote && *s == c) return s;
            ++s;
        }
        return s;
    }

    static const char* skipToOutQuotes(const char* s, const char* sp)
    {
        // advance to a member of `sp' outside quotes or 0
        bool inQuote = false;
        char qc;
        while (*s) 
        {
            if (u_isquote(*s))
            {
                if (!inQuote)
                {
                    inQuote = true;
                    qc = *s;
                }
                else if (qc == *s) // must match original
                    inQuote = false;
            }
            
            if (!inQuote && *skipTo(sp, *s)) return s;
            ++s;
        }
        return s;
    }
    
    string makeContent(const char* s, const char* e)
    {
        static const char* transTab[] = 
        {
            "lt;", "<",
            "gt;", ">",
            "amp;", "&",
            "apos;", "'",
            "#39;", "'",
            "quot;", "\"",
            "nbsp;", " ",
            0,
        };

        string res;
        while (s < e)
        {
            const char* cp = s;
            s = skipSpace(s);

            // collapse all spaces into one
            if (s != cp) res += ' ';

            // NB: allow space on end providing < e
            if (!*s || s >= e) break;

            bool done = false;
            if (*s == '"')
            {
                cp = skipTo(s + 1, '"');
                if (!*cp || cp > e) cp = e;
                else ++cp; // include end quote in content
            }
            else if (*s == '&')
            {
                const char** tp = transTab;
                while (*tp)
                {
                    if (startsWith(s+1, *tp))
                    {
                        res += tp[1];
                        s += strlen(*tp) + 1;
                        done = true;
                        break;
                    }
                    tp += 2;
                }

                if (!done)
                {
                    // otherwise include the & as it is.
                    cp = skipWord(s);
                }
            }
            else
            {
                cp = skipWord(s);
            }

            if (!done)
            {
                if (cp > e) cp = e;
                res += string(s, cp - s);
                s = cp;
            }
        }
        return res;
    }

    var parseAttrValue(const char** sptr)
    {
        const char* s = *sptr;
        var v;

        // s points at start quote
        const char* ep = s;
        if (u_isquote(*s))
        {
            // quoted value, ends at end quote
            ep = s;

            // end at same quote as we started
            do
            {
                ++ep;
                ep = skipTo(ep, "\"'");
            } while (*ep && *ep != *s);

            // skip initial quote
            ++s;
            
        }
        else
        {
            // unquoted value ends at whitespace or quote
            ep = skipTo(s, " \t\"'");
        }

        bool done = false;
        
        // is a number?
        const char* p = s;
        if (u_isdigit(*p) || *p == '-')
        {
            ++p;
            while (p < ep)
            {
                if (!u_isdigit(*p)) break;
                ++p;
            }

            if (p == ep)
            {
                // all are digits
                //string nstr(s, p - s);
                int iv = atoi(s);
                v = var(iv);
                done = true;
            }
        }

        if (!done)
        {
            // treat as string
            v = makeContent(s, ep);
        }

        // skip ending quote
        if (u_isquote(*ep)) ++ep;
        *sptr = ep;

        return v;
    }

    NodeTag* parseNodeTag(const char** sptr)
    {
        // parse node tag
        // ASSUME sptr == '<'
        // return '>' + 1

        NodeTag* n = 0;
        int flags = 0;
        
        const char* sp = *sptr;
        const char* cp = sp;

        ++sp; // skip <

        // are we a closing tag
        if (*sp == '/') 
        {
            // closing
            flags |= flagClose;
            ++sp;
        }
        else if (*sp == '?')
        {
            // XML decl, eg <?xml version="1.0" ?>
            flags |= flagXMLDecl;
            flags |= flagEmpty;
            ++sp;
        }
        else if (*sp == '!')
        {
            flags |= flagEmpty;
            
            // include ! in name
            
            // might be a comment
            if (startsWith(sp, "!--"))
            {
                // skip prefix
                sp += 3;

                flags |= flagEmpty;
                flags |= flagComment;

                // comment. eat everything until "-->"
                cp = sp;

                for (;;)
                {
                    cp = skipToOutQuotes(cp, "->");
                    if (!*cp) break;

                    if (startsWith(cp, "-->"))
                    {
                        // end quote
                        if (_loadComments)
                        {
                            n = new NodeTag(string(sp,cp - sp));
                        }
                        else
                        {
                            // make a tag otherwise caller not know if ok
                            n = new NodeTag();
                        }

                        n->flags(flags);
                        *sptr = cp + 3;
                        return n;
                    }

                    ++cp;
                }
                
                *sptr = cp;
                return 0; // malformed comment
            }
        }
        
        sp = skipSpace(sp);
        cp = skipToOutQuotes(sp, "/>?");
        
        if (*cp == '/')
        {
            // empty tag syntax (eg <br/>
            flags |= flagEmpty;
            cp = skipSpace(cp + 1);
        }
        else if (*cp == '?')
        {
            // ending XML decl <?xml foo ?>
            cp = skipSpace(cp + 1);
        }
            
        // expect closing  otherwise, does not parse
        if (*cp == '>')
        {
            // [sp,cp] is the text inside the node brackets
            // expect the first token to be the name, followed by the
            // attributes
            const char* ep = skipIdentifier(sp);
            if (ep > sp)
            {
                // make node name from identifier
                n = new NodeTag(string(sp, ep - sp));
                n->flags(flags);
                    
                // parse any attributes in [en,cp]
                for (;;)
                {
                    sp = skipSpace(ep);
                    ep = skipIdentifier(sp);
                    if (ep > sp)
                    {
                        string ident(sp, ep - sp);
                        
                        ep = skipSpace(ep);
                        var v;
                        if (*ep == '=')
                        {
                            ep = skipSpace(ep + 1);
                            
                            // attr=value
                            v = parseAttrValue(&ep);
                        }
                            
                        // allow void value to represent an attribute
                        // without a value
                        n->_attr[ident] = v;
                            
                        sp = ep;
                    }
                    else break;
                }
            }
        }
        else
        {
            // expect >
            // skip until >
            cp = skipTo(cp, '>');
        }

        if (*cp)
            ++cp; // skip '>'
        
        *sptr = cp;
        return n;
    }

protected:

    int _lineNum(const char* s)
    {
        const char* p = _basePtr;
        int line = 1;
        while (p < s)
        {
            if (*p == '\n') ++line;
            ++p;
        }
        return line;
    }

    void _init()
    {
        _root = 0;
        _loadComments = true;
    }

    void _purge()
    {
        delete _root;
        _root = 0;
    }


    const char* _basePtr;
    NodeTag*    _root;
    bool        _loadComments;
};


