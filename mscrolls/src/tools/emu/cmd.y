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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emu.h"
#include "commands.h"
#include "disopts.h"
#include "dump.h"
#include "display.h"
#include "emulate.h"
#include "printnum.h"
#include "breakpoints.h"
#include "load.h"
#include "dis1.h"
#include "errors.h"

/*#define YYDEBUG*/

extern int yylex();

int list_ad=0;	/* next address to be disassembled by list */
int dump_ad=0;	/* next address for dump */
int base=16;	/* current number base */
int last_expr=0; /* last expression calculated */
extern int warnings;
extern int moreflag;
extern int troff;
extern int tex;
extern FILE *log_file, *longlogfile, *exec_file;
%}

%token CSQ_L CSQ_W CSQ_B STRING NUMBER UMINUS
%token REG BOOLOP IF
%token EXAMINE EXAMINEV STEP NEXT GO FASTGO DUMP
%token PRINT PRINTX PRINTD RELOAD
%token MODIFY MODIFYB MODIFYW MODIFYL LIST LISTV BASE QUIT ERROR
%token BREAK DELETE LOG LONGLOG ENDLOG EXEC FILENAME
%token BREAKM DELETEM
%token CALL
%token SETD SETA SETPC
%token HELP MORE SHELLCMD
%token WARNINGS
%token TROFF
%token TEX
%token EOL

%left '+' '-' '|'
%left '*' '/' '&' '^'
%right '~'
%nonassoc UMINUS

%union {
	int number;
	char *string;
}

%type <number> expr rexpr NUMBER PRINT MODIFY SETA SETD REG BOOLOP
%type <string> STRING FILENAME SHELLCMD
%%

commands :	command EOL
		{
#if YYDEBUG
			yydebug=1;
#endif
		}
	|	commands command EOL
	|	commands error EOL
		{
			printf("bad command\n");
			yyerrok;
		}

command :	EXAMINE
			{
				displayregs();
			}
		|	EXAMINEV
			{
				displayregs_verbose();
			}
		|	STEP
			{
				step();
				displayregs();
				list_ad=PC;
			}
		|	STEP expr
			{
				PC=$2;
				step();
				displayregs();
				list_ad=PC;
			}
		|	STEP expr expr
			{
				int i;
				PC=$2;
				for (i = 0; i < $3; i++)
				    step();
				displayregs();
				list_ad=PC;
			}
		|	NEXT
			{
				next();
				displayregs();
				list_ad=PC;
			}
		|	NEXT expr
			{
				PC=$2;
				next();
				displayregs();
				list_ad=PC;
			}
		|	GO
			{
				emulate();
				displayregs();
				list_ad=PC;
			}
		|	GO expr
			{
				PC = $2;
				emulate();
				displayregs();
				list_ad=PC;
			}
		|	FASTGO
			{
				grr();
				displayregs();
				list_ad=PC;
			}
		|	FASTGO expr
			{
				PC=$2;
				grr();
				displayregs();
				list_ad=PC;
			}
		|	DUMP
			{
				dump_ad = dump(dump_ad, dump_ad+16);
			}
		|	DUMP expr
			{
				dump_ad = dump($2, $2+16);
			}
		|	DUMP expr expr
			{
				dump_ad = dump($2, $3);
			}
		|	PRINT expr
			{
				int temp=base;
				base = $1;	/* PRINT returns which base */
				printnum($2,2);	/* print number in current base */
				if (validaddr($2)) {
					printf(" contains ");
					printnum(fetch_l($2),2);
				}
				putchar('\n');
				base=temp;
			}
		|	RELOAD
			{
				int i;
				load(0,0);	/* reload code */
				PC = 0;		/* reset registers */
				for (i=0; i<8; i++) {
					Dreg[i] = 0;
					Areg[i] = 0;
				}
				Areg[7] = 128000;
			}
		|	MODIFY expr expr
			{
				if (!validaddr($2))
					printf("address not in valid range!\n");
				else {
					switch ($1) {	/* MODIFY returns size */
					case 0 :	/* byte */
						write_b($2,$3);
						break;
					case 1 :	/* word */
						write_w($2,$3);
						break;
					case 2 :	/* long */
						write_l($2,$3);
						break;
					}
				}
			}
		|	LIST
			{
				int i;
				for (i=0; i<20; i++)
					list_ad += dis(list_ad, D_PRINT|D_HEX|D_ASCII);
			}
		|	LIST expr
			{
				int i;
				list_ad = $2;
				for (i=0; i<20; i++)
					list_ad += dis(list_ad, D_PRINT|D_HEX|D_ASCII);
			}
		|	LIST expr expr
			{
				list_ad = $2;
				while (list_ad < $3)
					list_ad += dis(list_ad, D_PRINT|D_HEX|D_ASCII);
			}
		|	LISTV
			{
				int i;
				for (i=0; i<20; i++)
					list_ad += dis(list_ad, D_PRINT|D_HEX|D_PC);
			}
		|	LISTV expr
			{
				int i;
				list_ad = $2;
				for (i=0; i<20; i++)
					list_ad += dis(list_ad, D_PRINT|D_HEX|D_PC);
			}
		|	LISTV expr expr
			{
				list_ad = $2;
				while (list_ad < $3)
					list_ad += dis(list_ad, D_PRINT|D_HEX|D_PC);
			}
		|	BASE
			{
				if (base==10)
					printf("current base is base ten\n");
				else
					printf("current base is hexadecimal\n");
			}
		|	BASE STRING
			{
				if (!strcmp($2, "ten")) base=10;
				else if (!strcmp($2, "hex")) base=16;
				else printf("valid bases are 'ten' or 'hex'\n");
			}
		|	BREAK
			{
				list_bkpts();
			}
		|	BREAK expr
			{
                            set_bkpt($2, 0, -1, 0, 0, 0);
			}
		|	BREAK expr expr
			{
                            set_bkpt($2, $3, -1, 0, 0, 0);
			}
		|	BREAK expr IF REG BOOLOP expr
			{
				set_bkpt($2, 0, $4/4, $4&3, $5, $6);
			}
		|	BREAK expr expr IF REG BOOLOP expr
			{
				set_bkpt($2, $3, $5/4, $5&3, $6, $7);
			}
		|	DELETE expr
			{
				clear_bkpt($2);
			}
		|	BREAKM
			{
				list_mbkpts();
			}
		|	BREAKM expr
                        {
			    set_mbkpt($2, $2, 0, -1, 0, 0);
                        }
		|	BREAKM expr expr
			{
                            set_mbkpt($2, $3, 0, -1, 0, 0);
			}
		|	BREAKM expr expr expr
			{
                            set_mbkpt($2, $3, $4, -1, 0, 0);
			}
		|	BREAKM expr IF REG BOOLOP expr
			{
                            set_mbkpt($2, 0, $4/4, $4&3, $5, $6);
			}
		|	BREAKM expr expr IF REG BOOLOP expr
			{
				set_mbkpt($2, $3, $5/4, $5&3, $6, $7);
			}
/*
		|	BREAKM expr expr expr IF REG BOOLOP expr
			{
				set_mbkpt($2, $3, $4, $6/4, $6&3, $7, $8);
			}
*/
		|	DELETEM expr
			{
				clear_mbkpt($2);
			}
		|	QUIT
			{
				exit(0);
			}
		|	ERROR
			{
				YYERROR;
			}
		|	LOG FILENAME
			{
				log_file=fopen($2, "w");
			}
		|	LONGLOG FILENAME
			{
				longlogfile=fopen($2, "w");
			}
		|	ENDLOG
			{
				if (log_file) {
					fclose(log_file);
					log_file=0;
				}
				if (longlogfile) {
					fclose(longlogfile);
					longlogfile=0;
				}
			}
		|	EXEC FILENAME
			{
				exec_file=fopen($2,"r");
			}
		|	HELP
			{
				struct command *cp;
				printf("%-15s%-10s%-40s\n", "name", "abbrev",
					"description");
				putchar('\n');
				for (cp=commands; cp->name; cp++)
					printf("%-15s%-10s%-40s\n", cp->name, cp->abbrev,
						cp->comment);
			}
		|	MORE
			{
				moreflag = !moreflag;
				printf("moreing %s\n", moreflag?"enabled":"disabled");
			}
		|	SHELLCMD
			{
				system($1);
			}
		|	SETD expr
			{
				Dreg[$1] = $2;
			}
		|	SETA expr
			{
				Areg[$1] = $2;
			}
		|	SETPC expr
			{
				PC = $2;
			}
		|	CALL expr
			{
				call($2);
			}
		|	WARNINGS
			{
				warnings = !warnings;
				printf ("warnings %s\n", warnings?"enabled":"disabled");
			}
		|	TROFF
			{
			    troff = !troff;
			    printf(
"long logs will now %sproduce troff source\n",
				   troff ? "" : "not ");
			}
		|	TEX
			{
			    tex = !tex;
			    printf(
"long logs will now %sproduce tex source\n",
				   tex ? "" : "not ");
			}
		;

expr	:	rexpr	/* dummy called to set value of last_expr */
		{
			last_expr = $1;
			$$ = $1;
		}
/* real expressions */
rexpr	:	'[' rexpr CSQ_B
			{
				$$ = fetch_b($2);
			}
		|	'[' rexpr CSQ_W
			{
				$$ = fetch_w($2);
			}
		|	'[' rexpr CSQ_L
			{
				$$ = fetch_l($2);
			}
		|	'[' rexpr ']'
			{
				$$ = fetch_l($2);
			}
		|	rexpr '+' rexpr
			{
				$$ = $1 + $3;
			}
		|	rexpr '-' rexpr
			{
				$$ = $1 - $3;
			}
		|	rexpr '*' rexpr
			{
				$$ = $1 * $3;
			}
		|	rexpr '/' rexpr
			{
				$$ = $1 / $3;
			}
		|	'-' rexpr %prec UMINUS
			{
				$$ = -$2;
			}
		|	rexpr '|' rexpr
			{
				$$ = $1 | $3;
			}
		|	rexpr '&' rexpr
			{
				$$ = $1 & $3;
			}
		|	rexpr '^' rexpr
			{
				$$ = $1 ^ $3;
			}
		|	'~' rexpr
			{
				$$ = ~ $2;
			}
		|	'(' rexpr ')'
			{
				$$ = $2;
			}
		|	NUMBER
		|	BOOLOP	/* check for '=' */
			{
				if ($1 != 0) YYERROR;
				$$ = last_expr;
			}
		;
