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

#include <stdio.h>
#include "dis1.h"
#include "get_ea.h"
#include "kem.h"


extern char *PC;        /* vax pointer to 68k code */
extern char *pc;


extern struct registers registers;
extern struct flags flags;
extern char *memory;

int READWORD()
{
    u_short word;
    char temp[3];
    temp[1]= *PC;
    temp[0]= *(PC+1);
    word = *(u_short *)temp;
    PC=PC+2;
    return word;
}

/* map vax address space into 68k address space */
u_int vaxto68k(char* number)
{
    u_int v = number - memory;
    return reverse_l(v);
}

/* map 68k address space into vax address space */
char* vaxof68k(u_int number)
{
    return memory + reverse_l(number);
}

void push_ret_addr(char* vaxaddr)
{
    char *temp;
    temp=vaxof68k(registers.A[7].L)-4;
	registers.A[7].L=vaxto68k(temp);
    *(u_int *)temp=vaxto68k(vaxaddr);
}

char *pull_ret_addr()
{
    char *temp;
    temp=vaxof68k(registers.A[7].L);
    registers.A[7].L=vaxto68k(temp+4);
    return vaxof68k(*(u_int *)(temp));
}


/* get source effective address - gets pointer into VAX address space
   if the source is an ADDRESS reg it moves it into a static var. so that

   MOVE.L A0,(A0)+

   etc, won't screw it by changing the register when it calcs the
   destination ea
*/

char* get_sea(int op, int a) 
{
    int mode, arg, size, b, d;
    char *temp;
    static M68 old_address_reg;
    
    mode = bits(a,5,3);
    arg = bits(a,2,0);
    switch (mode)
    {
    case 0:     /* Dn */
        size=opsize(op,a);
        return adjustsize((char *)&registers.D[arg],size); /* data reg. direct */
    case 1:     /* An */
        size=opsize(op,a);
        old_address_reg=registers.A[arg];
        return adjustsize((char *)&old_address_reg,size); /* address reg direct */
    case 2:     /* (An) */
        return vaxof68k(registers.A[arg].L);  /* address register indirect */
    case 3:
        size=opsize(op,a);
        temp=vaxof68k(registers.A[arg].L); /* (An)+ */
        registers.A[arg].L=vaxto68k(temp+twotothe[size]);
        return temp;
    case 4:
        size=opsize(op,a);
        temp=vaxof68k(registers.A[arg].L)-twotothe[size]; /* -(An) */
        registers.A[arg].L=vaxto68k(temp);
        return temp;
    case 5:     /* 16bit(An) */
        return wordext(READWORD())+vaxof68k(registers.A[arg].L);
    case 6:     /* 8bit(An,... */
        {
            int secondreg;

            b = READWORD();
            d = b & 0XFF;
            if (d > 127) d = byteext(d);
            temp=vaxof68k(registers.A[arg].L)+d;  /* get first reg */

            /* now get second register, sussing if it's An or Dn */
            if (bits(b, 15, 15) == 1)   /* 1=> second reg is An */
                secondreg=reverse_l(registers.A[bits(b,14,12)].L);
            else
                secondreg=reverse_l(registers.D[bits(b,14,12)].L);

            /* ahh, but is it a word or long word ? bits(b,11,11)=1 => long */      
            if (bits(b, 11, 11) == 0) secondreg=wordext(secondreg&0xFFFF);
            return temp+secondreg;  
        }

    case 7: switch (arg)
        {
        case 0:     /* 16 bit absolute */
            printf("\n\nWarning: 16 bit absolute source <ea> at $%08x!!!!\n\n",PC-memory);
            return  (wordext(READWORD()&0xFFFF))+memory;
        case 1:     /* 32 bit absolute */
            {
                int hiword,loword;
                printf("\n\nWarning: 32 bit absolute source <ea> at $%08x!!!!\n\n",PC-memory);
                hiword=READWORD()<<16;
                loword=READWORD()&0xFFFF;
                return (hiword | loword)+memory;
            }
        case 2:     /* 16 bit PC relative */
            {
                register char *temp=PC;
                return temp + wordext(READWORD()); /* careful READWORD adds to PC */
            }
        case 3:     /* 8 bit PC relative with index */
            {
                int secondreg;

                b = READWORD();
                d = b & 0XFF;
                if (d > 127) d = byteext(d);
                temp=PC+d-2;    /* cos READWORD adds 2 to PC */

                /* now get second register, sussing if it's An or Dn */
                if (bits(b, 15, 15) == 1)   /* 1=> second reg is An */
                    secondreg=reverse_l(registers.A[bits(b,14,12)].L);
                else
                    secondreg=reverse_l(registers.D[bits(b,14,12)].L);

                /* ahh, but is it a word or long word ? bits(b,11,11)=1 => long */      
                if (bits(b, 11, 11) == 0) secondreg=wordext(secondreg&0xFFFF);
                return temp+secondreg;  
            }
        case 4:     /* immediate (byte/word/long) NOT quick */
            {
                size = opsize(op, a);
                if (size == BYTE) {
                    PC=PC+2;        /* maintain PC */
                    return PC-1;
                }
                if (size == WORD) {
                    PC=PC+2;
                    return PC-2;
                }
                if (size == 2) {
                    PC=PC+4;
                    return PC-4;
                }
            }
        }
    default:    /* oh really */
        break;
    }
    return 0;
}

/* get destination effective address - NB returns pointer into VAX address
   space, not 68k */

char *get_dea(int op, int a)
{
    int mode, arg, size, b, d;
    char *temp;

    mode = bits(a, 5, 3);
    arg = bits(a, 2, 0);
    switch (mode)
    {
    case 0:
        size=opsize(op,a);
        return adjustsize(&registers.D[arg].L,size); /* data reg. direct */
    case 1:
        size=opsize(op,a);
        return adjustsize(&registers.A[arg].L,size); /* address reg direct */
    case 2:
        return vaxof68k(registers.A[arg].L);  /* address register indirect */
    case 3:
        size=opsize(op,a);
        temp=vaxof68k(registers.A[arg].L); /* (An)+ */
        registers.A[arg].L=vaxto68k(temp+twotothe[size]);
        return temp;
    case 4:
        size=opsize(op,a);
        temp=vaxof68k(registers.A[arg].L)-twotothe[size]; /* -(An) */
        registers.A[arg].L=vaxto68k(temp);
        return temp;
    case 5:     /* 16bit(An) */
        return wordext(READWORD())+vaxof68k(registers.A[arg].L);
    case 6:     /* 8bit(An,... */
        {
            int secondreg;

            b = READWORD();
            d = b & 0XFF;
            if (d > 127) d = byteext(d);
            temp=vaxof68k(registers.A[arg].L)+d;  /* get first reg */

            /* now get second register, sussing if it's An or Dn */
            if (bits(b, 15, 15) == 1)   /* 1=> second reg is An */
                secondreg=reverse_l(registers.A[bits(b,14,12)].L);
            else
                secondreg=reverse_l(registers.D[bits(b,14,12)].L);

            /* ahh, but is it a word or long word ? bits(b,11,11)=1 => long */      
            if (bits(b, 11, 11) == 0) secondreg=wordext(secondreg&0xFFFF);
            return temp+secondreg;  
        }

    case 7: switch (arg) {
        case 0:     /* 16 bit absolute */
            printf("\n\nWarning: 16 bit  absolute dest. <ea> at $%8x!!!!\n\n",PC-memory);
            return memory+wordext(READWORD()&0xFFFF);
        case 1:     /* 32 bit absolute */
            {
                register int hiword,loword;
                printf("\n\nWarning: 32 bit absolute dest. <ea> at $%8x!!!!\n\n",PC-memory);
                hiword=READWORD()<<16;
                loword=READWORD()&0xFFFF;
                return (hiword | loword)+memory;
            }
        case 2:     /* 16 bit PC relative */
            {
                register char *temp=PC;
                return temp + wordext(READWORD()); /* careful READWORD adds to PC */
            }
        case 3:     /* 8 bit PC relative with index */
            {
                int secondreg;

                b = READWORD();
                d = b & 0XFF;
                if (d > 127) d = byteext(d);
                temp=PC+d-2; /* cos READWORD adds 2 to PC */

                /* now get second register, sussing if it's An or Dn */
                if (bits(b, 15, 15) == 1)   /* 1=> second reg is An */
                    secondreg=reverse_l(registers.A[bits(b,14,12)].L);
                else
                    secondreg=reverse_l(registers.D[bits(b,14,12)].L);

                /* ahh, but is it a word or long word ? bits(b,11,11)=1 => long */      
                if (bits(b, 11, 11) == 0) secondreg=wordext(secondreg&0xFFFF);
                return temp+secondreg;  
            }
        case 4:     /* immediate (byte/word/long) NOT quick */
            {
                size = opsize(op, a);
                if (size == BYTE) {
                    PC=PC+2;        /* maintain PC */
                    return PC-1;
                }
                if (size == WORD) {
                    PC=PC+2;
                    return PC-2;
                }
                if (size == 2) {
                    PC=PC+4;
                    return PC-4;
                }
            }
        }
    }
    return 0;
}
