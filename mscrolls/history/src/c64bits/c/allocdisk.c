/****************************************************************************
 *
 * %W%
 * %G%
 *
 ****************************************************************************/

#include "c64disk.h"
#include "alloc.h"

int sectors_per_track[] = {
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 
	19, 19, 19, 19, 19, 19, 19, 
	18, 18, 18, 18, 18, 18, 
	17, 17, 17, 17, 17
};

struct c64disk *
allocdisk(diskname, diskid)
char *diskname;
char *diskid;
{
	register i, j;
	register struct c64disk *dp = alloc(struct c64disk);
	register struct track *tp;
	register struct dosdir_hdr *hp;
	register struct dosdir_sector *dsp;
	register l;

	/* allocate the sectors */
	for (i=0, tp= &dp->tracks[0]; i<35; i++, tp++) {
		tp->num_sectors = sectors_per_track[i];
		tp->sectors = (SECTOR *)zmalloc(sectors_per_track[i] *
			sizeof(SECTOR));
	}

	/* fix the dos directory header */
	hp = (struct dosdir_hdr *)dp->tracks[17].sectors[0];
	hp->next.track = 18;
	hp->next.sector = 1;
	hp->is_A = 'A';

	/* sector availability */
	for (i=0; i<35; i++) {
		hp->avail_maps[i].num_sectors = sectors_per_track[i];
		hp->avail_maps[i].avail[0] = 255;	/* all available */
		hp->avail_maps[i].avail[1] = 255;
		hp->avail_maps[i].avail[2] = (1<<(sectors_per_track[i]-16))-1;
	}
	/* dos directory track not available */
	hp->avail_maps[17].num_sectors = 17;	/* 17 free directory sectors */
	hp->avail_maps[17].avail[0] = 0xfc;		/* first two used */
	hp->avail_maps[17].avail[1] = 0xff;
	hp->avail_maps[17].avail[2] = 7;

	l = strlen(diskname);
	for (i=0; i<16; i++)
		if (i<l) hp->dsk_name[i] = cvt_to_cbm_ascii(diskname[i]);
		else hp->dsk_name[i] = 160;
	
	hp->pad1[0] = hp->pad1[1] = 160;

	hp->dsk_id[0] = diskid[0];
	hp->dsk_id[1] = diskid[1];

	hp->pad2 = 160;

	hp->is_2A[0] = '2';
	hp->is_2A[1] = 'A';

	hp->pad3[0] = hp->pad3[1] = hp->pad3[2] = 160;

	for (i=0; i<85; i++) hp->pad4[i] = 0;

	/* initialise the first directory sector */
	dsp = (struct dosdir_sector *)dp->tracks[17].sectors[1];
	/* this is the last directory sector at the moment */
	dsp->next_dir_sector.track = 0;
	dsp->next_dir_sector.sector = 255;
	/* set each file as deleted */
	for (i=0; i<8; i++) dsp->file_entries[i].type = 0x00;

	return dp;
}
