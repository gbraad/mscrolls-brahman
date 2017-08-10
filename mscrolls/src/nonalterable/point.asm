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
*	@(#)point.asm	2.10
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.POINT/SHINE
*
*--------------------------------

      include	"equates.asm"
      include	"macros.asm"
      include	"verbequ.asm"


      XREF     ADVERB,CHK4PT,CONJ,W.IDFY,SP.POINT,W.NHAP,P.BTN,ISAREN,V.SPLIT
      XREF     W.LIT,SP.SHINE

	IfVerb	POINT

	XDEF	V.POINT

V.POINT
 
	CMP.W   #AT,ADVERB(A4)    ;POINT AT
	BEQ.S   50$
	CMP.W   #TO,ADVERB(A4)      ;POINT TO
	BEQ.S   50$
	TEST_W  D5
	BNE.S   04$
	MOVEQ   #AT,D1              ;'AT WHAT?'
	DO	CHK4PT	             ;WONT EVER COME BACK
04$
 
	CMP.W   #TO,CONJ(A4)        ;POINT HOBBITS TO DRAGON?
	BEQ.S   50$
	CMP.W   #AT,CONJ(A4)        ;PONIT X AT Y?
	BEQ.S	10$
	MSGGO	IDFY
10$
	EXG     A0,A5
	EXG     D0,D5
50$
	CALL    SP.POINT
	MSGGO	NHAP

	ENDC

	IfVerb	SHINE

	XDEF	V.SHINE
 
      IFEQ	THE_PAWN 

V.SHINE

	MOVE.W  ADVERB(A4),D1
	CMP.W   #THROUGH,D1        ;THROUGH
	BEQ.S   05$
	CMP.W	#ON,CONJ(A4)
	BEQ.S	05$
	MOVEQ   #AT,D1            ; AT
	DO	CHK4PT
05$
	EXG     A0,A5
	EXG     D0,D5
	CALL    SP.SHINE
	BTST    #7,(A5)            ;ALIGHT?
	BNE.S   10$
	EXG	D0,D5		  ;THIS ONE IS NOT ALIGHT!
	DO	P.BTN
	DO	ISAREN
	MSGGO	LIT
10$
	MSGGO	NHAP
90$
	RET

      ENDC

      IFNE	THE_PAWN 

V.SHINE

	MOVE.W  ADVERB(A4),D1
	CMP.W   #THROUGH,D1             ;THTOUGH
	BEQ.S   SHTHRU
	MOVEQ   #AT,D1                  ; AT
	DO	CHK4PT
	BTST    #7,(A0)                 ;ALIGHT?
	BNE.S   10$
	DO	P.BTN
	DO	ISAREN
	MSGGO	LIT
10$
	EXG     A0,A5
	EXG     D0,D5
	CALL    SP.SHINE
	MSGGO	NHAP
90$
	RET



*--------------------------------
* 
* let's u split white by 'shine white thru prism
	        
SHTHRU

	DO	CHK4PT
	CLR.W   CONJ(A4)           ;TO 'WITH'
	BRA     V.SPLIT

      ENDC

*--------------------------------

	ENDC


      END
