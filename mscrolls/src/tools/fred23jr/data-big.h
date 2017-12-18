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
/* big endian bitfield version	*/

#define BIG_ENDIAN_BITFIELDS

struct data {
	unsigned char
		/* states	*/
		lit	:1,
		locked	:1,
		closed	:1,
		open	:1,
		local_contents	:1,
		broken	:1,
		dead	:1,
		plural	:1,	
		/* physical characteristics	*/
		hardness	:4,
		strength	:4,
		/* size and weight	*/
		size		:4,
		weight		:4,
		/* shape and texture	*/
		sharpness	:2,
		wet		:1,
		moveable	:1,
		texture		:4,
		/* data1	*/
		lightable	:1,
		is_npc		:1,
		npc_refuses	:1,
		explored	:1,
/*		money		:1,			*/
		has_icon	:1,
		short_look	:1,
		desc_only	:1,
		modified_quantity	:1,
		/* data2	*/
		another_noun	:1,
		given_to_npc	:1,
		force_plural	:1,
		force_adj	:1,
		sittable	:1,
		lit_room	:1,
		initial_done	:1,
		pseudo	:1,
		/* location head	*/
		out		:1,
		is_a_room	:1,
		contained	:1,
		inseparable	:1,
		intrinsic	:1,
		worn_by		:1,
		hidden		:1,
		related		:1,
		/* secondary locator	*/
		sl		:8;
		/* location	*/
	unsigned short
		location;
		/* data3	*/
	unsigned char
		cover_legs	:1,
		cover_groin	:1,
		cover_arms	:1,
		cover_head	:1,
		cover_chest	:1,
		container_vol	:3,
#define no_article cover_legs
/* this bit used by rooms to suppress article */
		/* data4	*/
		edible		:3,
		combustible	:2,
		wearable	:2,
		worn_by_npc	:1;
		/* fixtures	*/
	unsigned short
		fixed		:1,
		spare		:1,
		fixed_num	:14;
};

struct npc {
	unsigned char motivation;
	unsigned char num_questions;
	struct {
		unsigned char
			emotion		: 4,
			article		: 1,
			animal		: 1,
			clothing	: 1,
			sex		: 1;
		unsigned char
			undef3		:3,
			covetous	:1,
			quiet		:1,
			nonpause	:1,
			nice		:1,
			paused		:1;
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
