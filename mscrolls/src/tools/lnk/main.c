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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "symbols.h"
#include "linkit.h"
#include "output.h"

char *outfile = "b.out";
char *linkfile = NULL;

int debug=0;
int verbose=0;
int incremental=0;
int warnings=0;
int printtotalsize=0;
int patch=0;			/* !=0 to enable range error patching */

extern int code_load_base, code_address_base;

extern void Version();

static int getfromlinkfile(FILE* fp)
{
    int c = getc(fp);

    while (c == '#' || c == ';')
    {
        /* skip comments */
        while (c != '\n') c = getc(fp);
        c = getc(fp);
    }
    return c;
}

int main(int argc, char** argv)
{
    int i;
    char *p;
    FILE *fp;
    char s[100];

    for (i=1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for (p=argv[i]+1; *p; p++)
            {
                switch (*p)
                {
                case 'v' :
                    verbose=1;
                    break;
                case 'd' :
                    debug=1;
                    break;
                case 'l' :
                    if (i < argc-1) linkfile = argv[++i];
                    break;
                case 'o' :
                    if (i < argc-1) outfile = argv[++i];
                    break;
                case 'i' :
                    incremental = 1;
                    break;
                case 'b' :	/* set code load base */
                    if (i < argc-1)
                    {
                        sscanf(argv[++i], "%x", &code_load_base);
                        code_address_base = code_load_base;
                    }
                    break;
                case 'w' :
                    warnings = 1;
                    break;
                case 't' :
                    printtotalsize=1;
                    break;
                case 'p' :
                    patch = 1;
                    break;
                case 'V' :
                    Version();
                    exit(0);
                }
            }
        }
        else break; // all options first
    }

    if (i == argc && !linkfile)
    {
        printf("Usage: %s [-vdiwtpV] [-l <lfile>] [-o <outfile>] [-b <baseaddr>] [<ofiles...>]\n", argv[0]);
        return 1;
               
    }

    
    argc -= i-1;
    argv += i-1;

    init_symbols();
    if (!linkfile)
    {
        for (i=1; i<argc; i++)
        {
            if (verbose) printf("linking %s... (at %08x)\n", argv[i],
				code_address_base);
            linkit(argv[i]);
        }
    }
    else
    {
        if (!strcmp(linkfile, "-"))
            fp = stdin;
        else
            fp = fopen(linkfile, "rb");

        if (!fp)
        {
            perror("opening link file");
            exit(-1);
        }

        for (;;)
        {
            p = s;
            int c;

            do
            {
                c = getfromlinkfile(fp);
            } while (isspace(c));
            
            while (!isspace(c) && c != EOF)
            {
                assert(p - s < sizeof(s));
                *p++ = c;
                c = getfromlinkfile(fp);
            }
            *p = 0;
            
            if (c==EOF || *s==0) break;
            
            if (verbose)
                printf("linking %s... (at %08x)\n", s, code_address_base);
            
            linkit(s);
        }
    }

    if (!incremental)
        output_loadfile(outfile);
    else
        output_object(outfile);
    
    return 0;
}

