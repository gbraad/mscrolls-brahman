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

#include "dis.h"
#include "alines.h"
#include "emu.h"
#include "bits.h"
#include "dis2.h"
#include "dis1.h"

#ifdef XEN
long spacetable, minspaceentry;
#endif

extern long pc;
extern int maggot_rolls;
extern long minspaceentry;

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

		case 15:
			/*
			switch(a&0x7FF) {
				case 0x7FF: return i_CHAROUT;
				case 0x7FE: return i_GETWORD;
				case 0x7FD: return i_VALID_NP;
				case 0x7FC: return i_CHK4PT;
				case 0x7FB: return i_GETNPC;
				case 0x7FA: return i_GETRELN;
				case 0x7F9: return i_GET_CONT;
				case 0x7F8: return i_DBS;
				case 0x7F7: return i_ISARE;
				case 0x7F6: return i_ISAREN;
				case 0x7F5: return i_P_BTN;
				case 0x7F4: return i_P_SCN;
				case 0x7F3: return i_P_MSG;
				case 0x7F2: return i_P_NOUN;
				case 0x7F1: return i_P_SUB;
				case 0x7F0: return i_P_SUBV;
				case 0x7EF: return i_SETOUT;
				case 0x7EE: return i_P_TN;
				case 0x7ED: return i_P_TNSTOP;
				case 0x7EC: return i_UTIL_BS;
				case 0x7EB: return i_FULLSTOP;
				case 0x7EA: return i_BSFULLSTOP;
				case 0x7E9: return i_CARRIED;
				case 0x7E8: return i_CALCNET;
				case 0x7E7: return i_CHAR_SUB;
				case 0x7E6: return i_CHKNPC;
				case 0x7E5: return i_CLRADJ;
				case 0x7E4: return i_D72;
				case 0x7E3: return i_DBEXIT;
				case 0x7E2: return i_DEAD;
				case 0x7E1: return i_DECODE;
				case 0x7E0: return i_FIXCHK;
				case 0x7DF: return i_FIXCONT;
				case 0x7DE: return i_FIXIREL;
				case 0x7DD: return i_FIXSYS;
				case 0x7DC: return i_FNDWRD;
				case 0x7DB: return i_GETADJPH;
				case 0x7DA: return i_DOGETNOUNS;
				case 0x7D9: return i_GETVECT;
				case 0x7D8: return i_INCNOUN2;
				case 0x7D7: return i_ISDOFRE;
				case 0x7D6: return i_MNPH;
				case 0x7D5: return i_NOERR;
				case 0x7D4: return i_P_FROM;
				case 0x7D3: return i_P_LIST;
				case 0x7D2: return i_P_OBJPRN;
				case 0x7D1: return i_P_PRN;
				case 0x7D0: return i_P_QN;
				case 0x7CF: return i_P_SAY;
				case 0x7CE: return i_P_TNIN;
				case 0x7CD: return i_P_TSNOUN;
				case 0x7CC: return i_P_VAGE;
				case 0x7CB: return i_P_VERB;
				case 0x7CA: return i_PR20;
				case 0x7C9: return i_R_NOUN;
				case 0x7C8: return i_R_NOUN2;
				case 0x7C7: return i_RUSURE;
				case 0x7C6: return i_SCREEN;
				case 0x7C5: return i_DOSEARCH;
				case 0x7C4: return i_DOSEARCH2;
				case 0x7C3: return i_TOPLOC;
				case 0x7C2: return i_UTEXIT;
				case 0x7C1: return i_VALID;
				case 0x7C0: return i_YCANT;
				case 0x7BF: return i_YCANTNOW;
				case 0x7BE: return i_YESERR;
				case 0x7BD: return i_YCANTW;
				case 0x7BC: return i_P_SAY2;
				case 0x7BB: return i_GETNOUN2;
				case 0x7BA: return i_P_LISTSTOP;
				case 0x7B9: return i_VALID_WO;
				default: return i_MSG;
				}
			*/
			if (!minspaceentry) minspaceentry = 0x770;
			if ((a & 0x7ff) >= minspaceentry) return i_DO;
			return i_MSG;

		default: return i_none;
	}
}
