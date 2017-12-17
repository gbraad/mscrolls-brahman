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
*	@(#)dig.asm	1.11
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*	dig.asm (also bet & wind)
*
*--------------------------------
 
	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"


	XREF	DBS,P.TN,W.YCNDTT,P.SUB,P.VERB,VERB,P.MHAV,BSFULLSTOP
	XREF	P.IDFY,PRTTXT,ADVERB,NPCINRM,NOUNBUF3,CHK4PT,CONJ,WHOOPS
	XREF	V.EAT,V.SHINE,GET_,ISARE,CARRIED,W.NOONE,P.TNSTOP
        XREF    P.MHAVSTOP

*
* V.DIG
*
	IfVerb	DIG

	XDEF	V.DIG
	XREF	SP.DIG	
V.DIG

	CALL	SP.DIG
	DOGO	DBS		;hmmmmmmmm verbose verb of the week...

	ENDC

*
*  Roll  ( as in dice
*
	IfVerb	ROLL

	XDEF	V.ROLL
	XREF	SP.ROLL

V.ROLL

	DO	CARRIED
	BNE.S	10$
	CALL	SP.ROLL
	DOGO	DBS		; ... part two

10$
	CALL	P.MHAV		; You must have the xyzzy.
	DOGO	BSFULLSTOP

	ENDC

*
* V.CHEW
*
	IfVerb	CHEW

	XDEF	V.CHEW
	XREF	SP.CHEW	

V.CHEW

	CALL	SP.CHEW
	MOVE.W	#VNEAT,VERB(A4)
	BRA	V.EAT

	ENDC

*
* V.BET
*
* call SP.BET with 
*
* D0 = stake
* D1 = NPC
* D5 = runner
*
* if SP.BET returns then we have to print the <D1> refuses your bet
* if D1 = 0 use NPCINRM

	IfVerb	BET

	XDEF	V.BET
	XREF	SP.BET

V.BET

	CMP.W	#ON,ADVERB(A4)		;bet on x with y?
	BNE.S	30$
	TEST_W	CONJ(A4)		;with?
	BEQ.S	20$
10$
	MOVEQ	#0,D1			;with
	DO	CHK4PT			;exit doing a 'with what?'
20$
	TEST_W	D5
	BMI.S	10$
	MOVEQ	#0,D1
	EXG	D0,D5
	EXG	A0,A5			;proably spurious
	BRA.S	Bet_CallSP

* deal with 'bet x on y (with z)' z is bookie

30$
	MOVEQ	#ON,D1
	DO	CHK4PT			;make sure of the on

	MOVEQ	#0,D1			;default bookie
	TEST_W	CONJ+2(A4)		;with x?
	BNE.S	Bet_CallSP		;nope
	MOVE.L	NOUNBUF3(A4),D1		;get bookie


Bet_CallSP

	ST	WHOOPS(A4)		;no multiple betting!
	CALL	SP.BET

	TEST_W	D1
	BNE.S	10$
	MOVE.W	NPCINRM(A4),D1
10$
	MOVE.W	D1,D0
	BGT.S	20$
	MSGGO	NOBODYBET		;<'There is nobody here to bet with!^'>
20$
	DO	P.TN
	MSGGO	REFUSEBET		;<'refuses your bet.^'>

	ENDC

*
* V.STOP
*
	IfVerb	STOP

	XDEF	V.STOP
	XREF	SP.STOP

V.STOP
	
	CALL	SP.STOP
	DO	P.SUB
	MSG	YCNDTT
	DOGO	P.TNSTOP

	ENDC

*
* V.PLAY
*
	IfVerb	PLAY

	XDEF	V.PLAY
	XREF	SP.PLAY

V.PLAY
        TEST_L  6(A0)                   ; object must be in hands
	BNE	P.MHAVSTOP		;NO
	CALL	SP.PLAY
	DOGO	DBS

	ENDC

*--------------------------------

*  ORIGINAL VERSION        
*
* V.SWIM
*
*	IfVerb	SWIM
*
*	XDEF	V.SWIM
*	XREF	SP.SWIM
*
*V.SWIM
*
*	CALL	SP.SWIM
*	DOGO	DBS
*
*	ENDC

*--------------------------------

*
* V.FLUSH
*
	IfVerb	FLUSH

	XDEF	V.FLUSH
	XREF	SP.FLUSH

V.FLUSH

	CALL	SP.FLUSH
	DOGO	DBS

	ENDC

*
* V.PISS
*
	IfVerb	PISS

	XDEF	V.PISS
	XREF	SP.PISS

V.PISS

	CMP.W	#IN,ADVERB(A4)		;must give a target
	BEQ.S	10$			;in/into/on ok
	CMP.W	#ON,ADVERB(A4)
	BEQ.S	10$
	CMP.W	#INTO,ADVERB(A4)
	BNE	P.IDFY
10$
	CALL	SP.PISS
	MSG	CANTALLOW		;' allowed to '
	MOVE.W	VERB(A4),D1		;piss/shit/crap etc
	DO	P.VERB
	MSGGO	RESTRAIN		;<' just anywhere you know!
					; Restrain yourself please!^'>

	ENDC
*
* V.SLEEP
*
	IfVerb	SLEEP

	XDEF	V.SLEEP
	XREF	SP.SLEEP

V.SLEEP

	CALL	SP.SLEEP
	MSGGO	ZZZZZZ			;Zzzzz...

	ENDC
*
* V.WIND
*
	IfVerb	WIND

	XDEF	V.WIND
	XREF	SP.WIND

V.WIND

	CALL	SP.WIND

	DO	P.SUB		;You...
	MSG	YCNDTT		;Can't do that to the ...
	DOGO	P.TNSTOP	;xyzzy!

	ENDC

*
* V.SET
*
	IfVerb	SET

	XDEF	V.SET
	XREF	SP.SET

V.SET

	CALL	SP.SET

	DO	P.SUB
	MSG	YCNDTT
	DOGO	P.TNSTOP

	ENDC

*
* V.REFLECT
*
	IfVerb	REFLECT

	XDEF	V.REFLECT
	XREF	SP.REFLECT

V.REFLECT

	CALL	SP.REFLECT

	TEST_W	D0			;SINGV gives -1 in D0
	BMI.S	10$

	DO	P.SUB
	MSG	YCNDTT
	DOGO	P.TNSTOP
10$
	MSGGO	THINKAWHILE		;<'You think a while about the
					;deeper issues of life.^'>
	
	ENDC

*
* V.HOLD
*
	IfVerb	HOLD

	XDEF	V.HOLD
	XREF	SP.HOLD

V.HOLD

	CALL	SP.HOLD

	DO	CARRIED
	BNE	GET_
	DO	P.TN
	DO	ISARE
	MSGGO	TIGHTLYHELD		;<' tightly held.^'>

	ENDC
*
* V.ILLUMINATE
*
	IfVerb	ILLUMINATE

	XDEF	V.ILLUMINATE
	XREF	SP.ILLUMINATE

V.ILLUMINATE

	MOVEQ	#0,D1		;illuminatye x with y?
	DO	CHK4PT		;with what? (john!)

	MOVE.W	#AT,CONJ(A4)	;fake up 'shine y at x'
	EXG	A0,A5
	EXG	D0,D5
	BRA	V.SHINE
	
	ENDC

*
* V.CATCH
*
	IfVerb	CATCH

	XDEF	V.CATCH
	XREF	SP.CATCH

V.CATCH

	CALL	SP.CATCH
	BRA	GET_

	ENDC
*
* V.PRAY
*
	IfVerb	PRAY

	XDEF	V.PRAY
	XREF	SP.PRAY

V.PRAY
	CALL	SP.PRAY
	MSG	NOONE
	RET

	ENDC

	IfVerb	SING

	XDEF	V.SING
	XREF	SP.SING

V.SING

*	CALL	SP.SING
	PRINT	<'!!!! to be sorted !!!!'>
	RET

	ENDC

*
* V.FISH
*
* this will call SP.FISH with all sorts of random information
*
* if u 'fish for x with y in z'
* 
* D2 = x, D3 = y, D4 = z when we get to SP.FISH
* if any are missed out then the relevant reg will be zero
*

	IfVerb	FISH

	XDEF	V.FISH
	XREF	SP.FISH

V.FISH

	ST	WHOOPS(A4)		;can't use multiple D.O. (John)

	MOVEQ	#0,D2			; for [d2]
	MOVEQ	#0,D3			; with [d3]
	MOVEQ	#0,D4			; in [d4]
	MOVEQ	#0,D7			;flag that at least one found

	MOVE.W	ADVERB(A4),D1
	CALL	FishSet

	MOVE.W	CONJ(A4),D1
	MOVE.W	D5,D0
	CALL	FishSet

	MOVE.W	CONJ+2(A4),D1
	MOVE.W	NOUNBUF3(A4),D0
	CALL	FishSet

	TEST_B	D7			;good enough parse?
	BEQ	P.IDFY
	CALL	SP.FISH
	MSGGO	FISHING		;<'Fishing here is a complete waste of time.'>


* given: fish [for x] [with y] [in z]
*
* put D2 = x, D3 = y, D4 = z

FishSet

	CMP.W	#FOR,D1
	BNE.S	10$
	MOVE.W	D0,D2
	ST	D7			;flag some parts of speech ok
10$
	CMP.W	#WITH,D1
	BNE.S	20$
	MOVE.W	D0,D3
	ST	D7			;flag some parts of speech ok
20$
	CMP.W	#IN,D1
	BNE.S	90$
	MOVE.W	D0,D4
	ST	D7			;flag some parts of speech ok
90$
	RET



	ENDC			;of verb def



      END
