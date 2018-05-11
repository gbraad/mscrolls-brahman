%{
/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2016-2018 Voidware Ltd.  All Rights Reserved.
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
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for historical reference
 * and educational purposes.
 */


#include <ctype.h>
#include <signal.h>
#include "emu.h"
#include "y.tab.h"
#include "commands.h"
#include "symbols.h"
#include "errors.h"

extern int base;	/* current number base */

int n;
char *p, *q, s[100];

extern struct command commands[];

#define YY_NO_INPUT
#define YY_NO_UNPUT

#define YY_INPUT(buf,result,max_size)           \
{                                               \
    result = get_input(buf, max_size);          \
}

#ifdef _MSC_VER
// not under windows
#define YY_NO_UNISTD_H
#endif

static char *prompt="(emu) ";

static char linebuf[100];
static int lpos;

int get_input(char* buf, int size)
{
    int n = 0;

    for (;;)
    {
        int c;

        while (size)
        {
            c = linebuf[lpos];
            if (!c) break;

            buf[n++] = c;
            ++lpos;
            --size;
        }

        if (!size) break;
        if (!c && n) break;

        // fill linebuf
        printf(prompt);
        lpos = 0;
        if (feof(stdin)) clearerr(stdin);
        while ((c = getchar()) != '\n' && c != EOF)
        {
            if (isupper(c)) c = tolower(c);
            linebuf[lpos++] = c;
        }
        linebuf[lpos++] = '\n';
        linebuf[lpos] = 0;
        lpos = 0;
    }
    return n;
}

%}

digit		[0-9]
hexdigit	[0-9a-f]
alpha		[a-z_\$\.]
space		[ \t]

%%
{space}		{/* ignore */}
^"xpc"	{
		return SETPC;
	}
^"set"\ +"pc" {
		return SETPC;
	}
^(x|("set"\ +))[aA][0-7] {
		yylval.number=yytext[yyleng-1]-'0';
		return SETA;
	}
^(x|("set"\ +))[dD][0-7] {
		yylval.number=yytext[yyleng-1]-'0';
		return SETD;
	}
^\!.* {		/* shell command */
		yylval.string = yytext+1;
		return SHELLCMD;
	}
^[a-z?]+ {
                struct command *cp;
		for (cp=commands; cp->name; cp++)
			if (!strcmp(cp->name, yytext) || !strcmp(cp->abbrev, yytext))
				switch (cp->token) {
				case PRINT :
					yylval.number=base;
					return PRINT;
				case PRINTX :
					yylval.number=16;
					return PRINT;
				case PRINTD :
					yylval.number=10;
					return PRINT;
				case MODIFYB :
					yylval.number=0;
					return MODIFY;
				case MODIFYW :
					yylval.number=1;
					return MODIFY;
				case MODIFYL :
					yylval.number=2;
					return MODIFY;
				default :
					return cp->token;
				} 
		printf("unrecognised command\n");
		return ERROR;
	}
#d[0-7] {
		yylval.number = Dreg[yytext[2] - '0'];
		return NUMBER;
	}
#a[0-7] {
		yylval.number = Areg[yytext[2] - '0'];
		return NUMBER;
	}
#sp {
		yylval.number = Areg[7];
		return NUMBER;
	}
#pc {
		yylval.number = PC;
		return NUMBER;
	}
if {
		return IF;
	}
d[0-7]\.[bwl] {
		yylval.number = (yytext[1]-'0')*4;
		if (yytext[3] == 'w') yylval.number += 1;
		if (yytext[3] == 'l') yylval.number += 2;
		return REG;
	}
d[0-7] {
		yylval.number = (yytext[1]-'0')*4+1;	/* default size word */
		return REG;
	}
a[0-7]\.[bwl] {
		yylval.number = (yytext[1]-'0'+8)*4;
		if (yytext[3] == 'w') yylval.number += 1;
		if (yytext[3] == 'l') yylval.number += 2;
		return REG;
	}
a[0-7] {
		yylval.number = (8+yytext[1]-'0')*4+2;	/* default size long */
		return REG;
	}
pc {
		yylval.number = 16*4+2;	/* PC size is always L */
		return REG;
	}
{alpha}({alpha}|{digit}|[\?\*\[\]])*	{
		p=s; q=yytext;
		while (*q)
			if (isupper(*q))
				*p++ = tolower(*q++);
			else
				*p++ = *q++;
		*p=0;
		yylval.number = sym_to_ad(s);
		return NUMBER;
	}
{digit}{hexdigit}* {
		if (base == 10) {
			n=0;
			p=yytext;
			while (*p) {
				if (!isdigit(*p))
					error("hex digit in decimal number");
				n = n*10 + *p++ - '0';
			}
			yylval.number = n;
			return NUMBER;
		} else {	/* base = 16 */
			n=0;
			p=yytext;
			while (*p) {
				n *= 16;
				if (*p <= '9') n += *p++ - '0';
				else n += *p++ - 'a' + 10;
			}
			yylval.number = n;
			return NUMBER;
		}
	}
0t{digit}+ {
		n=0;
		p=yytext+2;
		while (*p)
			n = n*10 + *p++ - '0';
		yylval.number = n;
		return NUMBER;
	}
0x{hexdigit}+ {
		n=0;
		p=yytext+2;
		while (*p) {
			n *= 16;
			if (*p <= '9') n += *p++ - '0';
			else n += *p++ - 'a' + 10;
		}
		yylval.number = n;
		return NUMBER;
	}
\]\.b	return CSQ_B;
\]\.w	return CSQ_W;
\]\.l	return CSQ_L;
\".*\"	{	/* file name in quotes */
		char *p, *q;
		p = s;
		q = yytext+1;
		while ((*p++ = *q++) != '"') ;
		p[-1]=0;	/* remove '"' & zterm */
		yylval.string = s;
		return FILENAME;
	}
"="	{	yylval.number=0; return BOOLOP; }
"!="	{	yylval.number=1; return BOOLOP; }
"<"	{	yylval.number=2; return BOOLOP; }
">"	{	yylval.number=3; return BOOLOP; }
"<="	{	yylval.number=4; return BOOLOP; }
">="	{	yylval.number=5; return BOOLOP; }
\n	return EOL;
.	return yytext[0];
%%


int yywrap() { return 1; }

