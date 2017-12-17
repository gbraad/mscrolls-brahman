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
*	@(#)smoke.asm	2.11
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SMOKE/WAIT/HELP/LAUGH
*
*--------------------------------

	include	"equates.asm"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"macros.asm"
	include	"verbequ.asm"

      XREF     SP.SMOKE,DBS,W.TPASS,P.VAGE,W.TAPDCE,W.NOCHAN,W.HOHO,P.IDFY
      XREF	PRTTXT,SP.WAITUNTIL

	IfVerb	SMOKE

	XDEF	V.SMOKE
 
V.SMOKE
 
	CALL    SP.SMOKE           ;PLANT?
	DOGO	DBS	               ;NOPE

	ENDC
 
*--------------------------------

	IfVerb	WAIT

	XDEF	V.WAIT
   
V.WAIT

      IFNE	TimeCode

	XREF	CLOCK,GetTime,ADVERB,SPECIALS,SPECIALS_Before,Waiting
	XREF	R.NOUN,P.LOC,P.TN

	
	CMP.W	#UNTIL,ADVERB(A4)	;until?
	BEQ.S	WaitUntil
	CMP.W	#FOR,ADVERB(A4)		;wait for?
	BEQ.S	10$
	MSGGO	TPASS
10$
	DO	P.TN
	MSGGO	NEVERARRIVE		;<'might never arrive!^'>

WaitUntil

	CALL	GetTime			;grab a time+convert to KerovnianST
					;returns D2
	CALL	SP.WAITUNTIL		;any specials?
	SUB.W	CLOCK(A4),D2		;suss that it was in the past
	BMI	DBS

	MOVE.W	D2,Waiting(A4)

* loop through specials for a while - this is an iffy method

10$
	ADDQ.W	#1,CLOCK(A4)
	DO	R.NOUN
	CALL	P.LOC

      IFNE	VAX		;no top line vis.

	PRINT	<'.^'>

      ENDC

	CALL	SPECIALS_Before
	CALL	SPECIALS
	SUBQ.W	#1,Waiting(A4)
	BGT.S	10$
	RET

      ENDC

	MSGGO	TPASS

	ENDC
 
*--------------------------------

	IfVerb	HELP

	XDEF	V.HELP
	 
V.HELP
	
	CMP.W   #NPLAYER,D0        ;!!!!!!!!!!!!!!!!!!!!!!!!!!
	BEQ     P.VAGE
	TEST_W  D0                 ;ANY?
	BPL.S	10$
	MSGGO	TAPDCE
10$
	BTST    #6,4(A0)           ;ALIVE?
	BEQ     DBS                ;HELP TREE ETC..

      IFEQ	THE_PAWN
	
	XREF	SP.HELP
	CALL	SP.HELP

      ENDC

	MSGGO	NOCHAN

	ENDC
 
*--------------------------------

	IfVerb	LAUGH

	XDEF	V.LAUGH
 
V.LAUGH
 
	MSGGO	HOHO

	ENDC
 
*--------------------------------

      END

