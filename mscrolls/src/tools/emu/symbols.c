/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2016-2018 Voidware Ltd.  All Rights Reserved.
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
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for historical reference
 * and educational purposes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include "symbols.h"
#include "hunks.h"
#include "errors.h"

extern jmp_buf command;

#define INIT		char *sp = instring;
#define GETC()		(*sp++)
#define PEEKC()		(*sp)
#define UNGETC(c)	(--sp)
#define RETURN(c)	return(c)
#define ERROR(c)	regerr()

static void regerr()
{
	printf("regular expression error\n");
	longjmp(command,1);
}


#include "regexp.h"

extern jmp_buf command;
//char *malloc();

extern int quick, verbose, debug;	/* options */

extern int n_text, n_data, n_bss;	/* defined in load.c */

int n_symbols=0;
struct sym_entry sym_table[10000];	/* should be enuf */

void dump_symbols()
{
    struct sym_entry *sp;
    int i;

    printf("Symbol Table of %d symbols\n", n_symbols);
    sp = sym_table;
    for (i = 0; i < n_symbols; ++i)
    {
        if (!strchr(sp->name, '$'))
            printf("'%s' = %04X\n", sp->name, sp->value);
        ++sp;
    }
}

char* sym_from_ad(int ad, int exact)
{
    struct sym_entry *sp;
    int i;
    static char s[100];

    //if (debug) printf("look up sym at %04X\n", ad);

    for (i=n_symbols, sp=sym_table; i; i--, sp++)
    {
        if (sp->value == ad) return sp->name;
        if (sp->value > ad)
        {
            if (!exact && sp>sym_table)
            {
                sp--;
                sprintf(s, "%s+%x", sp->name, ad-sp->value);
                return s;
            } else
                return 0;
        }
    }
    return 0;
}

int sym_to_ad(char* s)
{
    struct sym_entry *sp;
    char *p, *q;
    int i;
	char expbuf[100];
	char temp[100];

	strcpy(temp, s);
	strcat(temp, "$");
    compile(temp, expbuf, expbuf + sizeof(expbuf), '\0');
    circf = 1;		/* force a '^' at the beginning */
    for (i=n_symbols, sp=sym_table; i; i--, sp++)
    {
        for (p=sp->name, q=temp; *p; p++, q++)
            *q = isupper(*p) ? tolower(*p) : *p;
        *q = 0;
        if (estep(temp, expbuf))
            return sp->value;
    }
    printf("unrecognised symbol \"%s\"\n", s);
    longjmp(command, E_SYM);
}

char* sym_create_label(int ad)
{
    struct sym_entry *sp=sym_table+n_symbols;
    static char s[64];

    if (quick) sprintf(s, "%x", ad);
    else
    {
        sprintf(s, "L%x", ad);
        sp->name = malloc(strlen(s) + 1);
        strcpy(sp->name, s);
        sp->value = ad;
        n_symbols++;
        order_symbols();
    }
    return s;
}

void add_symbol(char* name, int value)
{
    struct sym_entry *sp = sym_table+n_symbols;

    //if (debug && !strchr(name, '$')) printf("adding symbol '%s' %04X\n", name, value);
	sp->name = name;
    sp->value = value;
    //sp->flags = flags;
    n_symbols++;
}

int strequ(char* s, char* d)
{
    char c1,c2;
    do {
        c1 = *s++;
        c2 = *d++;
        if (islower(c1)) c1 = toupper(c1);
        if (islower(c2)) c2 = toupper(c2);
        if (c1!=c2) return 0;
    }
    while ((c1!=0) && (c2!=0));
    if ((c1==0) && (c2==0)) return 1;
    else return 0;
}

static int comp(struct sym_entry* p, struct sym_entry* q)
{
    if (p->value < q->value) return(-1);
    if (p->value == q->value) return(0);
    return(1);
}

void order_symbols()
{
    if (debug) printf("sorting symbols..\n");
    qsort(sym_table, n_symbols, sizeof(struct sym_entry),
          (int (*)(const void*, const void*))comp);
}

