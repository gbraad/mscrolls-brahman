/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2016-2018 Voidware Ltd.  All Rights Reserved.
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
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for historical reference
 * and educational purposes.
 */

#pragma once

struct bpcond {
	unsigned int count : 16;
	unsigned int size : 4;
	unsigned int cond : 4;
	unsigned int regnum : 8;
	long value;				/* note value is signed */
};

#define MAXBKPTS	20

extern int bkpt[MAXBKPTS];
struct bpcond bkpt_cond[MAXBKPTS];
extern int n_bkpts;

void init_bkpts();
void set_bkpt(int ad, int count, int regnum, int size, int cond, int value);
void clear_bkpt(int n);
void list_bkpts();
void init_mbkpts();
void set_mbkpt(int ad, int count, int regnum, int size, int cond, int value);
void clear_mbkpt(int n);
void list_mbkpts();
void printcond(struct bpcond cond);
int check_bkpt(struct bpcond* cond);

