/**
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
 */

/* This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include "inttype.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sections.h"
#include "expr.h"
#include "ea.h"
#include "opcodes.h"
#include "branches.h"
#include "assemins.h"
#include "errors.h"
#include "reloc.h"
#include "output.h"

#define MAXNARGS	6
#define MAXNEXTWORDS	6

extern inttype processor;
extern struct m68k_opcode m68k_opcodes[];
extern inttype numopcodes;
extern struct section *current_section;
extern inttype pass;
extern inttype compatibility;			/* set if trying to be mcc compatible */

struct ea args[MAXNARGS];
inttype n_args;
inttype opword;			/* opcode word */
inttype extword[MAXNEXTWORDS];
inttype n_extwords=0;

/* effective address validity masks */
static int am_mask1 =	0x7c;
static int am_mask2 =	0x03;
static int a_mask1 =	0x7f;
static int a_mask2 =	0x03;
static int d_mask1 =	0x7d;
static int d_mask2 =	0x1f;
static int d_not_im_mask1 = 0x7d;
static int d_not_im_mask2 = 0x0f;
static int c_mask1 =	0x64;
static int c_mask2 =	0x0f;
static int ac_mask1 =	0x64;
static int ac_mask2 =	0x03;
static int ad_mask1 =	0x7d;
static int ad_mask2 =	0x03;
static int ac_or_Dn_mask1 = 0x65;
static int ac_or_Dn_mask2 = 0x03;
static int c_or_Dn_mask1 =	0x65;
static int c_or_Dn_mask2 =	0x0f;

/* check whether an effective address is valid */
static int check_ea(struct ea* ap, int mask1, int mask2)
{
    if (!((1<<ap->mode) & mask1))
    {
        if (ap->mode==7)
        {
            if (!((1<<ap->reg) & mask2))
                return 0;
            else
                return 1;	/* matches */
        }
        else
            return 0;
    }
    return 1;				/* matches */
}

/* insert data into an opcode, possibly adding extension words and
 * adding ext_ref, reloc info if nesc */
static void ins_op(struct expr* dp, char place, int size)
{
    /* struct expr *dp;  pointer to data to insert */
    /* char place;  where to insert it */
    /* int size; how many bits to insert */
    
    inttype max = (1<<size)-1;	/* max data size */
    struct expr data;
    data = *dp;

    if (size == 32) max = 0xffffffff;

    if (data.sym)			/* is the data an address ? */
        /* only byte, word, long data allowed */
        if (size != 8 && size != 16 && size != 32)
            error("illegal data for operand");

    if ((size==8 && (data.value<-128 || data.value>max)) ||
        (size==16 && (data.value<-32768 || data.value>max)) ||
        ((size!=8 && size!=16 && size!=32) && data.value>max))
        error("data %x too large for operand", data.value);
    data.value &= max;
    switch (place) {
    case 's' :
        /* if the data is an address, then it must be byte size to be
         * inserted here */
        if (pass==2 && data.sym) {
            if (size != 8)
                /* this should never happen */
                fatal_error("trying to insert %d bit address in opcode");
            if (data.sym->type == S_RELOC)
                error("sorry - 8 bit relocation is not available");
            else
                add_ref8(data.sym, current_section->pc + 1);
        }
        opword |= data.value;
        return;
    case 'd' :
        opword |= data.value << 9;
        return;
    case '1' :
        if (n_extwords == 0) n_extwords++;
        extword[0] |= data.value << 12;
        return;
    case '2' :
        if (n_extwords == 0) n_extwords++;
        extword[0] |= data.value << 6;
        return;
    case '3' :
        if (n_extwords == 0) n_extwords++;
        extword[0] |= data.value;
        return;
    case '4' :
        if (n_extwords == 0) n_extwords+=2;
        if (n_extwords == 1) n_extwords++;
        extword[1] |= data.value << 12;
        return;
    case '5' :
        if (n_extwords == 0) n_extwords+=2;
        if (n_extwords == 1) n_extwords++;
        extword[1] |= data.value << 6;
        return;
    case '6' :
        if (n_extwords == 0) n_extwords+=2;
        if (n_extwords == 1) n_extwords++;
        extword[1] |= data.value;
        return;
    case '7' :
        if (n_extwords == 0) n_extwords++;
        extword[0] |= data.value << 7;
        return;
    case '8' :
        if (n_extwords == 0) n_extwords++;
        extword[0] |= data.value << 10;
        return;
    case 'D' :
        if (n_extwords == 0) n_extwords++;
        extword[0] |= data.value << 12;
        extword[0] |= data.value;
        return;
    case 'b' :	/* immediate byte data */
        n_extwords++;	/* should be zero at this point */
        extword[0] = data.value & 0xff;
        /* check for a bytewise external reference */
        if (pass==2 && data.sym)
        {
            if (data.sym->type == S_RELOC)
                error("sorry - 8 bit relocation not available");
            else
                add_ref8(data.sym, current_section->pc + 3);
        }
        return;
    case 'w' :	/* immediate word data */
        extword[n_extwords] = data.value & 0xffff;
        if (pass==2 && data.sym)
        {
            if (data.sym->type == S_RELOC)
            {
                warning("16 bit relocation");
                add_reloc16(data.sym,
                            current_section->pc + 2 + 2*n_extwords);
            } else
                add_ref16(data.sym,
                          current_section->pc + 2 + 2*n_extwords);
        }
        n_extwords++;
        return;
    case 'l' :	/* immediate long data */
        n_extwords+=2;	/* should be zero at this point */
        extword[0] = (data.value >>16) & 0xffff;
        extword[1] = data.value  & 0xffff;
        if (pass==2 && data.sym)
        {
            if (data.sym->type == S_RELOC)
                add_reloc32(data.sym, current_section->pc + 2);
            else
                add_ref32(data.sym, current_section->pc + 2);
        }
        return;
    default :
        fatal_error("bad argument to ins_op '%c' ?\n", place);
        return;
    }
}

static void ins_ea(struct ea* ap, char place)
{
    /* insert the effective address into the opcode */
    if (place == 'd')
        opword |= (ap->reg << 9) | (ap->mode << 6);
    else
        opword |= ap->reg | (ap->mode << 3);

    /* deal with any extension words */
    if (ap->mode == M_AREG_OFF) {	/* d(An) */
        if (pass==2 && ap->ext.sym)
            switch (ap->ext.sym->type) {
            case S_XREF :
                /* remember where the extension word is for later
                 * adjusting by the linker */
                add_ref16An(ap->ext.sym, current_section->pc+2+2*n_extwords);
                break;
            case S_RELOC :
                /* remember where ext word is for later relocation
                 * when linking */
                add_reloc16An(ap->ext.sym, current_section->pc+2+2*n_extwords);
                warning("Alink won't be able to link this !");
                break;
            default :
                printf("weird symbol type %d\n", ap->ext.sym->type);
                exit(-1);
            }
        extword[n_extwords++] = ap->ext.value;
    } else if (ap->mode == M_AREG_OFF_IND) {	/* d(An,Rn.S) */
        if (ap->ext.sym)
        {
            /* having an address in d(An,Rn.S) is ridiculous */
            if (ap->ext.sym->type==S_RELOC)
                error("offset must not be an address");
            else if (ap->ext.sym->type==S_XREF)
            {
                if (pass==2)
                    add_ref8(ap->ext.sym, current_section->pc+3+2*n_extwords);
            }
        }
        extword[n_extwords++] = ap->ext.value;
    } else if (ap->mode == M_PC)
        switch ((int)ap->reg) {
        case M_ABS16 :	/* addr16 */
            if (pass==2 && ap->ext.sym)
                switch (ap->ext.sym->type) {
                case S_XREF :
                    error(
                          "sorry - Alink can't handle 16 bit absolute external references"
                          );
                    break;
                case S_RELOC :
                    warning("16 bit relocation");
                    break;
                }
            extword[n_extwords++] = ap->ext.value;
            break;
        case M_ABS32 :	/* addr32 */
            if (pass==2 && ap->ext.sym)
                switch (ap->ext.sym->type) {
                case S_XREF :
                    add_ref32(ap->ext.sym, current_section->pc +2+
                              2*n_extwords);
                    break;
                case S_RELOC :
                    add_reloc32(ap->ext.sym, current_section->pc +2+
                                2*n_extwords);
                    break;
                }
            extword[n_extwords++] = (ap->ext.value >> 16) & 0xffff;
            extword[n_extwords++] = ap->ext.value & 0xffff;
            break;
        case M_PCREL :	/* d(PC) */
            if (pass==2 && ap->ext.sym)
                switch (ap->ext.sym->type) {
                case S_XREF :
                    if (pass==2)
                        /* 16 bit pc relative ref */
                        add_ref16(ap->ext.sym, current_section->pc +2+
                                  2*n_extwords);
                    extword[n_extwords++] = ap->ext.value;
                    break;
                case S_RELOC :
                    extword[n_extwords] = ap->ext.value -
                        current_section->pc - 2 - 2*n_extwords;
                    n_extwords++;
                    break;
                }
            else
                /* either an absolute ref, or in first pass, so value
                 * stored doesn't matter */
                extword[n_extwords++] = ap->ext.value;
            break;
        case M_PCREL_IND :	/* d(PC,Rn.S) */
            if (pass==2 && ap->ext.sym)
                if (ap->ext.sym->type == S_XREF)
                    add_ref8(ap->ext.sym, current_section->pc +2+
                             2*n_extwords+1);
            extword[n_extwords++] = ap->ext.value;
            break;
        case M_IMMED :	/* immediate */
            switch (place) {
            case 'd' :	/* destination, so no immediate data */
                error("bad operand");
                break;
            case 'b' :
                if (pass==2 && ap->ext.sym) switch (ap->ext.sym->type) {
                    case S_RELOC :
                        error(
                              "sorry - 8 bit relocation not available");
                        break;
                    case S_XREF :
                        add_ref8(ap->ext.sym, current_section->pc + 3 +
                                 2*n_extwords);
                        break;
                    }
                extword[n_extwords++] = ap->ext.value & 0xff;
                break;
            case 'w' :
                if (pass==2 && ap->ext.sym) switch (ap->ext.sym->type) {
                    case S_RELOC :
                        warning( "16 bit relocation");
                        add_reloc16(ap->ext.sym, current_section->pc + 2 +
                                    2*n_extwords);
                        break;
                    case S_XREF :
                        add_ref16(ap->ext.sym, current_section->pc + 2 +
                                  2*n_extwords);
                        break;
                    }
                extword[n_extwords++] = ap->ext.value;
                break;
            case 'l' :
                if (pass==2 && ap->ext.sym) switch (ap->ext.sym->type) {
                    case S_RELOC :
                        add_reloc32(ap->ext.sym, current_section->pc + 2 +
                                    2*n_extwords);
                        break;
                    case S_XREF :
                        add_ref32(ap->ext.sym, current_section->pc + 2 +
                                  2*n_extwords);
                        break;
                    }
                extword[n_extwords++] = (ap->ext.value >> 16) & 0xffff;
                extword[n_extwords++] = ap->ext.value & 0xffff;
                break;
            case 's' :	/* source, but no immediate */
                error("bad operand");
                break;
            }
        }
}

static void do_branch(struct expr* destp)
{
    inttype offset;
    struct branch *bp;

    if (!destp->sym)
        error("PC relative branch to absolute value");
    switch (destp->sym->type) {
    case S_RELOC :
        if (pass==1) {
            add_branch(destp, B_ANY);		/* work out size later */
            n_extwords = 1;
            extword[0] = 0;
            break;
        }
        offset = destp->sym->svalue - current_section->pc - 2;
        bp = find_branch();
        switch (bp->size) {
        case B_SHORT :
            if (offset < -128 || offset > 127)
                error("range error");
            opword |= offset & 0xff;
            break;
        case B_WORD :
            n_extwords = 1;
            if (offset < -32768 || offset > 32767)
                error("range error");
            if (offset == 2)
                warning("branch with a zero offset");
            extword[0] = offset & 0xffff;
            break;
        default :
            fatal_error("weird branch size");
        }
        break;
    case S_XREF :
        if (pass == 2) {
            add_ref16(destp->sym, current_section->pc + 2);
            find_branch(); /* keep the branch list in sync */
        } else
            add_branch(destp, B_WORD);
        n_extwords = 1;
        extword[0] = destp->value;
        break;
    case S_UNDF :
        /* remember the branch for optimisation after pass 1 */
        add_branch(destp, B_ANY);
        n_extwords = 1;
        extword[0] = 0;
        break;
    default :
        error("branch must be to an address!\n");
    }
}

void assemble_instruction(inttype index)
{
    struct m68k_opcode *op;
    inttype i, j;
    char *p;
    struct ea *ap;
    char instrname[20];
    static struct expr e = {0, 0};	/* used in calls to ins_op */

    if (current_section->pc & 1)	/* odd PC ? */
        error("Assembling instructions on an odd word boundary!!");
    for (i=0; i<MAXNEXTWORDS; i++) extword[i] = 0;
    op = m68k_opcodes+index;
    (void)strcpy(instrname, op->name);
    while (!strcmp(instrname, op->name)) {
        /* check for right number of args */
        if (n_args == strlen(op->args)/2) {
            /* check each arg */
            for (j=0, p=op->args, ap=args; j<n_args; j++, p+=2, ap++)
                switch (*p) {
                case 'D' :	/* data register */
                    if (ap->mode != M_DREG) goto nomatch;
                    break;
                case 'A' :	/* address register */
                    if (ap->mode != M_AREG) goto nomatch;
                    break;
                case 'R' :	/* any register */
                    if (ap->mode > M_AREG) goto nomatch;
                    break;
                case '+' :	/* autoinc */
                    if (ap->mode != M_POSTINC) goto nomatch;
                    break;
                case '-' :	/* autodec */
                    if (ap->mode != M_PREDEC) goto nomatch;
                    break;
                case 'Q' :	/* quick immediate data */
                    /* check for immediate */
                    if (ap->mode != 7) goto nomatch;
                    if (ap->reg != 4) goto nomatch;
                    /* on first pass, assume it matches */
                    if (pass==2 && (ap->ext.value < 0 || ap->ext.value > 8))
                        goto nomatch;
                    break;
                case 'M' :	/* moveq immediate data */
                    /* check for immediate */
                    if (ap->mode != M_PC) goto nomatch;
                    if (ap->reg != M_IMMED) goto nomatch;
                    if (pass==2 &&
                        (ap->ext.value < -128 || ap->ext.value > 255))
                        goto nomatch;
                    break;
                case 'T' :	/* trap vector */
                    /* check for immediate */
                    if (ap->mode != M_PC) goto nomatch;
                    if (ap->reg != M_IMMED) goto nomatch;
                    if (pass==2 &&
                        (ap->ext.value < 0 || ap->ext.value > 15))
                        goto nomatch;
                    break;
                case '#' :	/* immediate data */
                    if (ap->mode != M_PC) goto nomatch;
                    if (ap->reg != M_IMMED) goto nomatch;
                    break;
                case 'B' :	/* branch displacement */
                    /* check for addr32 */
                    if (ap->mode != M_PC) goto nomatch;
                    if (ap->reg != M_ABS32) goto nomatch;
                    break;
                case 'd' :	/* displacement & register */
                    /* check for d(An) */
                    if (ap->mode != M_AREG_OFF) goto nomatch;
                    break;
                case 'C' :	/* CCR */
                    if (ap->mode != M_CCR) goto nomatch;
                    break;
                case 'S' :	/* SR */
                    if (ap->mode != M_SR) goto nomatch;
                    break;
                case 'U' :	/* USP */
                    if (ap->mode != M_USP) goto nomatch;
                    break;
                case 'O' :	/* bitfield offset/width */
                    if (ap->mode != M_BITFIELD) goto nomatch;
                    break;
                case 'c' :	/* control register */
                    if (ap->mode != M_CONTROL) goto nomatch;
                    break;

                    /* check for general effective addresses */
                case '*' :	/* all */
                    break;
                case '~' :	/* alterable memory */
                    if (!check_ea(ap, am_mask1, am_mask2))
                        goto nomatch;
                    break;
                case '%' :	/* alterable */
                    if (!check_ea(ap, a_mask1, a_mask2))
                        goto nomatch;
                    break;
                case ';' :	/* data */
                    if (!check_ea(ap, d_mask1, d_mask2))
                        goto nomatch;
                    break;
                case '@' :	/* data but not immediate */
                    if (!check_ea(ap, d_not_im_mask1, d_not_im_mask2))
                        goto nomatch;
                    break;
                case '!' :	/* control */
                    if (!check_ea(ap, c_mask1, c_mask2))
                        goto nomatch;
                    break;
                case '&' :	/* alterable control */
                    if (!check_ea(ap, ac_mask1, ac_mask2))
                        goto nomatch;
                    break;
                case '$' :	/* alterable data */
                    if (!check_ea(ap, ad_mask1, ad_mask2))
                        goto nomatch;
                    break;
                case '?' :	/* alterable control or data reg */
                    if (!check_ea(ap, ac_or_Dn_mask1, ac_or_Dn_mask2))
                        goto nomatch;
                    break;
                case '/' :	/* control or data register */
                    if (!check_ea(ap, c_or_Dn_mask1, c_or_Dn_mask2))
                        goto nomatch;
                    break;
                }
            /* by this time all the args match */
            goto match;
        }
    nomatch:
        op++;
    }
    /* by this time, none of the opcodes in the table match */
    error("bad arguments to instruction\n");
    return;

 match:
    /* check that the opcode is legal for target processor */
    if (op->flags&O_M68010 && processor==0)
        error("68010 instruction");
    if (op->flags&O_M68020 && processor<20)
        error("68020 instruction");
    if (pass==1 && op->flags&O_WARN && processor==1)
        warning("use of this instruction is discouraged!");
    if (op->flags&O_NEMU && processor==1)
        error("This instruction cannot be emulated!");

    /* insert the args into the opcode & create the extension words */
    opword = op->opcode>>16;
    n_extwords=0;
    /* check for 2 word instructions */
    if (op->match & 0xffff) {
        n_extwords=1;
        extword[0] = op->opcode & 0xffff;
    }
    /* for movem.z ea,reglist the order of the arguments in memory
       is different from the order of the arguments in the source
       so I swap them here */
    if ((op->flags & O_MOVEM) &&
        n_args == 2 &&
        args[0].mode != M_POSTINC && /* not movem.l (An)+,regs */
        args[1].mode == M_PC &&      /* 2nd arg a reg list ? */
        args[1].reg == M_IMMED) {
        struct ea t;
        t = args[0];
        args[0] = args[1];
        args[1] = t;
        p = "#w!s"; /* point at our fake arg pattern */
    } else
        p=op->args;		/* point at the argument pattern */
    for (j=0, ap=args; j<n_args; j++, p+=2, ap++)
        switch (*p) {
        case 'D' :	/* data register */
        case 'A' :	/* address register */
        case '+' :	/* autoinc */
        case '-' :	/* autodec */
            e.value = ap->reg;
            ins_op(&e, p[1], 3);
            break;
        case 'R' :	/* any register */
            e.value = ap->reg+(ap->mode<<3);
            ins_op(&e, p[1], 4);
            break;
        case 'Q' :	/* quick immediate data */
			/* adjust so that the #8 gets inserted correctly */
            if (ap->ext.value == 8) ap->ext.value = 0;
            ins_op(&ap->ext, p[1], 3);
            break;
        case 'M' :	/* moveq immediate data */
            ins_op(&ap->ext, p[1], 8);
            break;
        case 'T' :	/* trap vector */
            ins_op(&ap->ext, p[1], 4);
            break;
        case '#' :	/* immediate data */
            if (op->flags&O_MOVEM2) {
                /* for movem ...,-(An) need to reverse bit order */
                inttype a=0, b=ap->ext.value, i;
                for (i=0; i<16; i++, a *= 2, b /= 2)
                    a |= b&1;
                ap->ext.value = a/2;
            }
            switch (p[1]) {
            case 'b' : /* byte immed */
                ins_op(&ap->ext, p[1], 8);
                break;
            case 'w' : /* word */
                ins_op(&ap->ext, p[1], 16);
                break;
            case 'l' : /* long */
                ins_op(&ap->ext, p[1], 32);
            }
            break;
        case 'B' :	/* branch displacement */
            if (p[1] == 'g')
                /* optimise branch displacement */
                do_branch(&ap->ext);
            else {
                /* check that the branch destination is an address */
                if (!ap->ext.sym)
                    error("branches must be to an address");
                /* only adjust if address is defined in this file
                 * if not, then an offset on the undefined symbol
                 * will be stored, and the linker will fix the
                 * reference */
                if (ap->ext.sym->type == S_RELOC) {
                    ap->ext.sym = 0;
                    ap->ext.value -= (current_section->pc + 2);
                }
                if (p[1] == 's') { /* byte displacement ? */
                    /* check for range errors */
                    if (pass==2) {
                        if (ap->ext.sym && ap->ext.sym->type == S_XREF)
                            warning("short branch to external label");
                        if (ap->ext.value == 0)
                            if (!ap->ext.sym || ap->ext.sym->type != S_XREF)
                                error("short branch with zero offset");
                        if (ap->ext.value < -128 ||
                            ap->ext.value > 127)
                            error("short branch range error");
                    }
                    ins_op(&ap->ext, p[1], 8);
                } else { /* must be word displacement */
                    if (pass==2)
                        if (ap->ext.value < -32768 ||
                            ap->ext.value > 32767)
                            error("branch range error");
                    ins_op(&ap->ext, p[1], 16);
                }
            }
            break;
        case 'd' :	/* displacement & register */
            e.value = ap->reg;
            ins_op(&e, p[1], 3);
            /* check for external 16 bit ref & add if nesc */
            if (ap->ext.sym)
            {
                if (ap->ext.sym->type == S_RELOC)
                    add_reloc16An(ap->ext.sym, current_section->pc +
                                  2 + 2*n_extwords);
                else
                    add_ref16An(ap->ext.sym, current_section->pc +
                                2 + 2*n_extwords);
            }
            extword[n_extwords++] = ap->ext.value;
            break;
        case 'C' :	/* CCR */
            break;
        case 'S' :	/* SR */
            break;
        case 'U' :	/* USP */
            break;
        case 'O' :	/* bitfield offset/width */
            ins_op(&ap->ext, p[1], 6);
            break;
        case 'c' :	/* control register */
            e.value = ap->reg;
            ins_op(&e, p[1], 12);
            break;

            /* check for general effective addresses */
        case '*' :	/* all */
        case '~' :	/* alterable memory */
        case '%' :	/* alterable */
        case ';' :	/* data */
        case '@' :	/* data but not immediate */
        case '!' :	/* control */
        case '&' :	/* alterable control */
        case '$' :	/* alterable data */
        case '?' :	/* alterable control or data reg */
        case '/' :	/* control or data register */
            ins_ea(ap, p[1]);
            break;
        }
    /* output the finished instruction */
    output_word(opword);
    for (i=0; i<n_extwords; i++)
        output_word(extword[i]);
    return;
}

