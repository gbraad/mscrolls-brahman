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
#include <stdarg.h>
#include "in_word.h"
#include "errors.h"

extern int lineno;
extern char *command;
extern char thisfile[20];
int err_flag=0;	/* flag to be set on error	*/
extern int warnings;

static void err_exit()
{
    fprintf(stderr, "%s: data not written out due to compilation errors\n",
            command);
    exit(-1);
}

/* this is supposed to recover from errors	*/
/* all it does is to skip chars until the next newline	*/
static void recover()
{
    int c;
    while ((c = in_char()) != '\n' && c != EOF) ;
    if (c == EOF) err_exit();
    err_flag = 1;	/* set the error flag	*/
}

void syntax()
{
    fprintf(stderr, "%s: \"%s\", line %d, syntax error\n",
            command, thisfile, lineno);
    recover();
}

void unrecognised(const char* s)
{
    fprintf(stderr, "%s: \"%s\", line %d, unrecognised '%s'\n",
            command, thisfile, lineno, s);
    recover();
}

void expected(const char* s)
{
    fprintf(stderr, "%s: \"%s\", line %d, expected '%s'\n",
            command, thisfile, lineno, s);
    recover();
}

void dup_id(const char* s)
{
    fprintf(stderr, "%s: \"%s\", line %d, duplicated identifier '%s'\n",
            command, thisfile, lineno, s);
    recover();
}

void invalid_prop(const char* s)
{
    fprintf(stderr, "%s: \"%s\", line %d, invalid property '%s'\n",
            command, thisfile, lineno, s);
    recover();
}

void range(int max)
{
    fprintf(stderr, "%s: \"%s\", line %d, number should be in range 0 - %d\n",
            command, thisfile, lineno, max);
    recover();
}

void settoolarge()
{
    fprintf(stderr, "%s: \"%s\", line %d, set too large\n",
            command, thisfile, lineno);
    recover();
}

void toomanysets()
{
    fprintf(stderr, "%s: \"%s\", line %d, too many sets\n",
            command, thisfile, lineno);
    recover();
}

void invalid_type(const char* s)
{
    fprintf(stderr, "%s: \"%s\", line %d, invalid type '%s'\n",
            command, thisfile, lineno, s);
}

void warning(const char* s, ...)
{
    if (warnings)
    {
        va_list args;
        fprintf(stderr, "%s: \"%s\", line %d, warning : ",
                command, thisfile, lineno);

        va_start(args, s);
        fprintf(stderr, s, args);
        va_end(args);
        
        putc('\n', stderr);
    }
}

