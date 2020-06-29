#include <stdio.h>

main(argc, argv)
char **argv;
{
	register c, n, lines=0;
	FILE *fin, *fout = stdout;

	if (argc != 2) {
		printf("Usage: dotitle64 file\n");
		exit(-1);
	}

	fin = fopen(argv[1], "r");

	for (;;) {
		for (n=0; n<40; n++) {
			c = getc(fin);
			if (c == EOF || lines==25) goto done;
			if (c == '\n') {
				for (; n<40; n++) putc(' ', fout);
				lines++;
				break;
			}
			out(c, fout);
		}
		if (c != '\n') {
			while (c != '\n') c = getc(fin);
			lines++;
		}
	}
done:
	while (lines < 25) {
		for (n=0; n<40; n++) putc(' ', fout);
		lines++;
	}
	for (n=0; n<24; n++) putc(0, fout);		/* round up to 1024 bytes */
	for (n=0; n<32; n++) putc(0, fout);		/* add 32 zeros for luck */
}

/* convert an ascii char to C64 screen code */
out(c, fp)
register c;
FILE *fp;
{
	if (c >= 96 && c < 128) putc(c-96, fp);
	else putc(c, fp);
}
