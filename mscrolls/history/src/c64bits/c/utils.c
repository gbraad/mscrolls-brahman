/****************************************************************************
 *
 * %W%
 * %G%
 *
 ****************************************************************************/

#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include "c64disk.h"

extern sectors_per_track[];

char *malloc();

cvt_to_cbm_ascii(c)
{
	if (islower(c)) return toupper(c);
	return c;
}

char *
zmalloc(size)
unsigned size;
{
	char *p = malloc(size);
	bzero(p, size);;
	return p;
}

filelength(name)
char *name;
{
	struct stat stbuf;

	if (stat(name, &stbuf)) {
		perror(name);
		exit(-1);
	}
	return stbuf.st_size;
}

/* return sector address of first free sector */
sector_address
find_free_sector(dp, dirtrackok)
register struct c64disk *dp;
int dirtrackok;	/* true when ok to use dirtrack */
{
	struct sector_address result;
	register i, j, bit, byte;
	register struct dosdir_hdr *hp;

	hp = get_dos_hdr(dp);
	for (i=0; i<35; i++) {
		if (!dirtrackok && i==17) continue;	/* ignore directory track */
		if (hp->avail_maps[i].num_sectors)	/* some in this track ? */
			for (j=0, bit=1, byte=0; j<sectors_per_track[i];
				j++, bit*=2) {
				if (bit==256) {
					bit = 1;
					byte++;
				}
				if (hp->avail_maps[i].avail[byte] & bit) {
					/* got one */
					result.track = i+1;
					result.sector = j;
					hp->avail_maps[i].avail[byte] &= ~bit;
					hp->avail_maps[i].num_sectors--;
					return result;
				}
			}
	}
	printf("Disk Full !!\n");
	exit(-1);
}

/* return sector address of first free directory sector */
sector_address
find_free_directory_sector(dp)
register struct c64disk *dp;
{
	struct sector_address result;
	register i, j, bit, byte;
	register struct dosdir_hdr *hp;

	hp = get_dos_hdr(dp);
	if (hp->avail_maps[17].num_sectors)	/* some in this track ? */
		for (j=0, bit=1, byte=0; j<sectors_per_track[17];
			j++, bit*=2) {
			if (bit==256) {
				bit = 1;
				byte++;
			}
			if (hp->avail_maps[17].avail[byte] & bit) {
				/* got one */
				result.track = 18;
				result.sector = j;
				hp->avail_maps[17].avail[byte] &= ~bit;
				hp->avail_maps[17].num_sectors--;
				return result;
			}
		}
	printf("Directory Full !!\n");
	exit(-1);
}

writedisk(dp, name)
register struct c64disk *dp;
char *name;
{
	register fd = open(name, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	register i, j;

	for (i=0; i<35; i++)
		for (j=0; j<sectors_per_track[i]; j++)
			write(fd, dp->tracks[i].sectors[j], 256);
	close(fd);
}

struct c64disk *
readdisk(name)
{
	register struct c64disk *dp = allocdisk(name, 0x0101);
	register i, j, fd;

	fd = open(name, O_RDONLY);
	for (i=0; i<35; i++)
		for (j=0; j<sectors_per_track[i]; j++)
			read(fd, dp->tracks[i].sectors[j], 256);
	close(fd);
	return dp;
}

load_dos_file(dp, name)
register struct c64disk *dp;
char *name;
{
	register struct file_entry *fp;
	register struct dosdir_hdr *hp;
	register struct dosdir_sector *sp;
	register i, len;
	register u_char *p;
	sector_address sa;
	int num_blocks=0;
	int fd;

	sa.track = 18; sa.sector = 1;
	hp = (struct dosdir_hdr *)get_dos_hdr(dp);
	while (sa.track) {
		sp = (struct dosdir_sector *)get_sector(dp, sa);
		for (i=0, fp=sp->file_entries; i<8; i++, fp++)
			if (fp->type == 0x00) goto gotfile;
		sa = sp->next_dir_sector;
	}
	/* current directory sectors are all full */
	sa = find_free_directory_sector(dp);
	sp->next_dir_sector = sa;	/* link to old last sector */
	sp = (struct dosdir_sector *)get_sector(dp, sa);
	/* initialise this sector */
	sp->next_dir_sector.track = 0;
	sp->next_dir_sector.sector = 255;
	for (i=0; i<8; i++) sp->file_entries[i].type = 0x00;
	fp = &sp->file_entries[0];	/* first free entry */
gotfile :
	fp->type = 0x82;	/* PROG file */
	len = strlen(name);
	for (i=0; i<16; i++)
		if (i>=len) fp->filename[i]=160;
		else fp->filename[i]=cvt_to_cbm_ascii(name[i]);
	sa = find_free_sector(dp,0);	/* sector for first file */
	len = filelength(name);		/* length of file */
	fd = open(name, O_RDONLY);
	fp->data = sa;
	while (len > 254) {
		p = get_sector(dp, sa);		/* point to sector */
		sa = find_free_sector(dp,0);	/* next sector in file */
		p[0] = sa.track;		/* fix link to next sector in file */
		p[1] = sa.sector;
		read(fd, p+2, 254);		/* load this sector */
		len -= 254;
		num_blocks++;
	}
	p = get_sector(dp, sa);		/* point to last sector */
	p[0] = 0;					/* terminate */
	p[1] = len;
	read(fd, p+2, len);
	num_blocks++;
	fp->num_blocks_lo = num_blocks & 0xff;
	fp->num_blocks_hi = (num_blocks>>8) & 0xff;
}
