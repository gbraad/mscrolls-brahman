#include <stdio.h>

#define NAME	"roddyhuf"

main(argc, argv)
register char **argv;
{
	FILE *fin, *fleft, *fright, *findex, *fout;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s file\n", NAME);
		exit(-1);
	}
	fin = fopen(argv[1], "r");
	if (!fin) {
		fprintf(stderr, "%s: can't open \"%s\"\n", NAME, argv[1]);
		exit(-1);
	}
	fleft = fopen("huff.lef", "w");
	fright = fopen("huff.rig", "w");
	findex = fopen("huff.ind", "w");
	fout = fopen("huff.out", "w");
	huf(fin, fleft, fright, findex, fout);
}
