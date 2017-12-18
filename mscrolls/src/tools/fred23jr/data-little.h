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

#pragma once

/* structure of the data	*/
/* little endian bitfield version */

/* #undef BIG_ENDIAN_BITFIELDS */

struct data {
	unsigned short
		/* states	*/
		plural	:1,	
		dead	:1,
		broken	:1,
		local_contents	:1,
		open	:1,
		closed	:1,
		locked	:1,
		lit		:1,
		/* physical characteristics	*/
		strength	:4,
		hardness	:4,
		/* size and weight	*/
		weight	:4,
		size	:4,
		/* shape and texture	*/
		texture	:4,
		moveable	:1,
		wet		:1,
		sharpness	:2,
		/* data1	*/
		modified_quantity	:1,
		desc_only	:1,
		short_look	:1,
/*		money	:1,				*/
		has_icon	:1,
		explored	:1,
		npc_refuses	:1,
		is_npc	:1,
		lightable	:1,
		/* data2	*/
		pseudo	:1,
		initial_done	:1,
		lit_room	:1,
		sittable	:1,
		force_adj	:1,
		force_plural	:1,
		given_to_npc	:1,
		another_noun	:1,
		/* location head	*/
		related	:1,
		hidden	:1,
		worn_by	:1,
		intrinsic	:1,
		inseparable	:1,
		contained	:1,
		is_a_room	:1,
		out		:1,
		/* secondary locator	*/
		sl		:8,
		/* location	*/
		location	:16,
		/* data3	*/
		container_vol	:3,
		cover_chest	:1,
		cover_head	:1,
		cover_arms	:1,
		cover_groin	:1,
		cover_legs	:1,
#define no_article cover_legs
/* this bit used by rooms to suppress article */
		/* data4	*/
		worn_by_npc	:1,
		wearable	:2,
		combustible	:2,
		edible	:3,
		/* fixtures	*/
		fixed_num	:14,
		spare	:1,
		fixed	:1;
};

struct npc {
	unsigned char motivation;
	unsigned char num_questions;
	struct {
		unsigned char
			sex		: 1,
			clothing	: 1,
			animal		: 1,
			article		: 1,
			emotion		: 4;
		unsigned char
			paused		:1,
			nice		:1,
			nonpause	:1,
			quiet		:1,
			covetous	:1,
			undef3		:3;
	} b34;
};

struct set {
	char name[MAXIDLEN];
	int num;
	char type;
	char elements[MAXSETSIZE][MAXIDLEN];
	char negflag;
	char rootnum_flag;
};
