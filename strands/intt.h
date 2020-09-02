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

#include "term.h"

struct Int: public Head
{
    // use the node pointer in the head to store a number
    
    Int(long v) : Head(type_int) { value(v); }

    long value() const
    {
        void* p = (void*)&_n;
        return *(long*)p;
    }
    
    void value(long v)
    {
        void* p = (void*)&_n;
        *(long*)p = v;
    }

    // convert to sbuf
    void toSBuf(SBuf& sb) const
    {
        sb.addLongString(value());
    }
    
    int compare(const Int& v) const
    {
        return value() - v.value();
    }
    
    void fold(SBuf& sb) const { sb.writeInt32(value()); } // XX 64 bits??
    void unfold(SBuf& sb) { _n = (Node*)(long)sb.readInt32(); } // XX 64 bits?

    static Term parse(Stream& s)
    {
        SBuf sb;
        
        //-?[0-9]+ [^a-z\.]

        bool neg = (*s == '-');

        if (neg)
        {
            sb.add(*s);
            ++s;
        }
        
        int cc;
        if (u_isdigit(*s))
        {
            cc = 0;
            do
            {
                sb.add(*s);
                ++s;
                if (++cc > 10) break; // 11 digits, always too big
                
            } while (u_isdigit(*s));

            // alpha or point following digits mean we are not an int.
            if (*s != '.' && !u_isalpha(*s)) 
            {
                // check the number is within the int range [-2^31,2^31]
            
                bool ok = cc < 10;   // < 10 digits, always ok
                if (!ok && cc == 10)
                {
                    // exactly 10 digits, we must check range
                    int c = strcmp(sb, "2147483648");
                    ok = c < 0 || (c == 0 && neg);
                }
            
                if (ok)
                    return Int(atoi(sb));
            }
        }

        // if we fail, must wind back all characters in buffer
        cc = sb.size();
        while (cc)
        {
            --cc;
            --s;
        }

        return Term();
    }
};
