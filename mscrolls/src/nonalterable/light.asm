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
*	@(#)light.asm	2.11
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.LIGHT/BURN/IGNITE
*
*--------------------------------
 
*--------------------------------
*
* LIGHT/BURN/IGNITE X WITH Y
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include "verbequ.asm"

	XREF	CHK4PT,P.BTN,ISAREN,W.LIT,P.TNIN,P.SUBV,W.UNABLE,W.TO
	XREF	W.LIGHT,P.TN,W.EXP,DEAD,W.MLTS,W.BURNAW,FIXCONT,SETOUT
	XREF	CARRIED,P.MHAV,M.YCNDTT,BSFULLSTOP,P.SUB,P.NOUN,W.YCNDTT
	XREF	CONJ,P.TNSTOP,FULLSTOP,YCANT,ISARE
	
	IfVerb	LIGHT

	XDEF     V.LIGHT

V.LIGHT

	IFEQ	THE_PAWN

	XREF	SP.VLIGHT
	CALL	SP.VLIGHT		;there's already an SP.LIGHT!

	ENDC

	CMP.W	#FROM,CONJ(A4)		;also allowed
	BEQ.S	10$

	CLR.W   D1                  ;WITH
	DO	CHK4PT	             ;WITH WHAT?
10$
	BTST    #7,(A5)             ;Y LIT?
	BEQ.S   15$                 ;O.K. IT IS
	BTST    #7,4(A5)            ;LIT & LIGHTABLE = CAN USE TO LIGHT
	BNE.S   20$                 ;OK
15$
	EXG     D0,D5               ;NOT LIT
	EXG     A0,A5               ;SWAP OBJECT,SUBJECT
	DO	P.BTN	              ;'BUT THE Y
	DO	ISARE	             ; IS/ARE
	MSG	NLIT                 ; not burning
	DOGO	BSFULLSTOP
 
20$
	CALL	CHKHAVE		   ;must have either
	BEQ.S	21$		   ;yep.
	BRA	P.MHAV
21$
	BTST    #7,4(A0)            ;LIGHTABLE?
	BNE.S   25$                 ;YES
	MOVE.B  11(A0),D1           ;GET COMBUST BYTE
	AND.B   #$18,D1             ;MASK REST OF BITS
	BEQ.S   30$                 ;0=NON FLAM
	CMP.B   #$18,D1             ;3=EXPLODES
	BEQ.S   40$
	CMP.B   #$10,D1             ;2=MELTS
	BEQ.S   50$
	BRA.S   60$                 ;BURNS AWAY
25$
	BSET    #7,(A0)             ;SET LIT BIT
	AND.B   #$F0,11(A0)         ;ZAP TEXTURE
	OR.B    #$03,11(A0)         ;TO BE HOT

	BeSure	M.LIT

	MOVE.W  #M.LIT,D1           ;THE X IS NOW LIT

	DO	P.TNIN
	DOGO	BSFULLSTOP
 
30$
	DOGO	YCANT
 
40$
	CALL_S	70$			;don't care about the SETOUT
	MSG	EXP			;explodes
	DOGO	DEAD
 
50$
	
	CALL_S  70$                ;PUT IT OUT/FIXCONT ETC
	MSG	MLTS
	DOGO	FULLSTOP
 
60$
	CALL_S  70$                ;PUT IT OUT ETC
	MSGGO	BURNAW
70$
	DO	SETOUT	            ;ZAP FROM GAME & CLEAN TIED ETC
	DOGO	P.TN


	ENDC


	IfVerb	UNLIGHT

	IFEQ	THE_PAWN


	XDEF	V.UNLIGHT
	XREF	SP.UNLIGHT

*
* V.UNLIGHT	- unlight/extinguish - particularly lamps!
*

V.UNLIGHT

	CALL	SP.UNLIGHT
	DO	P.SUB
	MSG	YCNDTT
	DO	P.NOUN
	DOGO	BSFULLSTOP


	ENDC

	ENDC

*--------------------------------------------- 
*
*  CHKHAVE, check have
*  must have either the direct object or the indirect object
*
* entry:  d0/a0 , a5/d5
* exit: eq=> ok, at least one is carried
*		ne=> no-go. (d0/a0 contain nocarried item)

CHKHAVE

	BSR	CARRIED		;have dobj?
	BEQ.S	90$		;yes, ok
	EXG	A0,A5
	EXG	D0,D5
	BSR	CARRIED		;have iobj?
	BNE.S	90$		;no, fail
	EXG	D0,D5
	EXG	A0,A5		;back
90$
	RET

*---------------------------------------------

      END

