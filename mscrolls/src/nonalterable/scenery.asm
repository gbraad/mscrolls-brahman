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


**************************************************************
*
*	5/16/87
*	@(#)scenery.asm	1.10
*
**************************************************************

*	scenery.asm - deals with section 24 words

	SECTION	"ADV",code

	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"

	XDEF	DoScenery,Scenery
	XREF	Info24Table,P.TAB,W.THE,VALID.NP,Section24,UTIL.BS
	XREF	PRTTXT,DECODE,MATCHBUF,LINE,VERB
	XREF	W.IS,W.ARE,W.IT,W.THEY,OLDBUF,W.HERE,W.CNTSEE,P.SUB

*
* DoScenery	- gets jumped to from DECODE instead of 
*		  saying 'I don't understand xyzzy in that contect'
*
* MUST finish by jumping to DECODE - no stack fudging allowed!
* 
* on entry D5=word# D6=SECT.SCEN
*

DoScenery

	PEA	DECODE(PC)		;so as we can RET from here ok

* Scenery - this gets called from WSYN2, returns rather than going straight
*           to DECODE (will get there in the end!)

Scenery

	CLR.W	OLDBUF(A4)		;so as it/them pronouns fodded

	MOVE.W	D5,D1			;save word# for later..

	LEA	MATCHBUF(A4),A2		;iffyy...
05$
	LSL.W	#1,D5			;Info24 is a word-wise table
	LEA	Info24Table(PC),A0	;top 4 bits are type, rest noun#
	MOVE.B	0(A0,D5.W),D2		;get type info
	LSR.B	#4,D2
	MOVE.W	0(A0,D5.W),D0		;get location info
	AND.W	#$07FF,D0		;lose type bits
	DO	VALID.NP		;is it valid?

	BEQ.S	BaseIsValid
	MOVE.W	(A2)+,D6
	CMP.W	#SECT.SCEN,D6
	BNE	BaseNotValid		;ooops - the location isn't valid
	MOVE.W	(A2)+,D5
	BRA.S	05$

BaseIsValid

	SUBQ.B	#1,D2			;was it type 'A' ?
	BNE.S	10$			;nope
	MSG	CURSGLANCE		;<'A cursory glance at '>
	DO	P.TSNOUN
	MSG	PERSUADES		;<'persuades you that '>
	TEST_B	D7			;check plural return from P.TSNOUN
	BNE.S	06$
	MSG	IT
	BRA.S	08$
06$
	MSG	THEY
08$
	CALL	SISARE
	MSGGO	NOIMPORTANCE		;<'of no importance.^'>
10$
	SUBQ.B	#1,D2
	BNE.S	20$
	CALL	IsVerbLook		;this cop-out is silly for look/examine
	BEQ.S	25$
	MSG	UNFORTUNE		;<'Unfortunately '>
	DO	P.TSNOUN
	CALL	SISARE
	MSGGO	BEYONDREACH		;<' beyond your reach.^'>
20$
	SUBQ.B	#1,D2
	BNE.S	30$
25$
	MSG	POINTLESS		;<'Really! It''s pointless messing
					;around with scenery like '>
	DO	P.TSNOUN
	DO	UTIL.BS
	PRINT	<'!^'>
	RET
30$
	SUBQ.B	#1,D2
	BNE.S	40$
	CALL	IsVerbLook		;this cop-out is silly for look/examine
	BEQ.S	45$

* this is now 'You need to be closer to do that.'

	MSGGO	NEEDCLOSE		;<'cant reach it/must
					;specify dir. (hmmmmm)^'>
40$
	SUBQ.B	#1,D2
	BNE.S	50$
45$
	DO	P.TSNOUN
	CALL	SISARE	
	MSGGO	ABSNOIMPORT		;<' of absolutely no importance!^'>
50$
	SUBQ.B	#1,D2
	BEQ.S	BaseNotValid

      IFNE	YCHEAT

	PRINT	<'Bad scenery noun type!!!! '>
      
      ENDC

	MSG	JNSP			;'i don't understand
	BRA	P.SNOUN

BaseNotValid

	DO	P.SUB
	MSG	CNTSEE
	DO	P.TSNOUN
	MSGGO	HERE

* IsVerbLook - returns EQ is VERB= look,examine,..

IsVerbLook

	CMP.W	#VNLOOK,VERB(A4)
	BEQ.S	90$
	CMP.W	#VNEXAMINE,VERB(A4)

90$
	RET
	
* P.TSNOUN - print (THE) scenery noun - takes D1 = word# 

	XDEF	P.TSNOUN

P.TSNOUN

	MSG	THE			;print 'The xyzzy '

P.SNOUN

	LEA	Section24(A4),A1
	CALL	P.TAB	
	AND.B	#$5F,D1			;fod case+top bit of last char in word
	CMP.B	#'S',D1			;very iffy plural test
	SEQ	D7			;'s' on end => plural !!!!
	RET

* Scenery noun IS/ARE - uses D7.B rather than noun data plural bit

SISARE
	
	TEST_B	D7			;is it plural?
	BEQ.S	10$
	MSGGO	ARE
10$
	MSGGO	IS

	END
