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

#include <ctype.h>
#include <string.h>

struct Opt
{
    static bool isOpt(char* opt, const char* val)
    {
        // test option and if so, remove it
        bool r = !strcmp(opt, val);
        if (r) *opt = 0;
        return r;
    }

    static char* isOptArg(char** opt, const char* val)
    {
        char* arg = 0;
        static char dummy[1] = { 0 };
    
        // test option and if so, remove it
        if (!strcmp(*opt, val))
        {
            arg = opt[1];
            if (arg)
            {
                opt[0] = dummy;
                opt[1] = dummy;

                // clean arg, sometimes they can have spaces
                while (isspace(*arg)) ++arg;
                char* p = arg;
                while (*p && !isspace(*p)) ++p;
                *p = 0;
            }
        }
        return arg;
    }

    static char* nextOptArg(char**& optAddr, const char* val, bool keep = false)
    {
        char* arg = 0;
        static char dummy[1] = { 0 };

        char** opt = optAddr;

        // test option and if so, remove it unless `keep`
        if (!strcmp(*opt, val))
        {
            ++optAddr; // skip arg
            
            arg = opt[1];
            if (arg)
            {
                if (!keep)
                {
                    opt[0] = dummy;
                    opt[1] = dummy;
                }

                // clean arg, sometimes they can have spaces
                while (isspace(*arg)) ++arg;
                char* p = arg;
                while (*p && !isspace(*p)) ++p;
                *p = 0;
            }
        }
        return arg;
    }

    static void rebuildArgs(int& argc, char** argv)
    {
        int n = 0;
        char** p = argv;
        char** q = p;

        while (*q)
        {
            if (**q)
            {
                *p++ = *q;
                ++n;
            }
            ++q;
        }
        *p = 0;
        argc = n;
    }
    
};
