#include <stdio.h>

unsigned char thelot[PERMSIZE];

unsigned char guildrest[0x10000];

main(argc, argv)
char **argv;
{
	register z;
	FILE *fp,*fp1,*f1,*f2,*f4;
	int i,count,run,lengthhi,lengthlow,x,y,c;
	int permsize;

	if (argc != 3) {
		printf("Usage: makeeltham <number of permanent pages> <file>\n");
		exit(-1);
	}

	sscanf(argv[1], "%x", &permsize);
	permsize *= 0x100;

	if ((in=fopen(argv[2],"w"))==NULL)
		{
		perror(argv[2]);
		exit (-1);
		}

	if ((out=fopen("progobj.run","w"))==NULL)
		{
		perror("progobj.run");
		exit (-1);
		}


	for (z=0;z<PERMSIZE;z++)
		putc(getc(in),out);

	x=y=0;
	while ((y=getchar())!=EOF)
		guildrest[x++]=y;


	fclose(fp);


/*
	This will take i/p from stdin and runline any zeroes it contains.
	It writes it's o/p to the file 'guildrun' since it is intended
	to be used to runline the Guild permanent section for storage on
	poxy small disks for 8-bit versions.
	There's a prog called unrun0.c which (just to prove it works)
	will restore the 'guildrun' file back to it's former glory
	in a file called 'guildunrun'.

	Paul 23/4/87
*/



	if ((f1=fopen("/tmp/guildrun.run0tmp","w"))==NULL)
		{
		fprintf(stderr,"Waaa, can't write to /tmp/guildrun.run0tmp");
		exit (-1);
		}

	if ((f4=fopen("guildperm","r"))==NULL)
		{
		fprintf(stderr,"Waaa, can't read guildperm");
		exit (-1);
		}

	i=c=count=run=0;

	while ((c=getc(f4))!=EOF)
		thelot[i++]=c;
	
	fclose (f4);

	while (count<i)
		{

		if (thelot[count]!=0)
			putc(thelot[count++],f1);

		else {

		if (thelot[count]==0)
			{
			run=0;
			while ((thelot[count]==0) && (count<i))
				{
				count++;
				run++;
				if (run==0xff) {
					putc(0,f1);
					putc(run,f1);
					run=0;
					}
				}
/*			--count; */
			if (run!=0) {
				putc(0,f1);
				putc(run,f1);
				}
			}
		     }
		}

	fclose(f1);

	if ((f2=fopen("/tmp/guildrun.run0tmp","r"))==NULL)
		{
		fprintf(stderr,"Waaa, can't read /tmp/guildrun.run0tmp");
		exit (-1);
		}


	i=c=0;
	while ((c=getc(f2))!=EOF)
		thelot[i++]=c;

	fclose(f2);

	i=i+2;		/* Add the 2 bytes used to store the length */
	fprintf(stderr,"File run-lined to length $%04x\n",i);
	lengthlow=i & 0xff;
	lengthhi= i>>8;
	lengthhi= lengthhi & 0xff;

	putc(lengthhi,fp1);
	putc(lengthlow,fp1);

	c=0;
	while (c<i-2)
		putc(thelot[c++],fp1);

/* Now pad the o/p file to a page boundary */
	while ((i & 0xff) !=0) {
		putc(0xff,fp1);
		i++;
		}

	c=0;
	while (c<x)
		putc(guildrest[c++],fp1);

	fclose(fp1);
}
