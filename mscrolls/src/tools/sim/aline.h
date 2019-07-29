/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
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
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for posterity and
 * historical reference.
 */

#pragma once

void get_text(char* textname);
void do_TEXT();
void do_SETNOUNS();
void do_GETDATA();
void do_WORDSCAN();
void do_NOUNSCAN();
void do_GETNOUNS();
void do_CHAROUT();
void do_CHAROUT2();
void do_LOADFILE();
void do_SAVEFILE();
void do_RANDOM1();
void do_RANDOM2();
void do_CHARIN();
void do_QCARRIED();
void do_SEARCH();
void do_GETTIED();
void do_FINDZERO();
void do_GRAPHICS();
void do_PRINTER();
void do_RESTART();
void do_QUIT();
void do_UNDO_DIFF();
void do_MAKE_DIFF();

#define	SEARCH 0xA0FF
#define	GETDATA 0xA0FE
#define	SETNOUNS 0xA0FD
#define	NOUNSCAN 0xA0FC
#define	WORDSCAN 0xA0FB
#define	GETNOUNS 0xA0FA
#define	QCARRIED 0xA0F9
#define	TEXT 0xA0F8
#define	RANDOM1 0xA0F7
#define	RANDOM2 0xA0F6
#define	LOADFILE 0xA0F5
#define	SAVEFILE 0xA0F4
#define	CHAROUT2 0xA0F3
#define GETTIED 0xA0F2
#define FINDZERO 0xA0F1
#define GRAPHICS 0xA0F0
#define PRINTER 0xA0EF
#define RESTART 0xA0EE
#define QUIT 0xA0ED
#define GETWTAB 0xA0EC
#define PUTWTAB 0xA0EB
#define PRINTWORD 0xA0EA
#define COPYWORD 0xA0E9
#define SETNE	0xa0e8
#define SETEQ	0xa0e7
#define SETNE_RET	0xa0e6
#define SETEQ_RET	0xa0e5
#define POPSP_RET	0xa0e4
#define MAKE_DIFF	0xa0e3
#define UNDO_DIFF	0xa0e2
#define GET_NEXT_EV	0xa0e1
#define PROMPT_EV	0xa0e0
#define NOTIFY	0xa0df


#define RET 0xA200
#define CALL 0xA400
#define TEST 0xA600
#define PUSHM 0xA800
#define PULLM 0xAA00

#define	CHAROUT 0xF000
#define A_line 0xA000
#define F_line 0xF000
