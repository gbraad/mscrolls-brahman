/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2017.
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

/* 
 * LZSS with arithmetic encoding.
 * Original code based on LZARI
 */

/**************************************************************
    LZARI.C -- A Data Compression Program
***************************************************************
    4/7/1989 Haruhiko Okumura
    Use, distribute, and modify this program freely.
    Please send me your improved versions.
        PC-VAN        SCIENCE
        NIFTY-Serve    PIAF01022
        CompuServe    74050,1022
**************************************************************/


#include <stdio.h>
#include "types.h"

struct Compress
{
    const uchar*        _inbuf;
    size_t              _insz;
    const uchar*        _inp;

    uchar*              _outbuf;
    size_t              _outsz;
    uchar*              _outp;
    size_t              _outmax;

    unsigned int        buffer;
    unsigned int        mask;

    Compress() { _init(); }
    ~Compress() { _purge(); }

    void _init()
    {
        _inbuf = 0;
        _insz = 0;
        _outbuf = 0;
        _outsz = 0;
        _outmax = 0;
    }

    void _purge()
    {
        delete _outbuf;
        _init();
    }

    void compress(const unsigned char* data, size_t sz)
    {
        _purge();
        buffer = 0;
        mask = 128;
        
        _inp = _inbuf = data;
        _insz = sz;

        _outmax = sz;
        _outp = _outbuf = new unsigned char[_outmax];
        //_outsz = 0;
        
        Encode();
    }

    void decompress(const unsigned char* data, size_t sz, unsigned int usize)
    {
        _purge();
        buffer = 0;
        mask = 0;
        
        _inp = _inbuf = data;
        _insz = sz;

        _outmax = usize;

        // allocate one more byte for uncompress buffer than the size
        _outp = _outbuf = new unsigned char[usize + 1];

        Decode();

        // zero terminate the uncompressed buffer
        // so that it can be used as a string
        _outbuf[usize] = 0;
    }

    int get()
    {
        return (_inp < _inbuf + _insz) ? *_inp++ : EOF;
    }

    void put(unsigned char c)
    {
        if (_outp < _outbuf + _outmax)
        {
            *_outp++ = c;
            ++_outsz;
        }
        else
        {
            printf("warning outbuf overrrun\n");
        }
    }

    unsigned char* donate(size_t* size)
    {
        // donate the output buffer
        unsigned char* p = _outbuf;
        _outbuf = 0;
        if (size) *size = _outsz;
        return p;
    }

    /********** Bit I/O **********/

    void PutBit(int bit)  /* Output one bit (bit = 0,1) */
    {
        if (bit) buffer |= mask;
        if ((mask >>= 1) == 0)
        {
            put(buffer);
            buffer = 0;
            mask = 128;
        }
    }

    void FlushBitBuffer(void)  /* Send remaining bits */
    {
        for (int i = 0; i < 7; i++) PutBit(0);
    }

    int GetBit(void)  /* Get one bit (0 or 1) */
    {
        if ((mask >>= 1) == 0)
        {
            buffer = get();
            mask = 128;
        }
        return ((buffer & mask) != 0);
    }

    /********** LZSS with multiple binary trees **********/

#undef N
#undef NIL

#define N         4096    /* size of ring buffer */
#define FF           60    /* upper limit for match_length */
#define THRESHOLD    2   /* encode string into position and length
                            if match_length is greater than this */
#define NIL            N    /* index for root of binary search trees */

    unsigned char  text_buf[N + FF - 1];    /* ring buffer of size N,
                                              with extra FF-1 bytes to facilitate string comparison */
    int        match_position, match_length,  /* of longest match.  These are
                                                 set by the InsertNode() procedure. */
        lson[N + 1], rson[N + 257], dad[N + 1];  /* left & right children &
                                                    parents -- These constitute binary search trees. */

    void InitTree(void)  /* Initialize trees */
    {
        int  i;

        /* For i = 0 to N - 1, rson[i] and lson[i] will be the right and
           left children of node i.  These nodes need not be initialized.
           Also, dad[i] is the parent of node i.  These are initialized to
           NIL (= N), which stands for 'not used.'
           For i = 0 to 255, rson[N + i + 1] is the root of the tree
           for strings that begin with character i.  These are initialized
           to NIL.  Note there are 256 trees. */

        for (i = N + 1; i <= N + 256; i++) rson[i] = NIL;    /* root */
        for (i = 0; i < N; i++) dad[i] = NIL;    /* node */
    }

    void InsertNode(int r)
    /* Inserts string of length F, text_buf[r..r+F-1], into one of the
       trees (text_buf[r]'th tree) and returns the longest-match position
       and length via the global variables match_position and match_length.
       If match_length = F, then removes the old node in favor of the new
       one, because the old one will be deleted sooner.
       Note r plays double role, as tree node and position in buffer. */
    {
        int  i, p, cmp, temp;
        unsigned char  *key;

        cmp = 1;  key = &text_buf[r];  p = N + 1 + key[0];
        rson[r] = lson[r] = NIL;  match_length = 0;
        for ( ; ; ) {
            if (cmp >= 0) {
                if (rson[p] != NIL) p = rson[p];
                else {  rson[p] = r;  dad[r] = p;  return;  }
            } else {
                if (lson[p] != NIL) p = lson[p];
                else {  lson[p] = r;  dad[r] = p;  return;  }
            }
            for (i = 1; i < FF; i++)
                if ((cmp = key[i] - text_buf[p + i]) != 0)  break;
            if (i > THRESHOLD) {
                if (i > match_length) {
                    match_position = (r - p) & (N - 1);
                    if ((match_length = i) >= FF) break;
                } else if (i == match_length) {
                    if ((temp = (r - p) & (N - 1)) < match_position)
                        match_position = temp;
                }
            }
        }
        dad[r] = dad[p];  lson[r] = lson[p];  rson[r] = rson[p];
        dad[lson[p]] = r;  dad[rson[p]] = r;
        if (rson[dad[p]] == p) rson[dad[p]] = r;
        else                   lson[dad[p]] = r;
        dad[p] = NIL;  /* remove p */
    }

    void DeleteNode(int p)  /* Delete node p from tree */
    {
        int  q;
    
        if (dad[p] == NIL) return;  /* not in tree */
        if (rson[p] == NIL) q = lson[p];
        else if (lson[p] == NIL) q = rson[p];
        else {
            q = lson[p];
            if (rson[q] != NIL) {
                do {  q = rson[q];  } while (rson[q] != NIL);
                rson[dad[q]] = lson[q];  dad[lson[q]] = dad[q];
                lson[q] = lson[p];  dad[lson[p]] = q;
            }
            rson[q] = rson[p];  dad[rson[p]] = q;
        }
        dad[q] = dad[p];
        if (rson[dad[p]] == p) rson[dad[p]] = q;
        else                   lson[dad[p]] = q;
        dad[p] = NIL;
    }

    /********** Arithmetic Compression **********/

    /*  If you are not familiar with arithmetic compression, you should read
        I. E. Witten, R. M. Neal, and J. G. Cleary,
        Communications of the ACM, Vol. 30, pp. 520-540 (1987),
        from which much have been borrowed.  */

#define M   15

    /*    Q1 (= 2 to the M) must be sufficiently large, but not so
          large as the unsigned long 4 * Q1 * (Q1 - 1) overflows.  */

#define Q1  (1UL << M)
#define Q2  (2 * Q1)
#define Q3  (3 * Q1)
#define Q4  (4 * Q1)
#define MAX_CUM (Q1 - 1)

#define N_CHAR  (256 - THRESHOLD + FF)
    /* character code = 0, 1, ..., N_CHAR - 1 */

    unsigned long int  low = 0, high = Q4, value = 0;
    int  shifts = 0;  /* counts for magnifying low and high around Q2 */
    int  char_to_sym[N_CHAR], sym_to_char[N_CHAR + 1];
    unsigned int
    sym_freq[N_CHAR + 1],  /* frequency for symbols */
                    sym_cum[N_CHAR + 1],   /* cumulative freq for symbols */
                    position_cum[N + 1];   /* cumulative freq for positions */

    void StartModel(void)  /* Initialize model */
    {
        int ch, sym, i;
    
        sym_cum[N_CHAR] = 0;
        for (sym = N_CHAR; sym >= 1; sym--) {
            ch = sym - 1;
            char_to_sym[ch] = sym;  sym_to_char[sym] = ch;
            sym_freq[sym] = 1;
            sym_cum[sym - 1] = sym_cum[sym] + sym_freq[sym];
        }
        sym_freq[0] = 0;  /* sentinel (!= sym_freq[1]) */
        position_cum[N] = 0;
        for (i = N; i >= 1; i--)
            position_cum[i - 1] = position_cum[i] + 10000 / (i + 200);
        /* empirical distribution function (quite tentative) */
        /* Please devise a better mechanism! */
    }

    void UpdateModel(int sym)
    {
        int i, c, ch_i, ch_sym;
    
        if (sym_cum[0] >= MAX_CUM) {
            c = 0;
            for (i = N_CHAR; i > 0; i--) {
                sym_cum[i] = c;
                c += (sym_freq[i] = (sym_freq[i] + 1) >> 1);
            }
            sym_cum[0] = c;
        }
        for (i = sym; sym_freq[i] == sym_freq[i - 1]; i--) ;
        if (i < sym) {
            ch_i = sym_to_char[i];    ch_sym = sym_to_char[sym];
            sym_to_char[i] = ch_sym;  sym_to_char[sym] = ch_i;
            char_to_sym[ch_i] = sym;  char_to_sym[ch_sym] = i;
        }
        sym_freq[i]++;
        while (--i >= 0) sym_cum[i]++;
    }

    void Output(int bit)  /* Output 1 bit, followed by its complements */
    {
        PutBit(bit);
        for ( ; shifts > 0; shifts--) PutBit(! bit);
    }

    void EncodeChar(int ch)
    {
        int  sym;
        unsigned long int  range;

        sym = char_to_sym[ch];
        range = high - low;
        high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
        low +=       (range * sym_cum[sym    ]) / sym_cum[0];
        for ( ; ; ) {
            if (high <= Q2) Output(0);
            else if (low >= Q2) {
                Output(1);  low -= Q2;  high -= Q2;
            } else if (low >= Q1 && high <= Q3) {
                shifts++;  low -= Q1;  high -= Q1;
            } else break;
            low += low;  high += high;
        }
        UpdateModel(sym);
    }

    void EncodePosition(int position)
    {
        unsigned long int  range;

        range = high - low;
        high = low + (range * position_cum[position    ]) / position_cum[0];
        low +=       (range * position_cum[position + 1]) / position_cum[0];
        for ( ; ; ) {
            if (high <= Q2) Output(0);
            else if (low >= Q2) {
                Output(1);  low -= Q2;  high -= Q2;
            } else if (low >= Q1 && high <= Q3) {
                shifts++;  low -= Q1;  high -= Q1;
            } else break;
            low += low;  high += high;
        }
    }

    void EncodeEnd(void)
    {
        shifts++;
        if (low < Q1) Output(0);  else Output(1);
        FlushBitBuffer();  /* flush bits remaining in buffer */
    }

    int BinarySearchSym(unsigned int x)
    /* 1      if x >= sym_cum[1],
       N_CHAR if sym_cum[N_CHAR] > x,
       i such that sym_cum[i - 1] > x >= sym_cum[i] otherwise */
    {
        int i, j, k;
    
        i = 1;  j = N_CHAR;
        while (i < j) {
            k = (i + j) / 2;
            if (sym_cum[k] > x) i = k + 1;  else j = k;
        }
        return i;
    }

    int BinarySearchPos(unsigned int x)
    /* 0 if x >= position_cum[1],
       N - 1 if position_cum[N] > x,
       i such that position_cum[i] > x >= position_cum[i + 1] otherwise */
    {
        int i, j, k;
    
        i = 1;  j = N;
        while (i < j) {
            k = (i + j) / 2;
            if (position_cum[k] > x) i = k + 1;  else j = k;
        }
        return i - 1;
    }

    void StartDecode(void)
    {
        for (int i = 0; i < M + 2; i++) value = 2 * value + GetBit();
    }

    int DecodeChar(void)
    {
        int     sym, ch;
        unsigned long int  range;
    
        range = high - low;
        sym = BinarySearchSym((unsigned int)
                              (((value - low + 1) * sym_cum[0] - 1) / range));
        high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
        low +=       (range * sym_cum[sym    ]) / sym_cum[0];
        for ( ; ; ) {
            if (low >= Q2) {
                value -= Q2;  low -= Q2;  high -= Q2;
            } else if (low >= Q1 && high <= Q3) {
                value -= Q1;  low -= Q1;  high -= Q1;
            } else if (high > Q2) break;
            low += low;  high += high;
            value = 2 * value + GetBit();
        }
        ch = sym_to_char[sym];
        UpdateModel(sym);
        return ch;
    }

    int DecodePosition(void)
    {
        int position;
        unsigned long int  range;
    
        range = high - low;
        position = BinarySearchPos((unsigned int)
                                   (((value - low + 1) * position_cum[0] - 1) / range));
        high = low + (range * position_cum[position    ]) / position_cum[0];
        low +=       (range * position_cum[position + 1]) / position_cum[0];
        for ( ; ; ) {
            if (low >= Q2) {
                value -= Q2;  low -= Q2;  high -= Q2;
            } else if (low >= Q1 && high <= Q3) {
                value -= Q1;  low -= Q1;  high -= Q1;
            } else if (high > Q2) break;
            low += low;  high += high;
            value = 2 * value + GetBit();
        }
        return position;
    }

    /********** Encode and Decode **********/

    void Encode()
    {
        int  i, c, len, r, s, last_match_length;
    
        if (_insz == 0) return;
        
        StartModel();
        InitTree();
        s = 0;  r = N - FF;
        for (i = s; i < r; i++) text_buf[i] = ' ';
        for (len = 0; len < FF && (c = get()) != EOF; len++)
            text_buf[r + len] = c;

        for (i = 1; i <= FF; i++) InsertNode(r - i);
        InsertNode(r);
        do
        {
            if (match_length > len) match_length = len;
            if (match_length <= THRESHOLD) {
                match_length = 1;  EncodeChar(text_buf[r]);
            } else {
                EncodeChar(255 - THRESHOLD + match_length);
                EncodePosition(match_position - 1);
            }
            last_match_length = match_length;
            for (i = 0; i < last_match_length &&
                     (c = get()) != EOF; i++)
            {
                DeleteNode(s);
                text_buf[s] = c;
                if (s < FF - 1) text_buf[s + N] = c;
                s = (s + 1) & (N - 1);
                r = (r + 1) & (N - 1);
                InsertNode(r);
            }
            while (i++ < last_match_length)
            {
                DeleteNode(s);
                s = (s + 1) & (N - 1);
                r = (r + 1) & (N - 1);
                if (--len) InsertNode(r);
            }
        } while (len > 0);
        EncodeEnd();
    }

    void Decode(void)
    {
        int  i, j, k, r, c;
        unsigned int count;

        StartDecode();
        StartModel();
        for (i = 0; i < N - FF; i++) text_buf[i] = ' ';
        r = N - FF;
        for (count = 0; count < _outmax; )
        {
            c = DecodeChar();
            if (c < 256)
            {
                put(c);
                text_buf[r++] = c;
                r &= (N - 1);
                count++;
            }
            else
            {
                i = (r - DecodePosition() - 1) & (N - 1);
                j = c - 255 + THRESHOLD;
                for (k = 0; k < j; k++)
                {
                    c = text_buf[(i + k) & (N - 1)];
                    put(c);
                    text_buf[r++] = c;
                    r &= (N - 1);
                    count++;
                }
            }
        }
    }
};

#undef N
#undef NIL

