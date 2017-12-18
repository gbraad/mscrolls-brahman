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
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "alloc.h"
#include "symbols.h"
#include "opcodes.h"
#include "tokens.h"
#include "directives.h"

struct symbol *hash_table[HASHSIZE];
struct symbol_chunk *first_chunk, *last_chunk;
inttype n_symbols=0;
extern inttype local_index;
extern inttype processor;

static int hashfn(char* s)
{
    inttype l = strlen(s);
    inttype t = l/3;
    char result[4];

    result[3] = s[0];
    result[2] = s[t];
    result[1] = s[2*t];
    result[0] = s[l-1];
    return (*(inttype*)result) % HASHSIZE;
}

struct token
{
    char *name;
    inttype token;
};

static struct token tokens[] =
{
  {	"section", D_SECTION },
  {	"rorg", D_RORG },
  {	"offset", D_OFFSET },
  {	"end", D_END },
  {	"equ", D_EQU },
  {	"equr", D_EQUR },
  {	"reg", D_REG },
  {	"set", D_SET },
  {	"dc.b", D_DC_B },
  {	"dc.w", D_DC_W },
  {	"dc.l", D_DC_L },
  {	"dc", D_DC_W },
  {	"dcb.b", D_DCB_B },
  {	"dcb.w", D_DCB_W },
  {	"dcb.l", D_DCB_L },
  {	"dcb", D_DCB_W },
  {	"ds.b", D_DS_B },
  {	"ds.w", D_DS_W },
  {	"ds.l", D_DS_L },
  {	"ds", D_DS_W },
  {	"page", D_UNIMP },
  {	"list", D_UNIMP },
  {	"nolist", D_UNIMP },
  {	"spc", D_UNIMP },
  {	"nopage", D_UNIMP },
  {	"llen", D_UNIMP },
  {	"plen", D_UNIMP },
  {	"ttl", D_UNIMP },
  {	"noobj", D_UNIMP },
  {	"fail", D_FAIL },
  {	"format", D_NOTHING },
  {	"noformat", D_NOTHING },
  {	"cnop", D_CNOP },
  {	"ifeq", D_IFEQ },
  {	"ifne", D_IFNE },
  {	"ifgt", D_IFGT },
  {	"ifge", D_IFGE },
  {	"iflt", D_IFLT },
  {	"ifle", D_IFLE },
  {	"ifc", D_IFC },
  {	"ifnc", D_IFNC },
  {	"ifd", D_IFD },
  {	"ifnd", D_IFND },
  {	"endc", D_ENDC },
  {	"else", D_ELSE },
  {	"macro", D_MACRO },
  {	"endm", D_ENDM },
  {	"mexit", D_MEXIT },
  {	"xdef", D_XDEF },
  {	"xref", D_XREF },
  {	"include", D_INCLUDE },
  {	"mask2", D_MASK2 },
  {	"idnt", D_IDNT },
  {	"current_file", D_CURRENT_FILE },
  {	"code", T_CODE },
  {	"data", T_DATA },
  {	"bss", T_BSS },
  {	"sr", T_SR },
  {	"ccr", T_CCR },
  {	"usp", T_USP },
  {	"narg", T_NARG }
};

#define N_TOKENS	(sizeof(tokens)/sizeof(tokens[0]))

struct token control_regs[] =
{
/*	"usp", 0x800,	USP has to be decoded as a separate token for move USP */
    { "vbr", 0x801 },
    { "sfc", 0x000 },
    { "dfc", 0x001 },
    { "msp", 0x803 },
    { "isp", 0x804 },
    { "cacr", 0x002 },
    { "caar", 0x802 }
};

#define N_CONTROL_REGS	(sizeof(control_regs)/sizeof(control_regs[0]))

void init_symbols()
{
    inttype i;
    struct m68k_opcode *op;
    struct token *tp;
    char opname[20];

    for (i=0; i<HASHSIZE; i++)
        hash_table[i] = 0;
    for (i=0, op=m68k_opcodes; i<numopcodes;) {
        insert_opcode(op->name, i);
        (void)strcpy(opname, op->name);
        while (i<numopcodes && !strcmp(opname, op->name)) {
            i++;
            op++;
        }
    }
    for (i=0, tp=tokens; i<N_TOKENS; i++, tp++)
        insert_token(tp->name, tp->token);
    last_chunk = first_chunk = alloc(struct symbol_chunk);
    first_chunk->n_free = CHUNKSIZE;
    first_chunk->first_free = first_chunk->syms;
    first_chunk->next = 0;
    if (processor >= 10)
        for (i=0, tp=control_regs; i<N_CONTROL_REGS; i++, tp++)
            insert_control(tp->name, tp->token);
}

void insert_opcode(char* name, inttype index)
{
    inttype i;
    struct symbol *sp;

    i = hashfn(name);
    sp = alloc(struct symbol);
    sp->next = hash_table[i];
    hash_table[i] = sp;
    sp->name = allocstr(name);
    (void)strcpy(sp->name, name);
    sp->type = S_OPCODE;
    sp->svalue = index;
}

void insert_token(char* name, inttype token)
{
    inttype i;
    struct symbol *sp;

    i = hashfn(name);
    sp = alloc(struct symbol);
    sp->next = hash_table[i];
    hash_table[i] = sp;
    sp->name = allocstr(name);
    (void)strcpy(sp->name, name);
    sp->type = S_TOKEN;
    sp->svalue = token;
}

void insert_control(char* name, inttype code)
{
    inttype i;
    struct symbol *sp;

    i = hashfn(name);
    sp = alloc(struct symbol);
    sp->next = hash_table[i];
    hash_table[i] = sp;
    sp->name = allocstr(name);
    (void)strcpy(sp->name, name);
    sp->type = S_CONTROL;
    sp->svalue = code;
}

struct symbol* find_symbol(char* s)
{
    inttype i;
    struct symbol *sp;
    char temp[100];

    if (isdigit(s[0]))
        (void)sprintf(temp, "%s%d", s, local_index);
    else
        (void)strcpy(temp, s);

    i = hashfn(temp);
    sp = hash_table[i];
    while (sp) {
        if (!strcmp(sp->name, temp)) {
            sp->flags |= F_USED;
            return sp;
        }
        sp = sp->next;
    }
    return 0;
}

/* adds a symbol to the table, initially undefined */
struct symbol* add_symbol(char* s)
{
    inttype i;
    struct symbol *sp;
    char temp[100];

    n_symbols++;
    if (isdigit(s[0]))	/* local label ? */
        (void)sprintf(temp, "%s%d", s, local_index);	/* tack on a number */
    else
        (void)strcpy(temp, s);

    i = hashfn(temp);
    sp = last_chunk->first_free++;
    if (!--last_chunk->n_free) {
        last_chunk->next = alloc(struct symbol_chunk);
        last_chunk = last_chunk->next;
        last_chunk->n_free = CHUNKSIZE;
        last_chunk->first_free = last_chunk->syms;
        last_chunk->next = 0;
    }
    sp->next = hash_table[i];
    hash_table[i] = sp;
    sp->section_index = current_index;
    sp->name = allocstr(temp);
    (void)strcpy(sp->name, temp);
    sp->type = S_UNDF;
    sp->othersym = 0;
    sp->svalue = 0;
    sp->flags = 0;
    return sp;
}

#ifdef DEBUG
void print_symbols()
{
    struct symbol_chunk *cp;
    struct symbol *sp;

    for (cp=first_chunk; cp; cp=cp->next)
        for (sp=cp->syms; sp<cp->first_free; sp++)
            printf("%s, %d, %d\n", sp->name, sp->type, sp->svalue);
}

void hash_stat()
{
    inttype i;
    inttype collisions=0;
    inttype used=0;
    struct symbol *sp;

    for (i=0; i<HASHSIZE; i++)
        if (hash_table[i]) {
            used++;
            sp = hash_table[i]->next;
            while (sp) {
                collisions++;
                sp = sp->next;
            }
        }
    printf("%d used out of %d with %d collisions\n", used, i, collisions);
}
#endif

