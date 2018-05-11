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

#include "types.h"

#define MAXCODESIZE	0x20000
#define S_BYTE 0
#define S_WORD 1
#define S_LONG 2

extern long Dreg[8], Areg[8], PC;
extern short SR;

#ifdef VAX

extern u_char *m68mem_start, *m68mem_end;
#define fetch_b(ad)	(*(u_char *)(m68mem_end - (ad)))
#define fetch_w(ad)	(*(u_short *)(m68mem_end-1 - (ad)))
#define fetch_l(ad)	(*(u_long *)(m68mem_end-3 - (ad)))
#define write_b(ad,v)	fetch_b(ad)=v
#define write_w(ad,v)	fetch_w(ad)=v
#define write_l(ad,v)	fetch_l(ad)=v

#endif // VAX

#if defined(C) || defined(ARM)

extern u_char *memory_start, *memory_end;
#define fetch_b(ad)	(*(u_char *)(memory_end - (ad)))
#define fetch_w(ad)	(*(u_short *)(memory_end-1 - (ad)))
#define fetch_l(ad)	(*(u_long *)(memory_end-3 - (ad)))
#define write_b(ad,v)	fetch_b(ad)=v
#define write_w(ad,v)	fetch_w(ad)=v
#define write_l(ad,v)	fetch_l(ad)=v

#endif
#ifdef XEN

extern u_char *code, *wtable;
#define fetch_b(ad)	(*(u_char *)(code + (ad)))
#define fetch_w(ad)	((fetch_b(ad)<<8) + fetch_b((ad)+1))
#define fetch_l(ad)	((fetch_w(ad)<<16) + fetch_w((ad)+2))
#define write_b(ad,v)	code[ad]=v
#define write_w(ad,v)	{ write_b(ad,(v)>>8); write_b((ad)+1,(v)&0xff); }
#define write_l(ad,v)	{ write_w(ad,(v)>>16); write_w((ad)+2,(v)&0xffff); }

#endif //XEN

#define validaddr(ad)	((ad)>=0 && (ad)<128000)

