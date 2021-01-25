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

#include "pbase.h"

namespace ST
{
    typedef std::string string;
    
    struct enode: public node<enode>
    {
        enum nodeType
        {
            e_void = 0,
            e_name,
            e_and,
            e_or,
        };

        string  _name;
        Term*   _binding = 0;
        bool    _neg = false;

        enode(int t) : node(t) {}
        enode(enode* l, int t) : node(l, t) {}
        enode(const string& name) : node(e_name), _name(name) {}

        bool isTermName() const { return _type == e_name; }

        string _toStringList(enode* e, const char* op)
        {
            string s;
            do
            {
                s += e->toString();
                e = e->_next;
                if (e) s += op;
            } while (e);
            return s;
        }

        string toString()
        {
            string s; 
            if (_neg) s = "!";

            if (_type == e_name)
            {
                assert(!_head);
                s += _name;
            }
            else
            {
                s += '(';
                switch (_type)
                {
                case e_and:
                    assert(_head);
                    s += _toStringList(_head, " and ");
                    break;
                case e_or:
                    assert(_head);
                    s += _toStringList(_head, " or ");
                    break;
                }
                s += ')';
            }
            
            return s;
        }
        
    };

    struct ParseExpr: public ParseBase
    {
        typedef enode::nodeType nodeType;

        bool    _err = false;
        
        string parseName()
        {
            // `[A-Z][A-Z0-9-_]+`

            int l = atName(POS);
            string s;
            if (l)
            {
                s = string(POS, l);
                SETPOS(POS + l); 
            }
            return s;
        }
        
        enode* parseAtom()
        {
            enode* e = 0;
            
            skipws();
            string n = parseName();
            if (n.size()) e = new enode(n);
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
            else
            {
                e = parseAtom();
            }

            if (e) e->_neg = b;
            return e;
        }

        enode* parseAnd()
        {
            // (e_and (A B)) or A
            
            enode* a = parseFactor();
            if (a)
            {
                enode* el = 0;
                
                while (!_err)
                {
                    skipws();
                    if (parseLiteral("and ") || parseLiteral("&&"))
                    {
                        enode* b = parseFactor();
                        if (b)
                        {
                            if (!el)
                            {
                                el = a;
                                a = new enode(el, nodeType::e_and);
                            }
                            
                            el->_next = b;
                            el = b;
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
                            if (!el)
                            {
                                el = a;
                                a = new enode(el, nodeType::e_or);
                            }
                            
                            el->_next = b;
                            el = b;
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

        enode* parseExpr()
        {
            return parseOr();
        }
        
        enode* parse(const char* s, int line = 0)
        {
            SETPOS(s);
            lineno = line;

            enode* en = parseExpr();
            
            if (!en && line)
            {
                PERR0("expression syntax error");
            }

            return en;
        }
    };
    
}; // ST

