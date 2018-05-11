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

#include <setjmp.h>
#include <stdio.h>
#include "emu.h"
#include "symbols.h"
#include "breakpoints.h"
#include "printnum.h"

extern jmp_buf command;

int bkpt[MAXBKPTS];
struct bpcond bkpt_cond[MAXBKPTS];
int n_bkpts=0;

void init_bkpts()
{
    int i;

	for (i=0; i<MAXBKPTS; i++)
		bkpt[i] = -1;
}

void set_bkpt(int ad, int count, int regnum, int size, int cond, int value)
{
    int i;
	char *p;

	/* check if already set */
	for (i=0; i<n_bkpts; i++)
		if (bkpt[i] == ad) {
                    printf("There is already a breakpoint at ");
                    p = sym_from_ad(ad, 0);
			if (p) printf("%s\n", p);
			else {
				printnum(ad,2);
				putchar('\n');
			}
			return;
		}
	/* find free slot */
	for (i=0; i<n_bkpts; i++)
		if (bkpt[i] == -1) {
			printf("breakpoint %d set\n", i);
			bkpt[i] = ad;
			bkpt_cond[i].count = count;
			if (regnum >= 0) {
				bkpt_cond[i].size = size;
				bkpt_cond[i].cond = cond;
				bkpt_cond[i].regnum  = regnum;
				bkpt_cond[i].value = value;
			} else
				bkpt_cond[i].size = 3;
			return;
		}
	printf("breakpoint %d set\n", n_bkpts);
	bkpt[i] = ad;
	bkpt_cond[i].count = count;
	if (regnum >= 0) {
		bkpt_cond[i].size = size;
		bkpt_cond[i].cond = cond;
		bkpt_cond[i].regnum  = regnum;
		bkpt_cond[i].value = value;
	} else
		bkpt_cond[i].size = 3;
	n_bkpts++;
	return;
}

void clear_bkpt(int n)
{
	printf("breakpoint %d cleared\n", n);
	bkpt[n] = -1;
	while (bkpt[n_bkpts-1] == -1) n_bkpts--;
}

void list_bkpts()
{
	char *p;
	int i;

	for (i=0; i<n_bkpts; i++) {
		if (bkpt[i] != -1) {
			printnum(i,0);
			printf(" - ");
			p = sym_from_ad(bkpt[i], 0);
			if (p) {
				printf("%s=", p);
				printnum(bkpt[i],2);
			} else
				printnum(bkpt[i],2);
			printf(" contains ");
			printnum(fetch_l(bkpt[i]),2);
			printcond(bkpt_cond[i]);
		}
	}
}

int mbkpt[MAXBKPTS];
struct bpcond mbkpt_cond[MAXBKPTS];
int n_mbkpts=0;

void init_mbkpts()
{
	int i;

	for (i=0; i<MAXBKPTS; i++)
		mbkpt[i] = -1;
}

void set_mbkpt(int ad, int count, int regnum, int size, int cond, int value)
{
	int i;
	char *p;

	/* check if already set */
	for (i=0; i<n_mbkpts; i++)
		if (mbkpt[i] == ad) {
			printf("There is already a breakpoint at ");
			p = sym_from_ad(ad, 0);
			if (p) printf("%s\n", p);
			else {
				printnum(ad,2);
				putchar('\n');
			}
			return;
		}
	/* find free slot */
	for (i=0; i<n_mbkpts; i++)
		if (mbkpt[i] == -1) {
			printf("memory breakpoint %d set\n", i);
			mbkpt[i] = ad;
			mbkpt_cond[i].count = count;
			if (regnum >= 0) {
				mbkpt_cond[i].size = size;
				mbkpt_cond[i].cond = cond;
				mbkpt_cond[i].regnum  = regnum;
				mbkpt_cond[i].value = value;
			} else
				mbkpt_cond[i].size = 3;
			return;
		}
	printf("memory breakpoint %d set\n", n_mbkpts);
	mbkpt[i] = ad;
	mbkpt_cond[i].count = count;
	if (regnum >= 0) {
		mbkpt_cond[i].size = size;
		mbkpt_cond[i].cond = cond;
		mbkpt_cond[i].regnum  = regnum;
		mbkpt_cond[i].value = value;
	} else
		mbkpt_cond[i].size = 3;
	n_mbkpts++;
	return;
}

void clear_mbkpt(int n)
{
	printf("memory breakpoint %d cleared\n", n);
	mbkpt[n] = -1;
	while (mbkpt[n_mbkpts-1] == -1) n_mbkpts--;
}

void list_mbkpts()
{
	char *p;
	int i;

	for (i=0; i<n_mbkpts; i++) {
		if (mbkpt[i] != -1) {
			printnum(i,0);
			printf(" - ");
			p = sym_from_ad(mbkpt[i], 0);
			if (p) {
				printf("%s=", p);
				printnum(mbkpt[i],2);
			} else
				printnum(mbkpt[i],2);
			printf(" contains ");
			printnum(fetch_l(mbkpt[i]),2);
			printcond(mbkpt_cond[i]);
		}
	}
}

void printcond(struct bpcond cond)
{	
	if (cond.count) {
		printf(" count=");
		printnum(cond.count,1);
	}
	if (cond.size != 3) {
		printf(" stop only if ");
		if (cond.regnum < 8)
			printf("D%d", cond.regnum);
		else if (cond.regnum < 16)
			printf("A%d", cond.regnum-8);
		else
			printf("PC");
		putchar('.');
		switch (cond.size) {
		case 0 :
			putchar('B');
			break;
		case 1 :
			putchar('W');
			break;
		case 2 :
			putchar('L');
			break;
		}
		switch (cond.cond) {
		case 0 :
			printf(" = ");
			break;
		case 1 :
			printf(" != ");
			break;
		case 2 :
			printf(" < ");
			break;
		case 3 :
			printf(" > ");
			break;
		case 4 :
			printf(" <= ");
			break;
		case 5 :
			printf(" >= ");
			break;
		}
		printnum(cond.value,2);
	}
	putchar('\n');
}

int check_bkpt(struct bpcond* cond)
{
	int reg;

	if (cond->count) {
		cond->count--;
		return 0;
	}

	/* is the breakpoint conditional ? */
	if (cond->size == 3) return 1;

	if (cond->regnum < 8) 
		reg = Dreg[cond->regnum];
	else if (cond->regnum < 16)
		reg = Areg[cond->regnum-8];
	else if (cond->regnum == 16)
		reg = PC;
	else {
		printf("help help the breakpoints have gone wild!!!\n");
		longjmp(command,1);
	}

	/* sign extend the register if needed */
	if (cond->size == 0)
		reg |= (reg & 0x80) ? 0xffffff00 : 0;
	else if (cond->size == 1)
		reg |= (reg & 0x8000) ? 0xffff0000 : 0;
	
	/* finally, do the compare */
	switch (cond->cond) {
	case 0 :
		return reg == cond->value;
	case 1 :
		return reg != cond->value;
	case 2 :
		return reg < cond->value;
	case 3 :
		return reg > cond->value;
	case 4 :
		return reg <= cond->value;
	case 5 :
		return reg >= cond->value;
	}
	printf("wild breakpoint (man)!!\n");
	longjmp(command,1);
        return 1;
}
