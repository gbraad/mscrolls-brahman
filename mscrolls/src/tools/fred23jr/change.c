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
#include "vfiles.h"
#include "defs.h"
#include "data.h"
#include "change.h"
#include "errors.h"
#include "in_word.h"
#include "utils.h"

extern char *command;
extern int lineno;
extern int type;
extern int wanted_negsets;
extern char id[];
extern char locname[];
extern char fixedname[];
extern char slname[];
extern char pseudos[][MAXIDLEN];
extern struct set sets[];
extern int set_no;
extern int pseudo_no;
extern int value;
extern FILE *fpseudos;
extern FILE *fnetwork;
extern FILE *fwtable11;
extern FILE *fsyns;
extern FILE *ftext;
extern FILE *fwtable24;
extern FILE *finfo24;
extern int support_money_property;

extern VFILE *vitemicons, *vroompictures;
extern char *valid_item_props[];

char *valid_textures[] = {
#include "textures.str"
	NULL
};
char *valid_sexes[] = {
	"male",
	"female",
	"neuter",
	NULL
};
char *valid_covers[] = {
#include "covertypes.str"
	NULL
};

static int is_num(char* s)
{
    char *p;
    for (p=s; *p; p++) if (!isdigit(*p)) return 0;
    return 1;
}

static int in_num(int max)
{
    char s[50];
    int i;

    in_word(s);
    if (!is_num(s)) syntax();
    i = atoi(s);
    if (i<0 || i>max) range(max);
    return i;
}

#if 0
static int yes_no()
{
    char s[20];
    in_word(s);
    if (!strcmp(s, "no")) return 0;
    if (!strcmp(s, "yes")) return 1;
    unrecognised(s);
}
#endif

void change_item_prop(struct data* ptr, int n)
{
    int i, count, c;
    struct set *setp;
    char s[100];
    int set_type;

    set_type=0;
    switch (n) {
    case ICON:
        in_word(s);
        Vprintf(vitemicons, "\tICON\t%s,%s\n", ucaseof(id), s);
        if (!support_money_property) ptr->has_icon_or_money = 1;
        else
            fprintf(stderr,"WARNING: use of icon, but money property enabled\n");
        break;
    case DESC :
        in_word(s);
        if (!ISOPENB(s)) expected("{");
        if ((c = in_char()) != '\n') {
            while (c != '}') {
                putc(c, ftext);
                c = in_char();
            }
            putc('\n', ftext);
        } else {
            i = 0;	/* flag	*/
            for (;;) {
                c = in_char();	/* skip spaces at beginning of line	*/
                while (isspace(c)) c = in_char();
                if (c == '}') {	/* finished ?	*/
                    putc('\n', ftext);
                    return;
                }
                if (i) putc(' ', ftext);	/* subst EOL's with ' '	*/
                else i++;
                while (c != '\n') {
                    putc(c, ftext);
                    c = in_char();
                }
            }
        }
        break;
    case PLURAL :
        ptr->plural = value;
        break;
    case DEAD :
        ptr->dead = value;
        break;
    case BROKEN :
        ptr->broken = value;
        break;
    case LOCAL_CONTENTS :
        ptr->local_contents = value;
        break;
    case OPEN :
        ptr->open = value;
        break;
    case CLOSED :
        ptr->closed = value;
        break;
    case LOCKED :
        ptr->locked = value;
        break;
    case LIT :
        ptr->lit = value;
        break;
    case STRENGTH :
        ptr->strength = in_num(15);
        break;
    case HARDNESS :
        ptr->hardness = in_num(15);
        break;
    case WEIGHT :
        ptr->weight = in_num(15);
        break;
    case SIZE :
        ptr->size = in_num(15);
        break;
    case TEXTURE :
        in_word(s);
        if (!(i = from_word(s, valid_textures))) unrecognised(s);
        ptr->texture = i - 1;
        break;
    case MOVEABLE :
        ptr->moveable = value;
        break;
    case WET :
        ptr->wet = value;
        break;
    case SHARPNESS :
        ptr->sharpness = in_num(3);
        break;
    case VALUE :
        /* ptr->value = in_num(7); */
        in_word(s);
        putvalue("\tDC.W\t");
        putvalue(s);
        putvalue(",N");
        putvalue(ucaseof(id));
        putvalue("\n");
        break;

    case DESC_ONLY:
        ptr->desc_only = value;
        break;

    case MODIFIED_QUANTITY:
        ptr->modified_quantity = value;
        break;

    case SHORT_LOOK:
        ptr->short_look = value;
        break;
    case MONEY :
        if (support_money_property)
            ptr->has_icon_or_money = value;
        else
        {
            fprintf(stderr,"WARNING: use of money property, use \"-money\" option\n");
        }
        break;
    case EXPLORED :
        ptr->explored = value;
        break;
    case NPC_REFUSES :
        ptr->npc_refuses = value;
        break;
    case IS_NPC :
        ptr->is_npc = value;
        break;
    case LIGHTABLE :
        ptr->lightable = value;
        break;
    case PSEUDO :
        ptr->pseudo = value;
        break;
    case INITIAL_DONE :
        ptr->initial_done = value;
        break;
    case LIT_ROOM :
        ptr->lit_room = value;
        break;
    case SITTABLE :
        ptr->sittable = value;
        break;
    case FORCE_ADJ :
        ptr->force_adj = value;
        if (value) ptr->another_noun = value;
        break;
    case FORCE_PLURAL :
        ptr->force_plural = value;
        break;
    case GIVEN_TO_NPC :
        ptr->given_to_npc = value;
        break;
    case ANOTHER_NOUN :
        ptr->another_noun = value;
        break;
    case RELATED :
        ptr->related = value;
        break;
    case HIDDEN :
        ptr->hidden = value;
        break;
    case WORN :
        ptr->worn_by = value;
        break;
    case INTRINSIC :
        ptr->intrinsic = value;
        break;
    case INSEPARABLE :
        ptr->inseparable = value;
        break;
    case CONTAINED :
        ptr->contained = value;
        break;
    case IS_A_ROOM :	/* should never happen - set by item type	*/
        ptr->is_a_room = value;
        break;
    case OUT :
        ptr->out = value;
        break;
    case SL :
        in_word(s);
        strcpy(slname,s);		/* set secondary locator up */
        ptr->sl = word_val(s);
        break;
    case LOCATION :
        if (ptr->pseudo) {
            /* input a list pseudo locations	*/
            fprintf(fpseudos, "\tPS_DEF\t%s\n", ucaseof(id));
            fprintf(fpseudos, "\tPS_CONT\t");
            in_word(s);
            if (ISOPENB(s)) {
                in_word(s);
                count = 0;
                while(!ISCLOSEB(s)) {
                    if (count)	/* count=0 for 1st on line	*/
                        putc(',', fpseudos);
                    fprintf(fpseudos, "%s", ucaseof(s));
                    in_word(s);
                    if (++count == 8 && !ISCLOSEB(s)) {
                        count = 0;
                        fprintf(fpseudos, "\n\tPS_CONT\t");
                    }
                }
            } else expected("{");
            fprintf(fpseudos, "\n\n");
            strcat(strcpy(locname, "P_"), ucaseof(id));
            strcpy(pseudos[pseudo_no++], locname);
        } else {
            in_word(s);
            if (ptr->related || ptr->hidden || ptr->worn_by ||
                ptr->inseparable || ptr->contained)
                strcat(strcpy(locname, "N"), ucaseof(s));
            else
                strcat(strcpy(locname, "RN"), ucaseof(s));
        }
        break;
    case CONTR_VOL :
        ptr->container_vol = in_num(7);
        break;
    case COVERS :
        in_word(s);
        if (ISOPENB(s)) {
            in_word(s);
            while (!ISCLOSEB(s)) {
                switch (from_word(s, valid_covers)) {
                case 0 :
                    unrecognised(s);
                    break;
                case CHEST :
                    ptr->cover_chest = 1;
                    break;
                case HEAD :
                    ptr->cover_head = 1;
                    break;
                case ARMS :
                    ptr->cover_arms = 1;
                    break;
                case GROIN :
                    ptr->cover_groin = 1;
                    break;
                case LEGS :
                    ptr->cover_legs = 1;
                    break;
                }
                in_word(s);
            }
        }
        break;
    case WORN_BY_NPC :
        ptr->worn_by_npc = value;
        break;
    case WEARABLE :
        ptr->wearable = in_num(3);
        break;
    case COMBUSTIBLE :
        ptr->combustible = in_num(3);
        break;
    case EDIBLE :
        ptr->edible = in_num(7);
        break;
    case FIXED_NUM :
        in_word(fixedname);
        break;
    case FIXED :
        ptr->fixed = 1;
        break;
    case SYNONYMS :
        in_word(s);
        if (!ISOPENB(s)) expected("{");
        do {
            in_word(s);
            if (!ISCLOSEB(s)) {
                do_wtable(s, fwtable11);
                fprintf(fsyns, "\tSYNMACRO\tN%s-1,SECT.NOUN\n",
                        ucaseof(id));
            }
        } while (!ISCLOSEB(s));
        break;
    case OWNER:
        set_type=TYPE_OWNERSHIP;
    case ATTRIBUTE:
        if (!set_type) set_type= TYPE_ATTRIBUTE;
    case NEGSET:
    case CLASS:
    case SET :
        if (!set_type) set_type=TYPE_SET;
        in_word(s);
        for (i=0, setp=sets; i<set_no; i++, setp++)
            if (!strcmp(s, setp->name)) break;
        if (i==set_no) {
            if (set_no == MAXNSETS) {
                toomanysets();
                break;
            }
            strcpy(setp->name, s);
            setp->num = 0;
            setp->type=set_type;
            if (n==CLASS) {
                setp->rootnum_flag=1;
            }
            if (n==NEGSET) {
                wanted_negsets = 1;
                setp->negflag=1;
            }
            else setp->negflag=0;
            set_no++;
        }
        if (setp->num == MAXSETSIZE)
            settoolarge();
        else
            strcat(strcpy(setp->elements[setp->num++], "N"), ucaseof(id));
        break;
	
    case SCEN_NOUN_A:
        in_word(s);
        do_wtable(s,fwtable24);
        fprintf(finfo24,"\tINFO24\t1,%s\n",ucaseof(id));
        break;
	
    case SCEN_NOUN_B:
        in_word(s);
        do_wtable(s,fwtable24);
        fprintf(finfo24,"\tINFO24\t2,%s\n",ucaseof(id));
        break;
	
    case SCEN_NOUN_C:
        in_word(s);
        do_wtable(s,fwtable24);
        fprintf(finfo24,"\tINFO24\t3,%s\n",ucaseof(id));
        break;

    case SCEN_NOUN_D:
        in_word(s);
        do_wtable(s,fwtable24);
        fprintf(finfo24,"\tINFO24\t4,%s\n",ucaseof(id));
        break;

    case SCEN_NOUN_E:
        in_word(s);
        do_wtable(s,fwtable24);
        fprintf(finfo24,"\tINFO24\t5,%s\n",ucaseof(id));
        break;

    case SCEN_NOUN_F:
        in_word(s);
        do_wtable(s,fwtable24);
        fprintf(finfo24,"\tINFO24\t6,%s\n",ucaseof(id));
        break;

    }
}

void change_clone_props(char* id)
{
    int i, c, n;
    struct set *setp;
    char s[100];
    int set_type;
    extern VFILE *vcdtext;

    set_type = 0;
    in_word(s);
    if (!ISOPENB(s)) expected("{");
    do {
	in_word(s);
	if (n=from_word(s,valid_item_props))
	    switch (n) {
	    case DESC :
		if (type==CLONE) {
		    fprintf(stderr,"use clonedesc for clones with descriptions\n");
		    break;
		}
		in_word(s);
		if (!ISOPENB(s)) expected("{");
		if ((c = in_char()) != '\n') {
		    while (c != '}') {
			vputc(c, vcdtext);
			c = in_char();
		    }
		    vputc('\n', vcdtext);
		} else {
		    i = 0;	/* flag	*/
		    for (;;) {
			c = in_char();	/* skip spaces at beginning of line	*/
			while (isspace(c)) c = in_char();
			if (c == '}') {	/* finished ?	*/
			    vputc('\n', vcdtext);
			    break;
			}
			if (i) vputc(' ', vcdtext);	/* subst EOL's with ' '	*/
			else i++;
			while (c != '\n') {
			    vputc(c, vcdtext);
			    c = in_char();
			}
		    }
		}
		break;

	    case SYNONYMS :
		in_word(s);
		if (!ISOPENB(s)) expected("{");
		do {
		    in_word(s);
		    if (!ISCLOSEB(s)) {
			do_wtable(s, fwtable11);
			fprintf(fsyns, "\tSYNMACRO\tN%s-1,SECT.NOUN\n",
				ucaseof(id));
		    }
		} while (!ISCLOSEB(s));
		*s=0;			/* lose } else main do is confused */
		break;
	    case OWNER:
		set_type=TYPE_OWNERSHIP;
	    case ATTRIBUTE:
		if (!set_type) set_type= TYPE_ATTRIBUTE;
	    case NEGSET:
	    case CLASS:
	    case SET :
		if (!set_type) set_type=TYPE_SET;
		in_word(s);
		for (i=0, setp=sets; i<set_no; i++, setp++)
		    if (!strcmp(s, setp->name)) break;
		if (i==set_no) {
		    if (set_no == MAXNSETS) {
			toomanysets();
			break;
		    }
		    strcpy(setp->name, s);
		    setp->num = 0;
		    setp->type=set_type;
		    if (n==CLASS) {
			setp->rootnum_flag=1;
		    }
		    if (n==NEGSET) {
			wanted_negsets = 1;
			setp->negflag=1;
		    }
		    else setp->negflag=0;
		    set_no++;
		}
		if (setp->num == MAXSETSIZE)
		    settoolarge();
		else
		    strcat(strcpy(setp->elements[setp->num++], "N"), ucaseof(id));
		break;
				
	    case SCEN_NOUN_A:
		in_word(s);
		do_wtable(s,fwtable24);
		fprintf(finfo24,"\tINFO24\t1,%s\n",ucaseof(id));
		break;
				
	    case SCEN_NOUN_B:
		in_word(s);
		do_wtable(s,fwtable24);
		fprintf(finfo24,"\tINFO24\t2,%s\n",ucaseof(id));
		break;
				
	    case SCEN_NOUN_C:
		in_word(s);
		do_wtable(s,fwtable24);
		fprintf(finfo24,"\tINFO24\t3,%s\n",ucaseof(id));
		break;
			
	    case SCEN_NOUN_D:
		in_word(s);
		do_wtable(s,fwtable24);
		fprintf(finfo24,"\tINFO24\t4,%s\n",ucaseof(id));
		break;
			
	    case SCEN_NOUN_E:
		in_word(s);
		do_wtable(s,fwtable24);
		fprintf(finfo24,"\tINFO24\t5,%s\n",ucaseof(id));
		break;
			
	    case SCEN_NOUN_F:
		in_word(s);
		do_wtable(s,fwtable24);
		fprintf(finfo24,"\tINFO24\t6,%s\n",ucaseof(id));
		break;
	
	    default:
		fprintf(stderr,"Data modifier not appropriate to a clone!\n");
	    }
    } while (!ISCLOSEB(s));
}	

char *valid_emotions[] = {
    "happy",
    "angry",
    "violent",
    "asleep",
    "unconscious",
    "peaceful",
    "excited",
    "friendly",
    "drunk",
    "sad",
    "tired",
    "suffering",
    "frightened",
    "cheerful",
    "mischievous",
    NULL
};

void change_npc_prop(struct npc* ptr, int n)
{
    int i;
    char s[100];

    switch(n) {
    case MOTIVATION :
        ptr->motivation = in_num(255);
        break;
    case NUM_QUESTIONS :
        ptr->num_questions = in_num(255);
        break;
    case SEX :
        in_word(s);
        if (!strcmp(s, "male")) ptr->b34.sex = 1;
        else if (!strcmp(s, "female")) ptr->b34.sex = 0;
        else unrecognised(s);
        break;
    case SUP_CLOTHING :
        ptr->b34.clothing = value;
        break;
    case ANIMAL :
        ptr->b34.animal = value;
        break;
    case ARTICLE :
        ptr->b34.article = value;
        break;
    case EMOTION :
        in_word(s);
        if (!(i = from_word(s, valid_emotions))) unrecognised(s);
        ptr->b34.emotion = i;
        break;
    case PAUSED:
        ptr->b34.paused = value;
        break;
    case NICE:
        ptr->b34.nice = value;
        break;
    case NONPAUSE:
        ptr->b34.nonpause = value;
        break;
    case QUIET:
        ptr->b34.quiet = value;
        break;
    case COVETOUS:
        ptr->b34.covetous = value;
        break;
    }
}

char *valid_noway[] = {
    "nway_wall",
    "nway_riverbank",
    "nway_trees",
    "nway_castlemoat",
    "nway_vanes",
    "nway_toll",
    "nway_shop",
    "nway_slope",
    "nway_death",
    "nway_river",
    "nway_lake",
    "nway_railings",
    "nway_door",
    "nway_pigeon",
    "nway_rockface",
    "nway_fridge",
    "nway_ironbars",
    "nway_misc1",
    "nway_misc2",
    "nway_misc3",
    "nway_misc4",
    "nway_misc5",
    "nway_misc6",
    "nway_smallwall",
    "nway_bridgewest",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    "nway_",
    NULL
};

char *valid_directions[] = {
    "north",
    "northeast",
    "east",
    "southeast",
    "south",
    "southwest",
    "west",
    "northwest",
    "up",
    "down",
    "in",
    "out",
    NULL
};

void change_room_prop(struct data* ptr, int n)
{
    int i;
    char s[100], s2[100];
    char dirns[12][30];

    switch(n) {

    case PICTURES:
        i = 0;
        Vprintf(vroompictures, "PIC_%s\tPIC_LIST\t", id);
        in_word(s);
        if (!ISOPENB(s)) expected("{");
        while (!ISCLOSEB(s)) {
            in_word(s);
            if (!ISCLOSEB(s)) {
                if (i++) Vprintf(vroompictures, ",%s", s);
                else Vprintf(vroompictures, "%s", s);
            }
            else Vprintf(vroompictures, "\n");
        }
        break;

    case NO_ARTICLE :
        ptr->no_article = value;
        break;
	
    case TOPLINE:
        {
            int tmp = in_num(255);
            ptr->hardness = tmp/16;		/* frig cos it's 2 4 bit fields */
            ptr->strength = tmp & 0x0F;	/* pretending in rooms to be 1 8 bit */
        }
        break;

    case NET :
        for (i=0; i<12; i++) dirns[i][0] = '\0';
        in_word(s);
        if (!ISOPENB(s)) expected("{");
        while (!ISCLOSEB(s)) {
            in_word(s);	/* direction word	*/
            if (!ISCLOSEB(s)) {
                if (!(i = from_word(s, valid_directions))) unrecognised(s);
                in_word(s2);	/* location to go to	*/
                strcpy(dirns[i-1], s2);
            }
        }
        fprintf(fnetwork, "\tDC.B\t");
        for (i=0; i<12; i++)
        {
            if (dirns[i][0])
            {
                if (!(from_word(dirns[i],valid_noway)) && 
                    strncmp("nway_",dirns[i],5) &&
                    strncmp("pway_",dirns[i],5)) 
                    fprintf(fnetwork, "RN%s", ucaseof(dirns[i]));
                else
                    fprintf(fnetwork, "%s", dirns[i]);
            }
            else putc('0', fnetwork);
            if (i<11) fprintf(fnetwork, ",");
        }
        fprintf(fnetwork, "\t; %s\n", id);
        break;
    }
}

