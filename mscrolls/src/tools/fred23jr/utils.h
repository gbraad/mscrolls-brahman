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

#include <stdio.h>
#include "vfiles.h"

// forward
struct data;
struct npc;

int word_val(char* s);
int from_word(char* s, char** p);
void cantopen(char* name);
char *ucaseof(char* s);
void output_data(struct data* dp, FILE* fp);
void output_npc_data(struct npc* np, FILE* fp);
void print_hexb(int n, FILE* fp);
void do_adj(char* s);
void vdo_wtable(char* s, VFILE* fp);
void do_wtable(char* s, FILE* fp);
FILE *openasm(char* name);
VFILE *vopenasm(char* name);
VFILE *vopeni(char* name);
FILE *openi(char* name);
void vcloseasm(VFILE* fp, char* name, char* ending);
void closeasm(FILE* fp, char* name, char* ending);
void vcopy_file(char* from, VFILE* to);
void copy_file(char* from, FILE* to);
void putvalue(char* string);
void copy_file_to(char* from, char* name);
