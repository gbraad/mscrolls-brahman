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
 */

/* This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include "inttype.h"
#include <string.h>
#include <stdio.h>
#include "alloc.h"
#include "reloc.h"
#include "sections.h"
#include "symbols.h"
#include "errors.h"

#ifdef DEBUG
extern inttype debug;
#endif

extern inttype pass;

struct reloc *reloc32[MAXSECTIONS];
struct reloc *reloc16[MAXSECTIONS];
struct reloc *reloc16An[MAXSECTIONS];
struct ref *ref32[MAXSECTIONS];
struct ref *ref16An[MAXSECTIONS];
struct ref *ref16[MAXSECTIONS];
struct ref *ref8[MAXSECTIONS];

void init_reloc()
{
    inttype i;

    for (i=0; i<MAXSECTIONS; i++)
    {
        reloc32[i] = 0;
        reloc16An[i] = 0;
        ref32[i] = 0;
        ref16An[i] = 0;
        ref16[i] = 0;
        ref8[i] = 0;
    }
}

static void add_reloc(struct symbol* sym, inttype ad, struct reloc** reloc)
{
    struct reloc *rp = alloc(struct reloc);

    if (pass==1)
        fatal_error("add_reloc called in 1st pass");
#ifdef DEBUG
    if (debug)
        printf("adding reloc at %08x in %d\n", ad, current_index);
#endif
    rp->dest_index = sym->section_index;
    rp->ad = ad;
    rp->next = reloc[current_index];
    reloc[current_index] = rp;
}

void add_reloc32(struct symbol* sym, inttype ad)
{
    add_reloc(sym, ad, reloc32);
}

void add_reloc16(struct symbol* sym, inttype ad)
{
    add_reloc(sym, ad, reloc16);
}

void add_reloc16An(struct symbol* sym, inttype ad)
{
    add_reloc(sym, ad, reloc16An);
}

static void add_ref(struct symbol* sym, inttype ad, struct ref** ref_array)
{
    struct ref *rp = ref_array[current_index];
    struct some_refs *refs;

    if (pass==1)
        fatal_error("add_ref called in 1st pass");
#ifdef DEBUG
    if (debug)
        printf("adding ref to %s at %08x in %d\n",
               sym->name, ad, current_index);
#endif
    while (rp) {
        if (!strcmp(sym->name, rp->name)) break;
        rp = rp->next;
    }
    if (rp) {	/* ref to this sym already exists so add to it */
        rp->n_refs++;
        refs = rp->refs;
        refs->ref[refs->n_refs++] = ad;
        /* if current node full, add a new one onto beginning of list */
        if (refs->n_refs == 20) {
            rp->refs = alloc(struct some_refs);
            rp->refs->n_refs = 0;
            rp->refs->next = refs;
        }
    } else {	/* no ref to this sym already */
        rp = alloc(struct ref);
        rp->next = ref_array[current_index];
        ref_array[current_index] = rp;
        rp->name = allocstr(sym->name);
        (void)strcpy(rp->name, sym->name);
        rp->n_refs = 1;
        rp->refs = alloc(struct some_refs);
        rp->refs->next = 0;
        rp->refs->n_refs = 1;
        rp->refs->ref[0] = ad;
    }
}


void add_ref32(struct symbol* sym, inttype ad)
{
    add_ref(sym, ad, ref32);
}

void add_ref16An(struct symbol* sym, inttype ad)
{
    add_ref(sym, ad, ref16An);
}

void add_ref16(struct symbol* sym, inttype ad)
{
    add_ref(sym, ad, ref16);
}

void add_ref8(struct symbol* sym, inttype ad)
{
    add_ref(sym, ad, ref8);
}

