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
 *  Copyright (c) Strand Games 2021.
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

#include "strutils.h"

struct MDFilter
{
    bool skipMarkup()
    {
        // skip "[foo](bar)"
        bool r = (*_pos == '[');

        if (r)
        {
            while (*++_pos)
            {
                if (*_pos == ']')
                {
                    if (*++_pos == '(')
                    {
                        int level = 1;
                        while (*++_pos)
                        {
                            if (*_pos == '(') ++level;
                            else if (*_pos == ')' && !--level)
                            {
                                ++_pos;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
        return r;
    }

    void advance()
    {
        // advance position, but skip existing markup as if it is not
        // present in the input.
        if (!skipMarkup()) ++_pos;
    }

    void skipSpace()
    {
        while (u_isspace(*_pos)) advance();
    }

    void skipWord()
    {
        while (wordChar(*_pos)) ++_pos;
    }

    void skipToWord()
    {
        // skip markup, spaces and punctuation until we start a word
        for (;;)
        {
            skipSpace();
            if (!*_pos || wordChar(*_pos)) break;
            advance();
        }
    }

    static bool wordChar(char c)
    {
        return u_isalnum(c);
    }
    
    const char*         _pos;
};

