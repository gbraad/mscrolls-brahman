* 
* 
*     _    __        _      __                           
*    | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
*    | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
*    | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
*    |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
*                                                        
*  Copyright (c) 2016-2017 Voidware Ltd.  All Rights Reserved.
* 
*  This file contains Original Code and/or Modifications of Original Code as
*  defined in and that are subject to the Voidware Public Source Licence version
*  1.0 (the 'Licence'). You may not use this file except in compliance with the
*  Licence or with expressly written permission from Voidware.  Please obtain a
*  copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
*  it before using this file.
* 
*  THIS IS NOT FREE SOFTWARE.
*  
*  The Original Code and all software distributed under the Licence are
*  distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
*  OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
*  WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
*  PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
* 
*  Please see the Licence for the specific language governing rights and 
*  limitations under the Licence.
* 
*  contact@strandgames.com
* 
*  This software was originally written and owned By Magnetic Scrolls Ltd.
*  It is being released under a Public Source Licence for historical reference
*  and educational purposes.
*


******************************************************
*
*	@(#)searcher.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.SEARCHER
*
*--------------------------------

	include	"equates.asm"
	include "macros.asm"

	XDEF    SEARCHER

	XREF    SEARCH,SEARCH2,MATCHBUF,ADJBUF,LIST.N

	IFEQ	THE_PAWN

	XREF	Section24

	ENDC


SEARCHER
	
*ENTRY:
* A6= START OF WRD
	
	DO	SEARCH
	LEA     MATCHBUF(A4),A0
	CMP.L   A0,A2             ;ANY?
	BNE.S   99$               ;EXIT, YES
	TEST_W  ADJBUF(A4)        ;ANY ADJECTIVES?
	BNE.S   99$               ;IF SO, MUST BE CORRECT WORD
	MOVE.L  A5,A0             ;A5 IS START OF WRD  NOW
	
	MOVEQ   #SLIM-1,D4        ;S LIMIT
05$
	MOVE.B  (A0)+,D1          ;GET CHR
	BEQ.S   10$               ;END
	CMP.B   #39,D1		 ; Apostrophe?
	BEQ.S   10$
	CMP.B   #' ',D1           ;SPACE?
	BEQ.S   10$
	SUBQ.B  #1,D4
	BRA.S   05$ 
	
10$
	TEST_B  D4
	BPL.S   30$               ;NOT ENOUGH, EXIT
	LEA     LIST.N(A4),A3     ;START OF TABLE
	MOVEQ   #SECT.NOUN,D6     ;SET UP SECTION
	MOVE.B  -2(A0),D1
	CMP.B   #'S',D1
	BEQ.S   20$
	CMP.B   #'s',D1
	BEQ.S   20$
	MOVE.B  (A0),-(A7)        ;SAVE CHR FOLLOWING
	MOVE.B  #' ',(A0)         ;BLAT WI A SPACE
	MOVE.B  -(A0),-(A7)       ;SAVE TERMINATOR
	MOVE.B  #'S',(A0)         ;SIMULATE PLURAL
	MOVE.L  A0,-(A7)          ;SAVE END ADDR
	MOVE.L  A5,A6             ;SET BACK TO BEGINNINING
	DO	SEARCH2           ;OK, ANY
	MOVE.L  (A7)+,A0          ;OLD END
	MOVE.B  (A7)+,(A0)+
	MOVE.B  (A7)+,(A0)
	BRA.S   30$               ;IF NONE, THEN TOO BAD..
20$
	SUBQ.L  #2,A0
	MOVE.B  (A0),-(A7)        ;SAVE 'S'
	MOVE.B  #' ',(A0)         ;BLAT LAST S
	MOVE.L  A0,-(A7)
	MOVE.L  A5,A6             ;OLD
	DO	SEARCH2
	MOVE.L  (A7)+,A0
	MOVE.B  (A7)+,(A0)        ;REPLACE S
30$
	IFEQ	THE_PAWN

	LEA	MATCHBUF(A4),A0	 ;have we found ANY matches yet?
	CMP.L	A0,A2
	BNE.S	99$               ;EXIT, YES
	LEA	Section24(A4),A3	;pointer to text of section to try
	MOVEQ	#SECT.SCEN,D6		;section # to start at
	MOVE.L	A5,A6
	DO	SEARCH2			;build matchbuf

	ENDC
99$
	RET

*--------------------------------

	END

