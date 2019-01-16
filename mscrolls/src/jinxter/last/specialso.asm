*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   specialso
*
*--------------------------------


	include	"equates.i"
*	include	"verbequ.i"
	include	"nounequ1.i"
*	include	"nounequ2.i"
	include	"roomequ.i"
	include	"macros.i"
	include	"scenmacros.i"
	include	"scoreref.i"



	XDEF	SP.OPEN

	XREF	DO.SCORE
	XREF	DoorOiled,DoorCreaked,ROOM
	XREF	TIMER9,CONJ,AirlockFlooded,GetWet
	XREF	UnderWater,FloodPos,OfficeFloods
	XREF	TIMER9,BellMuffled,TIMER8,ISIN,SETIN
	XREF	FloodObjects

* Get here if we try to open can of worms (used to be dorab food tin too)

OpenTin
	TST_CLOSED				; Still closed?
	BEQ.S	90$			;=> nope, (prob caught in verb anyway)
	TEST_W	CONJ(A4)		;with?
	BNE.S	20$
	CMP.W	#NTIN_OPENER,D5
	BNE.S	20$
	CLR.W	D5			; Make it a simple open, and do it.
	SCN	22
* I hope you know what you're doing!^

10$
	BRA.S	90$

20$
	SCN	69			; Not that easy!
	POPSP				; Prevent player opening it!
90$
	RET

*--------------------

	XREF	FloodPos,LibraryFlooded,FlagWitchDoorOpen,STOPRON
	XREF	OfficeFlooded

SP.OPEN

	CMP.W	#NSOLID_DOOR,D0
	BNE.S	002$
	CMP.W	#RNDUNGEON,ROOM(A4)
	BNE.S	002$
	SCN	569
	POPSP_RET
002$
	CALL	OpenCloseHatches
	BEQ	80$			;done, exit
	CMP.W	#NRED_DOOR,D0		;open red door?
	BNE.S	01$
	MOVE.W	FloodPos(A4),D1		;has flooding happened?
	CMP.W	#1,D1			;passed 1 turn?
	BLS.S	01$			;otherwise ok
	TEST_B	LibraryFlooded(A4)	;but you can stil open it then
	BNE.S	01$
	SCN	217
	BRA	80$			;exit
01$
	CMP.W	#NCANOWORMS,D0
	BEQ.S	OpenTin
	CMP.W	#NSLIDINGDOOR,D0	; Opening sliding door?
	BNE.S	10$
	TEST_B	DoorOiled(A4)		; Is it oiled?
	BNE.S	5$
	SCN	168			; No, so 'Creeeeaaaak!'
	ST	DoorCreaked(A4)		; Set the flag when it creaks.
	BRA	90$
5$:
	SCN	169			; Silence...
	BRA	90$			;exit

10$:

	CALL	OpenMailBox		;Trying to open a mailbox?
	BNE	80$			;=> handled it

	CMP.W	#NWITCH_DOOR,D0
	BEQ.S	20$
	CMP.W	#NWITCH_DOOR1,D0
	BNE.S	30$
20$
	BSET	#7,FlagWitchDoorOpen(A4)
	BEQ	90$			;There wasn't one open, just do it
	DO	P.TN			;'The xyzzy door'
	SCN	754
* won't budge.

	BRA	80$

30$
	CMP.W	#NAIRLOCK_DOOR,D0
	BNE.S	40$
	GETINFO
	TST_LOCKED			;locked?
	BNE.S	40$			;yes, no-go
; can only open the door if airlock empty, office not flooded
; or if airlock full and office flooded
; or airlock full, office empty (floods office a bit)
	TEST_B	OfficeFlooded(A4)
	BEQ.S	300$
	TEST_B	AirlockFlooded(A4)
	BNE	90$			;both flooded, so ok
	MOVE.W	D0,STOPRON(A4)
	SCN	214			;@ won't budge
	BRA	80$
300$
	TEST_B	AirlockFlooded(A4)
	BEQ	90$			;no, skip
; by this time, office is empty, and airlock is full
	MOVE.W	#NHATCH,D0
	GETINFO
	TST_OPEN			;hatch open?
	BNE.S	32$			;yes!!
	SCN	125			;water in airlock away
	TEST_W	FloodPos(A4)		;flood happened?
	BNE.S	31$			;yes
	CALL	OfficeFloods		;just the first step
	CLR.B	TIMER9(A4)		;stop the flood
31$
	CLR.B	AirlockFlooded(A4)	;water gone
	CLR.B	UnderWater(A4)
	BRA.S	34$			;quit
32$
	SCN	134
	CALL	GetWet
	MOVE.B	#1,TIMER9(A4)
34$
	MOVE.W	#NAIRLOCK_DOOR,D0
	GETINFO
	MOVEQ	#0,D5			;no second noun
	BRA.S	90$			;exit

40$
	CMP.W	#NHATCH,D0
	BNE.S	50$
	TEST_B	AirlockFlooded(A4)
	BNE.S	90$			;ok
	MOVE.W	D0,STOPRON(A4)
	SCN	214			;@ won't budge
	BRA.S	80$			;quit

50$
	CMP.W	#NAUTODOORS,D0
	BNE.S	60$
	DO	YCANT		;Can't do that to the auto doors
	BRA.S	80$

60$
	CMP.W	#NPODOOR,D0	;Post Office door?
	BNE.S	70$

*** Dunno why  I did this - seems counterproductive if anything!
*	TEST_W	D5		;Indirect object?
*	BNE.S	90$		;I don't want to know, thankyou
***

	CALL	BellMuffled	;Has the bell been silenced?
	BEQ.S	62$		;=> yep, so no tinkle
	TST_LOCKED		; make sure door aint locked
	BNE.S	90$		; let verb handle
	SCN	325		;Verb has already made sure it's closed
* You hear the tinkle of a bell.^
62$
	MOVE.B	#2,TIMER8(A4)	;Door closes next move
	BRA.S	90$		;Let verb open it

70$
* Open oven door?
	CMP.W	#NOVENDOOR,D0
	BNE.S	90$
	SCN	84
* It can only be opened from the outside!
80$
	POPSP
90$
	RET


*
*
* OpenMailbox	-	Handles opening Xam's mailbox puzzle
*			Branched to from SP.OPEN and SP.UNLOC
*

		XDEF	OpenMailBox

OpenMailBox

	CMP.W	#NMAILBOX,D0		;Yours?
	BNE.S	5$
	CMP.W	#NKEY4,D5		;With deformed key?
	BNE	90$			;Nope, exit
	SCN	730
* The key doesn't fit anymore.

	BRA.S	80$
5$
	CMP.W	#NMAILBOX1,D0		;Xam's?
	BNE.S	90$
	TST_LOCKED				;Locked?
	BEQ.S	90$			;=> nope, not interested yet
	CMP.W	#NKEY1,D5		;Original plastic key?
	BNE.S	90$
	MOVE.B	3(A5),D1
	AND.B	#$F,D1
	CMP.B	#TEXTURE.HOT,D1		;Is it hot?
	BEQ.S	10$			;=> Yeah, he's cracked it!
	CMP.B	#TEXTURE.WARM,D1	;May be warm?
	BNE.S	75$			;=> no chance at all
	SCN	44
* You force the warm key into the lock with some degree of success.
* Unfortunately, the plastic hardens too soon and the key becomes jammed.

	MOVE.W	#NKEYRING,D0
	GETINFO
	DO	SETOUT			;Fix tied etc sensibly
	BCLR	#4,3(A0)		;!Moveable
	MOVE.L	#REL.MASK!NMAILBOX1,6(A0)	;Update it's location
	BRA.S	80$

10$
	SCN	43
* The hot plastic moulds itself to the contours of the lock. As it cools
* the key hardens once more, and, with a quick twist, the mailbox
* springs open.

	BCLR	#6,(A0)			;!locked
	BSET	#4,(A0)			;Open
	MOVE.W	#NKEY1,D0
	DO	SETOUT			;Lose the 'old' key...
	MOVE.W	#NKEY4,D0
	DO	SETIN			;...And bring in the new one
	SCORE	OPENMBX
	BRA.S	80$
75$
	SCN	428			; The key goes into the lock but doesn't					; quite fit
80$
	SETNE_RET
90$
	SETEQ_RET



*----------------------------

	XDEF	SP.OIL

	XREF	ODT_RUNNERS,STOPRON,ODT_SLIDINGDOOR


SP.OIL:
; Oiling the runners.
	CMP.W	#NOIL,D5		; With oil ?
	BNE.S	90$			; No, so 'with what?' or whatever.
	CMP.W	#NRUNNERS,D0		; either runners or door?
	BEQ.S	10$
	CMP.W	#NSLIDINGDOOR,D0
	BNE.S	80$
10$:
	SCN	200			; 'Looks like you got to them in time'
	BCLR	#7,ODT_RUNNERS(A4)	; Change desc of runners.
	BCLR	#7,ODT_SLIDINGDOOR(A4)  ; ... and of door
	ST	DoorOiled(A4)		; Set the flag.
	BRA.S	89$
80$:
; General oil stuff.
	AND.B	#$F0,3(A0)		; remove texture
	OR.B	#$05,3(A0)		; make it slippery
	SCN	196
	DO	P.TNSTOP		;hack by ken 26/10/87
89$:
	MOVE.W	D5,D0			; Lose the oil.
	DO	SETOUT	
	POPSP
90$:
	RET

*-----------------------------------------

	XDEF	SP.OPEN_After
	XREF	IsRoomWet,SEATED

SP.OPEN_After

	DO	IsRoomWet	;are we submerged?
	BEQ.S	90$		;not wet
	TEST_W	SEATED(A4)	;are you on a boat?
	BNE.S	90$		;yes, don't flood
	MOVE.L	D0,-(A7)	;parameter
	MOVE.L	#1,-(A7)	;nargs
	CALL	FloodObjects	;flood it
	ADDQ.L	#8,A7		;adjust stack
90$
	RET

*------------------------------------------

	XDEF	OpenCloseHatches

*		EQ <=> printed a message, exit

OpenCloseHatches

	CMP.W	#NINNER_HATCH,D0
	BEQ.S	10$
	CMP.W	#NOUTER_HATCH,D0	;can't just open these
	BNE.S	20$
10$
	MOVE.W	D0,STOPRON(A4)
	SCN	214			;@ won't budge
	SETEQ
20$
	RET

*------------------------------------------

      END
