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

template<class T> class Arithmetic
{
    // eg class Number: public Arithmetic<Number>
    // T is derived class.
    // N is type of underlying number format on which arithmetic is already
    // defined (eg a base type).
    // 
    // compliance:
    // const N& T::get() const;  // convert T to N.
    // N& T::set();  // N l-value in T
    
public:

    /*
    friend T operator+(const T& a, const T& b)
    {
        T c;
        c.set(a.get() + b.get());
        return c;
    }

    friend T operator-(const T& a, const T& b)
    {
        T c;
        c.set(a.get() - b.get());
        return c;
    }

    friend T operator*(const T& a, const T& b)
    {
        T c;
        c.set(a.get() * b.get());
        return c;
    }

    friend T operator/(const T& a, const T& b)
    {
        T c;
        c.set(a.get() / b.get());
        return c;
    }
    */

    void operator+=(const T& b)
    {
        T* me = (T*)this;
        me->set(me->get() + b.get());
    }

    void operator-=(const T& b)
    {
        T* me = (T*)this;
        me->set(me->get() - b.get());
    }

    void operator*=(const T& b)
    {
        T* me = (T*)this;
        me->set(me->get() * b.get());
    }

    void operator/=(const T& b)
    {
        T* me = (T*)this;
        me->set(me->get() / b.get());
    }
    

    /*
    void operator+=(const T& b) { ((T*)this)->set() += b.get(); }
    void operator-=(const T& b) { ((T*)this)->set() -= b.get(); }
    void operator*=(const T& b) { ((T*)this)->set() *= b.get(); }
    void operator/=(const T& b) { ((T*)this)->set() /= b.get(); }
    */
};

