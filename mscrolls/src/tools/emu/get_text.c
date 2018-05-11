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

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>

#include "get_text.h"

char *textindexes[MAX_MESSAGES];
int n_messages;
char *Text;

void get_text(char* textname)
{
    int c,i,j,n,fd;
#ifndef ARM
	struct stat stbuf;

	fd = open(textname,O_RDONLY);
	if (fd == -1) 
        {
            printf("can't open file '%s'\n", textname);
            return;
        }

	fstat(fd, &stbuf);
	Text = malloc(stbuf.st_size+100);	/* enough space for the text */
	n = read (fd, Text, stbuf.st_size);

	close(fd);
#else
        FILE *fp;
        fp = fopen(textname, "r");
        if (fp == NULL) {
            printf("can't open file '%s'\n", textname);
            return;
        }
        Text = malloc(128000);
        n = fread(Text, 1, 128000, fp);
        fclose(fp);
#endif

	i=0;
	textindexes[i] = &Text[0];

	for (j=0;j<n;j++)
		{
		c = Text[j];
		if (c==10) 
			{
			Text[j] = 0;
			textindexes[++i] = &Text[j+1];
			}
		}
	Text[n+1] = 0;		/* just in case */
	n_messages=i;
}

