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


****************************************************************
*
*	@(#)newparsersub.asm	1.5
*	5/16/87
*
****************************************************************

	SECTION	"ADV",code

* newparsersub.asm - new parser related subroutines
*
* First edition 22/2/87 - Ken

	include "equates.asm"
	include "macros.asm"

	XREF	P.SCN,P.MSG,CHAROUT,INBUFF,INPUT,INFORMAT,SPACE,UTIL.BS
	XREF	SEARCHER,FNDWRD,MATCHBUF

	XDEF	INSIST,P.STRING

*
* INSIST - this is intended to allow bits of game to insist that one of
*          a given list of replies is make by the player
*
* eg - (Please answer YES or NO)>>
*
* the text is supplied by the caller via a string pointer (see below)
* and the list of allowed words also vi a pointer.
*
* ENTRY:
*        D0.W = no. of tries before returning failure (-1 => forever)
*        A0 points at the text (WORD zero term)
*        A1 points at the matchbuf list (LONG -1 term)
*
* EXIT: 
*        D0.W = no. of tries which happened
*        D1 = which entry in list matched (ie 0 => first 1 => second etc)
*             (-1 => none, failed)
*        D5 = word no. of word we got (nouns beware!!)
*        D6 = section no. of word we got
*

INSIST

	PUSH_L	D2-D4/D7/A0-A6		;save things that are not returns
	MOVEQ	#0,D2			;no. of attempts it took
10$
	ADDQ.W	#1,D2
	TEST_W	D0			;neg => don't count down
	BMI.S	20$
	SUBQ.W	#1,D0
	BMI.S	80$			;waa - ran out of goes
20$
	PUSH_L	D0/D2/A0/A1

	CALL	P.STRING		;print our fancy message
	MSG	SPACE			;make sure text is flushed
	DO	UTIL.BS
	CALL	INPUT			;get the line
	CALL	INFORMAT		;kill mult-spaces &,. etc

	PULL_L	D0/D2/A0/A1
	TEST_B	D7			;any hassle?
	BNE.S	10$			;yes, ask again

	LEA	INBUFF(A4),A6

	PUSH_L	D0/D2/A0/A1

	DO	FNDWRD			;suss initial spaces

	PULL_L	D0/D2/A0/A1
	TEST_B	D7
	BNE.S	10$			;waa - no word!

	PUSH_L	D0/D2/A0/A1

	CALL	SEARCHER
	MOVE.L	#-1,(A2)		;terminate buffer
	LEA	MATCHBUF(A4),A2

	PULL_L	D0/D2/A0/A1

* this bit will be non-obvious since there are about 3 ply of meaning to
* 'match' here - so I'll try to say what's going on first

* A1 points at a matchbuf of ALLOWED words
* A2 points at a matchbuf of the ENTERED word
*
* the two are compared, if an ALLOWED word is found in the ENTERED list we
* have won, and exit ok, else we try asking again
*
* the compare is done by getting the first ALLOWED word and trying it
* against all the ENTERED words, if none same then getting the next ALLOWED...
*
* Phew..

	MOVE.L	A1,A3			;required matchbuf
	MOVEQ	#-1,D1			;count which allowed word it was
30$
	ADDQ.W	#1,D1			;so caller can easilly tell
	MOVE.L	(A3)+,D6		;get section,word
	BMI.S	10$			;we're out of allowed matched - retry

	MOVE.L	A2,A5			;user's one
40$
	MOVE.L	(A5)+,D5		;get section,word
	BMI.S	30$			;this one's not it

	CMP.L	D5,D6			;allowed?
	BNE.S	40$			;nope - see if more there

* OK we got one!!!!!!

	SWAP	D6			;cos it's got section,word now
	MOVE.W	D2,D0			;how many tries it took

	SETEQ				;flag passed
	BRA.S	90$			;exit

* failures come here

80$
	MOVEQ	#-1,D0
	MOVEQ	#-1,D1

*	SETNE				;moveq does this
90$
	PULL_L	D2-D4/D7/A0-A6
	RET



*
* P.STRING - this prints out a fancy string which may have SCN & prog MSG's
*            as sub strings
*
* ENTRY: A0 points at the start of a string
*
* format of a string:
*
* consists of a series of WORDS which may be ASCII or message #'s
*
* if a word < 0x8000 then both bytes are assumed to be ASCII, if the
* hi byte is zero it's taken as END of message, if the low bytes is zero
* it's taken as a padding byte (cos msg words got to be ALIGNED)
*
* if a word > 0x8000 it's taken as a message no. 
* if greater than 0xC000 it's taken as a SCENARIO message
*

P.STRING

	PUSH_L	D0-D1/A0
10$
	MOVE.W	(A0)+,D1
	BPL.S	30$			;chars!
	AND.W	#$7FFF,D1
	BCLR	#14,D1			;NE => SCN message
	BEQ.S	20$
	DO	P.SCN
	BRA.S	10$
20$
	DO	P.MSG
	BRA.S	10$

30$
	BEQ.S	90$			;word of zero => we've finished
	MOVE.B	D1,D0			;save second char
	LSR.W	#8,D1			;get first one
	DO	CHAROUT
	MOVE.B	D0,D1
	BEQ.S	10$			;EQ => padding
	DO	CHAROUT
	BRA.S	10$
90$
	PULL_L	D0-D1/A0
	RET



	END
