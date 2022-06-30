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
 *  Copyright (c) Strand Games 2021.
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

#include <functional>
#include "pbase.h"

namespace ST
{
    typedef std::string string;
    
    struct enode: public node<enode>
    {
        static const int nodeBase = 100;
        enum nodeType
        {
            e_void = nodeBase, // so that types dont clash with pnode
            e_name,
            e_val,
            e_and,
            e_or,
            e_mul,
            e_div,
            e_add,
            e_sub,
            e_neg,
            e_ge,
            e_le,
            e_gt,
            e_lt,
            e_eq,
            e_neq,
            e_simplename,  // property or function
            e_function,
            e_typeCount,
        };
        
        var     _v;
        Term*   _binding = 0;
        bool    _neg = false;

        enode(int t) : node(t) {}
        enode(enode* l, int t) : node(l, t) {}
        enode(const string& name) : node(e_name), _v(name) {}
        enode(var& v) : node(e_val), _v(v) {} // consume v

        ~enode() override
        {
            //LOG1("~enode ", *this);
            _purge(); 
        }

        bool valid() const { return _type >= nodeBase && _type < e_typeCount; }
        bool isTermName() const { return _type == e_name; }

        static const char* typeSymbol(uint t)
        {
            t -= nodeBase;
            assert(t >= 0 && t < e_typeCount);
            static const char* sTab[] =
            {
                "void",
                "name",
                "val",
                "and",
                "or",
                "*",
                "/",
                "+",
                "-",
                ">=",
                "<=",
                ">",
                "<",
                "==",
                "!=",
                "name"
                "fn",
            };
            assert(t < ASIZE(sTab));
            return sTab[t];
        }

        string _toStringList(enode* e, const char* op) const
        {
            string s;
            do
            {
                s += e->toString();
                e = e->_next;
                if (e)
                {
                    s += ' ';
                    s += op;
                    s += ' ';
                }
            } while (e);
            return s;
        }

        const char* name() const
        {
            assert(isTermName());
            return _v.rawString();
        }

        string toString() const
        {
            assert(_type);
            
            string s; 
            if (_neg) s = "!";
            if (_type == e_name || _type == e_simplename)
            {
                assert(!_head);
                assert(_v.isString());
                s += _v.rawString();
            }
            else if (_type == e_val)
            {
                assert(!_head);
                var::Format f;
                f._quoteStrings = true;
                s += _v.toString(&f); // quote strings
            }
            else if (_type == e_function)
            {
                assert(_head);
                s += _head->toString();
                s += '(';

                enode* e = _head->_next;
                while (e)
                {
                    s += e->toString();
                    e = e->_next;
                    if (e) s += ',';
                }
                s += ')';
            }
            else
            {
                s += '(';
                assert(_head);
                s += _toStringList(_head, typeSymbol(_type));
                s += ')';
            }
            
            return s;
        }

        friend std::ostream& operator<<(std::ostream& os, const enode& e)
        {return os << e.toString(); }

    protected:
    
        void _purge() 
        {
            delete _head; _head = 0;
            delete _next; _next = 0;
        }
        
    };

    struct ParseExpr: public ParseBase
    {
        typedef enode::nodeType nodeType;

        bool    _err = false;
        int     _fndepth = 0;  // nesting of function calls
        bool    _allowSimpleNames = false;
        
        int stringWalk(const char* p)
        {
            // return the number of bytes INCLUDING THE TERMINATOR
            bool esc = 0;
            const char* p0 = p;
            
            assert(p);
    
            while (*p)
            {
                char c = *p++;
                if (!esc)
                {
                    if (c == '"') break;
                    if (c == '\n')
                    {
                        PERR1("expr, unterminated string, at ", p0);
                        _err = true;
                        return 0;
                    }
                    if (c == '\\') esc = 1;
                }
                else esc = 0;
            }
            return p - p0;
        }

        int stringSize(const char* p) { return stringWalk(p); }

        int atFunction()
        {
            // parse NAME(, return length of name if function,
            // zero otherwise
            
            int l = atSimpleName(POS);
            if (l > 0)
            {
                if (POS[l] != '(') l = 0;
            }
            return l;
        }
        
        bool parseString(var& v)
        {
            bool r = (AT == '"');
            if (r)
            {
                BUMP;

                int l = stringSize(POS); // includes terminator
                r = l > 0;

                if (r)
                {
                    v = var(POS, l-1); // hand over
                
                    // skip length of string + 1 for terminator
                    SETPOS(POS + l);
                }
            }
            return r;
        }

        enode* parseTermName()
        {
            // `[A-Z][A-Z0-9-_]+`
            enode* e = 0;
            int l = atName(POS);

            if (l > 0)
            {
                e = new enode(string(POS, l));  // makes e_name
                SETPOS(POS + l);
            }
            return e;
        }

        enode* parseSimpleName()
        {
            // when we just have a lower case string
            enode* e = 0;
            int l = atSimpleName(POS);
            if (l > 0)
            {
                // this could be a property name for example
                // we package this as a string
                var v(string(POS, l));
                SETPOS(POS + l);
                e = new enode(v);
            }
            return e;
        }

        bool expectComma()
        {
            if (_err) return false;
    
            skipws();
            if (AT == ',')
            {
                BUMP;
            }
            else
            {
                PERR1("expr, expected comma at ", POS);
                _err = true;
            }
            return !_err;
        }

        int parseArgs(enode** args)
        {
            int n = 0;
            for (;;)
            {
                skipws();
                if (AT == ')')
                {
                    BUMP;
                    break;
                }

                if (n)
                {
                    if (!expectComma()) break;
                }
            
                *args = parseExpr();
                if (!*args || _err) break;

                args = &(*args)->_next;
                assert(!*args);
                
                // number of valid args
                ++n;
            }
            return n;
        }

        enode* parseFunction()
        {
            enode* e = 0;
            int l = atFunction();
            if (l > 0)
            {
                // is this a term function like PLAYER(SCORE)
                // or something like sin(x).

                enode* fn = 0;
                string fname = string(POS, l);
                
                if (atName(POS))
                {
                    fn = new enode(fname);
                }
                else
                {
                    fn = new enode(enode::e_simplename);
                    fn->_v = fname;                    
                }
                
                SETPOS(POS + l + 1); // skip name + "("

                // (fn (name args...))
                e = new enode(fn, enode::e_function);
                ++_fndepth;
                parseArgs(&fn->_next);
                --_fndepth;
            }
            return e;
        }
        
        enode* parseAtom()
        {
            skipws();

            enode* e = parseFunction();
            if (!e) e = parseTermName();
            if (!e)
            {
                var v;
                if (parseString(v) || parseNumber(v))
                {
                    e = new enode(v); // v consumed
                }
                else if (_allowSimpleNames || _fndepth > 0)
                {
                    // when inside function calls, look for simple
                    // names and encode them as strings
                    e = parseSimpleName();
                }
            }
            return e;
        }

        enode* parseFactor()
        {
            // (A) or A

            bool b = parseLiteral("not ") || parseLiteral("!");
            enode* e;
            if (AT == '(')
            {
                BUMP;
                e = parseExpr();
                if (e)
                {
                    skipws();
                    if (AT == ')') BUMP;
                    else
                    {
                        PERR1("expr, expected ')' at ", POS);
                        _err = true;
                    }
                }
            }
            else if (AT == '-')
            {
                BUMP;
                e = parseFactor();
                if (e)
                {
                    if (e->_type == nodeType::e_val)
                    {
                        // can just negate value
                        e->_v = e->_v.neg();
                    }
                    else
                    {
                        e = new enode(e, enode::e_neg);
                    }
                }
            }
            else
            {
                e = parseAtom();
            }

            if (e) e->_neg = b;
            return e;
        }

#define CHAIN(_el, _a, _b, _t)                  \
{                                               \
    if (!_el)                                   \
    {                                           \
        _el = _a;                               \
        _a = new enode(_el, _t);                \
    }                                           \
    _el->_next = _b;                            \
    _el = _b;                                   \
}

        typedef enode* (ParseExpr::*TermFn)();

        enode* parseBinTerm(const char* op, nodeType t, TermFn fn)
        {
            // A or (op A B...)
            // ASSUME `op` is 1 or 2 characters only
            
            enode* a = (this->*fn)();
            if (a)
            {
                enode* el = 0;
                bool twochars = op[1] != 0;
                
                while (!_err)
                {
                    skipws();

                    bool match;
                    if (twochars)
                    {
                        match = AT == *op && AT1 == op[1];
                    }
                    else
                    {
                        match = AT == *op;
                    }

                    if (match)
                    {
                        BUMP;
                        if (twochars) BUMP;
                        skipws();
                        enode* b = (this->*fn)();
                        if (b)
                        {
                            CHAIN(el, a, b, t);
                        }
                        else
                        {
                            PERR1("expr, expected expression after ", POS);
                            _err = true;
                        }
                    }
                    else break;
                }
            }
            return a;
        }        
        
        enode* parseMulTerm()
        { return parseBinTerm("*", nodeType::e_mul, &ParseExpr::parseFactor); }

        enode* parseDivTerm()
        { return parseBinTerm("/", nodeType::e_div, &ParseExpr::parseMulTerm); }

        enode* parseAddTerm()
        { return parseBinTerm("+", nodeType::e_add, &ParseExpr::parseDivTerm); }

        enode* parseSubTerm()
        { return parseBinTerm("-", nodeType::e_sub, &ParseExpr::parseAddTerm); }
        
        enode* parseAnd()
        {
            // (e_and (A B)) or A
            enode* a = parseSubTerm();
            if (a)
            {
                enode* el = 0;
                
                while (!_err)
                {
                    skipws();
                    if (parseLiteral("and ") || parseLiteral("&&"))
                    {
                        enode* b = parseSubTerm();
                        if (b)
                        {
                            CHAIN(el, a, b, nodeType::e_and);
                        }
                        else
                        {
                            PERR1("expr, expected expression after AND ", POS);
                            _err = true;
                        }
                    }
                    else break;
                }
            }

            return a;
        }

        enode* parseOr()
        {
            // (e_or (A B)) or A
            enode* a = parseAnd();
            if (a)
            {
                enode* el = 0;
                
                while (!_err)
                {
                    skipws();
                    if (parseLiteral("or ") || parseLiteral("||"))
                    {
                        enode* b = parseAnd();
                        if (b)
                        {
                            CHAIN(el, a, b, nodeType::e_or);
                        }
                        else
                        {
                            PERR1("expr, expected expression after OR ", POS);
                            _err = true;
                        }
                    }
                    else break;
                }
            }

            return a;
        }

        enode* parseGE()
        { return parseBinTerm(">=", nodeType::e_ge, &ParseExpr::parseOr); }

        enode* parseLE()
        { return parseBinTerm("<=", nodeType::e_le, &ParseExpr::parseGE); }

        enode* parseGT()
        { return parseBinTerm(">", nodeType::e_gt, &ParseExpr::parseLE); }

        enode* parseLT()
        { return parseBinTerm("<", nodeType::e_lt, &ParseExpr::parseGT); }

        enode* parseNEqu()
        { return parseBinTerm("!=", nodeType::e_neq, &ParseExpr::parseLT); }

        enode* parseEqu()
        { return parseBinTerm("=", nodeType::e_eq, &ParseExpr::parseNEqu); }
        
        enode* parseExpr()
        {
            return parseEqu();
        }
        
        enode* parse(const char* s, int line = 0)
        {
            setup(s, line);
            enode* en = parseExpr();
            
            if (!en && line)
            {
                PERR0("expression syntax error");
            }

            return en;
        }
    };

    struct eNodeCtx
    {
        typedef std::function<var(eNodeCtx*, const char*)> termEval;
        typedef std::function<var(eNodeCtx*, enode*)> functionEval;
        typedef var binOp(const var& a, const var& b);
    
        static var _mulOp(const var& a, const var& b) { return a*b; }
        static var _divOp(const var& a, const var& b) { return a/b; }
        static var _addOp(const var& a, const var& b) { return a+b; }
        static var _subOp(const var& a, const var& b) { return a-b; }

        static var _geOp(const var& a, const var& b) { return a>=b; }
        static var _leOp(const var& a, const var& b) { return a<=b; }
        static var _gtOp(const var& a, const var& b) { return a>b; }
        static var _ltOp(const var& a, const var& b) { return a<b; }

        static var _eqOp(const var& a, const var& b)
        {
            return a.equ(b) ? 1 : 0;
        }
        
        static var _neqOp(const var& a, const var& b)
        {
            return a.equ(b) ? 0 : 1;
        }

        enode*          _top;
        termEval        _termFn;
        functionEval    _fnFn;

        eNodeCtx(enode* e) : _top(e) {}

        var _evalBinOp(enode* e, binOp op)
        {
            assert(e);
            var v = eval(e);
            while ((e = e->_next) != 0) v = op(v, eval(e));
            return v;
        }

        var eval(enode* e)
        {
            var v;
            assert(e);
            switch (e->_type)
            {
            case enode::e_name:
                assert(_termFn);
                v = _termFn(this, e->name());
                break;
            case enode::e_val:
                v = e->_v.copy();
                break;
            case enode::e_and:
                {
                    enode* ei = e->_head;
                    while (ei)
                    {
                        v = eval(ei);
                        if (!v.isTrue()) break;
                        ei = ei->_next;
                    }
                }
                break;
            case enode::e_or:
                {
                    enode* ei = e->_head;
                    while (ei)
                    {
                        v = eval(ei);
                        if (v.isTrue()) break;
                        ei = ei->_next;
                    }
                }
                break;
            case enode::e_mul:
                v = _evalBinOp(e->_head, _mulOp);
                break;
            case enode::e_div:
                v = _evalBinOp(e->_head, _divOp);
                break;
            case enode::e_add:
                v = _evalBinOp(e->_head, _addOp);
                break;
            case enode::e_sub:
                v = _evalBinOp(e->_head, _subOp);
                break;
            case enode::e_ge:
                v = _evalBinOp(e->_head, _geOp);
                break;
            case enode::e_le:
                v = _evalBinOp(e->_head, _leOp);
                break;
            case enode::e_gt:
                v = _evalBinOp(e->_head, _gtOp);
                break;
            case enode::e_lt:
                v = _evalBinOp(e->_head, _ltOp);
                break;
            case enode::e_eq:
                v = _evalBinOp(e->_head, _eqOp);
                break;
            case enode::e_neq:
                v = _evalBinOp(e->_head, _neqOp);
                break;                
            case enode::e_neg:
                assert(e->_head);
                v = eval(e->_head).neg();
                break;
            case enode::e_function:
                assert(e->_head);
                if (e->_head->_type == enode::e_simplename)
                {
                    // not a term property function
                    // eg sin(1)
                    // XX TODO
                    v = 0;
                }
                else
                {
                    assert(_fnFn);
                    assert(e->_head->_type == enode::e_name);
                    v = _fnFn(this, e->_head);
                }
                break;
            default:
                LOG1("enodeCtx, unhandled operator ", e->_type);
            }

            if (e->_neg)
            {
                // invert
                if (v.isTrue()) v = 0;
                else v = 1;
            }

            return v;
        }
    };
     
    
}; // ST

