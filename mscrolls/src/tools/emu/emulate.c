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
#include <setjmp.h>
#include <signal.h>
#include "emuc.h"
#include "emulate.h"
#include "dis1.h"
#include "decode.h"
#include "breakpoints.h"
#include "display.h"

extern jmp_buf command;

void init_backend() {}

int ins_size()
{
    // XXX
    return dis((int)PC,0);		/* flags=0 => print nothing */
}

void step()
{
	PC = (UWORD *)E_to_C(SIZE_word,(int)PC);
	decode();
	PC = (UWORD *)C_to_E(SIZE_word,PC);
}

void next()
{
	int t;
	C_ad nextins;
	ULONG oldA7;
	t = ins_size();
	PC = (UWORD *)E_to_C(SIZE_word,(int)PC);
	nextins = (C_ad)PC ADD t;
	oldA7 = Areg[7].l;
	setintr();
	do {
		decode();
	} while ((C_ad)PC != nextins && DIFF(Areg[7].l, oldA7) > 0);
	unsetintr();
	PC = (UWORD *)C_to_E(SIZE_word,PC);
}

void emulate()
{
	PC = (UWORD *)E_to_C(SIZE_word,(int)PC);
	setintr();
	for (;;) {
		decode();
		do_breakpoints();
	}
}

void grr()
{
	PC = (UWORD *)E_to_C(SIZE_word,(int)PC);
	setintr();
	for (;;) {
		decode();
	}
}

void call(int ad)
{
	UWORD *tempPC;
	tempPC = PC;
	setintr();
	push_l(-1);
	PC = (UWORD *)E_to_C(SIZE_word, ad);
	do {
		decode();
	} while (PC != (UWORD *)E_to_C(SIZE_word, -1));
	PC = tempPC;
	unsetintr();
}

void do_breakpoints()
{
    //extern long bkpt[], bkpt_cond[][2];
    //extern int n_bkpts;
	int i, ad;

	ad = C_to_E(SIZE_word,PC);
	for (i=0; i<n_bkpts; i++) {
		if (bkpt[i] == -1) continue;
		if (ad == bkpt[i])
                    if (check_bkpt(&bkpt_cond[i]))
				hit_breakpoint();
	}
}

void hit_breakpoint()
{
	printf("\nBreakpoint reached\n");
	PC = (UWORD *)C_to_E(SIZE_word, PC);
	displayregs();
	unsetintr();
	longjmp(command,3);
}

static void (*oldintr)(int);

static void myintr(int s)
{
	unsetintr();
	PC = (UWORD *)C_to_E(SIZE_word, oldPC);
	printf("\nWarning:  interrupted mid instruction.\n");
	displayregs();
	longjmp(command,0);
}


void setintr()
{
	oldintr = signal(SIGINT, myintr);
}

void unsetintr()
{
	signal(SIGINT, oldintr);
}

