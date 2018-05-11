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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "emu.h"
#include "dis.h"
#include "dis1.h"
#include "dis2.h"
#include "dis3.h"
#include "symbols.h"
#include "bits.h"
#include "disopts.h"
#include "printnum.h"

int pc;
int instrwords;
int maggot_rolls = TRUE;
int noprint=0;
char string[400];

int byteext (int n) { return (n > 127) ? (n - 256) : n; }
int wordext(int n) { return (n > 32767) ? (n - 65536) : n; }    /* assumes 32 bit int */
int addrext(int n) { return (n > 0X7FFFFF) ? (n - 0X1000000) : n; }   /* ditto */
int shortval(int n) { return n == 0 ? 8 : n; }


int readword() 
{
    int word;
    char *p;

	word = wordext(fetch_w(pc));
	if (!noprint && instrwords == 0) {
		p = sym_from_ad(pc,1);	/* only returns str if ad's exactly equal */
		if (p) printf("%-8s ", p);
		else printf("         ");
	}
	instrwords++;
	pc += 2;
	return word;
}

char *addr_str(int ad)
{
    char *p;

    p = sym_from_ad(ad, 0);		/* returns "fred+n" if close to fred */
    if (p) return p;
    return sym_create_label(ad);
}



int opsize(int op, int a)
{
	switch (op) {
		case i_ori:
		case i_andi:
		case i_subi:
		case i_addi:
		case i_eori:
		case i_cmpi:
		case i_negx:
		case i_clr:
		case i_neg:
		case i_not:
		case i_TEST:
		case i_tst:
		case i_addq:
		case i_subq:
		case i_or:
		case i_sub:
		case i_subx:
		case i_cmp:
		case i_cmpm:
		case i_eor:
		case i_and:
		case i_add:
		case i_addx:
		case i_dshift: return bits(a, 7, 6);

		case i_move: switch (bits(a, 13, 12)) {
			 case 1: return 0;
			 case 2: return 2;
			 case 3: return 1;
		}
		case i_movea: return bits(a, 13, 12) == 3 ? 1 : 2;

		case i_adda:
		case i_suba:
		case i_cmpa: return bits(a, 8, 6) == 3 ? 1 : 2;

		case i_mmtreg:
		case i_mmfreg:
		case i_movep: return bits(a, 6, 6) == 0 ? 1 : 2;

		case i_dbit:
		case i_sbit:
		case i_scc:
			return 0;

		default: return 1;
	}
}

const char* condcode(int op, int a)
{
    switch (bits(a, 11, 8))
    {
		case  4: return "CC";
		case  5: return "CS";
		case  7: return "EQ";
		case 12: return "GE";
		case 14: return "GT";
		case  2: return "HI";
		case 15: return "LE";
		case  3: return "LS";
		case 13: return "LT";
		case 11: return "MI";
		case  6: return "NE";
		case 10: return "PL";
		case  8: return "VC";
		case  9: return "VS";
		case  0: return op == i_bcc ? "RA" : "T ";
		case  1: return op == i_bcc ? "SR" : "F ";
	}
    return "";
}

void dprint(const char* form, ...)
{
    va_list args;
    va_start(args, form);

    vsprintf(string + strlen(string), form, args);
    va_end(args);
}


void writeop(char* op, int n)
{
    assert(op);
	int s=0;
	while (op[s] != 0) s++;
	dprint (op);
	if (n == 0 || n == 4) dprint(".B");
	if (n == 1 || n == 5) dprint(".W");
	if (n == 2 || n == 6) dprint(".L");
	if (n >= 0) s = s + 2;
	for (; s<=8; s++) dprint(" ");
}


int dis(int ad, int flags)
{
    int op, a, i, c;

	pc = ad;
	instrwords=0;
	string[0]=0;
	noprint=!(flags&D_PRINT);
	if (flags&D_PRINT && flags&D_PC)
		printf("%05x ", pc);
	a = readword();
	op = readinstr(a);
	writeinstr(op, a);
	if (!(flags&D_PRINT)) return pc-ad;
	printf("%-35s ", string);
	if (flags&D_HEX) {
		for (i=ad; i<pc; i+=2) {
                    printnum(fetch_w(i),1);
			putchar(' ');
		}
		for (; i<ad+10; i+= 2)	/* max 10 byte instructions */
			printf("     ");
	}
	if (flags&D_ASCII) {
		for (i=ad; i<pc; i++) {
			c = fetch_b(i);
			if ((c & 0x7f) < 32) putchar('.');
			else putchar(c&0x7f);
		}
	}
	putchar('\n');
	return pc-ad;
}


