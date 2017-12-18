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
#include "utils.h"
#include "in_word.h"

#define MAX_INC_DEPTH 10

extern char *command;
extern char prefix_path[];

int lineno=1;
FILE *fileptr;
char thisfile[30];

int old_lineno[MAX_INC_DEPTH];
FILE *old_fileptr[MAX_INC_DEPTH];
char old_thisfile[MAX_INC_DEPTH][30];
static int inword_fileno = 0;

void fileopen(char* name)
{
    if ((fileptr = fopen(name, "rb")) == NULL) cantopen(name);
    strcpy(thisfile, name);
    lineno = 1;
}

int white_sp(int c)
{
    return((c == ' ') || (c == '\n') || (c == '\t'));
}

int in_word(char* s)
{
    int c;
    char *p;

    do {
        if ((c = in_char()) == EOF) return EOF;
    } while (white_sp(c));
    p = s;
    *p++ = c;
    if (c == ';' || c == '{' || c == '}' || c == '"' || c == '!') {
        *p = '\0';
        return 0;
    }
    while(((c = in_char()) != EOF) && !white_sp(c)
          && c != ';' && c != '{' && c != '}' && c != '"' && c != '!') *p++ = c;
    ungetc(c, fileptr);	/* push the last char back onto file	*/
    if (c == '\n') lineno--;	/* make sure line no. is correct	*/
    *p = '\0';
    return 0;
}

static int _in_char()
{
    int c;
    char *p;
    static int start_line=1;

 ic:

    c = getc(fileptr);
    if (c == '\r') goto ic; // handle DOS format input

    if (c == EOF)
    {
        if (!inword_fileno) return EOF;	/* end of first file	*/
        inword_fileno--;
        fclose(fileptr);
        lineno = old_lineno[inword_fileno];
        fileptr = old_fileptr[inword_fileno];
        strcpy(thisfile, old_thisfile[inword_fileno]);
        goto ic;
    }

    if (start_line)
    {
        if (c == '.')
        {	/* special line	*/
            c = getc(fileptr);
            switch(c) {
            case 'i' :
                old_lineno[inword_fileno] = ++lineno;
                old_fileptr[inword_fileno] = fileptr;
                strcpy(old_thisfile[inword_fileno], thisfile);
                inword_fileno++;
                p = thisfile;
                strcpy(p,prefix_path);
                p+= strlen(prefix_path);
                while ((c = getc(fileptr)) != '\n') *p++ = c;
                *p = '\0';
                if ((fileptr = fopen(thisfile, "rb")) == NULL)
                    cantopen(thisfile);
                lineno = 1; start_line = 1;
                goto ic;
            default :
                fprintf(stderr, "%s: \"%s\", line %d, bad special directive\n",
                        command, thisfile, lineno);
                exit(-1);
            }
        }
        else start_line = 0;
    }

    if (c == '\n')
    {
        lineno++;
        start_line=1;
    }
    return c;
}

int in_char()
{
    int c;

    do {
        if ((c = _in_char()) == EOF) return EOF;
        if (c == '#') {
            while (((c = _in_char()) != '\n') && (c != EOF)) ;
            c = 0;
        }
    } while (!c);
    return c;
}
