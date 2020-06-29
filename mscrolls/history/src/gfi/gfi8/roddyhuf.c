#include <stdio.h>

#define NAME	"roddyhuf"

main(argc, argv)
register char **argv;
{
	FILE *fin, *fleft, *fright, *findex, *fout;
	int wrap;

	if (argc == 3)
	    sscanf(argv[2], "%x", &wrap); /* where to wrap the indexes */
	else if (argc == 2)
	    wrap = 0;		/* flag to compress old style (small) text */
	else {
		fprintf(stderr, "Usage: %s file\n", NAME);
		exit(-1);
	}
	fin = fopen(argv[1], "r");
	if (!fin) {
		fprintf(stderr, "%s: ", NAME);
		perror(argv[1]);
		exit(-1);
	}
	fleft = fopen("huff.lef", "w");
	fright = fopen("huff.rig", "w");
	findex = fopen("huff.ind", "w");
	fout = fopen("huff.out", "w");
	huf(fin, fleft, fright, findex, fout, wrap);
}
