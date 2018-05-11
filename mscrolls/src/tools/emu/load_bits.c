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

/* machine dependant bits factored from load.c */

#include <stdlib.h>
#include "emu.h"
#include "m-vax.h"
#include "load_bits.h"
#include "errors.h"

#if defined(VAX) || (defined(C) && !defined(BIG_ENDIAN))

/* VAX version loads and runs the code backwards to get around the
 * Eltham byte ordering problem.
 * New C version for little endian machines works similarly. */

#ifdef VAX
u_char *m68mem_start, *m68mem_end;
#define memory_start	m68mem_start
#define memory_end		m68mem_end
u_char *wtable;
#else
u_char *memory_start, *memory_end;
u_char *wtable_start, *wtable_end;
int wtable_loaded = 0;
#endif
u_long memory_last_ad;

void load_code(u_char* ptr, int size)
{
	register u_char *p;
	if (!memory_start) {	/* need to allocate space ? */
		memory_last_ad = (size > 0x1f400) ? size : 0x1f400;
		memory_start = malloc(memory_last_ad);
		if (!memory_start)
                    fatal_error("no memory");
		memory_end = memory_start + memory_last_ad - 1;
	}
	/* load in the code backwards */
	p = memory_end;
/*	ptr -= 256;	hack to load game at $100
	size += 256;		*/
	for (; size; size--) *p-- = *ptr++;
}

#ifdef VAX

load_wtable(ptr, size)
register u_char *ptr;
{
	register u_char *p;
	if (!wtable) {	/* need to allocate space ? */
		wtable = malloc(size);
		if (!wtable)
			fatal_error("no memory");
	}
	p = wtable;
	for (; size; size--) *p++ = *ptr++;
}

#else // VAX

void load_wtable(u_char* ptr, int size)
{
	register u_char *p;
	if (!wtable_start) {	/* need to allocate space ? */
		wtable_start = malloc(size);
		if (!wtable_start)
			fatal_error("no memory");
		wtable_end = wtable_start + size - 1;
	}
	wtable_loaded = 1;
	p = wtable_end;
	for (; size; size--) *p-- = *ptr++;
}

#endif // VAX
#endif // VAX || (C && LITTLE_ENDIAN)

#if defined(XEN) || (defined(C) && defined(BIG_ENDIAN))

/* XEN needs the code loaded in the normal direction */

#ifdef XEN
u_char *code;
#define memory code
#else
u_char *memory;
#endif
u_char *wtable;
int wtable_loaded = 0;
u_long memory_last_ad;

load_code(ptr, size)
register u_char *ptr;
{
	register u_char *p;
	if (!memory) {	/* need to allocate space ? */
		memory_last_ad = (size > 0x1f400) ? size : 0x1f400;
		memory = malloc(memory_last_ad);
		if (!memory)
			fatal_error("no memory");
	}
	p = memory;
	for (; size; size--) *p++ = *ptr++;
}

load_wtable(ptr, size)
register u_char *ptr;
{
	register u_char *p;
	if (!wtable) {	/* need to allocate space ? */
		wtable = malloc(size);
		if (!wtable)
			fatal_error("no memory");
	}
	p = wtable;
	for (; size; size--) *p++ = *ptr++;
	wtable_loaded = 1;
}

#endif // XEN
