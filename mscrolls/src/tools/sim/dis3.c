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

#include <assert.h>
#include <string.h>
#include "dis.h"
#include "kem.h"
#include "dis3.h"
#include "dis2.h"
#include "dis1.h"
#include "get_ea.h"
#include "symbols.h"

extern int pc;

static inline void dogo(char* s, int op)
{
    if (op & 0x0800) dprint("DOGO     %s",s);
    else dprint("DO       %s",s);
}

void writeinstr(int op, int a)
{
    int b, i, val, dir, addr, pcx, oplen, mode, rs, rl, end;
    char *shiftop, *bitop, *str;
    const char* cond;
    
    switch (op)
    {
    case i_abcd:
        writeop("ABCD", -1);
        if (bits(a, 3, 3) == 0)
            dprint("-(A%d),-(A%d)", bits(a, 2, 0), bits(a, 11, 9));
        else dprint("D%d,D%d", bits(a, 2, 0), bits(a, 11, 9));
        break;

    case i_add:
        writeop("ADD", bits(a, 8, 6));
        if (bits(a, 8, 6) <= 2) {
            dis_ea(op, a);
            dprint(",D%d", bits(a, 11, 9));
        }
        if ((4 <= bits(a, 8, 6)) && (bits(a, 8, 6) <= 6)) {
            dprint("D%d,", bits(a, 11, 9));
            dis_ea(op, a);
        }
        break;

    case i_adda:
        writeop("ADDA", opsize(op, a));
        dis_ea(op, a);
        dprint(",A%d", bits(a, 11, 9));
        break;

    case i_addi:
        writeopi("ADDI",op,  a);
        break;

    case i_addq:
        writeop("ADDQ", bits(a, 7, 6));
        dprint("#%d,", shortval(bits(a, 11, 9)));
        dis_ea(op, a);
        break;

    case i_addx:
        writeop("ADDX", bits(a, 7, 6));
        if (bits(a, 3, 3) == 0)
            dprint("-(A%d),-a(%d)", bits(a, 2, 0), bits(a, 11, 9));
        else dprint("D%d,D%d", bits(a, 2, 0), bits(a, 11, 9));
        break;

    case i_and:
        writeop("AND", bits(a, 8, 6));
        if (bits(a, 8, 6) <= 2) {
            dis_ea(op, a);
            dprint(",D%d", bits(a, 11, 9));
        }
        if ((4 <= bits(a, 8, 6)) && (bits(a, 8, 6) <= 6)) {
            dprint("D%d,", bits(a, 11, 9));
            dis_ea(op, a);
        }
        break;

    case i_andi:
        writeopi("ANDI",op,  a);
        break;

    case i_dshift:
        dir = bits(a, 8, 8);
        switch (bits(a, 4, 3)) {
        case 0: shiftop = dir == 0 ? "ASR" : "ASL"; break;
        case 1: shiftop = dir == 0 ? "LSR" : "LSL"; break;
        case 2: shiftop = dir == 0 ? "ROXR" : "ROXL"; break;
        case 3: shiftop = dir == 0 ? "ROR" : "ROL"; break;
        }
        writeop(shiftop, bits(a, 7, 6));
        if (bits(a, 5, 5) == 0)
            dprint("#%d,D%d", shortval(bits(a, 11, 9)), bits(a, 2, 0));
        else dprint("D%d,D%d", bits(a, 11, 9), bits(a, 2, 0));
        break;

    case i_mshift:
        dir = bits(a, 8, 8);
        switch (bits(a, 10, 9)) {
        case 0: shiftop = dir == 0 ? "ASR" : "ASL"; break;
        case 1: shiftop = dir == 0 ? "LSR" : "LSL"; break;
        case 2: shiftop = dir == 0 ? "ROXR" : "ROXL"; break;
        case 3: shiftop = dir == 0 ? "ROR" : "ROL"; break;
        }
        writeop(shiftop, -1);
        dis_ea(op, a);
        break;

    case i_bcc:
        addr = bits(a,7,0);
        pcx = pc;
        cond = condcode(op,a);
        oplen = 3;
        dprint("B");
        dprint(cond);
        if (addr > 127) addr = byteext(addr);
		if (addr != 0 && addr != -1)
        {
            dprint(".S");
            oplen = oplen + 2;
        }
        for (i=oplen; i<=8; i++) dprint(" ");
        if (addr == 0) addr = wordext(readword());
		if (addr == -1) addr = readlong();
        dprint("%s", addr_str((addr + pcx) & 0xffffff));
        break;

    case i_dbcc:
        pcx = pc;
        cond = condcode(op,a);
        oplen = 4;
        dprint("DB");
        dprint(cond);
        dprint("     ");
        dprint("D%d,",bits(a,2,0));
        addr = wordext(readword());
        dprint("%s", addr_str((addr + pcx) & 0xffffff));
        break;

    case i_scc:
        pcx = pc;
        cond = condcode(op,a);
        oplen = 3;
        dprint("S");
        dprint(cond);
        dprint("      ");
        dis_ea(op,a);
        break;

    case i_dbit:
        switch (bits(a, 7, 6)) {
        case 0: bitop = "BTST"; break;
        case 1: bitop = "BCHG"; break;
        case 2: bitop = "BCLR"; break;
        case 3: bitop = "BSET"; break;
        }
        writeop(bitop, -1);
        dprint("D%d,", bits(a, 11, 9));
        dis_ea(op, a);
        break;

    case i_sbit:
        switch (bits(a, 7, 6))
        {
        case 0: bitop = "BTST"; break;
        case 1: bitop = "BCHG"; break;
        case 2: bitop = "BCLR"; break;
        case 3: bitop = "BSET"; break;
        }
        writeop(bitop, -1);
        dprint("#%d,", readword());
        dis_ea(op, a);
        break;

    case i_chk:
        writeop("CHK", -1);
        dis_ea(op, a);
        dprint(",D%d", bits(a, 11, 9));
        break;

    case i_clr:
        writeop("CLR", bits(a, 7, 6));
        dis_ea(op, a);
        break;

    case i_cmp:
        writeop("CMP", bits(a, 8, 6));
        dis_ea(op, a);
        dprint(",D%d", bits(a, 11, 9));
        break;

    case i_cmpa:
        writeop("CMPA", opsize(op, a));
        dis_ea(op, a);
        dprint(",A%d", bits(a, 11, 9));
        break;

    case i_cmpi:
        writeopi("CMPI",op,  a);
        break;

    case i_cmpm:
        writeop("CMPM", bits(a, 7, 6));
        dprint("(A%d)+,(A%d)+", bits(a, 2, 0), bits(a, 11, 9));
        break;

    case i_divs:
        writeop("DIVS", -1);
        dis_ea(op, a);
        dprint(",D%d", bits(a, 11, 9));
        break;

    case i_divu:
        writeop("DIVU", -1);
        dis_ea(op, a);
        dprint(",D%d", bits(a, 11, 9));
        break;

    case i_eor:
        writeop("EOR", bits(a, 8, 6));
        dprint("D%d,", bits(a, 11, 9));
        dis_ea(op, a);
        break;

    case i_eori:
        writeopi("EORI",op,a);
        break;

    case i_exgd:
    case i_exga:
        mode = bits(a, 7, 3);
        str = "D%d,D%d";
        writeop("EXG", -1);
        if (mode == 9) str = "A%d,A%d";
        if (mode == 17) str = "D%d,A%d";
        dprint(str, bits(a, 11, 9), bits(a, 2, 0));
        break;

    case i_extw:
    case i_extl:
        writeop("EXT", bits(a, 8, 6) == 2 ? 1 : 2);
        dprint("D%d", bits(a, 2, 0));
        break;

    case i_jmp:
        writeop("JMP", -1);
        dis_ea(op, a);
        break;

    case i_jsr:
        writeop("JSR", -1);
        dis_ea(op, a);
        break;

    case i_lea:
        writeop("LEA", -1);
        dis_ea(op, a);
        dprint(",A%d", bits(a, 11, 9));
        break;

    case i_link:
        writeop("LINK", -1);
        dprint("A%d,#%d", bits(a, 2, 0), readword());
        break;

    case i_move:
        b = (bits(a, 8, 6)<<3) | bits(a, 11, 9) | (bits(a, 15, 12)<<12);
        writeop("MOVE", opsize(op, a));
        dis_ea(op, a);
        dprint(",");
        dis_ea(op, b);
        break;

    case i_movea:
        writeop("MOVEA", opsize(op, a));
        dis_ea(op, a);
        dprint(",A%d", bits(a, 11, 9));
        break;

    case i_mtccr:
        writeop("MOVE", -1);
        dis_ea(op, a);
        dprint(",CCR");
        break;

    case i_mtsr:
        writeop("MOVE", -1);
        dis_ea(op, a);
        dprint(",SR");
        break;

    case i_mfsr:
        writeop("MOVE", -1);
        dprint("SR,");
        dis_ea(op, a);
        break;

    case i_mtusp:
    case i_mfusp:
        writeop("MOVE", -1);
        if (bits(a, 3, 3) == 0)
            dprint("A%d,USP", bits(a, 2, 0));
        else dprint("USP,A%d", bits(a, 2, 0));
        break;

    case i_mmfreg:
    case i_mmtreg:
        writeop("MOVEM", bits(a, 6, 6) == 0 ? 1 : 2);
        if (bits(a, 10, 10) == 0) {
            b = readword();
            for (i=0; i<=15; i++)
                if ((b>>i)&1) break;
            end = 15-i;
            rs = 0; rl = 0;
            for (i=15; i>=0; i--) {
                if ((b>>i)&1) rl++;
                else
                    if (rl > 0) {
                        if (rl == 1)
                            dprint("%c%d", rs<8 ? 'D' : 'A', rs&7);
                        else
                            dprint("%c%d-%c%d", rs<8 ? 'D' : 'A', rs&7,
                                   (rs+rl-1)<8 ? 'D' : 'A', (rs+rl-1)&7);
                        if (rs+rl-1 < end) dprint("/");
                        rs += rl+1; rl = 0;
                    } else rs++;
            }
            dprint(",");
            dis_ea(op, a);
        } else {
            dis_ea(op, a);
            dprint(",");
            b = readword();
            for (i=15; i>=0; i--)
                if ((b>>i)&1) break;
            end = i;
            rs = 0; rl = 0;
            for (i=0; i<=15; i++) {
                if ((b>>i)&1) rl++;
                else
                    if (rl > 0) {
                        if (rl == 1)
                            dprint("%c%d", rs<8 ? 'D' : 'A', rs&7);
                        else
                            dprint("%c%d-%c%d", rs<8 ? 'D' : 'A', rs&7,
                                   (rs+rl-1)<8 ? 'D' : 'A', (rs+rl-1)&7);
                        if (rs+rl-1 < end) dprint("/");
                        rs += rl+1; rl = 0;
                    } else rs++;
            }
        }
        break;

    case i_movep:
        writeop("MOVEP", bits(a, 6, 6) == 0 ? 1 : 2);
        if (bits(a, 7, 7))
            dprint("%d(A%d),D%d", readword(),
                   bits(a, 2, 0), bits(a, 11, 9));
        else dprint("D%d,%d(A%d)", bits(a, 11, 9),
                    readword(), bits(a, 2, 0));
        break;

    case i_moveq:
        val = a & 0xff;
        if (val > 127) val = byteext(val);
        writeop("MOVEQ", -1);
        dprint("#$%02x,D%d", val, bits(a, 11, 9));
        break;

    case i_muls:
        writeop("MULS", -1);
        dis_ea(op, a);
        dprint(",D%d", bits(a, 11, 9));
        break;

    case i_mulu:
        writeop("MULU", -1);
        dis_ea(op, a);
        dprint(",D%d", bits(a, 11, 9));
        break;

    case i_nbcd:
        writeop("NBCD", -1);
        dis_ea(op, a);
        break;

    case i_neg:
        writeop("NEG", bits(a, 7, 6));
        dis_ea(op, a);
        break;

    case i_negx:
        writeop("NEGX", bits(a, 7, 6));
        dis_ea(op, a);
        break;

    case i_nop:
        dprint("NOP");
        break;

    case i_not:
        writeop("NOT", bits(a, 7, 6));
        dis_ea(op, a);
        break;

    case i_or:
        writeop("OR", bits(a, 7, 6));
        if (bits(a, 8, 6) <= 2) {
            dis_ea(op, a);
            dprint(",D%d", bits(a, 11, 9));
        }
        if ((4 <= bits(a, 8, 6)) && (bits(a, 8, 6) <= 6)) {
            dprint("D%d,", bits(a, 11, 9));
            dis_ea(op, a);
        }
        break;

    case i_ori:
        writeopi("ORI", op,  a);
        break;

    case i_pea:
        writeop("PEA", -1);
        dis_ea(op, a);
        break;

    case i_reset:
        dprint("RESET");
        break;

    case i_rte:
        dprint("RTE");
        break;

    case i_rtr:
        dprint("RTR");
        break;

    case i_rts:
        dprint("RTS");
        break;

    case i_sbcd:
        writeop("SBCD", -1);
        if (bits(a, 3, 3) == 0)
            dprint("-(A%d),-(A%d)", bits(a, 2, 0), bits(a, 11, 9));
        else dprint("D%d,D%d", bits(a, 2, 0), bits(a, 11, 9));
        break;

    case i_stop:
        dprint("STOP");
        break;

    case i_sub:
        writeop("SUB", bits(a, 8, 6));
        if (bits(a, 8, 6) <= 2) {
            dis_ea(op, a);
            dprint(",D%d", bits(a, 11, 9));
        }
        if ((4 <= bits(a, 8, 6)) && (bits(a, 8, 6) <= 6)) {
            dprint("D%d,", bits(a, 11, 9));
            dis_ea(op, a);
        }
        break;

    case i_suba:
        writeop("SUBA", opsize(op, a));
        dis_ea(op, a);
        dprint(",A%d", bits(a, 11, 9));
        break;

    case i_subi:
        writeopi("SUBI", op, a);
        break;

    case i_subq:
        writeop("SUBQ", bits(a, 7, 6));
        dprint("#%d,", shortval(bits(a, 11, 9)));
        dis_ea(op, a);
        break;

    case i_subx:
        writeop("SUBX", bits(a, 7, 6));
        if (bits(a, 3, 3) == 0)
            dprint("-(A%d),-(A%d)", bits(a, 2, 0), bits(a, 11, 9));
        else dprint("D%d,D%d", bits(a, 2, 0), bits(a, 11, 9));
        break;

    case i_swap:
        writeop("SWAP", -1);
        dprint("D%d", bits(a,2,0));
        break;

    case i_tas:
        writeop("TAS", -1);
        dis_ea(op, a);
        break;

    case i_trap:
        writeop("TRAP",-1);
        dprint("#%d", a & 0x0F);
        break;

    case i_trapv:
        writeop("TRAPV", -1);
        break;

    case i_tst:
        writeop("TST", bits(a, 7, 6));
        dis_ea(op, a);
        break;

    case i_unlk:
        writeop("UNLK", -1);
        dprint("A%d", bits(a, 2, 0));
        break;

    case i_SEARCH:
        dprint("SEARCH");
        break;
    case i_GETDATA:
        dprint("GETINFO"); 
        break;
    case i_SETNOUNS: 
        dprint("SETNOUNS"); 
        break;
    case i_NOUNSCAN: 
        dprint("NOUNSCAN"); 
        break;
    case i_WORDSCAN: 
        dprint("WORDSCAN"); 
        break;
    case i_GETNOUNS: 
        dprint("GETNOUNS"); 
        break;
    case i_QCARRIED: 
        dprint("QCARRIED"); 
        break;
    case i_TEXT: 
        dprint("TEXT"); 
        break;
    case i_RANDOM1: 
        dprint("RANDOM1"); 
        break;
    case i_RANDOM2:
        dprint("RANDOM2"); 
        break;
    case i_LOADFILE:
        dprint("LOADFILE");
        break;
    case i_SAVEFILE:
        dprint("SAVEFILE"); 
        break;
    case i_GETWTAB:
        dprint("GETWTAB");
        break;
    case i_PUTWTAB:
        dprint("PUTWTAB");
        break;
    case i_PRINTWORD:
        dprint("PRINTWORD");
        break;
    case i_COPYWORD:
        dprint("COPYWORD");
        break;
    case i_SETNE:
        dprint("SETNE");
        break;
    case i_SETEQ:
        dprint("SETEQ");
        break;
    case i_SETNE_RET:
        dprint("SETNE_RET");
        break;
    case i_SETEQ_RET:
        dprint("SETEQ_RET");
        break;
    case i_POPSP_RET:
        dprint("POPSP_RET");
        break;
    case i_MAKE_DIFF:
        dprint("MAKE_DIFF");
        break;
    case i_UNDO_DIFF:
        dprint("UNDO_DIFF");
        break;
    case i_GET_NEXT_EV:
        dprint("GET_NEXT_EV");
        break;
    case i_PROMPT_EV:
        dprint("PROMPT_EV");
        break;
    case i_NOTIFY:
        dprint("NOTIFY");
        break;
    case i_DO:
        {
            int ad;
            char *cp;
            static int spacetable, minspaceentry;
            
            if (!spacetable)
            {
                spacetable = sym_to_ad("spacetable");
                assert(spacetable);
                minspaceentry = 0x770;
            }
            
            /* find out what the target address is */
            ad = spacetable + (0x7ff-(a & 0x7ff))*2;

            char* p = vaxof68k(ad);
            int a = (p[0]<<8) | p[1];
            
            //ad = ad + wordext(fetch_w(ad));		/* offset from table entry */
            ad = ad + wordext(a);
            cp = sym_from_ad(ad, 0);	/* find the target symbol */
            if (!strncmp(cp, "is_", 3))	/* TST_OPEN etc */
                dprint(cp+3);
            else
                dogo(cp,a);
        }
        break;
    case i_MSG:
        if (a & 0x0800) dprint("MSGGO    %03d",a&0x7FF);
        else dprint("MSG      %03d",a & 0x7FF);
        break;

    case i_CHAROUT:
        dprint("CHAROUT"); 
        break;
    case i_CHAROUT2:
        dprint("CHAROUT2"); 
        break;
    case i_CHARIN:
        dprint("CHARIN");
        break;
    case i_GETTIED:
        dprint("GETTIED");
        break;
    case i_FINDZERO:
        dprint("FINDZERO");
        break;

    case i_RET:			/* fake RTS */
        dprint("RET");
        break;
	
    case i_CALL:			/* fake BSR */
        addr = bits(a,7,0);
        pcx = pc;
        oplen = 4;
        dprint("CALL");
        if (addr > 127) addr = byteext(addr);
		if (addr != 0 && addr != -1)
        {
            dprint(".S");
            oplen = oplen + 2;
        }
        for (i=oplen; i<=8; i++) dprint(" ");
        if (addr == 0) addr = wordext(readword());
		if (addr == -1) addr = readlong();
        dprint("%s", addr_str((addr + pcx) & 0xffffff));
        break;

    case i_TEST:
        writeop("TEST", bits(a, 7, 6));
        dis_ea(op, a);
        break;

    case i_PUSHM:
		writeop("PUSH", bits(a, 6, 6) == 0 ? 1 : 2);
        b = readword();
        for (i=0; i<=15; i++)
            if ((b>>i)&1) break;
        end = 15-i;
        rs = 0; rl = 0;
        for (i=15; i>=0; i--) {
            if ((b>>i)&1) rl++;
            else
                if (rl > 0) {
                    if (rl == 1)
                        dprint("%c%d", rs<8 ? 'D' : 'A', rs&7);
                    else
                        dprint("%c%d-%c%d", rs<8 ? 'D' : 'A', rs&7,
                               (rs+rl-1)<8 ? 'D' : 'A', (rs+rl-1)&7);
                    if (rs+rl-1 < end) dprint("/");
                    rs += rl+1; rl = 0;
                } else rs++;
        }
        dprint(",");
        dis_ea(op, a);
        break;
	
		
    case i_PULLM:
		writeop("PULL", bits(a, 6, 6) == 0 ? 1 : 2);
        dis_ea(op, a);
        dprint(",");
        b = readword();
        for (i=15; i>=0; i--)
            if ((b>>i)&1) break;
        end = i;
        rs = 0; rl = 0;
        for (i=0; i<=15; i++) {
            if ((b>>i)&1) rl++;
            else
                if (rl > 0) {
                    if (rl == 1)
                        dprint("%c%d", rs<8 ? 'D' : 'A', rs&7);
                    else
                        dprint("%c%d-%c%d", rs<8 ? 'D' : 'A', rs&7,
                               (rs+rl-1)<8 ? 'D' : 'A', (rs+rl-1)&7);
                    if (rs+rl-1 < end) dprint("/");
                    rs += rl+1; rl = 0;
                } else rs++;
        }
        break;

    case i_GRAPHICS:
        dprint("GRAPHICS");
        break;
    case i_PRINTER:
        dprint("PRINTER");
        break;
    case i_RESTART:
        dprint("RESTART");
        break;
    case i_QUIT:
        dprint("QUIT");
        break;
    case i_A_line:
        dprint("A_line"); 
        break;
    case i_F_line:
        dprint("F_line"); 
        break;

    case i_eoriCCR:
        writeop("EORI",-1);
        dprint("#$%04x,CCR",readword());
        break;

    case i_andiCCR:
        writeop("ANDI",-1);
        dprint("#$%04x,CCR",readword());
        break;

    case i_oriCCR:
        writeop("ORI",-1);
        dprint("#$%04x,CCR",readword());
        break;

	case i_illegal:
		dprint("ILLEGAL");
		break;
    default:
        dprint("????");
        break;
    }
}
