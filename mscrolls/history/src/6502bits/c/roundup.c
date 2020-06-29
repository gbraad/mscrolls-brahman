#include <stdio.h>

main(argc, argv)
char **argv;
{
    register i, n, c;
    int size;
    register FILE *fp;
    
    if (argc != 3) {
	printf("Usage: roundup size file\n");
	exit(-1);
    }
    
    size = atoi(argv[1]);
    fp = fopen(argv[2], "r");
    if (!fp) {
	perror(argv[2]);
	exit(-1);
    }
    n = 0;
    while ((c = getc(fp)) != EOF) {
	n++;
	putchar(c);
    }
    while (n % size) {
	n++;
	putchar(0);
    }
    
}

