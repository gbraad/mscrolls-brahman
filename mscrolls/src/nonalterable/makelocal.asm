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
*	@(#)makelocal.asm	1.12
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* makelocal things
*
*--------------------------------


	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.i"
	include	"roomequ.i"

	XREF	ForceRefresh,PRTTXT,FindRoom,ROOM,CALCNET,CALCNET2,M.SURECONTINUE
	XREF	P.TNSTOP,P.TN,SafeSetmode,CHEAT,VALID.NP,P.Refresh,WHOOPS
	XREF	SETMODE,DIED,LIGHT,SP.LIGHT,SPECIALS,SPECIALS_Before,CLOCK
	XREF	W.HERE,ISARE,VERB,INSIST,GETDATA


AnitasBits	EQU	FALSE

	IfVerb	MAKELOCAL

	XDEF	V.MAKELOCAL


V.MAKELOCAL

      IFNE	YCHEAT

	MOVE.W	D0,MKLSaved(A4)
	
	TEST_B	CHEAT(A4)
	BNE.S	20$

      ENDC

	DO	VALID.NP
	BNE.S	03$
	DO	P.TN			;waaa it's local already!!
	DO	ISARE
	MSGGO	HERE
03$
	BTST	#0,5(A0)		;pseudo?
	BNE.S	10$

	BTST	#6,6(A0)		;room?
	BNE.S	20$

	BTST	#4,4(A0)		;explored
	BEQ.S	10$			;if not (EQ) then no can get

05$
	MOVE.W	ROOM(A4),-(A7)
	MOVE.W	D0,-(A7)
	CALL	FindRoom
	MOVE.W	D0,ROOM(A4)
	MOVE.W	(A7)+,D0
	DO	VALID.NP
	MOVE.W	(A7)+,ROOM(A4)
	TEST_W	D7
	BEQ.S	20$
10$
	ST	WHOOPS(A4)
	MSG	NOTFAINTEST		;<'I haven''t the faintest idea
					;where to begin looking for '>
	DOGO	P.TNSTOP

20$
	MOVE.W	ROOM(A4),D1
	PUSH_L	D0/A0
	CALL	GenerateMakeLocalList		;A3 -> end of list
	PULL_L	D0/A0
	TEST_W	D7
	BNE.S	10$				;NE => failed to find route


	LEA	RelBuf(A4),A1
	ADDQ.W	#1,A3				;so as I can use pre-dec mode

	MOVEQ	#0,D1
	MOVE.W	ROOM(A4),D0			;start in current room
40$
	MOVE.B	-(A3),D1			;direction

	DO	CALCNET				;steps on D2, returns A0
	MOVE.B	0(A0,D1.W),D1			;new target room

	PUSH_L	D0-D7/A0-A6

	MOVE.W	ROOM(A4),D0			;'cos we've forgotten room#
	CALL	SafeSetmode		;take D0=source, D1=target, ret EQ => ok
	BEQ.S	50$
	CALL	NotSafeSM		;no good - ask if should continue
50$

	PEA	Stopped(PC)			;in case we do!
	CALL	SETMODE		;waaaaaaaa (said whilest quaking in boots)

	POPSP				;loose PEA

	TEST_B	DIED(A4)
	BNE	Stopped

	CALL	SP.LIGHT

	ADDQ.W	#1,CLOCK(A4)
	CALL	SPECIALS_Before

	MOVE.W	VERB(A4),-(A7)
*
* set the verb to a non-global type so as to avoid adjectives being listed
* spuriously - since P.NOUN detects more than just the objects in the room
* as local
*

	MOVE.W	#VNLOOK,VERB(A4)
	CALL	P.Refresh		;this will stand on VERB(A4) too
*
* but we have to maintain the verb else we can't come through a second time
*
	MOVE.W	(A7)+,VERB(A4)

	CALL	SPECIALS

	TEST_W	LIGHT(A4)		;0 => is dark in here!
	BEQ	Stopped

	TEST_B	DIED(A4)
	BNE	Stopped

	PULL_L	D0-D7/A0-A6
	CMP.W	ROOM(A4),D1		;did we get there?
	BNE.S	90$			;nope - what hit me?

	CMP.L	A1,A3			;finished?
	BNE.S	40$			;EQ => yes

	MSG	GOTHERE			;<'^O.K. I''m here! (Phew!!)^'>
      
      IFNE	YCHEAT

	XREF	GET_,MKLSaved

	TEST_B	CHEAT(A4)
	BEQ.S	90$

	MOVE.W	MKLSaved(A4),D0
	GETINFO
	BRA	GET_
      
      ENDC
90$
	RET

* come here if setmode is likely to cause hassle
* we can then ask the punter what he wants to do

MyMatchBuf
	
	DC.W	SECT.YESNO,0		;yes
	DC.W	SECT.YESNO,1		;no
	DC.L	-1			;end of buffer

MyString
	
	DC.W	M.SURECONTINUE+HI,0		;a P.STRING format message

	ALIGN

NotSafeSM

	PUSH_L	D0-D1/D5-D6/A0-A1	;better safe...

	MOVEQ	#-1,D0			;try forever
	LEA	MyString(PC),A0
	LEA	MyMatchBuf(PC),A1

	CALL	INSIST
	
	TEST_W	D1			;0 => yes
	PULL_L	D0-D1/D5-D6/A0-A1	;leaves flags ok

	BNE.S	10$			;waaa they backed off!
	RET
10$
	POPSP
*
* FALLS THROUGH!!!!!!!!!!!!!!!!!!!!!!!!!
*
*

*
* if we have to abort mid-stream come here!
*

Stopped

	PULL_L	D0-D7/A0-A6
	RET




	ENDC

	XREF	RelBuf

	
GenerateMakeLocalList

* A1 must be undisturbed
* this uses the STACK (a7)
* player location passed in D1 (room #)
* object required passed in D0
	CALL	FLUSHRelBuf
	MOVE.L	A5,-(A7)		;push datastream
	CALL	FindRoom		;returns d0 as room number of location.
	MOVE.W	D0,D6			;store it for safety sake.
	MOVE.W	D1,D7			;our current location

* Now get current room
	CMP.W	D7,D6			;already here?
	BNE.S	5$
	MOVE.W	#1,D7			;means its done
	MOVE.L	(A7)+,A5		;pull it back
	RET
5$
	MOVE.W	D7,D0			;get current player location
	LEA	RelBuf(A4),A2	;pointer to rooms for flagging
	BSET	#7,0(A2,D0.W)		;flag place as done
	MOVE.L	A7,A5			;a5 becomes our SP Saved pointer
	MOVE.L	A7,A3			;A3 becomes sub stack pointer
			    
	MOVE.W	#0,D5			;our offset...
	SUBQ.L	#4,A7			;This is to overcome the move.b
*hassles onto the stack where on real 68k it always keeps stack even
	MOVE.B	D0,3(A7)		;current room
	MOVE.B	#-1,2(A7)		;means where we started.
	MOVE.W	D5,(A7)			;offset from start from last room
	MOVE.W	#Sizeof_NET,D2		;the counter
10$
	CMP.W	#Sizeof_NET,D2		;are we finished with current room?
	BNE.S	30$
	CMP.L	A3,A7			;if equal can't be got!
	BEQ	NOTPOSSIBLE
	MOVE.B	-(A3),D0		;next exit
	MOVE.B	D0,D1			;store the room away for doors etc...
	SUB.W	#3,A3			;point a3 at next exit!
	ADDQ	#4,D5			;add four to offset
	MOVE.W	ROOM(A4),-(A7)
	MOVE.W	D0,ROOM(A4)		;to calculate network
	DO	CALCNET
* RETURNS A0 pointing at network stuff
	MOVE.W	(A7)+,ROOM(A4)
	CLR.W	D2			;index into network
30$
	MOVE.B	0(A0,D2.W),D0		;get exit
	BEQ.S	40$			;if eq=> no exits

	 IFNE	AnitasBits		; cant go through doors
	CMP.B	#nway_door,D0		;is it a door?
	BNE.S	35$
	PUSH_W	D3/D4
	CALL	RETURNDOOR		;returns d3 = door no d4 = room no
	CMP.W	#-1,D4			;no door...
	BEQ.S	33$			;no door guys go away
	MOVE.B	D4,D0			;this is the new one chaps
33$
	PULL_W	D3/D4			;and get back reggies
	 ENDC
35$
	CMP.W	#MAXROOM,D0		;is it an NWAY?
	BHI.S	40$			;yes
	CMP.B	D0,D6			;found our place?
	BEQ	50$			;whoopee!!!!
	BSET	#7,0(A2,D0.W)		;flag it
	BNE.S	40$			;Already done?
	SUBQ.L	#4,A7			;pushing stack hassles (c above)
	MOVE.B	D0,3(A7)
	MOVE.B	D2,2(A7)		;flag exit and direction
	MOVE.W	D5,(A7)			;the offset to last room...

40$
	ADDQ.W	#1,D2			;add one to network index
	BRA.S	10$			;And continue
50$
* okay guys we now have pointed to by A7 the start of the backwards
* tracking required to get from b to a!

	SUBQ.L	#4,A7
	MOVE.B	D0,3(A7)
	MOVE.B	D2,2(A7)
	MOVE.W	D5,(A7)
	LEA	RelBuf(A4),A3	;where our command stream will
					;live
45$
	CMP.B	3(A7),D7			;are we back where we started?
	BEQ.S	60$			;yup
	MOVE.B	2(A7),(A3)+		;direction of travel
	MOVE.L	A5,A2			;the next one
	SUB.W	(A7),A2		;the next pointer
	MOVE.L	A2,A7			;reset it
	BRA.S	45$
60$
* okay reverse the order now...
	ADDQ	#4,A7			; reset a7 to where it belongs
	SUBQ	#1,A3			;point a3 back one
	MOVE.L	(A7)+,A5		;a5 was saved for safety
	CLR.W	D7			;it worked
* This returns as a3 pointing into RelBuf to the point
* where we got too!
*  
	RET
	
NOTPOSSIBLE:

	MOVE.L	A5,A7			;restre the stack
	MOVE.L	(A7)+,A5
	MOVE.W	#-1,D7
	RET

FLUSHRelBuf:

	LEA	RelBuf(A4),A2
	MOVE.W	#NROOMS-1,D3
10$
	CLR.B	(A2)+
	DBRA	D3,10$
	RET




	END
