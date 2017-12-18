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

#define MAXSYMS 10000
#define HASHSIZE 20000      /* size of the hash table */
#define MAXRELOC 10000

#pragma once

/*
 * Format of a relocation datum.
 */
struct relocation_info
{
    int r_address;  /* address which is relocated */
    unsigned int    r_symbolnum:24, /* local symbol ordinal */
        r_pcrel:1,  /* was relocated pc relative already */
        r_length:2, /* 0=byte, 1=word, 2=long */
        r_extern:1, /* does not include value of sym referenced */
        :4;     /* nothing, yet */
};

/*
 * Format of a symbol table entry
 */
struct nlist
{
    union
    {
        char    *n_name;    /* for use when in-core */
        long    n_strx;     /* index into file string table */
    } n_un;
    unsigned char   n_type;     /* type flag, i.e. N_TEXT etc; see below */
    char    n_other;    /* unused */
    short   n_desc;     /* see <stab.h> */
    unsigned long   n_value;    /* value of this symbol (or sdb offset) */
};

#define n_hash  n_desc      /* used internally by ld */

/*
 * Simple values for n_type.
 */
#define N_UNDF  0x0     /* undefined */
#define N_ABS   0x2     /* absolute */
#define N_TEXT  0x4     /* text */
#define N_DATA  0x6     /* data */
#define N_BSS   0x8     /* bss */
#define N_COMM  0x12        /* common (internal to ld) */
#define N_FN    0x1f        /* file name symbol */

#define N_EXT   01      /* external bit, or'ed in */
#define N_TYPE  0x1e        /* mask for all the type bits */

struct symbol
{
    struct symbol *next;
    struct nlist *sym;
};

struct reloc_item
{
    struct reloc_item *next;
    int section;                /* N_TEXT or N_DATA */
    struct relocation_info info;
};

extern struct symbol *hash_array[];
extern struct nlist nlist_array[];
extern int n_syms;
extern struct reloc_item reloc[];
extern int n_reloc;

void init_symbols();
void add_global_symbol(char* s, int type, int value);
int find_global_symbol(char* s);
int add_undef_symbol(char* s, int type);
void add_symbol(char* s, int type, int value);
void add_reloc(int section, int addr, int symnum, int pcrel, int length, int ext);
void print_syms();
