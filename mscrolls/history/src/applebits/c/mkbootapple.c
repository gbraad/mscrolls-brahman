
/**************************************************************************
*
*	@(#)mkbootapple.c	1.3
*	7/15/88
*
**************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/param.h>

/*
	Construct a file which will bootstrap and begin to run the 6502
	front end system on an Apple II.

	Paul 24/6/88
*/


#define BIGENUF	0x7800
#define MAXFILNAMLEN 50



main(argc, argv)
int argc;
char **argv;
{
	char s[NCARGS];
	char filnam[MAXFILNAMLEN];
	char prognam[30];
	char *p;
	unsigned char buf[BIGENUF];
	int volume,track,sector=0;
	int i=0;
	register j;
	FILE *fpin,*fpout;


  	strcpy(prognam,argv[0]);

    if (argc != 4) {
		printf("usage: %s file volume # o/p file\n",prognam);
		exit(-1);
		}

	if ((fpin=fopen(argv[1],"r"))==NULL) {		/* .asm files to make */
		perror(argv[1]);
		exit(-1);
		}

    volume = atoi(argv[2]);						/* get disk volume # */
	if ((!volume) || (volume > 0xff)) {
		printf("%s: volume # %s is silly\n",prognam,argv[2]);
		exit(-1);
		}

	if ((fpout=fopen(argv[3],"w"))==NULL) {	/* o/p from this prog */
		perror(argv[3]);
		exit(-1);
		}

	while (fgets(filnam,MAXFILNAMLEN,fpin)) {

/* build a command line using the file names in file fpin */

		p = index(filnam, '\n');			/* fod the CR fgets left there */
		*p = '\0';					/* make it a space */
		strcpy(s,"as6502 -o ");		/* assembler name and options */
		strcat(s,filnam);			/* file to be assembled */
		if (p = index(filnam, '.')) 	/* replace .xyz with .ls */
			strcpy(p,".ls");
			else 					/* else just add .ls */
				strcat(filnam,".ls");
		strcat(s," > ");				/* redirect listing */
		strcat(s,filnam);			/* name of listing */
		
		callsys(s);					/* execute command we built */
		i=addfile("6502.out",buf,i);		/* slurp the executable */
	}

/* Need to fix boot disk volume # as per command line */

	buf[i-20]=volume;			/* ahem! NB if .asm files change this
								   won't work!!!! */
	buf[i-31]=volume;

/* and initial track & sector # depending on how big the bootfile is */

	j=i;
	while (j&0xff)
		j++;

	track = ((j/256)&0xf0);
	sector = ((j/256)&0xf);

	buf[i-30]=track;
	buf[i-29]=sector;

	i=pad(buf,i);					/* pad to page boundary */

	i=addfile("result",buf,i);		/* add compressed 6502 */
	i=pad(buf,i);					/* pad for luck */

	for (j=0;j<i;j++) 				/* stir thoroughly */
		putc(buf[j],fpout);			/* and pour it out */
	fclose(fpout);
	fclose(fpin);

}

callsys(s)
char *s;
{
	if (system(s))
		exit(-1);
}

addfile(s,buffer,i)
char *s,*buffer;
int i;
{
	FILE *ipfp;
	int c;

	if ((ipfp=fopen(s,"r"))==NULL) {
		perror(s);
		exit(-1);
		}

	while ((c = getc(ipfp)) != EOF)
		buffer[i++]=c;

	fclose(ipfp);
	return(i);
}


/*
   pad buffer with zeroes to a page boundary.
   NB should do nothing if already at a boundary
*/

pad(buffer,i)
char *buffer;
int i;
{
	while ((i&0xff)!=0)
		buffer[i++]=0;
	return(i);
}

