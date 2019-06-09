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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbols.h"
#include "memory.h"
#include "patch.h"
#include "errors.h"

#ifdef DEBUG
extern int debug;
#endif

struct symbol *hash_array[HASHSIZE];
struct nlist nlist_array[MAXSYMS];
int n_syms=0;
struct reloc_item reloc[MAXRELOC];	/* grr. */
int n_reloc=0;

static int symbol_hashfn(char* s)
{
    int len = strlen(s);
    return ((s[0]<<16) + (s[len/2]<<8) + s[len-1]) % HASHSIZE;
}

void init_symbols()
{
    int i;
    for (i=0; i<HASHSIZE; i++)
        hash_array[i] = 0;
}

static struct symbol *allocsymbol()
{
    void* p = malloc(sizeof(struct symbol));
    memset(p, 0, sizeof(struct symbol));
    return (struct symbol *)p;
}

void add_global_symbol(char* s, int type, int value)
{
    int hash,index;
    struct nlist *np;
    struct symbol *sp;
    struct reloc_item *rp;

    if (n_syms == MAXSYMS)
    {
	printf("out of symbol space!!!\n");
	exit(-1);
    }

#ifdef DEBUG
    if (debug) printf("adding %s\n", s);
#endif
    /* find out if it is in symbol table */
    index = find_global_symbol(s);
    if (index >= 0)
    {
	np = &nlist_array[index];
	if (np->n_type == (N_UNDF|N_EXT))
        {
            /* ahah! an undefined sym */
	    /* do relocation */
	    /* the value field of an undefined symbol is a pointer to */
	    /* the first relocation info for that symbol */
	    for (rp = (struct reloc_item *)np->n_value;
		 rp;
		 rp = rp->next)
            {
#ifdef DEBUG
                if (debug) printf("patching %s at %d\n",
                                  rp->section==N_TEXT ? "text" : "data",
                                  rp->info.r_address);
#endif
                switch (rp->info.r_length)
                {
                case 0 :
                    if (type != N_ABS && rp->info.r_pcrel)
                        /* the -1 is due to the fact that the PC
                         * to which it is relative is 1 after the
                         * actual byte in Bcc.S instructions */
                        addtobyte(s, rp->section, rp->info.r_address,
                                  value-rp->info.r_address-1);
                    else
                        addtobyte(s, rp->section, rp->info.r_address, value);
                    break;
                case 1 :
                    if (type != N_ABS && rp->info.r_pcrel)
                        patch_word(s, rp->section, rp->info.r_address,
                                   value);
                    else
                        addtoword(s, rp->section, rp->info.r_address, value);
                    break;
                case 2 :
                    if (type != N_ABS && rp->info.r_pcrel)
                        addtolong(rp->section, rp->info.r_address,
                                  value-rp->info.r_address);
                    else
                        addtolong(rp->section, rp->info.r_address, value);
                    break;
                }
                if (type == N_ABS)
                    /* don't bother to remember absolute
                     * values for relocation */
                    rp->info.r_address = -1;
            }
	    /* insert into nlist entry */
	    np->n_type = type | N_EXT;
	    np->n_value = value;
	    np->n_other = 1;	/* flag to work out whether sym referenced */
	}
        else if (np->n_type & N_EXT)	/* already a global sym ? */
	    doubledefinition(np);
    }
    else
    {
        /* not in symbol table, so add it */
        hash = symbol_hashfn(s);
        sp = allocsymbol();
        sp->next = hash_array[hash];
        hash_array[hash] = sp;
        np = sp->sym = &nlist_array[n_syms];
        np->n_un.n_name = malloc(strlen(s) + 1);
        strcpy(np->n_un.n_name, s);
        np->n_type = type | N_EXT;
        np->n_value = value;
        np->n_other = 0;	/* flag to work out whether sym referenced */
        n_syms++;
    }
}

int find_global_symbol(char* s)
{
    struct symbol *sp;
    int hash;

    hash = symbol_hashfn(s);
    sp = hash_array[hash];
    while (sp)
    {
        if (sp->sym->n_type&N_EXT && !strcmp(s, sp->sym->n_un.n_name))
            return sp->sym - nlist_array;
        sp = sp->next;
    }
    return -1;
}

int add_undef_symbol(char* s, int type)
{
    struct nlist *np;
    struct symbol *sp;
    int hash;

    hash = symbol_hashfn(s);
    sp = allocsymbol();
    sp->next = hash_array[hash];
    hash_array[hash] = sp;
    np = sp->sym = &nlist_array[n_syms];
    np->n_un.n_name = malloc(strlen(s)+1);
    strcpy(np->n_un.n_name, s);
    np->n_type = N_UNDF | N_EXT;
    /* fix value field to point to first relocation info */
    np->n_value = 0;
    n_syms++;
    return n_syms-1;
}

void add_symbol(char* s, int type, int value)
{
    struct nlist *np;
    struct symbol *sp;
    int hash;

    hash = symbol_hashfn(s);
    sp = allocsymbol();
    sp->next = hash_array[hash];
    hash_array[hash] = sp;
    np = sp->sym = &nlist_array[n_syms];
    np->n_un.n_name = malloc(strlen(s)+1);
    strcpy(np->n_un.n_name, s);
    np->n_type = type;
    np->n_value = value;
    n_syms++;
}

void add_reloc(int section, int addr, int symnum, int pcrel, int length, int ext)
{
    struct reloc_item *rp;
    struct nlist *np;

    if (n_reloc == MAXRELOC)
    {
        printf("out of reloc space\n");
        exit(-1);
    }
    rp = &reloc[n_reloc];
    rp->section = section;
    rp->info.r_address = addr;
    rp->info.r_symbolnum = symnum;
    rp->info.r_pcrel = pcrel;
    rp->info.r_length = length;
    rp->info.r_extern = ext;
    if (ext)
    {
        /* fix linked list of relocation info for symbol */
        np = &nlist_array[symnum];
        if (np->n_type == (N_UNDF|N_EXT))
        {
            rp->next = (struct reloc_item *)np->n_value;
            np->n_value = (unsigned long)rp;
        }
    }
    n_reloc++;
}


void print_syms()
{
    int i;
    struct nlist *np;

    for (i=0, np=nlist_array; i<n_syms; i++, np++)
        printf("%-20s=%08x\n", np->n_un.n_name, (unsigned int)np->n_value);
}


