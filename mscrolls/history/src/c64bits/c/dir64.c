/****************************************************************************
 *
 * %W%
 * %G%
 *
 ****************************************************************************/

#include "c64disk.h"
#include <sys/file.h>

extern sectors_per_track[];

main(argc, argv)
char **argv;
{
	register struct c64disk *dp;
	register struct gamedir *gdp;
	register struct entry *ep;
	register struct dosdir_hdr *hp;
	register struct dosdir_sector *sp;
	register struct file_entry *fp;
	sector_address sa;
	u_char *p;
	int i,j,n;
	char name[20];
	int fd;
	int extract=0;

	if (argc == 3) {
		if (strcmp(argv[1], "-x"))
			goto usage;
		extract=1;
		argc--;
		argv++;
	}
	if (argc != 2) {
usage:
		printf("Usage: c64dir [-x] file\n");
		exit(-1);
	}

	dp = readdisk(argv[1]);

	printf("game directory :\n");

	gdp = (struct gamedir *)dp->tracks[0].sectors[2];
	for (i=0, ep = &gdp->entries[0]; i<64; i++, ep++) {
		if (ep->start.track == 0) continue;
		printf("file #%d : start (%d,%d), %d sectors, disk %d\n",
			i,
			ep->start.track,
			ep->start.sector,
			ep->num_sectors,
			ep->which
		);
	}

	printf("dos directory :\n");
	hp = get_dos_hdr(dp);
	printf("Disk Name : ");
	printname(hp->dsk_name,23);
	putchar('\n');

	sa.track = 18;
	sa.sector = 1;
	while (sa.track) {
		sp = (struct dosdir_sector *)get_sector(dp, sa);
		for (i=0; i<8; i++) {
			fp = &sp->file_entries[i];
			if (!(fp->type & 0x3f))
				continue;
			printname(fp->filename, 16);
			putchar(' ');
			switch(fp->type & 0x3f) {
			case 1 :
				printf("SEQ\t");
				break;
			case 2 :
				printf("PROG\t");
				break;
			case 3 :
				printf("USER\t");
				break;
			case 4 :
				printf("REL\t");
				break;
			}
			printf("sectors : ");
			strcpy(name, "XX");
			for (j=0; j<16; j++)
				if (fp->filename[j] == 160) break;
				else name[j+2] = fp->filename[j];
			name[j+2] = 0;
			if (extract)
				fd = open(name, O_WRONLY|O_CREAT|O_TRUNC, 0666);
			sa = fp->data;
			n=0;
			while (sa.track) {
				printf("%d,%d ", sa.track, sa.sector);
				p = get_sector(dp, sa);
				if (extract && p[0])
					write(fd, p+2, 254);
				sa.track = p[0];
				sa.sector = p[1];
				n++;
			}
			if (extract) {
				write(fd, p+2, p[1]);
				close(fd);
			}
			printf("last sector has %d bytes", sa.sector);
			printf(" total of %d sectors (%d)\n", n, fp->num_blocks_lo +
				256*fp->num_blocks_hi);
		}
		sa = sp->next_dir_sector;
	}
	j=0;
	for (i=0; i<35; i++) {
		if (i==17) continue;	/* ignore dos track */
		j += hp->avail_maps[i].num_sectors;
	}
	printf("%d blocks free\n", j);
}

printname(s, len)
u_char *s;
{
	register i;
	for (i=0; i<len; i++)
		if (s[i]==160) putchar(' ');
		else putchar(s[i]);
}
