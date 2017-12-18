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

#include "inttype.h"
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "getline.h"
#include "os.h"

extern jmp_buf mainloop;
extern inttype pass;
extern char current_filename[], line_buf[];
extern inttype current_lineno;
extern inttype memorytemp;
extern char tempfile[];

inttype n_errors=0;

void error_line()
{
    printf("\"%s\" line %d :\n%s\t%s\t%s\n", current_filename, current_lineno,
           label_field, opcode_field, operand_field);
}


void tidy_exit(int n)
{
    if (!memorytemp)
	deleteFile(tempfile);
    exit(n);
}

void fatal_error(const char* s, ...)
{
    va_list args;
    
    error_line();
    va_start(args, s);
    vprintf(s, args);
    va_end(args);
    
    putchar('\n');
    //abort();					/* dump a core */
    tidy_exit(-1);
}

void error(const char* s, ...)
{
    va_list args;
    error_line();
    printf("error : ");
    
    va_start(args, s);
    vprintf(s, args);
    va_end(args);

    putchar('\n');
    n_errors++;
    longjmp(mainloop, 1);
}

void warning(const char* s, ...)
{
    va_list args;

    error_line();
    printf("warning : ");

    va_start(args, s);
    vprintf(s, args);
    va_end(args);
    putchar('\n');
}

