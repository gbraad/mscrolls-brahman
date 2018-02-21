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
 *
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include <stdio.h>
#include "symbols.h"
#include "memory.h"
#include "patch.h"
#include "errors.h"

extern BYTE *fileptr;   /* points to current byte of current file */
extern BYTE *fileend;   /* points to 1+ last char in current file */
extern int patch;       /* !=0 if range error patching enabled */

#define getch()         ((fileptr==fileend)?EOF:*fileptr++)

/* space to store code and data */
BYTE Code[MAXCODE];
BYTE Data[MAXDATA];

int code_load_base=0;                   /* address to add to ALL code refs */
int code_address_base=0;                /* address to add to code base */
int data_address_base=0;                /* address to add to data base */
int bss_address_base=0;                 /* address to add to bss base */

void loadcode(int amount)
{
#ifdef MS_BIG_ENDIAN
    BYTE *p = &Code[code_address_base - code_load_base];
#else
    BYTE *p = &Code[MAXCODE - (code_address_base-code_load_base) - 1];
#endif
    
    code_address_base += amount;
    
#ifdef MS_BIG_ENDIAN
    bcopy(fileptr, p, amount);
    fileptr += amount;
    if (patch)
    {
        *p++ = 0x60;
        *p++ = 0x00;
        *p++ = 0x00;
        *p++ = 0x00;
        add_tunnel(code_address_base);
        code_address_base += 4; /* space for the tunnel */
    }
#else
    for (; amount; amount--) *p-- = getch();
    if (patch)
    {
        *p-- = 0x60;
        *p-- = 0x00;
        *p-- = 0x00;
        *p-- = 0x00;
        add_tunnel(code_address_base);
        code_address_base += 4; /* space for the patch */
    }
#endif
}

void loaddata(int amount)
{
#ifdef MS_BIG_ENDIAN
    BYTE *p = &Data[data_address_base];
    data_address_base += amount;
    bcopy(fileptr, p, amount);
    fileptr += amount;
#else
    BYTE *p = &Data[MAXDATA - data_address_base - 1];

    data_address_base += amount;
    for (; amount; amount--)
        *p-- = getch();
#endif
}

void addtolong(int type, int ad, int value)
{
    LONG *lp = 0;

#ifdef MS_BIG_ENDIAN
    if (type == N_TEXT)
        lp = (LONG *)(Code + (ad - code_load_base));
    else
        lp = (LONG *)(Data + ad);
#else
    if (type == N_TEXT)
        lp = (LONG *)(Code + MAXCODE-4 - (ad-code_load_base));
    else if (type == N_DATA)
        lp = (LONG *)(Data + MAXDATA-4 - ad);
#endif
    
    if (lp) *lp += value;
}

int addtoword(char* symname, int type, int ad, int value)
{
    WORD *wp = 0;

#ifdef MS_BIG_ENDIAN
    if (type == N_TEXT)
        wp = (WORD *)(Code + (ad - code_load_base));
    else
        wp = (WORD *)(Data + ad);
#else
    if (type == N_TEXT)
        wp = (WORD *)(Code + MAXCODE-2 - (ad-code_load_base));
    else if (type == N_DATA)
        wp = (WORD *)(Data + MAXDATA-2 - ad);
#endif

    if (wp)
    {
        int result;
        result = *wp + value;
        if (result > 32767 || result < -32768)
        {
            rangeerror(symname, type, ad, result);
            return 0;
        }
        *wp = result;
    }
    return 1;
}

void addtobyte(char* symname, int type, int ad, int value)
{
    BYTE *bp = 0;

#ifdef MS_BIG_ENDIAN
    if (type == N_TEXT)
        bp = Code + (ad - code_load_base);
    else
        bp = Data + ad;
#else
    if (type == N_TEXT)
        bp = Code + MAXCODE-1 - (ad-code_load_base);
    else if (type == N_DATA)
        bp = Data + MAXDATA-1 - ad;
#endif

    if (bp)
    {
        int result = *bp + value;
        if (result > 127 || result < -128)
            rangeerror(symname, type, ad, result);
        *bp = result;
    }
}
