/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
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
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for posterity and
 * historical reference.
 */

#include <stdio.h>
#include "kem.h"
#include "display.h"

extern struct registers registers;
extern struct flags flags;
char *PC;
extern char *memory;
extern int InstructionCount;

void displayregs()
{
    int i;
    printf("D:  ");
    for (i=0;i<8;i++) {
        printf("%02x",registers.D[i].chars.c1);
        printf("%02x",registers.D[i].chars.c2);
        printf("%02x",registers.D[i].chars.c3);
        printf("%02x ",registers.D[i].chars.c4);
    }
	
    printf("\nA:  ");
    for (i=0;i<8;i++) {
        printf("%02x",registers.A[i].chars.c1);
        printf("%02x",registers.A[i].chars.c2);
        printf("%02x",registers.A[i].chars.c3);
        printf("%02x ",registers.A[i].chars.c4);
    }

    printf("\nPC: %08X  ",PC-memory);

    printf("Status: ");
    if (flags.Z) putchar('Z');
    else putchar('-');
    if (flags.N) putchar('N');
    else putchar('-');
    if (flags.C) putchar('C');
    else putchar('-');

    printf(" count = %06x\n",InstructionCount);
    InstructionCount=0;

    putchar('\n');
}
