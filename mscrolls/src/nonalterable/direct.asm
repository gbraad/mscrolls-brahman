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
*	@(#)direct.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.DIRECTION
*
*--------------------------------

	include	"equates.asm"
	include "macros.asm"

	XREF   VERB,VLIST.GO,VLEN.GO,CALCNET,SETMODE,SCREEN,P.SUB
	XREF   W.CGTW,LINE,LIGHT,W.ONLY,P.EXITS,MLISTNGO,P.SCN
	XREF   W.CSTFH,EXTRA,EXTRAPNT,GETADJPH,LastDirection

	XDEF   GO

*--------------------------------
*
* GO - CALLED IF A DIRECTION
*      (SECTION 7) IS FOUND BY
*      THE PARSER. SUSSES IF       !!!!!! must preserve A3
*      THE VERB (IF ANY) CAN       !!!!!! do to PARVERB
*      HAVE A DIR_ PROCESSES
*      ACORDINGLY
*
*--------------------------------
 
GO
 
	MOVE.W  VERB(A4),D0
*	CMP.W   #VNLOOK,D0         ;LOOK?  VNLOOK = 0
	BEQ     50$                ;CANNOT SEE THAT FAR FROM HERE
	LEA     VLIST.GO(A4),A0    ;VERB ALLOWED ?     
	MOVE.W  #VLEN.GO,D1        ;# OF ALLOWED VERBS (-1) URK!!!!! DR!!
10$
	CMP.B   (A0)+,D0           ;IN LIST?
	DBEQ    D1,10$             ;LOOP 'TIL FOUND OR END OF LIST
	BNE.S   90$                ;NO GOOD - NOT IN LIST
	
	DO	CALCNET            ;GET EXITS LIST FOR THIS ROOM -> A0 
	MOVE.B  0(A0,D5.W),D2
	BEQ.S   30$                ;NO EXIT

      IFEQ     THE_PAWN

	CMP.B   #MAXROOM,D2
	BHI     40$

      ENDC


      IFNE     THE_PAWN

	BMI.S   40$                ;NO WAY - BUT SPECIAL MSG

      ENDC

	MOVEQ   #0,D1              ;COS ONLY A BYTE!
	MOVE.B  D2,D1              ;NEW ROOM NO.

	MOVE.W	D5,LastDirection(A4)
 
	CALL    SETMODE            ;TEST SPECIALS - SNOWMAN TYPE & change room#

*
*  Due to PARVERB and other nasties, we may really be part way through a
*  command line here. So lets setup EXTRA and EXTRAPNT in case
*
 
	ST      EXTRA(A4)
	MOVE.L  A6,EXTRAPNT(A4)

	DO	GETADJPH             ;was there a noun after the dir?
	CMP.W   #SECT.NOUN,D6
	BNE.S   20$               ;no, ok
	MOVE.L  A6,EXTRAPNT(A4)   ;yes, lose it

20$
	PULL_L  D0/D1
	DOGO	SCREEN

30$
	DO	P.SUB	             ;YOU CANNOT GO THAT WAY ONLY...
	MSG	CGTW
	MSG    LINE
	TEST_W  LIGHT(A4)          ;DARK ROOM= NO LIST OF EXITS
	BEQ.S   20$
	MSG	ONLY
	CALL    P.EXITS
	BRA.S   20$                ;IF ERR STOP PARSING
40$
	CLR.L   D1
	MOVE.B  D2,D1
	NEG.B   D1
	LSL.W   #1,D1
	LEA     MLISTNGO(PC),A1
	ADD.L   D1,A1
	MOVE.W  (A1),D1
	DO	P.SCN
	BRA.S   20$ 
50$
	MSG	CSTFH          ;CANNOT SEE THAT FAR FROM HERE
	BRA.S   20$    
 
90$
	RET

	END


