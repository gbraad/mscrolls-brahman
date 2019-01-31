*****************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALST
*
*--------------------------------


    include "equates.i"
    include "macros.i"
    include "scenmacros.i"
*   include "verbequ.i"
    include "nounequ1.i"
*   include "nounequ2.i"
    include "roomequ.i"
    include "scoreref.i"


    XDEF    SP.TOUCH
    XREF    REMASTER

SP.TOUCH

    RET

    XDEF    SP.TearBefore

SP.TearBefore

    RET

*---------------------------

    XDEF    SP.TEAR
*   XREF    SP.MEDDLE,
    XREF    DO.SCORE

SP.TEAR

    RET

*--------------------------
    XREF    ROOM,CALCNET,R.NOUN,R.NOUN2,DoWaterLagoon,DoWaterLagoon2
SP.Throwatwater
* returns if there is no water....
    CMP.W   #NLAKE1,D5      ; at lagoon (pseudo)
    BEQ.S   10$
    CMP.W   #NLAKE2,D5      ; at lagoon (room)
    BEQ.S   10$
    CMP.W   #NWATER,D5      ; at water
    BEQ.S   10$
    RET
10$
* okay they wish to throw something at the water....
    PUSH_L  D0/A0           ; don't let them get in the way
    DO  R.NOUN          ; returns D0 as noun number of cr
    GETINFO
    TST_WET             ; is current room wet?
    BEQ.S   20$
15$
    PULL_L  D0/A0       ; get back our stuff.
    POPSP           ; don't go back to sp.throw
    BRA DoWaterLagoon   ; and go and put the stuff there.
20$             ; okay this room isn't wet is another.
    DO  CALCNET     ; returns A0 pointing at network.
    MOVE.L  A0,A3       ; for saving sake
    MOVE.W  #Sizeof_NET-1,D6    ; the number of possible exits
    CLR.W   D2
25$
    MOVE.B  (A3)+,D2
    BEQ.S   30$     ; not a room
    CMP.W   #MAXROOM,D2 ; greater than max
    BHI.S   30$     ; An nway
    DO  R.NOUN2     ; returns D0
    GETINFO
    TST_WET             ; is THIS room wet?
    BNE.S   40$     ; yes....
30$
    DBRA    D6,25$      ; around again
    BRA.S   15$     ; go and do the water lagoon
40$
**** YIPPIE We have one....
    PULL_L  D0/A0       ; get back our bits.
    MOVE.W  ROOM(A4),-(A7)
    MOVE.W  D2,ROOM(A4) ; and fake up our room...
    SCN 617     ; you hear a whizzing sound
    CALL    DoWaterLagoon2  ; (with no popsp)
    MOVE.W  (A7)+,ROOM(A4)  ; get back our room
    ADDQ.W  #8,A7       ; get A7 back to top level
    RET

    XDEF    SP.THROW

    XREF    P.SCN,SETOUT,GetRidofBull
    XREF    BlackHoles
    XREF    SP.GivePostMistress,SP.RowdyInPub,SP.WAKE

SP.THROW

    CALL_S  SP.Throwatwater
    CMP.W   #NGAOLER,D5     ; throwing something at him.?
    BNE.S   1$
    MOVE.W  D5,D0
    BRA SP.WAKE         ; let that ahdnel it
1$
    CALL    SP.GivePostMistress ; throwing her the mouse
    CMP.W   #RNPUB,ROOM(A4)     ; in the pub ?
    BNE.S   5$
    POPSP               ;no verb routines today thank you
    BRA SP.RowdyInPub       ;chuck out if nesc.
5$
    CALL    BlackHoles      ;Throw x at black hole?
    BNE.S   80$         ;=> BlackHoles coped - exit

    LEA     LIST.THROW(PC),A1
10$
    MOVE.L  (A1)+,D1
    BEQ.S   90$         ;Not in table, back to verb

    CMP.W   D1,D5                     ;AT X?
    BNE.S   10$                       ;NO, NEXT
    POPSP                             ;Dump ret
    SWAP    D1                        ;GET ADDR/MSG
    BCLR    #15,D1                    ;HIBIT?
    BNE     P.SCN                     ;NO, IS A MSG
    JMP     -4(A1,D1.W)               ;GO THERE
80$
    POPSP
90$
    RET
    
 
*
* These two macros allow you to define the action taken if the player
* types 'Throw x at z'.
*
* ThrowCode is used if you want a piece of code called if a 'throw x at z'
* occurs.
* 
* ThrowSCN is used if you simply want a scenario message printed.
*
*



ThrowCode   MACRO   * SP.xyzzy,NOUN

    DC.W    \1-*
    DC.W    N\2

    ENDM

ThrowSCN    MACRO   * SCN#,NOUN

    DC.W    \1+HI
    DC.W    N\2

    ENDM


LIST.THROW
    ThrowCode   SP.Throw.Inspector,INSPECTOR
    ThrowCode   SP.Throw.Bull,BULL
    ThrowCode   SP.Throw.Clockmaker,CLOCKMAKER
    ThrowCode   SP.Throw.Lamp,OIL_LAMP
    ThrowCode   SP.Throw.Clockmaker,STOOL

    DC.L  0             ;LONG word 0 terminator

*----------------------

    XREF    STOPRON,LeaveBus,BaseTMess,AskedNTimes

SP.Throw.Inspector

* Inspector never leaves bus so we know which room we're in
    DO  SETOUT          ;Fix tie etc
    MOVE.L  #RNONBUS,6(A0)
    MOVE.W  D0,STOPRON(A4)      ;Set up the '@'
    CMP.W   #NTICKET,D0     ;Was it the ticket?
    BNE.S   10$
    CMP.W   #BaseTMess+2,AskedNTimes(A4)    ;Currently asking for it?
    BLT.S   10$         ;=> nope
    SCN 117
* (Standard lobbing off message)
    BRA LeaveBus        ;And get chucked off

10$
    SCN 177
* He easily avoids @ with a nimble side-step. His relaxed attitude
* obviously having been gained from years of experience with childrens'
* outings.
90$
    RET

*------------------------------------

    XREF    ISIN,TIMER1

SP.Throw.Bull

    PUSH_L  D0/A0           ;Stash noun & data
    MOVE.W  #NHOLE,D0
    DO  ISIN            ;Hole around yet?
    PULL_L  D0/A0           ;Preserve flags
    BNE.S   10$         ;=> nope
    DO  P.TN            ;'The xyzzy'
    SCN 112
* hits the bull squarely on the head. He is suitably provoked.

    DO  SETOUT          ;Lose the offending article
    BRA.S   90$

10$
    CMP.W   #NTABLECLOTH,D0     ;Throwing the tablecloth at the bull?
    BNE.S   20$
    TEST_B  TIMER1(A4)      ;Bull charging?
    BEQ.S   20$         ;=> nope
    CALL    GetRidofBull        ;=> yep, this is a solution then!
* NB GetRidofBull will POP stack - hence CALL (hmmm)
* So this is effectively a BRA

20$
    MOVE.W  D0,STOPRON(A4)      ;Set up the @
    SCN 110
* The bull eyes @ with interest for a few moments before
* returning to his meal.

    MOVE.L  #RNFIELD1,6(A0)     ;Object simply in room

90$
    RET

*---------------------------

*
* Throwing stuff at the clockmaker/oil lamp/window/stool at clockmakers shop
*

    XREF    ClockMakerGone

SP.Throw.Clockmaker

    CALL    WindowOpen      ;Won't ret if closed

ClockmakerShuts

    TEST_B  TIMERAFTER19(A4)    ;Is he in mid room-change?
    BNE.S   90$
    TEST_B  ClockMakerGone(A4)  ;Has he run away yet?
    BEQ.S   10$         ;=> Nope
    DO  P.TN            ;The x
    MSG HITS            ;hits
    EXG D0,D5
    DO  P.TN            ;the y
    MSG BUT         ;But
    MSGGO   NHAP            ;nothing happens (EXIT)
10$
    SCN 785
* Clockmaker gets real annoyed and locks the window.

    MOVE.W  #NCMAKERS_WINDOW,D0
    GETINFO
    TEST_B  REMASTER(A4)
    BEQ.S   80$
    MOVE.B  #$20,(A0)       ;he only closes window 
    BRA.S   90$
80$    
    MOVE.B  #$40,(A0)       ;Lock window
90$
    RET

*------------------------

    XREF    ODT_CMAKERS_WINDOW,TIMERAFTER19

SP.Throw.Lamp

    CALL    WindowOpen      ;Won't ret if closed
    DO  P.TN            ;The x
    SCN 781
* hits the lamp 

    MOVE.W  2(A0),D1        ;Get properties
    AND.B   #$F,D1          ;Get texture
    CMP.B   #TEXTURE.SOFT,D1    ;nothing soft works
    BEQ.S   20$
    CMP.B   #TEXTURE.FURRY,D1   ;nor furry
    BEQ.S   20$
    LSR.W   #8,D1           ;Okay, size and weight
    MOVE.W  D1,D2           ;Copy
    AND.B   #$F,D1          ;Get weight
    AND.B   #$F0,D2         ;and size
    ADD.B   D2,D1           ;Combine
    CMP.B   #4,D1           ;Size & weight 4 or more works
    BLT.S   20$         ;=> not hefty enuf
10$
    TEST_B  TIMERAFTER19(A4)    ;Was he in the room?
    BNE.S   30$         ;=> nope, you win!
    SCN 782
* which falls to the floor. The clockmaker hurriedly retrieves it before
* any damage is done.

    BRA ClockmakerShuts     ;He locks the window

20$
    SCN 783
* and falls to the floor. 

    BRA ClockmakerShuts     ;He gets annoyed and locks window

30$
    SCN 784
* which falls to the floor. Oil spills from the lamp and the place catches on
* fire. The clockmaker, returning to his workshop, sees the fire and rushes
* out of the shop in a mad panic.

    MOVE.W  #374,ODT_CMAKERS_WINDOW-2(A4)   ;Use second ODT msg
    MOVE.W  #NCLOCKMAKER,D0
    DO  SETOUT          ;Fod the sucker
    EXG D0,D5
    DO  SETOUT          ;And the lamp
    CLR.B   TIMERAFTER19(A4)    ;Stop the move timer
    ST  ClockMakerGone(A4)  ;Tell the world
    MOVE.W  #NCMAKER_FIRE,D0    ;a fire in the shop 
    DO  SETIN
    MOVE.W  #NCMAKERS_DOOR,D0
    GETINFO
    BCLR    #6,(A0)         ;!Locked
    DOGO    OpenDoor        ;Open the door and connect

*----------------------------------

WindowOpen

    DO  SETOUT          ;Fod thrown object (fixes container etc)
    PUSH_L  D0/A0           ;Stash
    MOVE.W  #NCMAKERS_WINDOW,D0
    GETINFO
    TST_OPEN
    PULL_L  D0/A0           ;With flags
    BNE.S   90$         ;Window open, back to caller
    MOVE.L  #RNCAROUSEL,6(A0)   ;Bung it in the room
    DO  P.TN            ;The x
    SCN 780
* hits the closed window and drops to the ground.

* (hugh2018) no! only 1 pop, because LIST.THROW actions never return to verb
* and have already POPSP once.    
*	ADDQ.W	#8,A7			;2*pops
    POPSP
90$ 
    RET

*---------------------------------------------------
    XDEF    SP.TurnUpsideDown

SP.TurnUpsideDown
    CMP.W   #NLADDER,D0     ;Attempting to invert ladder?
    BNE.S   90$
    SCN 370
* Well, if you think it will help. The ladder is now upside down.

    NOT.B   LadderOrientation(A4)   ;Invert it
    POPSP
90$
    RET

    XDEF    SP.TURN
    XREF    TIMER9,GetWet
    XREF    IsHandle,PrHandle,HandlePos,LadderOrientation,CONJ


SP.TURN
    CMP.W   #NWHEEL,D0      ;turn the wheel
    BNE.S   20$
    SCN 121         ;it turns
    MOVE.W  #NAIRLOCK_DOOR,D0
    GETINFO
    BCHG    #6,(A0)         ;locked?
    BNE.S   10$         ;yes
    AND.B   #$CF,(A0)       ;clear closed&open
    BRA 80$         ;quit
10$
    BCLR    #4,(A0)         ;incase open
    BSET    #5,(A0)         ;now just closed
    MOVE.W  #NHATCH,D0      ;hatch
    GETINFO
    TST_OPEN                ;is hatch open?
    BEQ 80$         ;yes, ok
    SCN 327         ;door is forced open
    CALL    GetWet          ;you are drenched.
    MOVE.B  #1,TIMER9(A4)       ;begin the flood
    MOVE.W  #NAIRLOCK_DOOR,D0
    DO  OpenDoor
    BRA 80$
20$
; Turning a handle on the PO safe??

    CALL    IsHandle        ; is it a handle??
    BNE 90$
    LEA HandlePos(A4),A5    ; point to states
    MOVE.W  D0,D5           
    SUB.W   #NTLHANDLE,D5       ; convert to handle number.
    BCHG    D5,(A5)         ; Flip handle
    CALL    PrHandle        ; print message
    TEST_W  D5          ; Top left does nothing
    BEQ.S   70$
    CLR.W   D1          ; Just in case!
    BTST    D5,(A5)         ; Look at handle state
    BEQ.S   50$         ; Turned up...

; This used to be ...
;   MOVE.B  HandleUpTab(PC,D5.W),D1 ; This is because...
; 
    LEA HandleUpTab(PC),A2  ; ... nobody trusts my emulator!
    MOVE.B  0(A2,D5.W),D1       ; Get handle to move

    BCLR    D1,(A5)
    BEQ.S   70$         ; It's already up!
    ADD.W   #NTLHANDLE,D1
    MOVE.W  D1,STOPRON(A4)
    SCN 480         ; 'unfortunately, x moves up.'
    BRA.S   70$
50$:
; This used to be ...
;   MOVE.B  HandleDownTab(PC,D5.W),D1; This is because...
; 
    LEA HandleDownTab(PC),A2    ; ... nobody trusts my emulator!
    MOVE.B  0(A2,D5.W),D1       ; Get handle to move

    BSET    D1,(A5)         ; test handle to be moved...
    BNE.S   70$         ; It's already up!
    ADD.W   #NTLHANDLE,D1
    MOVE.W  D1,STOPRON(A4)
    SCN 481         ; 'luckily, x moves down.'
; Get to 70$ when any handle is turned...
70$ 
    MOVE.W  #NSAFE,D0
    GETINFO
    CMPI.B  #$0F,HandlePos(A4)  ; can safe open?
    BNE.S   75$
    SCN 482         ; safe clicks open.
    BCLR    #6,(A0)         ; unlock safe
    BSET    #5,(A0)
    SCORE   OPENSAFE
    BRA.S   80$
75$:
    BSET    #6,(A0)         ; lock it
    BCLR    #5,(A0)         ; un-close it.  
80$
    POPSP
90$ 
    RET

HandleUpTab
    DC.B    0,2,1,0
HandleDownTab
    DC.B    0,3,0,2

*---------------------------------------

    XDEF    SP.TAP
    XREF    TapDancing

SP.TAP
    CMP.W   #NDANCE,D0      ;tap dance?
    BNE.S   90$         ;quit
    TEST_B  TapDancing(A4)      ;already
    BEQ.S   01$
    SCN 461         ;you are already
    BRA.S   80$
01$
    MOVE.W  #NTAPDANCE_SHOES,D0
    GETINFO
    TST_WORNBYPLAYER                ;worn?
    BNE.S   10$
    SCN 456         ;you arnt wearing them
    BRA.S   80$
10$
    SCN 457         ;you try
80$
    POPSP
90$
    RET

*------------------------------------------



    END
