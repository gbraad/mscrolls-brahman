#include <stdio.h>

unsigned char progdata[0x10000];
unsigned char reversed[0x10000];
unsigned char crypted[0x10000];

main (argc,argv)
int argc;
char **argv;
{
	int i,position,offset,seed,up,down,point;
	int permsize;
	register c;
	char temp[100];
	FILE *ipfp,*opfp;

	i=c=position=seed=0;
	bzero(progdata, 0x10000);

	if (argc != 3)
		{
		fprintf(stderr,"Useage: ecrypt <perm> <rest>\n");
		exit(-1);
		}


	/* open perm file */
	if ((ipfp=fopen(argv[1],"r"))==NULL)
		{
		perror(argv[1]);
		exit(-1);
		}
	/* load perm section */
	while ((c=getc(ipfp))!=EOF)
		progdata[position++]=c;
	fclose(ipfp);
	/* round up to 256 bdry */
	position = (position + 255) & ~255;
	permsize = position;

	/* open rest */
	if ((ipfp=fopen(argv[2],"r"))==NULL)
		{
		perror(argv[2]);
		exit(-1);
		}
	/* load rest section */
	while ((c=getc(ipfp))!=EOF)
		progdata[position++]=c;
	fclose(ipfp);
	/* pad to 256 byte bdry */
	position = (position + 255) & ~255;

	/* now reverse the data on a page-wise basis */

	seed=0;

	for (offset=0;offset!=position;offset=offset+0x100)
		{
		down=0xff;
		for (up=0;up<0x100;up++,down--)
			reversed[offset+up]=progdata[offset+down];

		xcluse(seed,offset);
		seed++;
		}

	/* output the crypted data */
	strcpy(temp, argv[1]);
	strcat(temp, ".cr");
	opfp = fopen(temp, "w");
	if (!opfp) {
		perror(temp);
		exit(-1);
	}
	for (i=0; i<permsize; i++)
		putc(crypted[i], opfp);
	fclose(opfp);
	strcpy(temp, argv[2]);
	strcat(temp, ".cr");
	opfp = fopen(temp, "w");
	if (!opfp) {
		perror(temp);
		exit(-1);
	}
	for (; i<position; i++)
		putc(crypted[i], opfp);
	fclose(opfp);
}

/* This does the EORing .... */

xcluse(page68,startpos)
int  page68,startpos;
{
	int begin,index;

	begin=0;
	index=(page68 & 0x7)+1;

	while (index<0x100) {
	crypted[startpos+begin]=(reversed[startpos+begin]^reversed[startpos+index]);
	begin++;
	index++;
	}


	if (begin<0xff) {

	for (index=0xff;index!=begin; index--)
	crypted[startpos+index]=(reversed[startpos+begin]^reversed[startpos+index]);
	}
	crypted[startpos+begin]=reversed[startpos+begin];
}
