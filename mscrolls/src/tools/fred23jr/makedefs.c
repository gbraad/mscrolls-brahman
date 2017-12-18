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
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static char *ucaseof(char* s)
{
    char *p, *q;
    static char temp[64];

    p = s;
    q = temp;
    while (*p)
    {
        *q++ = islower(*p) ? toupper(*p) : *p;
        p++;
    }
    *q = '\0';
    return temp;
}

static inline int get(FILE* fp)
{
    // ignore DOS newlines
    int c;
    while ((c = getc(fp)) == '\r') ;
    return c;
}

int main(int argc, char** argv)
{
    int i=1, c;
    char *p;
    char fnameh[30], fnamec[30], buf[30];
    FILE *fin, *fouth, *foutc;
    int j;
    char* fname = 0;
    const char* prefix = 0;

    for (j = 1; j < argc; ++j)
    {
        if (argv[j][0] == '-')
        {
            if (argv[j][1] == 'p') prefix = argv[j] + 3;
            else
            {
                fprintf(stderr, "Unrecognised option '%s'\n", argv[j]);
                exit(-1);
            }
        }
        else
        {
            fname = argv[j];
        }
    }

    if (!fname)
    {
        fprintf(stderr, "Usage: makedefs [-p=prefix] <file>\n");
        exit(-1);
    }
    
    strcat(strcpy(fnameh, fname), ".h");
    strcat(strcpy(fnamec, fname), ".str");
    
    fin = fopen(fname, "rb");
    if (!fin)
    {
        fprintf(stderr, "Can't open input file '%s'\n", fname);
        exit(-1);
    }

    fouth = fopen(fnameh, "wb");
    if (!fouth)
    {
        perror(fnameh);
        exit(-1);
    }
    
    foutc = fopen(fnamec, "wb");
    if (!foutc)
    {
        perror(fnamec);
        exit(-1);
    }
    
    while ((c = get(fin)) != EOF)
    {
        p = buf;
        do {
            *p++ = c;
            c = get(fin);
        } while (c != '\n' && c != EOF);
        *p = '\0';
        if (p = strchr(buf, ',')) *p++ = '\0';
        else p = buf;

        if (prefix)
            fprintf(fouth, "#define %s_%s %d\n", prefix, ucaseof(buf), i++);
        else
            fprintf(fouth, "#define %s %d\n", ucaseof(buf), i++);
        
        fprintf(foutc, "\t\"%s\",\n", p);
    }
    fclose(fin);
    fclose(fouth);
    fclose(foutc);
    return 0;
}
