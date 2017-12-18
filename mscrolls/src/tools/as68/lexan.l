%{
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
 */

/* This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include <ctype.h>
#include "inttype.h"
#include "symbols.h"
#include "expr.h"
#include "tokens.h"
#include "directives.h"
#include "macros.h"
#include "errors.h"

extern char *macro_ptr;
extern inttype local_index; /* number to be added to the end of xx$ */
extern int in_macro;
extern int defining_macro;
extern int n_macro_calls; /* number of times any macro has been used */
extern inttype n_macro_args; /* number of args in current macro */
extern char macro_arg[][MAXMACROARGS][MAXMACROARGLEN];
extern inttype assembling;
static char s[_PATH_MAX];
extern inttype lowercase;

static void strcpy_lowercase(char* p, char* q)
{
    while (*q)
    {
        if (isupper(*q)) *p = tolower(*q);
        else *p = *q;
        p++; q++;
    }
    *p = '\0';
}

/* use my own input routines */
//#undef unput
//#define YY_NO_UNPUT
#define YY_NO_INPUT
#include "getline.h"

#define YY_INPUT(buf,result,max_size)           \
{                                               \
    int c, n = 0;                               \
    while (n < max_size)                        \
    {                                           \
        if ((c = input()) == 0) break;          \
        buf[n++] = c;                           \
    }                                           \
    result = n;                                 \
}

#ifdef _MSC_VER
// not under windows
#define YY_NO_UNISTD_H
#endif

%}

alpha		[a-zA-Z_]
digit		[0-9]
hexdigit	[0-9a-fA-F]
label		(({alpha}|\.)({alpha}|{digit}|\.)*)|({digit}+\$)
space		[ \t]

%%

"<<"		return T_LSHIFT;
">>"		return T_RSHIFT;
[dD][0-7]	{
				yylval.number = yytext[1] - '0';
				return T_DREG;
			}
[dD][0-7]\.[bBwWlL] {
				unput(yytext[yyleng-1]);
				unput('.');
				yylval.number = yytext[1] - '0';
				return T_DREG;
			}
[aA][0-7]	{
				yylval.number = yytext[1] - '0';
				return T_AREG;
			}
[aA][0-7]\.[bBwWlL] {
				unput(yytext[yyleng-1]);
				unput('.');
				yylval.number = yytext[1] - '0';
				return T_AREG;
			}
[pP][cC]	{
				return T_PC;
			}
\".*\"		{	/* string */
				yylval.string = yytext+1;
				yytext[yyleng-1] = '\0';
				return T_STRING;
			}
'([^']|(''))*'	{
				int i;
				char *p=yytext+1;
				char *q=s;
				for (i=1; i<yyleng-1; i++)
					if (*p != '\'') *q++ = *p++;
					else {	/* only copy one ' */
						*q++ = *p++;
						p++;
						i++;
					}
				*q = '\0';
				yylval.string = s;
				return T_ASCII;
			}
\.[bB]		{
				yylval.number = 0;
				return T_SIZE;
			}
\.[wW]		{
				yylval.number = 1;
				return T_SIZE;
			}
\.[lL]		{
				yylval.number = 2;
				return T_SIZE;
			}
{label}		{
				//int len = strlen(yytext);
				struct symbol *sp;

				strcpy_lowercase(s, yytext);	/* lcase it */

				sp = find_symbol(s);
				if (sp) {
					switch (sp->type) {
					case S_OPCODE :
						error("opcode in operand field");
					case S_TOKEN :
						if (sp->svalue == T_NARG) {
							yylval.number = in_macro?n_macro_args:0;
							return T_NUMBER;
						}
						return sp->svalue;
					case S_DREG :
						yylval.number = sp->svalue;
						return T_DREG;
					case S_AREG :
						yylval.number = sp->svalue;
						return T_AREG;
					case S_CONTROL :
						yylval.number = sp->svalue;
						return T_CONTROL;
					}
				}
				if (!lowercase) {
					/* look up the non-lcased one */
				    sp = find_symbol(yytext);
				}
				if (sp)
					switch (sp->type) {
					case S_MACRO :
						error("macro name in operand field");
					}
				yylval.symbol = sp;
				return T_LABEL;
			}
{digit}+	{
				yylval.number = atoi(yytext);
				return T_NUMBER;
			}
"$"{hexdigit}+ {
				sscanf(yytext+1, "%x", &yylval.number);
				return T_NUMBER;
			}
"%"[01]+	{
				int i, n=0;
				for (i=1; i<yyleng; i++) {
					n *= 2;
					n += yytext[i]=='1';
				}
				yylval.number = n;
				return T_NUMBER;
			}
.			return yytext[0];
\n			return '\n';

%%


int lex()
{
    return yylex();
}

void lex_unput(int c)
{
    unput(c);
}

/* force the last token back onto the input */
void unlex()
{
    char *p;
    p = yytext+yyleng;
    while (p>yytext) unput(*--p);
}
