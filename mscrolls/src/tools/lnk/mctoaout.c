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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "a.out.h"
#include "hunks.h"

extern void *malloc();
char *make_object_name();

static struct exec header = {
	OMAGIC,					/* a_magic */
	0,						/* a_text */
	0,						/* a_data */
	0,						/* a_bss */
	0,						/* a_syms */
	0,						/* a_entry */
	0,						/* a_trsize */
	0,						/* a_drsize */
};
int done_code = 0, done_data=0, done_bss=0;
int code_hunk = -1, data_hunk = -1, bss_hunk = -1;
char *buffered_code=0, *buffered_data = 0;

/* swapping */

static void swap(void* ptr, int size)
{
    char* p = (char*)ptr;
    char t;
    if (size == 1) return;
    if (size == 2) {
        t = p[0];
        p[0] = p[1];
        p[1] = t;
    } else {
        t = p[0];
        p[0] = p[3];
        p[3] = t;
        t = p[1];
        p[1] = p[2];
        p[2] = t;
    }
}

void swap_nlist(struct nlist* np)
{
    swap(&np->n_un.n_strx, 4);
    swap(&np->n_desc, 2);		/* not really needed */
    swap(&np->n_value, 4);
}

void swap_header(struct exec* ep)
{
    swap(&ep->a_magic, 4);
    swap(&ep->a_text, 4);
    swap(&ep->a_data, 4);
    swap(&ep->a_bss, 4);
    swap(&ep->a_syms, 4);
    swap(&ep->a_entry, 4);
    swap(&ep->a_trsize, 4);
    swap(&ep->a_drsize, 4);
}


int getlong(FILE* fp)
{
    int a,b,c,d;
    a=getc(fp);
    b=getc(fp);
    c=getc(fp);
    d=getc(fp);
    if (feof(fp)) return EOF;
    return (a<<24)+(b<<16)+(c<<8)+d;
}

int getstring(char* s, FILE* fp)
{
    int i, len=getlong(fp);
    for (i=len*4; i; i--)
        *s++ = getc(fp);
    *s=0;
    return len;
}

int getstring2(int len, char* s, FILE* fp)
{
    int i;
    for (i=len*4; i; i--)
        *s++ = getc(fp);
    *s=0;
    return len;
}

/* relocation */

#define alloc(t)		(t *)malloc(sizeof(t))

struct reloc
{
    struct reloc *next;
    int hunk_number;
    struct relocation_info r;
} *first_reloc = NULL;

void add_reloc(int hunk, int address, int symnum, int pcrel, int length, int ext)
{
    struct relocation_info *rp;
    struct reloc *r;
    r = alloc(struct reloc);
    r->next = first_reloc;
    first_reloc = r;
    r->hunk_number = hunk;
    rp = &r->r;
    rp->r_address = address;
    rp->r_symbolnum = symnum;
    rp->r_pcrel = pcrel;
    rp->r_length = length;
    rp->r_extern = ext;
}

void fix_hunk_reloc_addresses(int hunk, int to_add)
{
    struct reloc *r;
    for (r = first_reloc; r; r = r->next)
        if (r->hunk_number == hunk)
            r->r.r_address += to_add;
}

void fix_internal_reloc_sections(int c_hunk, int d_hunk)
{
    struct reloc *r;
    for (r = first_reloc; r; r = r->next)
        if (r->r.r_extern == 0 && r->hunk_number == c_hunk)
            r->r.r_symbolnum = N_TEXT;
        else if (r->hunk_number == d_hunk)
            r->r.r_symbolnum = N_DATA;
}

int count_hunk_reloc(int hunk)
{
    int n = 0;
    struct reloc *r;
    for (r = first_reloc; r; r = r->next)
        if (r->hunk_number == hunk) n++;
    return n;
}

void write_hunk_reloc(int hunk, FILE* fp)
{
    struct reloc *r;
    for (r = first_reloc; r; r = r->next)
        if (r->hunk_number == hunk)
            fwrite(&r->r, 1, sizeof(r->r), fp);
}

void purge_reloc()
{
    struct reloc *r, *next_r;
    for (r = first_reloc; r; r = next_r) {
        next_r = r->next;
        free(r);
    }
    first_reloc = NULL;
}

/* symbols */
struct symbol
{
    struct symbol *next;
    char *name;
    struct nlist nl;
} *first_symbol, *last_symbol;

int string_offset = 4;			/* first string table offset */
int symbol_number = 0;			/* first symbol index */

int add_symbol(char* name, int type, int value)
{
    struct symbol *sym = alloc(struct symbol);
    int len = strlen(name);
    sym->name = malloc(len + 1);
    strcpy(sym->name, name);
    sym->nl.n_un.n_strx = string_offset;
    string_offset += len + 1;
    sym->nl.n_type = type;
    sym->nl.n_other = 0;
    sym->nl.n_desc = 0;
    sym->nl.n_value = value;
    sym->next = NULL;
    /* I chain the symbols in order of this routine being called
       so that the symbol indexes used in the relocation are correct */
    if (first_symbol == NULL)
        first_symbol = last_symbol = sym;
    else {
        last_symbol->next = sym;
        last_symbol = sym;
    }
    symbol_number++;
    return symbol_number-1;
}

void add_to_symbols_in(int section, int offset)
{
    struct symbol *sym;
    for (sym = first_symbol; sym; sym = sym->next)
        if ((sym->nl.n_type & N_TYPE) == section)
            sym->nl.n_value += offset;
}

int count_symbols()
{
    return symbol_number;
}

void write_symbols(FILE* fp)
{
    char *string_table, *next_string_table;
    struct symbol *sym;

    string_table = malloc(string_offset);
    *(int *)string_table = string_offset; /* size of string table */
    next_string_table = string_table + 4;
    for (sym = first_symbol; sym; sym = sym->next) {
        strcpy(next_string_table, sym->name);
        next_string_table += strlen(sym->name)+1;
        swap_nlist(&sym->nl);
        fwrite(&sym->nl, 1, sizeof(sym->nl), fp);
    }
    swap(string_table, 4);
    fwrite(string_table, 1, string_offset, fp);
    free(string_table);
    string_offset = 4;			/* back to the start */
    symbol_number = 0;
}

void purge_symbols()
{
    struct symbol *sym, *next_sym;
    for (sym = first_symbol; sym; sym = next_sym) {
        next_sym = sym->next;
        free(sym->name);
        free(sym);
    }
    first_symbol = last_symbol = NULL;
}

/* makes a name for the object file from the first external text
   symbol in the file */

char *make_object_name()
{
    struct symbol *sym;
    static char name[256];
    name[0] = '\0';
    for (sym = first_symbol; sym; sym = sym->next)
        if (sym->nl.n_type == (N_TEXT|N_EXT)) {
            strcpy(name, sym->name[0] == '_' ? sym->name + 1 : sym->name);
            strcat(name, ".o");
            if (access(name, F_OK) == -1) break; /* unique? */
        }
    for (sym = first_symbol; sym; sym = sym->next)
        if (sym->nl.n_type == (N_DATA|N_EXT)) {
            strcpy(name, sym->name[0] == '_' ? sym->name + 1 : sym->name);
            strcat(name, ".o");
            if (access(name, F_OK) == -1) break; /* unique? */
        }
    for (sym = first_symbol; sym; sym = sym->next)
        if (sym->nl.n_type == (N_BSS|N_EXT)) {
            strcpy(name, sym->name[0] == '_' ? sym->name + 1 : sym->name);
            strcat(name, ".o");
            if (access(name, F_OK) == -1) break; /* unique? */
        }
    for (sym = first_symbol; sym; sym = sym->next)
        if (sym->nl.n_type == (N_ABS|N_EXT)) {
            strcpy(name, sym->name[0] == '_' ? sym->name + 1 : sym->name);
            strcat(name, ".o");
            if (access(name, F_OK) == -1) break; /* unique? */
        }
    if (name[0] == '\0') {		/* last resort */
        strcpy(name, "unknownXXXXXX");
        mktemp(name);
    }
    return name;
}

void output_object()
{
    FILE *out;
    char *object_name = make_object_name();
    printf("writing '%s'...\n", object_name);
    /* fix the offsets for data relocation items to include the
       size of the text section */
    fix_hunk_reloc_addresses(data_hunk, header.a_text);
    /* now that we know the hunk numbers, we can fix the sections in
       the internal relocation */
    fix_internal_reloc_sections(code_hunk, data_hunk);
    /* change the values of symbols in the data and bss sections */
    add_to_symbols_in(N_DATA, header.a_text);
    add_to_symbols_in(N_BSS, header.a_text + header.a_data);
    out = fopen(object_name, "wb");
    header.a_syms = sizeof(struct nlist) * count_symbols();
    header.a_trsize = sizeof(struct relocation_info) *
        count_hunk_reloc(code_hunk);
    header.a_drsize = sizeof(struct relocation_info) *
        count_hunk_reloc(data_hunk);
    swap_header(&header);
    fwrite(&header, 1, sizeof(header), out);
    swap_header(&header);
    if (buffered_code) {
        fwrite(buffered_code, 1, header.a_text, out);
        free(buffered_code); buffered_code = 0;
    }
    if (buffered_data) {
        fwrite(buffered_data, 1, header.a_data, out);
        free(buffered_data); buffered_data = 0;
    }
    write_hunk_reloc(code_hunk, out);
    write_hunk_reloc(data_hunk, out);
    purge_reloc();
    write_symbols(out);
    purge_symbols();
    header.a_text = header.a_data = header.a_syms = header.a_trsize =
        header.a_drsize = 0;
    fclose(out);
}

int main(int argc, char** argv)
{
    int i, n, type;
    int f,l;
    FILE *fp;
    int hunk_number = 0;
    int first_hunk_unit = 1;
    int current_section = 0;		/* N_TEXT,N_DATA,N_BSS */
    char s[256];

    if (argc != 2) {
        fprintf(stderr,"Usage: mctoaout infile\n");
        exit(-1);
    }
    fp = fopen(argv[1], "rb");
    if (!fp) {
        perror(argv[1]);
        exit(-1);
    }
    while ((n = getlong(fp)) != EOF)
        switch (n) {	/* hunk type */
        case hunk_unit :
            if (!first_hunk_unit)
                output_object();
            first_hunk_unit = 0;
            getstring(s, fp);
            hunk_number = 0;
            break;
        case hunk_name :
            getstring(s, fp);
            break;
        case hunk_code :
            i = getlong(fp);
            i *= 4;
            goto copy_code_common;
        case hunk_code_word :
            i = getlong(fp);
            i *= 2;
        copy_code_common:
            header.a_text = i;
            buffered_code = malloc(i);
            fread(buffered_code, 1, i, fp);
            done_code = 1;
            code_hunk = hunk_number;
            current_section = N_TEXT;
            break;
        case hunk_data :
            i = getlong(fp);
            i *= 4;
            header.a_data = i;
            buffered_data = malloc(i);
            fread(buffered_data, 1, i, fp);
            done_data = 1;
            data_hunk = hunk_number;
            current_section = N_DATA;
            break;
        case hunk_bss :
            i = getlong(fp);
            header.a_bss = i*4;
            done_bss = 1;
            bss_hunk = 1;
            current_section = N_BSS;
            break;
        case hunk_reloc32 :
            while ((i=getlong(fp)) != 0)
            {
                n=getlong(fp);
                for (; i; i--)
                    add_reloc(hunk_number, getlong(fp), n, 0, 2, 0);
            }
            break;
        case hunk_reloc16An :
            while ((i=getlong(fp)) != 0)
            {
                n=getlong(fp);
                for (; i; i--)
                    add_reloc(hunk_number, getlong(fp), n, 0, 1, 0);
            }
            break;
        case hunk_reloc16 :
            while ((i=getlong(fp)) != 0)
            {
                n=getlong(fp);
                for (; i; i--)
                    add_reloc(hunk_number, getlong(fp), n, 1, 1, 0);
            }
            break;
        case hunk_reloc8 :
            while ((i=getlong(fp)) != 0)
            {
                n=getlong(fp);
                for (; i; i--)
                    add_reloc(hunk_number, getlong(fp), n, 1, 0, 0);
            }
            break;
        case hunk_ext :
            while ((i=getlong(fp)) != 0)
            {
                type = (i>>24)&0xff;
                i &= 0xffffff;
                getstring2(i, s, fp);
                switch (type)
                {
                case ext_symb :
                    break;
                case ext_def :
                    add_symbol(s, current_section|N_EXT, getlong(fp));
                    break;
                case ext_abs :
                    add_symbol(s, N_ABS|N_EXT, getlong(fp));
                    break;
                case ext_res :
                    printf("\trelocatable library definition %s=%08x\n",
                           s, getlong(fp));
                    break;
                case ext_common :
                    printf("\tcommon reference to %s size %d, %d refs\n",
                           s, getlong(fp), i=getlong(fp));
                    for (; i; i--) getlong(fp);
                    break;
                case ext_ref32 :
                    i = getlong(fp);
                    n = add_symbol(s, N_UNDF|N_EXT, 0);
                    for (; i; i--)
                        add_reloc(hunk_number, getlong(fp), n, 0, 2, 1);
                    break;
                case ext_ref16 :
                    i = getlong(fp);
                    n = add_symbol(s, N_UNDF|N_EXT, 0);
                    for (; i; i--)
                        add_reloc(hunk_number, getlong(fp), n, 0, 1, 1);
                    break;
                case ext_ref16An :
                    i = getlong(fp);
                    n = add_symbol(s, N_UNDF|N_EXT, 0);
                    for (; i; i--)
                        add_reloc(hunk_number, getlong(fp), n, 1, 1, 1);
                    break;
                case ext_ref8 :
                    i = getlong(fp);
                    n = add_symbol(s, N_UNDF|N_EXT, 0);
                    for (; i; i--)
                        add_reloc(hunk_number, getlong(fp), n, 1, 0, 1);
                    break;
                }
            }
            break;
        case hunk_symbol :
            while (getstring(s, fp))
                add_symbol(s, current_section, getlong(fp));
            break;
        case hunk_end :
            hunk_number++;
            break;
        case hunk_header :
            printf("hunk header :\n");
            printf("\tname list :\n");
            while (getstring(s, fp))
                printf("\t\t%s\n", s);
            printf("\ttable size %08x\n", getlong(fp));
            printf("\tfirst hunk %d\n", f=getlong(fp));
            printf("\tlast hunk %d\n", l=getlong(fp));
            printf("\thunk sizes :\n");
            for (i=f; i<=l; i++)
                printf("\t\t%08x\n", getlong(fp));
            break;
        default :
            printf("bad hunk %d, %x\n", n, n);
        }
    output_object();
    exit(0);
}


