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
#include "ea.h"
#include "illegal.h"

#if defined(EMU) && defined(CHECKING)
static C_ad check_ea();
#else
#define check_ea	E_to_C
#endif

#if defined(EMU) && defined(CHECKING)

static void address_error(const char* s)
{
    printf("Address Error : %s\n", s);
    illegal();
}

/* check that an Eltham address is valid and convert to a C address */
static C_ad check_ea(int size, E_ad ad)
{
    if (ad > memory_last_ad) address_error("out of range");
    if (size != SIZE_byte && (ad & 1))
        address_error("address must be even");
    return E_to_C(size, ad);
}


#endif // EMU


#ifdef CHECKING
C_ad dea(int ins) { return ea(ins); }
C_ad adea(int ins) { return ea(ins); }
C_ad cea(int ins) { return ea(ins); }
C_ad acea(int ins) { return ea(ins); }
C_ad mea(int ins) { return ea(ins); }
C_ad amea(int ins) { return ea(ins); }
C_ad aea(int ins) { return ea(ins); }
#endif

C_ad ea(int ins)
{
    int size = (ins & SIZE_mask) >> SIZE_shift;
    int reg = ins & REG_mask;
    C_ad temp_C_ad;
    E_ad temp_E_ad;
    REG ext;

    switch (ins & MODE_mask) {
	/* Dn */
	case MODE_Dreg :
#ifdef BIG_ENDIAN
		return (C_ad)&Dreg[reg].l + 4 - (1<<size);
#else
		return (C_ad)&Dreg[reg].l;
#endif

	/* An */
	case MODE_Areg :
#ifdef CHECKING
		if (!size) address_error("byte access illegal");
#endif

#ifdef BIG_ENDIAN
		return (C_ad)&Areg[reg].l + 4 - (1<<size);
#else
		return (C_ad)&Areg[reg].l;
#endif

	/* (An) */
	case MODE_ind :
		return check_ea(size, Areg[reg].l);

	/* (An)+ */
	case MODE_ind_postinc :
		temp_C_ad = check_ea(size, Areg[reg].l);
		Areg[reg].l += 1<<size;
		return temp_C_ad;

	/* -(An) */
	case MODE_ind_predec :
		Areg[reg].l -= 1<<size;
		return check_ea(size, Areg[reg].l);
	
	/* d16(An) */
	case MODE_ind_disp :
		return check_ea(size, Areg[reg].l + fetch_w);	/* note: signed fetch */

	/* d8(An,Xn.Z) */
	case MODE_ind_indexed :
		ext.l = fetch_uw;
		temp_E_ad = Areg[reg].l;
		reg = (ext.l >> 12) & 15;
		temp_E_ad += ext.l & 0x0800 ? Dreg[reg].l : Dreg[reg].w;
		return check_ea(size, temp_E_ad + ext.b);	/* sign extend byte disp */
		break;

	case MODE_pc :
		switch (reg) {
		/* abs.w */
		case 0 :
			return check_ea(size, fetch_w);	/* signed */

		/* abs.l */
		case 1 :
			return check_ea(size, fetch_l);
		
		/* d16(PC) */
		case 2 :
			temp_E_ad = C_to_E(SIZE_word, PC);
			return check_ea(size, temp_E_ad + fetch_w);	/* signed offset */

		/* d8(PC,Xn.Z) */
		case 3 :
			temp_E_ad = C_to_E(SIZE_word, PC);
			ext.l = fetch_uw;
			reg = (ext.l >> 12) & 15;
			temp_E_ad += ext.l & 0x0800 ? Dreg[reg].l : Dreg[reg].w;
			return check_ea(size, temp_E_ad + ext.b);	/* signed byte disp */
			break;

		/* immediate */
		case 4 :
			switch (size) {
			case SIZE_byte :
#ifdef BIG_ENDIAN
			        INC(PC); return ((C_ad)PC) SUB 1;
#endif
			case SIZE_word :
			        INC(PC); return (C_ad)(PC SUB 1);
			case SIZE_long :
#ifndef ARM
				return (C_ad)&fetch_l;
#else
				/* fetch_l works differently on ARM, so .. */
				PC -= 2; return PC + 1;
#endif
			}

#ifdef CHECKING
		default :
			address_error("illegal effective address");
#endif
		}
	}
    return 0;
}


