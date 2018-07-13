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

template<class T> struct AP
{
    AP() { _p = 0; }
    AP(T* p) : _p(p) {}
    AP(const AP& ap) { _p = 0; *this = ap; }

    ~AP() { delete _p; }
    
    void operator=(const AP& ap)
    {
        // donate semantics
        delete _p;
        _p = ap.donate();
    }

    operator bool() const { return _p != 0; }

    T*          operator->() const { return _p; }

    T*          donate() const
    {
        T* p = _p;
        const_cast<AP*>(this)->_p = 0;
        return p;
    }

    const T&    operator*() const { return *_p; }
    T&          operator*() { return *_p; }

    T*          get() const { return _p; }

protected:

    T*          _p;
};

template<class T> struct OAP: public AP<T>
{
    OAP() {}
    OAP(T* t) : AP<T>(t) {}

    OAP(const OAP<T>& p)
    { AP<T>::operator=(static_cast<const AP<T>&>(p)); }

    friend bool operator==(const OAP<T>& a, const OAP<T>& b)
    {
        return a._p == b._p || (a._p && b._p && *a._p == *b._p);
    }

    friend bool operator!=(const OAP<T>& a, const OAP<T>& b)
    { return !(a == b); }

    friend bool operator<(const OAP<T>& a, const OAP<T>& b)
    {
        if (a._p) 
        {
            if (!b._p) return false;
            return *a._p < *b._p;
        }
        return b._p != 0; // null < x
    }

    friend bool operator>(const OAP<T>& a, const OAP<T>& b)
    { return b < a; }

    friend bool operator<=(const OAP<T>& a, const OAP<T>& b)
    { return !(b < a); }

    friend bool operator>=(const OAP<T>& a, const OAP<T>& b)
    { return !(a < b); }
};

