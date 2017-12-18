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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "vfiles.h"
#include "alloc.h"
#include "macros.h"
#include "directives.h"
#include "tokens.h"
#include "symbols.h"
#include "getline.h"
#include "errors.h"

extern inttype current_lineno;
extern char current_filename[];
extern inttype memorytemp;
extern VFILE *vtemp;
extern FILE *ftemp;
extern inttype cond_depth;
extern inttype assembling;
inttype defining_macro=0;

char* define_macro()
{
    char *p, *q;

    p = q = malloc(MAXMACROSIZE);

    defining_macro=1;
    for (;;)
    {
        getline();
        putc('\n', ftemp);

        if (opcode_symbol && opcode_symbol->type == S_TOKEN &&
            opcode_symbol->svalue == D_ENDM) break;

        (void)strcpy(p, label_field);
        p += strlen(label_field);
        if (*opcode_field) {
            *p++ = ' ';
            (void)strcpy(p, opcode_field);
            p += strlen(opcode_field);
            if (*operand_field) {
                *p++ = ' ';
                (void)strcpy(p, operand_field);
                p += strlen(operand_field);
            }
        }
        *p++ = '\n';
    }
    *p = 0;
    defining_macro=0;
    return q;
}

static char *saved_macro_ptrs[MAXMACRODEPTH];	/* place to store macro_ptr for
                                                 * recursive macros */
inttype in_macro=0;					/* non-zero when expanding a macro */
char *macro_ptr;				/* ptr to next char from macro */
inttype n_macro_calls=0;			/* number of times any macro called */
char macro_arg[MAXMACRODEPTH][MAXMACROARGS][MAXMACROARGLEN];
inttype saved_cond_depth[MAXMACRODEPTH];
static inttype saved_n_macro_args[MAXMACRODEPTH];
inttype n_macro_args=0;

/* expand the macro symbol sp */
void expand_macro(struct symbol* sp)
{
    int c;
    char *p, *q;

    if (in_macro > MAXMACRODEPTH)
        error("macro recursion too deep");
    /* find the arguments */
    saved_n_macro_args[in_macro] = n_macro_args;
    saved_cond_depth[in_macro] = cond_depth;
    n_macro_args = 0;
    c = ',';							/* bodge */
    while (c == ',') {
        q = p = macro_arg[in_macro+1][n_macro_args++];
        c = input();					/* first char of arg */
        /* arg surrounded by <..> */
        if (c == '<') {
            /* copy whatever is inside the <..> */
            while ((c = input()) != '>') *p++ = c;
            c = input();	/* get char after '>' */
        } else {
            /* copy up to the end or next ',' */
            while (c && c != ',') {
                *p++ = c;
                c = input();
            }
        }
        *p = '\0';		/* zterm the arg */
        if (!*q) n_macro_args--;	/* ignore null arguments */
        /* by this time c will be either ',' or zero */
    }
    if (in_macro)
        saved_macro_ptrs[in_macro-1] = macro_ptr;
    in_macro++;
    macro_ptr = sp->charp;
    n_macro_calls++;
}

/* finish expanding the current macro */
void finish_macro()
{
    if (!in_macro)
        fatal_error("finish_macro called when not in a macro");

    in_macro--;
    if (!in_macro) current_lineno++;			/* randomly */
    n_macro_args = saved_n_macro_args[in_macro];
    if (in_macro)	/* restore the old macro_ptr */
        macro_ptr = saved_macro_ptrs[in_macro-1];
    if (assembling)
    {
        if (memorytemp)
            Vprintf(vtemp, " current_file \"%s\",%d\n",
                    current_filename, current_lineno);
        else
            fprintf(ftemp, " current_file \"%s\",%d\n",
                    current_filename, current_lineno);
    }
}

/* exit the current macro */
void exit_macro()
{
    if (!in_macro)
        fatal_error("exit_macro called when not in a macro");

    in_macro--;
    if (!in_macro) current_lineno++;			/* randomly */
    n_macro_args = saved_n_macro_args[in_macro];
    cond_depth = saved_cond_depth[in_macro];
    if (in_macro)	/* restore the old macro_ptr */
        macro_ptr = saved_macro_ptrs[in_macro-1];
    if (assembling)
    {
        if (memorytemp)
            Vprintf(vtemp, " current_file \"%s\",%d\n",
                    current_filename, current_lineno);
        else
            fprintf(ftemp, " current_file \"%s\",%d\n",
                    current_filename, current_lineno);
    }
}
