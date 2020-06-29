#include <stdio.h>

/* simple byte-resolution checksum proggy */

main()
{
int c,sum=0;


	while ((c=getchar())!=EOF) {
		if (c!=EOF)
		sum=sum+c;
			}
	printf ("Checksum is $%x\n",sum&0xffff);
}
