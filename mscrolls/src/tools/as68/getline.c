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
#include <ctype.h>
#include <stdio.h>
#include "vfiles.h"
#include "symbols.h"
#include "macros.h"
#include "opcodes.h"
#include "getline.h"
#include "errors.h"
#include "include.h"

extern inttype pass;
extern jmp_buf pass_finished;
extern inttype current_lineno;
extern FILE *inputfile;
extern FILE *ftemp;
extern VFILE *vtemp;
extern inttype lowercase;
extern inttype memorytemp;
extern inttype assembling;
extern inttype include_depth;
extern inttype in_macro;
extern inttype n_macro_args;
extern char macro_arg[][MAXMACROARGS][MAXMACROARGLEN];
extern inttype defining_macro;
extern char *macro_ptr;
extern inttype n_macro_calls;
extern struct m68k_opcode m68k_opcodes[];

char label_field[MAXLABELLEN];	/* the label field of the current line */
char opcode_field[MAXLABELLEN];	/* opcode field */
char operand_field[1000];		/* operand field */
struct symbol *opcode_symbol;
char *operand_ptr; /* ptr to current posn in operand field */
//char unput_buffer[200];			/* buffer for unput */
//char *unput_ptr = unput_buffer;
inttype in_string=0; /* set when in "..." */
inttype in_ascii=0; /* set when in '...' */
static int end_of_line;

static int next_macro_char();

static int skip_comment()
{
    int c;
    while ((c = getc(inputfile)) != EOF && c != '\n') ;
    return c;
}

int next_char()
{
    /* return next char from input stream, ignoring ";...\n" comments */
    
    int c;
    if (in_macro) return next_macro_char();

    // ignore any DOS ^M
    do {
        c = getc(inputfile);
    } while (c == '\r');
    
    if (!in_string && !in_ascii)
    {
        if (c == ';') c = skip_comment();
    }
    return c;
}
    
static int next_macro_char()
{
    int c, n;
    static char *macro_arg_ptr=0;	/* points to next char from a macro
					 * argument, 0 for no argument */
    char s[32];		/* buf to hold \@ expansion */

 nc :
    /* return a character from a macro if expanding one */
    /* also expand macro arguments */
    if (in_macro) {
        if (macro_arg_ptr) {
            c = *macro_arg_ptr++;
            if (c) return c;
            macro_arg_ptr = 0;
        }
        c = *macro_ptr++;
        if (!c) {
            finish_macro();
            goto nc;
        }
        if (c=='\\') {
            if (!assembling || defining_macro)
                return c;
            c = next_char();	/* get next char of macro arg */
            if (isdigit(c)) {
                n = c - '0';
                if (n > n_macro_args) return next_char();
                macro_arg_ptr = macro_arg[in_macro][n-1];
                return *macro_arg_ptr++;
            }
            if (c=='@') {
                sprintf(s, ".%03d", n_macro_calls);
                macro_arg_ptr = s;
                return *macro_arg_ptr++;
            }
            error("weird macro argument %c", c);
        }
        return c;
    }

    c = getc(inputfile);
    if (in_string || in_ascii) return c;
    if (c == ';')
        while (c != '\n' && c != EOF)
            c = getc(inputfile);
    return c;
}

#if 0
void lex_unput(char c)
{
    *unput_ptr++ = c;
    
}
#endif

/* read one character from operand field or macro argument */
int input()
{
    int c;

    //if (unput_ptr > unput_buffer) c = *--unput_ptr;
    //else
    {
        c = *operand_ptr;
        if (c) ++operand_ptr;
        else
        {
            // stay on terminator
            end_of_line = 1;
        }
    }
    return c;
}

int yywrap()
{
    return end_of_line;
}

/* dump the current line to a temp file */
void dump_line()
{
    extern FILE *ftemp;
    extern inttype pass;

    if (pass==1)
    {
        if (memorytemp)
            Vprintf(vtemp, "%s %s %s\n", label_field, opcode_field,
                    operand_field);
        else
        {
            fputs(label_field, ftemp);
            putc(' ', ftemp);
            fputs(opcode_field, ftemp);
            putc(' ', ftemp);
            fputs(operand_field, ftemp);
            putc('\n', ftemp);
            fflush(ftemp);
        }
    }
}

void puttemp(char c)
{
    if (pass==2) return;
    if (memorytemp) vputc(c, vtemp);
    else putc(c, ftemp);
}

extern void yyrestart(FILE*);

/* get a line from input stream */
/* assumes that the file pointer is at the beginning of the line */
void lgetline()
{
    int c;
    char *p, *q;
    char s[256];

    end_of_line = 0;
    yyrestart(stdin);

 tryagain:
    
    if (!in_macro) current_lineno++;

    c = next_char();		/* first character of the line */

    /* check for end of file */
    if (c==EOF)
    {
	if (include_depth) finish_include();
	else longjmp(pass_finished, 1);
	goto tryagain;
    }

    if (c=='*')
    {
        /* comment line ? */
	skip_comment();
	if (pass==1) puttemp('\n'); /* make sure the number of lines in temp
				     * file is correct for second pass */
	goto tryagain;
    }

    if (c=='\n')
    {
        /* blank line ? */
	if (pass==1) puttemp('\n');
	goto tryagain;
    }

    operand_ptr = operand_field;	/* ptr to current char in operands */
    //unput_ptr = unput_buffer;		/* clear any unputted chars */

    if (!isspace(c))
    {
        /* label present ? */
	p = label_field;
	do
        {
	    if (lowercase && isupper(c)) c = tolower(c);
	    *p++ = c;
	    c = next_char();
	} while (!isspace(c) && c != ':' && c != EOF);	/* colon can terminate label */
	*p = '\0';			/* zero terminate */
	if (c == ':') c = next_char();	/* if last was colon, get next */
    } else				/* first char of line is white space */
	label_field[0] = '\0';		/* so no label */

    /* skip spaces up to opcode/end of line */
    while (c != '\n' && c != EOF && isspace(c)) c = next_char();

    if (c == EOF) c = '\n';	/* handle EOF on partial line */

    /* opcode present ? */
    if (c == '\n')
    {
	opcode_field[0] = '\0';
	operand_field[0] = '\0';
	return;
    }
    p = opcode_field;
    do
    {
	*p++ = c;
	c = next_char();
    } while (!isspace(c) && c != EOF);
    *p = '\0';
    
    /* lcase the opcode */
    p = s;
    q = opcode_field;
    while (*q)
    {
	if (isupper(*q)) *p = tolower(*q);
	else *p = *q;
	p++; q++;
    }
    *p = '\0';
    
    opcode_symbol = find_symbol(s);
    if (!opcode_symbol)
	opcode_symbol = find_symbol(opcode_field);

    /* only bother to collect args if this opcode takes them */
    if (opcode_symbol && opcode_symbol->type == S_OPCODE &&
	!*(m68k_opcodes[opcode_symbol->svalue].args))
    {
	while (c != '\n') c = next_char();	/* skip rest of line */
	operand_field[0] = '\0';
	goto check_opcode;
    }

    /* skip spaces up to operand field */
    while (c != '\n' && isspace(c)) c = next_char();

    /* are there any operands ? */
    if (c == '\n')
    {
	operand_field[0] = '\0';
	goto check_opcode;
    }
    p = operand_field;
    
    do
    {
	*p++ = c;
	if (c == '"')
        {
	    in_string = 1;
	    c = next_char();
	    while (c != '"' && c != '\n')
            {
		*p++ = c;
		c = next_char();
	    }
	    if (c == '\n')
            {
		error("missing double quote");
		*p++ = '"';
		break;
	    }
	    *p++ = c;
	    in_string = 0;
	}
        else if (c == '\'')
        {
	    in_ascii = 1;
	    c = next_char();
	    while (c != '\'' && c != '\n')
            {
		*p++ = c;
		c = next_char();
	    }
	    if (c == '\n')
            {
		error("missing single quote");
		*p++ = '"';
		break;
	    }
	    *p++ = c;
	    in_ascii = 0;
	}
        else if (c == '<')
        {
	    c = next_char();
	    if (c == '<') {
		*p++ = c;
		c = next_char();
		continue;
	    }
	    while (c != '>' && c != '\n')
            {
		*p++ = c;
		c = next_char();
	    }
	    if (c == '\n')
            {
		error("missing '>'");
		*p++ = '"';
		break;
	    }
	    *p++ = c;
	}
	c = next_char();
    } while (c != EOF && !isspace(c));
    *p = '\0';

    /* skip the rest of the line */
    while (c != '\n' && c != EOF) c = next_char();

    /* check that the opcode is valid */
 check_opcode :
    
    if (assembling && !defining_macro)
    {
	if (!opcode_symbol)
	    error("unrecognised opcode");
    }
}

