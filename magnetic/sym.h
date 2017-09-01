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

#include <string>
#include <map>
#include <ctype.h>
#include "hunks.h"

struct Symbols
{
    typedef std::string string;
    typedef std::map<string, int> Syms;

    Syms _syms;
    
    void add_symbol(const string& name, int value)
    {
        //printf("symbol '%s' = %08x\n", name.c_str(), value);
        _syms.insert(std::make_pair(name, value));
    }
    
    int get_value(const char* name) const
    {
        Syms::const_iterator it = _syms.find(name);
        if (it != _syms.end()) return it->second;
        return 0;
    }

    size_t size() const { return _syms.size(); }
    
    static int get_word(FILE* fp)
    {
        int a = getc(fp);
        int b = getc(fp);
        return (a << 8) | b;
    }

    static int get_int(FILE* fp)
    {
        int a = get_word(fp);
        int b = get_word(fp);
        return (a << 16) | b;
    }
    
    bool load_syms(FILE* fp)
    {
        /* format of a very simple AmigaDOS load file:

           hunk_header L
           0 L
           1 L (n hunks, 2=>BSS, but none present)
           0 L
           0 L
           size/4
           hunk_code
           size of code/4 (also size_of_code == size)
           the code (size_of_code longwords)

           hunk_reloc32
           [n, hunk#, then n longwords of reloc 32] repeats until n == 0

           hunk_symbol

           [sym_len/4 sym_len bytes (sym name) value.L ] repeat until sym_len == 0
           hunk_end
        */

        bool res = false;

        for (;;)
        {
            int v = get_int(fp);
            if (v == hunk_reloc32)
            {
                // skip reloc
                for (;;)
                {
                    int n = get_int(fp);
                    if (n <= 0) break;
                    get_int(fp); // hn
                    while (n--) get_int(fp);
                }
            }
            else if (v == hunk_symbol)
            {
                res = true;
                for (;;)
                {
                    int sz = get_int(fp);
                    if (sz <= 0) break;

                    string s;
                    for (int i = 0; i < sz*4; ++i)
                    {
                        int c = getc(fp);
                        if (c > 0) s += (char)c;
                    }

                    int val = get_int(fp);

                    if (s.size())
                    {
                        bool skip = isdigit(s[0]);
                        
                        if (!skip)
                            add_symbol(s, val);
                    }
                }
            }
            else break; // unknown
        }

        return res;
    }
};
