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

#pragma once

/*
 * change this to include the config file for your machine
 */
#include "m-vax.h"

/* C address */
typedef UBYTE *C_ad;

/* Eltham address */
typedef ULONG E_ad;

/*
 * Union to divide up a register in useful ways
 */
typedef union {
	struct {
#ifdef BIG_ENDIAN
		BYTE pad[3], val;
#else
		BYTE val, pad[3];
#endif
	} B;
#define b	B.val
	struct {
#ifdef BIG_ENDIAN
		WORD pad, val;
#else
		WORD val, pad;
#endif
	} W;
#define w	W.val
	LONG	l;
	struct {
#ifdef BIG_ENDIAN
		UBYTE pad[3], val;
#else
		UBYTE val, pad[3];
#endif
	} UB;
#define ub	UB.val
	struct {
#ifdef BIG_ENDIAN
		UWORD pad, val;
#else
		UWORD val, pad;
#endif
	} UW;
#define uw	UW.val
	ULONG	ul;
} REG;

/*
 * Global variables
 */

extern REG Dreg[8], Areg[8];
extern UWORD *PC;			/* a C address */
extern UWORD *oldPC;			/* value of PC at start of decode */

#ifndef ARM
extern UWORD SR;
#else
extern ULONG SR;			/* more efficient on ARM */
#endif

extern ULONG memory_last_ad;		/* last valid Eltham address */

#ifdef BIG_ENDIAN
extern C_ad memory;			/* big endians only need start */
extern C_ad wtable;
#else
extern C_ad memory_start, memory_end;
extern C_ad wtable_start, wtable_end;
#endif

extern int wtable_loaded;

/*
 * Routines
 */

#ifndef CHECKING
#define adea	ea
#define dea	ea
#define amea	ea
#define mea	ea
#define acea	ea
#define cea	ea
#define aea	ea
C_ad ea(int ins);
#else
C_ad adea(int), dea(int), amea(int), mea(int);
C_ad acea(int), cea(int), aea(int), ea(int);
#endif

LONG pull_l(void);

/*
 * Constants
 */

/* SR defines */
#define C_bit	1
#define V_bit	2
#define Z_bit	4
#define N_bit	8

/* various bits in an instruction */
#define MODE_mask	(7<<3)
#define MODE_Dreg	0
#define MODE_Areg	(1<<3)
#define MODE_ind	(2<<3)
#define MODE_ind_postinc (3<<3)
#define MODE_ind_predec	(4<<3)
#define MODE_ind_disp	(5<<3)
#define MODE_ind_indexed (6<<3)
#define MODE_pc		(7<<3)
#define MODE_abs_w	(7<<3)
#define MODE_abs_l	((7<<3)|1)
#define MODE_pcrel	((7<<3)|2)
#define MODE_pcrel_ind	((7<<3)|3)
#define MODE_immed	((7<<3)|4)

#define REG_mask	7

#define EA_mask		(MODE_mask|REG_mask)

/* op size is often in bits 6,7 */
#define SIZE_mask	(3<<6)
#define SIZE_shift	6
#define SIZE_byte	0
#define SIZE_word	1
#define SIZE_long	2

/*
 * macros
 */

/*
 * change a C address to point to the next consecutive address
 * in Eltham space. I.e. decrement for little endian, increment
 * for big endian. returns the value of the ptr before the increment
 */
#ifdef BIG_ENDIAN
#define INC(ad)		(ad)++
#else
#define INC(ad)		(ad)--
#endif

/*
 * ditto for previous and other useful macros
 */
#ifdef BIG_ENDIAN
#define DEC(ad)		(ad)--
#define ADD		+
#define SUB		-
#define DIFF(p, q)	((p) - (q))
#else
#define DEC(ad)		(ad)++
#define ADD		-
#define SUB		+
#define DIFF(p, q)	((q) - (p))
#endif

/* convert an Eltham address to a C one. size=SIZE_byte/word/long */
#ifdef BIG_ENDIAN
#define E_to_C(size,ad)		((C_ad)(memory + (int)(ad)))
#define W_to_C(ad)		((C_ad)(wtable + (int)(ad)))
#else
#define E_to_C(size,ad)		((C_ad)(memory_end - (ad) - ((1<<(size)) - 1)))
#define W_to_C(ad)		((C_ad)(wtable_end - (ad)))
#endif

/* convert a C address to an Eltham one */
#ifdef BIG_ENDIAN
#define C_to_E(size,ad)		((E_ad)((C_ad)(ad) - memory))
#define C_to_W(ad)		((E_ad)((C_ad)(ad) - wtable))
#else
/* note: this is the same as E_to_C on little endian machines */
#define C_to_E(size,ad)		((E_ad)(memory_end - (C_ad)(ad) - ((1<<(size))-1)))
#define C_to_W(ad)		((E_ad)(wtable_end - (C_ad)(ad)))
#endif

/*
 * access data at a C address.
 * note: write_x macros return the value written
 */
#define read_b(ad)		(*(BYTE *)(ad))
#define read_ub(ad)		(*(UBYTE *)(ad))
#define read_w(ad)		(*(WORD *)(ad))
#define read_uw(ad)		(*(UWORD *)(ad))
#ifndef ARM
#define read_l(ad)		(*(LONG *)(ad))
#define read_ul(ad)		(*(ULONG *)(ad))
#endif
#define write_b(v,ad)	(*(BYTE *)(ad) = (v))
#define write_ub(v,ad)	(*(UBYTE *)(ad) = (v))
#define write_w(v,ad)	(*(WORD *)(ad) = (v))
#define write_uw(v,ad)	(*(UWORD *)(ad) = (v))
#ifndef ARM				/* functions on archimedes */
#define write_l(v,ad)	(*(LONG *)(ad) = (v))
#define write_ul(v,ad)	(*(ULONG *)(ad) = (v))
#endif

/*
 * access data at the PC
 */
#define fetch_uw		(*(UWORD*)INC(PC))
#define fetch_w			(*(WORD *)INC(PC))
#define fetch_ul		((ULONG)fetch_l)
#ifdef BIG_ENDIAN
/* noxious, but I can't think of another way */
#define fetch_l			(INC(PC),INC(PC),*(LONG *)(PC-2))
#else
/* this will subtract 2 from PC to point at the low word, and then
 * read the longword, subtracting 2 more from the PC */
#ifndef ARM /* function on Archimedes */
#define fetch_l			(*(LONG *)(INC(PC),INC(PC)))
#else
#define fetch_l _fetch_l()
#endif
#endif

/*
 * extract quick immediate data from an instruction
 */
#define quick(ins)	((ins) & (7<<9) ? ((ins)>>9) & 7 : 8)

/*
 * extract a condition code from SR
 */
#define Cond_HI		(!(SR & (C_bit|Z_bit)))
#define Cond_LS		(SR & (C_bit|Z_bit))
#define Cond_CC		(!(SR & C_bit))
#define Cond_CS		(SR & C_bit)
#define Cond_NE		(!(SR & Z_bit))
#define Cond_EQ		(SR & Z_bit)
#define Cond_VC		(!(SR & V_bit))
#define Cond_VS		(SR & V_bit)
#define Cond_PL		(!(SR & N_bit))
#define Cond_MI		(SR & N_bit)
#define Cond_GE		(!(SR & N_bit))	/* note: V not supported */
#define Cond_LT		(SR & N_bit)	/* note: V not supported */
#define Cond_GT		(!(SR & (N_bit|Z_bit)))	/* note: V not supported */
#define Cond_LE		(SR & (N_bit|Z_bit))	/* note: V not supported */

/*
 * insert a particular size into the size field of the opcode
 */

#define insert_size(size, op)	((op & ~SIZE_mask) | (size << SIZE_shift))

#define setnz_b(v) \
{ \
    BYTE _t = v; \
    SR = 0; \
    if (_t < 0) SR = N_bit; \
    if (!_t) SR |= Z_bit; \
}

#define setnz_w(v) \
{ \
    WORD _t = v; \
    SR = 0; \
    if (_t < 0) SR = N_bit; \
    if (!_t) SR |= Z_bit; \
}

#define setnz_l(v) \
{ \
    LONG _t = v; \
    SR = 0; \
    if (_t < 0) SR = N_bit; \
    if (!_t) SR |= Z_bit; \
}

#define setnz_b_leave_carry(v) \
{ \
    BYTE _t = v; \
    SR &= C_bit; \
    if (_t < 0) SR |= N_bit; \
    if (!_t) SR |= Z_bit; \
}

#define setnz_w_leave_carry(v) \
{ \
    WORD _t = v; \
    SR &= C_bit; \
    if (_t < 0) SR |= N_bit; \
    if (!_t) SR |= Z_bit; \
}

#define setnz_l_leave_carry(v) \
{ \
    LONG _t = v; \
    SR &= C_bit; \
    if (_t < 0) SR |= N_bit; \
    if (!_t) SR |= Z_bit; \
}

#define push_l(v) \
{ \
    Areg[7].l -= 4; \
    write_l((v), E_to_C(SIZE_long, Areg[7].l)); \
}
