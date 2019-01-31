/**
 *
 *    _    __        _      __
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/
 *
 *  Copyright (©) Voidware 2016-2017.
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

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include "emux.h"

/*
 * Creating disassembly for The Pawn:
 * Run game with -rpawn.rec
 * execute the following commands:
 * save -> test -> y
 * load -> test -> y
 * restore -> (some wrong name) -> Y
 * quit -> q -> n
 * restart -> r -> y
 * hint -> asasas
 * hint -> some garbage
 * hint -> dfdfdfe
 * #dgaps
 */
struct Instruction
{
    typedef std::string string;

    int addr=-1;
    char op[6]="";
    char opsize[2]="";
    char arg1[25]="";
    char arg2[25]="";
    char comment[200]="";
    int size=1;
    int isText=0;
    string cbytes="";


    bool operator < (const Instruction& i) const { return addr < i.addr; }

    friend std::ostream& operator<<(std::ostream& os, const Instruction& ins)
    { return os << ""; }

};

struct DisAssembly
{

    std::map<int,Instruction> code;
    bool showBits = true;
    std::vector<int> targets;
    std::vector<int> tnused;
    int dbg = 0;
    int gapstart=-1;
    int gapend=-1;

    DisAssembly() { _init(); }

    size_t size() const { return code.size(); }
    const Instruction& get(int addr) const { return code.at(addr); }
    void insert(int key, Instruction& ins){ code.insert(std::pair<int, Instruction>(key, ins)); }
    void get_absaddrlist();
    void find_unusedtargets();
    bool is_absaddr(std::string const& s);
    void dump_dasm(const char * filename);
    void list_gaps();
    int nextgap();
    int stopgap();

    int* getUTArray(){ return (int*)&tnused[0]; }
    int getUTNum(){ return tnused.size();}

private:

    void _init()
    {
    }

};


