/****************************************************************************
 *
 * %W%
 * %G%
 *
 ****************************************************************************/

/* only define u_char if sys/types.h not already included */
#ifndef _TYPES_
typedef unsigned char u_char;
#endif
typedef u_char SECTOR[256];

#define get_sector(dp, sa)	(dp->tracks[sa.track-1].sectors[sa.sector])

typedef struct sector_address {
	u_char track;
	u_char sector;
} sector_address;

sector_address find_free_sector();

struct track {
	int num_sectors;			/* number of sectors in track */
	SECTOR *sectors;			/* points to array of sectors */
};

struct c64disk {
	struct track tracks[35];	/* note track numbers start at one,
								 * so for index, subtract one */
};

/* sector avaiability map for one track */
struct avail_map {
	u_char num_sectors;			/* number of sectors in track */
	u_char avail[3];			/* bit map */
};

struct dosdir_hdr {
	sector_address next;		/* =(18,1) */
	u_char is_A;				/* ='A' */
	u_char ds_flag;				/* ignored */
	struct avail_map avail_maps[35];	/* index = track num-1 */
	u_char dsk_name[16];		/* padded with 160s */
	u_char pad1[2];				/* =160s */
	u_char dsk_id[2];
	u_char pad2;				/* =160 */
	u_char is_2A[2];			/* ="2A" */
	u_char pad3[4];				/* =160s */
	u_char pad4[85];			/* =0s */
};

#define get_dos_hdr(dp)		((struct dosdir_hdr *)dp->tracks[17].sectors[0])

struct dosdir_sector {
	struct file_entry {
		sector_address pad;		/* for entry 0, next directory sector */
#define next_dir_sector		file_entries[0].pad
		u_char type;			/* type 0 = DELeted
								 *		1 = SEQuential
								 *		2 = PROGram
								 *		3 = USER
								 *		4 = RELative
								 * +$80 if properly closed file
								 * +$c0 if file locked */
		sector_address data;	/* address of data on disk */
		u_char filename[16];	/* padded with 160s */
		sector_address side;	/* only for REL */
		u_char record_length;	/* only for REL */
		u_char pad1[4];			/* unused */
		sector_address at;		/* used in @SAVE, @LOAD */
		u_char num_blocks_lo,	/* size of file */
			   num_blocks_hi;
	} file_entries[8];		/* info for each file in this sector */
};

struct c64disk *allocdisk(), *readdisk();

struct gamedir {
	struct entry {
		sector_address start;
		u_char num_sectors;
		u_char which;
	} entries[64];
};
