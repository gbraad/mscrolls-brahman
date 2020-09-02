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

#include <cmath>
#include <limits>
#include "term.h"
#include "arith.h"

// uncomment to use DPD rather than machine float
//#define USE_DPD

#ifdef USE_DPD
#include "dpd.h"
#endif

struct NumberBase: public Head
{
    // use the node pointer in the head to store a number

    NumberBase() : Head(type_number) { _n = 0; }
    
    static bool parseToBuf(Stream& s, SBuf& sb)
    {
        // -?[.0-9]+(:[0-9]+)?([eE]-?[0-9]+)?

        bool point = false;
        bool exp = false;
        bool allowneg = true;
        bool range = false;

        int md = 0;
        int ed = 0;
        bool ok;
        
        do
        {
            ok = u_isdigit(*s);
            if (ok)
            {
                if (exp) ++ed;  // count mantissa and exp digits
                else ++md;
                
                allowneg = false;
            }
            else
            {
                ok = (*s == '-' && allowneg);
                if (ok)
                    allowneg = false;
                else
                {
                    ok = (*s == '.' && !point && !exp);
                    if (ok)
                    {
                        ++md;
                        point = true;
                    }
                    else
                    {
                        ok = ((*s == 'e' || *s == 'E') && !exp && md > 0);
                        if (ok) 
                        {
                            exp = true;
                            allowneg = true;
                        }
                        else
                        {
                            ok = (*s == ':' && !range && !exp && md > 0);
                            if (ok)
                            {
                                range = true;
                            }
                        }
                    }
                }
            }
            
            if (ok)
            {
                sb.add(*s);
                ++s;
            }
            
        } while (ok);

        // have mantissa digits, not followed by alpha
        ok = sb.size() && md > 0 && (!exp || ed > 0) && !u_isalpha(*s);
        return ok;
    }
};

#ifdef USE_DPD
struct Number: public NumberBase, public Arithmetic<Number>
{
    // use the node pointer in the head to store a number
    
    Number() {}
    Number(int n) 
    {
        DPDFloat v(n);
        _n = (Node*)v._donate();
    }

    ~Number() { _purge(); } 

    Number& operator=(const Number& n)
    {
        _purge();
        DPD v;
        v = n.get();
        _n = (Node*)((DPDFloat*)&v)->_donate();
        return *this;
    }

    DPD&        set() { return *(DPD*)&_n; }
    const DPD&  get() const { return *(DPD*)&_n; }
    DPDFloat&   dpdf() const { return *(DPDFloat*)&_n; }


    // convert to sbuf
    void toSBuf(SBuf& sb) const
    {
        sb.add(get().asString());
    }
    
    int compare(const Number& v) const
    {
        return 0;
    }
    
    void fold(SBuf& sb) const
    {
        //sb.writeInt32((unsigned int)_n); 
    }

    void unfold(SBuf& sb)
    {
    }

    static Term parse(Stream& s)
    {
        // -?[.0-9]+(:[0-9]+)?([eE]-?[0-9]+)?

        SBuf sb;
        
        if (parseToBuf(s, sb))
        {
            Number n;
            DPDFloat v(sb);
            n._n = (Node*)v._donate();
            return n;
        }
        else
        {
            // if we fail, must wind back all characters in buffer
            // XX need better stream reposition here.
            int cc = sb.size();
            while (cc)
            {
                --cc;
                --s;
            }
        }
        return Term();
    }

    static Number nan() 
    {
        Number v;
        v.set() = DPD::nan();
        return v;
    }

    static Number posInf()
    {
        Number v;
        v.set() = DPD::posInf();
        return v;
    }

    static Number negInf()
    {
        Number v;
        v.set() = DPD::negInf();
        return v;
    }

private:

    void _purge() 
    {
        DBIF(_n, "~" << *this);
        delete [] dpdf()._donate();
    }
};
#else // USE_DPD


struct Number: public NumberBase, public Arithmetic<Number>
{
    // store a float in the pointer

    typedef float valueT;

    Number() {}
    Number(long n) { set((valueT)n); }
    Number(valueT v) { set(v); }

    Number& operator=(const Number& n)
    {
        set(n.get());
        return *this;
    }

    // Compliance, Arithmetic
    valueT get() const
    {
        //return *(const valueT*)&_n;
        void* p = (void*)&_n;
        return *(valueT*)p;
    }
    
    void set(const valueT& v)
    {
        void* p = (void*)&_n;
        *(valueT*)p = v;
        //*(valueT*)&_n = v;
    }

    // convert to sbuf
    void toSBuf(SBuf& sb) const
    {
        sb.addFloatString(get());
    }
    
    int compare(const Number& v) const
    {
        valueT f = get() - v.get();
        if (f < 0) return -1;
        if (f > 0) return 1;
        return 0;
    }
    
    void fold(SBuf& sb) const
    {
        //sb.writeInt32((unsigned int)_n); 
    }

    void unfold(SBuf& sb)
    {
    }

    static Term parse(Stream& s)
    {
        // -?[.0-9]+(:[0-9]+)?([eE]-?[0-9]+)?

        SBuf sb;
        
        if (parseToBuf(s, sb))
        {
            Number n;
            n.set(atof(sb));
            return n;
        }
        else
        {
            // if we fail, must wind back all characters in buffer
            // XX need better stream reposition here.
            int cc = sb.size();
            while (cc)
            {
                --cc;
                --s;
            }
        }
        return Term();
    }

    static Number nan() 
    {
        Number v;
        //v.set(nanf(0));
        v.set(std::numeric_limits<valueT>::quiet_NaN());
        return v;
    }

    static Number posInf()
    {
        Number v;
        v.set(std::numeric_limits<valueT>::infinity());
        return v;
    }

    static Number negInf()
    {
        Number v;
        v.set(-std::numeric_limits<valueT>::infinity());
        return v;
    }
};


#endif // USE_DPD

