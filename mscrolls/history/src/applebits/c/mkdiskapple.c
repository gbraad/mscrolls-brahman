/****************************************************************************
 *
 * %W%
 * %G%
 *
 ****************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <strings.h>

#define	DISKSIZE 143360
#define MAXFILNAMLEN 50

char prognam[30];

main(argc, argv)
char **argv;
int argc;
{
	int disk,fd;
	register i,j;
	char *p;
	FILE *fp,*opfp1,*opfp2,*opfp3;
	char s[200];
	char trackmap[1000];
	char sectormap[1000];
	unsigned char DISK1[DISKSIZE];
	unsigned char DISK2[DISKSIZE];


  	strcpy(prognam,argv[0]);

	if (argc != 4) {
		printf("\
Usage: %s <file of names> <output file 1> <output file 2>>\n",prognam);
		exit(-1);
	}


	fp = fopen(argv[1], "r");				/* open list of files */
	if (!fp) {
		perror(argv[1]);
		exit(-1);
	}

	opfp1 = fopen(argv[2], "w");				/* open DISK1 */
	if (!opfp1) {
		perror(argv[2]);
		exit(-1);
	}

	opfp2 = fopen(argv[3], "w");				/* open DISK2 */
	if (!opfp2) {
		perror(argv[3]);
		exit(-1);
	}

	opfp3 = fopen("diskmap.i", "w");				/* open diskmap.i */
	if (!opfp3) {
		perror("diskmap.i");
		exit(-1);
	}

	for(i=0;i<DISKSIZE;i++) {					/* zero buffers */
		DISK1[i]=0;
		DISK2[i]=0;
		}
	i=j=0;					/* zero counters */

	initmap(sectormap,"BASESECT:");
	initmap(trackmap,"BASETRACK:");

	while (fgets(s, 200, fp)) {
		/* each line has format :
		 * name,disk
		 * where disk is 1 or 2 for disk1 or disk2
		*/
		p = index(s, ',');
		if (!p) {
			printf("comma expected\n");
			exit(-1);
		}
		*p++ = 0;		/* zap comma so that s is now file name */

		printf("%s...\n", s);

		disk = atoi(p);		/* which disk this file goes on */
		fd = open(s, O_RDONLY);
			if (!fd) {
				perror(s);
				exit(-1);
				}

		switch (disk) {

		case 1 :
			if (j!=0)						/* don't do BOOTFILE! */
				define(j,trackmap,sectormap);	/* add to .byte values */
			j=addfile(s,DISK1,j);		/* slurp the file */
			j=pad(DISK1,j);				/* pad to next sector if need be */
			break;
		case 2 :
			define(i,trackmap,sectormap);		/* add to .define values */
			i=addfile(s,DISK2,i);		/* slurp the file */
			i=pad(DISK2,i);				/* pad to next sector if need be */
			break;
		default :
			printf("%s: invalid disk number %d\n",prognam,disk);
			exit(-1);
		}

		close(fd);
	}
	writedisk(DISK1, opfp1);
	writedisk(DISK2, opfp2);
	writemap(trackmap,opfp3);
	writemap(sectormap,opfp3);

	fclose(opfp1);
	fclose(opfp2);
	fclose(opfp3);
	fclose(fp);
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

	while ((c = getc(ipfp)) != EOF) {
		buffer[i++]=c;
			if (i>DISKSIZE) {
				printf("Disk full!\n");
				exit(-1);
				}
		}

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
	while ((i&0xff)!=0) {
		buffer[i++]=0;
			if (i>DISKSIZE) {
				printf("Disk full!\n");
				exit(-1);
		}
	}
	return(i);
}

writedisk(buf,fp)
char *buf;
FILE *fp;
{
	register i;

	for (i=0;i<DISKSIZE;i++)
		putc(buf[i],fp);
}

/* add to track/sector positions */
define(count,trackptr,sectptr)
char *trackptr, *sectptr;
int count;
{
	int track,sector;

	track = ((count/(256*16))&0x3f);
	sector = ((count/256)&0xf);

	addvalue(track,trackptr);
	addvalue(sector,sectptr);

}

addvalue(n,ptr)
char *ptr;
int n;
{

	char *p;

	p = rindex(ptr, ',');
		if (!p) {
			printf("Help! Help!\n");
			exit(-1);
			}
	sprintf(p,"%d,,",n);

}

writemap(buf,fp)
char *buf;
FILE *fp;
{
	int i=0;
	char c;

	while ((buf[i+2])!='\0') {
		c=buf[i++];
		putc(c,fp);
		}
		putc('\n',fp);
}

itoa(n,s)
char s[];
int n;
{
	int i,sign;

	if ((sign=n) < 0 )
		n = -n;

	i=0;

	do {
		s[i++]=n % 10 +'0';
	}
	while ((n /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

reverse(s)
char s[];
{
	int c,i,j;

	for(i=0,j=strlen(s)-1;i < j;i++,j--) {
		c = s[i];
		s[i]=s[j];
		s[j]=c;
	}
}

initmap(ptr,label)
char *ptr,*label;
{
	strcpy(ptr,label);
	strcat(ptr,"\n\t.byte\t,");
}
