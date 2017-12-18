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

#include "sections.h"

#define HASHSIZE	10007
#define CHUNKSIZE	100
#define MAXLABELLEN	255

struct symbol {
	struct symbol *next;
	inttype section_index;	/* section in which symbol is defined */
	char *name;
	union {
		inttype Value;
		char *Charp;
	} sU;
#define svalue	sU.Value
#define charp	sU.Charp
	struct symbol *othersym;/* if value is a reference to another symbol
				 * e.g. type1 EQU TYPE+1 */
	char type;
#define S_UNDF	1		/* symbol is undefined */
#define S_ABS	2		/* symbol is an absolute value */
#define S_RELOC	3		/* symbol is an address */
#define S_XREF	4
#define S_OPCODE 5		/* an opcode */
#define S_TOKEN 6		/* other tokens */
#define S_DREG	7		/* EQUR'ed to a data register */
#define S_AREG	8		/* EQUR'ed to an address register */
#define S_NOTHING 9		/* S_AREG,S_DREG get set to this at start of
						 * second pass */
#define S_MACRO	10		/* value field is a char ptr to the macro */
#define S_REG	11		/* register mask */
#define S_CONTROL 12	/* control reg for movec, value field is ctrl num */
	char flags;			/* various flags */
#define F_SET	1		/* set if symbol defined by SET */
#define F_EXTERNAL	2	/* set if symbol has been XDEF'ed */
#define F_USED	4		/* set if symbol has been used */
};

struct symbol_chunk
{
	struct symbol_chunk *next;
	inttype n_free;			/* number of free syms in this chunk */
	struct symbol *first_free;	/* ptr to 1st free sym */
	struct symbol syms[CHUNKSIZE];
};

extern struct symbol_chunk *first_chunk, *last_chunk;

extern struct symbol *hash_table[];
extern inttype n_symbols;

/* macro to step through symbol table */
#define FOR_SYMS(cp, sp) \
	for (cp=first_chunk; cp; cp=cp->next) \
		for (sp=cp->syms; sp<cp->first_free; sp++)


void init_symbols();
void insert_opcode(char* name, inttype index);
void insert_token(char* name, inttype token);
void insert_control(char* name, inttype code);
struct symbol* find_symbol(char* s);
struct symbol* add_symbol(char* s);

void print_symbols();
void hash_stat();
