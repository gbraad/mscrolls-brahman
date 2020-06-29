/*
 * args :
 *	mkobj input label output
 *
 * format :
 * 
 * hunk_unit, 0
 * hunk_name, "ADV"
 * hunk_code, size/4
 * text.raw	rounded to lword
 * hunk_ext, ext_def label = 0
 * 0
 * hunk_end
 */

#include <stdio.h>
#include "hunks.h"

main(argc, argv)
char **argv;
{
	register c, i;
	register char *p;
	int size;	/* = filelength("text.raw");*/
	int rnd;	/* = (size + 3) & ~3;	/* round up to lword bdry */
	register FILE *fp, *fq;

	if (argc != 4) {
		printf("Usage: mkobj binaryfile labelname outputfile\n");
		exit(-1);
	}
	size = filelength(argv[1]);
	rnd = (size + 3) & ~3;
	fp = fopen(argv[3], "w");
	putlong(hunk_unit, fp);
	putlong(0, fp);

	putlong(hunk_name, fp);
	putlong(1, fp);
	putc('A', fp);
	putc('D', fp);
	putc('V', fp);
	putc('\0', fp);

	putlong(hunk_code, fp);
	putlong(rnd/4, fp);
	fq = fopen(argv[1], "r");
	while ((c = getc(fq)) != EOF) putc(c, fp);
	for (i=rnd-size; i; i--) putc(0, fp);	/* round up */
	fclose(fq);

	putlong(hunk_ext, fp);
	i = strlen(argv[2]);
	rnd = (i+3) & ~3;
	putlong((ext_def<<24) + (rnd/4), fp);
	for (p=argv[2]; *p; p++) putc(*p, fp);
	while (i & 3) {
		putc(0, fp);		/* padding */
		i++;
	}
	putlong(0, fp);		/* value */
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
