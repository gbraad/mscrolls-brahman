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

#ifdef BSD42
#include <sys/file.h>
#include <sys/types.h>
#else
#include <fcntl.h>
#ifdef AUX
#include <sys/types.h>
#endif
#endif

#include <sys/stat.h>
#include <string.h>
#include "ofiles.h"
#include "symbols.h"
#include "memory.h"
#include "hunks.h"
#include "errors.h"
#include "patch.h"

#ifndef _MSC_VER
#include <unistd.h>
#endif

typedef unsigned char u_char;

#define getch()		(fileptr==fileend?EOF:*fileptr++)

struct objfile objfiles[MAXOBJFILES];
int n_objfiles;
char *currentfilename;
char *code_name=0, *data_name=0, *bss_name=0;
u_char *filebuf, *fileptr, *fileend;
extern int code_load_base;

static int getlong()
{
    int i, n;

    n = 0;
    for (i=0; i<4; i++) {
        n <<= 8;
        n += getch();
    }
    return n;
}

void linkit(char* filename)
{
    int n, len, hunk, count, offset;
    char *p;
    struct nlist *np;
    struct objfile *op;
    int fd;
    struct stat stbuf;
    int value, index;
    int base = 0; /* base address of block being loaded */
    int type = 0; /* type of block being loaded */
    char s[100];
    char name[100];
    int hunk_number=0;	/* number of current hunk in current program unit */
    int flags;
    
    currentfilename = filename;
    if (n_objfiles == MAXOBJFILES)
    {
	printf("Too many object files!!\n");
	exit(-1);
    }
    
    op = &objfiles[n_objfiles++];
    op->filename = malloc(strlen(filename)+1);
    strcpy(op->filename, filename);
    op->code_base = code_address_base;
    op->data_base = data_address_base;
    op->bss_base = bss_address_base;
    op->code_ceiling = 0x7fffffff;
    op->data_ceiling = 0x7fffffff;
    op->bss_ceiling = 0x7fffffff;
    
    if (stat(filename, &stbuf)<0)
    {
	perror(filename);
	exit(-1);
    }
    if (!stbuf.st_size)
    {
	badfile("zero length file");
    }

    flags = O_RDONLY;

#ifdef _WIN32
    flags |= O_BINARY;
#endif

    fd = open(filename, flags);
    if (fd<0)
    {
	perror(filename);
	exit(-1);
    }
    filebuf = (u_char *)malloc(stbuf.st_size);
    if (read(fd, filebuf, stbuf.st_size) != stbuf.st_size)
    {
	printf("bad read on %s\n", filename);
	exit(-1);
    }
    close(fd);
    fileptr = filebuf;
    fileend = filebuf + stbuf.st_size;
    
    do {
	hunk = getlong();
	/* handle multiple program units in an object file */
	if (hunk == hunk_unit) {
	    /*badfile("no hunk_unit");*/
	    len = 4*getlong();
	    for (; len; len--) getch();	/* advance to end of hunk */
	    hunk_number = 0;
	    hunk = getlong();
	}
	
	*name = '\0';
	/* check for hunk_name */
	if (hunk == hunk_name) {
	    len = 4*getlong();		/* get string length */
	    for (p=name; len; len--)	/* read name */
		*p++ = getch();
	    *p=0;					/* zero terminate */
	    hunk = getlong();		/* read type of next hunk */
	} else
	    
	    /* check for relocatable block */
	    type = 0;
	if (hunk == hunk_code) {
	    len = 4*getlong();		/* get code length */
	    base = code_address_base;
	    type = N_TEXT;			/* type of this reloc block */
	    loadcode(len);
	    hunk = getlong();
	} else if (hunk == hunk_code_word) {	/* word aligned code section */
	    len = 2*getlong();		/* code length */
	    base = code_address_base;
	    type = N_TEXT;
	    loadcode(len);
	    hunk = getlong();
	} else if (hunk == hunk_data) {
	    len = 4*getlong();		/* get data length */
	    base = data_address_base;
	    type = N_DATA;			/* type of this reloc block */
	    loaddata(len);
	    hunk = getlong();
	} else if (hunk == hunk_bss) {
	    base = bss_address_base;
	    type = N_BSS;
	    /* remember how much bss allocated */
	    bss_address_base += 4*getlong();
	    hunk = getlong();
	}
	if (type == 0) badfile("no code, data or bss block");
	
	switch (type)
        {
	case N_TEXT :
	    if (!code_name) {
		code_name = malloc(strlen(name)+1);
		strcpy(code_name, name);
	    } else
		/* check that all code sections have same name */
		if (strcmp(name, code_name))
		    badfile("multiple code sections (use alink...)");
	    break;
	    
	case N_DATA :
	    if (!data_name) {
		data_name = malloc(strlen(name)+1);
		strcpy(data_name, name);
	    } else
		/* check that all data sections have same name */
		if (strcmp(name, data_name))
		    badfile("multiple data sections (use alink...)");
	    break;
	    
	case N_BSS :
	    if (!bss_name) {
		bss_name = malloc(strlen(name)+1);
		strcpy(bss_name, name);
	    } else
		/* check that all bss sections have same name */
		if (strcmp(name, bss_name))
		    badfile("multiple bss sections (use alink...)");
	    break;
	}
	
	/* handle any relocation information */
	while (hunk == hunk_reloc32 ||
	       hunk == hunk_reloc16 ||
	       hunk == hunk_reloc16An ||
	       hunk == hunk_reloc8) {
            if (type == N_BSS)
                badfile("attempted to relocate value in bss section");
            switch (hunk) {
            case hunk_reloc32 :
                while ((n = getlong()) != 0)
                {
                    /* get no offsets */
                    if (getlong() != hunk_number)
                        badfile("relocation info too vicious");
                    for (; n; n--) {
                        offset = getlong();
                        addtolong(type, base + offset, base);
                    }
                }
                break;
		       
                /* if reference is to the current hunk, then the relocation
                 * refers to 16 bit absolute data */
            case hunk_reloc16 :
                while ((n = getlong()) != 0)
                {
                    /* get no offsets */
                    if (getlong() != hunk_number)
                        badfile("relocation info too vicious");
                    for (; n; n--) {
                        offset = getlong();
                        addtoword("", type, base + offset, base);
                    }
                }
                break;
		       
                /* 16 bit offset from the start of the section.
                 * the assembler produces this for fred(An) when fred
                 * is external */
            case hunk_reloc16An :
                while ((n = getlong()) != 0)
                {
                    /* get no offsets */
                    if (getlong() != hunk_number)
                        badfile("relocation info too vicious");
                    for (; n; n--) {
                        offset = getlong();
                        addtoword("", type, base + offset, base);
                    }
                }
                break;
		       
            case hunk_reloc8 :
                badfile("relocation info too vicious");
            }
            hunk = getlong();
        }
	
	/* external symbol info block */
	if (hunk == hunk_ext)
        {
	    while ((len = getlong()) != 0)
            {
		n = (len>>24) & 0xff;	/* type of ext sym info */
		len &= 0xffffff;
		/* read symbol name */
		for (p=s, len *= 4; len; len--)
		    *p++ = getch();
		*p = '\0';			/* zero terminate string */
		switch (n) {
		case ext_def :
		    /* add to global symbol table & patch undefineds */
		    value = getlong();
		    add_global_symbol(s, type, value+base);
		    break;
		case ext_abs :
		    value = getlong();
		    add_global_symbol(s, N_ABS, value);
		    break;
		case ext_ref32 :
		    /* 32 bit abs refs to a symbol undefined in this file */
		    if (type == N_BSS)
			badfile(
				"tried to resolve external reference in bss section");
		    count = getlong();
		    /* look it up in the symbol table */
		    index = find_global_symbol(s);
		    if (index == -1)
			/* not there, so add it as an undefined sym */
			index = add_undef_symbol(s, type);
		    /* point to the nlist entry for the symbol */
		    np = &nlist_array[index];
		    np->n_other = 1;	/* flag to show sym has been referenced */
		    if (np->n_type != (N_UNDF|N_EXT))
			/* symbol exists, so patch its value into the
			 * specified places in this relocatable block */
			for (; count; count--) {
			    offset = getlong();
			    addtolong(type, base+offset, np->n_value);
			    /* if a relocatable symbol, add it to relocation
			     * information */
			    if (np->n_type != (N_ABS|N_EXT))
				add_reloc(type, base+offset, index, 0, 2, 1);
			}
		    else
			/* symbol is still undefined, so add relocation info */
			for (; count; count--)
			    add_reloc(type, base+getlong(), index, 0, 2, 1);
		    break;
		    
		case ext_ref16 :
		    /* 16 bit pc relative refs to a symbol undefined in this file */
		    if (type == N_BSS)
			badfile(
                                "tried to resolve external reference in bss section");
		    count = getlong();
		    /* look it up in the symbol table */
		    index = find_global_symbol(s);
		    if (index == -1)
			/* not there, so add it as an undefined sym */
			index = add_undef_symbol(s, type);
		    /* point to the nlist entry for the symbol */
		    np = &nlist_array[index];
		    np->n_other = 1;	/* flag to show sym has been referenced */
		    if (np->n_type != (N_UNDF|N_EXT))
			/* symbol exists, so patch its value into the
			 * specified places in this relocatable block */
			for (; count; count--) {
			    offset = getlong();
			    /* absolute symbols should not be done pc
			     * relative */
			    if (np->n_type != (N_ABS|N_EXT))
				/* don't bother to keep relocation info
				 * since isn't needed */
				patch_word(np->n_un.n_name, type,
					   base+offset,
					   np->n_value);
			    else
				addtoword(np->n_un.n_name, type, base+offset,
					  np->n_value);
			}
		    else
			/* symbol is still undefined, so add relocation info */
			for (; count; count--)
			    add_reloc(type, base+getlong(), index, 1, 1, 1);
		    break;
		    
		case ext_ref16An :
		    /* 16 bit abs refs to a symbol undefined in this file */
		    if (type == N_BSS)
			badfile(
				"tried to resolve external reference in bss section");
		    count = getlong();
		    /* look it up in the symbol table */
		    index = find_global_symbol(s);
		    if (index == -1)
			/* not there, so add it as an undefined sym */
			index = add_undef_symbol(s, type);
		    /* point to the nlist entry for the symbol */
		    np = &nlist_array[index];
		    np->n_other = 1;	/* flag to show sym has been referenced */
		    if (np->n_type != (N_UNDF|N_EXT))
			/* symbol exists, so patch its value into the
			 * specified places in this relocatable block */
			for (; count; count--) {
			    offset = getlong();
			    addtoword(np->n_un.n_name, type, base+offset,
				      np->n_value);
			    /* if a relocatable symbol, add it to relocation
			     * information */
			    if (np->n_type != (N_ABS|N_EXT))
				add_reloc(type, base+offset, index, 0, 1, 1);
			}
		    else
			/* symbol is still undefined, so add relocation info */
			for (; count; count--)
			    add_reloc(type, base+getlong(), index, 0, 1, 1);
		    break;
		    
		case ext_ref8 :
		    /* 8 bit pc relative refs to a symbol undefined in this file */
		    if (type == N_BSS)
			badfile(
				"tried to resolve external reference in bss section");
		    count = getlong();
		    /* look it up in the symbol table */
		    index = find_global_symbol(s);
		    if (index == -1)
			/* not there, so add it as an undefined sym */
			index = add_undef_symbol(s, type);
		    /* point to the nlist entry for the symbol */
		    np = &nlist_array[index];
		    np->n_other = 1;	/* flag to show sym has been referenced */
		    if (np->n_type != (N_UNDF|N_EXT))
			/* symbol exists, so patch its value into the
			 * specified places in this relocatable block */
			for (; count; count--) {
			    offset = getlong();
			    /* absolute symbols should not be done pc
			     * relative */
			    if (np->n_type != (N_ABS|N_EXT))
				/* don't bother to keep relocation info
				 * since isn't needed 
				 * the -1 is due to the fact that the PC
				 * to which it is relative is 1 after the
				 * actual byte in Bcc.S instructions */
				addtobyte(np->n_un.n_name, type, base+offset,
					  np->n_value - (base+offset) - 1);
			    else
				addtobyte(np->n_un.n_name, type, base+offset, np->n_value);
			}
		    else
			/* symbol is still undefined, so add relocation info */
			for (; count; count--)
			    add_reloc(type, base+getlong(), index, 1, 0, 1);
                    break;
                }
            }
            hunk = getlong();	/* read type of next hunk */
        }

        if (hunk == hunk_symbol)
        {
            /* AAAAAAAAAAGH == !!! */
            while ((len = getlong()) != 0)
            {
                len *= 4;
                for (p=s; len; len--) *p++ = getch();
                *p = '\0';	/* zero terminate */
                value = getlong();
                /* have we already added this as a global */
                index = find_global_symbol(s);
                if (index<0 || nlist_array[index].n_value!=base+value)
                    add_symbol(s, type, base+value);
            }
            hunk = getlong();
        }

        if (hunk != hunk_end)
            badfile("unexpected hunk %d", hunk);
		
    } while (fileptr < fileend);

    op->code_ceiling = code_address_base;
    op->data_ceiling = data_address_base;
    op->bss_ceiling = bss_address_base;

    free(filebuf);
}


