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

#pragma once

#include "types.h"

#define BYTE 0
#define WORD 1
#define LONG 2

#define AREG 0
#define DREG 1

#define MEMSIZE 100000

typedef int (*intfnp)();
typedef int intfn();

struct flags
{
    u_int
    C : 1,
        V : 1,
        Z : 1,
        N : 1,
        X : 1;
};

#define set_Z flags.Z=1
#define clear_Z flags.Z=0
#define set_N flags.N=1
#define clear_N flags.N=0
#define set_C flags.C=1
#define clear_C flags.C=0
#define set_V flags.V=1
#define clear_V flags.V=0
#define set_X flags.X=1
#define clear_X flags.X=0

struct where
{
    char type;
    int num;
};

struct chars
{
    u_char c1,c2,c3,c4;
};

union useful
{
    struct chars chars;
    struct
    {
        u_short s1,s2;
    } shorts;
    u_int L;
};

union signed_useful
{
    struct chars chars;
    struct
    {
        short s1,s2;
    } shorts;
    int L;
};

typedef union useful M68;
typedef union signed_useful s_M68;

#define dest_B dest->chars.c1
#define dest_W dest->shorts.s1
#define dest_L dest->L

#define source_B source->chars.c1
#define source_W source->shorts.s1
#define source_L source->L

struct registers
{
    M68 D[8], A[8];
    u_short SR,CCR;
};

#define bits(a,b,c) (((a)>>(c)) & ((1 << ((b)-(c)+1))-1))

#define EXTRA_DEBUG	1	/* do stack trace */

struct stack_trace
{
    int pc,sp;
};


extern u_int twotothe[];

void emulate(int op, int a);
