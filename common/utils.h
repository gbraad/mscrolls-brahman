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

#include <vector>
#include <list>
#include <algorithm>
#include <string>

#ifdef ANDROID
#include <sstream>

namespace std
{
    // missing function for android
template <typename T> string to_string(T value)
{
    ostringstream os;
    os << value;
    return os.str();
}

}; // std

#endif // android

template<typename T> inline void purge(std::vector<T*>& l)
{
    size_t n = l.size();
    for (size_t i = 0; i < n; ++i) delete l[i];
    l.clear();
}

template<typename T> inline void purge(std::list<T*>& l)
{
    for (typename std::list<T*>::iterator it = l.begin(); it != l.end(); ++it)
        delete *it;
    l.clear();
}

template<typename T> inline
bool contains(const std::vector<T>& l, const T& v)
{
    return std::find(l.cbegin(), l.cend(), v) != l.cend();
}

template<typename T> inline
bool contains(const std::list<T>& l, const T& v)
{
    return std::find(l.cbegin(), l.cend(), v) != l.cend();
}

template<typename T> inline bool erase(std::vector<T*>& l, T* v)
{
    for (typename std::vector<T*>::iterator it = l.begin(); it != l.end(); ++it)
    {
        if (*it == v)
        {
            l.erase(it);
            return true;
        }
    }
    return false;
}

template<typename T> inline bool erase(std::vector<T>& l, const T& v)
{
    typename std::vector<T>::iterator it = l.begin();
    while (it != l.end())
    {
        if (*it == v)
        {
            l.erase(it);
            return true;
        }
        ++it;
    }
    return false;
}

template<typename T> struct ListOf: public std::vector<T>
{
    std::string toString() const
    {
        std::string s;
        for (size_t i = 0; i < this->size(); ++i)
        {
            if (i) s += ' ';
            //s += std::to_string(this->at(i));
            s += this->at(i);
        }            
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const ListOf& p)
    {
        return os << p.toString();
    }
};

struct Strings: public std::vector<std::string>
{
    std::string toString() const
    {
        std::string s;
        for (size_t i = 0; i < this->size(); ++i)
        {
            if (i) s += ' ';
            s += this->at(i);
        }            
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Strings& p)
    {
        return os << p.toString();
    }
};



