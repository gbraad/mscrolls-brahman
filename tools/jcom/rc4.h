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
 *  contact@strandgames.com
 */
 

#pragma once

#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef uint32_t uint32;

struct RC4
{
    unsigned char       SBox[256];
    uint32              key[64];        // max key size

    unsigned char       si, sj;

    void init(uint32* key, size_t sz)
    {
        unsigned char K[256];
        unsigned int i, j;

        assert(sz <= 64);

        for (i = 0; i < 256; ++i) SBox[i] = i;

        //for (i = 0; i < 32; ++i) memcpy(K + i * 8, key, 8);
        j = 64/sz;
        for (i = 0; i < j; ++i)
            memcpy(K + i*sz*4, key, sz*4);
        
        // initial permutation of S
        j = 0;
        for (i = 0; i < 256; ++i) 
        {
            j = (j + SBox[i] + K[i]) & 0xff;
            unsigned int t = SBox[i];
            SBox[i] = SBox[j];
            SBox[j] = t;
        }

        si = 0;
        sj = 0;
    }

    unsigned int next()
    {
        /* generate the next key value */
        unsigned char ti, tj;
        
        ti = SBox[++si];
        sj += ti;
        tj = SBox[sj];

        SBox[si] = tj;
        SBox[sj] = ti;
        
        ti += tj;
        return SBox[ti];
    }
};

