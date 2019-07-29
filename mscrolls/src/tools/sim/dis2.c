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

#include "dis.h"
#include "aline.h"
#include "kem.h"
#include "dis2.h"
#include "dis1.h"

extern int pc;
extern int maggot_rolls;

void dis_ea(int op, int a)
{
    int mode, arg, size, b, d;

    mode = bits(a, 5, 3);
    arg = bits(a, 2, 0);
    switch (mode) {
    case 0: dprint ("D%d", arg);
        break;
    case 1: dprint ("A%d", arg);
        break;
    case 2: dprint ("(A%d)", arg);
        break;
    case 3: dprint ("(A%d)+", arg);
        break;
    case 4: dprint ("-(A%d)", arg);
        break;
    case 5:
        if (maggot_rolls && (arg==4 || arg==6))
            dprint("%s(A%d)", addr_str(readword()), arg);
        else
            dprint ("%d(A%d)", readword(), arg);
        break;
    case 6: {
        b = readword();
        d = b & 0XFF;
        if (d > 127) d = byteext(d);
        dprint ("%d(A%d,", d, arg);
        dprint (bits(b, 15, 15) == 1 ? "A%d" : "D%d",
                bits(b, 14, 12));
        if (bits(b, 11, 11) == 1) dprint (".L");
        dprint (")");
        break;
    }
    case 7: switch (arg) {
        case 0: 
            dprint ("%s", addr_str(readword()&0xffff));
            break;
        case 1: d = readword();
            dprint ("%s", addr_str(d*0x10000 + (readword()&0xffff)));
            break;
        case 2:
            {
                int temp=pc;
                dprint("%s(PC)",addr_str(temp+wordext(readword())));
            }
            break;
        case 3: {
            b = readword();
            d = b & 0XFF;
            if (d > 127) d = byteext(d);
            dprint ("$%02X(PC,", d&0xff);
            dprint (bits(b, 15, 15) == 1 ? "A%d" : "D%d",
                    bits(b, 14, 12));
            if (bits(b, 11, 11) == 1) dprint (".L");
            dprint (")");
            break;
        }
        case 4: {
            size = opsize(op, a);
            if (size == 0) dprint ("#$%02x", readword());
            if (size == 1) dprint ("#$%04x", readword());
            if (size == 2) {
                d = readword();
                dprint ("#$%04X%04X", d&0xffff, readword()&0xffff);
            }
            break;
        }
        default: dprint("???");
        }
    }
}

void writeopi(char* instruction, int op, int a)
{
    int size, d;
    size = opsize(op, a);
    writeop(instruction, size);
    if (size == 0) dprint ("#$%02x,", readword());
    if (size == 1) dprint ("#$%04x,", readword());
    if (size == 2) {
        d = readword();
        dprint ("#$%04X%04X,", d&0xffff, readword()&0xffff);
    }
    dis_ea(op, a);
}

int readinstr(int a)
{
    unsigned short an_unsigned_short;

    switch (bits(a, 15, 12))
    {
    case 0: switch (bits(a, 11, 8)) {
        case 12: return i_cmpi;
        case 10: 
            if ((a & 0xFF)==0x3C) return i_eoriCCR;
            return i_eori;
        case  6: return i_addi;
        case  4: return i_subi;
        case  2:
            if ((a & 0xFF)==0x3C) return i_andiCCR;
            return i_andi;
        case  0:
            if ((a & 0xFF)==0x3C) return i_oriCCR;
            return i_ori;
        case  8: return i_sbit;
        default: if ((bits(a, 5, 3) == 1) &&
                     (4 <= bits(a, 8, 6)) &&
                     (bits(a, 8, 6) <= 7)) return i_movep;
            if (bits(a, 8, 8) == 1) return i_dbit;
            return i_none;
        }

    case  1:
    case  2:
    case  3: return bits(a, 8, 6) == 1 ? i_movea : i_move;

    case  4: switch (bits(a, 11, 8)) {
        case  0:
            if (bits(a, 7, 6) == 3) return i_mfsr;
            else return i_negx;
        case  2: return i_clr;
        case  4:
            if (bits(a, 7, 6) == 3) return i_mtccr;
            else return i_neg;
        case  6:
            if (bits(a, 7, 6) == 3) return i_mtsr;
            else return i_not;
        case  8: switch (bits(a, 7, 6)) {
            case  0: return i_nbcd;
            case  1: if (bits(a, 5, 3) == 0)
                    return i_swap;
                else return i_pea;
            case  2: if (bits(a, 5, 3) == 0)
                    return i_extw;
                else return i_mmfreg;
            case  3: if (bits(a, 5, 3) == 0)
                    return i_extl;
                else return i_mmfreg;
            }
			case 10:
				if (bits(a, 7, 6) == 3)
					if (bits(a, 5, 0) == 074) return i_illegal;
					else return i_tas;
            else return i_tst;
        case 12: if (bits(a, 7, 7) == 1)
                return i_mmtreg;
            else return i_none;
        case 14: switch (bits(a, 7, 3)) {
            case  8:
            case  9: return i_trap;
            case 10: return i_link;
            case 11: return i_unlk;
            case 12: return i_mtusp;
            case 13: return i_mfusp;
            case 14: switch (bits(a, 2, 0)) {
                case 0: return i_reset;
                case 1: return i_nop;
                case 2: return i_stop;
                case 3: return i_rte;
                case 5: return i_rts;
                case 6: return i_trapv;
                case 7: return i_rtr;
                }

            default: if (bits(a, 7, 6) == 2) return i_jsr;
                if (bits(a, 7, 6) == 3) return i_jmp;
                return i_none;
            }
        default: if (bits(a, 8, 6) == 6) return i_chk;
            if (bits(a, 8, 6) == 7) return i_lea;
            return i_none;
        }

    case  5: if (bits(a, 7, 3) == 25) return i_dbcc;
        if (bits(a, 7, 6) == 3) return i_scc;
        if (bits(a, 8, 8) == 0) return i_addq;
        else return i_subq;

    case  6: return i_bcc;
    case  7: if (bits(a, 8, 8) == 0) return i_moveq;
        else return i_none;

    case  8: if (bits(a, 8, 4) == 16) return i_sbcd;
        if (bits(a, 8, 6) == 7) return i_divs;
        if (bits(a, 8, 6) == 3) return i_divu;
        return i_or;

    case  9: if ((bits(a, 5, 4) == 0) &&
                 (bits(a, 8, 8) == 1) &&
                 (bits(a, 7, 6) <= 2)) return i_subx;
        if ((bits(a, 8, 6) == 3) || (bits(a, 8, 6) == 7))
            return i_suba;
        return i_sub;

    case 10:
        an_unsigned_short = a & 0xFFFF;

	/* for speed put the bespoke ops first */

        if (an_unsigned_short==RET) return i_RET;
        if ((an_unsigned_short&0xFF00)==CALL) return i_CALL;
        if ((an_unsigned_short&0xFF00)==TEST) return i_TEST;
        if ((an_unsigned_short&0xFF00)==PUSHM) return i_PUSHM;
        if ((an_unsigned_short&0xFF00)==PULLM) return i_PULLM;
        if (an_unsigned_short==CHAROUT2) return i_CHAROUT2;
        if (an_unsigned_short==SEARCH) return i_SEARCH;
        if (an_unsigned_short==SETNOUNS) return i_SETNOUNS;
        if (an_unsigned_short==GETNOUNS) return i_GETNOUNS;
        if (an_unsigned_short==TEXT) return i_TEXT;
        if (an_unsigned_short==QCARRIED) return i_QCARRIED;
        if (an_unsigned_short==NOUNSCAN) return i_NOUNSCAN;
        if (an_unsigned_short==WORDSCAN) return i_WORDSCAN;
        if (an_unsigned_short==LOADFILE) return i_LOADFILE;
        if (an_unsigned_short==SAVEFILE) return i_SAVEFILE;
        if (an_unsigned_short==RANDOM1) return i_RANDOM1;
        if (an_unsigned_short==RANDOM2) return i_RANDOM2;
        if (an_unsigned_short==GETDATA) return i_GETDATA;
        if (an_unsigned_short==GETTIED) return i_GETTIED;
        if (an_unsigned_short==FINDZERO) return i_FINDZERO;
        if (an_unsigned_short==GRAPHICS) return i_GRAPHICS;
        if (an_unsigned_short==PRINTER) return i_PRINTER;
        if (an_unsigned_short==RESTART) return i_RESTART;
        if (an_unsigned_short==QUIT) return i_QUIT;
			if (an_unsigned_short==GETWTAB) return i_GETWTAB;
			if (an_unsigned_short==PUTWTAB) return i_PUTWTAB;
			if (an_unsigned_short==PRINTWORD) return i_PRINTWORD;
			if (an_unsigned_short==COPYWORD) return i_COPYWORD;
			if (an_unsigned_short==SETNE) return i_SETNE;
			if (an_unsigned_short==SETEQ) return i_SETEQ;
			if (an_unsigned_short==SETNE_RET) return i_SETNE_RET;
			if (an_unsigned_short==SETEQ_RET) return i_SETEQ_RET;
			if (an_unsigned_short==POPSP_RET) return i_POPSP_RET;
			if (an_unsigned_short==MAKE_DIFF) return i_MAKE_DIFF;
			if (an_unsigned_short==UNDO_DIFF) return i_UNDO_DIFF;
		if (an_unsigned_short==GET_NEXT_EV) return i_GET_NEXT_EV;
			if (an_unsigned_short==PROMPT_EV) return i_PROMPT_EV;
			if (an_unsigned_short==NOTIFY) return i_NOTIFY;

        return i_CHARIN;
		

    case 11: if ((bits(a, 8, 8) == 1) &&
                 (bits(a, 5, 3) == 1) &&
                 (bits(a, 7, 6) <= 2)) return i_cmpm;
        if ((4 <= bits(a, 8, 6)) &&
            (bits(a, 8, 6) <= 6)) return i_eor;
        if ((bits(a, 8, 6) == 3) || (bits(a, 8, 6) == 7))
            return i_cmpa;
        return i_cmp;

    case 12: switch (bits(a, 8, 6)) {
        case  3: return i_mulu;
        case  7: return i_muls;
        case  4: if (bits(a, 5, 4) == 0)
                return i_abcd;
            else return i_and;
        case  5: if (bits(a, 5, 3) == 0) return i_exgd;
            if (bits(a, 5, 3) == 1) return i_exga;
            return i_and;
        case  6: if (bits(a, 5, 3) == 1) return i_exgm;
        default: return i_and;
        }

    case 13: if ((bits(a, 8, 8) == 1) &&
                 (bits(a, 5, 4) == 0) &&
                 (bits(a, 7, 6) <= 2)) return i_addx;
        if ((bits(a, 8, 6) == 3) || (bits(a, 8, 6) == 7))
            return i_adda;
        return i_add;

    case 14: if ((bits(a, 7, 6) == 3) &&
                 (bits(a, 11, 11) == 0))
            return i_mshift;
        else return i_dshift;

    case 15: return i_CHAROUT;
    default: return i_none;
    }
}
