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

/* This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include "inttype.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "alloc.h"
#include "vfiles.h"
#include "errors.h"

extern FILE *inputfile;
extern inttype pass;
extern inttype memorytemp;
extern VFILE *vtemp;
extern FILE *ftemp;

char filenames[20][_PATH_MAX];
FILE *fileptrs[20];
inttype linenos[20];
inttype include_depth=0;
char current_filename[_PATH_MAX];
inttype current_lineno;

#define MAXINCLUDEDIRS	20

char *include_dir[MAXINCLUDEDIRS];
inttype n_include_dirs=0;

static FILE *open_include(char* name)
{
    inttype i;
    FILE *fp;
    char s[_PATH_MAX];

    if ((fp = fopen(name, "rb"))) return fp;
    
    for (i=0; i<n_include_dirs; i++)
    {
        strcpy(s, include_dir[i]);
        strcat(s, "/");
        strcat(s, name);
        if ((fp = fopen(s, "rb"))) return fp;
    }
    
    error("include file \"%s\" not found", name);
    return 0;
}


void include(char* filename)
{
    FILE *fp = open_include(filename);
    if (!fp) return;
    strcpy(filenames[include_depth], current_filename);
    fileptrs[include_depth] = inputfile;
    linenos[include_depth++] = current_lineno;
    inputfile = fp;
    current_lineno = 0;
    strcpy(current_filename, filename);
    if (pass==1)
    {
        if (memorytemp)
            Vprintf(vtemp, " current_file \"%s\",0\n", filename);
        else
            fprintf(ftemp, " current_file \"%s\",0\n", filename);
    }
}

/* this routine is called by getline when EOF is detected on the input stream */
void finish_include()
{
    if (!include_depth)
        fatal_error("finish_include called when not in include file");
    include_depth--;
    fclose(inputfile);
    inputfile = fileptrs[include_depth];
    current_lineno = linenos[include_depth];
    strcpy(current_filename, filenames[include_depth]);
    if (pass==1)
    {
        if (memorytemp)
            Vprintf(vtemp, " current_file \"%s\",%d\n",
                    current_filename, current_lineno+1);
        else
            fprintf(ftemp, " current_file \"%s\",%d\n",
                    current_filename, current_lineno+1);
    }
}


void add_include_dirs(char* list)
{
    char *p, *q;

    if (!list) return;

    p = list;
    while ((q = strchr(p, ',')) != 0)
    {
        *q = 0;	/* fod the comma */
        include_dir[n_include_dirs] = allocstr(p);
        strcpy(include_dir[n_include_dirs], p);	/* copy it into list */
        n_include_dirs++;
        p=q+1;
    }
    include_dir[n_include_dirs] = allocstr(p);
    strcpy(include_dir[n_include_dirs], p);	/* copy it into list */
    n_include_dirs++;
}
