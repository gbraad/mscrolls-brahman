*****************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSU
*
*--------------------------------


    include "nway.i"
    include "equates.i"
*   include "verbequ.i"
    include "nounequ1.i"
*   include "nounequ2.i"
    include "roomequ.i"
    include "macros.i"
    include "scenmacros.i"
    include "scoreref.i"


    XDEF    SP.UNTIE

    XREF    RopeTied
    XREF    SpareTank,AquaTank,ObjectUnderRope

SP.UNTIE

    CMP.W   #NSPARE_TANK,D0
    BEQ RemoveTank
    CMP.W   #NAQUALUNG,D0
    BEQ RemoveTank
    RET

*--------------------------------
*
* SP.UNLIGHT
*
    XDEF    SP.UNLIGHT
    XREF    TIMER5,CandleLifeLeft

SP.UNLIGHT
    CMP.W   #NCANDLE,D0
    BNE.S   90$

    BCLR    #7,(A0)     ;Clear anyway but test the lit bit
    BEQ.S   10$         ;=> Wasn't lit anyway
    DO  P.TN            ;The noun is now
    DO  ISARE
    SCN 212         ; 'extinguished.'
    MOVE.B  TIMER5(A4),CandleLifeLeft(A4)   ;save remain incase re-light
    CLR.B   TIMER5(A4)  ;stop the burning
    BRA.S   80$

10$
    DO  P.BTN           ; 'But the candle'
    DO  ISAREN          ; 'is not' (hmm)
    SCN 211             ; 'lit'

80$
    POPSP
90$
    RET
 
;**********************************

    XDEF    SP.UNLOCK_Before
    XREF    SP.PICKUP

SP.UNLOCK_Before
* Pick gets here (as in pick lock). But I want to catch 'pick roses'
    CALL    SP.PICKUP       ;Roses are caught by this
    BEQ.S   90$         ;=> not interested
    POPSP
90$
    RET

*-------------------------------

    XDEF    SP.UNLOC

    XREF    OpenMailBox

 
SP.UNLOC

    CALL    OpenMailBox     ;Check mailboxes
    BEQ.S   90$
    POPSP
90$
    RET


*-----------------------------------
    XREF    TapDancing,CurrentPlayerClothing

    XDEF    SP.UNWEA_BEFORE

SP.UNWEA_BEFORE
; Can't remove clothing which covers chest if aqualung is worn.
    PUSH_L  D2/D3
    BTST    #3,10(A0)       ; covers chest ?
    BEQ.S   60$         ; no so ok
    PUSH_L  D0/A0
    MOVE.W  #NAQUALUNG,D0       ; wearing the aqualung ?
    GETINFO
    TST_WORNBYPLAYER
    PULL_L  D0/A0
    BNE.S   65$         ; yes so moan
60$
    CMP.W   #NDIVING_SUIT,D0    ; take off wetsuit
    BNE.S   70$
; I want to call CurrentPlayerClothing to find out if the player is
; wearing any other clothing over the wet suit, but this will also
; return the mask of the wet suit in D2 with the others.
; In order to fix this, I will clear the clothing mask of the wetsuit
; temporarily and put it back after.
    MOVE.B  10(A0),D3       ; save clothing mask
    AND.B   #7,10(A0)       ; clear it
    CALL    CurrentPlayerClothing   ; into D2
    MOVE.B  D3,10(A0)       ; restore wetsuit mask
    AND.B   D3,D2           ; intersect with wetsuit mask (in D3)
    BEQ.S   90$         ; EQ => OK
65$
    SCN 98          ; something prevents you
    BRA.S   80$         ; don't do verb
70$
    CMP.W   #NTAPDANCE_SHOES,D0 ;take off shoes
    TEST_B  TapDancing(A4)      ;are you dancing
    BEQ.S   90$         ;no
    SCN 486         ;you cant while dancing
*   BRA.S   80$
80$
    PULL_L  D2/D3
    POPSP_RET
90$
    PULL_L  D2/D3
    RET

*----------------------------------

    XDEF    SP.UNTIE_After,CloseHatch
    XREF    VALID.NP,TIMERAFTER11

SP.UNTIE_After

    CMP.W   #NMANACLES,D0   ;untie manacles?
    BEQ.S   10$     ;yes
    CMP.W   #NXAM,D0    ;untie xam?
    BNE.S   20$
10$
    NVALID  XAM     ;is xam around?
    BNE.S   90$     ;no, exit
    SCN 568     ;xam is free, door closed
    CALL    ReleaseXam  ;set up adata
    BRA.S   80$
20$
    CMP.W   #NDUNGEON_ROPE,D0   ;untie dungeon rope
    BNE.S   90$
    TEST_B  RopeTied(A4)
    BEQ.S   90$         ; the rope was not tied niceley
    BRA.S   CloseHatch
80$
    POPSP
90$
    RET
    XREF    CloseDirection
CloseHatch:
    CLR.B   RopeTied(A4)
    CLR.B   TIMERAFTER11(A4)
    CLR.W   ObjectUnderRope(A4)
    SCN 553
    MOVE.W  #NINNER_HATCH,D0
    MOVE.W  #nway_hatch,D2
    POPSP
    BRA CloseDirection

*--------------------------------------

    XDEF    ReleaseXam
    XREF    XamRescued

ReleaseXam

    ST  XamRescued(A4)      ;flag for future
    SCORE   RELEASEXAM      ;lots of lovely pts
    MOVE.W  #NXAM,D0
    DO  SETOUT
    MOVE.W  #NSOLID_DOOR,D0
    DO  CloseDoor       ;and close it
    RET

*---------------------------------------

RemoveTank


    MOVE.W  #NSPARE_TANK,D0
    GETINFO
    CMP.W   #NAQUALUNG,12(A0)
    BNE.S   90$
    MOVE.W  AquaTank(A4),D1 ;amount left
    LSR.W   #1,D1       ;/2, half taken away.
    MOVE.B  D1,SpareTank(A4)    ;left in spare
    SUB.W   D1,AquaTank(A4) ;lose this much.
    SCN 66
    CLR.W   12(A0)      ;disconnect it
    POPSP
90$
    RET

      END
