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
*	@(#)show.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SHOW/PUNCH/KICK/FILL
*
*--------------------------------


	include	"equates.asm"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"macros.asm"
	include	"verbequ.asm"



      XREF     CHK4PT,DBS,CHKNPC,SP.GIVE,P.SAY2,W.HNICE,PRTTXT,GETDATA,CHK.OWN
      XREF     CONJ,V.BREAK,V.PUT,EMPTY,P.IDFY,ADVERB,WHOOPS,P.NORCH
      XREF	ISARE,P.TN,GETNPC

	IfVerb	SHOW

	XDEF	V.SHOW

V.SHOW

	MOVEQ   #1,D1              ;TO
	DO	CHK4PT
	BTST    #6,4(A5)           ;NPC?
	BEQ     DBS                ;NO
	DO	CHKNPC
	CALL    SP.GIVE            ;HAS SAME EFFECT
	MOVE.W	D5,D0
* is it an animal?

	DO	GETNPC			; Returns A3 pointing at npc data
	BTST	#2,2(A3)		; is it an animal.
	BEQ.S	10$			; Nope
	DO	P.TN			; the xyzzy
	GETINFO				; ISARE needs a0 set up
	DO	ISARE			; are .. is
	MSGGO	NOTINTERESTED		; not interested in the stuivff

10$
* it is not an animal
	DO	P.SAY2
	MSGGO	HNICE

	ENDC
	
*--------------------------------

	IfVerb	PUNCH

	XDEF	V.PUNCH
	
V.PUNCH
	
	TEST_W  CONJ(A4)
	BMI.S   PU50               ;WITHOUT
	MOVEQ   #0,D1         
	DO	CHK4PT	            ;WITH WHAT?
	CMP.W   #NFIST,D5          ;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	BEQ.S   PU50               ;OK
	CMP.W   #NHAND,D5          ;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	BNE     DBS
PU50
	CLR.W   CONJ(A4)
	MOVE.W  #NFIST,D5          ;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	EXG     D0,D5
	EXG     A0,A5
	CALL    GETDATA
	EXG     D0,D5
	EXG     A0,A5
	BRA     V.BREAK

	ENDC
	
*--------------------------------

	IfVerb	KICK

	XDEF	V.KICK
	        
V.KICK
	
	TEST_W  CONJ(A4)
	BPL     DBS
	BRA     PU50               ;*PUNCH*

	ENDC
	
*--------------------------------

	IfVerb	FILL

	XDEF	V.FILL
	
V.FILL
	
*FILL X WITH Y = PUT Y IN X
*FILL X FROM Y = EMPTY Y INTO X
	
	TEST_W	D5			;is there an I.O. ?
	BLT.S	40$			;no - ask CHK4PT about it

	CMP.W   #FROM,CONJ(A4)     ;FROM?
	BEQ.S   50$                ;CASE 2
40$
	MOVEQ	#WITH,D1
	DO	CHK4PT
	CALL_S  FillSub
	BRA     V.PUT
50$
	CALL_S  FillSub
	BRA     EMPTY
	
FillSub

	EXG     D0,D5
	EXG     A0,A5
	MOVE.W  #INTO,CONJ(A4)         ;SET TO INTO
	RET

	ENDC
	
*--------------------------------

	IfVerb	VOTE

	XDEF	V.VOTE
	
V.VOTE
	MOVE.W  ADVERB(A4),D1
	CMP.W   #6,D1                ;FOR?
	BEQ.S   10$                  ;OK
	CMP.W   #$11,D1              ;AGAINST
	BNE     P.IDFY
10$
	PRINT  <'HOW?'>
	RET

	ENDC
	
*--------------------------------

   END

