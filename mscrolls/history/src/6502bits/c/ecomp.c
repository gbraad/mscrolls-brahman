#include <stdio.h>

main(argc, argv)
char **argv;
{
	register run, i, c;
	int permsize, runsize=0;
	FILE *in, *out;
	char temp[100];

	if (argc != 3) {
		printf("Usage: ecomp <perm pages in hex> <prog>\n");
		exit(-1);
	}

	sscanf(argv[1], "%x", &permsize);
	permsize *= 0x100;

	in = fopen(argv[2], "r");
	if (!in) {
		perror(argv[2]);
		exit(-1);
	}

	strcpy(temp, argv[2]);
	strcat(temp, ".perm");
	out = fopen(temp, "w");
	if (!out) {
		perror(out);
		exit(-1);
	}

	/* runline the permanent section */
	putc(0, out);		/* these get written over later */
	putc(0, out);
	runsize = 2;		/* size of runlined perm section */
	run = 0;
	for (i=0; i<permsize; i++) {
		c = getc(in);
		if (c==0) run++;
		else {
			if (run) {
				while (run > 255) {
					putc(0, out);
					putc(255, out);
					run -= 255;
					runsize += 2;
				}
				if (run) {
					putc(0, out);
					putc(run, out);
					run = 0;
					runsize += 2;
				}
			}
			putc(c, out);
			runsize++;
		}
	}
	if (run) {
		while (run > 255) {
			putc(0, out);
			putc(255, out);
			run -= 255;
			runsize += 2;
		}
		if (run) {
			putc(0, out);
			putc(run, out);
			runsize += 2;
		}
	}
	/* pad with -1's */
	for (i=((runsize+255)&~255) - runsize; i; i--)
		putc(-1, out);
	/* zap in the size of the runlined perm section */
	rewind(out);
	putc(runsize >> 8, out);
	putc(runsize, out);
	fclose(out);
	printf(".define NUMPERM = 0%xh\n", permsize/256);
	printf(".define PHYSNUMPERM = 0%xh\n", (runsize+255)/256);

	strcpy(temp, argv[2]);
	strcat(temp, ".rest");
	out = fopen(temp, "w");
	if (!out) {
		perror(temp);
		exit(-1);
	}

	/* copy the rest */
	while ((c = getc(in)) != EOF) putc(c, out);
	fclose(out);
	fclose(in);
}
