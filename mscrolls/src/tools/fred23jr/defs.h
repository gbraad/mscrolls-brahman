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

#include "itemtypes.h"
#include "itemprops.h"
#include "covertypes.h"
#include "textures.h"
#include "npcprops.h"
#include "roomprops.h"
#include "sections.h"

/* macros	*/
#define ISOPENB(s)	(*(s) == '{')
#define ISCLOSEB(s)	(*(s) == '}')
#define ISQUOTE(s)	(*(s) == '"')
#define ISPLING(s)	(*(s) == '!')

/* constants	*/
#define MAXNITEMS 3000
#define MAXNNPCS 100
#define MAXNROOMS 1000
#define MAXNWORDS 2000
#define MAXNMESGS 2000
#define MAXNPSEUDOS 100
#define MAXNSETS 200
#define MAXSETSIZE 200
#define MAXIDLEN 30	
#define MAXVALUESTRING 4096
#define MAXVERBS	250
#define MAXVERBLEN	32

#define TYPE_SET		1
#define TYPE_ATTRIBUTE	2
#define TYPE_OWNERSHIP	3

#define FNAME_SZ 64

#define _PATH_MAX  255
