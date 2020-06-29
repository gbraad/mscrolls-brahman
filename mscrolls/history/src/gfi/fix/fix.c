#include <stdio.h>
#include "hunks.h"
#include "fix.h"

#define TRUE 1
#define FALSE 0

#define getlong() readlong(fp)
#define putlong(xyzzy) writelong(out,xyzzy)

int verbose=FALSE;
int code_base=0;

main (argc,argv)
int argc;
char **argv;
{
	int i;
	FILE *infile,*outfile;

	if (argc==1)
		{
		verbose=FALSE;
		dump(stdin,"STDIN",stdout,"STDOUT");
		}
	else
		{
		i=3;	/* for extreme randomness */
		sscanf(argv[1], "%x", &code_base);
		if (argc!=(i+1))
			{
			fprintf(stderr,"Usage: fix <code base> <AmigaDOS file> <ST file>\n");
			exit(-1);
			}
		if ((infile=fopen(argv[i-1],"r"))==NULL)
			{
			fprintf(stderr,"Fix: unable to open %s\n",argv[i]);
			exit(-1);
			}
		if ((outfile=fopen(argv[i],"w"))==NULL)
			{
			fprintf(stderr,"Fix: unable to open %s\n",argv[i]);
			exit(-1);
			}
		dump(infile,argv[i-1],outfile,argv[i]);
		}
}


dump(fp,name,out,oname)
FILE *fp,*out;
char *name,*oname;
{
	register int n,code_size,bss_size;
	register char *codepnt;
	int data_size=0;	/* frig cos we don't use the data segment */	
	if (getlong()!=hunk_header)
		{
		fprintf(stderr,"Bad file format (no hunk_header)  - %s\n",name);
		return;
		}
	while ((n=getlong())!=0) skip(fp,n);	/* miss library names to load */
	if ((n=getlong())>2)
		{
		fprintf(stderr,"Bad file format (too many hunks)  - %s\n",name);
		return;
		}
	skip(fp,n);
	code_size=4*getlong();
	bss_size=4*getlong();
	if (verbose) printf("code size = %08x\nBSS size = %08x\n",code_size,bss_size);
	if ((n=getlong())!=hunk_code) 	
		{
		fprintf(stderr,"Bad file format (no code hunk)  - %s\n",name);
		return;
		}
	if ((n=4*getlong())!=code_size)
		{
		fprintf(stderr,"Bad file format (code size mismatch)  - %s\n",name);
		return;
		}
	
	putc(0x60,out);		/* write ST 'magic' no. (really a BRA.S...) */
	putc(0x1A,out);
	putlong(code_size);	/* no. of BYTES in text segment */
	putlong(data_size);	/* no. in data segment (ZERO TODAY!!!) */
	putlong(bss_size);	/* size of BSS */
	putlong(0);			/* no symbol table */

	putlong(0);			/* always zero (see DRI for details) */
	putlong(0);			/* ditto */


/* this next word is FFFF for absolute code (no relocation info), otherwise 0 */

	putc(0xFF,out);		/* and a word (ie 16 bits) */
	putc(0xFF,out);		/* at the end of it all */

/* now the code starts... */

	if ((codepnt=(char *)malloc(code_size))==NULL)
		{
		fprintf(stderr,"Waaaaaaaa: out of memory in %s\n",name);
		exit(-1);
		}
	

	for (n=0;n<code_size;n++) codepnt[n]=getc(fp);

	if ((n=getlong())!=hunk_reloc32)
		{
		if (verbose) printf("No relocation bits in %s\n",name);
		}
	else
		{
		register xyzzy;
		while ((xyzzy=getlong())!=0)
			{
			if (getlong()==0)
				{
				if (verbose) printf("Ignoring hunk 0 reloc32 of %04x records\n",xyzzy);
				skip(fp,xyzzy);
				}
			else
				{
				register int loop,offset,newval,*pnt;
				if (verbose) printf("Relocating hunk 1 reloc32 of %04x records\n",xyzzy);
				for (loop=0;loop<xyzzy;loop++)
					{
					offset=getlong()-code_base;
					if (verbose) printf("%08x\n",offset);
					pnt=(int *)&codepnt[offset];
					newval=reverse_l(*pnt)+code_size+code_base;
					*pnt=reverse_l(newval);
					}
				}
			}
		}
	for (n=0;n<code_size;n++) putc(codepnt[n],out);

	fclose(out);
	fclose(fp);
	return;
}
	
/* skip n long words in input file */

skip(fp,n)
FILE *fp;
int n;
{
	register int i;
	for (i=0;i<n;i++) getlong();
}

/* read a long word from input file */


readlong(fp)
FILE *fp;
{
	register u_long result;
	u_char c0,c1,c2,c3;

	c0=getc(fp);
	c1=getc(fp);
	c2=getc(fp);
	c3=getc(fp);

	result=(c0<<24)|(c1<<16)|(c2<<8)|c3;

	return (int)result;
}

/* writelong - writes a long to a file */
 

writelong(fp,l)
FILE *fp;
u_long l;
{
	u_char c0,c1,c2,c3;

	c0=(l>>24)&0xFF;
	c1=(l>>16)&0xFF;
	c2=(l>>8)&0xFF;
	c3=l&0xFF;

	putc(c0,fp);
	putc(c1,fp);
	putc(c2,fp);
	putc(c3,fp);
}


reverse_l(number)
M68 number;
{
	u_char temp;

	temp=number.chars.c1;
	number.chars.c1=number.chars.c4;
	number.chars.c4=temp;
	temp=number.chars.c3;
	number.chars.c3=number.chars.c2;
	number.chars.c2=temp;

	return (int)number.L;
}
