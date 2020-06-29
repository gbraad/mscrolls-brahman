/*
 * args :
 *	make_xdef(objfile, label, value);
 *
 * format :
 * 
 * hunk_unit, 0
 * hunk_name, "ADV"
 * hunk_code, 0
 * hunk_ext, ext_abs label = value
 * 0
 * hunk_end
 */

#include <stdio.h>
#include "hunks.h"

make_xdef(objfile, label, value)
char *objfile, *label;
int value;
{
	register i, rnd;
	register char *p;
	register FILE *fp;

	fp = fopen(objfile, "w");
	if (!fp) {
	    perror(objfile);
	    exit(-1);
	}
	putlong(hunk_unit, fp);
	putlong(0, fp);

	putlong(hunk_name, fp);
	putlong(1, fp);
	putc('A', fp);
	putc('D', fp);
	putc('V', fp);
	putc('\0', fp);

	putlong(hunk_code, fp);
	putlong(0, fp);

	putlong(hunk_ext, fp);
	i = strlen(label);
	rnd = (i+3) & ~3;
	putlong((ext_abs<<24) + (rnd/4), fp);
	for (p=label; *p; p++) putc(*p, fp);
	while (i & 3) {
		putc(0, fp);		/* padding */
		i++;
	}
	putlong(value, fp);
	putlong(0, fp);		/* end of hunk_ext */

	putlong(hunk_end, fp);

	fclose(fp);
}

putlong(n, fp)
register n;
register FILE *fp;
{
	putc(n>>24, fp);
	putc(n>>16, fp);
	putc(n>>8, fp);
	putc(n, fp);
}
