/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2017 Voidware Ltd.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code as
 * defined in and that are subject to the Voidware Public Source Licence version
 * 1.0 (the 'Licence'). You may not use this file except in compliance with the
 * Licence or with expressly written permission from Voidware.  Please obtain a
 * copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
 * it before using this file.
 *
 * THIS IS NOT FREE SOFTWARE.
 * 
 * The Original Code and all software distributed under the Licence are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Please see the Licence for the specific language governing rights and 
 * limitations under the Licence.
 *
 * contact@strandgames.com
 *
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for posterity and
 * historical reference.
 */

#include <stdio.h>
#include <stdlib.h>
#include "symbols.h"
#include "hunks.h"
#include "read_sym.h"
#include "dis1.h"

extern int n_text,n_data,n_bss;
extern char *Madness,*infile_pnt;
extern int debug;

void read_sym_tab(int n)
{
    int i;
    char buf[64];
    
    if (verbose) printf("Reading DR style symbol table..");

    for (i=0; i<n/14; i++)
    {
        int j;
        char* q = buf;
        for (j = 0; j < 8; ++j) *q++ = mgetb_infile();
        *q = 0;

        int flags = mgetb_infile();
        mgetb_infile();
        int value = mgetw_infile();

        add_symbol(buf, value, flags);
        
    }
    printf(".\n");
    // free(Madness);		/* de-allocate our chunk of memory */
}

void read_sym_tab_AmigaDOS()
{
    int i,no_of_symbols,sym_len,word,no_of_rels;
    char buf[64];
    int hv;
    
    word=mgetw_infile();
    if (word==hunk_reloc32)
    {
        if (verbose) printf("Skipping reloc info...\n");
        do
        {
            no_of_rels=(mgetw_infile());
            if (no_of_rels!=0) for (i=0;i<(1+no_of_rels);i++) mgetw_infile();
        }
        while (no_of_rels!=0);
        word=mgetw_infile();
    }

    if (word!=hunk_symbol) return;
    if (verbose) printf("Reading AmigaDOS style symbol table...\n");
    for (no_of_symbols=0;(sym_len=mgetw_infile())!=0;no_of_symbols++)
    {
        char* q = buf;
        for (i=0;i<(sym_len*4);i++) *q++ =mgetb_infile();
        *q = 0;
        
        int value = mgetw_infile();
        add_symbol(buf, value, 0);
    }
    
    if (mgetw_infile()!=hunk_end) printf("Missing hunk_end in code symbol table\n");

    hv = mgetw_infile();
    
    if (hv==hunk_bss)
    {
        int bss_syms;
        if (mgetw_infile()!=(n_bss/4)) if (verbose) printf("Iffy BSS size...\n");
        if (mgetw_infile()!=hunk_symbol)
        {
            if (verbose) printf("No BSS symbols...\n");
            else
            {
                if (verbose) printf("Getting BSS symbols...\n");
                for (bss_syms=0;(sym_len=mgetw_infile())!=0;bss_syms++)
                {
                    char* q = buf;
                    for (i=0;i<(sym_len*4);i++) *q++ =mgetb_infile();
                    *q = 0;
                    
                    int value = mgetw_infile()+n_text+n_data;
                    add_symbol(buf, value, 0);
                }
            }
        }
    }
    else
    {
        if (debug) printf("hunk value = %04X\n", hv);
    }

    if (verbose) printf("number of symbols %d\n", n_symbols);


    // free(Madness);		/* de-allocate our chunk of memory */
}

int mgetw(FILE* fp)
{
    int a,b,c,d;
    a = getc(fp);
    b = getc(fp);
    c = getc(fp);
    d = getc(fp);
    return a*0x1000000 + b*0x10000 + c*0x100 + d;
}
