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

#include <stdio.h>
#include "inttype.h"
#include "alloc.h"
#include "sections.h"
#include "expr.h"
#include "branches.h"
#include "errors.h"

extern struct section *current_section;
extern inttype current_index;
extern inttype optimise;

#ifdef DEBUG
extern inttype debug;
#endif

struct branch *first_branch=0, *last_branch=0;

void init_branches()
{
}

void add_branch(struct expr* destp, int size)
{
    struct branch *bp;

    bp = alloc(struct branch);
    bp->next = 0;
    /* add branch onto the end of branch list */
    if (last_branch) last_branch->next = bp;
    else first_branch = bp;
    last_branch = bp;
    bp->section_index = current_index;
    bp->address = current_section->pc;
    bp->dest = *destp;
    bp->size = size;
}

struct branch* find_branch()
{
    struct branch *bp;

    bp = first_branch;
    first_branch = bp->next;		/* next branch in list */
    if (!bp)
        fatal_error("branch not found");
    return bp;
}

void resolve_branches()
{
    struct branch *bp;
    struct symbol_chunk *chunk;
    struct symbol *sp;
    int offset, adjust;

    adjust = 0;			/* number of bytes to adjust branch address */
    for (bp=first_branch; bp; bp=bp->next) {
	bp->address -= adjust;
	if (optimise && bp->size == B_ANY
	    && bp->dest.sym->type == S_RELOC) {
	    offset = bp->dest.sym->svalue - bp->address-2;
	    if (offset!=2 && offset >= -128 && offset <= 127) { /* short */
#ifdef DEBUG
		if (debug)
		    printf("optimising branch to %s\n", bp->dest.sym->name);
#endif // DEBUG
		bp->size = B_SHORT;
		adjust += 2;
		/* adjust symbols */
		FOR_SYMS(chunk, sp)
		    if (sp->type == S_RELOC &&
			sp->section_index == bp->section_index &&
			sp->svalue > bp->address) {
#ifdef DEBUG
			if (debug)
			    printf("adjusting %s\n", sp->name);
#endif
			sp->svalue -= 2;
		    }
	    } else
		bp->size = B_WORD;
	} else if (!optimise && bp->size == B_ANY)
	    bp->size = B_WORD;
    }
}
