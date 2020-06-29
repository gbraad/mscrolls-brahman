#include <stdio.h>

extern char *tokens[];
extern char *tokensCE[];
extern char *tokensFE[];

main(argc, argv)
char **argv;
{
	register c;
	register instring=0;
	register FILE *fp;

	if (argc != 2) {
		printf("Usage: ls64 file\n");
		exit(-1);
	}
	if (!strcmp(argv[1], "-"))
		fp = stdin;
	else
		fp = fopen(argv[1], "r");
	getc(fp);	/* ignore load address */
	getc(fp);
	for (;;) {
		c = getc(fp) + 256*getc(fp);	/* link to next line */
		if (!c) break;
		c = getc(fp);
		c += 256*getc(fp);
		printf("%d ", c);	/* line number */
		c = getc(fp);
		while (c) {
			if (instring)
				if (c == '"') {
					instring = 0;
					putchar(c);
				} else
					putchar(c);
			else
				if (c & 0x80)	/* a token */
					if (c == 0xCE) {
						c = getc(fp);
						printf("%s", tokensCE[c]);
					} else if (c == 0xFE) {
						c = getc(fp);
						printf("%s", tokensFE[c]);
					} else
						printf("%s", tokens[c-128]);
				else {
					if (c == '"') instring = 1;
					putchar(c);
				}
			c = getc(fp);
		}
		putchar('\n');
	}
}
