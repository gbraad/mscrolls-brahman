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
#include <stdarg.h>
#include <string.h>
#include "symbols.h"
#include "ofiles.h"
#include "errors.h"

extern int warnings;
extern char *currentfilename;

static char* lasterrorfile;

void printfiles(int section, int ad)
{
    int i;
    struct objfile *op;
    
    section &= N_TYPE;		/* mask out N_EXT bit */

    for (i=0, op=objfiles; i<n_objfiles; i++, op++)
    {
        char* errorfile = 0;
            
        switch (section)
        {
        case N_TEXT :
            if (ad >= op->code_base && ad < op->code_ceiling)
                errorfile = op->filename;
            break;
		
        case N_DATA :
            if (ad >= op->data_base && ad < op->data_ceiling)
                errorfile = op->filename;
            break;
		
        case N_BSS :
            if (ad >= op->bss_base && ad < op->bss_ceiling)
                errorfile = op->filename;
            break;
        }

        if (errorfile && (!lasterrorfile || strcmp(errorfile, lasterrorfile)))
        {
            printf(" %s", errorfile);
            lasterrorfile = errorfile;
        }
    }
}

static const char* symbol_type(int section)
{
    static char buf[32];
    
    section &= N_TYPE;		/* mask out N_EXT bit */

    if (section == N_TEXT) return "text";
    if (section == N_DATA) return "data";
    if (section == N_BSS) return "bss";
    if (section == N_ABS) return "abs";

    sprintf(buf, "type:%d", section);
    return buf;
}

void badfile(char* s, ...)
{
    va_list args;
    
    printf("bad file format in file %s:", currentfilename);

    va_start(args, s);
    vprintf(s, args);
    va_end(args);
    
    putchar('\n');
    exit(-1);
}

void doubledefinition(struct nlist* np)
{
    printf("symbol \"%s\" (%s) externally defined in both",
           np->n_un.n_name, symbol_type(np->n_type));
    
    lasterrorfile = 0;
    printfiles(np->n_type, np->n_value);
    printf(" and %s\n", currentfilename);
}

void rangeerror(char* symname, int section, int ad, int value)
{	
    printf("range error in");
    lasterrorfile = 0;
    printfiles(section, ad);
    printf(", value %d", value);
    if (*symname)
        printf(", reference was to \"%s\"\n", symname);
    else
        putchar('\n');
}

void unused(struct nlist* np)
{
    if (warnings)
	printf("Warning: global symbol \"%s\" unused (maybe)\n", np->n_un.n_name);
}

void undefined(struct nlist* np)
{
    struct reloc_item *rp;

    printf("symbol \"%s\" undefined in", np->n_un.n_name);

    lasterrorfile = 0;
    for (rp=(struct reloc_item *)np->n_value; rp; rp=rp->next)
        printfiles(rp->section, rp->info.r_address);
    
    putchar('\n');
}

