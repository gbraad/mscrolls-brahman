#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main()
{
	register FILE *fleft, *fright;
	register i;
	struct stat stbuf;

	stat("huff.lef", &stbuf);	/* to find out file size	*/
	fleft = fopen("huff.lef", "r");
	fright = fopen("huff.rig", "r");
	putchar(stbuf.st_size-1);	/* number of nodes minus one	*/
	for (i=stbuf.st_size; i; i--) {
		putchar(getc(fleft) ^ 128);
		putchar(getc(fright) ^ 128);
	}
}
