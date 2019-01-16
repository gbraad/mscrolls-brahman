*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

	include	"equates.i"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"roomequ.i"
	include	"macros.i"
	include	"scenmacros.i"
	include	"scenequ.i"


	XDEF	SP.FOLLOW,SP.FEED,SP.FISH,SP.FLUSH,SP.FIX,SP.FEEL

	XREF	BurntCake,STOPRON

	XREF	VALID.NP,PUTD0IND5,P.TNSTOP

*****************************************************
SP.FEEL
	CMP.W	#NNATURESTUFF,D0	; to cope with pseudo problem
					; with the herbs berries etc..
	BNE.S	90$
	SCN	765
*	PRINT	<'They feel just like they should.'>
	POPSP
90$
	RET

*****************************************************
	XDEF	SP.FIXTO_exceptions
	XDEF	SP.FIXTOGETHER_exceptions
	XDEF	SP.FIX_exceptions

* to let u tie x,y together with z with less hassle
*
* gets here with D0=z D2=x D5=y (as per tie x to y wi z)
* 
* SETEQ to always allow, even if not in save loc

SP.FIXTOGETHER_exceptions

*	CMP.W	#NIRON_RING,D2		;can tie anything, anytime, anywhere
*	BEQ.S	90$			;to iron ring
*	CMP.W	#NIRON_RING,D5
*90$
	RET

; This allows exceptions for fixing things TO strange objects
SP.FIXTO_exceptions:

*	CMP.W	#NIRON_RING,D5
*	BEQ.S	90$
	CMP.W	#NBELL,D5
	BEQ.S	90$
	CMP.W	#NMANACLES,D5
90$
	RET

; This allows exceptions for fixing things WITH strange objects
SP.FIX_exceptions:

*	CMP.W	#NIRON_RING,D0
*	BEQ.S	90$
*	CMP.W	#NIRON_RING,D5
*	BEQ.S	90$
	CMP.W	#NDUNGEON_ROPE,D0
*90$
	RET

;*****************************************************
	XREF	SP.PICKUP,P.TNIN,P.TNSTOP

SP.FIX

	CMP.W	#NDUNGEON_ROPE,D0
	BNE.S	1$
	CMP.W	#NSOLID_DOOR,D5
	BNE.S	1$
	SCN	464
	POPSP_RET
1$
	CMP.W	#NSPARE_TANK,D0	;fix the spare tank
	BEQ	FixSpareTank

; Watch for "fix A to B, pick up B" as means of sneakily getting A.
	CALL	SP.PICKUP
	BNE.S	80$
	EXG	D0,D5
	CALL	SP.PICKUP
	EXG	D0,D5
	BEQ.S	90$

80$:
	POPSP
90$:
	RET

*******************************************************
*
* SP.FLUSH - toilet?
*

	XREF	P.LISTIS,COUNTLIST

SP.FLUSH

	CMP.W	#NLAVATORY,D0		;flush the loo?
	BNE.S	90$			;no, exit
	DO	GET_CONT		;->A1
	BCS.S	20$			;not a container
	DO	COUNTLIST		;->d4
	BEQ.S	20$			;no forget it
	MOVE.L	A1,A0
	ST	D5			;use 'the'
	DO	P.LISTIS		;the x y & z are
	CALL	FodList
	SCN	623			;flushed away.
	BRA.S	80$			;dont say that is flushed as wel
20$
	SCN	318			;it roors
80$
	POPSP
90$
SP.FISH	
	RET

*-------------------------------- 
*
* SP.FEED
*
* passed D0 = npc D5=food


	XREF	P.FLOO,UnicornFriendly,TIMERAFTER10
	XREF	BakerFed,BakerFollows,BakerAngry
	XREF	MovePlayer
    XREF    BurntChucked

	XREF	SP.GivePostMistress
SP.FEED
	EXG	D0,D5
	CALL	SP.GivePostMistress	; feeding her the mouse?
	EXG	D0,D5
	

20$:
	CMP.W	#NINSPECTOR,D0		;Feed ticket to inspector?
	BNE.S	SP.FeedBaker
	CMP.W	#NTICKET,D5
	BNE	SP.Feed_90
	CALL	P.FLOO
	BRA	SP.Feed_80	

	XREF	CharmInCake,BakerEject,BakerEject2,ROOM
	XDEF	SP.FeedBaker2		; called by SP.GIVE tin to baker

SP.FeedBaker
	CMP.W	#NBAKER,D0		; To baker??
	BNE	SP.FeedPublican
SP.FeedBaker2
	CMP.W	#RNBAKERY,ROOM(A4)	; In the bakery???
	BEQ.S	10$
	DO	P.SAY2
	SCN	423			; 'I can't see it well down here'
	DO	P.ENDQUOTE		; 'show it to me upstairs'
	SETEQ				; randomly
	BRA	SP.Feed_80
10$
	CMP.W	#NPUDDING2,D5		; Burnt pudding?
	BNE.S	20$
	SCN	344			; Chucks you out!
	EXG	D0,D5
	GETINFO
	MOVE.L	#RNFAIRGROUND,6(A0)	; put it in fairground
    ST      BurntChucked(A4)
	CALL	BakerEject2		; clears BakerFollows, moves u etc.
	SETEQ				; in case called from givecont2baker
	POPSP_RET			; cheaper than BRA SP.Feed_80
20$
	CMP.W	#NPUDDING,D5		; Good pudding???
	BNE	SP.Feed_90
	EXG	D0,D5
	DO	SETOUT			; It's eaten!
	SCN	343			; 'He eats the pudding!'
	TEST_B	CharmInCake(A4)		; charm in the good pudding ?
	BEQ.S	25$
	MOVE.W	#NCHARM4+BIT14,STOPRON(A4) ; no trailing space
	SCN	728			; Crunch! he breaks his teeth
	POPSP				; don't return to verb
	SETEQ
	BRA	BakerEject		; he throws you out
25$
	SCN	727			; he likes it
30$:
	CLR.B	BakerFollows(A4)	; No longer follows you.
	ST	BakerFed(A4)		; Make him FED
	SETEQ
	BRA	SP.Feed_80

;******************************************************************************

SP.FeedPublican:
; Giving drink to publican...?
	CMP.W	#NPUBLICAN,D0
	BNE.S	SP.Feed_90
	CMP.W	#NDRINK,D5
	BEQ.S	10$
	CMP.W	#NBEER,D5			; Feed to publican?	
	BNE	SP.Feed_90
10$:
	MOVE.W	#NDRINK,D0
	GETINFO
	DO	SETOUT				; roddy forgot this!!!
	MOVE.L	#REL.MASK+NPUBLICAN,6(A0)	; publican takes it
	SCN	123				; 'Thanks a lot'
						; but no other effect!
SP.Feed_80:
	POPSP

SP.Feed_90:
	RET


*-------------------------------- 

	XDEF	SP.DOOFER
	XREF	P.SAY2,P.ENDQUOTE,NPC.OK,DBS,P.SCN
	XREF	SpellWorks,FlyzzControl

SP.DOOFER

	MOVE.W	D0,-(A7)		;Stash our noun
	MOVE.W	#NCHARM4,D0		;The charm this spell needs
	CALL	SpellWorks		;Everything ok?
	PULL_W	D0			;With flags
	BNE.S	80$			;=> summit wrong, exit
	PUSH_L	D0/D5/A0/A5
	CALL	FlyzzControl
	PULL_L	D0/D5/A0/A5
	BEQ.S	10$			;=> didn't handle it
80$
	POPSP_RET

* Not in special-case table, try and do summit sensible.....

10$
	MOVE.W	#800,D1			;Base SCN msgs for flyzz cop-outs

* Fall Through.......................

*
*
* SpellCopouts	-	Used by Flyzz and Freeze to attempt a sensible
*			response to player casting these spells on
*			any given object.
*
*	Takes	D1	Base scenario msg of 4 contiguous cop-outs
*			for this spell.
*


SpellCopouts

	CMP.W	#NPLAYER,D0		;Trying to flyzz yerself?
	BNE.S	40$
	DO	P.SCN
	BRA.S	80$

40$
	ADDQ.W	#1,D1			;next msg
	TST_NPC				;Is it an NPC?
	BEQ.S	50$
	DO	NPC.OK			;How's life?
	BEQ.S	42$			;Fine thanks!
* Hmm, flyzzing/freezing dead NPC's - have to think about this one!
* There MAY never be any in this game - dunno.
	DO	P.TN			;'The NPC '
	SCN	371			;NB plural dead NPC's don't work
* twitches violently for a while, before becoming still once more.

	BRA.S	80$

42$
	DO	GETNPC			;-> A3
	BTST    #2,2(A3)		;Animal?
	BNE.S   44$			;=> yep
	DO	P.SAY2			;'The NPC says, "'
	DO	P.SCN
	DO	P.ENDQUOTE		;End the quotes sensibly
	BRA.S	80$

44$
	DO	P.TN			;The animal!
	SCN	358
* shudder@ violently for a few moments.

	BRA.S	80$

50$
	ADDQ.W	#1,D1			;Next msg
	TST_PSEUDO			;A pseudo?
	BEQ.S	60$
	DO	P.TN			;'The noun '
	DO	P.SCN
	BRA.S	80$

60$
	ADDQ.W	#1,D1			;next msg
	TST_INTRINSIC			;Intrinsic? head, feet and stuff
	BEQ.S	70$
	DO	P.SCN
	BRA.S	80$

70$
	TST_INSEPARABLE			;Inseperable?
	BEQ.S	90$			;VERB will do NHAP
	DO	DBS			;Hmmm

80$
	POPSP
90$
SP.FOLLOW
	RET

*-------------------------------

*
*
* Fire	-	Gets called each move via SPECIALS.
*		Checks to see if the player has started a fire or if
*		one has started by other means. If so it calls DoLight
*		on the room in question as long as the player isn't local.
*		If not, it branches to SpreadFire which determines
*		wether we have a fire in progress and acts accordingly.
*


*	XREF	Smoulder,StartFireTable,EndFireTable,CurrentWind
*	XDEF	Fire
*
*
*Fire
*	CLR.W	D1				;Coz of room# .W .B hassles
*	MOVE.B	Smoulder(A4),D1			;See if specials have flagged
*	BEQ.S	SpreadFire			;=> Nope
*	CMP.W	ROOM(A4),D1
*	BNE	DoLight				;NE=> Player not local
*	RET


*
*
* SpreadFire	-	Checks our table of rooms currently ablaze.
*			If there are any it calls DoRooms which spreads
*			the blaze according to wind direction and player
*			locality.
*

*WindMask	EQU	$F

*SpreadFire

*	LEA	StartFireTable(A4),A1
*	LEA	EndFireTable(A4),A2
*	CLR.W	D0				;Zap base room # to 0
*5$
*	MOVE.L	(A1)+,D3			;Get nth long word
*	BNE.S	10$				;NE=> Room ablaze
*	ADD.W	#32,D0				;Bump room # for next long word
*	CMP.L	A1,A2				;Finished?
*	BNE.S	5$				;=> Nope, loop
*	RET
*
*10$
*	CLR.W	D4				;Coz it gets added to An as .W
*	MOVE.B	CurrentWind(A4),D4		;Any wind?
*
*	IFNE	YCHEAT
*
*	BPL.S	15$
*	PRINT	<'Illegal wind direction in SpreadFire!'>
*	BRA.S	90$
*15$
*	ENDC
*
*	BEQ.S	90$				;=> Nope - Exit

* Here D0.W contains the base room # of the long word being considered and
* D3 contains the long word from which we calc the offset room # from D0.
* A1 points to the next long word, A2 points to the end of the table and
* D4.W holds the wind direction

*20$
*	PUSH_L	A1/D0				;A2 is safe so no push
*	CALL	DoRooms				;Do the meaty part
*	PULL_L	A1/D0

*	CMP.L	A1,A2				;Done all rooms?
*	BEQ.S	90$				;=> Yep - Exit
*	MOVE.L	(A1)+,D3			;Get next long word
*	ADD.W	#32,D0				;Bump base room #
*	BRA.S	20$
*90$
*	RET



*
*
* DoRooms	-	This is the meaty part which susses the real room #
*			from a base and a long word of bits, checks the
*			network and works out wether we have an exit in
*			the current wind direction which is okay to set
*			alight.
*
*	On Entry:	A1 points at the next long word to consider
*			A2 points at the end of the long word table
*			D0.W is a base room # for the current long word
*			D3.L contains the long word we are considering
*			D4.B contains the current wind direction
*
*	NB Must preserve A2 - else push it in SpreadFire


*	XREF	CALCNET2

*DoRooms
*10$
*	TEST_L	D3				;This lets us exit early if poss
*	BEQ.S	90$				;=> No more set bits - Exit

* Bump room # each shift - coz room #'s start at 1 this should just work!
*	ADDQ.B	#1,D0
*	LSR.L	#1,D3				;Test the current bit
*	BCC.S	10$				;=> Not set - try next

* Here we have encountered a set bit, the room # is in D0 and there's some wind


*	MOVE.W	D0,D2
*	DO	CALCNET2			;Get adjacent rooms ptr
*	ADD.W	D4,A0				;Point at exit in current
*						;wind direction
*	MOVE.B	(A0),D1
*	BEQ.S	10$				;0 => No exit in this dir
* Hmm, should we disallow NWAYS??
* Wet rooms need to be disallowed here

*	CMP.W	ROOM(A4),D1
*	BEQ.S	10$				;=> player local so loop

*	CALL_S	DoLight1			;Set fire to the room
*	BRA.S	10$
*90$
*	RET



*
*
* DoLight	-	FOD's smoulder flag and....
* DoLight1	-	Sets passed room # alight in table
*			
*	On Entry:	D1 contains room #
*	On Exit:	D2,D7 and A1 corrupted
*
*	NB Must preserve A2 - else push it in SpreadFire


* Internal REM: Maybe we should only do one at a time such that if the
* BSET below flags EQ then exit until next move else continue????
* Hmmmmm, maybe......

*DoLight
*	CLR.B	Smoulder(A4)			;Fod the smoudering room
*DoLight1
*	CALL_S	RoomToBits			;Get pointer + index
*	BSET.L	D2,D7				;Flag the room as burning
*	MOVE.L	D7,(A1)
*	RET


*
*
* RoomToBits	-	Given a real room number this will
*			return a pointer and an offset into
*			our table of bits representing the room #'s
*
*	On Entry:	D1.W = Room #
*
*	On Exit	:	A1.L -> Correct Long word of table
*			D2 gives offset from A1 in bits
*			D7 contains the long word
*			D1,A2 MUST be preserved
*
* Note - this could all be bytewise really.
*

*	XDEF	RoomToBits

*RoomToBits
*	MOVE.W	D1,-(A7)
*	LEA	StartFireTable(A4),A1
*	SUB.W	#1,D1				;Room #'s start at 1!!
*	MOVE.W	D1,D2				;Stash Target room #
*	AND.W	#$00E0,D2			;Byte really
*	LSR.W	#3,D2
*	ADD.W	D2,A1				;Get long word # 
*
*	MOVE.L	(A1),D7				;Hmmmmmmmmm
*
*	MOVE.W	D1,D2				;And the other half
*	AND.W	#$001F,D2			;Suss the bit #
*	MOVE.W	(A7)+,D1
*	RET



*-------------------------

	XDEF	FloodRoom

FloodRoom
	XREF	R.NOUN2,GETRELN2

* This general, thou' specialised routine, acts to FLOOD those rooms
* on the argument stack.
	
	LINK_	A6,#0
	PUSH_L	D0/D2/D4/A0-A2
	MOVE.L	8(A6),D4	;nargs
	ASL.L	#2,D4		;*4
	BEQ.S	90$		;none, exit
10$
	MOVE.L	8(A6,D4.W),D2	;get room#
	DO	R.NOUN2		;noun#-> d0
	MOVE.W	D0,D2		;target
	DO	GETRELN2	;all related objects (and more) ->a1
20$
	MOVE.W	(A1)+,D0	;get an object
	BEQ.S	30$		;all
	BMI.S	20$		;ignor those not related
	GETINFO			;on related object
	TST_MOVEABLE				;we'l dissalow non-moveable items
	BEQ.S	20$		;next
	TST_PSEUDO				;nor stray pseudos
	BNE.S	20$
	MOVE.L	8(A6,D4.W),6(A0)	;ok move that object!
	BRA.S	20$		;anymore?
30$
	;right!, now flood each item.
	MOVE.L	8(A6,D4.W),D2	;this time find those rel room#
	DO	GETRELN2	;those located in room ->a1
	MOVE.L	A1,-(A7)	;save start
35$
	MOVE.W	(A1)+,D0	;get object
	BEQ.S	40$		;end
	BMI.S	35$		;forget these
	GETINFO
	TEST_W	6(A0)		;must be simply in room
	BEQ.S	35$		;yes, leave it.
	BSET	#7,-2(A1)	;otherwise flag it
	BRA.S	35$		;next
40$
	MOVE.L	(A7)+,A1	;start again.
	CALL	PUSHBUF
	CALL	FloodObjects
	ADD.L	D2,A7		;adjust stack
	SUBQ.L	#4,D4		;-1 arg
	BNE.S	10$		;next
90$
	PULL_L	D0/D2/D4/A0-A2
	UNLINK_	A6		;back
	RET

FloodObjects

	XDEF	FloodObjects
	XREF	GET_CONT,DESITTABLE,TOPLOC,FIXCONT

* floods objects on parameter stack

	LINK_	A6,#0
	PUSH_L	D0/D2/D4/A0-A2
	MOVE.L	8(A6),D4	;nargs
	ASL.L	#2,D4		;*4
	BEQ	90$		;none exit
10$
	MOVE.L	8(A6,D4.W),D0	;get object
	GETINFO			;and data ->a0
	TST_PSEUDO				;no pseudos
	BNE.S	80$		;next
	TST_MOVEABLE				;moveable?
	BEQ.S	80$		;no, forget this one
	TST_SITTABLE				;sittable?
	BEQ.S	20$		;no
	DO	DESITTABLE	;not anymore!
	BSET	#3,5(A0)	;back again
20$
	TST_CLOSED				;closed?
	BNE.S	30$		;yes, skip containers.
	DO	GET_CONT	;contents ->a1
	BCS.S	30$		;not a container.
	CALL	PUSHBUF		;paramatrize buffer ->d2
	CALL	FloodObjects	;wow! proper recursion.
	ADD.L	D2,A7		;adjust
30$
	BSET	#5,3(A0)	;set as wet now.
	BCLR	#7,(A0)		;not lit anymore
	BEQ.S	31$		;not lit anyway
	AND.B	#$F0,3(A0)	;mask out texture
	OR.B	#9,3(A0)	;set as default= hard
31$
	DO	FIXCONT		;of item
	TST_INSEPARABLE				;inseparable?
	BNE.S	80$		;yes, leave it
	TST_WORNBYPLAYER				;worn by player?
	BNE.S	80$		;leave these as well.
	PUSH_L	D1/A0
	DO	TOPLOC		;of this item ->d0 (must be a room)
	PULL_L	D1/A0
	MOVE.W	D0,8(A0)	;move item to here (NB inv=> stays)
	CLR.W	6(A0)		;and relator
80$
	SUBQ.L	#4,D4		;-1 counter.
	BNE	10$		;next
90$
	PULL_L	D0/D2/D4/A0-A2
	UNLINK_	A6
	RET

*-----------------------------------

	XDEF	PUSHBUF

PUSHBUF

* buffer paramitrizer for recurive routines.
* converts a buffer of words (pointed to by a1)
* onto the stack of arguments for a call
* ie. it pushed nargs as well.
* note same stack frame as caller

* on exit D2=offset to add to the stack
* STOMPS on A2

	MOVE.L	(A7)+,A2	;fetch ret address
	CLR.L	D2		;counter
10$
	TEST_W	(A1)+		;end?
	BEQ.S	90$		;yes
	BMI.S	10$		;ignor, next
	MOVE.W	-2(A1),-(A7)	;stack this argnument.
	CLR.W	-(A7)		;no hi word
	ADDQ.W	#1,D2		;and the counter
	BRA.S	10$
90$
	MOVE.L	D2,-(A7)	;and nargs
	ADDQ.W	#1,D2		;take nargs into account.
	ASL.L	#2,D2		;convert to offset
	JMP	(A2)		;and return.

*----------------------------------

SP.Dungeon.rope

	XREF	RopeTied

	CMP.W	#NDUNGEON_ROPE,D0
	BNE.S	90$			;quit
	CMP.W	#NMANACLES,D5		;tie rope to manacles
	BNE.S	90$
;	GETINFO
;	MOVE.L	#$0100<<16+NDUNGEON,6(A0)	;related to dungoen
	SCN	556				;door opens
	ST	RopeTied(A4)
	MOVE.W	#NINNER_HATCH,D0
	DO	OpenDoor
80$
	SETNE_RET
90$
	SETEQ_RET
*-------------------------------------

	XDEF	SP.FIX_After

SP.FIX_After
	
	CALL	SP.Dungeon.rope
	BEQ.S	90$
80$
	POPSP
90$
	RET
*-----------------------------------

FixSpareTank
	
	XREF	SpareTank,AquaTank,VERB
	XREF	P.MHAV

	CMP.W	#NAQUALUNG,D5		;fix to aqualung ok.
	BNE.S	90$			;else noway
	CMP.W	#VNTIE,VERB(A4)		;not if tie
	BEQ.S	90$			;else back to verb
	GETINFO
	TEST_W	12(A0)			;fixed already?
	BEQ.S	10$			;no
	DO	P.BTN
	DO	ISARE
	MSG	ALRDF			;but it is already fixed.
	BRA.S	80$			;exit, no
10$
	DO	CARRIED			;must have tank
	BEQ.S	11$			;ok
12$
	POPSP
	DOGO	P.MHAV			;exit
11$
	EXG	D5,D0			;swap
	DO	CARRIED			;must have aqualung too
	BNE.S	12$			;else exit wi msg
	EXG	D5,D0			;swap back
	SCN	16			;you attatch it
	MOVE.W	D5,12(A0)		;ok fix it there.
	MOVE.B	SpareTank(A4),D1	;air left in spare tank
	EXT.W	D1
	ADD.W	D1,AquaTank(A4)		;onto main supply
80$
	POPSP
90$
	RET

*------------------------------------------

	XDEF	FodList

FodList

* fods items in a list pointed to by a1
* ignors hi-bit flagged ones

	PUSH_L	D0/A1
10$
	MOVE.W	(A1)+,D0
	BEQ.S	90$
	BMI.S	10$		;ignor
	DO	SETOUT		;fod it
	BRA.S	10$		;next
90$	
	PULL_L	D0/A1
	RET

*------------------------------------------


	XDEF	SP.OOJIMY
	XREF	FreezeControl

SP.OOJIMY

	MOVE.W	D0,-(A7)		;Stash our noun
	MOVE.W	#NCHARM3,D0
	CALL	SpellWorks		;Spell gonna work?
	PULL_W	D0			;With flags
	BNE.S	80$			;=> no can do and I handled it
	PUSH_L	D0/D5/A0/A5
	CALL	FreezeControl		;Do any specials
	PULL_L	D0/D5/A0/A5
	BNE.S	80$			;=> handled it
	MOVE.W	#804,D1			;Base scn # for SpellCopOuts
	BRA	SpellCopouts		;Try and do summit sensible with it
80$
	POPSP_RET

*-------------------------------

	END
