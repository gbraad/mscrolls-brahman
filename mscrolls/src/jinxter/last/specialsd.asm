*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSD
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


	XDEF	SP.DIG

	XREF	ROOM,SUBJECT,SETOUT,SETIN,ISIN,Do_Dirt
	XREF	DO.SCORE

SP.DIG
	CALL	Do_Dirt			;Fiddling with one of the dirt mounds?
	BNE.S	90$
	POPSP
90$:
	RET

;***************************************
;
	XDEF	SP.DIAL

	XREF	GetNumber,FlagDialled,TIMER3,P.SCN,RND.D1
	XREF	TelephoneRung
SP.DIAL   


	CMP.W	#RNLIBRARY1,ROOM(A4)	; in a room with a phone?
	BEQ.S	10$
	CMP.W	#RNSTUDY1,ROOM(A4)
	BNE	99$
10$
	TEST_B	RingCount(A4)		;Is it ringing?
	BEQ.S	15$			;=> nope
	SCN	605
* But the phone is ringing!
	BRA	85$
15$
	CALL	GetNumber
	TEST_W	D2			;Hmmm 0 isn't a number eh?
	BEQ	90$			;can cause range error on 68k
	SCN	607
* You lift the receiver and dial.^
	CMP.W	#999,D2			; Phone 999
	BNE.S	20$
	SCN	49
	BRA.S	80$
20$:
	CMP.W	#234,D2			; Phone XAM
	BNE.S	25$
	TEST_B	TelephoneRung(A4)	
	BNE.S	22$			; Has he called you??
	SCN	48			; Do engaged until he calls you
	BRA.S	80$
		
22$:					; ... then do no answer
	SCN	47
	BRA.S	80$
25$:
	CMP.W	#300,D2			; Phone postman
	BNE.S	30$
	TEST_B	FlagDialled(A4)		; Already done?
	BNE.S	70$
	MOVE.W	#NPOSTAL_NOTE,D0
	DO	ISIN			;Postman left note?
	BNE.S	70$			;=> nope
*	TEST_B	OpenMbx(A4)		; found note?
*	BEQ.S	70$
	ST	FlagDialled(A4)		; Say we've done it
	SCN	45
	MOVE.B	#4,TIMER3(A4)		; Kick off postie
	BRA.S	80$
30$:
	CMP.W	#192,D2			; Directory enquiries?
	BNE.S	70$
	SCN	164
	BRA.S	80$
70$:
	MOVE.B	#4,D1			; One time in 4, do a silly one!
	DO	RND.D1
	TEST_B	D1			; A silly one this time??
	BNE.S	75$
	MOVE.B	#2,D1			; Generate # of silly message
	DO	RND.D1
	ADD.W	#165,D1
	BRA.S	77$
75$:
	MOVE.B	#3,D1			; Standard no answer type of msg.
	DO	RND.D1
	ADD.W	#46,D1
77$:
	DO	P.SCN
80$:
	SCN	606
* ^You replace the receiver.

85$
	POPSP_RET

90$:
	SCN	64			; Dial a number please!
	BRA.S	85$

99$:
	SCN	63			; Nothing suitable
	BRA.S	85$

;*****************************************


	XDEF	SP.WATCHERCALLIT
	XREF	DracmazzaControl,SpellWorks,P.MHAVSTOP

SP.WATCHERCALLIT

	PUSH_W	D0			;Stash our noun
	MOVE.W	#NCHARM2,D0
	CALL	SpellWorks		;Spell gonna work?
	PULL_W	D0
	BNE.S	80$			;=> no can do
	TEST_L	6(A0)			;Can only drac simply carried things
	BEQ.S	10$			;=> ok, no catches
	CALL	P.MHAVSTOP
	BRA.S	80$
10$
	MOVE.B	2(A0),D1
	LSR.B	#4,D1			;get size.
	CMP.B	#2,D1			;bigger than 2 not allowed!
	BLS.S	11$			;small enough.
	DO	P.TN
	DO	ISARE
	MSG	2BIG
	DO	BSFULLSTOP
	POPSP_RET
11$
	PUSH_L	D0/D5/A0/A5
	CALL	DracmazzaControl	;Do any specials
	PULL_L	D0/D5/A0/A5
	BEQ.S	90$			;=> didn't handle it
80$
	POPSP
90$
	RET				;Verb will do NHAP

*-------------------------------



	XDEF	SP.DRINK
	XREF	BoozedUp,TIMER13,StomachLined

SP.DRINK
	
	CaseOf	D0
	When	NDRINK,SP.DRINK_BEER-*
	When	NBEER,SP.DRINK_BEER-*
	When	NMILK,SP.DRINK_MILK-*
	When	NWATER,SP.DRINK_WATER-*
	EndCase
	RET

SP.DRINK_BEER

	MOVE.W	#NBEER,D0
	GETINFO
	TST_OUT				; OUT?
	BNE.S	10$
	SCN	333			; 'Wow - powerful stuff!'
	BRA.S	GetPissed
10$
	SCN	601
	POPSP_RET

SP.DRINK_MILK

	ST	StomachLined(A4)	; okay all done
 	RET

SP.DRINK_WATER

	SCN	732			;the water isn't very nice
	POPSP_RET


	XDEF	GetPissed
GetPissed
	TEST_B	StomachLined(A4)	; lined stomach
	BNE.S	90$			; wheeew. no pissed.
	TEST_B	BoozedUp(A4)		; if sober then ...
	BNE.S	10$
	MOVE.B	#10,TIMER13(A4)		; start timer...
	ADDQ.B	#1,BoozedUp(A4)		; Make initial value = 2 (tipsy)
10$:
	ADDQ.B	#1,BoozedUp(A4)		; else increment counter!
90$
	RET

*---------------------------------------------

	XDEF	SP.DROP_Before

* This happens right at the top of DROP.  Point is that it happens
* before drop checks for STEADFAST etc. This means we can get round
* the DROP KEYS hassle of KEY1 steadfastly blah blah, KEY2 steadfast..
* etc. NB, it shouldn't POP coz it may be a drop x INTO y

	XDEF	Do_Keys
	XREF	WHOOPS

SP.DROP_Before

Do_Keys

* NB Also called from SP.PICKUP, since it needs to do the same
* Don't mess with this without looking at SP.PICKUP coz you
* may stop the bus running the player over!

	CMP.W	#NKEY1,D0		;if it's one of the keys...
	BEQ.S	10$
	CMP.W	#NKEY2,D0
	BEQ.S	10$
	CMP.W	#NKEY3,D0
	BEQ.S	10$
	CMP.W	#NKEY4,D0		;Key4 is replacement plastic one
	BNE.S	90$
10$
	ST	WHOOPS(A4)		;Kill multiples...
	MOVE.W	#NKEYRING,D0		;and pretend it was the keyring.
	GETINFO
90$
	RET





;******************************************
	XDEF	SP.DROP,GetRidofBull

	XREF	SP.UNWEA,TIMER1,FlagBull,SETPRON,TIMER7

SP.DROP

	CMP.W	#NMOUSE_TRAP,D0		; dropping the trap?
	BNE.S	5$
	MOVE.B	#4,TIMER7(A4)		; timer
5$
	CMP.W	#NTABLECLOTH,D0		; Dropping curtain?
	BNE.S	Drop_90
	TEST_B	TIMER1(A4)		; Is bull charging?
	BEQ.S	Drop_90

GetRidofBull:

	SCN	23			; Stop charge!
	CLR.B	TIMER1(A4)
	DO	SETOUT			; Lose old curtain
	MOVE.W	#NBULL,D0
	DO	SETOUT			; And old bull
	MOVE.W	#NBULL2,D0
	CALL	SETPRON
	DO	SETIN			; Set new bull in!
	ST	FlagBull(A4)		; Bull disposed of.
	SCORE	DEFEATBULL
Drop_80
80$
	POPSP
90$:
Drop_90
	RET



****************************************
*
*
* SP.Dismount	-	Loosely maps to 'get off xyzzy' as well
*
*

	XDEF	SP.DISMOUNT

	XREF	GO.LEAVE

SP.DISMOUNT
	CMP.W	#RNONBUS,ROOM(A4)	;Are we on the bus?
	BNE.S	90$
	CMP.W	#NBUS,D0		;Said 'get off bus'?
	BEQ.S	10$
* What about a straight 'get off'?...
	TEST_W	D0
	BGE.S	90$
10$
	MOVE.W	#NAUTODOORS,D0		;Fool GO.LEAVE
	GETINFO
	POPSP				;Sensible stack please
	CALL	GO.LEAVE		;Hmmmmmmmmmmmm

90$
	RET


*--------------------------

	XDEF	SP.DANCE
	XREF	TapDancing,ROOM,WakeUpGaoler.DO

SP.DANCE

	TEST_B	TapDancing(A4)		;already
	BEQ.S	10$
	SCN	461			;already dancing
	BRA.S	80$
10$
	CMP.W	#RNOUTSIDE_STATION,ROOM(A4)	;otherwise begin busk
	BNE.S	20$
	SCN	441			;more entertaining
	BRA.S	80$
20$
	CMP.W	#RNGAOL,ROOM(A4)	;danceing in the gaol?
	BNE.S	90$
	SCN	539			;wakes up
	SCN	598			; and takes off you...
	BRA	WakeUpGaoler.DO		;he wakes
80$
	POPSP
90$
	RET	

*------------------------------

*
*
* Do_Event	-	Called when we've decided that an event is
*			to occur. Works out which one it is and calls
*			it. Also prints relevant text and sets end of event
*
*	Takes	D0	-	Event Number
*		D2	-	Room Number of event (current room)
*


	XDEF	Do_Event
	XREF	LIGHT,EventNum,GETWOSSNAME,FlagEventDone
	XREF	EndEventMSG,TIMERAFTER14,Waiting

BaseSCN		EQU	645		;All text is contiguous (starting here)

Do_Event

	TEST_W	LIGHT(A4)		;If no light source, exit
	BEQ.S	90$
	MOVE.L	FlagEventDone(A4),D3
	BTST	D0,D3			;Already had this one?
	BNE.S	90$			;=> yep, exit
	LEA	Event.LST(PC),A1
	ASL.W	#1,D0			;Event # as index
	MOVE.W	D0,D1			;stash for llook and p.scn
	ADD.W	D0,A1			;Point to correct entry
	MOVE.W	(A1),D0			;Get PC rel offset
	JSR	0(A1,D0.W)		;Go to routine
* SP code must preserve D3.L
	BNE.S	90$			;SP code says no msg's, exit
	CLR.W	Waiting(A4)
	MOVE.W	EventNum(A4),D1
	BSET	D1,D3			;Say this has happened
	MOVE.L	D3,FlagEventDone(A4)
	ASL.W	#2,D1			;*4 (4 messages / event)
	ADD.W	#BaseSCN,D1		;Get llook msg # for this event
	CALL	SetLLookGuardian
	CALL	GETWOSSNAME		;Enter guardian
	ADDQ.W	#1,D1			;Bump for start event msg
	MOVE.W	D1,-(A7)		;Waaaaaaa
	DO	P.SCN
	MOVE.W	(A7)+,D1
	ADDQ.W	#1,D1			;and agan for end event msg
	MOVE.W	D1,EndEventMSG(A4)	;msg # for timer to print
	MOVE.B	#2,TIMERAFTER14(A4)	;Prime end event timer
90$
	RET



*
* DefEvent macro lays down the address of some code to cope with
* the relevant event.
*

DefEvent	MACRO	*SP.xyzzy

	DC.W	\1-*

	ENDM


Event.LST

	DefEvent	SP.Phone		;Library1
	DefEvent	SP.CanoeMooring		;canoe_mooring
	DefEvent	SP.Magpie		;hollybush
	DefEvent	SP.Mailbox		;frontgarden2
	DefEvent	SP.BoatHouse		;Boathouse
	DefEvent	SP.Dirt			;Steps1
	DefEvent	SP.Girder		;Clock
	DefEvent	SP.Platform		;On_platform
	DefEvent	SP.EndBeam		;end_of_beam
	DefEvent	SP.Pub			;Pub
	DefEvent	SP.Station		;Train_station
	DefEvent	SP.MountainTop		;Top_mountain
	DefEvent	SP.Dungeon		;Dungeon
	DefEvent	SP.PostOffice		;Postoffice
	DefEvent	SP.StationBridge	;Platform_bridge


*--------------------------------

	XREF	AnswerPhone,RingCount

SP.Phone
* This one is a special case coz it's just text (always exits NE)

	TEST_B	RingCount(A4)			;Phone ringing?
	BEQ.S	85$				;=> nope, exit NE
	SCN	646
* Guardian answers the phone

	CALL	AnswerPhone			;Xam's call
85$
	SETNE_RET				;This is all we want

*-------------------------

	XREF	CanoeFilled

SP.CanoeMooring

	TEST_W	CanoeFilled(A4)			;Hole been stuffed?
	RET					;exit with Z flag

*------------------------

	XREF	GotSpecs

SP.Magpie

	TEST_B	GotSpecs(A4)			;Solved this?
	RET					;ret with Z flag set up


*------------------------

SP.Mailbox

	MOVE.W	#NMAILBOX1,D0
	GETINFO
	TST_LOCKED				;Locked?
	BEQ.S	90$				;nope, exit NE
	MOVE.W	#NPARCEL,D0
	GETINFO
	TST_OUT				;parcel in?
	BNE.S	90$				;nope, exit NE
	CMP.W	#NMAILBOX1,8(A0)		;still in mailbox?
	BNE.S	90$				;nope, exit NE

	SETEQ_RET				;EQ if we get here anyhow
90$
	SETNE_RET


*------------------------

	XREF	DoorOiled

SP.BoatHouse

	TEST_B	DoorOiled(A4)			;Door been oiled?
	RET					;With Z flag


*------------------------

	XREF	DirtGone

SP.Dirt

	TEST_B	DirtGone(A4)			;Solved the puzzle?
	RET					;ret with Z flag set up

*------------------------

SP.Girder

	MOVE.W	#RNGIRDER_ROOM,D2
	BRA.S	DoExploredRoom			;(implied ret) Z flag set up

*-----------------------

SP.Platform

	MOVE.W	#RNEND_OF_BEAM,D2
	BRA.S	DoExploredRoom			;(implied ret) Z flag set up

*--------------------------------

	XREF	GoneCloud

SP.EndBeam

	TEST_B	GoneCloud(A4)			;Been on cloud?
	RET

*-----------------------------

	XREF	FlagPubHearthCold

SP.Pub

	TEST_B	FlagPubHearthCold(A4)		;Frozen it yet?
	RET					;With Z flag

	
*---------------------------

	XREF	Stationmaster_givein

SP.Station

	TEST_B	Stationmaster_givein(A4)	;Solved it?
	RET					;ret with z flag set up

*---------------------------

SP.MountainTop

	MOVE.W	#RNWALLED_GARDEN,D2
	BRA.S	DoExploredRoom			;implied ret with Z flag

*---------------------------


SP.Dungeon

	MOVE.W	#RNCASTLE_KITCHEN,D2
	BRA.S	DoExploredRoom			;implied ret with Z flag

*---------------------------

SP.PostOffice

	MOVE.W	#NPOSTMISTRESS,D0
	DO	ISIN				;Post mistress about?
	BEQ.S	90$				;=> yes, exit NE
	MOVE.W	#NSAFE,D0			;Safe been cracked?
	GETINFO
	TST_LOCKED				;Locked?
	BEQ.S	90$				;=> nope, exit NE

	SETEQ_RET

90$
	SETNE_RET

*---------------------------

SP.StationBridge

	CLR.W	Waiting(A4)
* Special case event - just text
	SCN	702
* Guardian turns up and encourages you to jump

	MOVE.W	EventNum(A4),D1		;Ensure it doesn't happen again
	BSET	D1,D3			;D3 has long word flag in it
	MOVE.L	D3,FlagEventDone(A4)	;Restore flag with right bit set
	SETNE_RET			;No further action (ALWAYS!)
	
*---------------------------


*
*	This is just a factored piece of code coz several of the above
*	Need to do almost the same thing. Also called from elsewhere now
*
* D2	-	Room # to check for explored
* returns Z flag after btst on explored bit
*

	XDEF	DoExploredRoom

DoExploredRoom

	DO	R.NOUN2
	GETINFO
	TST_EXPLORED				;Explored?
	RET					;ret with Z flag set up

*------------------------



*
*
* SetLongLook	-	Sets up the spiel tagged onto 'The guardian is...'
*			for the various events.
*			Takes D1 as SCN # for longlook entry
*


	XREF	EntryLongLook

SetLLookGuardian

	MOVE.W	D1,-(A7)			;Stash D1
	SWAP	D1				;LongLook takes hi word SCN #
	MOVE.W	#NGUARDIAN,D1			;...and low word as noun
	CALL	EntryLongLook			;Go set it up
	MOVE.W	(A7)+,D1			;Recover original SCN #
	RET


*-----------------------

      END


