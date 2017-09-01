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


#include <assert.h>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include "disasm.h"
#include "emux.h"

using namespace std;

bool DisAssembly::is_absaddr(std::string const& s)
{
  return s.compare(0, 1, "$") == 0
      && s.size() == 5
      && s.find_first_not_of("0123456789abcdefABCDEF", 1) == std::string::npos;
}

void DisAssembly::get_absaddrlist()
{
    // Collect all absolute targets
    targets.clear();
    typedef map<int,Instruction>::const_iterator MapIterator;
    for (MapIterator ins = code.begin(); ins != code.end(); ins++)
    {
        Instruction i = ins->second;
        if (is_absaddr(i.arg1))
            targets.push_back( (int)strtol(i.arg1+1, NULL, 16));
        if (is_absaddr(i.arg2))
            targets.push_back( (int)strtol(i.arg2+1, NULL, 16));
    }
    sort(targets.begin(),targets.end());
    targets.erase(std::unique(targets.begin(), targets.end()), targets.end());
    if (dbg == 2)
    {
        cout << "Jump targets:" << endl;
        for (int i=0; i<targets.size();i++){
            cout << hex << setw(4) << setfill('0') << targets[i] << ", ";
        }

    }
}

void DisAssembly::find_unusedtargets()
{
    vector<int> thits;
    tnused.clear();
    thits.assign(targets.size(),0);
    typedef map<int,Instruction>::const_iterator MapIterator;
    for (MapIterator ins = code.begin(); ins != code.end(); ins++)
    {
        Instruction i = ins->second;

        auto fres = std::find(targets.begin(), targets.end(),i.addr);
        if (fres != targets.end())
            thits[distance(targets.begin (), fres)]=1;
    }
    for (int i=0; i<thits.size();i++)
        if (!thits[i])
            tnused.push_back(targets[i]);

}

void DisAssembly::dump_dasm(const char * filename)
{
    streambuf *coutbuf;
    ofstream fileout;
    if (filename != 0)
    {
        fileout = ofstream(filename);
        coutbuf = cout.rdbuf();
        cout.rdbuf(fileout.rdbuf());
    }
    string outbuf = "";

    get_absaddrlist();
    typedef map<int,Instruction>::const_iterator MapIterator;
    for (MapIterator ins = code.begin(); ins != code.end(); ins++)
    {
        outbuf = "";
        Instruction i = ins->second;

        auto fres = std::find(targets.begin(), targets.end(),i.addr);
        if (fres != targets.end())
        {
            //cout << "LBL_" << dec << distance(targets.begin (), fres) << endl;
            cout << "LBL_" << uppercase << hex << setw(4) << setfill('0') << i.addr << " ";
        }
        else
            cout << "         ";
        outbuf += "   ";
        outbuf += i.op;
        if (strcmp(i.opsize,"")!=0) {
            outbuf += ".";
            outbuf += i.opsize;
        }
        cout << left << setw(12) << setfill(' ') << uppercase << outbuf;
        outbuf = "";
        if (strcmp(i.arg1,"")!=0)
        {
            if (is_absaddr(i.arg1))
            {
               auto idx = std::find(targets.begin(), targets.end(),(int)strtol(i.arg1+1, NULL, 16));
               if (idx!=targets.end())
               {
                  outbuf += "LBL_";
                  //outbuf += std::to_string(distance(targets.begin (), idx));
                  outbuf += i.arg1+1;
               }
               else
                  outbuf += i.arg1;
            }
            else
                outbuf += i.arg1;
        }
        if (strcmp(i.arg2,"")!=0){
            outbuf += ",";
            if (is_absaddr(i.arg2))
            {
               auto idx = std::find(targets.begin(), targets.end(),(int)strtol(i.arg2+1, NULL, 16));
               if (idx!=targets.end())
               {
                  outbuf += "LBL_";
                  //outbuf += std::to_string(distance(targets.begin (), idx));
                  outbuf += i.arg2+1;
               }
               else
                  outbuf += i.arg2;
            }
            else
                outbuf += i.arg2;
        }
        cout << left << setw(32) << setfill(' ') << uppercase << outbuf;
        cout << ";" << right << hex << setw(4) << setfill('0') << i.addr << ": ";
        cout << i.cbytes;

        if (showBits)
        {
            cout << " " << setw(16) << setfill('0') << bitset<16>(strtol(i.cbytes.substr(0,4).c_str(), NULL, 16));
            if (i.size >= 2)
               cout << " " << setw(16) << setfill('0') << bitset<16>(strtol(i.cbytes.substr(5,4).c_str(), NULL, 16));
            if (i.size >= 3)
               cout << " " << setw(16) << setfill('0') << bitset<16>(strtol(i.cbytes.substr(10,4).c_str(),NULL,16));
            if (i.size == 4)
               cout << " " << setw(16) << setfill('0') << bitset<16>(strtol(i.cbytes.substr(15,4).c_str(), NULL, 16));
        }
        cout << "       " << i.comment << endl;
    }
    if (filename != 0)
        cout.rdbuf(coutbuf);

    find_unusedtargets();
    if (dbg)
    {
        cout << endl << "Unused jump targets:" << endl;
        for (int i=0; i<tnused.size();i++)
            cout << hex << setw(4) << setfill('0') << tnused[i] << endl;
    }
}

void DisAssembly::list_gaps()
{
    int ins_end=0;

    cout << "Gaps in disassembly:" << endl;
    typedef map<int,Instruction>::const_iterator MapIterator;
    for (MapIterator ins = code.begin(); ins != code.end(); ins++)
    {
        Instruction i = ins->second;

        if (ins_end && i.addr != ins_end)
        {
            cout << hex << setw(4) << setfill('0') << ins_end << " - " << hex << setw(4) << setfill('0') << i.addr;

            if (i.addr-ins_end <= 100)
            {
                for (int cnt=0;ins_end+cnt<i.addr;cnt++)
                    cout << (cnt%4==0?" ":"") << hex << setw(2) << setfill('0') << (int)(effective(ins_end+cnt)[0]);
                for (int cnt=0;ins_end+cnt<i.addr;cnt++)
                {
                    char c = effective(ins_end+cnt)[0];
                    if (c >=30 && c <= 127)
                        cout << c;
                    else
                        cout << "_";
                }
            }
            else
                cout << "...";

            // try finding possible rts or bsr in the block
            for (int ia=ins_end;ia<= i.addr-2;ia+=2)
            {
                if (effective(ia)[0]==0xa2 && effective(ia)[1]==0x00)
                    cout << " rts/" << hex << setw(4) << setfill('0') << ia << " ";
                if (effective(ia)[0]==0x60 && effective(ia)[1]==0x00)
                    cout << " bra.w/" << hex << setw(4) << setfill('0') << ia << " ";
                if (effective(ia)[0]==0xa4 && effective(ia)[1]==0x00)
                    cout << " bsr.w/" << hex << setw(4) << setfill('0') << ia << " ";
            }

            cout << endl;
        }
        ins_end=i.addr+i.size*2;
    }
    if (ins_end < 0xcb12)
        cout << hex << setw(4) << setfill('0') << ins_end << " - CB12" << endl;
}

int DisAssembly::nextgap()
{
    if (gapstart==-1)
    {
        // TEST: complete jump table
        gapstart=0x525A;
        gapend=0x5662;
    }
    else
    {
        gapstart=-1;
        gapend=-1;
    }
    return gapstart;

}

int DisAssembly::stopgap()
{
    return gapend;
}

extern "C" {
   void* create_dasm(){
      return reinterpret_cast<void*> (new DisAssembly);
   }
   void dasm_addins(void* dasm,void * ins) {
       DisAssembly* d = reinterpret_cast<DisAssembly*> (dasm);
       Instruction* i = reinterpret_cast<Instruction*> (ins);

       d->insert(i->addr,*i);
   }
   void* create_ins(){
      return reinterpret_cast<void*> (new Instruction);
   }
   void ins_setaddr(void* i, int addr) {
       Instruction* ins = reinterpret_cast<Instruction*> (i);
       ins->addr=addr;
   }
   void ins_setop(void* i, char* op){
       Instruction* ins = reinterpret_cast<Instruction*> (i);
       strcpy(ins->op,op);
   }
   void ins_setopsize(void* i, char* opsize){
       Instruction* ins = reinterpret_cast<Instruction*> (i);
       strcpy(ins->opsize,opsize);
   }
   void ins_appendarg1(void* i, char* arg1){
       Instruction* ins = reinterpret_cast<Instruction*> (i);
       strcat(ins->arg1,arg1);
   }
   void ins_appendarg2(void* i, char* arg2){
       Instruction* ins = reinterpret_cast<Instruction*> (i);
       strcat(ins->arg2,arg2);
   }
   void ins_setarg1(void* i, char* arg1){
       Instruction* ins = reinterpret_cast<Instruction*> (i);
       strcpy(ins->arg1,arg1);
   }
   void ins_setarg2(void* i, char* arg2){
       Instruction* ins = reinterpret_cast<Instruction*> (i);
       strcpy(ins->arg2,arg2);
   }
  void ins_appendcomment(void* i, char* comment){
      Instruction* ins = reinterpret_cast<Instruction*> (i);
      strcat(ins->comment,comment);
  }
  void ins_setsize(void* i, int blength) {
      Instruction* ins = reinterpret_cast<Instruction*> (i);
      ins->size=blength;
  }
  void ins_setcode(void* i, int append, char * codebytes) {
      Instruction* ins = reinterpret_cast<Instruction*> (i);
      if (append)
        ins->cbytes += codebytes;
      else
        ins->cbytes = codebytes;
  }
  void dasm_dump(void* dasm, const char* filename) {
      DisAssembly* d = reinterpret_cast<DisAssembly*> (dasm);
      d->dump_dasm(filename);
  }
  void dasm_jmp_reset(void* dasm, const char* filename) {
      DisAssembly* d = reinterpret_cast<DisAssembly*> (dasm);
      d->get_absaddrlist();
      d->find_unusedtargets();
      dasm_fillgaps(d->getUTArray(),d->getUTNum(),filename);
  }
  void dasm_listgaps(void* dasm) {
      DisAssembly* d = reinterpret_cast<DisAssembly*> (dasm);
      d->list_gaps();
  }
  int dasm_nextgap(void* dasm) {
      DisAssembly* d = reinterpret_cast<DisAssembly*> (dasm);
      return d->nextgap();
  }
  int dasm_gapend(void* dasm) {
      DisAssembly* d = reinterpret_cast<DisAssembly*> (dasm);
      return d->stopgap();
  }
}
