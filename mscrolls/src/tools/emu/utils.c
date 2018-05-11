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
#include "utils.h"
#include "emulate.h"
#include "display.h"

#ifndef STANDALONE
#include <setjmp.h>
extern jmp_buf command;
#endif

LONG pull_l()
{
    LONG v;
    v = read_l(E_to_C(SIZE_long, Areg[7].l));
    Areg[7].l += 4;
    return v;
}

void eltham_error(const char* s)
{
#ifndef STANDALONE
    printf("%s\n", s);
#endif
    illegal();
}

void illegal()
{
#ifndef STANDALONE
    unsetintr();
    printf("Illegal instruction at PC=%08x\n", (int)C_to_E(SIZE_word,oldPC));
    PC = (UWORD *)C_to_E(SIZE_word, oldPC);
    displayregs();
	longjmp(command,0);
#else
	printf("Internal error 1 at %08x\n", C_to_E(SIZE_word, oldPC));
	abort();
#endif
}

void unimp()
{
#ifndef STANDALONE
	unsetintr();
	printf("Unimplemented instruction at PC=%08x\n", (int)C_to_E(SIZE_word,oldPC));
	PC = (UWORD *)C_to_E(SIZE_word, oldPC);
	displayregs();
	longjmp(command,0);
#else
	printf("Internal error 2 at %08x\n", C_to_E(SIZE_word, oldPC));
	abort();
#endif
}
