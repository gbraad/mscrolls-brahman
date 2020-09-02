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


#include "kllib.h"

using namespace std;

void Blob::_purge()
{
    Info* i = info();
    if (i)
    {
        //LOG3("~Blob, ", "purge");
        
        assert(i->_lib);
        i->_lib->destroy(this);
        delete i;
    }
}

void Blob::toSBuf(SBuf& sb) const
{
    Info* i = info();

    string s;
    
    if (annotateString())
    {
        s = "[Blob ";
        if (i)
        {
            assert(i->_lib);
            s += i->_lib->libname();
            s += ' ';
            s += i->_lib->asString(this);
        }
        else s += "null";
        s += "]";
    }
    else
    {
        // without annotation, we just call the blob stringifier
        assert(i->_lib);
        s = i->_lib->asString(this);   
    }
    
    sb.add(s.c_str());
}

int Blob::compare(const Blob& v) const
{
    Info* i1 = info();
    Info* i2 = v.info();

    assert(i1->_lib);

    // ensure libs the same
    int d = (int)(i1->_lib - i2->_lib);
    if (!d)
    {
        d = i1->_lib->compare(i1, i2);
    }
    return d;
}
