******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SPECIALS4
*
*--------------------------------




	include	"equates.asm"
	include	"nounequ1.i"
	include	"roomequ.i"
	include	"macros.asm"
	include	"scoreref.asm"

	XREF	P.SCN,SETOUT,DEAD,DO.SCN,GETDATA,P.TN,PRTTXT,UTIL.BS,WHOOPS
	XREF	FlagGem,P.OBJPRN,STOPRON,ROOM,NOUNBUF3,FlagBeam,CARRIED
	XREF	TIMER5,SEATED,SCRPNT,NOUNBUF,NOUNBUF2,CONJ,SETPRON,FGT_Cage
	XREF	Flooded,Flooding,FloodClock,FGT_System1,FGT_System2,TIMER6
	XREF	FlagChampers,ODT_BEAR,GETNPC,DO.SCORE,BlackHoles,SCORE_,DOBREAK
	XREF	SP.SCORE.TREASURE,IsTreasure,GET_CONT,TIMERF,TIMER7
	XREF	P.TNSTOP,PUTD0IND5,FIXCONT,DBS,OpenDoor,TIMER16
	XREF	SP.PRESS,SP.PRESS2

      XDEF     SP.POINT,SP.SHINE,SP.MIX,SP.SPLIT,SP.HIT,SP.LOOK
      XDEF     SP.CATCH,SP.EXIT,SP.CLOSE,SP.UNTIE,SP.HELP
      XDEF     SP.OPEN,SP.THROW,SP.FEED,SP.HOLD,SP.REFLECT,SP.FIX
      XDEF	StartFlood,SP.BRKD0,SP.JOIN,SP.HIT2

        XDEF    HoldMirror
        XREF    CANTSAVE
        
* SP.BRKD0 - special called by BRKD0 to give a different message from
*            'the xyzzy breaks'
*
* if it prints something ZERO should be set on exit

SP.BRKD0

	XREF	SP.BREAK

	PEA	10$(PC)
	CALL	SP.BREAK		;hmmm
	POPSP
	SETNE
	BRA.S	20$
10$
	SETEQ
20$
	RET


	XREF	SpotBits
	XREF	BSFULLSTOP

SP.LOOK

	CMP.W	#NDIE5,D0		;looking at the plastic die?; 
	BNE.S	99$
	CMP.B	#4,SpotBits(A4)		; and nothing changed?
	BEQ.S	99$
	CMP.B	#63,SpotBits(A4)	; all gone
	BNE.S	5$
	SCN	165
	BRA.S	80$
5$
* Okay print out how many faces have changed...
	CLR.W	D4
	SCN	163			; the dice is normal except..
10$
	BTST	D4,SpotBits(A4)		; EQ => STILL THERE
	BNE.S	20$
	MOVE.W	#568,D1			; The start of the numbers.
	ADD.B	D4,D1			; index it into the one we want
	DO	P.SCN			; print scen number
20$
	ADDQ.W	#1,D4		
	CMP.W	#6,D4			; Too many
	BNE.S	10$
	DO	UTIL.BS			; backspace over space
	DO	BSFULLSTOP		; backspace over comma
80$
	POPSP
99$
	RET


SP.HOLD
        CALL    HoldMirror
        BNE.S   99$
	POPSP
99$
	RET

HoldMirror
	CMP.W	#NMIRROR,D0		;hold mirror?
	BNE.S	99$
	CMP.W	#NBEAM,D5		;in beam?
	BNE.S	99$
	SCN	360
	MOVE.B	#1,FlagBeam(A4)		;for turn and reflect etc.
        MOVE.B  #$1A,3(A0)              ;set mirror warm
        SETEQ 
99$
	RET        

SP.REFLECT
	CMP.W	#NBEAM,D0	;we can only use the beam in this way!
	BNE.S	99$
	TEST_W	NOUNBUF3(A4)		;eq = no sixth part!
	BEQ.S	50$
	CMP.W	#NMIRROR,NOUNBUF3(A4)	;the mirror?
	BEQ.S	10$
	CMP.W	#NMIRROR,D5		;is that a mirror?
	BNE.S	99$			;urk no go guv!

	MOVE.W	NOUNBUF3(A4),D5		;put it right
	MOVE.W	#NMIRROR,NOUNBUF3(A4)	;it is a mirror
10$
* okay we have a 6 part sentance....
* which has been changed around to look like
* reflect x off y with z
* and z is a mirror and x is a beam
	CMP.W	#NWAX,D5		;is it wax
	BEQ	SP.Beam			;yeah
	MOVE.W	D5,STOPRON(A4)		;for the @
	SCN	362			;the beam hits the y glowin merr
	BRA.S	90$
50$
* Get here with 2 part sentances...
	CMP.B	#1,FlagBeam(A4)		;if 1 then goto 10..
	BEQ.S	10$	
	CMP.W	#NMIRROR,D5		;reflect beam off mirror
	BNE.S	60$
	SCN 	356			;they glare at each other
	BRA.S	90$
60$
	SCN	364			;you cannot manipulate beam..
90$
	POPSP
99$
	RET

 

SP.CATCH
* are we trying to catch a falling gem?
	CMP.W	#NGEM,D0
	BNE.S	99$
	TEST_B	FlagGem(A4)		;ne=> the gem is falling...
	BEQ.S	99$
	CLR.B	FlagGem(A4)

        ;; now done in SP.Beam
*	GETINFO				;get the info on the gem
*	MOVE.L	#RNRMUD,6(A0)           ;set it to be in the room
*	BSET	#4,3(A0)		;make it moveable
	SCORE	CATCHGEM		; catch the gem
99$
	RET

SP.SHINE
	CMP.W	#RNRMUD,ROOM(A4)	;In the muddy room
	BEQ.S	SP.POINT		;yes use the point special then!
	RET
 
 
SP.POINT
	CMP.W	#RNRMUD,ROOM(A4)	;are we in the muddy room
	BNE 	99$
	CMP.W	#NMIRROR,D5		;point mirror?
	BNE.S	30$
	CMP.W	#NWAX,D0		;at wax?
	BNE.S	20$
* Is mirror in beam
	TEST_B	FlagBeam(A4)		;1 = mirror in beam...
	BNE	SP.Beam			;yes go do the special.
	SCN	358			;the mirror reflects the wax
	BRA.S	90$			;RETURN
20$
* So far we have point mirror ..
	CMP.W	#NBEAM,D0		;at beam
	BNE.S	25$
	SCN	356			;they look at each other...
	BRA.S	90$			;RETURN
25$
* point mirror at random object!
	MOVE.W	D0,STOPRON(A4)		;for @
	TEST_B	FlagBeam(A4)		;is beam illuminating?
	BEQ.S	28$
	SCN	362			;the x gets illuminated
	BRA.S	90$
28$
	SCN	363			;the x gets reflected in the mirror
	BRA.S	90$
30$
	CMP.W	#NBEAM,D5		;point beam?
	BNE.S	99$			;nope
	CMP.W	#NWAX,D0		;at wax?
	BNE.S	50$			;nope
	CMP.W	#NMIRROR,NOUNBUF3(A4)	;with the mirror?
	BEQ	SP.Beam			;Yeah it works.
	TEST_B	FlagBeam(A4)		;is the mirror in the beam already?
	BNE	SP.Beam			;yep do it
	SCN	357			;no go
	BRA.S	90$
50$
* point beam ...
	TEST_B	FlagBeam(A4)		;can they illuminate object
	BNE.S	60$
	CMP.W	#NMIRROR,NOUNBUF3(A4)	;have they said with mirror...
	BEQ.S	60$
	SCN	364			;Unfortunately you cannot manipulate
	BRA.S	90$
60$
	MOVE.W	D0,STOPRON(A4)		;for @
	SCN	362			;the x illuminates the beam
90$
	POPSP
99$
	
      RET

	XDEF	SP.Beam
	XREF	FGT_Gem,GemReachStart
SP.Beam
* it will only get here if all the conditions are right for the wax
* to melt.
	CMP.B	#-1,FlagBeam(A4)	;already happened?
	BEQ.S	99$
	SCN	117		;the wax melts
	SCORE	MOVEGEM		; do some scoreing
	MOVE.W	#NWAX,D0
	DO	SETOUT		;remove the wax
	CLR.L	GemReachStart(A4)	;Make it a reachable object
	MOVE.W	#-1,FGT_Gem+2(A4)		;No more First Glance on room!
	ST	FlagBeam(A4)	;it has happened.
	ST	FlagGem(A4)
	MOVE.B	#2,TIMER5(A4)	;if they don't catch it in a bit gem goes to
				;pool`

        PUSH_L  D0/A0
        MOVE.W  #NGEM,D0
        GETINFO
	MOVE.L	#RNRMUD,6(A0)           ;set it to be in the room
	BSET	#4,3(A0)		;make it moveable
        PULL_L  D0/A0
99$
	POPSP
	RET				;MORE TO BE DONE
*--------------------------------
 
	XREF	SP_MakeGlue
 
SP.MIX

	CALL	SP_MakeGlue
	TEST_B	WHOOPS(A4)
	BEQ.S	90$
	POPSP
90$
	RET
*--------------------------------
 
SP.SPLIT
SP.EXIT
	RET
 
  
 
SP.THROW
* The list.throw stuff has been conditionally
* commented out by Roddy - as it's not used when list is empty!
* IF YOU PUT SOMETHING IN THE LIST, FOD THE CONDITIONALS!
*

	CALL	BlackHoles
	BNE.S	80$

*
* Also, FOD this branch, if you use the list
	BRA.S	90$			; That's all!

    IFNE	0 

	LEA     LIST.THROW(PC),A1
*10$
	MOVE.L  (A1)+,D1
	BEQ.S   20$                       ;NOT IN TABLE
	CMP.W   D1,D5                     ;AT HIM?
	BNE.S   10$                       ;NO, NEXT
	MOVE.L  (A7)+,D2                  ;DUMP RET
	SWAP    D1                        ;GET ADDR/MSG
	BCLR    #15,D1                    ;HIBIT?
	BNE     P.SCN                     ;NO, IS A MSG
	DO	SETOUT	                   ;NO BOTTLE
	JMP     -4(A1,D1.W)               ;GO THERE
20$
	BTST    #6,4(A5)                  ;AT AN NPC?
	BEQ.S   90$                       ;NO, EXIT
	DO	SETOUT

     ENDC

80$
	POPSP
90$
	RET
    
 
LIST.THROW
 
      DC.L  0
 
 
*--------------------------------
*
 
 
*--------------------------------
 
        XREF    YCANT
        XREF    REMASTER

SP.CLOSE
	CMP.W	#NSTOPCOCK,D0		;Turning off water ?
	BEQ	CloseStopcock
	CMP.W	#NDBOX1,D0		;Closing night-safe?
	BLT.S	20$
	CMP.W	#NDBOX4,D0
	BLE	CloseNightsafe
10$
	CMP.W	#NCOUNTER,D0
	BNE.S	20$
	MOVE.W	#NFLAP,D0		;fake counter=flap for close counter
	GETINFO
20$
	CMP.W	#NCAGE4,D0		;trying to close the cage?
	BNE.S	30$
	PUSH_L	D0/A0
	MOVE.W	#NBIRD3,D0		;is the mynah bird in the cage
	GETINFO
	CMP.L	#CONT.MASK+NCAGE4,6(A0)	;Contained"?
	BNE.S	28$			;as far as we know,no!
	MOVE.B	#1,FlagMynah(A4)	;let the world know bird is secure
28$
	PULL_L	D0/A0
	BRA.S	90$
30$
	CMP.W	#NSTABLEDOOR,D0		;closing stable door?
	BNE.S	40$
	SCN	509			;a bit late now...
	POPSP
        BRA.S   90$
40$
        CMP.W   #NBOTTLE4,D0
        BNE.S   90$
        TEST_B  REMASTER(A4)
        BEQ.S   90$
        POPSP
      	DO      YCANT             	;you cant
90$
	RET

	XREF	V.READ

OpenBook

	POPSP
	BRA	V.READ

*-------------------------------- 
*
* SP.OPEN - opening the cage (big grizzly's cage)?
*
	XREF	FlagBear,TearSachet,FlagRuby,TIMER15
	XDEF	SP.OPENBags

SP.OPEN
	CMP.W	#NHIVE,D0		; opened the hive?
	BNE.S	10$
	MOVE.B	#1,TIMER15(A4)		; set up the death sequance
10$
	CMP.W	#NBOOK1,D0		;open a book == read a book
	BEQ.S	OpenBook
	CMP.W	#NBOOK2,D0
	BEQ.S	OpenBook
	CMP.W	#NBOOK3,D0
	BEQ.S	OpenBook
	CMP.W	#NRUBBAG,D0	;open rubbish bags?
	BNE.S	OPENOthers
SP.OPENBags
	SCN	151		;tear open bags
	PUSH_L	A0
	BSET	#4,(A0)		;open so get_cont works
	BCLR	#5,(A0)		;and not closed
	DO	GET_CONT
03$
	MOVE.W	(A1)+,D0
	BEQ.S	04$
	GETINFO
	MOVE.L	#RNKBANK,6(A0)	;contents and related items outside bank
	BRA.S	03$
04$
	PULL_L	A0
	BSET	#7,6(A0)	;bags out
	POPSP
	RET
OPENOthers
	CMP.W	#NBOTTLE4,D0	; Champagne?
	BEQ	OpenChampers
	CMP.W	#NSTOPCOCK,D0	; Opening stopcock to flood cellar
	BEQ	OpenStopcock
	CMP.W	#NSACHET,D0	;opening the magic powder's sachet
	BEQ	TearSachet
	CMP.W	#NCAGE4,D0	;trying to open the mynah cage
	BEQ	OpenCage4
	
	CMP.W	#NCAGE3,D0	;cage containing a bear
	BNE.S	20$
	BTST	#6,(A0)		;locked?
	BEQ.S	10$		;no, so they get to open it!
	CMP.W	#NKEY1,D5	;using the golden key to open it?
	BNE	90$
10$
	TEST_B	FlagBear(A4)	;bear out of it?
	BNE	90$		;NE=> yes, u'r ok
	SCN	68		;mauled to death by unhappy bear
	POPSP
	DOGO	DEAD

20$
	CMP.W	#NBAG2,D0	;plastic bag with manuscript in it
	BNE.S	30$
	PUSH_L	D0/A0
	MOVE.W	#NSHEET,D0
	GETINFO
	BTST	#7,6(A0)
	BNE.S	25$		; Already fodded !
	DO	SETOUT

* lose points for opening bag.
*-not any more since don't get points until bag in night safe

*	TEST_W	SCORE_(A4)	; Got any points?
*	BLE.S	22$
*	SUBQ.W	#1,SCORE_(A4)
*22$
	SCN	93		; Manuscript disintegrates
25$
	PULL_L	D0/A0
	BRA.S	90$
30$
	CMP.W	#NTILL,D0
	BNE.S	40$
	PRINT	<'How?^'>		; How?
	POPSP
	BRA.S	90$
40$
	CMP.W	#NBALL2,D0		;the red ball?
	BNE.S	50$
	BCLR	#2,4(A0)		; Clear 'short look'
	SCORE	FINDRING
	BRA.S	90$
50$
	CMP.W	#NCOUNTER,D0
	BNE.S	90$
	MOVE.W	#NFLAP,D0		;fake counter=flap for open counter
	GETINFO
90$
	RET

	XREF	FlagMynah

OpenCage4
* if they open the cage4 when the mynah bird is in it 
* it sets up a flag to tell the special0 about it!
				
	CMP.B	#1,FlagMynah(A4)	;is it in the cage?
	BNE.S	99$			;don't have to worry about it
	MOVE.B	#2,FlagMynah(A4)	;no longer safe as a bird
99$
	RET

 
*-------------------------------- 
*
* SP.FEED
*
* passed D0 = npc D5=food


	XREF	Bet_RatList,FeedMacawFlag

SP.FEED

	CMP.W	#NBEAR,D0
	BNE	30$
	CMP.W	#NPFISH,D5
	BEQ.S	10$
	CMP.W	#NDFISH,D5
	BNE.S	20$
	SCN	67			; OK Fish - not poisoned.
	BRA.S	15$
10$
	SCN	67
	SCN	138			;kills bear
	DO	GETNPC
	AND.B	#$0F,2(A3)		; Remove emotion
	OR.B	#$50,2(A3)		; Make unconscious
	ST	FlagBear(A4)		;so as open knows
	MOVE.W	#$8000+406,FGT_Cage(A4)	; cage now contains nobbled bear.
	BCLR	#7,ODT_BEAR(A4)		;Fodded bear	
15$
	BRA	79$			; Remove the food
20$

	BTST	#3,6(A5)		;intrinsic?
	BNE	90$
	BTST	#4,3(A5)		;moveable?
	BEQ	90$

	TEST_B	FlagBear(A4)		;bear unconcious?
	BEQ.S	22$
	SCN	141			;Tthe bear ignores you
	BRA.S	28$
22$
	EXG	D0,D5			;D0 is food
	BSET	#2,(A5)			;damage the 'food'

* The bear ignores 

	SCN	341
	DO	P.TN
	DO	UTIL.BS
	SCN	342
	EXG	D0,D5			;D5 is food
	MOVE.B	2(A5),D7
	LSR.B	#4,D7			;size of food
	MOVE.W	#NCAGE3,D0
	GETINFO
	MOVE.B	10(A0),D6
	AND.B	#7,D6			;container volume of cubic cage
	CMP.B	D7,D6
	BLS.S	25$
	EXG	D0,D5			;D0 is food, D5 id cage
	CALL	PUTD0IND5
	SCN	346		 	; , which he leaves lying in the cage.^
	BRA.S	28$
25$
	SCN	137			;, Nibbles and returns.
28$
	POPSP
	RET

* trying to dope the rat race?

30$
	CMP.W	#NCOCONUT,D5		;feeding the coconut to someone?
	BNE.S	35$
	CMP.W	#NMACAW,D0
	BEQ.S	72$
	BTST	#2,(A5)			;is it broken?
	BNE.S	35$
	SCN	383			;eating an unbroken coconut is
					;rather difficult
	BRA	80$
35$
	LEA	Bet_RatList(A4),A1	;can't feed rats!!
40$
	MOVE.W	(A1)+,D1
	BEQ.S	60$			;zero terminated list
	CMP.W	D0,D1			;our npc?
	BNE.S	40$
	SCN	490			;The gatekeeper spots your attempt to
					;'influence' the race and takes swift
					;action, he removes 
	MOVE.W	D5,D0
	MOVE.L	A5,A0
	DO	P.TN
	DO	SETOUT
	SCN	491			;<'from your grasp and throws '>
	DO	P.OBJPRN
	SCN	492		; into the moat (not a bad throw actually).^
	BRA.S	80$
60$
	CMP.W	#NPOISON,D5	; Poison to the rats ? (dead or alive)
	BNE.S	70$
	CMP.W	#NRATS,D0
	BNE.S	70$
	SCN	63
	BRA.S	79$
70$:
	CMP.W	#NMACAW,D0	; Feeding macaw?
	BNE.S	90$
	CMP.W	#NFOODS,D5	; Macaw food?
	BNE.S	75$
	TEST_B	FeedMacawFlag(A4)
	BNE.S	71$
	SCN	45
	BRA.S	80$
71$
	SCN	205
	BRA.S	80$
72$:

	BTST	#2,(A5)		; Broken ?
	BNE.S	74$
	
	SCN	48		; Can't eat it! its not broken!
	BRA.S	80$
74$:
	SCN	46
	ST	FeedMacawFlag(A4)
	SCORE	FEEDMACAW
	BRA.S	79$
75$:
	MOVE.B	11(A5),D1
	LSR.B	#4,D1			; Get edibility
	BEQ.S	90$
	BSET	#1,(A0)			; Fod macaw
	MOVE.W	D5,STOPRON(A4)		;for @
	SCN	47
79$
	MOVE.W	D5,D0
	DO	SETOUT			; Remove whatever you fed him
80$
	POPSP
90$
	RET

*-------------------------------- 

	XREF	FGT_WaterFall,FGT_TopFall

SP.UNTIE

	CMP.W	#NBOAT2,D0		; Boat
	BEQ.S	5$
	CMP.W	#NMOORING,D0		; Mooring rope?
	BNE.S	10$
5$:
	SCN	377			; The knot is too complicated!
	BRA.S	80$
10$
	CMP.W	#NROPE,D0
	BNE.S	20$
	SCN	489			; cant reach the knot from below
	BRA.S	80$
20$
	CMP.W	#NLADDER2,D0		;undoing the rope ladder?
	BNE.S	90$
	DO	SETOUT			;fod non-pseudo ladder
	MOVE.W	#NLADDER,D0
	GETINFO
	BCLR	#7,6(A0)		;unfod pseudo ladder
	PUSH_L	D0/A0
	CALL	OpenDoor		;to allow people up and down
	PULL_L	D0/A0
	BCLR	#4,(A0)
	CALL	SETPRON			;cos LADDER2 is now LADDER!
	SCN	167			;suitable message

	MOVE.W	#HI+476,FGT_TopFall(A4)	;change descs at top and bottom of
	MOVE.W	#NCAVE1,FGT_WaterFall+2(A4)	;waterfall
	SCORE	LADDER
80$
	POPSP
90$
	RET


*
* SP.HIT (also cue/pot)
*
* ... now handles HIT VEIN WITH PICK special... Roddy.
* 
* this has got to give clue about red ball + suss what they mean
* control comes here vanilla from the verb routine
*
* NO CHECKING has been done yet!
*

SP.HIT

	CMP.W	#NUPBUTTON,D0
	BEQ	SP.PRESS
	CMP.W	#NDOWNBUTTON,D0
	BEQ	SP.PRESS
	CMP.W	#NBUTTON,D0
	BEQ	SP.PRESS2
	CMP.W	#NMINERAL,D0		; Hitting the vein?
	BNE.S	SP.HIT2
	CMP	#WITH,CONJ(A4)		; 'With' ?
	BNE.S	SP.HIT2
	CMP.W	#NPICK,D5		; With the pick?
	BNE.S	SP.HIT2
	PUSH_L	D0/A0			; Push temp regs.
	MOVE.W	D5,D0
	GETINFO
	BTST	#2,(A0)			; Is pick broken ?
	PULL_L	D0/A0			; Restore regs...
	BNE.S	SP.HIT2			; Pick IS broken, so jump. 
	MOVE.W	#NCHIPS,D0
	GETINFO
	MOVE.L	#RNFACE,6(A0)		; Chips now simply in room
	MOVE.W	#NPICK,D0
	GETINFO
	BSET	#2,(A0)			; Break the pick!
	SCN	106
	POPSP
	RET
SP.HIT2
	PUSH_L	D0-D7/A1/A2

	MOVEQ	#0,D7			;D7 flags the cue EQ => not found
	MOVE.B	D7,D4			;ball1 count
	MOVE.B	D7,D5			;ball2 count
	MOVE.B	D7,D6			;ball3 count
	LEA	NOUNBUF(A4),A1		;this approach to a special is
	CALL	ChkBufferCue		;*NOT* nice!!!
	LEA	NOUNBUF2(A4),A1
	CALL	ChkBufferCue
	LEA	NOUNBUF3(A4),A1
	CALL	ChkBufferCue

* now count the balls!

	LEA	NOUNBUF(A4),A1
	CALL	ChkBufferBalls
	LEA	NOUNBUF2(A4),A1
	CALL	ChkBufferBalls
	LEA	NOUNBUF3(A4),A1
	CALL	ChkBufferBalls

	TEST_B	D4		;did we find any balls?
	BNE.S	10$
	TEST_B	D5
	BNE.S	10$
	TEST_B	D6
	BEQ.S	99$
10$
	TEST_B	D7		;and a cue?
	BNE.S	20$
	SCN	385		;u need a cue!
	BRA.S	90$
20$
	TEST_B	D4		;cue/white ball1?
	BEQ.S	30$		;eq => not mentioned
	MOVE.W	#NBALL1,D0
	MOVEQ	#61,D1		;msg about bouncing off cushions
	CALL_S	BallOnTable
30$
	TEST_B	D6		;spot ball?
	BEQ.S	40$
	MOVE.W	#NBALL3,D0
	MOVEQ	#61,D1		;msg about bouncing off cushions
	CALL_S	BallOnTable
40$
	TEST_B	D5		;red ball
	BEQ.S	90$
	MOVE.W	#NBALL1,D0
	MOVEQ	#62,D1		;msg about non-straight
	CALL_S	BallOnTable
90$
	ST	WHOOPS(A4)
99$
	PULL_L	D0-D7/A1/A2
	TEST_B	WHOOPS(A4)
	BEQ.S	999$
	POPSP
999$
	RET


* is he talking about the cue at all?

ChkBufferCue

10$
	MOVE.W	(A1)+,D0
	BEQ.S	20$
	CMP.W	#NCUE,D0
	BNE.S	10$
	DO	CARRIED
	SEQ	D7		;EQ => ok (D7.B is stomped as CARRIED return)
20$
	RET


*
* deal with is ball on table if so msg (D1) else - play on the table!
*

BallOnTable

	MOVE.W	D0,STOPRON(A4)
	GETINFO
	CMP.L	#REL.MASK+NTABLE2,6(A0)		;playing on the table?
	BEQ.S	10$				;EQ => ok use given msg
	DO	P.TN
	MOVE.W	#384,D1				;sarcastic msg
10$
	DOGO	P.SCN



* ball counting bit

ChkBufferBalls

10$
	MOVE.W	(A1)+,D0		;all noun buffs zero (word) term.
	BEQ.S	90$
	CMP.W	#NBALL1,D0
	BNE.S	20$
	ADDQ.B	#1,D4
	BRA.S	10$
20$
	CMP.W	#NBALL2,D0
	BNE.S	30$
	ADDQ.B	#1,D5
	BRA.S	10$
30$
	CMP.W	#NBALL3,D0
	BNE.S	10$
	ADDQ.B	#1,D6
	BRA.S	10$

90$
	RET

*--------------------------------------------------------------
*
* SP.HELP - so u can help random NPC's
*
*

SP.HELP

	CMP.W	#NGUEST,D0		;helping the frail old man?
	BNE.S	90$
	CALL_S	DoHelpMan
	POPSP
90$
	RET

	XDEF	DoHelpMan

	XREF	FlagDBlowered

DoHelpMan

	MOVE.W	#NGUEST,D0		;PRSPCMD may not have this set-up
	DO	SETOUT
	SCN	400			;spiel
	MOVE.W	#RNSCRUB3,ROOM(A4)	;u get left south of the drawbridge
	MOVE.W	#1,SCRPNT(A4)
	BSET	#0,FlagDBlowered(A4)	;put drawbridge down
	SCORE	HELPMAN
	RET

OpenStopcock

	TEST_W	FGT_System2(A4)		; has system been vandalised?
	BNE.S	StartFlood		; Yes!!!
	SCN	407			; nope, so gurgle gurgle gurgle
	RET

StartFlood

        BSET    #5,CANTSAVE(A4) 
	SCN	64                      ; water starts to flood cellar
	MOVE.B	#1,TIMER6(A4)		; Kick off the timer
	ST	Flooding(A4)
	MOVE.W	#HI+408,FGT_System1(A4)
	RET

CloseStopcock

	SCN	139
	CMP.B	#2,FloodClock(A4)	; Flooded successfully?
	BNE.S	10$
        BCLR    #5,CANTSAVE(A4)
	ST	Flooded(A4)
	SCN	66
10$:
	CLR.B	Flooding(A4)
	MOVE.W	#HI+398,FGT_System1(A4)
	RET

OpenChampers

        XREF    REMASTER,TIMER8
        
	BSET	#1,FlagChampers(A4)	; Already opened?
	BNE.S	90$
	BTST	#0,FlagChampers(A4)	; Shaken?
	BEQ.S	20$
	SCN	413			; Champagne everywhere!
        TEST_B  REMASTER(A4)
        BEQ.S   90$
        MOVE.B  #1,TIMER8(A4)           ; will reset texture
	BRA.S	90$
20$:
	SCN	412			; Celebration!
90$:
	RET


	XREF	BankedTreasure
	XREF	BankNotice
	XREF	ExplodeBankFlag

CloseNightsafe				 ;On entry D0,A0 is the relevant safe

	PUSH_L	D0/A0
	MOVE.W	#NRUBBAG,D0
	GETINFO
	BCLR	#7,6(A0)		;make them 'in'
	BEQ.S	06$
	BCLR	#5,(A0)			;if bags were out make contents
	DO	GET_CONT		;and related items out in case
	BSET	#5,(A0)			;the mynah/bottle blew up)
	BSET	#7,6(A0)		;and out again
05$
	MOVE.W	(A1)+,D0
	BEQ.S	06$
	GETINFO
	BSET	#7,6(A0)
	BRA.S	05$
06$
	PULL_L	D0/A0
	MOVE.L	A0,-(A7)
	DO	GET_CONT		;get A1 to point to list of 'contents'
	CLR.L	D6			;container treasures indicator
	CLR.B	D7			;contents count
10$
	MOVE.W	(A1)+,D0		;The next object to destroy
	BEQ.S	20$			;0=end of contents list
	BMI.S	10$			;Ignore items not 'in' the safe
	ADDQ.B	#1,D7			;one more to the contents
	CALL	PlaceItem
	BRA.S	10$
20$
	TEST_B	D6			;pot in the treasure?
	BEQ.S	25$
	MOVE.W	#NPOT,D0
	CALL	DoContainer
25$
	SWAP	D6
	TEST_B	D6
	BEQ.S	30$
	MOVE.W	#NCHALICE,D0
	CALL	DoContainer
30$
	CLR.W	D0
	TEST_B	D7
	BNE.S	40$
	SCN	32			;close empty safe
	BRA.S	90$			;range hassles!
40$
	SCN	28			;close full safe
90$
	MOVE.L	(A7)+,A0
	BSET	#5,(A0)			;safe closed
	BCLR	#4,(A0)			;and not open
	MOVE.B	#2,TIMER16(A4)		;deal with the little man
	POPSP				;we've done the description
	RET

DoContainer

	DO	GET_CONT
	GETINFO
	MOVE.L	#RNFOURROOM,6(A0)
	MOVE.W	(A1),D0			;item in the pot (there's at most one)
	BNE.S	PlaceItem
	RET				;pot empty

	XREF	RubbishFlag

PlaceItem

	GETINFO
	DO	FIXCONT			;remove it from the night safe
	TEST_B	ExplodeBankFlag(A4)	;anyone in bank?
	BNE.S	19$			;everything into vault if no-one here
	BTST	#2,(A0)			;damaged?
	BNE.S	15$                     ;yes
        TEST_B  REMASTER(A4)
        BEQ.S   10$                     ;money goes in the trash !remaster
        TST_MONEY                       ;money?
        BEQ.S   10$                     ;no
        DOGO    SETOUT                  ;destroy money
10$        
	CALL	IsTreasure		;Was it a treasure?
	BNE.S	16$
15$
	ST	RubbishFlag(A4)		;there is some rubbish
	MOVE.L	#NRUBBAG+$20000000,6(A0)	;contained in bags
	RET
16$
	CMP.W	#NBAG2,D0
	BNE.S	17$
	PUSH_L	D0/A0
	MOVE.W	#NSHEET,D0
	GETINFO
	BTST	#7,6(A0)
	PULL_L	D0/A0
	BNE.S	15$
17$
	CALL	SP.SCORE.TREASURE	;..then score some points
	ADDQ.B	#1,BankedTreasure(A4)	;one more treasure in the bank
	CMP.W	#NPOT,D0		;is it the pot?
	BNE.S	18$
	ST.B	D6			;the pot is here
	RET				;we'll move the pot later
18$
	CMP.W	#NCHALICE,D0
	BNE.S	19$
	BSET	#23,D6			;the chalice is here
	RET				;we'll move the chalice later
19$
	MOVE.L	#RNFOURROOM,6(A0)	;treasures into the vault
	RET
	
*
* SP.JOIN
*

	XDEF	GoQueue

GoQueue					;called by SP.GO.N
SP.JOIN

	CMP.W	#RNTILL1,ROOM(A4)	;at the till?
	BNE.S	10$
	MOVE.W	#RNINBANK,ROOM(A4)	;inside main area of bank
	CLR.B	TIMER7(A4)		;shut up teller
	SCN	537			;you go to the back of the queue
	DO	P.TNSTOP		;print the noun
10$
	CMP.W	#NQUEUE1,D0		;joining the first queue in bank?
	BEQ.S	ShortQueue
	CMP.W	#NQUEUE2,D0		;joining the second queue in bank?
	BEQ.S	LongQueue
	RET

	XREF	TellerMessage

LongQueue

	SCN	528			;describe move to the till
	MOVE.W	#RNTILL1,ROOM(A4)	;the player is now at the till
	MOVE.W	#2,SCRPNT(A4)		;describe getting to the till
	CLR.B	TIMERF(A4)		;clear waiting in short queue timer
	MOVE.B	#2,TIMER7(A4)		;start teller messages
	MOVE.W	#533,TellerMessage(A4)	;initialise messages
	POPSP				;join completed
	RET

ShortQueue

	SCN	529			;waiting in long queue
	MOVE.B	#2,TIMERF(A4)		;set timer to describe the wait
	POPSP
	RET



************************
*
*
* SP.FIX - ought to be called SP.FIX really but nobody knows TIE = FIX
*
* called just after FIX (V.TIE - honest) before anything else happens
* but AFTER SP.JOIN if JOIN x TO y is found rather the TIE/FIX x TO y
*
	XREF	SP.PutGum

SP.FIX

	CMP.W	#NGUM,D0		;tying gum to things?
	BEQ.S	10$
	CMP.W	#NGUM,D5		;tying things to the gum?
	BNE.S	90$

*
* let's see what sp.putgum thinks...
*
	EXG	A0,A5			;it wants put GUM on y
	EXG	D0,D5			;not tie y to GUM

10$
	POPSP				;whatever happens we dont want 
					;them to tie it!			

	CALL	SP.PutGum		;won't return if this sticks it 
	DOGO	DBS			;rediculous
90$
	RET

	END
