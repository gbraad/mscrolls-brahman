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

main(argc,argv)
char **argv;
{
	register i;
	register FILE *fp;

	if (argc > 1)
		for (i=1; i<argc; i++) {
			fp = fopen(argv[i], "r");
			if (!fp) {
				fprintf(stderr, "%s: can't open \"%s\" : ",
					*argv, argv[i]);
				perror("");
			} else {
				line(fp);
				fclose(fp);
			}
		}
	else
		line(stdin);
}

line(fp)
register FILE *fp;
{
	register c;
	register char *p;
	int lineno=0;
	char buf[200];

	while ((c = getc(fp)) != EOF) {
		p = buf;
		while (c != '\n') {
			*p++ = c;
			c = getc(fp);
		}
		*p++ = c;
		*p = 0;
		printf("\t.line\t%d\n%s", lineno++, buf);
	}
}