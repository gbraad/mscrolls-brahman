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

/* remember line number information for GDB */

#include "line.h"
#include "alloc.h"

struct line_chunk *first_line_chunk = 0, *last_line_chunk;
inttype n_line_numbers=0;		/* total number of line number entries */

void add_line_number(int line, long pc)
{
    if (first_line_chunk == 0) {
	first_line_chunk = last_line_chunk = alloc(struct line_chunk);
	first_line_chunk->next = 0;
	first_line_chunk->n_lines == 0;
    }
    if (last_line_chunk->n_lines == LINE_CHUNK_SIZE) {
	/* time for a new chunk */
	last_line_chunk->next = alloc(struct line_chunk);
	last_line_chunk = last_line_chunk->next;
	last_line_chunk->next = 0;
	last_line_chunk->n_lines = 0;
    }
    last_line_chunk->line_numbers[last_line_chunk->n_lines] = line;
    last_line_chunk->line_addresses[last_line_chunk->n_lines] = pc;
    last_line_chunk->n_lines ++;
    n_line_numbers++;		/* makes it easier writing the symbols out */
}
