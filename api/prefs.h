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


#pragma once

#include "apitypes.h"

struct DLLX Prefs: public API<Prefs>
{
    Prefs(Control* c) : API(c) { _init(); }
    ~Prefs();

    string      getString(const char* key, const char* fallback = "") const;
    int         getInt(const char* key, int fallback = 0) const;
    double      getDouble(const char* key, double fallback = 0) const;

    bool        isDefined(const char* key) const;
    bool        remove(const char* tag);
    
    void        set(const char* key, const string& v) { set(key, v.c_str()); }
    void        set(const char* key, const char* v);
    void        set(const char* key, int v);
    void        set(const char* key, double v);

    static int         sliderToSeconds(int v);
    static int         secondsToSlider(int v);
    static string      timeoutSummary(int t, bool smallformat);
    
    string      toString() const;

private:

    void _init();
    
    struct Imp;
    Imp*                _imp;
};



