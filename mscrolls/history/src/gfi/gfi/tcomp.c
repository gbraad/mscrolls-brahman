#include <sys/file.h>
#include <stdio.h>

char *filebuf, *newbuf, *malloc();
int filelen, newlen;
char *tokenfile = "tokenfile";
int dryrun = 0;

main(argc, argv)
char **argv;
{
	register fd;
	register i;
	FILE *fleft, *fright, *findex, *fout;
	register char *p;

	/* args */
	for (i=1; i<argc && argv[i][0]=='-'; i++)
		for (p = &argv[i][1]; *p; p++)
			switch (*p) {
			case 't' :	/* tokenfile */
				tokenfile = argv[i+1];
				i++;
				break;
			case 'd' :	/* dryrun */
				dryrun = 1;
				break;
			}

	read_tokens(tokenfile);

	/* load up the file */
	filelen = filelength(argv[i]);
	filebuf = malloc(filelen);
	fd = open(argv[i], O_RDONLY);
	read(fd, filebuf, filelen);
	close(fd);

	/* tokenise it */
	printf("tokenising...\n");
	tokenise();

	printf("tokenising saved %d bytes\n", filelen-newlen);
	if (dryrun)
		exit(0);

	/* huf it */
	printf("huffing...\n");
	filebuf = newbuf; filelen = newlen;
	fleft = fopen("text.tree.left", "w");
	fright = fopen("text.tree.right", "w");
	findex = fopen("text.indexes", "w");
	fout = fopen("text.out", "w");
	huf (fleft, fright, findex, fout);
}
