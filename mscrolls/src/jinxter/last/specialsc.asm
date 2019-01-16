*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSC
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


	XDEF	SP.CUT
	XDEF	SP.CATCH,SP.CLOSE,SP.CAST

	XREF	OPEN,CONJ,FlagLash,FlagHoldFence,ODT_FENCE,FlagFence
	XREF	TIMER9,OfficeFlooded,DO.SCORE

;*****************************************

SP.CUT:

	CMP.W	#NFENCE,D0		; Cut fence with shears ???
	BNE.S	90$
	CMP.W	#WITH,CONJ(A4)
	BNE.S	90$
	CMP.W	#NSHEARS,D5
	BNE.S	90$
	BCLR	#7,3(A0)		;Secateurs/shears now blunt
	BNE.S	5$			;=> were sharp
	SCN	37
* The secateurs are blunt!

	BRA.S	80$
5$
	BCLR	#7,FlagHoldFence(A4)	; Holding the fence?
	BNE.S	70$			;=> yep, he's done it! (maybe!)
10$
	ESCAPE	FENCE
	BRA.S	75$			;Fence obstacle is removed by escape

70$:
	MOVE.W	#NGLOVES,D0
	GETINFO
	CMP.L	#WORN.MASK,6(A0)	;He may have held the fence and
	BNE.S	10$			;dropped gloves!!!
	SCN	38			; We've done it!
	SCORE	DEFEATBULL		; effectively the same
75$
	BCLR	#7,ODT_FENCE(A4)	; Other fence description
	ST	FlagFence(A4)		; Tell setmode that it's cut
80$:
	POPSP_RET
90$:
	CMP.W	#NROSES,D0
	BNE.S	95$
	SCN	80			;but the roses are far too pretty!
	POPSP
95$
	RET

;********************************************

	XREF	SP.SUCK
	XDEF	SP.CHEW

SP.CHEW
	CMP.W	#NTICKET,D0		; Chewing the ticket??
	BNE.S	90$
	BSET	#5,3(A0)		;Make it wet
	SCN	70
* Old habits die hard eh?

80$
	POPSP
90$
SP.CATCH
	RET
;********************************************
*
*
*SP.CLOSE
*	XREF	ROOM,OpenCloseHatches
*	XREF	TIMER15
*
*	CMP.W	#NFRIDGE,D0		; closeing fridge...?
*	BNE.S	001$
*	MOVE.B	#20,TIMER15(A4)		; soon.....
*001$
*	CALL	OpenCloseHatches
*	BEQ	80$
*	CMP.W	#NSOLID_DOOR,D0		;close dungeon door
*	BNE.S	01$
*	CMP.W	#RNDUNGEON,ROOM(A4)	;in dungeon cell?
*	BNE.S	01$			;no ok.
*	SCN	569			;no handle here.
*	BRA.S	80$
*01$
*	CMP.W	#NAUTODOORS,D0		; You can't close bus doors.
*	BNE.S	10$
*	DO	YCANT
*	BRA.S	80$
*10$
*
*20$
*	CMP.W	#NHATCH,D0		;close hatch
*	BEQ.S	21$			;same as closing airlock door
*
*	CMP.W	#NAIRLOCK_DOOR,D0
*	BNE.S	30$
*21$
*	TEST_B	OfficeFlooded(A4)	;flood over?
*	BNE.S	90$			;yes, ok
*	TEST_B	TIMER9(A4)		;any flooding?
*	BEQ.S	90$			;no
*	SCN	216			;cant because of flood
*	BRA.S	80$			;quit
*30$
*; Surely, your mailbox should do this as well?
*	CMP.W	#NMAILBOX1,D0		;Closing Xam's mailbox?
*	BNE.S	40$
*	BSET	#6,(A0)			;Lock it
*	BNE.S	90$			;Already locked
*	BCLR	#4,(A0)			;!open
*	SCN	280
** The mailbox latches shut, locking itself.
*
*	BRA.S	80$
*
*40$
*	CMP.W	#NPODOOR,D0		;Post office door?
*	BNE.S	50$
*	CALL	BellMuffled		;Will it ring?
*	BEQ.S	90$			;=> nope
*	SCN	325
** You hear the tinkle of a bell.^
*
*	BRA.S	90$			;Let verb close it
*50$
** close can or tin?
*	CMP.W	#NCANOWORMS,D0
*	BNE.S	90$
*52$
*	DO	YCANT			;You can't do that to the can/tin
*80$:
*	POPSP
*90$:
*	RET
*
*
*
********************************************


	XREF	ROOM,OpenCloseHatches
	XREF	TIMER15

SP.Close_Fridge

	MOVE.B	#20,TIMER15(A4)		; soon.....
	RET

SP.Close_Solid_Door

	CMP.W	#RNDUNGEON,ROOM(A4)	;in dungeon cell?
	BNE.S	90$			;no ok.
	SCN	569			;no handle here.
	POPSP
90$
	RET



SP.Close_AirLockDoor

	TEST_B	OfficeFlooded(A4)	;flood over?
	BNE.S	90$			;yes, ok
	TEST_B	TIMER9(A4)		;any flooding?
	BEQ.S	90$			;no
	SCN	216			;cant because of flood
80$
	POPSP
90$
	RET

SP.CloseMailBox1

	BSET	#6,(A0)			;Lock it
	BNE.S	90$			;Already locked
	BCLR	#4,(A0)			;!open
	SCN	280		;The mailbox latches shut, locking itself.
	POPSP
90$
	RET

	XREF	ClosePODoorBehind,ShutTheDoor,IntroPMistress

SP.Close_PODoor

	CALL	BellMuffled		;Will it ring?
	BEQ.S	90$			;=> nope
	SCN	325			;You hear the tinkle of a bell.^
	BSET	#7,ShutTheDoor(A4)	;flag to ClosePODoorBehind
	BSET	#7,IntroPMistress(A4)	;ditto
	POPSP				;zap verb
	BRA	ClosePODoorBehind	;close door
90$
	RET				;Let verb close it

SP.Close_AutoDoors
SP.Close_Shutter
SP.Close_CanOWorms

	POPSP
	DOGO	YCANT			;You can't do that to the can/tin


	XREF	FlagWitchDoorOpen

SP.Close_WitchDoor

	CLR.B	FlagWitchDoorOpen(A4)	;So OPEN knows where it stands
	RET

	XREF	DoorOiled

SP.Close_SlidingDoor
	TEST_B	DoorOiled(A4)		; has door been oiled ?
	BNE.S	10$
	SCN	168			; Creeeeak
	RET
10$
	SCN	169			; silently ...
	RET


SP.CLOSE

	CALL	OpenCloseHatches
	BEQ.S	80$

	CaseOf	D0
	When	NFRIDGE,SP.Close_Fridge-*
	When	NSOLID_DOOR,SP.Close_Solid_Door-*
	When	NAUTODOORS,SP.Close_AutoDoors-*
	When	NHATCH,SP.Close_AirLockDoor-*	;same as closing airlock door
	When	NAIRLOCK_DOOR,SP.Close_AirLockDoor-*
	When	NMAILBOX1,SP.CloseMailBox1-*
	When	NPODOOR,SP.Close_PODoor-*
	When	NCANOWORMS,SP.Close_CanOWorms-*
	When	NWITCH_DOOR,SP.Close_WitchDoor-*
	When	NWITCH_DOOR1,SP.Close_WitchDoor-*
	When	NSLIDINGDOOR,SP.Close_SlidingDoor-*
	When	NWOODEN_SHUTTER,SP.Close_Shutter-*
	EndCase
	
	RET
80$
	POPSP_RET


	XREF	CloseDirection,NwayandDoor	

CloseStrangeNway:
* this will loop around a small table that is found in scendata.i
* checking the object out with one in the table:- if it is it will 
* call CloseDirection that handles it.
	LEA	NwayandDoor(A4),A3
10$
	TEST_W	(A3)
	BMI.S	99$			; help a finish!
	CMP.W	(A3)+,D0		; found one?
	BEQ.S	20$
	ADDQ.W	#2,A3
	BRA.S	10$			; loop around again
20$
	MOVE.W	(A3),D2			; the nway required
	BRA	CloseDirection
99$
	RET

*
*
* BellMuffled	-	Susses whether the bell has been silenced
*			or not.
*
*	RETURNS		EQ	=>	Muffled
*			NE	=>	!Muffled
*

	XDEF	BellMuffled

BellMuffled
	PUSH_L	D0/A0
	MOVE.W	#NSOCK,D0
	GETINFO
	CMP.L	#CONT.MASK!NBELL,6(A0)		;Sock in bell?
	BEQ.S	90$			;=> yep
	CMP.W	#NBELL,12(A0)		;or fixed to it?
	BNE.S	99$
90$
	SCORE	MUFFLEBELL
	SETEQ
99$
	PULL_L	D0/A0
	RET

;********************************************

	XDEF	SP.CAST_Before
	XREF	Do_Aqua,Do_Sol

*!!!!  NB This is NOT allowed to POP UNLESS It's one of the SINGV spells!!!!

SP.CAST_Before

* NB No ROOMTODANCE call here coz you NEED sol & aqua when seated

	CMP.W	#NINT_SOL,D0
	BEQ.S	10$
	CMP.W	#NINT_AQUA,D0
	BNE.S	90$			;Back to verb
	CALL	Do_Aqua			;Do the stuff
	BRA.S	20$
10$
	CALL	Do_Sol			;Do the stuff
20$
	BNE.S	80$			;Do_xyzzy did summit
	MSG	NHAP			;'Nothing happens'
80$
	POPSP
90$
	RET


*------------------------------------

	XREF	SP.DOOFER,SP.WATCHERCALLIT,SP.OOJIMY

SP.CAST

	CMP.W	#NINT_FLYZZ,D0		;Cast Flyzz?
	BNE.S	10$
	CALL	Do_Swap

* Fake up a 'FLYZZ xyzzy'
	BRA	SP.DOOFER

10$
	CMP.W	#NINT_DRACMAZZA,D0
	BNE.S	20$
	CALL	Do_Swap

* Fake up a 'DRACMAZZA xyzzy'
	BRA	SP.WATCHERCALLIT

20$
* Fake up a 'FREEZE xyzzy'
	CMP.W	#NINT_FREEZE,D0
	BNE.S	90$
	CALL	Do_Swap
	BRA	SP.OOJIMY

90$
  	RET


Do_Swap
	EXG	D5,D0
	EXG	A5,A0
	CLR.W	D5			;So SP.FLYZZ/DRACMAZZA CONJ etc
	RET


*----------------------------------

	XDEF	SP.CLOSE_After

SP.CLOSE_After

	BRA	CloseStrangeNway

*------------------------------------------

	XDEF	SP.COUNT

SP.COUNT
	CMP.W	#NRUNGS,D0
	BNE.S	90$
	SCN	794
	POPSP
90$
	RET

*------------------------------------------


	END

