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
#include "kem.h"
#include "do.h"
#include "aline.h"
#include "dis1.h"
#include "dis2.h"
#include "dis3.h"
#include "get_ea.h"

extern int InstructionCount;

#define _M68(_x) ((M68*)(_x))

char *PC;

u_int twotothe[] = {1,2,4,8,16,32,64,128,256};
extern short code[];
char *memory=(char *)&code[0];

struct registers registers;
struct flags flags;

void emulate(int op, int a)
{
    char *source,*dest;
    int size, cond, b;
    InstructionCount++;
    switch (op)
    {
    case i_abcd:
        do_illegal(i_abcd);
        break;
    case i_add:
        size=opsize(op,a);
        if (bits(a,8,8) == 0) 	/* 0=> ADD <ea>,Dn */
        {
            source=get_sea(op,a);
            dest=adjustsize(&registers.D[bits(a,11,9)].L,size);
        }
        else					/* 1=> ADD Dn,<ea> */
        {
            source=adjustsize(&registers.D[bits(a,11,9)].L,size);
            dest=get_dea(op,a);
        }
        do_add(size,_M68(source),_M68(dest));
        break;

    case i_adda:				/* always ADDA <ea>,An */
        size=opsize(op,a);
        source=get_sea(op,a);
        dest=adjustsize(&registers.A[bits(a,11,9)].L,size);
        do_add(size,_M68(source),_M68(dest));
        break;

    case i_addi:				/* ADD #,<ea> */
        source=PC;
        size=opsize(op,a);
        switch (size) {
        case BYTE:
            source=source+1;
        case WORD:
            PC=PC+2;
            break;
        case LONG:
            PC=PC+4;
            break;
        }
        dest=get_dea(op,a);
        do_add(size,_M68(source),_M68(dest));
        break;
		
    case i_addq:				/* ADD #3bits,<ea> */
        {
            static u_int temp;
            size=opsize(op,a);
            temp=reverse_l(shortval(bits(a,11,9)));
            source=adjustsize(&temp,size);
            dest=get_dea(op,a);
            do_add(size,_M68(source),_M68(dest));
        }
        break;

    case i_addx:
        do_illegal(i_addx);
        break;

    case i_and:
        size=opsize(op,a);
        if (bits(a,8,8) == 0) 	/* 0=> AND <ea>,Dn */
        {
            source=get_sea(op,a);
            dest=(char *)adjustsize(&registers.D[bits(a, 11, 9)].L,size);
            do_and(size,_M68(source),_M68(dest));
            set_flags(size,_M68(dest));
        }
        else					/* 1=> AND Dn,<ea> */
        {
            source=(char *)adjustsize(&registers.D[bits(a,11,9)].L,size);
            dest=get_dea(op,a);
            do_and(size,_M68(source),_M68(dest));
            set_flags(size,_M68(dest));
        }
        break;

    case i_andi:				/* AND #,<ea> */
        source=PC;
        size=opsize(op,a);
        switch (size) {
        case BYTE:
            source=source+1;
        case WORD:
            PC=PC+2;
            break;
        case LONG:
            PC=PC+4;
            break;
        }
        dest=get_dea(op,a);
        do_and(size,_M68(source),_M68(dest));
        set_flags(size,_M68(dest));
        break;

    case i_dshift:					/* SHIFT #/Dn,Dn i.e. to register */
        {
            static u_int temp;
            size=opsize(op,a);
            if (bits(a,5,5) == 0)		/* 0=>immediate 1=>register count */
            {
                temp=reverse_l(shortval(bits(a,11,9)));
                source=(char *)&temp;
            }
            else
                source=(char *)&registers.D[bits(a,11,9)].L;

            dest=(char *)adjustsize(&registers.D[bits(a,2,0)].L,size);

            if (bits(a,8,8))	/* 1=>rotate left, 0=> rotate right */
            {
                switch (bits(a, 4, 3)) {		/* left shifts */
                case 0: 
                    do_asl(size,_M68(source),_M68(dest));
                    break;
                case 1: 
                    do_lsl(size,_M68(source),_M68(dest));
                    break;
                case 2:
                    do_illegal(i_dshift);	/* roxl */
                    break;
                case 3:
                    do_rol(size,_M68(source),_M68(dest));
                    break;
                }
            }
            else {
                switch (bits(a,4,3)) {		/* right shifts */
                case 0: 
                    do_asr(size,_M68(source),_M68(dest));
                    break;
                case 1: 
                    do_lsr(size,_M68(source),_M68(dest));
                    break;
                case 2:
                    do_illegal(i_dshift);	/* roxr */
                    break;
                case 3:
                    do_ror(size,_M68(source),_M68(dest));
                    break;
                }
            }
        }

        break;

    case i_mshift:
        {
            M68 temp;
            temp.L = 0x01000000; /* memory shifts by 1 bit only */
            
            /* already reversed */
            dest=get_dea(op,a);
            size=WORD;
            if (bits(a,8,8))	/* 1=>rotate left, 0=> rotate right */
            {
                switch (bits(a, 4, 3)) {		/* left shifts */
                case 0: 
                    do_asl(size,&temp,_M68(dest));
                    break;
                case 1: 
                    do_lsl(size,&temp,_M68(dest));
                    break;
                case 2:
                    do_illegal(i_mshift);	/* roxl */
                    break;
                case 3:
                    do_rol(size,&temp,_M68(dest));
                    break;
                }
            }
            else {
                switch (bits(a,4,3)) {		/* right shifts */
                case 0: 
                    do_asr(size,&temp,_M68(dest));
                    break;
                case 1: 
                    do_lsr(size,&temp,_M68(dest));
                    break;
                case 2:
                    do_illegal(i_mshift);	/* roxr */
                    break;
                case 3:
                    do_ror(size,&temp,_M68(dest));
                    break;
                }
            }
            /* have to think about setting flags for shifts */
        }
        break;


    case i_bcc:
        {
            int temp;
            char *oldpc;
            cond=bits(a,11,8);		/* 0=> BRA 1=> BSR */
            temp=byteext(bits(a,7,0));
            oldpc=PC;	/* cos READWORD fods PC to PC+2 */
            if (temp==0) temp=READWORD();
            if (cond==0)
                PC = oldpc+wordext(temp);	/* BRA */
            else
            {
                if (cond==1)
                {
                    push_ret_addr(PC);		/* this really is the PC after extension word has been taken into account */
                    PC = oldpc+wordext(temp);

#if	EXTRA_DEBUG
                    push_stack_trace(PC,reverse_l(registers.A[7].L));
#endif
                }
                else
                    if (istrue(cond)) PC = oldpc+wordext(temp);
            }
        }
        break;	
		
    case i_dbcc:
        {
            int temp;
            char *oldpc;
            oldpc=PC;	/* waaaa READWORD is a pain */
            temp=READWORD();
            if (istrue(bits(a,11,8))==0)	/* if exit condition is FALSE */
            {
                int regno;
                regno=bits(a,2,0);
                registers.D[regno].shorts.s2=reverse(reverse(registers.D[regno].shorts.s2)-1);
                if (registers.D[regno].shorts.s2!=0xFFFF) PC=oldpc+wordext(temp);
                /* if result isn't -1 */
            }
            break;


            case i_scc:
		dest=get_dea(op,a);
		if (istrue(bits(a,11,8))) *dest=0xFF;
		else *dest=0x00;
		break;

                case i_dbit:		/* Bxyz Dn,<ea> */
                    source=(char *)adjustsize(&registers.D[bits(a,11,9)].L,BYTE);
                    dest=get_dea(op,a);
                    switch (bits(a,7,6)) {
                    case 0:
				do_btst(source,dest);
                        break;
                    case 1:
				do_bchg(source,dest);
                        break;
                    case 2: 
				do_bclr(source,dest);
                        break;
                    case 3:
				do_bset(source,dest);
                        break;
                    }
                    break;

                    case i_sbit:		/* Bxyz #,<ea> */
                        {
                            static u_int temp;
                            temp= *(u_int *)(PC+1);
                            source= (char *)&temp;
                            PC=PC+2;
                            dest=get_dea(op,a);
                            switch (bits(a,7,6)) {
                            case 0:
				do_btst(source,dest);
				break;
                            case 1:
				do_bchg(source,dest);
				break;
                            case 2: 
				do_bclr(source,dest);
				break;
                            case 3:
				do_bset(source,dest);
				break;
                            }
                        }
                        break;

                        case i_chk:
                            do_illegal(i_chk);
                            break;

                            case i_clr:		/* CLR <ea> */
                                {
                                    M68 *dest;
                                    dest=(M68 *)get_dea(op,a);
                                    switch (opsize(op,a)) {
                                    case BYTE:
                                        dest_B=0x00;
                                        break;
                                    case WORD:
                                        dest_W=0x0000;
                                        break;
                                    case LONG:
                                        dest_L=0x00000000;
                                        break;
                                    }
                                }

                                /* clr always sets flags sameway */
                                clear_N;
                                set_Z;
                                clear_C;
        }
        break;

    case i_cmp:		/* CMP <ea>,Dn */
        size=opsize(op,a);
        source=get_sea(op,a);
        dest=adjustsize(&registers.D[bits(a,11,9)].L,size);
        do_cmp(size,_M68(source),_M68(dest));			/* sets flags itself!! */
        break;

    case i_cmpa:	/* CMP <ea>,An */
        size=opsize(op,a);
        source=get_sea(op,a);
        dest=adjustsize(&registers.A[bits(a,11,9)].L,size);
        do_cmp(size,_M68(source),_M68(dest));			/* sets flags itself!! */
        break;

    case i_cmpi:	/* CMP #,<ea> */
        source=PC;
        size=opsize(op,a);
        switch (size) {
        case BYTE:
            source=source+1;	/* falls into next case to frig PC */
        case WORD:
            PC=PC+2;
            break;
        case LONG:
            PC=PC+4;
            break;
        }
        dest=get_dea(op,a);
        do_cmp(size,_M68(source),_M68(dest));	/* flags set as appropriate */
        break;

    case i_cmpm:	/* CMPM (An)+,(Am)+ */
        {
            int reg1,reg2;
            reg1=bits(a,2,0);
            reg2=bits(a,11,9);
            size=opsize(op,a);
            source=vaxof68k(registers.A[reg1].L);
            registers.A[reg1].L=reverse_l(reverse_l(registers.A[reg1].L)+twotothe[size]);
            dest=vaxof68k(registers.A[reg2].L);
            registers.A[reg2].L=reverse_l(reverse_l(registers.A[reg2].L)+twotothe[size]);
            do_cmp(size,_M68(source),_M68(dest));
        }
        break;

    case i_divs:			/* DIVS <ea>,Dn */
        do_illegal(i_divs);
        break;

    case i_divu:			/* DIVU <ea>,Dn */
        do_illegal(i_divu);
        break;

    case i_eor:				/* EOR Dn,<ea> */
        size=opsize(op,a);
        source=(char *)&registers.D[bits(a,11,9)].L;
        dest=get_dea(op,a);
        do_eor(size,_M68(source),_M68(dest));
        break;

    case i_eori:			/* OR #,<ea> */
        source=PC;
        size=opsize(op,a);
        switch (size) {
        case BYTE:
            source=source+1;	/* falls into next case to frig PC */
        case WORD:
            PC=PC+2;
            break;
        case LONG:
            PC=PC+4;
            break;
        }
        dest=get_dea(op,a);
        do_eor(size,_M68(source),_M68(dest));
        set_flags(size,_M68(dest));
        break;

    case i_exgd:	/* EXG Dn,Dm if mode=8 */
        {
            int temp,reg1,reg2;
            reg1=bits(a,11,9);
            reg2=bits(a,2,0);
            temp=registers.D[reg1].L;
            registers.D[reg1].L=registers.D[reg2].L;
            registers.D[reg2].L=temp;
        }
        break;

    case i_exga:				/* EXG An,Am or EXG Dn,Am */
        {
            int temp,reg1,reg2;
            reg1=bits(a,11,9);
            reg2=bits(a,2,0);
            temp=registers.A[reg2].L;

            if (bits(a,7,7))			/* 1=> An,Dm */
            {
                registers.A[reg2].L=registers.D[reg1].L;
                registers.D[reg1].L=temp;
            }
            else					/* 0=> An,Am */
            {
                registers.A[reg2].L=registers.A[reg1].L;
                registers.A[reg1].L=temp;
            }
        }
        break;

    case i_extw:				/* EXT.W Dn (byte->word) */
        {
            int regno;
            regno=bits(a,2,0);
            registers.D[regno].shorts.s2=reverse(byteext(registers.D[regno].chars.c4));
            set_flags(WORD,_M68(&registers.D[regno].shorts.s2));
        }
        break;

    case i_extl:				/* EXT.L Dn (word->long) */
        {
            int regno;
            regno=bits(a,2,0);
            registers.D[regno].L=reverse_l(wordext(reverse(registers.D[regno].shorts.s2)));
            set_flags(LONG,&registers.D[regno]);
        }
        break;

    case i_jmp:		/* JMP <ea> */
        PC=get_sea(op,a);
        break;

    case i_jsr:		/* JSR <ea> */
        {
            char *temp;
            temp=get_sea(op,a);
            push_ret_addr(PC);
            PC=temp;

#if	EXTRA_DEBUG
            push_stack_trace(PC,reverse_l(registers.A[7].L));
#endif
        }
        break;

    case i_lea:		/* LEA <ea>,An */
        registers.A[bits(a,11,9)].L=vaxto68k(get_sea(op,a));
        break;

    case i_link:	/* LINK #,An */
        do_illegal(i_link);
        break;

    case i_move:	/* MOVE <ea>,<ea> */
        b = (bits(a,8,6)<<3) | bits(a,11,9) | (bits(a,15,12)<<12);
        size=opsize(op,a);
        source=get_sea(op, a);
        dest=get_dea(op, b);
        do_move(size,_M68(source),_M68(dest));
        set_flags(size,_M68(dest));
        clear_C;
        break;

    case i_movea:	/* MOVE <ea>,An */
        {
            int regno,temp;
            size=opsize(op,a);
            source=get_sea(op,a);
            regno=bits(a,11,9);
            if (size==WORD)
            {
                temp= *(u_int *)(source-2);
                if (temp&0x00800000) temp=temp|0x0000FFFF;
                else temp=temp&0xFFFF0000;
                registers.A[regno].L=temp;
            }
            else
		registers.A[regno].L= *(u_int *)source;
        }
        break;

    case i_mtsr:	/* MOVE <ea>,SR */	/* should not happen */
    case i_mtccr:	/* MOVE <ea>,CCR */
        source=get_sea(op,a)+1;			/* CCR is low word ALWAYS */
        if (*source & 0x01) set_C;
        else clear_C;
        if (*source & 0x04) set_Z;
        else clear_Z;
        if (*source & 0x08) set_N;
        else clear_N;

        break;

    case i_mfsr:	/* MOVE SR,<ea> */
        {
            char d=0;
            dest=get_dea(op,a);
            *dest++ = 0;				/* forget T S etc */
            if (flags.C==TRUE) d=d | 0x01;
            if (flags.Z==TRUE) d=d | 0x04;
            if (flags.N==TRUE) d=d | 0x08;
            *dest=d;
        }
        break;

    case i_mtusp:	/* MOVE An,USP */
        do_illegal(i_mtusp);
        break;

    case i_mfusp:	/* MOVE USP,An */
        do_illegal(i_mfusp);
        break;

    case i_PUSHM:
    case i_mmfreg:	/* MOVEM <reg list>,<ea> */
        {
            int regs,i,regno;
            regs=READWORD();
            if (bits(a,5,3)!=4) /* ie not predecrement */
            {
                dest=get_dea(op,a);
                if (a&0x40)		/* ie long words... */
                {
                    for (i=0;i<8;i++)
                        if (regs & (0x0001<<i))
                        {
                            *(u_int *)dest=registers.D[i].L;
                            dest=dest+4;
                        }	
                    for (i=0;i<8;i++)
                        if (regs & (0x0100<<i))
                        {
                            *(u_int *)dest=registers.A[i].L;
                            dest=dest+4;
                        }	
                }
                else	/* word wise... */
                {
                    for (i=0;i<8;i++)
                        if (regs & (0x0001<<i))
                        {
                            *(u_short *)dest=registers.D[i].shorts.s2;
                            dest=dest+2;
                        }	
                    for (i=0;i<8;i++)
                        if (regs & (0x0100<<i))
                        {
                            *(u_short *)dest=registers.A[i].shorts.s2;
                            dest=dest+2;
                        }	
                }
            }
            else		/*  ie predecrement - regs go other way! */
            {
                if (a&0x40)		/* ie long words... */
                {
                    regno=bits(a,2,0);
                    dest=vaxof68k(registers.A[regno].L);
                    for (i=0;i<8;i++)
                        if (regs & (0x0001<<i))
                        {
                            dest=dest-4;
                            *(u_int *)dest=registers.A[7-i].L;
                        }	
                    for (i=0;i<8;i++)
                        if (regs & (0x0100<<i))
                        {
                            dest=dest-4;
                            *(u_int *)dest=registers.D[7-i].L;
                        }	
                    registers.A[regno].L=vaxto68k(dest);
                }
                else	/* word wise... */
                {
                    dest=vaxof68k(registers.A[bits(a,2,0)].L);
                    for (i=0;i<8;i++)
                        if (regs & (0x0001<<i))
                        {
                            dest=dest-2;
                            *(u_short *)dest=registers.A[7-i].shorts.s2;
                        }	
                    for (i=0;i<8;i++)
                        if (regs & (0x0100<<i))
                        {
                            dest=dest-2;
                            *(u_short *)dest=registers.D[7-i].shorts.s2;
                        }	
                    registers.A[bits(a,2,0)].L=vaxto68k(dest);
                }
            }
        }
        break;

    case i_PULLM:	/* fake movem */
    case i_mmtreg:	/* MOVEM <ea>,<reg list) */

        {
            int regs,i,regno;
            regs=READWORD();
            if (bits(a,5,3)==3)		/* post increment */
            {
                regno=bits(a,2,0);
                dest=vaxof68k(registers.A[regno].L);
                if (a&0x40)		/* ie long words... */
                {
                    for (i=0;i<8;i++)
                        if (regs & (0x0001<<i))
                        {
                            registers.D[i].L = *(u_int *)dest;
                            dest=dest+4;
                        }	
                    for (i=0;i<8;i++)
                        if (regs & (0x0100<<i))
                        {
                            registers.A[i].L = *(u_int *)dest;
                            dest=dest+4;
                        }	
                }
                else	/* word wise... */
                {
                    for (i=0;i<8;i++)
                        if (regs & (0x0001<<i))
                        {
                            registers.D[i].shorts.s2 = *(u_short *)dest;
                            dest=dest+2;
                        }	
                    for (i=0;i<8;i++)
                        if (regs & (0x0100<<i))
                        {
                            registers.A[i].shorts.s2 = *(u_short *)dest;
                            dest=dest+2;
                        }	
                }
                registers.A[regno].L=vaxto68k(dest);
            }
            else
            {
                dest=get_sea(op,a);
                if (a&0x40)		/* ie long words... */
                {
                    for (i=0;i<8;i++)
                        if (regs & (0x0001<<i))
                        {
                            registers.D[i].L = *(u_int *)dest;
                            dest=dest+4;
                        }	
                    for (i=0;i<8;i++)
                        if (regs & (0x0100<<i))
                        {
                            registers.A[i].L = *(u_int *)dest;
                            dest=dest+4;
                        }	
                }
                else	/* word wise... */
                {
                    for (i=0;i<8;i++)
                        if (regs & (0x0001<<i))
                        {
                            registers.D[i].shorts.s2 = *(u_short *)dest;
                            dest=dest+2;
                        }	
                    for (i=0;i<8;i++)
                        if (regs & (0x0100<<i))
                        {
                            registers.A[i].shorts.s2 = *(u_short *)dest;
                            dest=dest+2;
                        }	
                }
            }
        }
        break;

    case i_movep:	/* MOVEP hmmm I don't even know the syntax... */
        do_illegal(i_movep);
        break;

    case i_moveq:	/* MOVEQ #<8bit>,Dn */
        {
            int temp,regno;
            regno=bits(a,11,9);
            clear_Z;
            clear_N;
            clear_C;
            temp=a&0xFF;
            if (temp==0)
            {
                set_Z;
                registers.D[regno].L=0;
                break;
            }
            if (temp>127)
            {
                set_N;
                registers.D[regno].L=0xFFFFFFFF;
                registers.D[regno].chars.c4=(unsigned char)temp;
                break;
            }
            registers.D[regno].L=0;
            registers.D[regno].chars.c4=(unsigned char)temp;
        }
        break;

    case i_muls:	/* MULS <ea>,Dn */
        do_illegal(i_muls);
        break;

    case i_mulu:	/* MULU <ea>,Dn */
        do_illegal(i_mulu);
        break;

    case i_nbcd:	/* NBCD <ea> */
        do_illegal(i_nbcd);
        break;

    case i_neg:		/* NEG <ea> */
        {
            M68 *dest;
            dest=(M68 *)get_dea(op,a);
            size=bits(a,7,6);
            switch(size) {
            case BYTE:
                dest_B= -dest_B;
                break;
            case WORD:
                dest_W=reverse(-reverse(dest_W));
                break;
            case LONG:
                dest_L=reverse_l(-reverse_l(dest_L));
                break;
            }
            set_flags(size,_M68(dest));
            flags.C=~flags.Z;		/* flags.V is going to be ignored today */
        }
        break;

    case i_negx:	/* NEGX <ea> */
        do_illegal(i_negx);
        break;

    case i_nop:		/* NOP */
        break;

    case i_not:		/* NOT <ea> */
        {
            M68 *dest;
            dest=(M68 *)get_dea(op,a);
            size=bits(a,7,6);
            switch(size) {
            case BYTE:
                dest_B= ~dest_B;
                break;
            case WORD:
                dest_W= ~dest_W;
                break;
            case LONG:
                dest_L= ~dest_L;
                break;
            }
            set_flags(size,_M68(dest));
            clear_C;
        }
        break;

    case i_or:
        size=opsize(op,a);
        if (bits(a,8,8) == 0) 	/* 0=> AND <ea>,Dn */
        {
            source=get_sea(op,a);
            dest=(char *)adjustsize(&registers.D[bits(a,11,9)].L,size);
            do_or(size,_M68(source),_M68(dest));
            set_flags(size,_M68(dest));
        }
        else					/* 1=> OR Dn,<ea> */
        {
            source=(char *)adjustsize(&registers.D[bits(a,11,9)].L,size);
            dest=get_dea(op,a);
            do_or(size,_M68(source),_M68(dest));
            set_flags(size,_M68(dest));
        }
        break;

    case i_ori:					/* OR #,<ea> */
        source=PC;
        size=opsize(op,a);
        switch (size) {
        case BYTE:
            source=source+1;	/* falls into next case to frig PC */
        case WORD:
            PC=PC+2;
            break;
        case LONG:
            PC=PC+4;
            break;
        }
        dest=get_dea(op,a);
        do_or(size,_M68(source),_M68(dest));
        set_flags(size,_M68(dest));
        break;

    case i_pea:
        push_ret_addr(get_sea(op,a));
        break;

    case i_reset:
        do_illegal(i_reset);
        break;

    case i_rte:
        do_illegal(i_rte);
        break;

    case i_rtr:
        do_illegal(i_rtr);
        break;

    case i_RET:
    case i_rts:
        PC=pull_ret_addr();

#if EXTRA_DEBUG
        pop_stack_trace(reverse_l(registers.A[7].L));
#endif

        break;

    case i_sbcd:
        do_illegal(i_sbcd);
        break;

    case i_stop:
        do_illegal(i_stop);
        break;

    case i_sub:
        size=opsize(op,a);
        if (bits(a,8,8) == 0) 	/* 0=> SUB <ea>,Dn */
        {
            source=get_sea(op,a);
            dest= adjustsize(&registers.D[bits(a, 11, 9)].L,size);
            do_sub(size,_M68(source),_M68(dest));
        }
        else					/* 1=> SUB Dn,<ea> */
        {
            source= adjustsize(&registers.D[bits(a,11,9)].L,size);
            dest=get_dea(op,a);
            do_sub(size,_M68(source),_M68(dest));
        }
        break;

    case i_suba:				/* always SUBA <ea>,An */
        size=opsize(op,a);
        source=get_sea(op,a);
        dest=adjustsize(&registers.A[bits(a,11,9)].L,size);
        do_suba(size,_M68(source),_M68(dest));
        break;

    case i_subi:				/* SUB #,<ea> */
        source=PC;
        size=opsize(op,a);
        switch (size) {
        case BYTE:
            source=source+1;
        case WORD:
            PC=PC+2;
            break;
        case LONG:
            PC=PC+4;
            break;
        }
        dest=get_dea(op,a);
        do_sub(size,_M68(source),_M68(dest));
        break;
		
    case i_subq:				/* SUB #3bits,<ea> */
        {
            static u_int temp;
            size=opsize(op,a);
            temp=reverse_l(shortval(bits(a,11,9)));
            source=adjustsize(&temp,size);
            dest=get_dea(op,a);
            do_sub(size,_M68(source),_M68(dest));
        }
        break;

    case i_subx:
        do_illegal(i_subx);
        break;

    case i_swap:		/* SWAP Dn */
        {
            int temp,regno;
            regno=bits(a,2,0);
            temp=registers.D[regno].shorts.s1;
            registers.D[regno].shorts.s1=registers.D[regno].shorts.s2;
            registers.D[regno].shorts.s2=temp;
        }
        break;

    case i_tas:
        do_illegal(i_tas);
        break;

    case i_trap:
        do_illegal(i_trap);
        break;

    case i_trapv:
        do_illegal(i_trapv);
        break;

    case i_TEST:
    case i_tst:			/* TST <ea> */
        size=opsize(op,a);
        set_flags(size,_M68(get_dea(op,a)));
        clear_C;
        break;

    case i_unlk:
        do_illegal(i_unlk);
        break;

    case i_SEARCH:
        do_SEARCH();
        break;
    case i_GETDATA:
        do_GETDATA(); 
        break;
    case i_SETNOUNS: 
        do_SETNOUNS(); 
        break;
    case i_NOUNSCAN: 
        do_NOUNSCAN(); 
        break;
    case i_WORDSCAN: 
        do_WORDSCAN(); 
        break;
    case i_GETNOUNS: 
        do_GETNOUNS(); 
        break;
    case i_QCARRIED: 
        do_QCARRIED(); 
        break;
    case i_TEXT: 
        do_TEXT(); 
        break;
    case i_RANDOM1: 
        do_RANDOM1(); 
        break;
    case i_RANDOM2:
        do_RANDOM2(); 
        break;
    case i_LOADFILE:
        do_LOADFILE();
        break;
    case i_SAVEFILE:
        do_SAVEFILE(); 
        break;
    case i_CHAROUT:
        do_CHAROUT(); 
        break;
    case i_CHAROUT2:
        do_CHAROUT2(); 
        break;
    case i_CHARIN:
        do_CHARIN();
        break;
    case i_GETTIED:
        do_GETTIED();
        break;
    case i_FINDZERO:
        do_FINDZERO();
        break;
    case i_GRAPHICS:
        do_GRAPHICS();
        break;
    case i_PRINTER:
        do_PRINTER();
        break;
    case i_RESTART:
        do_RESTART();
        break;
    case i_QUIT:
        do_QUIT();
        break;
    case i_MAKE_DIFF:
        do_MAKE_DIFF();
        break;
    case i_UNDO_DIFF:
        do_UNDO_DIFF();

    case i_CALL:				/* fake BSR */
        {
            int temp;
            char *oldpc;
            temp=byteext(bits(a,7,0));
            oldpc=PC;						/* cos READWORD fods PC to PC+2 */
            if (temp==0) temp=READWORD();
            push_ret_addr(PC);		/* this really is the PC after extension word has been taken into account */
            PC = oldpc+wordext(temp);

#if	EXTRA_DEBUG
            push_stack_trace(PC,reverse_l(registers.A[7].L));
#endif
        }
        break;	

    case i_eoriCCR:
        {
            int arg,C,N,Z;
            arg=READWORD();
            C = arg & 0x01;
            N = arg & 0x08;
            Z = arg & 0x04;
            if ((C) && (flags.C))
                clear_C;
            else
                if ((!C) && (!flags.C))
                    clear_C;
                else
                    set_C;
            if ((Z) && (flags.Z))
                clear_Z;
            else
                if ((!Z) && (!flags.Z))
                    clear_Z;
                else
                    set_Z;
            if ((N) && (flags.N))
                clear_N;
            else
                if ((!N) && (!flags.N))
                    clear_N;
                else
                    set_N;
        }
        break;

    case i_andiCCR:
        {
            int arg;
            arg=READWORD();
            if ((arg & 0x01) && (flags.C)) set_C;
            else clear_C;
            if ((arg & 0x08) && (flags.N)) set_N;
            else clear_N;
            if ((arg & 0x04) && (flags.Z)) set_Z;
            else clear_Z;
        }
        break;
	
    case i_oriCCR:
        {
            int arg;
            arg=READWORD();
            if ((arg & 0x01) || (flags.C)) set_C;
            if ((arg & 0x08) || (flags.N)) set_N;
            if ((arg & 0x04) || (flags.Z)) set_Z;
        }
        break;

    default:
        do_illegal(op);
        break;
    }
}


