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

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inttype.h"
#include "sections.h"
#include "reloc.h"
#include "symbols.h"
#include "hunks.h"
#include "line.h"
#include "output.h"
#include "errors.h"

#ifdef DEBUG
extern inttype debug;
#endif

extern inttype wordaligned;
extern char *program_unit_name;
extern struct section *current_section;
extern inttype pass;
extern struct symbol star;

static void putlong(inttype n, FILE* fp);
static void putsymbol(int type, char* name, FILE* fp);

void align(int n)
{
    inttype ad, t;
    if (n>2) {
	warning("linker will not preserve ALIGN %d", n);
	warning("changing ALIGN %d to ALIGN 2", n);
	n = 2;
    }
    t = 1<<n;
    ad = (current_section->pc + t-1) & ~(t-1);
    t = ad - current_section->pc;
    for (; t; t--) output_byte((inttype)0);
    star.svalue = current_section->pc;	/* make sure star is ok */
}

void output_byte(inttype b)
{
    if (pass == 2) {
#ifdef DEBUG
	if (debug) printf ("%02x\n", b&0xff);
#endif
	if (b<-128 || b>255)
	    error("byte value too large");
	current_section->space[current_section->pc++] = (char)(b & 0xff);
    } else
	current_section->pc += 1;
}

void output_word(inttype w)
{
    if (pass == 2) {
#ifdef DEBUG
	if (debug) printf ("%06o %04x\n", w&0xffff, w&0xffff);
#endif
	if (w<-32768 || w>65535)
	    error("word value %04x too large", w);
	current_section->space[current_section->pc++] = (char)((w >> 8) & 0xff);
	current_section->space[current_section->pc++] = (char)(w & 0xff);
    } else
	current_section->pc += 2;
}

void output_long(inttype l)
{
    if (pass == 2) {
#ifdef DEBUG
	if (debug) printf ("%08x\n", l);
#endif
	current_section->space[current_section->pc++] = (char)((l >> 24) & 0xff);
	current_section->space[current_section->pc++] = (char)((l >> 16) & 0xff);
	current_section->space[current_section->pc++] = (char)((l >> 8) & 0xff);
	current_section->space[current_section->pc++] = (char)(l & 0xff);
    } else
	current_section->pc += 4;
}

extern struct reloc *reloc32[], *reloc16[], *reloc16An[];
extern struct ref *ref32[], *ref16An[], *ref16[], *ref8[];
extern char *sourcefile;

#ifdef A_OUT

#include <a.out.h>
#include <stab.h>

#ifdef MS_BIG_ENDIAN
#define SWAP(lv)
#else
#define SWAP(lv) swap(&lv, sizeof(lv))

inline void swap(char* p, int size)
{
	char t;
	if (size == 0) return;
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

#endif

static void swap_header(stricy exec* hdr)
{
	SWAP(hdr->a_magic);
	SWAP(hdr->a_text);
	SWAP(hdr->a_data);
	SWAP(hdr->a_bss);
	SWAP(hdr->a_syms);
	SWAP(hdr->a_entry);
	SWAP(hdr->a_trsize);
	SWAP(hdr->a_drsize);
}

static void swap_sym(struct nlist* np)
{
	SWAP(np->n_un.n_strx);
	SWAP(np->n_desc);
	SWAP(np->n_value);
}

#define FOR_REFS(refs, rfp) \
    for (rfp = refs; rfp; rfp = rfp->next)

#define FOR_RELOC(rel, rlp) \
    for (rlp = rel; rlp; rlp = rlp->next)

static inline int count_refs(struct ref* r)
{
    return r->n_refs;
}

static void output_refs(struct ref* ref, int symnum,
                        int len, int pcrel, FILE* fp)
{
    struct some_refs *refs;
    int i;
    struct relocation_info r;

    r.r_length = len;
    r.r_pcrel = pcrel;
    r.r_symbolnum = symnum;
    r.r_extern = 1;
    for (refs = ref->refs; refs; refs = refs->next)
	for (i = 0; i < refs->n_refs; i++) {
	    r.r_address = refs->ref[i];
	    fwrite(&r, 1, sizeof(r), fp);
	}
}

static void fix_pcrel_refs(int data, int len, struct ref* ref)
{
    struct some_refs *refs;
    int i, to_add, addr;

    for (refs = ref->refs; refs; refs = refs->next)
	for (i = 0; i < refs->n_refs; i++) {
	    to_add = -refs->ref[i];
	    addr = refs->ref[i];
	    switch (len) {
	    case 0 :
		*(char *)(data + addr) += to_add;
		break;
	    case 1 :
		SWAP(*(short *)(data + addr));
		*(short *)(data + addr) += to_add;
		SWAP(*(short *)(data + addr));
		break;
	    case 2 :
		SWAP(*(int *)(data + addr));
		*(int *)(data + addr) += to_add;
		SWAP(*(int *)(data + addr));
	    }
	}
}

char *string_table = 0;
int string_table_size = 4;
int string_table_maxsize = 0;

static void output_symbol(char* name, int type, int desc, int value, FILE* fp)

{
    int namelen;
    struct nlist nl;

    if (name) {
	namelen = strlen(name)+1;
	if (string_table_size + namelen > string_table_maxsize) {
	    if (string_table) {
		string_table = realloc(string_table, 2*string_table_maxsize);
		string_table_maxsize *= 2;
	    } else {
		string_table = malloc(1000);
		string_table_maxsize = 1000;
	    }
	}
	bcopy(name, string_table + string_table_size, namelen);
	nl.n_un.n_strx = string_table_size;
	string_table_size += namelen;
    } else
	nl.n_un.n_strx = 0;
    nl.n_type = type;
    nl.n_other = 0;
    nl.n_desc = desc;
    nl.n_value = value;
    swap_sym(&nl);
    fwrite(&nl, 1, sizeof(nl), fp);
}

#define ROUNDED(x)	((x + 3) & ~3)

void output_object(char* name)
{
    inttype i, j, n;
    struct section *sp;
    struct reloc *rlp;
    struct ref *rfp;
    struct some_refs *srp;
    struct symbol *sym;
    struct symbol_chunk *chunk;
    FILE *fp;
    int text_section = -1;
    int data_section = -1;
    int bss_section = -1;
    int etext, edata;
    int nsyms=0;
    int ntrel=0, ndrel=0;
    inttype done_reloc=0;   /* set when reloc section output */
    struct exec hdr;
    struct relocation_info r;

    fp = fopen(name, "wb");
    if (!fp) {
	perror(name);
	exit(-1);
    }

    for (i=0; i<n_sections; i++) {
	sp = &sections[i];
	if (sp->size)
	    switch (sp->type) {
	    case CODE :
		if (text_section >= 0)
		    error("multiple text sections\n");
		text_section = i;
		break;
	    case DATA :
		if (data_section >= 0)
		    error("multiple data sections\n");
		data_section = i;
		break;
	    case BSS :
		if (bss_section >= 0)
		    error("multiple bss sections\n");
		bss_section = i;
		break;
	    }
    }
    
    /* count the relocations and symbols */
    if (text_section >= 0) {
	if (reloc32[text_section]) {
	    rlp = reloc32[text_section];
	    while (rlp) {
		ntrel++;
		rlp = rlp->next;
	    }
	}
	if (reloc16[text_section]) {
	    rlp = reloc16[text_section];
	    while (rlp) {
		ntrel++;
		rlp = rlp->next;
	    }
	}
	if (reloc16An[text_section]) {
	    rlp = reloc16An[text_section];
	    while (rlp) {
		ntrel++;
		rlp = rlp->next;
	    }
	}
	if (ref32[text_section])
	    FOR_REFS(ref32[text_section], rfp) {
		ntrel += count_refs(rfp);
	    }
	if (ref16[text_section])
	    FOR_REFS(ref16[text_section], rfp) {
		ntrel += count_refs(rfp);
		fix_pcrel_refs(sections[text_section].space, 1, rfp);
	    }
	if (ref16An[text_section])
	    FOR_REFS(ref16An[text_section], rfp) {
		ntrel += count_refs(rfp);
	    }
	if (ref8[text_section])
	    FOR_REFS(ref8[text_section], rfp) {
		ntrel += count_refs(rfp);
		fix_pcrel_refs(sections[text_section].space, 0, rfp);
	    }
    }

    if (data_section >= 0) {
	if (reloc32[data_section]) {
	    rlp = reloc32[data_section];
	    while (rlp) {
		ndrel++;
		rlp = rlp->next;
	    }
	}
	if (reloc16[data_section]) {
	    rlp = reloc16[data_section];
	    while (rlp) {
		ndrel++;
		rlp = rlp->next;
	    }
	}
	if (reloc16An[data_section]) {
	    rlp = reloc16An[data_section];
	    while (rlp) {
		ndrel++;
		rlp = rlp->next;
	    }
	}
	if (ref32[data_section])
	    FOR_REFS(ref32[data_section], rfp) {
		ndrel += count_refs(rfp);
	    }
	if (ref16[data_section])
	    FOR_REFS(ref16[data_section], rfp) {
		ndrel += count_refs(rfp);
	    }
	if (ref16An[data_section])
	    FOR_REFS(ref16An[data_section], rfp) {
		ndrel += count_refs(rfp);
	    }
	if (ref8[data_section])
	    FOR_REFS(ref8[data_section], rfp) {
		ndrel += count_refs(rfp);
	    }
    }

    /* I'm going to need to know the indices of all the XREF'd symbols
       so i go through them here and use the svalue field to store the
       index */

    /* Allow for the stab entries for the filename and line numbers
       and one type which we use for global symbols. */
    nsyms = 2 + n_line_numbers;

    /* Generate debugging symbols for all global symbols which start
       with '_'. */
    FOR_SYMS(chunk, sym)
	if (sym->type == S_RELOC &&
	    (sym->flags & F_EXTERNAL) &&
	    sym->name[0] == '_')
	    nsyms++;

    FOR_SYMS(chunk, sym)
	if (sym->type == S_RELOC) nsyms++;
	else if (sym->type == S_XREF && (sym->flags & F_USED))
	    sym->svalue = nsyms++;

    /* write out the a.out header */
    hdr.a_magic = OMAGIC;
    hdr.a_text = text_section >= 0 ? ROUNDED(sections[text_section].size) : 0;
    hdr.a_data = data_section >= 0 ? ROUNDED(sections[data_section].size) : 0;
    hdr.a_bss = bss_section >= 0 ? ROUNDED(sections[bss_section].size) : 0;
    hdr.a_syms = nsyms * sizeof(struct nlist);
    hdr.a_entry = 0;
    hdr.a_trsize = ntrel * sizeof(struct relocation_info);
    hdr.a_drsize = ndrel * sizeof(struct relocation_info);
    swap_header(&hdr);
    fwrite(&hdr, 1, sizeof(hdr), fp);

    /* now the sections */
    if (text_section >= 0)
	fwrite(sections[text_section].space, 1,
	       ROUNDED(sections[text_section].size), fp);
    if (data_section >= 0)
	fwrite(sections[data_section].space, 1,
	       ROUNDED(sections[data_section].size), fp);

    /* text relocation */
    if (ntrel > 0) {
	/* internal relocations */
	r.r_length = 2;		/* 32 bit */
	r.r_pcrel = 0;		/* absolute */
	r.r_extern = 0;		/* internal relocation */
	if (reloc32[text_section])
	    FOR_RELOC(reloc32[text_section], rlp) {
		if (rlp->dest_index == text_section)
		    r.r_symbolnum = N_TEXT;
		else if (rlp->dest_index == data_section)
		    r.r_symbolnum = N_DATA;
		else
		    r.r_symbolnum = N_BSS;
		r.r_address = rlp->ad;
		fwrite(&r, 1, sizeof(r), fp);
	    }
	r.r_length = 1;		/* 16 bit */
	if (reloc16An[text_section])
	    FOR_RELOC(reloc32[text_section], rlp) {
		if (rlp->dest_index == text_section)
		    r.r_symbolnum = N_TEXT;
		else if (rlp->dest_index == data_section)
		    r.r_symbolnum = N_DATA;
		else
		    r.r_symbolnum = N_BSS;
		r.r_address = rlp->ad;
		fwrite(&r, 1, sizeof(r), fp);
	    }
	/* now external relocation */
	if (ref32[text_section])
	    FOR_REFS(ref32[text_section], rfp) {
		output_refs(rfp, find_symbol(rfp->name)->svalue, 2, 0, fp);
	    }
	if (ref16[text_section])
	    FOR_REFS(ref16[text_section], rfp) {
		output_refs(rfp, find_symbol(rfp->name)->svalue, 1, 1, fp);
	    }
	if (ref16An[text_section])
	    FOR_REFS(ref16An[text_section], rfp) {
		output_refs(rfp, find_symbol(rfp->name)->svalue, 1, 0, fp);
	    }
	if (ref8[text_section])
	    FOR_REFS(ref8[text_section], rfp) {
		output_refs(rfp, find_symbol(rfp->name)->svalue, 0, 1, fp);
	    }
    }

    /* data relocation */
    if (ndrel > 0) {
	/* internal relocations */
	r.r_length = 2;		/* 32 bit */
	r.r_pcrel = 0;		/* absolute */
	r.r_extern = 0;		/* internal relocation */
	if (reloc32[data_section])
	    FOR_RELOC(reloc32[data_section], rlp) {
		if (rlp->dest_index == data_section)
		    r.r_symbolnum = N_TEXT;
		else if (rlp->dest_index == data_section)
		    r.r_symbolnum = N_DATA;
		else
		    r.r_symbolnum = N_BSS;
		r.r_address = rlp->ad;
		fwrite(&r, 1, sizeof(r), fp);
	    }
	r.r_length = 1;		/* 16 bit */
	if (reloc16An[data_section])
	    FOR_RELOC(reloc32[data_section], rlp) {
		if (rlp->dest_index == data_section)
		    r.r_symbolnum = N_TEXT;
		else if (rlp->dest_index == data_section)
		    r.r_symbolnum = N_DATA;
		else
		    r.r_symbolnum = N_BSS;
		r.r_address = rlp->ad;
		fwrite(&r, 1, sizeof(r), fp);
	    }
	/* now external relocation */
	if (ref32[data_section])
	    FOR_REFS(ref32[data_section], rfp) {
		output_refs(rfp, find_symbol(rfp->name)->svalue, 2, 0, fp);
	    }
	if (ref16[data_section])
	    FOR_REFS(ref16[data_section], rfp) {
		output_refs(rfp, find_symbol(rfp->name)->svalue, 1, 1, fp);
	    }
	if (ref16An[data_section])
	    FOR_REFS(ref16An[data_section], rfp) {
		output_refs(rfp, find_symbol(rfp->name)->svalue, 1, 0, fp);
	    }
	if (ref8[data_section])
	    FOR_REFS(ref8[data_section], rfp) {
		output_refs(rfp, find_symbol(rfp->name)->svalue, 0, 1, fp);
	    }
    }

    /* and now for the symbols */
    /* first the debugging information */
    output_symbol(sourcefile, N_SO, 0, 0, fp);
    output_symbol("int:t1=r1;-2147483648;2147483647", N_LSYM, 0, 0, fp);
    {
	struct line_chunk *l;
	for (l = first_line_chunk; l; l = l->next)
	    for (i = 0; i < l->n_lines; i++)
		output_symbol(0, N_SLINE,
			      l->line_numbers[i], l->line_addresses[i],
			      fp);
    }
    FOR_SYMS(chunk, sym)
	if (sym->type == S_RELOC &&
	    (sym->flags & F_EXTERNAL) &&
	    sym->name[0] == '_') {
	    char buf[100];
	    int type;
	    if (sym->section_index == text_section)
		type = N_TEXT;
	    else if (sym->section_index == data_section)
		type = N_DATA;
	    else
		type = N_BSS;
	    strcpy(buf, sym->name + 1);
	    if (type == N_TEXT) {
		strcat(buf, ":F1");
		type = N_FUN;
	    } else {
		strcat(buf, ":G1");
		type = N_GSYM;
	    }
	    output_symbol(buf, type, 0, sym->svalue, fp);
	}

    FOR_SYMS(chunk, sym) {
	int type;
	if (sym->type == S_RELOC) {
	    if (sym->section_index == text_section)
		type = N_TEXT;
	    else if (sym->section_index == data_section)
		type = N_DATA;
	    else
		type = N_BSS;
	    if (sym->flags & F_EXTERNAL) type |= N_EXT;
	    output_symbol(sym->name, type, 0, sym->svalue, fp);
	} else if (sym->type == S_XREF && (sym->flags & F_USED))
	    output_symbol(sym->name, N_UNDF|N_EXT, 0, 0, fp);
    }

    /* and now for the string table */
    if (string_table == 0) string_table = malloc(4);
    *(int *)string_table = string_table_size;
    SWAP(*(int *)string_table);
    fwrite(string_table, 1, string_table_size, fp);
    free(string_table);
    (void)fclose(fp);
}

#else /* A_OUT */

void output_object(char* name)
{
    inttype i, j, n;
    struct section *sp;
    struct reloc *rlp;
    struct ref *rfp;
    struct some_refs *srp;
    struct symbol *sym;
    struct symbol_chunk *chunk;
    FILE *fp;
    inttype done_reloc=0;   /* set when reloc section output */

    fp = fopen(name, "wb");
    if (!fp) {
	perror(name);
	exit(-1);
    }

    /* hunk_unit */
    putlong((inttype)hunk_unit, fp);
    putsymbol(0, program_unit_name, fp);

    for (i=0; i<n_sections; i++) {
	sp = &sections[i];
	if (sp->size || (!done_reloc && i==n_sections-1)) {
	    done_reloc=1;
	    if (i) {
		/* hunk_name */
		putlong((inttype)hunk_name, fp);
		putsymbol(0, sp->name, fp);
	    }

	    /* relocatable block */
	    switch (sp->type) {
	    case CODE :
		if (!wordaligned) {
		    putlong((inttype)hunk_code, fp);
		    putlong((sp->size + 3)/4, fp);  /* round up */
		    /* note - on machines with < 32 bit ints, this
		     * might not work */
		    (void)fwrite(sp->space, 1, (int)sp->size, fp);
		    /* pad with zeros */
		    for (j=((sp->size + 3) & ~3) - sp->size; j; j--)
			putc(0, fp);
		    break;
		} else {
		    putlong((inttype)hunk_code_word, fp);
		    putlong((sp->size + 1)/2, fp);  /* round up */
		    /* note - on machines with < 32 bit ints, this
		     * might not work */
		    (void)fwrite(sp->space, 1, (int)sp->size, fp);
		    /* pad with zeros */
		    for (j=((sp->size + 1) & ~1) - sp->size; j; j--)
			putc(0, fp);
		    break;
		 }
	    case DATA :
		putlong((inttype)hunk_data, fp);
		putlong((sp->size + 3)/4, fp);	/* round up */
		/* note - on machines with < 32 bit ints, this
		 * might not work */
		(void)fwrite(sp->space, 1, (int)sp->size, fp);
		/* pad with zeros */
		for (j=((sp->size + 3) & ~3) - sp->size; j; j--)
		    putc(0, fp);
		break;
	    case BSS :
		putlong((inttype)hunk_bss, fp);
		putlong((sp->size + 3)/4, fp);	/* round up */
		break;
	    default :
		printf("weird section type %d\n", sp->type);
		abort();
	    }

	    /* relocation info */
	    if (reloc32[i]) {
		putlong((inttype)hunk_reloc32, fp);
		for (j=0; j<n_sections; j++) {
		    rlp = reloc32[i];
		    n = 0;
		    while (rlp) {
			if (rlp->dest_index == j) n++;
			rlp = rlp->next;
		    }
		    if (n) {
			rlp = reloc32[i];
			putlong(n, fp);
			putlong((inttype)sections[j].hunk_num, fp);
			while (rlp) {
			    if (rlp->dest_index == j)
				putlong(rlp->ad, fp);
			    rlp = rlp->next;
			}
		    }
		}
		putlong((inttype)0, fp);
	    }
	    if (reloc16An[i]) {
		putlong((inttype)hunk_reloc16An, fp);
		for (j=0; j<n_sections; j++) {
		    rlp = reloc16An[i];
		    n = 0;
		    while (rlp) {
			if (rlp->dest_index == j) n++;
			rlp = rlp->next;
		    }
		    if (n) {
			rlp = reloc16An[i];
			putlong(n, fp);
			putlong((inttype)sections[j].hunk_num, fp);
			while (rlp) {
			    if (rlp->dest_index == j)
				putlong(rlp->ad, fp);
			    rlp = rlp->next;
			}
		    }
		}
		putlong((inttype)0, fp);
	    }
	    if (reloc16[i]) {
		putlong((inttype)hunk_reloc16, fp);
		for (j=0; j<n_sections; j++) {
		    rlp = reloc16[i];
		    n = 0;
		    while (rlp) {
			if (rlp->dest_index == j) n++;
			rlp = rlp->next;
		    }
		    if (n) {
			rlp = reloc16[i];
			putlong(n, fp);
			putlong((inttype)sections[j].hunk_num, fp);
			while (rlp) {
			    if (rlp->dest_index == j)
				putlong(rlp->ad, fp);
			    rlp = rlp->next;
			}
		    }
		}
		putlong((inttype)0, fp);
	    }

	    if (sections[i].contains_ext) {
		/* extern symbol info */
		putlong((inttype)hunk_ext, fp);
		/* ext_abs & ext_def */
		FOR_SYMS(chunk, sym) {
		    if (sym->section_index == i && sym->flags&F_EXTERNAL) {
			if (sym->type == S_RELOC)
			    putsymbol(ext_def, sym->name, fp);
			else if (sym->type == S_ABS)
			    putsymbol(ext_abs, sym->name, fp);
			else
			    fatal_error("weird XDEFed symbol type %d",
				sym->type);
			putlong(sym->svalue, fp);
		    }
		}
		for (rfp=ref32[i]; rfp; rfp=rfp->next) {
		    putsymbol(ext_ref32, rfp->name, fp);
		    putlong(rfp->n_refs, fp);
		    for (srp=rfp->refs; srp; srp=srp->next)
			for (j=0; j<srp->n_refs; j++)
			    putlong(srp->ref[j], fp);
		}
		for (rfp=ref16An[i]; rfp; rfp=rfp->next) {
		    putsymbol(ext_ref16An, rfp->name, fp);
		    putlong(rfp->n_refs, fp);
		    for (srp=rfp->refs; srp; srp=srp->next)
			for (j=0; j<srp->n_refs; j++)
			    putlong(srp->ref[j], fp);
		}
		for (rfp=ref16[i]; rfp; rfp=rfp->next) {
		    putsymbol(ext_ref16, rfp->name, fp);
		    putlong(rfp->n_refs, fp);
		    for (srp=rfp->refs; srp; srp=srp->next)
			for (j=0; j<srp->n_refs; j++)
			    putlong(srp->ref[j], fp);
		}
		for (rfp=ref8[i]; rfp; rfp=rfp->next) {
		    putsymbol(ext_ref8, rfp->name, fp);
		    putlong(rfp->n_refs, fp);
		    for (srp=rfp->refs; srp; srp=srp->next)
			for (j=0; j<srp->n_refs; j++)
			    putlong(srp->ref[j], fp);
		}
		putlong((inttype)0, fp);
	    }

	    /* symbol table */
	    putlong((inttype)hunk_symbol, fp);
	    FOR_SYMS(chunk, sym)
		if (sym->section_index == i &&
		    (sym->type == S_RELOC)) {
		    putsymbol(0, sym->name, fp);
		    putlong(sym->svalue, fp);
		}
	    putlong((inttype)0, fp);

	    putlong((inttype)hunk_end, fp);
	}
    }

    (void)fclose(fp);
}

static void putlong(inttype n, FILE* fp)
{
    putc((char)(n>>24), fp);
    putc((char)(n>>16), fp);
    putc((char)(n>>8), fp);
    putc((char)n, fp);
}

static void putsymbol(int type, char* name, FILE* fp)
{
    inttype i, len, rnd;
    char *p;

    len = strlen(name);
    rnd = (len + 3) & ~3;
    putlong(((inttype)type<<24) + rnd/4, fp);
    for (i=len, p=name; i; i--, p++)
	putc(*p, fp);
    /* pad with zeros */
    for (i=rnd-len; i; i--)
	putc(0, fp);
}

#endif /* A_OUT */
