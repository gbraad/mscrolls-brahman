

/*************************************************************************
*
*	@(#)makerun.c	1.1
*	6/24/88
*
*************************************************************************/



#include <stdio.h>

main()
{
	FILE *run,*reloc;
	register i,c,start;

	if ((reloc=fopen("reloc.o","r"))==NULL)
		{
		printf("waaa can't open reloc.o\n");
		exit(-1);
		}
	if ((run=fopen("run4","w"))==NULL)
		{
		printf("waaa can't open run4\n");
		exit(-1);
		}

	if ((c=getchar())!=0x4C) 
		{
		printf("Bad file format: must start with a JMP!\n");
		exit(-1);
		}

	/* calculate address to put on stack */

	start=getchar()&0xFF;
	start=start+((getchar()&0xFF)<<8)-1;

	while ((c=getchar())!=EOF) whex(c,run);

	whex((start>>8)&0xFF,run);
	whex(start&0xFF,run);

	while ((c=getc(reloc))!=EOF)  whex(c,run);

	fclose(run);
	fclose(reloc);
}

whex(c,fp)
int c;
FILE *fp;
{
	putc((c>>4)&0x0F,fp);
	putc(c&0x0F,fp);
}
