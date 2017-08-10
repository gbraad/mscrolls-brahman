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
*	@(#)in.asm	2.10
*	9/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.IN
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include "verbequ.asm"


	XDEF		INDIR,DO.GOIN,DO.INONLY

	XREF		MOVE.TAB,VALID.NP,GO.N,PRTTXT,VERB,ADVERB,PROC.LAB
	XREF		NOUN,CALCNET,R.NOUN2,CHEAT,SCREEN

*--------------------------------
*
* INDIR
*
*	calculate the direction 'in' from the currect location
*	using either a table of otherwise
*	register useage:
*		d0
*	exit:
*		d0= item 'in' refers to
*		or d0=0 => nowhere to go
*

INDIR

	PUSH_L	D2/D5/A0		;affected registers

      IFEQ	THE_PAWN

	DO	CALCNET			;A0 points at network info for ROOM
	MOVEQ	#0,D2			;kill hi bits
	MOVE.B	DIR_IN(A0),D2
	BEQ.S	10$			;no way in
	CMP.B	#MAXROOM,D2		;an nway_xyzzy message?
	BCC.S	10$			;CC => yes
	DO	R.NOUN2			;convert room# to noun#
	BRA.S	90$			;exit

10$

      ENDC

* getting here means there is no DIR_IN 'in' - so see if there are any
* objects which may lead somewhere

	LEA	MOVE.TAB(A4),A0		;start of movement vectors
20$
	MOVE.W	(A0)+,D0		;item to go in
	BEQ.S	90$			;end, nowhere to go
	DO	VALID.NP		;is around?
	BEQ.S	90$			;a hit!  
	ADDQ.L	#2,A0			;skip rest of data
	BRA.S	20$			;next
90$
	PULL_L	D2/D5/A0		;restore
	RET


*--------------------------------

DO.GOIN
*	entry:

	CMP.W	#VNGO,VERB(A4)		;go?
	BNE.S	90$			;no, forget it 
	MOVE.W	ADVERB(A4),D0		;get verb modifier
	BMI.S	90$			;is flagged as a direction, forget it
	CMP.W	#INTO,D0		;== INTO ?
	BEQ.S	05$			;yes, ok
	CMP.W	#IN,D0			;== IN ?
	BNE.S	90$			;nope.
05$
	CALL	INDIR			;get item to go in -> d0
	TEST_W	D0			;nowhere
	BNE.S	10$			;no
	MOVEQ	#2,D7			;error flag, (no 'eh- ...' )
	MSG	NOENTER			;<'There is nothing to enter here.'>
	POPSP
	DOGO	SCREEN
10$
	MOVE.L	NOUN(A4),A0
	MOVE.W	D0,(A0)+
	CLR.W	(A0)			;terminate
	ADDQ.L	#4,A7			;fod RTS
	BRA	PROC.LAB		;continue
90$
	RET

	
*--------------------------------
* 
* DO.INONLY 
*
*	process the command 'in'
*       gets here from STCONJ
*

DO.INONLY

	CMP.W	#INTO,D5		;into?
	BEQ.S	10$			;yes
	CMP.W	#IN,D5			;in?
	BNE.S	90$			;fail
10$
	POPSP				;not going back to STCONJ!
	MOVE.W	#VNGO,VERB(A4)		;set verb up
	MOVE.W	#IN,ADVERB(A4)		;set to 'in'
	BRA	DO.GOIN			;take over
90$
	RET

*--------------------------------

	END
