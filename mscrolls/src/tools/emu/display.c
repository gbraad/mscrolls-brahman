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
#include "dis.h"
#include "emu.h"
#include "symbols.h"
#include "disopts.h"
#include "display.h"
#include "printnum.h"
#include "dis1.h"
#include "symbols.h"

int inscount=0;

void displayregs()
{
	int i;
	for (i=0; i<8; i++)
		printf("        %d", i);
	printf("\nD:  ");
	for (i=0;i<8;i++) {
		printnum(Dreg[i],2); putchar(' ');
	}
	
	printf("\nA:  ");
	for (i=0;i<8;i++) {
		printnum(Areg[i],2); putchar(' ');
	}

	printf("\nPC: ");
	printnum(PC,2);
	putchar(' ');

	printf("Status: ");
	if (SR & 4) putchar('Z');
	else putchar('-');
	if (SR & 8) putchar('N');
	else putchar('-');
	if (SR & 1) putchar('C');
	else putchar('-');
	printf(" count = ");
	printnum(inscount,2);
	putchar('\n');
	inscount=0;

	dis(PC,D_PRINT|D_HEX|D_ASCII);
}

void displayregs_verbose()
{
    int i;
    char *p;

	printf("%-5s%-10s%-12s%s\n", "reg", "hex", "decimal", "symbol");
	for (i=0; i<8; i++) {
            p = sym_from_ad(Dreg[i], 0);
		if (!p) p="";
		printf("D%d - %08x  %-10d  %s\n", i,
                       (int)Dreg[i], (int)Dreg[i], p);
	}
	for (i=0; i<8; i++) {
            p = sym_from_ad(Areg[i], 0);
		if (!p) p="";
		printf("A%d - %08x  %-10d  %s\n", i,
                       (int)Areg[i], (int)Areg[i], p);
	}
	p = sym_from_ad(PC, 0);
	if (!p) p="";
	printf("PC - %08x  %-10d  %s\n", (int)PC, (int)PC, p);
	printf("instruction:\n");
	dis(PC,D_PRINT|D_HEX|D_ASCII);
}
