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

#include <iostream>
#include <assert.h>
#include <stdint.h>
#include "node.h"
#include "sbuf.h"
#include "cutils.h"
#include "logged.h"
#include "stream.h"

// HEADs have odd address
#define IS_HEAD(__n)  (((uintptr_t)(__n))&1)
#define GET_HEAD(_x) ((Head*)((uintptr_t)(_x) - 1))
#define SET_HEAD(_x, _h) (_x) = (Node*)((uintptr_t)(_h) + 1)

#if !defined(NDEBUG) && defined(LOGGING)        
#define DEBUGOUT
#endif

#ifdef DEBUGOUT
#define DB(_x)  { if (_showRefs) { LOG3("DB ", _x); }}
#define DBIF(_if, _x)  if (_showRefs && (_if)) DB(_x)
#else
#define DB(_x)  
#define DBIF(_if, _x)
#endif

// Forward
struct Group;
struct ParseContext;
struct Head;

struct HeadTraits
{
    // NB: numeric types must be in order or generality
    // collections are sorted in reverse order of type
    enum Type
    {
        type_null = 0,
        type_tree, // 1
        type_list, // 2
        type_symbol,  // 3
        type_int, // 4
        type_number, // 5
        type_string, // 6
        type_stream, // 7
        type_primitive, // 8 
        type_function, //9
        type_macro, // 10
        type_thunk,
        type_blob,  
        type_lref,      // local ref
        type_variable,
        type_count,
    };

    static const int ownedFlag = 1;
    static const int markFlag = 2;
};

struct Comparator
{
    virtual int operator()(const Head& a, const Head& b) = 0;
};

struct Head: public HeadTraits
{
    Node*               _n;
    ushort              _type;
    uchar               _flags;
    uchar               _fnBodyType;
    mutable uint        _refCount;

    // total number of heads
    static uint         _count;
    static int          _showRefs; // debugging

    // Constructors
    Head() { ++_count; }
    Head(int type)
    {
        _type = type;
        _flags = ownedFlag;
        _fnBodyType = 0;
        _refCount = 0;
        ++_count; 
    }

    ~Head() { --_count; }

    // Accessors
    bool owned() const { return (_flags & ownedFlag) != 0; }
    bool marked() const { return (_flags & markFlag) != 0; }

    Type type() const { return (Type)_type; }
    bool isSymbol() const { return _type == type_symbol; }
    bool isList() const { return _type == type_list; }
    bool isInt() const { return _type == type_int; }
    bool isNumber() const { return _type == type_number; }
    bool isString() const { return _type == type_string; }
    bool isTree() const { return _type == type_tree; }
    bool isStream() const { return _type == type_stream; }
    bool isPrimitive() const { return _type == type_primitive; }
    bool isFunction() const { return _type == type_function; }
    bool isMacro() const { return _type == type_macro; }
    bool isThunk() const { return _type == type_thunk; }
    bool isBlob() const { return _type == type_blob; }
    bool isNumeric() const
    { return _type == type_int || _type == type_number; }
    
    bool isAtomic() const
    {
        // a concrete type
        return _type == type_int ||
            _type == type_number ||
            _type == type_string ||
            _type == type_symbol ;
    }

    bool isEmptyList() const;

    // Modifiers
    void setOwned() { _flags |= ownedFlag; }
    
    void setMark() const { ((Head*)this)->_flags |= markFlag; }
    void clearMark() const { ((Head*)this)->_flags &= ~markFlag; }

    static int compare(const Head&, const Head&, Comparator&);
    const Head& primaryTerm() const;
    const Head& orderTerm() const;

    bool identical(const Head& h)
    {
        // same underlying object?
        return _type == h._type && _n == h._n;
    }

    void _dup(Head* h) const
    {
        h->_n = _n;
        h->_flags = _flags;
        h->_fnBodyType = _fnBodyType;
        h->_refCount = 0;
    }

    // dup for embedding
    Head* dup() const
    {
        Head* h = new Head(_type);
        _dup(h);
        return h;
    }
    
    void incRef() const { ++_refCount; }

    Node* ref(bool merge = true) const;
    Head& use() const
    {
        // eliminate pure `HeadTmp' objects.
        assert(owned() || _refCount);
        
        Head* self = const_cast<Head*>(this); // XX const
        self->_useAux(0);
        return *self;
    }

    const Head& demote() const
    {
        if (isList() && _refCount == 1)
        {
            assert(owned());
            
            // kill the reference count so that the caller can take
            //  a new REF which will either be DUP or MERGE
            _refCount = 0;

            // the dup or merge will consume the object, so that the
            // head here will be invalid
            // NB: this TERM should not be used subsequently to demotion
            // NB: HEAD is deleted when ref = 0
        }
        return *this;
    }

    const Head& prepuse() const;
    
    void drop()
    {
        // if not referenced, a free object.
        if (!_refCount || !--_refCount) 
                _destroy(_type);
    }

    // stream to sbuf
    void toSBuf(SBuf&) const;

    std::string toString() const
    {
        SBuf sb;
        toSBuf(sb);
        return (const char*)sb;
    }
    
    // pack into buffer
    void fold(const Group&, SBuf&) const;

    // unpack from buffer
    void unfold(Group&, SBuf&);

    static const char* typeName(uint type)
    {
        static const char* types[] =
        {
            "null",
            "tree",
            "list",
            "symbol",
            "int",
            "number",
            "string",
            "stream",
            "primitive",
            "function",
            "macro",
            "thunk",
            "blob",
            "lref",
            "variable",
            "unknown",
        };

        if (type > type_count) type = type_count;
        return types[type];
    }
        
        
protected:

    void _useAux(int);
    
    Node* _donate() const
    {
        Node* n = _n;
        const_cast<Head*>(this)->_n = 0; 
        return n;
    }

    static int _compare(const Head&, const Head&, Comparator&);

private:

    // release memory
    void _destroy(int type);
    void _toSBuf(int type, SBuf&) const;
};

struct HeadTmp: public Head
{
    HeadTmp() {}
    HeadTmp(int type) : Head(type) {}
    
    Node**      _nAddr;

    HeadTmp* dup() const
    {
        HeadTmp* h = new HeadTmp(_type);
        _dup(h);
        h->_nAddr = _nAddr;
        return h;
    }

    void use()
    {
        assert(_nAddr);

#ifdef DEBUGOUT
        if (_showRefs)
        {
            LOG3("KL promoting ref ", toString());
        }
#endif        

        // wire the originator (a list host) to the temp head
        // as a fully referenced head object.
        incRef();
        
        // owned by the original container
        setOwned();

        SET_HEAD(*_nAddr, this);
    }
};

inline Node* Head::ref(bool merge) const
{
    Node* n;
    if (!_refCount)
    {
        // not already used
        if (!owned())
        {
            // we are a list
            // we are, in fact, a HeadTmp
            // make a dup ref that remains non-owned [ref=1]
            // NB: assume that the referee remains valid for this lifetime

            Head* h = ((HeadTmp*)this)->dup();
            h->incRef(); // ref = 1
            SET_HEAD(n, h);
        }
        else
        {
            // owned && not ref => steal object
            if (merge && _type == type_list)
            {
                // lists do not need a HEAD.
                n = _n;
            }
            else
            {
                Head* h = dup();
                h->incRef();  // ref=1
                SET_HEAD(n, h);
            }
                
            // consume original content
            _donate();
        }
    }
    else
    {
        incRef();
        SET_HEAD(n, this);
    }
    return n;
}

inline const Head& Head::prepuse() const
{
    const Head* hu = this;

    // original code used to demote here for `owned`
    // this was wrong.
    
    if (!owned())
    {
        if (_refCount > 0)
        {
            // not owned & ref => headtmp
            hu = &use();
        }
        else
        {
            // not owned, ref == 0
            // pure headtmp, inside iterator
                
            // here we do the same as head.ref() without inc
            // `use` will inc ref=1 to indicate reference from originator
            HeadTmp* ht = ((HeadTmp*)this)->dup();
            ht->use();
            hu = ht;
        }
    }
    return *hu;
}

static inline void emitHead(std::ostream& os, const Head& h, int limit = 0)
{
    SBuf sb;
    sb.setMaxSize(limit);
    h.toSBuf(sb);
    
    os << (const char*)sb;
    
    // if we truncated, indicate we did so.
    if (sb.truncated()) os << "...";
}

inline std::ostream& operator<<(std::ostream& os, const Head& h)
{
    int lim = 0;

#ifdef DEBUGOUT
    if (Logged::_logLevel > 1)
    {
        // limit size of output when logging and debug mode
        lim = 1024;
    }
#endif

    emitHead(os, h, lim);
    return os;
}

struct ComparatorEq: public Comparator
{
    // compliance
    int operator()(const Head& a, const Head& b)
    { return Head::compare(a, b, *this); }
};

