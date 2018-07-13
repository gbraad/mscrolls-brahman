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

/// intrusive Dlist
template<class T> class DL
{
public:

    struct iterator
    {
        iterator() {}
        iterator(const DL* pos) : _pos(pos) {}

        /// get object
        T& operator*() const { return *(T*)_pos; }
        T* operator->() const { return (T*)_pos; }

        operator const T*() const { return (const T*)_pos; }
        operator T*() { return (T*)_pos; }
        
        /// comparison
        bool operator==(const iterator& it) const { return _pos == it._pos; }
        bool operator!=(const iterator& it) const { return _pos != it._pos; }
        
        /// bump
        iterator& operator++() { _pos=_pos->_next; return *this; }

        iterator operator++(int)
        {
            iterator i(_pos);
            return ++i;
        }

        iterator& operator--() { _pos = _pos->_prev; return *this; }

        DL* remove()
        {
            // ASSUME valid and not at end()
            // removes element at current position from list
            // and moves iterator to the next element (which could be end())
            DL* p = const_cast<DL*>(_pos); 
            _pos = p->_next;
            p->remove();
            return (T*)p;
        }

    private:
        const DL*     _pos;
    };

    struct List
    {
        typedef typename DL::iterator iterator;
        typedef T value_type;

        /// iteration
        iterator   begin() const { return _head._next; }
        iterator   end() const { return &_head; }

        /// are we empty
        bool empty() const { return _head._next == &_head; }

        // ASSUME not empty
        T* first() const { return (T*)_head._next; }
        T* last() const { return (T*)_head._prev; }

        /// add to end of list
        void        add(DL* r) { r->insert(&_head); }
        
        /// long-winded size measure.
        unsigned int size() const
        {
            unsigned int n = 0;
            for (iterator i = begin(); i != end(); ++i) ++n;
            return n;
        }

        void clear() { _head._init(); }

        List& operator=(const List& l)
        {
            // donate semantics
            clear();
            if (!l.empty())
            {
                _head = l._head;
                first()->_prev = &_head;
                last()->_next = &_head;

                // disconnect other list
                const_cast<List&>(l).clear();
            }
            return *this;
        }

    protected:
        DL    _head;
    };

    // list of owned DLs
    struct ListO: public List
    {
        ~ListO() { clear(); }

        ListO& operator=(const ListO& l)
        {
            clear();
            List::operator=(l);
            return *this;
        }

        void clear()
        {
            while (!this->empty()) this->first()->removeAndDelete();
        }
    };

    DL() { _init(); }

    ~DL() { remove(); }

    /// insert in front of `r'
    void        insert(DL* r)
    { _next = r; _prev = r->_prev; _prev->_next = this; r->_prev = this; }

    /// remove from list
    void        remove() 
    { _prev->_next = _next; _next->_prev = _prev; _init(); }

    void        removeAndDelete()
    {
        _prev->_next = _next;
        _next->_prev = _prev;
        delete this;
    }

private:
    
    void _init() { _next = this; _prev = this; }

    DL*          _next;
    DL*          _prev;

};

