/**
 *
 * Copyright (c) 2017 Voidware Ltd.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code as
 * defined in and that are subject to the Voidware Public Source Licence version
 * 1.0 (the 'Licence'). You may not use this file except in compliance with the
 * Licence or with expressly written permission from Voidware.  Please obtain a
 * copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
 * it before using this file.
 *
 * THIS IS NOT FREE SOFTWARE.
 * 
 * The Original Code and all software distributed under the Licence are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Please see the Licence for the specific language governing rights and 
 * limitations under the Licence.
 *
 * contact@strandgames.com
 *
 *
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "data.h"
#include "vfiles.h"
#include "utils.h"
#include "in_word.h"
#include "errors.h"
#include "othertypes.h"
#include "change.h"
#include "os.h"

char *command;

int warnings = 0;	/* no warning messages	*/
int forceupdate = 0;	/* force file updates	*/
int eltformat = 0;		/* produce .i files instead of .asm */
int wanted_negsets = 0;
int wanted_attributes = 0;

int rooms[MAXNITEMS];	/* word numbers of each room	*/

int this_word;	/* word number of word fred23 is working on at the moment	*/
int	type;	/* type of the current object	*/
char id[MAXIDLEN];	/* identifier of current item	*/
char locname[MAXIDLEN];	/* name of location for this item	*/
char fixedname[MAXIDLEN];	/* name of location for this item	*/
char slname[MAXIDLEN];	/* name of secondary location for this item	*/
char *fredname;	/* name of the fred source file	*/
char *thetime;	/* time at which fred was invoked	*/
char pseudos[MAXNPSEUDOS][MAXIDLEN];	/* names of all the pseudos	*/
char verbtext[MAXVERBS][MAXVERBLEN];	/* a list of verbs */
char item[MAXNITEMS][MAXIDLEN];
char clonedescid_strings[MAXNITEMS/4][MAXIDLEN];
char cloneid_strings[MAXNITEMS/4][MAXIDLEN];
int verbnum=0;
char prefix_path[100];
struct set sets[MAXNSETS];
int support_money_property = 0;

char valuestring[MAXVALUESTRING];
char *pvaluestring;

int set_no = 0;
int pseudo_no=0;
int value;	/* value to be given to flag fields on the data	*/

/* output files	*/
FILE *fpseudos, *fnetwork, *froomequ, *fnounequ, *fadjequ,
    *fdata, *ftext, *fadjectiv, *fwtable5, *fwtable6, *fnounequ2, *fnpc,
    *fwtable11, *fsyns, *fwtable24, *finfo24, *fnpclist;

FILE* roomheader;

VFILE *vcloneadj, *vclonewtable6, *vcloneequ, *vclonedata;
VFILE *vclonedescadj, *vclonedescwtable6, *vclonedescequ,
      *vclonedescdata, *vcdtext, *vitemicons, *vroompictures, *vroompicttab;

extern int lineno;	/* extern vars maintained by in_word etc.	*/
extern char thisfile[];
extern int clonenum, clonedescnum;

/* valid section identifiers (for synonyms) */
char *valid_sections[] = {
#include "sections.str"
	NULL
};

/* valid types of object	*/
char *valid_types[] = {
#include "itemtypes.str"
	NULL
};

/* valid properties for a data block	*/
char *valid_item_props[] = {
#include "itemprops.str"
	NULL
};

/* properties for an npc	*/
char *valid_npc_props[] = {
#include "npcprops.str"
	NULL
};

/* properties for a room	*/
char *valid_room_props[] = {
#include "roomprops.str"
	NULL
};

struct data item_dflt, room_dflt, npc_dflt;

static void init_dflt_data()
{
    memset(&item_dflt, 0, sizeof(struct data));
    item_dflt.strength = 15;
    item_dflt.weight = 15;
    item_dflt.texture = 9;
    memset(&room_dflt, 0, sizeof(struct data));
    memset(&npc_dflt, 0, sizeof(struct data));
    npc_dflt.texture = 11;
    npc_dflt.size=5;
}

static void init_item(struct data* dp, int type)
{
    slname[0] = 0x00  ;			/* kill secondary locator */
    strcpy (fixedname,"inventory");
    switch(type) {
    case ITEM :
        *dp = item_dflt;
        break;
    case ROOM :
        *dp = room_dflt;
        break;
    case NPC :
        *dp = npc_dflt;
        break;
    }
}

struct npc npcinfo_dflt = {
	0,	/* motivation	*/
	0,	/* no_questions	*/
#ifdef BIG_ENDIAN_BITFIELDS
	{ 0, 0, 0, 1, 1 }	/* sex=male, suppress clothing	*/
#else
	{ 1, 1, 0, 0, 0 }
#endif
};

void init_npc(struct npc* np)
{
    *np = npcinfo_dflt;
}

void do_bitmap(FILE* fq, struct set* setp,
               int nounnum, int clonedescs, int clones)
{
    /* the cost of a list is too great... use a bit map for each item */
    int i,j;
    char bits[8];
    int bitno;
    int flag;
    bitno=0;
    fprintf(fq, "\tDC.W\t1\t;flag as bitmap\n");

    for (i=1;i<nounnum;i++) {
	flag=0;
	for (j=0;j<setp->num;j++) {
            if (!strcmp(ucaseof(setp->elements[j])+1,item[i])) {
                flag=1;
                break;
            }
        }
	bits[bitno++]=flag;
	if (bitno==8) {
            fprintf(fq,"\tDC.B	%%");
            for (j=7;j>=0;j--) fprintf(fq,"%d",bits[j]);
            putc('\n',fq);
            bitno=0;
        }
    }

    for (i=0;i<clonedescs;i++) {
	flag=0;
	for (j=0;j<setp->num;j++) {
            if (!strcmp(ucaseof(setp->elements[j])+1,clonedescid_strings[i])) {
                flag=1;
                break;
            }
        }
	bits[bitno++]=flag;
	if (bitno==8) {
            fprintf(fq,"\tDC.B	%%");
            for (j=7;j>=0;j--) fprintf(fq,"%d",bits[j]);
            putc('\n',fq);
            bitno=0;
        }
    }

    for (i=0;i<clones;i++) {
	flag=0;
	for (j=0;j<setp->num;j++) {
            if (!strcmp(ucaseof(setp->elements[j])+1,cloneid_strings[i])) {
                flag=1;
                break;
            }
        }
	bits[bitno++]=flag;
	if (bitno==8) {
            fprintf(fq,"\tDC.B	%%");
            for (j=7;j>=0;j--) fprintf(fq,"%d",bits[j]);
            putc('\n',fq);
            bitno=0;
        }
    }

    if (bitno<8) {
	while (bitno<8) bits[bitno++]=0;	/*finish list */
	fprintf(fq,"\tDC.B	%%");
	for (j=7;j>=0;j--) fprintf(fq,"%d",bits[j]);
	putc('\n',fq);
    }
    fprintf(fq,"\tALIGN\n");
}

int main(int argc, char** argv)
{
    int i, j, c, nounnum=1;
    int roomnum=1; /* current room number */
    char *p, *q;
    struct set *setp;
    char *r;
    int print_every = 0, counter = 0;
    struct data data;
    struct npc npcdata;
    char fname[128];	/* filename of source file	*/
    char s[100];
    FILE *fp, *fq,*fr;
    int owner_flag;
    int set_counter = 0;
    int num_npcs=0;

    pvaluestring = valuestring;

    command = argv[0];
    while (argc > 1 && argv[1][0] == '-')
    {
        if (isdigit(argv[1][1]))
            print_every = atoi(argv[1]+1);
        else
        {
            if (!strcmp(argv[1], "-money"))
            {
                support_money_property = 1;
            }
            else if (!strcmp(argv[1], "-roomsh"))
            {
                // request we make rooms.h for C files
                // file will be in last/rooms.h
                roomheader = fopen("rooms.h", "w");
                if (roomheader)
                {
                    // emit header
                    fprintf(roomheader, "#pragma once\n\n");
                }
                else
                {
                    fprintf(stderr, "WARNING: Can't create rooms.h\n");
                }
            }
            else
            {
                for (p = argv[1]+1; *p; p++)
                {
                    switch (*p)
                    {
                    case 'w':
                        warnings = 1;
                        break;
                    case 'f':
                        forceupdate = 1;
                        break;
                    case 'i':
                        eltformat=1;
                    }
                }
            }
        }
        argv++;
        argc--;
    }

    if (argc != 2)
    {
        fprintf(stderr, "Usage : %s [-w] [-f] [-i] [-money] [-roomsh] <file>\n", command);
        exit(-1);
    }
    
    p=strrchr(argv[1],'/');
    q=prefix_path;
    if (p)
    {
        r=argv[1];
        while (r<=p) *q++ = *r++;	/* copy prefix in (including '/') */
    }
    else *q=0;
		
    init_dflt_data();	/* initialise the default headers	*/
    fredname = argv[1];
    thetime = timenow_string();
    fpseudos = openasm("data2");	/* open a .i output file & add hdr	*/
    fnetwork = openasm("network");
    fnounequ = openi("nounequ1");
    froomequ = openi("roomequ");
    fadjequ = openi("adjequ");
    fdata = openasm("noundat1");
    fadjectiv = openasm("adjectiv");
    fwtable5 = openi("wtable5");
    fwtable6 = openi("wtable6");
    fnpc = openi("npcdata");
    fwtable11 = openi("wtable11noun");
    fsyns = openi("nounsyms");
    fwtable24 = openi("wtable24");
    finfo24 = openasm("info24");
    fnpclist = openi("npclist");

    vcloneadj = vopeni("cloneadj");
    vcloneequ = vopeni("cloneequ");
    vclonewtable6 = vopeni("clonewtable6");
    vclonedata = vopenasm("clonedata");
    vclonedescadj = vopeni("cdadj");
    vclonedescequ = vopeni("cdequ");
    vclonedescwtable6 = vopeni("cdwtable6");
    vclonedescdata = vopeni("cddata");
    vitemicons = vopenasm("itemicons");
    vroompictures = vopenasm("roompictures");
    vroompicttab = vopeni("roompicttab");

    if ((ftext = fopen("tmpguild.text", "wb")) == NULL)
        cantopen("tmpguild.text");
    
    vcdtext = vopen("tmpguild.text", "ab"); /* make sure closed after ftext */
    strcat(strcpy(fname, argv[1]), ".f23");
    fileopen(fname);
    while (in_word(s) != EOF)
    {
        type = from_word(s, valid_types);
        counter++;
        if (!type) {
            invalid_type(s);
            /* make it an ITEM so as to try to make sense of the rest	*/
            type = ITEM;
        }

	switch (type) {
        case (CLONE):
        case CLONEDESC:
            do_clone();
            break;

        case (SYNONYM):
            do_synonym();
            break;

        case (VERBS):
            do_verbs();
            break;

        case (ROOM):
        case (ITEM):
        case (NPC):
            {
                in_word(s);
                if (print_every && counter > print_every) {
                    counter = 0;
                    printf("%s...\n", s);
                }
                strcpy(id, s);
                in_word(s);
                if (!ISQUOTE(s)) expected("\"");
                p = s;
                while ((c = in_char()) != '"') *p++ = c;
                *p = '\0';
                p = s;
                i = 0;	/* using this as a flag	*/
                fprintf(fadjectiv, "* %s\n\tXDEF\tADJ.%s\nADJ.%s\n\tADJ\t",
                        id, ucaseof(id), ucaseof(id));
                while (q = strchr(p, ' '))
                {
                    char tempstring[100],*ts;
                    *q++ = '\0';
                    if (i) putc(',', fadjectiv);
                    else i++;
                    strcpy(tempstring,p);
                    ts=tempstring;
                    /* loose '-' in adj's identifiers */
                    while (*ts)
                    {
                        if ((*ts == '-') || (*ts == ' ') || (*ts==':') ||
                            (*ts == '\'') || (*ts == '.')) *ts = '_';
                        ts++;
                    }
				
                    fprintf(fadjectiv, "%s", tempstring);
                    do_adj(p);	/* make a new adjective if ness	*/
                    p = q;
                }
                putc('\n', fadjectiv);
                do_wtable(p, fwtable6);	/* do word table entry for this noun	*/
                in_word(s);
                if (!ISOPENB(s)) expected("{");
                fprintf(fnounequ, "N%s\tEQU\t%d\n", ucaseof(id), nounnum);
                strcpy(item[nounnum],ucaseof(id));
                /* initialise to defaults	*/
                init_item(&data, type);

                /* fudge id into word table as a synonym */
                {
                    char tempstring[128];
                    strcat(strcpy(tempstring, "#"),id);
                    fprintf(fwtable11," IFNE YCHEAT&VAX\n");
                    do_wtable(tempstring, fwtable11);
                    fprintf(fwtable11," ENDC\n");
                    fprintf(fsyns, "\tSYNMACRO2\tN%s-1,SECT.NOUN\n", ucaseof(id));
                }

                if (type == ROOM)
                {
                    data.is_a_room = 1;
                    fprintf(froomequ, "RN%s\tEQU\t%d\n", ucaseof(id), roomnum);
                    if (roomheader)
                    {
                        fprintf(roomheader, "#define RN%s %d\n",
                                ucaseof(id), roomnum);
                    }
                    strcat(strcpy(locname, "RN"), ucaseof(id));
                    ++roomnum;
                }
                if (type == NPC) {
                    data.is_npc = 1;
                    num_npcs++;
                    init_npc(&npcdata);
                    fprintf(fnpclist,"\tDC.W N%s\n",ucaseof(id));
                }	
                in_word(s);
                while (!ISCLOSEB(s)) {
                    if (ISPLING(s) || !strcmp(s, "not")) {
                        value = 0;
                        in_word(s);
                    } else value = 1;
                    if (i = from_word(s, valid_item_props))
                        change_item_prop(&data, i);
                    else if (!strcmp(s, "combustable")) {
                        warning("bad spelling - should be 'combustible'");
                        change_item_prop(&data, COMBUSTIBLE);
                    } else if (!strcmp(s, "inseperable")) {
                        warning("bad spelling - should be 'inseparable'");
                        change_item_prop(&data, INSEPARABLE);
                    } else if (i = from_word(s, valid_npc_props))
                        if (type == NPC)
                            change_npc_prop(&npcdata, i);
                        else
                            invalid_prop(s);
                    else if (i = from_word(s, valid_room_props))
                        if (type == ROOM)
                            change_room_prop(&data, i);
                        else
                            invalid_prop(s);
                    else
                        invalid_prop(s);
                    in_word(s);		/* get next word	*/
                }	/* done this item/room/npc etc.	*/
                output_data(&data, fdata);	/* output data for this item to file */
                if (type == NPC) output_npc_data(&npcdata, fnpc);
                if (type == ROOM) Vprintf(vroompicttab, "\tPIC_TAB\t%s\n", id);
                nounnum++;
            }	
            break;
        default:
            fprintf(stderr,"Waaaa weird type!!\n");
            break;
        }
    }		/* done file	*/

    fprintf(fnounequ, "NNOUNS\tEQU\t%d\n", nounnum);
    fprintf(froomequ, "NROOMS\tEQU\t%d\n", roomnum);
    Vprintf(vclonedescequ, "MAXCLONEDESC\tEQU\tNNOUNS+%d\n", clonedescnum);

    /* do pseudo XREF's	*/
    c = 0;	/* using as a count of how name printed on this line	*/
    for (i=0; i<pseudo_no; i++) {
        if (c == 0) fputs("\tXREF\t", fdata);
        fputs(pseudos[i], fdata);
        if (++c < 6 && i < pseudo_no-1) putc(',', fdata);
        else {
            putc('\n', fdata);
            c = 0;
        }
    }

    /* write all associated with value of objects */

    fprintf(fpseudos,"\n\tDC.B\t0\n\tALIGN\nValueTable\n%s",valuestring);

    fclose(ftext);

    closeasm(fpseudos, "data2",".asm");	/* add footer, check for update	*/
    closeasm(fnetwork, "network",".asm");
    closeasm(froomequ, "roomequ",".i");
    closeasm(fnounequ, "nounequ1",".i");
    closeasm(fadjequ, "adjequ",".i");
    closeasm(fdata, "noundat1",".asm");
    closeasm(fadjectiv, "adjectiv",".asm");
    closeasm(fwtable5, "wtable5",".i");
    closeasm(fwtable6, "wtable6",".i");
    closeasm(fnpc, "npcdata",".i");
    closeasm(fwtable11, "wtable11noun",".i");
    closeasm(fsyns, "nounsyms",".i");
    closeasm(fwtable24, "wtable24",".i");
    closeasm(finfo24, "info24",".asm");
    fprintf(fnpclist,"\tDC.W\t0\t;terminator\n");
    closeasm(fnpclist, "npclist",".i");
    if (roomheader) fclose(roomheader);

    vcloseasm(vcloneadj, "cloneadj", ".i");
    vcloseasm(vcloneequ, "cloneequ", ".i");
    vcloseasm(vclonewtable6, "clonewtable6", ".i");
    vcloseasm(vclonedata, "clonedata", ".asm");
    vcloseasm(vclonedescadj, "cdadj", ".i");
    vcloseasm(vclonedescequ, "cdequ", ".i");
    vcloseasm(vclonedescwtable6, "cdwtable6", ".i");
    vcloseasm(vclonedescdata, "cddata", ".i");
    vcloseasm(vroompictures, "roompictures", ".asm");
    vcloseasm(vroompicttab, "roompicttab", ".i");
    vcloseasm(vitemicons, "itemicons", ".asm");

    fnounequ2 = openi("nounequ2");
    fprintf(fnounequ2, "NOUNNUM\tEQU\t%d\n", nounnum-1);
    fprintf(fnounequ2, "NMAXNOUN\tEQU\t%d\n", nounnum-1);
    fprintf(fnounequ2, "NPC.Number\tEQU\t%d\n",num_npcs);
    fprintf(fnounequ2, "NMAXCLONEDESC\tEQU\tNMAXNOUN+%d\n", clonedescnum);
    closeasm(fnounequ2, "nounequ2",".i");

    /* output the sets	*/
    fp = openi("wtable18");
    fq = openi("sets");
    fr = openi("scenequ2");
    fprintf(fq,"\tXDEF\tSETS.TBL\nSETS.TBL\n");

    j = 0;
    
    for (i=0, setp=sets; i<set_no; i++, setp++)
    {
        if (setp->type==TYPE_SET) {
            do_wtable(setp->name, fp);
            fprintf(fq,"\tXDEF\tDenotation_%s\n",ucaseof(setp->name));
            fprintf(fq,"Denotation_%s\n",ucaseof(setp->name));
            fprintf(fr,"attribute_%s\tEQU\tTRUE\n",ucaseof(setp->name));
            fprintf(fr,"set_%s\tEQU\t%d\n",ucaseof(setp->name),++set_counter);
            fprintf(fq, "\tDC.W\t");
            if (setp->negflag || setp->rootnum_flag) {
                fprintf(fq, "%s", setp->elements[0]);
                j=1;
            }
            if (setp->negflag) fprintf(fq,"+$8000");
            if (setp->rootnum_flag) fprintf(fq,"+$4000");
            if (setp->negflag || setp->rootnum_flag) fprintf(fq,",");
            else j=0;
            for (;j<setp->num; j++)
                fprintf(fq, "%s,", setp->elements[j]);
            fputs("0\n", fq);
        }
    }
    closeasm(fp, "wtable18",".i");
    
    /* now the attributes into sets.i	*/
    set_counter=0;
    fprintf(fq,"\tDC.W\t0\t\t; Terminate sets\n");
    for (i=0, setp=sets; i<set_no; i++, setp++)
    {
        if (setp->type==TYPE_ATTRIBUTE)
        {
            wanted_attributes=1;
            fprintf(fq,"\tXDEF\tDenotation_%s\n",ucaseof(setp->name));
            fprintf(fq,"Denotation_%s\n",ucaseof(setp->name));
            fprintf(fr,"attribute_%s\tEQU\tTRUE\n",ucaseof(setp->name));
            if ((setp->num+1)*2>(((int)((nounnum+clonenum+clonedescnum-1)/8))+1)) do_bitmap(fq,setp,nounnum,clonedescnum,clonedescnum);
            else {
                fprintf(fq, "\tDC.W\t0\t; code for simple list\n");
                fprintf(fq, "\tDC.W\t");
                for (j=0; j<setp->num; j++)
                    fprintf(fq, "%s,", setp->elements[j]);
                fputs("0\n", fq);
            }
        }
    }
    /* now the ownership attributes */

    owner_flag=0;
    fprintf(fq,"\tXDEF\tTBL.OWNERSHIP\n\n");
    fprintf(fq,"TBL.OWNERSHIP\n");
    for (i=0, setp=sets; i<set_no; i++, setp++) {
        if (setp->type==TYPE_OWNERSHIP) {
            owner_flag=1;
            fprintf(fq,"\tDC.W\tN%s\t;the %s\n",ucaseof(setp->name),setp->name);
            fprintf(fq, "\tDC.W\t");
            for (j=0; j<setp->num; j++)
                fprintf(fq, "%s,", setp->elements[j]);
            fputs("0\n", fq);
        }
    }
    fprintf(fq,"\tDC.W\t0\t;end of ownership table\n");
    if (owner_flag) fprintf(fr,"wanted_OWNERSHIP\tEQU\tTRUE\n");
    if (wanted_negsets) fprintf(fr,"wanted_NEGSETS\tEQU\tTRUE\n");
    if (wanted_attributes) fprintf(fr,"wanted_ATTRIBUTES\tEQU\tTRUE\n");
    if (clonenum!=0) fprintf(fr,"wanted_CLONES\tEQU\tTRUE\n");
    closeasm(fq, "sets",".i");
    closeasm(fr,"scenequ2",".i");

    write_verbs();		/* output verb info */
    fclose(ftext);
    vclose(vcdtext);		/* appended to tmpguild.text */
}

