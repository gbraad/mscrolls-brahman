

/*************************************************************************
*
*	@(#)toapple.c	1.1
*	6/24/88
*
*************************************************************************/


/*
	toapple.c

	Prog (filter) to send binary files direct to Apple 
	Uses 'beeb to apple' format:

		sends 4 bits at a time - low followed by high
		file terminated with a $7F
		only low bits of chars sent are used at Apple end.

*/

#include <stdio.h>

main()
{
	register c;

	while ((c=getchar())!=EOF) 
		{
		putchar((c&0x0F)+'0');
		putchar(((c>>4)&0x0F)+'0');
		}
	putchar(0x7F);
}


