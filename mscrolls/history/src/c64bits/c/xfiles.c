#include <sys/file.h>
#include "c64disk.h"

extern sectors_per_track[];

main(argc, argv)
char **argv;
{
	register struct c64disk *dp1, *dp2;
	register struct c64disk *dp;
	register struct gamedir *gdp;
	register struct entry *ep;
	sector_address sa;
	register i;
	int fd, n, max;
	char s[100];

	if (argc != 3) exit(-1);
	dp1 = readdisk(argv[1]);
	dp2 = readdisk(argv[2]);
	gdp = (struct gamedir *)dp1->tracks[0].sectors[2];
	for (n=0, ep= &gdp->entries[0]; ep->start.track; n++, ep++) {
		printf("%d...\n", n);
		sprintf(s, "XXfile%d", n);
		fd = open(s, O_WRONLY|O_CREAT|O_TRUNC, 0666);
		if (fd == -1) {
			printf("fuck!\n");
			exit(-1);
		}
		if (ep->which < 2)		/* on disk 1 */
			dp = dp1;
		else
			dp = dp2;

		sa = ep->start;
		max = ep->num_sectors;
		if (!max) max = 256;
		for (i=0; i<max; i++) {
			if (n==3) printf("%x,%d,%d\n", i, sa.track, sa.sector);
			write(fd, get_sector(dp, sa), 256);
			sa.sector++;
			if (sa.sector == sectors_per_track[sa.track-1]) {
				sa.sector = 0;
				sa.track++;
				if (sa.track == 18) sa.track++;	/* ignore dir track */
			}
		}
		close(fd);
	}
}
