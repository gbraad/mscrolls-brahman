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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "vfiles.h"

#define alloc(t)	((t *)malloc(sizeof(t)))
#define allocstr(s)	((char *)malloc(strlen(s)+1))

static struct vfileblock* blockalloc()
{
    struct vfileblock *bp = alloc(struct vfileblock);

    bp->nfree = VBLOCKSIZE;
    bp->free = bp->block = malloc(VBLOCKSIZE);
    bp->next = NULL;
    return bp;
}

VFILE* vopen(char* name, char* mode)
{
    VFILE *vp;

    vp = alloc(VFILE);
    vp->name = allocstr(name);
    strcpy(vp->name, name);
    vp->mode = allocstr(mode);
    strcpy(vp->mode, mode);
    vp->firstblock = vp->lastblock = blockalloc();
    return vp;
}

void vclose(VFILE* vp)
{
    FILE *fp;
    struct vfileblock *bp;
    char *p;

    if ((fp = fopen(vp->name, vp->mode)) == NULL) {
        perror(vp->name);
        exit(-1);
    }
    
    for (bp = vp->firstblock; bp; bp = bp->next)
        for (p = bp->block; p < bp->free; p++)
            putc(*p, fp);
    
    fclose(fp);
}

void vputc(char c, VFILE* vp)
{
    if (!vp->lastblock->nfree) {
        vp->lastblock->next = blockalloc();
        vp->lastblock = vp->lastblock->next;
    }

    vp->lastblock->nfree--;
    *vp->lastblock->free++ = c;
}

void Vprintf(VFILE* vp, char* s, ...)
{
    char *p;
    char temp[500];

    va_list args;
    va_start(args, s);
    
    vsprintf(temp, s, args);
    va_end(args);
    
    for (p=temp; *p; p++) vputc(*p, vp);
}

