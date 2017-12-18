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

#define MAXSECTIONS	10

struct section
{
	char *name;		/* name of section */
	char type;
#define CODE	1	/* section contains executable code */
#define DATA	2	/* section contains initialised data */
#define BSS		3	/* section contains uninitialised data */
	inttype pc;		/* next location to assemble into */
	char *space;	/* space to store code/data */
	inttype size;	/* size of section in bytes */
	char hunk_num;	/* hunk number in output file */
	char contains_ext;	/* set when section contains external sym info */
	unsigned long load_address;	/* where this section is going to load */
};

extern struct section sections[MAXSECTIONS];
extern struct section *current_section;
extern inttype current_index;
extern inttype n_sections;
