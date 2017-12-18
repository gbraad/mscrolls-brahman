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

#pragma once

struct reloc {
	struct reloc *next;	/* next relocation item for this section */
	inttype dest_index;	/* section to which relocation refers */
	inttype ad;			/* address in section where relocation should
						 * be performed */
};

struct ref {
	struct ref *next;	/* next bit of reloc info for this section */
	char *name;			/* name of symbol referred to */
	inttype n_refs;		/* total number of references */
	struct some_refs *refs;	/* ptr to the refs */
};

struct some_refs {
	struct some_refs *next;
	inttype n_refs;		/* number of refs in this node */
	inttype ref[20];	/* addresses of references */
};

// forward
struct symbol;

void init_reloc();
void add_reloc32(struct symbol* sym, inttype ad);
void add_reloc16(struct symbol* sym, inttype ad);
void add_reloc16An(struct symbol* sym, inttype ad);
void add_ref32(struct symbol* sym, inttype ad);
void add_ref16An(struct symbol* sym, inttype ad);
void add_ref16(struct symbol* sym, inttype ad);
void add_ref8(struct symbol* sym, inttype ad);
