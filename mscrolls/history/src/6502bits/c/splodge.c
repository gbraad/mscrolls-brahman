

/*************************************************************************
*
*	@(#)splodge.c	1.1
*	6/24/88
*
*************************************************************************/



#include <stdio.h>

main()
{
	FILE *prog,*mac,*result,*text;
	register i,c;

	if ((prog=fopen("undo.o","r"))==NULL)
		{
		printf("waaa can't open undo.o\n");
		exit(-1);
		}
	if ((mac=fopen("nicer4","r"))==NULL)
		{
		printf("waaa can't open nicer4\n");
		exit(-1);
		}
	if ((result=fopen("result","w"))==NULL)
		{
		printf("waaa can't open result\n");
		exit(-1);
		}

	for (i=0;i<256;i++)
		if ((c=getc(prog))==EOF) putc(0,result);
		else putc(c,result);

	while ((c=getc(mac))!=EOF) putc(c,result);

	fclose(prog);
	fclose(mac);
	fclose(result);
}
