*****************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE


    include "equates.i"
    include "verbequ.i"
    include "nounequ1.i"
*   include "nounequ2.i"
    include "roomequ.i"
    include "macros.i"
    include "scenmacros.i"
*   include "scoreref.i"
    include "scenequ.i"


*--------------------------------
    XDEF    SP.VLIGHT,SP.STRIKE

    XREF    TIMER5,TIMER4,VERB
    XREF    STOPRON
    XREF    REMASTER

*
* SP.VLIGHT - special for light VERB (rather than sussing if room is dark)
*

    XREF    V.WARM,CandleLifeLeft,LIGHT.RESTRICTION
    XREF    CandleUnderRope,TIMERAFTER11,CloseHatch,RopeTied
    XREF    FGT_Rope


SP.VLIGHT

**********************************
* This causes more problems than it's worth. If they light the key, it melts!
*
*   CMP.W   #NKEY1,D0           ;Plastic key?
*   BNE.S   10$
* Coz of CHK4PT hassles, ie which one? which one? we gotta do this...
*   TEST_W  D5              ;IO?
*   BNE V.WARM              ;Does a chk4pt
*   POPSP
*   MSGGO   IDFY                ;Hmmm
*10$
**********************************

    CMP.W   #NCMAKER_FIRE,D5
    BNE.S   05$
    SCN 631
* You can't reach the fire from here.

    POPSP_RET

05$
    CMP.W   #NCANDLE,D0
    BNE.S   10$
    GETINFO
    TST_LIT             ;already alight?
    BNE.S   90$         ;yes, exit, verb should catch it
    CALL    LIGHT.RESTRICTION   ;can you light it where it is?
    BNE NOTNOW
    TST_WET             ;can't if it is wet
    BNE NOTNOW
    MOVE.B  CandleLifeLeft(A4),TIMER5(A4)   ;Candle alight for 60 moves
    TEST_B  CandleUnderRope(A4) ;burn the rope?
    BEQ.S   90$         ;nope
    MOVE.B  #3,TIMERAFTER11(A4)
    BRA.S   90$
10$
    CMP.W   #NDUNGEON_ROPE,D0       ; lighting the rope?
    BEQ.S   BurnRope
    BRA SP.STRIKE

90$
    RET

    XDEF    BurnRope
BurnRope
    DO  P.TN            ; the rope
    MSG BURNAW          ; burns away
    MOVE.W  #-1,FGT_Rope+2(A4)  ; kill the append.
    TEST_B  RopeTied(A4)        ; was the rope tied?
    BNE CloseHatch
    POPSP_RET

    XREF    P.MHAVSTOP,ROOM,AreWeOutside

SP.STRIKE

    XREF    IsRoomWet,Denotation_MATCHES

    PUSH_L  D0/D5
    LEA Denotation_MATCHES(A4),A1
    CMP.W   #NWOODEN_BOX,D5     ;with the box?
    BEQ.S   09$         ;yes, allow the lighting
    MOVE.W  D5,D0
    BEQ.S   09$         ;nosuch
    DO  MEMBEROFSET     ;light using a match?
    PULL_L  D0/D5
    BNE 90$         ;no, exit
    BRA.S   07$
09$
    PULL_L  D0/D5
07$
    DO  MEMBEROFSET     ;is d0 a match?
    BNE.S   90$         ;no
    GETINFO
    TEST_W  6(A0)           ;on inv? or in room?
    BNE NOTNOW
    MOVE.W  D0,-(A7)

    MOVE.W  #NWOODEN_BOX,D0     ;Make sure they have the match box!
    DO  CARRIED
    BEQ.S   10$         ;=> Yep, it's carried

* Pops return address and the word-wise push above
    ADDQ.W  #6,A7           ;Pop stack but keep D0
    DOGO    P.MHAVSTOP
* You must have the match box.

10$
    GETINFO
    TST_CONTAINED               ;Carried, but is it contained?
    BEQ.S   15$         ;=> Nope , so okay
    MOVE.W  8(A0),STOPRON(A4)   ;So, what's it carried in?
    SCN 183
* It would help if you removed the match box from @ first!
    ADDQ.W  #2,A7           ;Tidy up (Yuk). Trying to keep D0/A0
    BRA.S   80$         ;...and back from whence we came

15$
    MOVE.W  (A7)+,D0        ;Do this instead
    GETINFO
*   BTST    #7,4(A0)        ;Still lightable?
*   BNE.S   20$         ;=> yep, okay
*   SCN 99
* But the match is spent.

*   BRA.S   80$

20$
    DO  IsRoomWet       ;not if things are wet here.
    BNE NOTNOW
    TST_WET             ;can't light a wet match!
    BNE NOTNOW
    CALL    TestSpent       ;is it spent?
    BEQ.S   90$         ;yes, then dont strike
    BRA SP.MATCHES      ;set up match timer (if nec.)
80$
    POPSP
90$
    RET

*-------------------------------
*
*
* SP.VLightXwiY -   Handles Light x with y type constructs, validates
*           the referenced objects and sets up 'Smoulder' flag
*           as necessary for the fire code.
*
*   On Entry:   D0 = Nx
*           D5 = Ny
*           A0 -> X's data
*           A5 -> Y's data

*   XREF    STOPRON,CONJ,ROOM,Smoulder


*SP.VLightXwiY

*   CMP.W   #WITH,CONJ(A4)      ;Trying to light summit WITH summit?
*   BEQ.S   5$          ;=> Yep
*   CMP.W   #FROM,CONJ(A4)      ;Or summit FROM summit?
*   BNE.S   90$         ;=> Nope, exit NE
*5$
*   MOVE.W  D5,STOPRON(A4)      ;For the '@'
*   BTST    #7,(A5)         ;Is the y thing alight?
*   BNE.S   10$         ;=> Yes

* But the xyzzy is not alight.^
*   SCN 273
*   BRA.S   80$
*10$
*   BTST    #3,11(A0)       ;Does x burn?
*   BNE.S   20$         ;=> Yes
*   MOVE.W  D0,STOPRON(A4)      ;For the '@'

* An accomplished arsonist should know better than to even consider setting 
* fire to the xyzzy.^
*   SCN 272
*   BRA.S   80$

*20$
*   DO  CARRIED
*   BNE.S   30$         ;=> Not carried

* I'm afraid that setting fire to yourself is not a socially acceptable
* pastime in these parts.^
*   SCN 277
*   BRA.S   80$

*30$
*   DO  FIXCHK          ;This could be nasty so check!
*   BEQ.S   40$         ;=> Not fixed, so allow it

* A sudden gust of wind extinguishes the @ .^  (What a cop out!)
*   SCN 276
*   BCLR    #7,(A5)         ;And do what we just said
*   CMP.W   #NMATCH,D5      ;Was it the match?
*   BNE.S   80$         ;=> Nope
*   MOVE.B  #0,TIMER4(A4)       ;FOD the timer
*   BRA.S   80$

* If we get here we've got 'light x with y' where x is flammable and y is
* alight. Also, x is not tied or carried.
*40$
*   BSET    #7,(A0)         ;Set it as lit
*   BEQ.S   50$         ;If it wasn't already lit

* But it's already lit.^
*   SCN 274
*   BRA.S   80$
                    ;Successful arsonists will get to here
*50$
*   MOVE.W  D0,STOPRON(A4)

* The xyzzy begins to smoulder.^ (Hmm plural hassles)
*   SCN 275
*   MOVE.W  ROOM(A4),D1
*   MOVE.B  D1,Smoulder(A4)

*80$
*   SETEQ               ;We handled it
*90$
*   RET

*--------------------------------------

NOTNOW

    DO  P.SUB           ;you
    MSG YCNDTT          ;can't do that to
    DO  P.TN            ;the moose
    MSG ATMOMT          ;at the moment.
    POPSP_RET



SP.MATCHES

    XDEF    SP.MATCHES

    MOVE.W  ROOM(A4),D1
    CALL    AreWeOutside
    BNE.S   30$         ;=> We're outside
    MOVE.B  #2,TIMER4(A4)       ; Start timer.
    BSET    #7,(A0)         ; Light match
    CALL    TestSpent       ;is match spent?
    BEQ.S   90$         ;yes, let it light though
    SCN 73          ; The match flares into life!
    CALL    SetSpent        ;mark as spent
    BRA.S   80$
30$
    CALL    SetSpent        ;mark as spent
    SCN 81          ; The match lights but a gust of wind
    TEST_B  REMASTER(A4)
    BEQ.S   80$
    DO  SETOUT                  ; remaster => spent matches disappear
80$
    POPSP
90$
    RET

*-----------------------------------

    XREF    MatchesSpent
SetSpent

    PUSH_L  D4/A0
    LEA MatchesSpent(A4),A0
    MOVE.W  #4-1,D4         ;nmatches-1
10$
    TEST_W  (A0)+           ;gap?
    DBEQ    D4,10$          ;until gap
    TEST_W  D4          ;off end?
    BMI.S   90$         ;yes, forget it
    MOVE.W  D0,-2(A0)       ;else insert it
90$
    PULL_L  D4/A0
    RET
*-----------------------------------

    XDEF    TestSpent

    XREF    MatchesSpent
TestSpent

* Exit
*   EQ <=> spent

    PUSH_L  D4/A0
    LEA MatchesSpent(A4),A0
    MOVE.W  #4-1,D4         ;nmatches-1
10$
    CMP.W   (A0)+,D0        ;a hit
    DBEQ    D4,10$          ;until gap
90$
    PULL_L  D4/A0
    RET

*------------------------------------


    END

