#include <stdio.h>

main(argc, argv)
char **argv;
{
	FILE *code,*reloc;
	register i,c,start;

	if (argc != 3) {
		fprintf(stderr,"Usage: donybbles <6502 code> <reloc.o file>\n");
		exit(-1);
	}
	if ((reloc=fopen(argv[2],"r"))==NULL)
		{
		fprintf(stderr,"waaa can't open %s\n", argv[2]);
		exit(-1);
		}
	if ((code=fopen(argv[1],"r"))==NULL)
		{
		fprintf(stderr,"waaa can't open %s\n", argv[1]);
		exit(-1);
		}

	if ((c=getc(code))!=0x4C) 
		{
		fprintf(stderr,"Bad file format: must start with a JMP!\n");
		exit(-1);
		}

	/* calculate address to put on stack */

	start=getc(code)&0xFF;
	start=start+((getc(code)&0xFF)<<8)-1;

	while ((c=getc(code))!=EOF) whex(c,stdout);

	whex((start>>8)&0xFF,stdout);
	whex(start&0xFF,stdout);

	while ((c=getc(reloc))!=EOF)  whex(c,stdout);

	fclose(stdout);
	fclose(reloc);
}

whex(c,fp)
int c;
FILE *fp;
{
	putc((c>>4)&0x0F,fp);
	putc(c&0x0F,fp);
}
