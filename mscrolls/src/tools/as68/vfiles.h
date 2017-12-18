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

#pragma once

#define VBLOCKSIZE	512

struct vfile {
	char *name, *mode;	/* params to vopen */
	struct vfileblock *firstblock;
	struct vfileblock *filepointer;	/* current read block */
	char *nextchar;		/* char in current read block */
	char *free;		/* copy of filepointer->free */
};

struct vfileblock {
	int nfree;		/* space left in block */
	char *block;		/* ptr to block start */
	char *free;		/* next free char in block */
	struct vfileblock *next; /* next block in file */
};

#define VFILE	struct vfile

VFILE* vopen(char* name, char* mode);
void vclose(VFILE* vp);
void Vprintf(VFILE* vp, char* s, ...);
void vflsbuf(char c, VFILE* vp);
int vfilbuf(VFILE* vp);
void vrewind(VFILE* vp);

#define vgetc(vp) (((vp)->nextchar==(vp)->free)?vfilbuf(vp):*(vp)->nextchar++)
#define vputc(c,vp) (((vp)->nextchar<(vp)->free)?(*(vp->nextchar)++) =(c) : vflsbuf((c),(vp)))
