************************************************************
*
*   %W%
*   %G%
*
************************************************************

    SECTION "ADV",code

    include "equates.i"
    include "macros.i"
    include "scenequ.i"
    include "roomequ.i"
    include "scenmacros.i"
    include "nounequ1.i"
    include "scoreref.i"

*--------------------------------

    XDEF    SP.RUB,SP.RUB2
    XREF    REMASTER

SP.RUB
* this happens before it has insisted on there being 4 parts
    RET

SP.RUB2
* this happens after it has insisted on there being 4 parts

    XREF    DoorOiled
    XREF    ODT_RUNNERS,ODT_SLIDINGDOOR

    CMP.W   #NCANDLE,D5     ;using candle?
    BEQ.S   10$
    CMP.W   #NOIL,D5        ;or the oil?
    BNE.S   30$
10$
    CMP.W   #NRUNNERS,D0        ;on the runners?
    BEQ 20$
    CMP.W   #NSLIDINGDOOR,D0    ;or on the door?
    BNE 30$
20$
    SCN 200         ;well done that man...
    BCLR    #7,ODT_RUNNERS(A4)  ;change runners desc
    BCLR    #7,ODT_SLIDINGDOOR(A4)  ;and the door too
    ST  DoorOiled(A4)
    TEST_B  REMASTER(A4)
    BEQ.S   22$
    SCORE   GETOIL
22$    
    MOVE.W  D5,D0
    TEST_B  REMASTER(A4)
    BEQ.S   25$
    CMP.W   #NCANDLE,D0         ; candle does not get used up for remaster
    BEQ.S   26$
25$
    DO  SETOUT          ;kill the lubricant
26$
    POPSP_RET
30$             ;get here if not lubricating door
    RET

;   XDEF    SP.ROLL
;SP.ROLL

;   RET

*******************************************************

    XDEF    SP.RIDE

    XREF    UnicornFriendly
 
SP.RIDE

* various frigs cos JUMP ON maps to ride!! And setmode looks at VERB!

* sit on saddle (when on unicorn)

    CMP.W   #NUNICORN2,D0       ;sit on unicorn2
    BNE.S   10$
    TEST_B  UnicornFriendly(A4) ;has he been tamed?
    BNE.S   90$         ;yes fine
    SCN 257         ;unicorn bucks
    DO  DESITTABLE      ;any junk on him
    BSET    #3,5(A0)        ;make him sittable again
    POPSP_RET           ;ignor verb
10$
    CMP.W   #NSADDLE,D0     ;sit on saddle?
    BNE.S   90$
    TEST_B  6(A0)           ;simple?
    BEQ.S   90$         ;yes, then sit on SADDLE
    MOVE.W  8(A0),D0        ;then sit on that really.
    GETINFO
90$
    RET

*--------------------------------
*
* SP.READ
*

    XDEF    SP.READ
    XREF    ReadDocument,GuardianHere,V.WARM,CONJ,SETOUT
    XREF    TIMER12,FODWOSSNAME,LUCK_
 
SP.READ
    CMP.W   #NDOCUMENT,D0
    BNE.S   90$
    BSET    #0,ReadDocument(A4) ; He's read it now..
    BNE.S   90$
    SCN 307         ; do spiel....
    CLR.B   GuardianHere(A4)    ; Remove guardian
    CLR.B   TIMER12(A4)     ;and his spiel
    CALL    FODWOSSNAME     ;Byeee
    MOVE.W  #4,LUCK_(A4)        ;Init luck
80$
    POPSP
90$
    RET

*5$:
*   CMP.W   #NPOSTAL_NOTE,D0
*   BNE.S   10$
*   ST  OpenMbx(A4)
*
*90$
*   RET


*---------------------------


    XDEF    SP.REFLECT

SP.REFLECT
    RET

;   XDEF    SP.RNOUN

;SP.RNOUN
  
;   EXG     D0,D1
;   DO      R.NOUN
;   EXG     D0,D1
;   RET
 
*-------------------------------------

    XDEF    SP.RUN
    XREF    ROOM,TIMERAFTER7
    XREF    TIMERAFTER9,GuardWantsTicket,DO.SCORE

SP.RUN
    CMP.W   #NLOCOMOTIVE,D0     ;run after train
    BNE.S   90$         ;nope
    TEST_B  TIMERAFTER7(A4)     ;train moving away?
    BNE.S   10$
    SCN 487         ;train stil here
    BRA.S   80$
10$
    CMP.B   #1,TIMERAFTER7(A4)  ;second time?
    BNE.S   20$         ;no
    CALL    NearEnoughToTrain
    BNE.S   20$         ;no, tough
    SCN 489         ;you grab the back
    MOVE.W  #RNON_TRAIN,ROOM(A4)    ; you are there now
    MOVE.B  #1,TIMERAFTER9(A4)  ;set guard to ask
    MOVE.B  #1,GuardWantsTicket(A4) ;the question he asks
    SCORE   CATCHTRAIN
    BRA.S   80$         ;and exit
20$
    SCN 488         ;b4 2 fast
80$
    POPSP
90$
    RET
 
*-------------------------------------

NearEnoughToTrain

    CMP.B   #RNPLATFORM3,ROOM+1(A4)
    BEQ.S   90$             ;ok, yes
    CMP.B   #RNRAILWAY_TRACK,ROOM+1(A4) ;or here
90$
    RET

*-------------------------------------

*
*
* RestartGame   -   Fuck knows how this will work. Maybe it'll
*           just work (you'll be lucky) More probably
*           it'll have to set up a sensible stack
*           level and other yukkies.

    XDEF    RestartGame
    XREF    GameFinished

* Yep, I know this won't work
RestartGame

    ST  GameFinished(A4)
    RET
*-------------------------------------

    END
