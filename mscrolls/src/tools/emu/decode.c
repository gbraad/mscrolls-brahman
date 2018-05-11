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

#include "emuc.h"
#include "decode.h"
#include "utils.h"

typedef int (*int_fp)();

extern int_fp decodetable[];
#ifndef STANDALONE
UWORD *oldPC;
extern int inscount;
#endif


void iMMfreg_W(int), iMMfreg_L(int);

/*
 * decode one instruction and call the specified instruction routine
 */
void decode()
{
    UWORD ins;

#ifndef STANDALONE
	oldPC = PC;
	inscount++;
#endif
	ins = fetch_uw;
	(void)(*decodetable[(ins>>6) & 1023])(ins);
}

static void or_b(int src, C_ad dest)
{
	setnz_b(write_b(src | read_b(dest), dest));
}

static void or_w(int src, C_ad dest)
{
	setnz_w(write_w(src | read_w(dest), dest));
}

static void or_l(int src, C_ad dest)
{
	setnz_l(write_l(src | read_l(dest), dest));
}

static void and_b(int src, C_ad dest)
{
    setnz_b(write_b(src & read_b(dest), dest));
}

static void
eor_b(int src, C_ad  dest)
{
	setnz_b(write_b(src ^ read_b(dest), dest));
}

static void
eor_w(int src, C_ad dest)
{
	setnz_w(write_w(src ^ read_w(dest), dest));
}

static void
eor_l(int src, C_ad dest)
{
	setnz_l(write_l(src ^ read_l(dest), dest));
}


static void
and_l(int src, C_ad dest)
{
	setnz_l(write_l(src & read_l(dest), dest));
}

static void
and_w(int src, C_ad dest)
{
	setnz_w(write_w(src & read_w(dest), dest));
}

#ifdef __GNUC__
inline
#endif
static void
add_b(int src, C_ad dest, int carry)
{
	src += read_ub(dest) + carry;
	SR &= ~C_bit;
	setnz_b(write_b(src, dest));
	if (src & 0x100) SR |= C_bit;
}

#ifdef __GNUC__
inline
#endif
static void
add_w(int src, C_ad dest, int carry)
{
	src += read_uw(dest) + carry;
	SR &= ~C_bit;
	setnz_w(write_w(src, dest));
	if (src & 0x10000) SR |= C_bit;
}

#ifdef __GNUC__
inline
#endif
static void
add_l(int src, C_ad dest, int carry)
{
#define HI	(1<<31)
	register LONG topsrc, topdst, topresult;
	register LONG result;
	topsrc = src & HI;
	result = read_l(dest);
	topdst = result & HI;
	result = (result & ~HI) + (src & ~HI) + carry;
	topresult = result & HI;
	setnz_l(write_l(result + topsrc + topdst, dest));
	SR &= ~C_bit;
	if (topsrc & topdst || topsrc & topresult || topdst & topresult)
		SR |= C_bit;
}


void
iORI_B(int ins)
{
	int data;
	data = fetch_uw & 0xff;		/* make sure its done before the adea */
	if ((ins & EA_mask) == MODE_immed)	/* OR to CCR */
		SR |= data;
	else
		or_b(data, adea(ins));
}

void
iORI_W(int ins)
{
	int data;
	data = fetch_uw;		/* make sure its done before the adea */
	if ((ins & EA_mask) == MODE_immed)
		SR |= data;
	else
		or_w(data, adea(ins));
}


void
iORI_L(int ins)
{
	int data;
	data = fetch_l;		/* make sure its done before the adea */
	or_l(data, adea(ins));
}

static void
btst(int data, int ins)
{
	SR |= Z_bit;
	/* check for BTST Ds,Dd */
	if ((ins & MODE_mask) == MODE_Dreg)
		if (Dreg[ins & REG_mask].l & (1 << (data & 31)))
			SR &= ~Z_bit;
		else ;
	else
		if (read_b(dea(insert_size(SIZE_byte,ins))) & (1 << (data & 7)))
			SR &= ~Z_bit;
}

static void
bset(int data, int ins)
{
	register ULONG mask, temp;
	register C_ad dest;

	SR |= Z_bit;
	if ((ins & MODE_mask) == MODE_Dreg) {
		mask = 1 << (data & 31);
		if (Dreg[ins & REG_mask].l & mask) SR &= ~Z_bit;
		Dreg[ins & REG_mask].l |= mask;
	} else {
		mask = 1 << (data & 7);
		dest = adea(insert_size(SIZE_byte,ins));
		temp = read_b(dest);
		if (temp & mask) SR &= ~Z_bit;
		write_b(temp | mask, dest);
	}
}

static void
bclr(int data, int ins)
{
	register ULONG mask, temp;
	register C_ad dest;

	SR |= Z_bit;
	if ((ins & MODE_mask) == MODE_Dreg) {
		mask = 1 << (data & 31);
		if (Dreg[ins & REG_mask].l & mask) SR &= ~Z_bit;
		Dreg[ins & REG_mask].l &= ~mask;
	} else {
		mask = 1 << (data & 7);
		dest = adea(insert_size(SIZE_byte,ins));
		temp = read_b(dest);
		if (temp & mask) SR &= ~Z_bit;
		write_b(temp & ~mask, dest);
	}
}

static void
bchg(int data, int ins)
{
	register ULONG mask, temp;
	register C_ad dest;

	SR |= Z_bit;
	if ((ins & MODE_mask) == MODE_Dreg) {
		mask = 1 << (data & 31);
		if (Dreg[ins & REG_mask].l & mask) SR &= ~Z_bit;
		Dreg[ins & REG_mask].l ^= mask;
	} else {
		mask = 1 << (data & 7);
		dest = adea(insert_size(SIZE_byte,ins));
		temp = read_b(dest);
		if (temp & mask) SR &= ~Z_bit;
		write_b(temp ^ mask, dest);
	}
}


void
iBTSTDn(int ins)
{
    btst(Dreg[(ins>>9)&7].l, ins);
}

void
iBSETDn(int ins)
{
	bset(Dreg[(ins>>9)&7].l, ins);
}

void
iBCLRDn(int ins)
{
	bclr(Dreg[(ins>>9)&7].l, ins);
}

void
iBCHGDn(int ins)
{
	bchg(Dreg[(ins>>9)&7].l, ins);
}


void
iANDI_B(int ins)
{
	int data;
	data = fetch_uw & 0xff;		/* make sure its done before the adea */
	if ((ins & EA_mask) == MODE_immed)
		SR &= data;
	else
		and_b(data, adea(ins));
}


void
iANDI_W(int ins)
{
	int data;
	data = fetch_uw;		/* make sure its done before the adea */
	if ((ins & EA_mask) == MODE_immed)
		SR &= data;
	else
		and_w(data, adea(ins));
}


void
iANDI_L(int ins)
{
	int data;
	data = fetch_l;		/* make sure its done before the adea */
	and_l(data, adea(ins));
}


#define sub_b(src, dst) \
	{ \
		add_b(src ^ 0xff, dst, 1); \
		SR ^= C_bit; \
	}

#define sub_w(src, dst) \
	{ \
		add_w(src ^ 0xffff, dst, 1); \
		SR ^= C_bit; \
	}

#define sub_l(src, dst) \
	{ \
		add_l(~src, dst, 1); \
		SR ^= C_bit; \
	}


void
iSUBI_B(int ins)
{
	register UBYTE data;
	data = fetch_uw & 0xff;
	sub_b(data, adea(ins));
}

void
iSUBI_W(int ins)
{
	register UWORD data;
	data = fetch_w & 0xffff;
	sub_w(data, adea(ins));
}

void
iSUBI_L(int ins)
{
	int data;
	data = fetch_l;
	sub_l(data, adea(ins));
}

void
iADDI_B(int ins)
{
	register UBYTE data;
	data = fetch_uw;
	add_b(data, adea(ins), 0);
}

void
iADDI_W(int ins)
{
	register UWORD data;
	data = fetch_uw;
	add_w(data, adea(ins), 0);
}

void
iADDI_L(int ins)
{
	int data;
	data = fetch_l;
	add_l(data, adea(ins), 0);
}

void iBTSTI(int ins)
{
	btst(fetch_uw, ins);
}

void iBCHGI(int ins)
{
	bchg(fetch_uw, ins);
}

void iBCLRI(int ins)
{
	bclr(fetch_uw, ins);
}

void iBSETI(int ins)
{
	bset(fetch_uw, ins);
}

void iEORI_B(int ins)
{
	register UBYTE data;
	data = fetch_uw;
	if ((ins & EA_mask) == MODE_immed)
		SR ^= data;
	else
		eor_b(data, adea(ins));
}

void
iEORI_W(int ins)
{
	register UWORD data;
	data = fetch_uw;
	if ((ins & EA_mask) == MODE_immed)
		SR ^= data;
	else
		eor_w(data, adea(ins));
}

void
iEORI_L(int ins)
{
	register ULONG data;
	data = fetch_ul;
	eor_l(data, adea(ins));
}

static void
cmp_b(UBYTE src, C_ad dest)
{
	UBYTE temp;
	temp = read_ub(dest);
	sub_b(src, (C_ad)&temp);
}

static void
cmp_w(UWORD src, C_ad dest)
{
	UWORD temp;
	temp = read_uw(dest);
	sub_w(src, (C_ad)&temp);
}

static void
cmp_l(ULONG src, C_ad dest)
{
	ULONG temp;
	temp = read_ul(dest);
	sub_l(src, (C_ad)&temp);
}


void iCMPI_B(int ins)
{
	register UBYTE src;
	src = fetch_uw;
	cmp_b(src, adea(ins));
}


void iCMPI_W(int ins)
{
	register UWORD src;
	src = fetch_uw;
	cmp_w(src, adea(ins));
}


void iCMPI_L(int ins)
{
	register ULONG src;
	src = fetch_ul;
	cmp_l(src, adea(ins));
}


void iMOVE_B(int ins)
{
	register UBYTE src;
	src = read_ub(dea(insert_size(SIZE_byte, ins)));
	setnz_b(write_b(src, adea(((ins>>9) & 7) | ((ins>>3) & (7<<3)))));
}

void iMOVE_L(int ins)
{
	register LONG src;
	src = read_l(dea(insert_size(SIZE_long,ins)));
	if (((ins >> 6) & 7) == 1)	/* MOVEA.L ? */
		Areg[(ins >> 9) & 7].l = src;
	else
		setnz_l(write_l(src,
			adea( (((ins>>9) & 7) | ((ins>>3) & (7<<3))) |
			(SIZE_long<<SIZE_shift) )
		));
}

void iMOVE_W(int ins)
{
	register WORD src;
	src = read_w(dea(insert_size(SIZE_word,ins)));
	if (((ins >> 6) & 7) == 1)	/* MOVEA.W ? */
		Areg[(ins >> 9) & 7].l = src;
	else
		setnz_w(write_w(src,
			adea( (((ins>>9) & 7) | ((ins>>3) & (7<<3))) |
			(SIZE_word<<SIZE_shift) )
		));
}

void iNEGX_B(int ins) { unimp(); }
void iNEGX_W(int ins) { unimp(); }
void iNEGX_L(int ins) { unimp(); }
void iMOVEfSR(int ins) { unimp(); }
void iCHK(int ins) { unimp(); }

void iLEA(int ins)
{
	Areg[(ins>>9) & 7].l = C_to_E(SIZE_byte, cea(insert_size(SIZE_byte,ins)));
}

void iCLR_B(int ins)
{
	write_b(0, adea(ins));
	SR = Z_bit;
}

void iCLR_W(int ins)
{
	write_w(0, adea(ins));
	SR = Z_bit;
}

void iCLR_L(int ins)
{
	write_l(0, adea(ins));
	SR = Z_bit;
}

void iNEG_B(int ins)
{
	C_ad src;
	int t;
	src = adea(ins);
	t = read_b(src);
	setnz_b(write_b(-t, src));
	if (t == 0) SR |= C_bit;
}

void iNEG_W(int ins)
{
	register C_ad src;
	int t;
	src = adea(ins);
	t = read_w(src);
	setnz_w(write_w(-t, src));
	if (t == 0) SR |= C_bit;
}

void iNEG_L(int ins)
{
	register C_ad src;
	int t;
	src = adea(ins);
	t = read_l(src);
	setnz_l(write_l(-t, src));
	if (t == 0) SR |= C_bit;
}

void iMOVEtCCR(int ins)
{
	SR = read_b(dea(insert_size(SIZE_byte,ins)));
}

void iNOT_B(int ins)
{
	register C_ad src;
	src = adea(ins);
	setnz_b(write_b(~read_b(src), src));
}

void iNOT_W(int ins)
{
	register C_ad src;
	src = adea(ins);
	setnz_w(write_w(~read_w(src), src));
}

void iNOT_L(int ins)
{
	register C_ad src;
	src = adea(ins);
	setnz_l(write_l(~read_l(src), src));
}

void iMOVEtSR(int ins) { unimp(); }
void iNBCD(int ins) { unimp(); }

void iSWAP_PEA(int ins)
{
	if (!(ins & MODE_mask)) {	/* SWAP */
		int reg, n;
		reg = ins & REG_mask;
		n = Dreg[reg].l;
		setnz_l(Dreg[reg].l = ((n>>16) & 0xffff) | (n<<16));
	} else	/* PEA */
		push_l( C_to_E(SIZE_byte, cea(insert_size(SIZE_byte,ins))) );
}

void iEXT_W_MMfreg_W(int ins)
{
	if (ins & MODE_mask)
		iMMfreg_W(ins);
	else
		setnz_w(Dreg[ins & REG_mask].w = Dreg[ins & REG_mask].b);
}

void iEXT_L_MMfreg_L(int ins)
{
	if (ins & MODE_mask)
		iMMfreg_L(ins);
	else
		setnz_l(Dreg[ins & REG_mask].l = Dreg[ins & REG_mask].w);
}

void iMMfreg_W(int ins)
{
    UWORD *nextmem;
    int mask;
    int i;

    mask = fetch_uw;
    if ((ins & MODE_mask) == MODE_ind_predec)
    {
        /* push regs ? */
        nextmem = (UWORD *)E_to_C(SIZE_word, Areg[ins & REG_mask].l-2);
        for (i=7; i>=0; --i)
        {
            if (mask & 1) *DEC(nextmem) = Areg[i].w;
            mask >>= 1;		/* shift next mask bit into lsb */
        }
        for (i=7; i>=0; --i)
        {
            if (mask & 1) *DEC(nextmem) = Dreg[i].w;
            mask >>= 1;		/* shift next mask bit into lsb */
        }
        /* write back new address reg value */
        Areg[ins & REG_mask].l = C_to_E(SIZE_word, nextmem) + 2;
    }
    else
    {
        nextmem = (UWORD *)acea(insert_size(SIZE_word,ins));
        for (i=0; i<8; i++)
        {
            if (mask & 1) *INC(nextmem) = Dreg[i].w;
            mask >>= 1;		/* shift next mask bit into lsb */
        }
        for (i=0; i<8; i++)
        {
            if (mask & 1) *INC(nextmem) = Areg[i].w;
            mask >>= 1;		/* shift next mask bit into lsb */
        }
    }
}

void iMMfreg_L(int ins)
{
    ULONG *nextmem;
    int mask;
    int i;

    mask = fetch_uw;
    if ((ins & MODE_mask) == MODE_ind_predec)
    {
        /* push regs ? */
        nextmem = (ULONG *)E_to_C(SIZE_long, Areg[ins & REG_mask].l-4);
        for (i=7; i>=0; i--)
        {
            if (mask & 1) write_l(Areg[i].l, DEC(nextmem));
            mask >>= 1;		/* shift next mask bit into lsb */
        }
        for (i=7; i>=0; i--)
        {
            if (mask & 1) write_l(Dreg[i].l, DEC(nextmem));
            mask >>= 1;		/* shift next mask bit into lsb */
        }
        /* write back new address reg value */
        Areg[ins & REG_mask].l = C_to_E(SIZE_long, nextmem) + 4;
    }
    else
    {
        nextmem = (ULONG *)acea(insert_size(SIZE_long,ins));
        for (i=0; i<8; i++)
        {
            if (mask & 1) write_l(Dreg[i].l, INC(nextmem));
            mask >>= 1;		/* shift next mask bit into lsb */
        }
        for (i=0; i<8; i++)
        {
            if (mask & 1) write_l(Areg[i].l, INC(nextmem));
            mask >>= 1;		/* shift next mask bit into lsb */
        }
    }
}

void iTST_B(int ins)
{
	setnz_b(read_b(adea(ins)));
}

void iTST_W(int ins)
{
	setnz_w(read_w(adea(ins)));
}

void iTST_L(int ins)
{
	setnz_l(read_l(adea(ins)));
}

void iTAS(int ins) { unimp(); }

void iMMtreg_W(int ins)
{
    UWORD *nextmem;
    int mask, i;

    mask = fetch_uw;
    int v = (ins & MODE_mask) == MODE_ind_postinc;
    if (v)
        nextmem = (UWORD *)E_to_C(SIZE_word, Areg[ins & REG_mask].l);
    else
        nextmem = (UWORD *)acea(insert_size(SIZE_word,ins));
    
    for (i=0; i<8; i++)
    {
        if (mask & 1) Dreg[i].w = *INC(nextmem);
        mask >>= 1;		/* move next mask bit into lsb */
    }
    for (i=0; i<8; i++)
    {
        if (mask & 1) Areg[i].w = *INC(nextmem);
        mask >>= 1;		/* move next mask bit into lsb */
    }

    if (v)
        Areg[ins & REG_mask].l = C_to_E(SIZE_word,nextmem);
}

void iMMtreg_L(int ins)
{
    ULONG *nextmem;
    int mask, i;

    mask = fetch_uw;
    int v = (ins & MODE_mask) == MODE_ind_postinc;
    if (v)
        nextmem = (ULONG *)E_to_C(SIZE_long, Areg[ins & REG_mask].l);
    else
        nextmem = (ULONG *)acea(insert_size(SIZE_long,ins));
    
    for (i=0; i<8; i++)
    {
        if (mask & 1) Dreg[i].l = read_l(INC(nextmem));
        mask >>= 1;		/* move next mask bit into lsb */
    }
    for (i=0; i<8; i++)
    {
        if (mask & 1) Areg[i].l = read_l(INC(nextmem));
        mask >>= 1;		/* move next mask bit into lsb */
    }

    if (v)
        Areg[ins & REG_mask].l = C_to_E(SIZE_long,nextmem);
}


void imisc4E(int ins)
{
	extern void (*table1[])();
	(*table1[(ins >> 3) & 7])(ins);
}

void iTRAP(int ins)
{
#ifdef GDBREMOTE
    switch (ins & 15) {
    case 1 :
	iTRAP1();
	break;
    default:
	raise(SIGILL);
    }
#else
    unimp();
#endif
}

void
iLINK_W(int ins)
{
    int An = ins & REG_mask;
    push_l(Areg[An].ul);
    Areg[An].ul = Areg[7].ul;
    Areg[7].ul += fetch_w;
}

void
iUNLK(int ins)
{
    int An = ins & REG_mask;
    Areg[7].ul = Areg[An].ul;
    Areg[An].ul = pull_l();
}

void iMOVEtUSP() { unimp(); }
void iMOVEfUSP() { unimp(); }
void iMOVEC() { unimp(); }

void imisc4E_2(int ins)
{
	extern void (*table2[])();
	(*table2[ins & 7])(ins);
}

void iRESET() { unimp(); }
void iSTOP() { unimp(); }
void iRTE() { unimp(); }
void iRTD() { unimp(); }
void iTRAPV() { unimp(); }
void iRTR() { unimp(); }

void iNOP(int ins) { return; }


void iRTS(int ins)
{
	PC = (UWORD *)E_to_C(SIZE_word, pull_l());
}

void iJSR(int ins)
{
	UWORD *newPC;
	newPC = (UWORD *)cea( insert_size(SIZE_word, ins) );
	push_l( C_to_E(SIZE_word, PC) );
	PC = newPC;
}

void iJMP(int ins)
{
	PC = (UWORD *)cea( insert_size(SIZE_word, ins) );
}

void iADDQ_B(int ins)
{
	add_b(quick(ins), adea(ins), 0);
}

void iADDQ_W(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)
		Areg[ins & REG_mask].l += quick(ins);
	else
		add_w(quick(ins), adea(ins), 0);
}

void iADDQ_L(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)
		Areg[ins & REG_mask].l += quick(ins);
	else
		add_l(quick(ins), adea(ins), 0);
}

void iSUBQ_B(int ins)
{
	add_b(quick(ins) ^ 0xff, adea(ins), 1);
	SR ^= C_bit;
}

void iSUBQ_W(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)
		Areg[ins & REG_mask].l -= quick(ins);
	else {
		add_w(quick(ins) ^ 0xffff, adea(ins), 1);
		SR ^= C_bit;
	}
}

void iSUBQ_L(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)
		Areg[ins & REG_mask].l -= quick(ins);
	else {
		add_l(~quick(ins), adea(ins), 1);
		SR ^= C_bit;
	}
}

void iS_DBT(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)	/* DBT */
		INC(PC);						/* skip the offset */
	else
		write_b(-1, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBF(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg) {	/* DBF */
		if ((--Dreg[ins & REG_mask].uw) == 0xffff)	/* if =-1, don't branch */
			INC(PC);				/* skip offset */
		else {
			PC = PC ADD (*(WORD *)PC)/2;	/* add offset (sub on little endians) */
		}
	} else
		write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBHI(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_HI || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_HI)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBLS(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_LS || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_LS)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBCC(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_CC || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_CC)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBCS(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_CS || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_CS)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBNE(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_NE || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_NE)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBEQ(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_EQ || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_EQ)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBVC(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_VC || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_VC)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBVS(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_VS || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_VS)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBPL(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_PL || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_PL)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBMI(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_MI || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_MI)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBGE(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_GE || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_GE)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBLT(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_LT || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_LT)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBGT(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_GT || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_GT)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

void iS_DBLE(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)		/* DBcc ? */
		if (Cond_LE || (--Dreg[ins & REG_mask].uw) == 0xffff)
			INC(PC);		/* skip offset */
		else
			PC = PC ADD (*(WORD *)PC)/2;	/* take the branch */
	else
		if (Cond_LE)
			write_b(-1, adea(insert_size(SIZE_byte,ins)));
		else
			write_b(0, adea(insert_size(SIZE_byte,ins)));
}

#define Bcc_offset(ins)	\
	((ins & 0xff) ? \
		((BYTE)ins) \
	: \
		(*(WORD *)PC) \
	)

void iBRA(int ins)
{
	PC = PC ADD Bcc_offset(ins)/2;
}

void iBSR(int ins)
{
	push_l(C_to_E(SIZE_word, PC) + (ins & 0xff ? 0 : 2));
	PC = PC ADD Bcc_offset(ins)/2;
}

void iBHI(int ins)
{
	if (Cond_HI)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBLS(int ins)
{
	if (Cond_LS)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBCC(int ins)
{
	if (Cond_CC)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBCS(int ins)
{
	if (Cond_CS)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBNE(int ins)
{
	if (Cond_NE)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBEQ(int ins)
{
	if (Cond_EQ)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBVC(int ins)
{
	if (Cond_VC)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBVS(int ins)
{
	if (Cond_VS)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBPL(int ins)
{
	if (Cond_PL)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBMI(int ins)
{
	if (Cond_LT)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBGE(int ins)
{
	if (Cond_GE)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBLT(int ins)
{
	if (Cond_LT)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBGT(int ins)
{
	if (Cond_GT)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void iBLE(int ins)
{
	if (Cond_LE)
		PC = PC ADD Bcc_offset(ins)/2;
	else if (!(ins & 0xff))
		INC(PC);	/* skip offset for word branches */
}

void
iMOVEQ(int ins)
{
	setnz_l(Dreg[(ins >> 9) & 7].l = (BYTE)ins);
}

void
iOR_BtDn(int ins) 
{
	or_b(read_ub(dea(ins)), &Dreg[(ins >> 9) & 7].ub);
}

void iOR_WtDn(int ins)
{
    or_w(read_uw(dea(ins)), (C_ad)&Dreg[(ins >> 9) & 7].uw);
}

void iOR_LtDn(int ins)
{
    or_l(read_ul(dea(ins)), (C_ad)&Dreg[(ins >> 9) & 7].ul);
}

void iDIVU(int ins) { unimp(); }

void iSBCD_OR_BfDn(int ins)
{
	if ((ins & MODE_mask) == MODE_Dreg)	/* SBCD */
		unimp();
	else
		or_b(Dreg[(ins >> 9) & 7].ub, amea(ins));
}

void iOR_WfDn(int ins)
{
	or_w(Dreg[(ins >> 9) & 7].uw, amea(ins));
}

void iOR_LfDn(int ins)
{
	or_l(Dreg[(ins >> 9) & 7].ul, amea(ins));
}

void iDIVS(int ins) { unimp(); }

void iSUB_BtDn(int ins)
{
	sub_b(read_ub(ea(ins)), &Dreg[(ins >> 9) & 7].ub);
}

void iSUB_WtDn(int ins)
{
    sub_w(read_uw(ea(ins)), (C_ad)&Dreg[(ins >> 9) & 7].uw);
}

void iSUB_LtDn(int ins)
{
    sub_l(read_ul(ea(ins)), (C_ad)&Dreg[(ins >> 9) & 7].ul);
}

void iSUBA_W(int ins)
{
	Areg[(ins >> 9) & 7].l -= read_w(ea(insert_size(SIZE_word,ins)));
	/* note: sign extends */
}

void iSUB_BfDn(int ins)
{
	sub_b(Dreg[(ins >> 9) & 7].ub, adea(ins));
}

void iSUB_WfDn(int ins)
{
	sub_w(Dreg[(ins >> 9) & 7].uw, adea(ins));
}

void iSUB_LfDn(int ins)
{
	sub_l(Dreg[(ins >> 9) & 7].ul, adea(ins));
}

void iSUBA_L(int ins)
{
	Areg[(ins >> 9) & 7].l -= read_l(ea(insert_size(SIZE_long,ins)));
	/* note: sign extends */
}

void iCMP_B(int ins)
{
	UBYTE temp = Dreg[(ins >> 9) & 7].ub;
	sub_b(read_ub(ea(ins)), &temp);
}

void iCMP_W(int ins)
{
	UWORD temp = Dreg[(ins >> 9) & 7].uw;
	sub_w(read_uw(ea(ins)), (C_ad)&temp);
}

void iCMP_L(int ins)
{
	ULONG temp = Dreg[(ins >> 9) & 7].ul;
	sub_l(read_ul(ea(ins)), (C_ad)&temp);
}

void iCMPA_W(int ins)
{
	ULONG temp = Areg[(ins >> 9) & 7].ul;
	sub_l(read_w(ea(ins)), (C_ad)&temp);	/* note: sign extends */
}

void iEOR_B(int ins)
{
	eor_b(Dreg[(ins >> 9) & 7].ub, adea(ins));
}

void iEOR_W_CMPM_W(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)	/* CMPM ? */
		unimp();
	eor_w(Dreg[(ins >> 9) & 7].uw, adea(ins));
}

void iEOR_L_CMPM_L(int ins)
{
	if ((ins & MODE_mask) == MODE_Areg)	/* CMPM ? */
		unimp();
	eor_l(Dreg[(ins >> 9) & 7].ul, adea(ins));
}

void iCMPA_L(int ins)
{
	ULONG temp = Areg[(ins >> 9) & 7].ul;
	sub_l(read_l(ea(insert_size(SIZE_long, ins))), (C_ad)&temp);
	/* note: sign extends */
}

void iAND_BtDn(int ins)
{
	and_b(read_ub(dea(ins)), &Dreg[(ins >> 9) & 7].ub);
}

void iAND_WtDn(int ins) 
{
    and_w(read_uw(dea(ins)), (C_ad)&Dreg[(ins >> 9) & 7].uw);
}

void iAND_LtDn(int ins)
{
    and_l(read_ul(dea(ins)), (C_ad)&Dreg[(ins >> 9) & 7].ul);
}

void iMULU(int ins) { unimp(); }

void iABCD_AND_BfDn(int ins)
{
	if ((ins & MODE_mask) <= MODE_Areg) unimp();	/* ABCD ? */
	and_b(Dreg[(ins >> 9) & 7].ub, adea(ins));
}

void iEXG1_AND_WfDn(int ins)
{
	ULONG t, s, d;

	s = (ins >> 9) & 7;
	d = ins & 7;
	switch (ins & MODE_mask) {
	case MODE_Dreg :	/* EXG Ds,Dd ? */
		t = Dreg[s].ul;
		Dreg[s].ul = Dreg[d].ul;
		Dreg[d].ul = t;
		return;
	case MODE_Areg :	/* EXG As,Ad ? */
		t = Areg[s].ul;
		Areg[s].ul = Areg[d].ul;
		Areg[d].ul = t;
		return;
	default :
		and_w(Dreg[s].uw, adea(ins));
	}
}

void iEXG2_AND_LfDn(int ins)
{
	ULONG t, s, d;

	s = (ins >> 9) & 7;
	d = ins & 7;
	switch (ins & MODE_mask) {
	case MODE_Dreg :
		t = Dreg[s].ul;
		Dreg[s].ul = Areg[d].ul;
		Areg[d].ul = t;
		return;
	case MODE_Areg :
		illegal();
		/*NOTREACHED*/
	default :
		and_l(Dreg[s].ul, adea(ins));
	}
}

void iMULS(int ins)
{ unimp(); }

void iADD_BtDn(int ins)
{
	add_b(read_ub(dea(ins)), &Dreg[(ins >> 9) & 7].ub, 0);
}

void iADD_WtDn(int ins)
{
    add_w(read_uw(dea(ins)), (C_ad)&Dreg[(ins >> 9) & 7].uw, 0);
}

void iADD_LtDn(int ins)
{
    add_l(read_ul(dea(ins)), (C_ad)&Dreg[(ins >> 9) & 7].ul, 0);
}

void iADDA_W(int ins)
{
	Areg[(ins >> 9) & 7].ul += read_w(ea(insert_size(SIZE_word,ins)));
}

void iADDX_B_ADD_BfDn(int ins)
{
    if ((ins & MODE_mask) <= MODE_Areg) unimp();
    add_b(Dreg[(ins >> 9) & 7].ub, adea(ins), 0);
}

void iADDX_W_ADD_WfDn(int ins)
{
	if ((ins & MODE_mask) <= MODE_Areg) unimp();
	add_w(Dreg[(ins >> 9) & 7].uw, adea(ins), 0);
}

void iADDX_L_ADD_LfDn(int ins)
{
	if ((ins & MODE_mask) <= MODE_Areg) unimp();
	add_l(Dreg[(ins >> 9) & 7].ul, adea(ins), 0);
}

void iADDA_L(int ins)
{
	Areg[(ins >> 9) & 7].ul += read_l(ea(insert_size(SIZE_long,ins)));
}

void lsl_b(int data, int ins)
{
	register UBYTE *dest;
	dest = &Dreg[ins & REG_mask].ub;
	SR &= ~C_bit;
	if (*dest & (256 >> data)) SR |= C_bit;
	setnz_b_leave_carry(*dest <<= data);
}

void roxl_b(int data,int ins)
{
	unimp();
}

void rol_b(int data, int ins)
{
	register UBYTE *dest;
	dest = &Dreg[ins & REG_mask].ub;
	SR &= ~C_bit;
	if (*dest & (256 >> data)) SR |= C_bit;
	setnz_b_leave_carry(*dest = (*dest >> (8-data)) | (*dest << data));
}

void iSL_B_ROL_B(int ins)
{
	switch ((ins >> 3) & 7) {
	case 0 :
	case 1 :
		lsl_b(quick(ins), ins);
		return;
	case 2 :
		roxl_b(quick(ins), ins);
		return;
	case 3 :
		rol_b(quick(ins), ins);
		return;
	case 4 :
	case 5 :
		lsl_b(Dreg[(ins >> 9) & 7].l & 7, ins);
		return;
	case 6 :
		roxl_b(Dreg[(ins >> 9) & 7].l & 7, ins);
		return;
	case 7 :
		rol_b(Dreg[(ins >> 9) & 7].l & 7, ins);
		return;
	}
}

void lsl_w(int data, int ins)
{
	register UWORD *dest;
	dest = &Dreg[ins & REG_mask].uw;
	SR &= ~C_bit;
	if (*dest & (0x10000 >> data)) SR |= C_bit;
	setnz_w_leave_carry(*dest <<= data);
}

void roxl_w(int data, int ins)
{
	unimp();
}

void rol_w(int data, int ins)
{
	register UWORD *dest;
	dest = &Dreg[ins & REG_mask].uw;
	SR &= ~C_bit;
	if (*dest & (0x10000 >> data)) SR |= C_bit;
	setnz_w_leave_carry(*dest = (*dest >> (16-data)) | (*dest << data));
}


void iSL_W_ROL_W(int ins)
{
	switch ((ins >> 3) & 7) {
	case 0 :
	case 1 :
		lsl_w(quick(ins), ins);
		return;
	case 2 :
		roxl_w(quick(ins), ins);
		return;
	case 3 :
		rol_w(quick(ins), ins);
		return;
	case 4 :
	case 5 :
		lsl_w(Dreg[(ins >> 9) & 7].l & 15, ins);
		return;
	case 6 :
		roxl_w(Dreg[(ins >> 9) & 7].l & 15, ins);
		return;
	case 7 :
		rol_w(Dreg[(ins >> 9) & 7].l & 15, ins);
		return;
	}
}

void lsl_l(int data, int ins)
{
	register ULONG *dest;
	dest = &Dreg[ins & REG_mask].ul;
	SR &= ~C_bit;
	if (*dest & (1 << (32-data))) SR |= C_bit;
	setnz_l_leave_carry(*dest <<= data);
}

void roxl_l(int data, int ins)
{
	unimp();
}

void rol_l(int data, int ins)
{
	register ULONG *dest;
	dest = &Dreg[ins & REG_mask].ul;
	SR &= ~C_bit;
	if (*dest & (1 << (32-data))) SR |= C_bit;
	/* NOTE: this may not work if >> is implemented as ASR */
	setnz_l_leave_carry(*dest = (*dest >> (32-data)) | (*dest << data));
}


void iSL_L_ROL_L(int ins)
{
	switch ((ins >> 3) & 7) {
	case 0 :
	case 1 :
		lsl_l(quick(ins), ins);
		return;
	case 2 :
		roxl_l(quick(ins), ins);
		return;
	case 3 :
		rol_l(quick(ins), ins);
		return;
	case 4 :
	case 5 :
		lsl_l(Dreg[(ins >> 9) & 7].l & 31, ins);
		return;
	case 6 :
		roxl_l(Dreg[(ins >> 9) & 7].l & 31, ins);
		return;
	case 7 :
		rol_l(Dreg[(ins >> 9) & 7].l & 31, ins);
		return;
	}
}

void asr_b(int data, int ins)
{
	register BYTE *dest;	/* NOTE: signed */
	dest = &Dreg[ins & REG_mask].b;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	/* NOTE: this may not work if the C compiler generates LSR */
	setnz_b_leave_carry(*dest >>= data);
}

void lsr_b(int data, int ins)
{
	register UBYTE *dest;
	dest = &Dreg[ins & REG_mask].ub;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	setnz_b_leave_carry(*dest >>= data);
}

void roxr_b(int data, int ins)
{
	unimp();
}

void ror_b(int data, int ins)
{
	register UBYTE *dest;
	dest = &Dreg[ins & REG_mask].ub;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	setnz_b_leave_carry(*dest = (*dest << (8-data)) | (*dest >> data));
}


void iSR_B_ROR_B(int ins)
{
	switch ((ins >> 3) & 7) {
	case 0 :
		asr_b(quick(ins), ins);
		return;
	case 1 :
		lsr_b(quick(ins), ins);
		return;
	case 2 :
		roxr_b(quick(ins), ins);
		return;
	case 3 :
		ror_b(quick(ins), ins);
		return;
	case 4 :
		asr_b(Dreg[(ins >> 9) & 7].l & 7, ins);
		return;
	case 5 :
		lsr_b(Dreg[(ins >> 9) & 7].l & 7, ins);
		return;
	case 6 :
		roxr_b(Dreg[(ins >> 9) & 7].l & 7, ins);
		return;
	case 7 :
		ror_b(Dreg[(ins >> 9) & 7].l & 7, ins);
		return;
	}
}

void asr_w(int data, int ins)
{
	register WORD *dest;
	dest = &Dreg[ins & REG_mask].w;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	/* NOTE: this may not work if the C compiler produces LSR for >> */
	setnz_w_leave_carry(*dest >>= data);
}

void lsr_w(int data, int ins)
{
	register UWORD *dest;
	dest = &Dreg[ins & REG_mask].uw;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	setnz_w_leave_carry(*dest >>= data);
}

void roxr_w(int data,int ins)
{
	unimp();
}

void asr_l(int data, int ins)
{
	register LONG *dest;
	dest = &Dreg[ins & REG_mask].l;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	/* NOTE: check for C compiler producing LSR instruction here */
	setnz_l_leave_carry(*dest >>= data);
}

void lsr_l(int data, int ins)
{
	register ULONG *dest;
	dest = &Dreg[ins & REG_mask].ul;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	setnz_l_leave_carry(*dest >>= data);
}


void ror_w(int data, int ins)
{
	register UWORD *dest;
	dest = &Dreg[ins & REG_mask].uw;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	setnz_w_leave_carry(*dest = (*dest << (16-data)) | (*dest >> data));
}



void iSR_W_ROR_W(int ins)
{
	switch ((ins >> 3) & 7) {
	case 0 :
		asr_w(quick(ins), ins);
		return;
	case 1 :
		lsr_w(quick(ins), ins);
		return;
	case 2 :
		roxr_w(quick(ins), ins);
		return;
	case 3 :
		ror_w(quick(ins), ins);
		return;
	case 4 :
		asr_w(Dreg[(ins >> 9) & 7].l & 15, ins);
		return;
	case 5 :
		lsr_w(Dreg[(ins >> 9) & 7].l & 15, ins);
		return;
	case 6 :
		roxr_w(Dreg[(ins >> 9) & 7].l & 15, ins);
		return;
	case 7 :
		ror_w(Dreg[(ins >> 9) & 7].l & 15, ins);
		return;
	}
}

void roxr_l(int data,int ins)
{
	unimp();
}

void ror_l(int data, int ins)
{
	register ULONG *dest;
	dest = &Dreg[ins & REG_mask].ul;
	SR &= ~C_bit;
	if (*dest & (1 << (data-1))) SR |= C_bit;
	/* NOTE: this may not work if >> is implemented as ASR */
	setnz_l_leave_carry(*dest = (*dest << (32-data)) | (*dest >> data));
}

void iASR(int ins)
{
	register WORD *dest;
	dest = (WORD *)adea(insert_size(SIZE_word,ins));
	SR &= ~C_bit;
	if (*dest & 1) SR |= C_bit;
	setnz_w_leave_carry(*dest >>= 1);
}

void iASL(int ins)
{
	register UWORD *dest;
	dest = (UWORD *)adea(insert_size(SIZE_word,ins));
	SR &= ~C_bit;
	if (*dest & 0x10000) SR |= C_bit;
	setnz_w_leave_carry(*dest <<= 1);
}


void iSR_L_ROR_L(int ins)
{
	switch ((ins >> 3) & 7) {
	case 0 :
		asr_l(quick(ins), ins);
		return;
	case 1 :
		lsr_l(quick(ins), ins);
		return;
	case 2 :
		roxr_l(quick(ins), ins);
		return;
	case 3 :
		ror_l(quick(ins), ins);
		return;
	case 4 :
		asr_l(Dreg[(ins >> 9) & 7].l & 31, ins);
		return;
	case 5 :
		lsr_l(Dreg[(ins >> 9) & 7].l & 31, ins);
		return;
	case 6 :
		roxr_l(Dreg[(ins >> 9) & 7].l & 31, ins);
		return;
	case 7 :
		ror_l(Dreg[(ins >> 9) & 7].l & 31, ins);
		return;
	}
}



void iLSR(int ins)
{
	register UWORD *dest;
	dest = (UWORD *)adea(insert_size(SIZE_word,ins));
	SR &= ~C_bit;
	if (*dest & 1) SR |= C_bit;
	setnz_w_leave_carry(*dest >>= 1);
}

void iLSL(int ins)
{
	register UWORD *dest;
	dest = (UWORD *)adea(insert_size(SIZE_word,ins));
	SR &= ~C_bit;
	if (*dest & 0x10000) SR |= C_bit;
	setnz_w_leave_carry(*dest <<= 1);
}

void iROXR(int ins)
{ unimp(); }

void iROXL(int ins)
{ unimp(); }

void iROR(int ins)
{
	register UWORD *dest;
	dest = (UWORD *)adea(insert_size(SIZE_word,ins));
	if (*dest & 1) {
		SR |= C_bit;
		setnz_w_leave_carry(*dest = 0x8000 | (*dest >> 1));
	} else {
		SR &= ~C_bit;
		setnz_w_leave_carry(*dest >>= 1);
	}
}

void iROL(int ins)
{
	register UWORD *dest;
	dest = (UWORD *)adea(insert_size(SIZE_word,ins));
	if (*dest & 0x8000) {
		SR |= C_bit;
		setnz_w_leave_carry(*dest = 1 | (*dest << 1));
	} else {
		SR &= ~C_bit;
		setnz_w_leave_carry(*dest <<= 1);
	}
}

