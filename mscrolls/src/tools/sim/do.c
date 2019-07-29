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

#include <setjmp.h>
#include <stdio.h>
#include "kem.h"
#include "dis.h"
#include "do.h"
#include "get_ea.h"
#include "display.h"

extern struct registers registers;
extern struct flags flags;
extern char *memory;
extern int pc;

int istrue(int num)
{
    switch(num)
    {
    case 0 : return TRUE;
    case 1 : return FALSE;
    case 2 : return !flags.C && !flags.Z;
    case 3 : return flags.C || flags.Z;
    case 4 : return !flags.C;
    case 5 : return flags.C;
    case 6 : return !flags.Z;
    case 7 : return flags.Z;
    case 8 : return !flags.V;
    case 9 : return flags.V;
    case 10 : return !flags.N;
    case 11 : return flags.N;
    case 12 : return (flags.N && flags.V) || (!flags.N && !flags.V);
    case 13 : return (flags.N && !flags.V) || (!flags.N && flags.V);
    case 14 : return (flags.N && flags.V && !flags.Z) ||
            (!flags.N && !flags.V && !flags.Z);
    case 15 : return flags.Z || (flags.N && !flags.V) ||
            (!flags.N && flags.V);
    }
    return FALSE;
}

void set_flags(int size, M68* dest)
{
    clear_N;
    switch (size) {
    case BYTE:
        if (dest_B==0) {
            set_Z;
            break;
        }
        if ((dest_B)&(0x80)) set_N;
        clear_Z;
        break;
    case WORD:
        if (dest_W==0) {
            set_Z;
            break;
        }
        if ((dest_W)&(0x0080)) set_N;
        clear_Z;
        break;
    case LONG:
        if (dest_L==0) {
            set_Z;
            break;
        }
        if ((dest_L)&(0x00000080)) set_N;
        clear_Z;
        break;
    }
}

void do_btst(char* source, char* dest)
{
    int bitno,temp;
    set_Z;
    bitno=(int)(*source);	/* get bit no to fod */
    if (bitno>7)
    {
        dest=dest-(bitno/8);
        bitno=bitno & 0x07;
    }
    temp= 0x01<<bitno;
    if (((int)(*dest))&temp) clear_Z;
}


void do_bchg(char* source, char* dest)
{
    int bitno,temp;
    set_Z;
    bitno=(int)(*source);	/* get bit no to fod */
    if (bitno>7)
    {
        dest=dest-(bitno/8);
        bitno=bitno & 0x07;
    }
    temp= 0x01<<bitno;
    if (((int)(*dest))&temp) clear_Z;
    *dest= (*dest)^((char)temp);
}

void do_bclr(char* source, char* dest)
{
    int bitno,temp;
    set_Z;
    bitno=(int)(*source);	/* get bit no to fod */
    if (bitno>7)
    {
        dest=dest-(bitno/8);
        bitno=bitno & 0x07;
    }
    temp= 0x01<<bitno;
    if (((int)(*dest))&temp) clear_Z;
    *dest= (*dest)&(~((char)temp));
}

void do_bset(char* source, char* dest)
{
    int bitno,temp;
    set_Z;
    bitno=(int)(*source);	/* get bit no to fod */
    if (bitno>7)
    {
        dest=dest-(bitno/8);
        bitno=bitno & 0x07;
    }
    temp= 0x01<<bitno;
    if (((int)(*dest))&temp) clear_Z;
    *dest= (*dest)|((char)temp);
}

void do_and(int size, M68* source, M68* dest)
{
    switch (size)
    {
    case BYTE:
        dest_B=(dest_B)&(source_B);
        break;
    case WORD:
        dest_W=(dest_W)&(source_W);
        break;
    case LONG:
        dest_L=(dest_L)&(source_L);
        break;
    }
    set_flags(size,dest);
    clear_V;
    clear_C;
}

void do_eor(int size, M68* source, M68* dest)
{
    switch (size)
    {
    case BYTE:
        dest_B=(dest_B)^(source_B);
        break;
    case WORD:
        dest_W=(dest_W)^(source_W);
        break;
    case LONG:
        dest_L=(dest_L)^(source_L);
        break;
    }
    set_flags(size,dest);
    clear_C;
}

void do_or(int size, M68* source, M68* dest)
{
    switch (size)
    {
    case BYTE:
        dest_B=(dest_B)|(source_B);
        break;
    case WORD:
        dest_W=(dest_W)|(source_W);
        break;
    case LONG:
        dest_L=(dest_L)|(source_L);
        break;
    }
    set_flags(size,dest);
    /* clear_V; */
    clear_C;
}

void do_add(int size, M68* source, M68* dest)
{
    set_C;
    switch (size)
    {
    case BYTE:
        dest_B=(dest_B)+(source_B);
        if (source_B<dest_B) clear_C;
        break;
    case WORD:
        {
            u_short s,d;
            s=reverse(source_W);
            d=reverse(dest_W);
            d=s+d;
            if (s<d) clear_C;
            dest_W=reverse(d);
        }
        break;
    case LONG:
        {
            u_int s,d;
            s=reverse_l(source_L);
            d=reverse_l(dest_L);
            d=s+d;
            if (s<d) clear_C;
            dest_L=reverse_l(d);
        }
        break;
    }
    set_flags(size,dest);
}

void do_sub(int size, M68* source, M68* dest)
{
    set_C;
    switch (size)
    {
    case BYTE:
        if (source_B<dest_B) clear_C;
        dest_B=dest_B-source_B;
        break;
    case WORD:
        {
            u_short s,d;
            s=reverse(source_W);
            d=reverse(dest_W);
            if (s<d) clear_C;
            d=d-s;
            dest_W=reverse(d);
        }
        break;
    case LONG:
        {
            u_int s,d;
            s=reverse_l(source_L);
            d=reverse_l(dest_L);
            if (s<d) clear_C;
            d=d-s;
            dest_L=reverse_l(d);
        }
        break;
    }
    set_flags(size,dest);
}

void do_suba(int size, M68* source, M68* dest)
{
    if (size==WORD)
        dest_W=reverse(reverse(dest_W)-reverse(source_W));
    else
        dest_L=reverse_l(reverse_l(dest_L)-reverse_l(source_L));
}

void do_adda(int size, M68* source, M68* dest)
{
    if (size==WORD)
        dest_W=reverse(reverse(dest_W)+reverse(source_W));
    else
        dest_L=reverse_l(reverse_l(dest_L)+reverse_l(source_L));
}


/* CMP is going to be really slow.... */

void do_cmp(int size, M68* source, M68* dest)
{
    M68 temp;
    clear_C;
    switch (size)
    {
    case BYTE:
        if (source_B>dest_B) set_C;
        temp.chars.c1=dest_B-source_B;
        break;
    case WORD:
        {
            u_short s,d;
            s=reverse(source_W);
            d=reverse(dest_W);
            if (s>d) set_C;
            d=d-s;
            temp.shorts.s1=reverse(d);
        }
        break;
    case LONG:
        {
            u_int s,d;
            s=reverse_l(source_L);
            d=reverse_l(dest_L);
            if (s>d) set_C;
            d=d-s;
            temp.L=reverse_l(d);
        }
        break;
    }
    set_flags(size,&temp);
}

void do_move(int size, M68* source, M68* dest)
{
    switch (size)
    {
    case BYTE:
        dest_B=source_B;
        break;
    case WORD:
        dest_W=source_W;
        break;
    case LONG:
        dest_L=source_L;
        break;
    }
}

void do_lsl(int size, M68* source, M68* dest)
{
    int times;
    clear_C;
    times=reverse_l(source_L)&0x1F;
    if ((dest_B) & (0x0100 >> times)) set_C;	
    switch (size) {
    case BYTE:
        dest_B=(dest_B)<<times;
        break;
    case WORD:
        dest_W=reverse((reverse(dest_W)) << times);
        break;
    case LONG:
        dest_L=reverse_l((reverse_l(dest_L)) << times);
        break;
    }
    set_flags(size,dest);	
}

void do_asl(int size, M68* source, M68* dest)
{
    int times;
    clear_C;
    times=reverse_l(source_L)&0x1F;
    if ((dest_B) & (0x0100 >> times)) set_C;	
    switch (size) {
    case BYTE:
        dest_B=(dest_B)<<times;
        break;
    case WORD:
        dest_W=reverse((reverse(dest_W)) << times);
        break;
    case LONG:
        dest_L=reverse_l((reverse_l(dest_L)) << times);
        break;
    }
    set_flags(size,dest);	
}

void do_lsr(int size, M68* source, M68* dest)
{
    int times;
    u_int temp;
    clear_C;
    times=reverse_l(source_L)&0x1F;
    switch (size)
    {
    case BYTE:
        if (dest_B & ( (0x01) << (times-1) ) ) set_C;	
        dest_B = dest_B >> times;
        break;
    case WORD:
        temp=reverse(dest_W);
        if (temp & ((0x01) << (times-1))) set_C;	
        dest_W=reverse(temp >>times);
        break;
    case LONG:
        temp=reverse_l(dest_L);
        if (temp & ((0x01) << (times-1))) set_C;	
        dest_L=reverse_l(temp >>times);
        break;
    }
    set_flags(size,dest);	
}

void do_asr(int size, M68* source, M68* dest)
{
    int times,temp;
    clear_C;
    times=reverse_l(source_L)&0x1F;
    switch (size)
    {
    case BYTE:
        if (dest_B & ( (0x01) << (times-1) ) ) set_C;	
        dest_B = dest_B >> times;
        break;
    case WORD:
        temp=reverse(dest_W);
        if (temp & ((0x01) << (times-1))) set_C;	
        dest_W=reverse(temp >>times);
        break;
    case LONG:
        temp=reverse_l(dest_L);
        if (temp & ((0x01) << (times-1))) set_C;	
        dest_L=reverse_l(temp >>times);
        break;
    }
    set_flags(size,dest);	
}

void do_rol(int size, M68* source, M68* dest)
{
    int times;
    clear_C;
    times=reverse_l(source_L)&0x1F;
    switch (size)
    {
    case BYTE:
        dest_B=(dest_B << times) | (dest_B >> (8-times));
        if (dest_B & 0x01) set_C;
        break;
    case WORD:
        {
            u_short temp;
            temp=reverse(dest_W);
            temp=(temp<<times)|(temp>>(16-times));
            dest_W=reverse(temp);
            if (temp&0x01) set_C;
        }
        break;
    case LONG:
        {
            u_int temp;
            temp=reverse_l(dest_L);
            temp=(temp<<times)|(temp>>(32-times));
            dest_L=reverse_l(temp);
            if (temp&0x01) set_C;
        }
        break;
    }
    set_flags(size,dest);
}
	

void do_ror(int size, M68* source, M68* dest)
{
    int times;
    clear_C;
    times=reverse_l(source_L)&0x1F;
    switch (size) {
    case BYTE:
        dest_B=((dest_B) >>times)|(dest_B<<(8-times));
        break;
    case WORD:
        {
            u_short temp;
            temp=reverse(dest_W);
            temp=(temp>>times)|(temp<<(16-times));
            dest_W=reverse(temp);
        }
        break;
    case LONG:
        {
            u_int temp;
            temp=reverse_l(dest_L);
            temp=(temp>>times)|(temp<<(32-times));
            dest_L=reverse_l(temp);
        }
        break;
    }
    if (dest_B&0x80) set_C;
    set_flags(size,dest);
}
	

extern jmp_buf env;

void do_illegal(int op)
{
    printf("\nBad instruction (%d) at PC = %08X\n",op, pc);
    displayregs();
    longjmp(env,0);
}
