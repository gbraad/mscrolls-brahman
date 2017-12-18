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
 */

/* This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

#pragma once

#include "symbols.h"
#include "expr.h"

#define T_LABEL		257
#define T_OPCODE	258
#define T_SIZE		259
#define T_NUMBER	260
#define T_AREG		261
#define T_DREG		262
#define T_PC		263
#define T_CCR		264
#define T_SR		265
#define T_USP		266
#define T_STRING	267
#define T_CODE		268
#define T_DATA		269
#define T_BSS		270
#define T_ASCII		271
#define T_LSHIFT	272
#define T_RSHIFT	273
#define T_NARG		274
#define T_CONTROL	275

typedef union
{
	struct symbol *symbol;
	struct expr expr;
	char *string;
	inttype number;
} YYSTYPE;

extern YYSTYPE yylval;
