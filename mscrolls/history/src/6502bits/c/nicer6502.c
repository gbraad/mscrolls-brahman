

/*************************************************************************
*
*	@(#)nicer6502.c	1.1
*	6/24/88
*
*************************************************************************/



#include <stdio.h>

int ltree[128],rtree[128],nodes;
int size;
unsigned char store[200000];

main()
{
	register i,c,S;

	S=getchar();
	S=S+(getchar()<<8);
	nodes=getchar()+1;

	for (i=0;i<nodes;i++)
		{
		ltree[i]=getchar();
		rtree[i]=getchar();
		}

	i=0;
	while ((c=getchar())!=EOF)
		store[i++]=(unsigned char)c;

	size = i;

	putchar(S&0xFF);
	putchar((S>>8)&0xFF);

/* tree size fixed */

	for (i=0;i<16;i++) putchar(ltree[i]);
	for (i=0;i<16;i++) putchar(rtree[i]);

	for (i=0;i<size;i++) putchar(store[i]);

}
