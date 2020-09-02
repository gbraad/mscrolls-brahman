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

#include "head.h"

// forward
struct List;
struct Tree;
struct Int;
struct Number;
struct Symbol;
struct Primitive;
struct Thunk;
struct Stringt;
struct Streamt;
struct Blob;

struct Term: public HeadTraits
{
    // A term is a head object that's referenced.
    // A term is not a temporary
    
    Head*       _head;

    Term() { _head = 0; }
    Term(const Head& h) { _head = GET_HEAD(h.ref(false)); }
    Term(const Term& t) {_head = 0;  *this = t; }

    Term& operator=(const Term& t)
    {
        if (t._head) t._head->ref(false);
        drop();
        _head = t._head;
        return *this;
    }

    ~Term() { drop(); }

    int type() const { return _head ? _head->_type : type_null; }
    operator bool() const { return _head != 0; }

    bool isSymbol() const { return _head && _head->_type == type_symbol; }
    bool isList() const { return _head && _head->_type == type_list; }
    bool isInt() const { return _head && _head->_type == type_int; }
    bool isString() const { return _head && _head->_type == type_string; }
    bool isTree() const { return _head && _head->_type == type_tree; }
    bool isThunk() const { return _head && _head->_type == type_thunk; }
    bool isPrimitive() const { return _head && _head->_type == type_primitive; }
    bool isFunction() const { return _head && _head->_type == type_function; }
    bool isMacro() const { return _head && _head->_type == type_macro; }
    bool isStream() const { return _head && _head->_type == type_stream; }
    bool isAtomic() const { return _head && _head->isAtomic(); }
    bool isBlob() const { return _head && _head->isBlob(); }

    Head* operator->() const
    {
        assert(_head);
        return _head;
    }
    
    Head& operator*() const
    {
        assert(_head);
        return *_head;
    }

    bool operator==(const Head& h) const
    {
        ComparatorEq co;
        return _head && !co(*_head, h);
    }

    List& asList() const {return (List&)*_head; }
    Tree& asTree() const { return (Tree&)*_head; }
    Int& asInt() const { return (Int&)*_head; }
    Number& asNumber() const { return (Number&)*_head; }
    Symbol& asSymbol() const { return (Symbol&)*_head; }
    Primitive& asPrimitive() const { return (Primitive&)*_head; }
    Streamt& asStream() const { return (Streamt&)*_head; }
    Thunk& asThunk() const { return (Thunk&)*_head; }
    Stringt& asString() const { return (Stringt&)*_head; }
    Blob& asBlob() const { return (Blob&)*_head; }

    void drop()
    {
        if (_head)
        {
            _head->drop();
            _head = 0;
        }
    }

    const Head& demote() const  { return _head->demote(); }

    std::string toString() const
    {
        return _head ? _head->toString() : "null";
    }

    std::string toStringRaw() const
    {
        // do not put quotes around strings and do not return "null"
        std::string s;
        if (_head)
        {
            SBuf sb;
            sb._mode = SBuf::mode_noquotes; 
            _head->toSBuf(sb);
            s = std::string(sb, sb.size());
        }
        return s;
    }

private:
    
    Term(int) {}
};

inline std::ostream& operator<<(std::ostream& os, const Term& t)
{
    if (t._head) 
        os << *t._head;
    else 
        os << "NULL";
    return os;
}

extern Term parseTerm(Stream& s, ParseContext* ctx = 0);

