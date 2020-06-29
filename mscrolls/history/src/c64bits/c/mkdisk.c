/****************************************************************************
 *
 * %W%
 * %G%
 *
 ****************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include "c64disk.h"

main(argc, argv)
char **argv;
{
	register struct c64disk *dp1;
	register struct c64disk *dp2;
	register struct dosdir_hdr *hp1;
	register struct dosdir_hdr *hp2;
	register struct gamedir *gdp1;
	register struct gamedir *gdp2;
	struct sector_address sa;
	int rnd, len, nfiles=0, fd;
	int filenum;
	int i, where;
	char *p, *q;
	FILE *fp;
	char s[200];

	if (argc != 6) {
		printf("\
Usage: mkdisk <file of names> <output file 1> <output file 2> <Disk title> <ID bytes>\n");
		exit(-1);
	}

	strncpy(s, argv[5], 2);
	dp1 = allocdisk(argv[4], s);
	strncpy(s, argv[5]+2, 2);
	dp2 = allocdisk(argv[4], s);
	hp1 = get_dos_hdr(dp1);
	hp2 = get_dos_hdr(dp2);

	/* track 1, sector 2 is the game directory, so remove from
	 * availability map */
	hp1->avail_maps[0].avail[0] &= 0xfb;	/* 1111 1011 binary */
	hp2->avail_maps[0].avail[0] &= 0xfb;	/* 1111 1011 binary */
	/* point to game directory */
	gdp1 = (struct gamedir *)dp1->tracks[0].sectors[2];
	gdp2 = (struct gamedir *)dp2->tracks[0].sectors[2];
	/* hmm */
	for (i=0; i<64; i++) {
		gdp1->entries[i].start.track = 0;
		gdp2->entries[i].start.track = 0;
	}

	fp = fopen(argv[1], "r");
	if (!fp) {
		perror(argv[1]);
		exit(-1);
	}
	while (fgets(s, 200, fp)) {
		/* each line has format :
		 * name,disk[,place]
		 * where disk is 0, 1 or 2 for both, disk1 or disk2
		 * or 'dos' for a DOS file
		 * and place forces it to put the file at a particular
		 * file number */
		p = index(s, ',');
		if (!p) {
			printf("comma expected\n");
			exit(-1);
		}
		*p++ = 0;		/* zap comma so that s is now file name */
		if (q = index(p, ',')) {	/* any more commas ? */
			*q++ = 0;
			filenum = atoi(q);
		} else
			filenum = nfiles++;
		printf("%s...\n", s);
		if (!strcmp(p, "dos\n")) {	/* a DOS file ? */
			load_dos_file(dp2, s);	/* only put it on disk 2 */
			continue;
		}
		where = atoi(p);	/* where to put the file */
		len = filelength(s);
		rnd = (len + 255) & ~255;	/* round up */
		fd = open(s, O_RDONLY);
		switch (where) {
		case 0 :
			sa = find_free_sector(dp1,0);
			find_free_sector(dp2,0);	/* make sure same sector on both */
			break;
		case 1 :
			sa = find_free_sector(dp1,0);
			break;
		case 2 :
			sa = find_free_sector(dp2,0);
			break;
		default :
			printf("freaky disk number %d\n");
			exit(-1);
		}
		/* add to game directory */
		gdp1->entries[filenum].start = sa;
		gdp1->entries[filenum].num_sectors = rnd/256;
		gdp1->entries[filenum].which = where;
		gdp2->entries[filenum].start = sa;
		gdp2->entries[filenum].num_sectors = rnd/256;
		gdp2->entries[filenum].which = where;
		for (i=rnd/256; i; i--) {	/* read in file */
			if (where < 2)
				read(fd, get_sector(dp1, sa), 256);
			else
				read(fd, get_sector(dp2, sa), 256);
			if (where == 0)
				/* copy into disk 2 */
				bcopy(get_sector(dp1, sa), get_sector(dp2, sa), 256);
			if (i > 1) {
				switch (where) {
				case 0 :
					sa = find_free_sector(dp1,0);
					find_free_sector(dp2,0);
					break;
				case 1 :
					sa = find_free_sector(dp1,0);
					break;
				case 2 :
					sa = find_free_sector(dp2,0);
					break;
				}
			}
		}
		close(fd);
	}
	writedisk(dp1, argv[2]);
	writedisk(dp2, argv[3]);
}
