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
#include <sys/file.h>
#include <sys/stat.h>

#include <stdlib.h>
#include "hunks.h"
#include "load.h"
#include "load_bits.h"
#include "errors.h"
#include "symbols.h"

#if ! (defined(BSD42) || defined(AUX))
typedef unsigned char u_char;
#endif

int n_text=0, n_data=0, n_bss=0;

u_char *filebuffer=0, *fileptr;

#define getm68c()	(*fileptr++)

static int getm68l()
{
    int i, n=0;

    for (i=0; i<4; i++)
        n = 256*n + getm68c();
    return n;
}

void load(char* name, int symbols)
{
    int i, type, size;
    int code_loaded=0, data_loaded=0, bss_loaded=0;
    int n_sections, section, section_size[4];
#ifndef ARM
    struct stat buf;
#endif

    if (!filebuffer) {	/* i.e. file not already loaded */
#ifndef ARM
        stat(name, &buf);
        filebuffer = (u_char *)malloc(buf.st_size);
        i = open(name, O_RDONLY);
        if (i<0) {
            perror(name);
            exit(-1);
        }
        read(i, filebuffer, buf.st_size);
        close(i);
#else
        FILE *fp;
        filebuffer = (u_char *)malloc(128000);
        fp = fopen(name, "r");
        fread(filebuffer, 128000, 1, fp);
        fclose(fp);
#endif
    }
    fileptr = filebuffer;

    if (getm68l() != hunk_header)
        fatal_error("bad header");

    /* skip list of libraries */
    while ((i = getm68l()) != 0)
    {
        for (; i; i--) getm68l();
    }
	
    n_sections = getm68l();
    if (n_sections > 3)
        fatal_error("too many sections");
	
    /* I am only going to consider the case when all the hunks are to
     * be loaded */
    if (getm68l() != 0) fatal_error("bad header");
    if (getm68l() != n_sections-1) fatal_error("bad header");

    /* load the table of sizes */
    for (i=0; i<n_sections; i++)
        section_size[i] = getm68l();

    size = section_size[0]*4;
    printf("Size = %08x (%d dec = %dk)\n",size,size,(size+512)/1024);
	
    /* now load each section */
    for (section=0; section < n_sections; section++)
    {
        type = getm68l();		/* should be hunk_code,data or bss */
        switch (type) 
        {
        case hunk_code :
            if (code_loaded) fatal_error("too many code sections");
            code_loaded = 1;
            size = getm68l();
            if (size != section_size[section])
                fatal_error("bad code size");
            n_text = size*4;
            load_code(fileptr, size*4);		/* load the code somewhere */
            fileptr += size*4;
            break;
		
        case hunk_data :
            /* We use the data section as the word table */
            if (data_loaded) fatal_error("too many word tables");
            data_loaded = 1;
            size = getm68l();
            if (size != section_size[section])
                fatal_error("bad word table size");
            n_data = size*4;
            load_wtable(fileptr, size*4);
            fileptr += size*4;
            break;
		
        case hunk_bss :
            /* I don't really need to do this for debugging, but it
             * may be useful for debugging ST, AMIGA versions which use bss
             * sections */
            if (bss_loaded) fatal_error("too many bss sections");
            bss_loaded = 1;
            size = getm68l();
            if (size != section_size[section])
                fatal_error("bad bss section size");
            n_bss = 4*size;
            break;
		
        default :
            fatal_error("unexpected hunk %d", type);
        }
        type = getm68l();

        //		if (!symbols) return;		/* hmmmmm */

        /* now for the relocation data */
        if (type == hunk_reloc32) {
            while ((i = getm68l())) {
                getm68l();		/* ignore the hunk number */
                for (; i; i--) getm68l();	/* and the offsets */
            }
            type = getm68l();	/* the next hunk */
        }

        /* any symbols ? */
        if (type == hunk_symbol) {
            if (symbols) 	/* load the symbols ? */
                while ((i = getm68l())) {	/* read symbol name length */
                    char *p, *q;
                    p = malloc(4*i + 1);	/* space to read the name into */
                    for (q=p, i*=4; i; i--, q++)
                        *q = getm68c();		/* read the name */
                    *q = 0;					/* terminate it */
                    add_symbol(p, getm68l());	/* add to word table */
                }
            else {
                int j=0;
                while ((i = getm68l())) {
                    for (; i; i--) getm68l();	/* skip name */
                    getm68l();					/* and value */
                    j++;
                }
            }
            type = getm68l();		/* type of next bit */
        }

        if (type != hunk_end)
            fatal_error("no hunk_end in section %d", section);
    }
	
    if (symbols) order_symbols();	/* order the symbol table */
}

