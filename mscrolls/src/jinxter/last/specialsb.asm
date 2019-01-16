*****************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSB
*
*--------------------------------


    include "equates.i"
*   include "verbequ.i"
    include "nounequ1.i"
*   include "nounequ2.i"
    include "roomequ.i"
    include "macros.i"
    include "scenmacros.i"
    include "scoreref.i"


    XDEF    SP.BET,SP.BRKD0,SP.BUY,SP.BUY_Before,SP.BREAK

    XREF    Stationmaster_givein,TIMERAFTER5
    XREF    TriedToBuy,SetValue,DO.SCORE,STOPRON
    XREF    WeirdAttire,PUTD0IND5
    XREF    REMASTER

SP.BUY
    CMP.W   #NTRAIN_TICKET,D0   ;buy ticket
    BNE 50$
    CMP.W   #NTWOFERG,D5        ;with 2 ferg coin?
    BEQ.S   30$         ;ok, u can buy ticket
    CMP.W   #NSILVER_COIN,D5    ;wi forgein coin
    BNE.S   10$
    SCN 251         ;accept fergs only
    BRA.S   20$         ;fix value & exit

10$
    CMP.W   #NAQUITANIAN_FERG,D5
    BNE.S   50$
    TEST_B  Stationmaster_givein(A4)    ;have he given up?
    BNE.S   30$         ;yes
    SCN 470         ;he wont acccpt that
20$
    ST  TriedToBuy(A4)      ;so that busking makes him change
    MOVEQ   #1,D1           ;value is the same
    CALL    SetValue        ;back
    BCLR    #7,6(A5)        ;coin is back in regardless.
    BTST    #5,6(A5)        ;was contained?
    BEQ.S   21$         ;no, then no adjustment
    MOVE.W  D5,D0           ;the coin.
    MOVE.W  8(A5),D5        ;the container.
    CLR.L   6(A5)           ;pretend its carried for a second.
    CALL    PUTD0IND5       ;and adjust all nec. volumes.
21$
    POPSP_RET           ;was BRA 40$
25$
; Trying to buy the ticke with the two ferg coin?
    TEST_B  Stationmaster_givein(A4)    ; has he given in?
    BEQ.S   30$             ; nope all okay
    SCN 600         ; After all THAT????
    BRA.S   32$         ; AND put the wind up you

30$
    SCN 471         ;ok then he does
    CMP.W   #NTWOFERG,D5        ;was it the 2 ferg coin?
    BEQ.S   35$         ;yes, then no wind
32$
    MOVE.B  #1,TIMERAFTER5(A4)  ;just the 1 move later
35$
    MOVE.W  D5,D0
    DO  SETOUT
    MOVE.W  #NTRAIN_TICKET,D0
    GETINFO
    CLR.L   6(A0)           ;you've got it
    CALL    StationMasterCloses
    SCORE   GETTICKET
40$
    POPSP_RET
50$
    CMP.W   #NDRINK,D0  ; Buying drink in pub???
    BEQ.S   60$
    CMP.W   #NBEER,D0   ; (or the 'beer')
    BNE.S   90$
60$:
    SCN 348     ; 'barman pulls a pint'
    TEST_B  REMASTER(A4)
    BEQ.S   80$
    CMP.W   #NTWOFERG,D5        ;with 2 ferg coin?
    BNE.S   80$
    MOVE.L  #REL.MASK+NBOTTOM_WELL,6(A5) ; puts coin back in well (sets in too).
80$
    SETEQ_RET       ; Make other message disappear
90$:
    SETNE_RET       ; This makes messages appear

    XREF    SetValue,P.NFND,FlagPubHearthCold
    XREF    WeirdAttire,SP.RowdyInPub

SP.BUY_Before
    CMP.W   #NTRAIN_TICKET,D0   ;buy ticket
    TEST_W  6(A0)           ;related etc?
    BEQ.S   5$          ;forget it!
    CMP.W   #NSTATION_MASTER,8(A0)  ;belongs to him?
    BNE.S   5$          ;else no.
    CALL    WeirdAttire     ;wetsuit fans?
    BEQ.S   5$          ;no so ok
    MOVE.W  #HI+NSTATION_MASTER,STOPRON(A4) ; no leading space
    SCN 363         ; " @ cracks up laughing."
    POPSP_RET
5$

    CMP.W   #NDRINK,D0      ; Trying to buy a drink?
    BEQ.S   10$
    CMP.W   #NHANDPUMP,D0       ; has synonym beer
    BEQ.S   10$
    CMP.W   #NBEER,D0
    BNE 90$
10$:
    MOVE.W  #NPUBLICAN,D0       ;can only buy beer if lld local
    DO  VALID.NP
    BEQ.S   15$
    CALL    P.NFND          ;you can't see a lld here
    BNE 80$         ;no more verb routine

; Buying drink in pub...?
15$
; wearing ridiculous clothing ?
    CALL    WeirdAttire
    BEQ.S   16$
    POPSP
    BRA SP.RowdyInPub
16$
; lld wont serve you if you have put his fire out
    TEST_B  FlagPubHearthCold(A4)
    BEQ.S   17$
    DO  P.TN
    SCN 364
* is sulking and ignores your request.
    BRA.S   80$
17$
; Don't let player have another drink IF landlord does not have glass,
; and (beer is in glass OR  glass not local).
; IF landlord does have glass, then its all easy!

    MOVE.W  #NBEER,D0       ; check on the beer
    GETINFO
    TST_OUT             ; is beer out ?
    BNE.S   25$         ; if not then moan
20$
    SCN 331         ; "Drink up first, please!"
    BRA.S   80$         ; don't bother with verb
25$
    MOVE.W  #NDRINK,D0      ; the glass
    GETINFO
    TST_BROKEN          ; can't buy drink wi broken glass
    BEQ.S   30$
    POPSP               ; don't bother with verb
    XREF    SP.RowdyInPub
    BRA SP.RowdyInPub       ; being rowdy again...
30$
    DO  VALID.NP        ; Check that glass is here.
    BNE.S   20$         ; If glass not here, then whinge
    MOVE.L  #REL.MASK+NPUBLICAN,6(A0) ; move glass to publican for V.BUY
    MOVE.W  #NBEER,D0       ; set up for PUTD0IND5
    GETINFO
    CLR.L   6(A0)           ; so FIXCONT wont cock it up too badly
    MOVE.W  D5,-(SP)        ; verb needs it
    MOVE.W  #NDRINK,D5      ; really the glass...
    XREF    PUTD0IND5
    CALL    PUTD0IND5       ; put beer in glass & fix cont volume
40$
    MOVE.W  #NDRINK,D5
    MOVEQ   #1,D1           ; Restore glass value to 1
    CALL    SetValue
    MOVE.W  (SP)+,D5        ; restore it.
    MOVE.W  #NDRINK,D0      ; buy beer => buy glass
    XREF    SETPRON
    CALL    SETPRON         ; so that 'buy beer,drink it' works
    BRA.S   90$         ; ... and let verb routine do the rest. 
80$
    POPSP
90$
    RET


SP.BRKD0
    CMP.W   #NPUDDING2,D0   ; burnt pudding
    BNE.S   90$
    CALL    SP.BreakPudding
    SETEQ           ; So message gets not printed
90$
    RET

*******************************************************
*
* SP.BET
*
* this should catch the stuff about betting with the gatekeeper
*
* u can bet with the two freg piece (NPIECE) and the tem freg note (NNOTE)
*
* by the time the special is called
*
*    D0=money to bet
*    D5=what to bet on
*   D1=who to bet with always NPC (may be zero => default)
*
* If this RET's then either
*
*    'Xyzzy refuses your bet'
*    'Nobody will take your bet'
*
* depending on whether D1 is zero or an NPC
*
SP.BET
    RET

*****************************************
*

    XREF    CharmInCake,DROPHERE
; When you smash the burnt pudding, check for the charm being present in it.

SP.BREAK
    TEST_W  D5      ; any indirect opjects
    BEQ.S   10$
    CMP.W   #NHAND,D5   ; allow break pudding with hands
    BNE.S   90$
10$
    CMP.W   #NPUDDING2,D0   ; burnt pudding ?
    BNE.S   90$
    CALL    SP.BreakPudding
    POPSP
90$
    RET

*-----------------------

    XDEF    SP.BLOW

    XREF    SP.UNLIGHT,DomeHasShattered,FodLongLook
    XREF    SP.PLAY

SP.BLOW
    CMP.W   #NHARMONICA,D0          ;blow harmonica
    BEQ SP.PLAY             ;hmm
    CMP.W   #NGLASS_FRAGMENTS,D0        ;blow fragments?
    BNE SP.UNLIGHT          ;else do this
    TEST_B  DomeHasShattered(A4)        ;is glass on it?
    BEQ.S   90$             ;no
    SCN 566             ;blow them away.
    MOVE.L  #NBRACELET,D0
    DO  FodLongLook
    CLR.B   DomeHasShattered(A4)        ;let it go
    POPSP
90$
    RET

*------------------------------------

    XREF    HatForCollecting,CoinContainer
    XREF    TapDancing,PlayHarmonica
    XREF    SETOUT,ROOM,SetBuskFlags,P.SCN

    XDEF    BeginBusking
    XREF    BuskDoneTable,TIMERAFTER4,TriedToBuy
    XREF    TIMERAFTER6,GivenCoin,SETIN,CoinUsed
    XREF    Waiting

BeginBusking

    LINK_   A6,#0
    PUSH_L  D1
    CALL    BuskOnTrain
    CALL    BuskableRoom
    BNE 50$         ;no
    MOVE.B  #7,TIMERAFTER6(A4)  ;set tourists away
    CALL    SetBuskFlags        ;HatFor & CoinContainer
    CLR.W   D2          ;combination index
    TEST_B  PlayHarmonica(A4)
    BEQ.S   10$         ;no
    MOVEQ   #1,D2
10$
    TEST_B  TapDancing(A4)      ;danging as well?
    BEQ.S   20$         ;no
    OR.W    #2,D2           ;yes
20$
    TEST_W  CoinContainer(A4)   ;coin out?
    BEQ.S   30$         ;no
    OR.W    #4,D2           ;coin out
    CMP.W   #NTOP_HAT,CoinContainer(A4)
    BNE.S   40$         ;if coin out, must be hat
    OR.W    #8,D2           ;ok then
    BRA.S   40$
30$
    TEST_B  HatForCollecting(A4)
    BEQ.S   40$
    OR.W    #8,D2           ;if no coin, then use any hat
40$
    TEST_L  8(A6)           ;print flag?
    BNE.S   44$         ;always print mode
    LEA BuskDoneTable(A4),A0    ;already done table
    BSET    #1,0(A0,D2.W)       ;already done this one?
    BNE 60$         ;yes
44$
    ASL.W   #2,D2           ;d1 is index *2 for offset
    LEA BuskCombinations(PC),A0 ;table
    MOVE.W  0(A0,D2.W),D1       ;get message
    BEQ.S   49$         ;none!
    SCN 433         ;tourists stroll by,
    CLR.W   Waiting(A4)
    BCLR    #15,D1          ;containue?
    BNE.S   41$
    DO  P.SCN           ;msg
    BRA.S   42$
41$
    DO  P.SCN
    MOVE.W  2(A0,D2.W),D1
    CMP.W   #438,D1         ;take away coin?
    BNE.S   45$         ;no
    TEST_B  GivenCoin(A4)       ;wont take away after f.coin
    BNE.S   42$         ;just exit
    DO  P.SCN
    MOVE.W  CoinUsed(A4),D0     ; take away coin used (AJS)
    DO  SETOUT          ;fod it
    BRA.S   42$         ;and exit
45$
    PUSH    D1
    DO  P.SCN
    POP D1
    CMP.W   #440,D1         ;make donation
    BNE.S   42$         ;no
    ST  GivenCoin(A4)       ;give coin
    MOVE.W  #NSILVER_COIN,D0
    DO  SETIN
    TEST_B  TriedToBuy(A4)      ;if tried he comes out
    BEQ.S   42$
    MOVE.B  #4,TIMERAFTER4(A4)  ;before station master cant take it
    BRA.S   42$
49$
    SCN 476         ;a group wander past
    CLR.W   Waiting(A4)
42$
    SCN 437         ;walk away
50$
    PULL_L  D1
    UNLINK_ A6
    RTD_    #4
60$

    TEST_B  GivenCoin(A4)       ;has coin been given?
    BEQ.S   50$         ;no, exit
    TEST_B  TriedToBuy(A4)      ;if tried he comes out
    BEQ.S   50$
    TEST_B  TIMERAFTER4(A4)     ;already in progess?
    BNE.S   50$         ;yes, exit
    MOVE.B  #4,TIMERAFTER4(A4)  ;before station master cant take it
    BRA.S   50$         ;exit
    

BuskCombinations

    DC.W    0,0     ;nothing
    DC.W    436,0       ;stare unimpressed
    DC.W    436,0       ;stare unimpressed
    DC.W    434+HI,435  ;look for hat
    DC.W    0,0
    DC.W    436+HI,438  ;unimpressed & take coin
    DC.W    436+HI,438  ;unimpressed & take coin
    DC.W    434+HI,435  ;look for hat
    DC.W    0,0
    DC.W    436,0       ;not impressed
    DC.W    436,0       ;not impressed
    DC.W    434+HI,439  ;watch & change mind
    DC.W    0,0
    DC.W    436+HI,438  ;unimpressed & take coin
    DC.W    436+HI,438  ;unimpressed & take coin
    DC.W    434+HI,440  ;watch & donate

*-----------------------------------

    XDEF    BuskableRoom

BuskableRoomTable

    DC.B    RNOUTSIDE_STATION
    DC.B    RNTRAIN_STATION
    DC.B    RNPLATFORM1
    DC.B    RNPLATFORM2
    DC.B    RNPLATFORM3
    DC.B    0

    ALIGN

BuskableRoom

    XREF    LookUpRoomInTable
* is room buskable?

    MOVE.L  A1,-(A7)
    LEA BuskableRoomTable(PC),A1
    CALL    LookUpRoomInTable
    MOVE.L  (A7)+,A1
    RET

*------------------------------------
    XDEF    AtStation

AtStationTable

    DC.B    RNPLATFORM1
    DC.B    RNPLATFORM2
    DC.B    RNPLATFORM3
    DC.B    RNPLATFORM_BRIDGE
    DC.B    RNOTHER_PLATFORM
    DC.B    RNTRAIN_ROOF
    DC.B    RNON_TRAIN
    DC.B    0
    ALIGN

AtStation

    XREF    LookUpRoomInTable

    MOVE.L  A1,-(A7)
    LEA     AtStationTable(PC),A1
    CALL    LookUpRoomInTable
    MOVE.L  (A7)+,A1
    RET


*------------------------------------------
    XREF    ROOM,GuardSeenTicket
    XREF    BuskingOnTrain,TIMERAFTERF
    XDEF    BuskOnTrain

BuskOnTrain

    CMP.W   #RNON_TRAIN,ROOM(A4)        ;if travelling on train
    BNE.S   90$             ;no ignor it
    TEST_B  GuardSeenTicket(A4)     ;we dont want specials overlap
    BEQ.S   90$             ;ignor busking til seen ticket
    TEST_B  TapDancing(A4)          ;r u dancing?
    BNE.S   10$             ;yes, do u
    TEST_B  PlayHarmonica(A4)       ;or playing
    BEQ.S   90$             ;neither, ignor
10$
    BSET    #0,BuskingOnTrain(A4)       ;test & set
    BNE.S   20$             ;yes, do the player
    SCN 300             ;warning message
    BRA.S   90$             ;exit
20$
    TEST_B  TIMERAFTERF(A4)         ;done bother if eo journey
    BNE.S   90$
    SCN 301             ;throws u into van
    MOVE.W  #RNGUARDS_VAN,ROOM(A4)      ;zap
90$
    RET

*-----------------------------------------

    XREF    FGT_HatchOpened,TIMERAFTER15
    XDEF    StationMasterCloses

StationMasterCloses

    PUSH_L  D0/D5/A0
    CALL    BuskableRoom        ;do you hear the master closing?
    BNE.S   10$         ;no, then no msg
    SCN 243         ;station master closes up
10$
    MOVE.W  #NSTATION_MASTER,D0
    DO  SETOUT
    MOVE.W  #NWOODEN_SHUTTER,D0
    GETINFO
    MOVE.B  #1<<6,(A0)  ;locked
    MOVE.W  #HI+174,FGT_HatchOpened(A4) ; closed ... station desc
    MOVE.W  #HI+174,4+FGT_HatchOpened(A4)   ; closed ... hatch
    CLR.B   TIMERAFTER15(A4)            ; becoz sometimes
                        ; after closes it says closed
    PULL_L  D0/D5/A0
    RET


*---------------------------------------
*
* Called by throw x at y, put x at y, to fod objects put in 'bottomless pits'
*

    XDEF    BlackHoles

    XREF    P.MHAV,IsPermanent

BlackHoles

    CMP.W   #NHOLE,D5       ;into the hole in field?
    BNE.S   90$         ;exit
    CALL    IsPermanent     ;can it be moved?
    BEQ.S   99$         ;no
    DO  CARRIED         ;do u have it?
    BNE P.MHAV          ;no
    DO  P.TN
    SCN 109          ; disappear@ from view.^
    DO  SETOUT
    SETNE               ;happened
    BRA.S   99$         ;exit
90$
    SETEQ
99$
    RET

*----------------------------------

SP.BreakPudding
    PUSH_L  D0/A0
    SCN 429     ; 'The pudding crumbles to ashes!'
    MOVE.W  #NPUDDING2,D0   ; burnt pudding ?
    DO  SETOUT      ; lose the burn offering...
    TEST_B  CharmInCake(A4) ; Is the charm in it??
    BEQ.S   10$
    MOVE.W  #NCHARM4,STOPRON(A4)
    SCN 431     ; A charm falls out of the burn crumbs!
    MOVE.W  #NCHARM4,D0 ; Put the charm...
    CALL    DROPHERE
    SCORE   DOBAKERY
10$:
    PULL_L  D0/A0
    RET

*----------------------------------

    END
