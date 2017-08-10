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
*	@(#)read.asm	2.9
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*      SRC.ADV.READ
*
*--------------------------------

	include	"equates.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"macros.asm"
	include	"verbequ.asm"

      XREF     CONJ,W.IDFY,YCANT,W.BECAUS,P.PRN,ISARE,W.CLOSED,SP.READ,BSFULLSTOP
      XREF     W.MINDRD,W.NTREAD,P.SCN,P.DESC,DO.SCN,MessageFromTable

	IfVerb	READ

	XDEF     V.READ

V.READ
 
	CMP.W   #-1,CONJ(A4)
	BEQ.S	0101$
	MSGGO	IDFY 
0101$
	AND.B   #$60,D1            ;CANNOT READ IF LOCKED OR CLOSED
	BEQ.S   10$
	DO	YCANT
	MSG	BECAUS
	DO	P.PRN
	DO	ISARE
	MSG	CLOSED
	BRA	BSFULLSTOP
10$
	CALL    SP.READ
	BTST    #6,4(A0)           ;NPC?
	BEQ.S	0102$
	MSGGO	MINDRD
0102$
 
*--------------------------------

      IFEQ	THE_PAWN

	XREF	TBL.READ

	LEA	TBL.READ(A4),A1
	CALL	MessageFromTable	;Do any message
	BNE.S	0103$
	MSGGO	NTREAD			;if none, then can't read that
0103$
	RET  

      ENDC

      IFNE	THE_PAWN

	LEA     TBL.READ(PC),A1
30$
	CMP.W   (A1)+,D0
	BEQ.S   40$
	TEST_W  (A1)+
	BNE.S   30$
 
  
	CMP.W   #NTOMES,D0
	BEQ     READ.TOMES

*--------------------------------
 
	MSGGO	NTREAD
40$
	MOVE.W  (A1),D1
	BPL     P.SCN
	AND.W   #$7FFF,D1          ;IF HI BIT SET THEN USE A NOUN DESC
	MOVE.W  D1,D0
	BRA     P.DESC
 
TBL.READ 
 
	DC.W     NPAPER,NPAPER+$8000
	DC.W     NPOSTER,NPOSTER+$8000
	DC.W     NGRAFFITTI,NGRAFFITTI+$8000
	DC.W     NWRITING,NWRITING+$8000
	DC.W     NBOARD,NWRITING+$8000
	DC.W     NPWALL,NGRAFFITTI+$8000
	DC.W     NWRITING2,NWRITING2+$8000
	DC.W     NNOTICE,NNOTICE+$8000
	DC.W     NMESSAGE,NMESSAGE+$8000
	DC.W     NPLAQUE,NPLAQUE+$8000
	DC.W     NREM,NREM+$8000
	DC.W     NCHIT,NCHIT+$8000
	DC.W     NLISTING,NLISTING+$8000
	DC.W     0 
 
 
*--------------------------------
 
 
   
*--------------------------------
 
READ.TOMES
 
	BSET    #7,6(A0)
	SCN     87
	RET

      ENDC

	ENDC

      END

