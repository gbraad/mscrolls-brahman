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
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include "inttype.h"
#include "alloc.h"
#include "sections.h"
#include "symbols.h"
#include "vfiles.h"
#include "errors.h"
#include "assemble.h"
#include "include.h"
#include "reloc.h"
#include "branches.h"
#include "output.h"
#include "os.h"

extern char current_filename[];
extern inttype n_macro_calls;
extern inttype local_index;
extern inttype n_errors;
extern inttype current_lineno;
extern inttype cond_depth;

extern void Version();


#ifdef DEBUG
inttype debug=0;
#endif

inttype lowercase=0;		/* convert all labels to lowercase */
inttype wordaligned=0;		/* set to produce word aligned code sections */
inttype optimise=0;		/* optimise branches when size not specified */
inttype memorytemp=0;		/* keep the temp file in memory */
FILE *inputfile;
char *program_unit_name="";
static struct section default_section = { "NONAME", CODE, 0, 0 };
struct section sections[MAXSECTIONS];
inttype n_sections = 1;
struct section *current_section = &sections[0];
inttype current_index=0;	/* index of current section */
inttype pass;
FILE *ftemp;
VFILE *vtemp;			/* for a temp file in memory */
char tempfile[]="tempXXXXXX", *mktemp();
char *sourcefile;
char *outfile="b.out";
jmp_buf pass_finished;
struct symbol star;
#ifdef EMU
inttype processor=1;		/* assemble instructions for 'm68001'
				 * i.e. with our extensions & restrictions */
#else // EMU
#	ifdef MR
inttype processor=0;		/* std m68000 */
#	endif
#endif

/* check a string to see if it contains a number */
static inttype check_num(char* s)
{
    while (*s)
        if (!isdigit(*s++))
            return 0;
    return 1;
}

int main(int argc, char** argv)
{
    inttype i;
    struct section *sp;
    struct symbol_chunk *chunk;
    struct symbol *sym;
    inttype hunk;
    
#ifdef A_OUT
    int text_section = -1;
    int data_section = -1;
    int bss_section = -1;
#endif

    /* options */
    for (i=1; i<argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'w' :
                wordaligned = 1;
                break;
            case 'o' :
                if (i < argc-1) outfile = argv[++i];
                break;
            case 'i' :
                if (i < argc-1) add_include_dirs(argv[++i]);
                break;
            case 'p' :	/* processor */
                if (i < argc-1)
                {
                    if (!check_num(argv[++i]))
                    {
                        printf("Processor number expected after '-p'\n");
                        exit(-1);
                    }
                    processor = atoi(argv[i]);
                }
                break;
            case 'O' :	/* optimise */
                optimise = 1;
                break;
            case 'm' : /* put the temp file in memory */
                memorytemp = 1;
                break;
            case 'l' : /* lowercase */
                lowercase = 1;
                break;
            case 'v':
            case 'V':
                // print version, usage and exit
                sourcefile = 0;
                i = argc;
                break;
            case 'D':
                debug = 1;
                break;
            default :
                printf("bad option '%s'\n", argv[i]);
                
                // print version, usage and exit
                sourcefile = 0;
                i = argc;
            }
        }
        else
        {
            if (!strcmp(argv[i], "opt"))
            {
                /* ignore "opt qs" */
                i++;
            }
            else
            {
                sourcefile = argv[i];
            }
        }
    }

    Version();

    if (!sourcefile)
    {
        fprintf(stderr, "Usage: %s [-w] [-o <outfile>] [-i <includedir>,...] [-p <0 or 1>] [-O] [-m] [-l] [-v] [-D] <file>\n", argv[0]);
        exit(-1);
    }

    signal(SIGINT, tidy_exit);
    
    mktemp(tempfile);
    if (memorytemp)
        vtemp = vopen(tempfile, "wb");
    else
    {
        ftemp = fopen(tempfile, "w+b");
        if (!ftemp)
        {
            perror("can't open temp file");
            exit(-1);
        }
    }

    if (debug) fprintf(stderr, "opened temp file '%s'\n", tempfile);
    
    sections[0] = default_section;
    init_reloc();
    init_symbols();
    init_branches();
    
    /* bodge sp=a7 */
    sym = add_symbol("sp");
    sym->type = S_AREG;
    sym->svalue = 7;
    
    /* fix star symbol */
    memset(&star, 0, sizeof(star));
    star.type = S_RELOC;
    star.name = "*";
    
    inputfile = fopen(sourcefile, "rb");
    if (!inputfile)
    {
        perror(sourcefile);
        exit(-1);
    }
    
    strcpy(current_filename, sourcefile);
    
    /* first pass */
    current_lineno=0;
    if (setjmp(pass_finished)) goto second_pass;
    pass = 1;
    assemble();
    
 second_pass:
    
    if (cond_depth)
    {
        printf("%d ENDC statement%s missing\n", cond_depth,
               cond_depth==1?"":"s");
        n_errors++;
    }
    current_lineno=0;
    resolve_branches();
    hunk=0;
    
    for (i=0, sp=sections; i<n_sections; i++, sp++)
    {
#ifdef A_OUT
        sp->size = (sp->pc + 3) & ~3; /* round up to multiple of 4 */
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
#else
        sp->size = sp->pc;
#endif
        if (sp->type != BSS) sp->space = malloc((unsigned)sp->pc);
        if (sp->pc) sp->hunk_num = hunk++;
        sp->pc = 0;
    }
#ifdef  A_OUT
    {
        unsigned long load_address = 0L;
        if (text_section >= 0) {
            sections[text_section].load_address = load_address;
            load_address += sections[text_section].size;
        }
        if (data_section >= 0) {
            sections[data_section].load_address = load_address;
            load_address += sections[data_section].size;
        }
        if (bss_section >= 0) {
            sections[bss_section].load_address = load_address;
        }
    }
#endif
    
    n_macro_calls=0;
    local_index=0;
    /* zap EQUR'ed symbols */
    FOR_SYMS(chunk, sym)
    {
        if (sym->type==S_DREG || sym->type==S_AREG)
            sym->type=S_NOTHING;
        else if (sym->type==S_UNDF)
        {
            printf("\"%s\" undefined after first pass\n", sym->name);
            n_errors++;
        } else if (sym->type == S_RELOC)
            sym->svalue += sections[sym->section_index].load_address;
    }
    if (n_errors)
    {
        /* check for 1st pass errors */
        printf("%d error%s in first pass\n", n_errors,
               n_errors>1?"s":"");
        tidy_exit(1);
    }
    
    /* second pass */
    sym = find_symbol("sp");
    sym->type = S_AREG;
    sym->svalue = 7;
    pass = 2;
    current_section = &sections[0];
    current_index = 0;
    fclose(inputfile);
    
    if (memorytemp)
        vrewind(vtemp);
    else
    {
        rewind(ftemp);
        inputfile = ftemp;		/* use temp file in 2nd pass */
    }
    if (!setjmp(pass_finished))
        assemble();

    // clean up
    
    if (n_errors)
    {
	/* check for 2nd pass errors */
        printf("%d error%s in second pass\n", n_errors,
               n_errors>1?"s":"");
        tidy_exit(1);
    }
    
    if (!memorytemp)
    {
        fclose(ftemp);
        deleteFile(tempfile);
    }
    
    output_object(outfile);
    return 0;
}


