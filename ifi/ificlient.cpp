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
 *  Copyright (c) Strand Games 2018.
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


#include "ificlient.h"
#include <iostream>
#include "jsonwalker.h"

using namespace std;

//////// Host features

bool IFIClient::eval(const char* json)
{
    // on host thread
    
    bool r = true;
    JSONWalker jw(json);

    string key;
    for (jw.begin(); !(key = jw.getKey()).empty(); jw.next())
    {
        cout << key << ": ";
        bool isObject;
        var v = jw.getValue(isObject);
        if (v) cout << v;
        else if (isObject) cout << "{}";
        else cout << "NULL";
        cout << endl;

        if (v)
        {
            if (key == "command") r = evalCommand(v.toString());
        }
    }
    
    return r;
}

