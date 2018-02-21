/**
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
 *
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "hunks.h"
#include "symbols.h"
#include "memory.h"
#include "errors.h"

extern char *code_name, *data_name, *bss_name;
extern int printtotalsize;
extern int debug;

int code_hunk;		/* hunk number of code section */
int data_hunk;		/* hunk number of data section */
int bss_hunk;		/* hunk number of bss section */
int n_hunks;

static void output_section(int section, int ext, FILE* fp);

static void putlong(int n, FILE* fp)
{
    putc(n>>24, fp);
    putc(n>>16, fp);
    putc(n>>8, fp);
    putc(n, fp);
}

static void output_symbolname(int type, char* s, FILE* fp)
{
    int len=strlen(s);
    int rnd;				/* set to the length rounded up */
    int i;
    char *p;

    rnd = (len + 3) & ~3;		/* round up to a lword */
    putlong((type<<24) + rnd/4, fp);
    p = s;
    while (*p) putc(*p++, fp);
    /* pad with zeros */
    for (i=rnd-len; i; i--) putc(0, fp);
}

static void output_name(char* s, FILE* fp)
{
    putlong(hunk_name, fp);
    output_symbolname(0, s, fp);
}

void output_loadfile(char* name)
{
    FILE *fp = fopen(name, "wb");
    int i;
    int hunk_size[3];			/* sizes of the hunks */
    int totalsize=0;

    if (!fp)
    {
        perror(name);
        exit(-1);
    }

    if (code_address_base)
    {
        code_hunk=n_hunks;
        totalsize += hunk_size[n_hunks] = (code_address_base-code_load_base + 3)/4;
        n_hunks++;
    }
    if (data_address_base)
    {
        data_hunk=n_hunks;
        totalsize += hunk_size[n_hunks] = (data_address_base + 3)/4;
        n_hunks++;
    }
    if (bss_address_base)
    {
        bss_hunk=n_hunks;
        totalsize += hunk_size[n_hunks] = (bss_address_base + 3)/4;
        n_hunks++;
    }

    /* output the header info */
    putlong(hunk_header, fp);
    putlong(0, fp);				/* no name list */
    putlong(n_hunks, fp);		/* table size */
    putlong(0, fp);				/* 1st hunk = 0 */
    putlong(n_hunks-1, fp);		/* last hunk */
    /* sizes */
    for (i=0; i<n_hunks; i++) putlong(hunk_size[i], fp);

    if (debug) fflush(fp);

    /* code section */
    output_section(N_TEXT, 0, fp);

    if (debug) fflush(fp);

    /* data section */
    output_section(N_DATA, 0, fp);

    if (debug) fflush(fp);

    /* bss section */
    output_section(N_BSS, 0, fp);

    if (debug) fflush(fp);

    if (printtotalsize)
        printf("total running size = $%08x (%d)\n",
               4*totalsize, 4*totalsize);
}

void output_object(char* name)
{
    FILE *fp = fopen(name, "wb");

    if (code_address_base)
    {
        code_hunk=n_hunks;
        n_hunks++;
    }
    if (data_address_base)
    {
        data_hunk=n_hunks;
        n_hunks++;
    }
    if (bss_address_base)
    {
        bss_hunk=n_hunks;
        n_hunks++;
    }

    /* program unit */
    putlong(hunk_unit, fp);
    putlong(0, fp);			/* no name */

    /* code */
    output_section(N_TEXT, 1, fp);

    /* data */
    output_section(N_DATA, 1, fp);
}


#define RELOC_N 500

static void output_reloc(int section, int want16, FILE* fp)
{
    /* want16 true if reloc16An info wanted */
    int i;
    struct reloc_item *rp;
    int target;

    int *reloc32_c, size32_c=RELOC_N, n32_c=0;
    int *reloc32_d, size32_d=RELOC_N, n32_d=0;
    int *reloc32_b, size32_b=RELOC_N, n32_b=0;
    int *reloc16_c = 0, size16_c=RELOC_N, n16_c=0;
    int *reloc16_d = 0, size16_d=RELOC_N, n16_d=0;
    int *reloc16_b = 0, size16_b=RELOC_N, n16_b=0;

    reloc32_c = (int *)malloc(RELOC_N*sizeof(int));
    reloc32_d = (int *)malloc(RELOC_N*sizeof(int));
    reloc32_b = (int *)malloc(RELOC_N*sizeof(int));
    
    if (want16)
    {
        reloc16_c = (int *)malloc(RELOC_N*sizeof(int));
        reloc16_d = (int *)malloc(RELOC_N*sizeof(int));
        reloc16_b = (int *)malloc(RELOC_N*sizeof(int));
    }

    /* separate out the different types of relocation info */
    for (i=0, rp=reloc; i<n_reloc; i++, rp++)
    {
        /* absolute 32 bit relocation info */
        if (!rp->info.r_pcrel && rp->info.r_length==2)
        {
            if (rp->section == section)
            {
                /* work out which section the absolute value
                 * points to */
                if (rp->info.r_extern)
                    target = nlist_array[rp->info.r_symbolnum].n_type & N_TYPE;
                else
                    target = rp->info.r_symbolnum;
                
                switch (target)
                {
                case N_TEXT :
                    if (n32_c == size32_c)
                    {
                        reloc32_c = (int *)realloc(reloc32_c, 2*size32_c*sizeof(int));
                        size32_c *= 2;
                    }
                    reloc32_c[n32_c++] = rp->info.r_address-code_load_base;
                    break;
                case N_DATA :
                    if (n32_d == size32_d)
                    {
                        reloc32_d = (int *)realloc(reloc32_d, 2*size32_d*sizeof(int));
                        size32_d *= 2;
                    }
                    reloc32_d[n32_d++] = rp->info.r_address;
                    break;
                case N_BSS :
                    if (n32_b == size32_b)
                    {
                        reloc32_b = (int *)realloc(reloc32_b, 2*size32_b*sizeof(int));
                        size32_b *= 2;
                    }
                    reloc32_b[n32_b++] = rp->info.r_address;
                    break;
                }
            }
            /* absolute 16 bit relocation info */
        }
        else if (want16 && !rp->info.r_pcrel && rp->info.r_length==1)
        {
            if (rp->section == section)
            {
                /* work out which section the absolute value
                 * points to */
                if (rp->info.r_extern)
                    target = nlist_array[rp->info.r_symbolnum].n_type & N_TYPE;
                else
                    target = rp->info.r_symbolnum;
                
                switch (target)
                {
                case N_TEXT :
                    if (n16_c == size16_c)
                    {
                        reloc16_c = (int *)realloc(reloc16_c, 2*size16_c*sizeof(int));
                        size16_c *= 2;
                    }
                    reloc16_c[n16_c++] = rp->info.r_address-code_load_base;
                    break;
                case N_DATA :
                    if (n16_d == size16_d)
                    {
                        reloc16_d = (int *)realloc(reloc16_d, 2*size16_d*sizeof(int));
                        size16_d *= 2;
                    }
                    reloc16_d[n16_d++] = rp->info.r_address;
                    break;
                case N_BSS :
                    if (n16_b == size16_b)
                    {
                        reloc16_b = (int *)realloc(reloc16_b, 2*size16_b*sizeof(int));
                        size16_b *= 2;
                    }
                    reloc16_b[n16_b++] = rp->info.r_address;
                    break;
                }
            }
        }
    }

    /* output the info */
    if (n32_c || n32_d || n32_b)
    {
        putlong(hunk_reloc32, fp);
        if (n32_c)
        {
            putlong(n32_c, fp);
            putlong(code_hunk, fp);
            for (i=0; i<n32_c; i++) putlong(reloc32_c[i], fp);
        }
        if (n32_d)
        {
            putlong(n32_d, fp);
            putlong(data_hunk, fp);
            for (i=0; i<n32_d; i++) putlong(reloc32_d[i], fp);
        }
        if (n32_b)
        {
            putlong(n32_b, fp);
            putlong(bss_hunk, fp);
            for (i=0; i<n32_b; i++) putlong(reloc32_b[i], fp);
        }
        putlong(0, fp);		/* zterm the 32 bit info */
    }

    if (want16 && (n16_c || n16_d || n16_b))
    {
        putlong(hunk_reloc16An, fp);
        if (n16_c)
        {
            putlong(n16_c, fp);
            putlong(code_hunk, fp);
            for (i=0; i<n16_c; i++)
                putlong(reloc16_c[i], fp);
        }
        if (n16_d)
        {
            putlong(n16_d, fp);
            putlong(data_hunk, fp);
            for (i=0; i<n16_d; i++)
                putlong(reloc16_d[i], fp);
        }
        if (n16_b)
        {
            putlong(n16_b, fp);
            putlong(bss_hunk, fp);
            for (i=0; i<n16_b; i++)
                putlong(reloc16_b[i], fp);
        }
        putlong(0, fp);		/* zterm the 16 bit info */
    }

    free(reloc32_c);
    free(reloc32_d);
    free(reloc32_b);
    
    if (want16)
    {
        free(reloc16_c);
        free(reloc16_d);
        free(reloc16_b);
    }
}

static void output_section(int section, int ext, FILE* fp)
{
    /* ext true if hunk_ext wanted */
    struct nlist *np;
    struct reloc_item *rp, *start;
    int i;
    int section_len = 0;
    static int done_abs_syms=0;		/* set if N_ABS syms already output */

    if (section == N_TEXT)
        section_len = (code_address_base-code_load_base + 3)/4;
    else if (section == N_DATA)
        section_len = (data_address_base + 3)/4;
    else if (section == N_BSS)
        section_len = (bss_address_base + 3)/4;

    if (section_len)
    {
        switch (section)
        {
        case N_TEXT :
            if (ext)
                output_name(code_name, fp);
            putlong(hunk_code, fp);
            putlong(section_len, fp);
#ifdef MS_BIG_ENDIAN
            fwrite(Code, 4, section_len, fp);
#else
            for (i=0; i<4*section_len; i++) putc(Code[MAXCODE-1-i], fp);
#endif
            break;
        case N_DATA :
            if (ext)
                output_name(code_name, fp);
            putlong(hunk_data, fp);
            putlong(section_len, fp);
#ifdef MS_BIG_ENDIAN
            fwrite(Data, 4, section_len, fp);
#else
            for (i=0; i<4*section_len; i++) putc(Data[MAXDATA-1-i], fp);
#endif
            break;
        case N_BSS :
            if (ext)
                output_name(code_name, fp);
            putlong(hunk_bss, fp);
            putlong(section_len, fp);
            break;
        }

        /* relocation info */
        /* only bother with 16 bit reloc info if incremental linking */
        output_reloc(section, ext, fp);

        /* external symbol info */
        if (ext)
        {
            int n8, n16pcr, n16abs, n32;
            putlong(hunk_ext, fp);
            for (np=nlist_array, i=n_syms; i; i--, np++)
                if (np->n_type == (N_UNDF|N_EXT)) {	/* an undefined sym */
                    /* find start of list of relocation info for sym */
                    start = (struct reloc_item *)np->n_value;
                    /* count how many are for this section */
                    n8=n16abs=n16pcr=n32=0;
                    for (rp = start; rp; rp = rp->next)
                        if (rp->section == section)
                            switch (rp->info.r_length)
                            {
                            case 0 :	/* byte size */
                                if (!rp->info.r_pcrel)
                                {
                                    printf("help! 8 bit absolute relocation!\n");
                                    exit(-1);
                                }
                                n8++;
                                break;
                            case 1 :	/* word size */
                                if (!rp->info.r_pcrel)
                                    n16abs++;
                                else
                                    n16pcr++;
                                break;
                            case 2 :	/* long size */
                                if (rp->info.r_pcrel)
                                {
                                    printf("help! 32 bit pc rel relocation!\n");
                                    exit(-1);
                                }
                                n32++;
                                break;
                            }

                    if (n8)
                    {
                        output_symbolname(ext_ref8, np->n_un.n_name, fp);
                        putlong(n8, fp);
                        for (rp = start; rp; rp = rp->next)
                            if (rp->section==section && rp->info.r_length==0)
                                putlong(rp->info.r_address, fp);
                    }

                    if (n16pcr)
                    {
                        output_symbolname(ext_ref16, np->n_un.n_name, fp);
                        putlong(n16pcr, fp);
                        for (rp = start; rp; rp = rp->next)
                            if (rp->section==section && rp->info.r_length==1 &&
                                rp->info.r_pcrel)
                                putlong(rp->info.r_address, fp);
                    }

                    if (n16abs)
                    {
                        output_symbolname(ext_ref16An, np->n_un.n_name, fp);
                        putlong(n16abs, fp);
                        for (rp = start; rp; rp = rp->next)
                            if (rp->section==section && rp->info.r_length==1 &&
                                !rp->info.r_pcrel)
                                putlong(rp->info.r_address, fp);
                    }

                    if (n32)
                    {
                        output_symbolname(ext_ref32, np->n_un.n_name, fp);
                        putlong(n32, fp);
                        for (rp = start; rp; rp = rp->next)
                            if (rp->section==section && rp->info.r_length==2)
                                putlong(rp->info.r_address, fp);
                    }
                }
                else if (np->n_type == (section|N_EXT))
                {
                    output_symbolname(ext_def, np->n_un.n_name, fp);
                    putlong(np->n_value, fp);
                }
                else if (!done_abs_syms && np->n_type == (N_ABS|N_EXT))
                {
                    output_symbolname(ext_abs, np->n_un.n_name, fp);
                    putlong(np->n_value, fp);
                }

            putlong(0, fp);		/* terminate the external symbol info */
            if (!done_abs_syms) done_abs_syms=1;
        }

        /* output symbols */
        putlong(hunk_symbol, fp);
        for (i=n_syms, np=nlist_array; i; i--, np++)
        {
            if ((np->n_type & N_TYPE) == section)
            {
                if (np->n_type & N_EXT && !np->n_other)
                    unused(np);
                output_symbolname(0, np->n_un.n_name, fp);
                putlong(np->n_value, fp);
            }
            else if (!ext && np->n_type == (N_UNDF|N_EXT))
                undefined(np);
        }
        putlong(0, fp);					/* terminate */

        putlong(hunk_end, fp);
    }
}

