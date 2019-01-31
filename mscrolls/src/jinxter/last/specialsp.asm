*****************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*   specialsp
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
    include "scenequ.i"

;********************************************
    XDEF    SP.PUSH

    XREF    CanoeLaunched,ODT_CANOE,DO.SCORE,Denotation_CHARMS
*   XREF    SP.MEDDLE
    XREF    AirlockFlooded,UnderWater,OfficeFlooded
    XREF    SETPL
    XREF    REMASTER
 
SP.PUSH

; Push canoe is like 'launch canoe'
    CMP.W   #NCANOE,D0
    BEQ.S   LaunchCanoe
    RET


    XREF    CanoeFilled

LaunchCanoe
    TEST_B  CanoeLaunched(A4)   ; check not already done.
    BNE.S   90$
    TEST_W  CanoeFilled(A4)     ; make sure that there is no hole
    BNE.S   10$
    SCN 244         ; really it will sink if you put it in
    BRA.S   80$
10$
    SCN 195         ; 'Splash'
    MOVE.W  #NCANOE,D0
    DO  FodLongLook     ;remove old
    MOVE.L  #(611+HI)<<16+NCANOE,D1 ;new look
    DO  EntryLongLook
    ST  CanoeLaunched(A4)   ; Show we've done it
    SCORE   LAUNCHCANOE

*MOVE.L #RNCANOE_MOORING,6(A0)  ; Now not related WHY?
80$                 ; don't go back to v.routine
    POPSP
90$:
    RET

;******************************************
;*
SP.PutCloud
** Putting things on the cloud.
    MOVE.W  D0,STOPRON(A4)      ; for the text...
    SCN 821         ; you put the x on the y but b4...
    POPSP_RET

    XREF    Vanquished,Denotation_CHARMS,WonGame
SP.PutBracelet

*   ADDQ.W  #8,A7           ; no matter what aint going there
* Anita, wot's this??? You aren't that deep! Paul
* Put this in instead...............
    POPSP

    LEA Denotation_CHARMS(A4),A1    ; are all the charms there?
10$
    MOVE.W  (A1)+,D0
    BEQ WonGame             ;The bastard won!
    GETINFO
    CMP.L   #REL.MASK+NBRACELET,6(A0)   ; related to bracelet?
    BEQ.S   10$             ; yeah1
    SCN 790             ; after all that..
    BRA Vanquished          ; and kill...
*30$


    XDEF    SP.PUTON

    XREF    SETOUT,P.TNIN,P.TNSTOP,M.ON,SP.OIL,DROPHERE,CALCWHT
    XREF    SP.LEAN_After,CombineD0D5,UnicornFriendly
    XREF    FlagHearth
    XREF    FlagPubHearthCold,SP.FreezePubHearth,SETPRON
    XREF    SP.WEIGH,PRONHIM,Denotation_CHARMS

* D0 = object, D5 = dest. object

SP.PUTON

    CMP.W   #NHEARTH,D5     ; putting on the hearth?
    BNE.S   001$
    ST  FlagHearth(A4)
001$
    CMP.W   #NPUBHEARTH,D5      ; on Pub hearth
    BEQ.S   002$
    CMP.W   #NPUBFIRE,D5        ; or fire
    BEQ.S   002$
    CMP.W   #NASH,D5        ; or ash
    BNE.S   010$
002$
    MOVE.L  D1,-(SP)
    LIQUID  D1          ; putting a liquid on hearth
    PULL_L  D1
    BNE 90$         ; not a liquid
    TEST_B  FlagPubHearthCold(A4)   ; already put out fire ?
    BNE 90$         ; if so, let verb handle
    MOVE.W  D0,STOPRON(A4)      ; s/ @ /the noun/ in SCN
    SCN 356         ; hissssss! the @ boil@ away
    DO  SETOUT          ; no more liquid
    CALL    SP.FreezePubHearth  ; sets out fire, fixes ash
    MOVE.W  #NASH,D0        ; might as well make ash wet
    GETINFO
    AND.B   #$F0,3(A0)      ; clear texture
    OR.B    #32+7,3(A0)     ; clammy and wet
    BRA 80$         ; no more verbs
010$
    CMP.W   #NNICECLOUD,D5          ; on the nice cloud?
    BEQ SP.PutCloud
    CALL    CombineD0D5
    CMP.L   #NWITCH_HAND<<16+NBRACELET,D0   ; put bracelet on witch
    BEQ SP.PutBracelet
    CMP.L   #NUNICORN2<<16+NSADDLE,D0   ;put saddle on unc?
    BNE.S   01$
    TEST_B  UnicornFriendly(A4)     ;both real by now?
    BNE 90$             ;yes
    SCN 258             ;turns to wood
    MOVE.L  #$1100<<16+NUNICORN,6(A0)   ;saddle is on unicorn
    SWAP    D0
    DO  SETOUT
    MOVE.W  #NUNICORN,D0            ;wooden one
    DO  SETPRON             ;it
    CALL    DROPHERE            ;it's here now
    CLR.W   PRONHIM(A4)         ;not an npc anymore
    POPSP_RET
01$
    CALL    SP.PICKUP   ; check we can pick up the dir. object
    BNE 80$

    CMP.W   #NSCALES,D5 ;puting things on the scales
    BEQ SP.WEIGH    ;do it there instead.
* Put oil on X == oil x with oil.

10$
    CMP.W   #NOIL,D0    ; is it the oil??
    BNE.S   15$
    EXG D5,D0
    EXG A5,A0
    BRA SP.OIL      ; As if 'oil y with x'

15$
    CMP.W   #NMOUSE_TRAP,D5 ;trying to put things on the mouse trap?
    BEQ SP.PUTanythingONTrap
* putting stuff on the bracelet?
    CMP.W   #NBRACELET,D5       ; put x on bracelet...
    BNE.S   60$
* is it a charm?
    MOVE.L  A1,-(A7)
    LEA Denotation_CHARMS(A4),A1
    DO  MEMBEROFSET
    PULL_L  A1
    BNE.S   90$
* Come here to 'do' the put x on y.
50$
    DO  FIXCONT     ; This fixes up containers!
    MOVE.W  #REL.MASK>>16,6(A0) ; related...
    MOVE.W  D5,8(A0)    ; ... to the indirect object.
    MOVE.W  #M.ON,D1    
        DO  P.TNIN      ; 'The X is now on '...
    MOVE.W  D5,D0
        DO  P.TNSTOP    ; 'the Y.'
* are all the charms on the bracelet yet ?
    LEA Denotation_CHARMS(A4),A1
55$
    MOVE.W  (A1)+,D0
    BEQ.S   56$     ; end of list
    GETINFO
    CMP.L   #REL.MASK+NBRACELET,6(A0)   ; on bracelet?
    BNE.S   80$     ; no so exit
    BRA.S   55$     ; next charm
56$
    SCORE   ASSEMBLEBRACELET ; bracelet has got all charms on it now
    BRA.S   80$
60$
    BRA SP.LEAN_After   ;Suss ladder (Yukky syntax!!)
80$
    POPSP
90$
    RET

SP.PUTanythingONTrap

    MOVE.B  2(A0),D1    ;get size&weight
    LSR.B   #4,D1       ;just want size
    CMP.B   #1,D1       ;must be 2 or less to fit
    BEQ.S   10$
    DO  P.TN
    MSG WNTF        ;won't fit
    MSG ON
    MOVE.W  D5,D0
    DO  P.TNSTOP
    POPSP
10$
    RET





*******************************************************
*
* SP.PLAY - piano/organ/billiards, and LUTE!
*
    XDEF    SP.PLAY
    XREF    PlayHarmonica,BeginBusking,CONJ

SP.PLAY

* Allow player to play with the (toy!) dragon.

    CMP.W   #NDRAGON,D0
    BNE.S   10$
    CMP.W   #WITH,ADVERB(A4)
    BNE.S   90$
    SCN 129     ; 'you play with the dragon'
    BRA.S   80$
10$:
    CMP.W   #NHARMONICA,D0
    BNE.S   90$
    SCN 462     ;you play the harmonica
    ST  PlayHarmonica(A4)   ;true
    ARGL    0           ;print if nec.
    CALLS   4,BeginBusking
    CLR.B   PlayHarmonica(A4)
80$:
    POPSP
90$
    RET

*******************************************************
*
* SP.PISS
*

    XDEF    SP.PISS
    XREF    PissInLoo,CLOCK
SP.PISS
    CMP.W   #NLAVATORY,D0
    BNE.S   90$
    TEST_W  PissInLoo(A4)       ;if zero then ok
    BEQ.S   05$
    MOVE.W  CLOCK(A4),D1        ;time now
    SUB.W   PissInLoo(A4),D1    ;difference
    CMP.W   #50,D1          ;another 50 moves?
    BLT.S   10$         ;no
05$
    SCN 316         ;much better!
    MOVE.W  CLOCK(A4),PissInLoo(A4) ;update
    BRA.S   80$         ;exit
10$
    SCN 317         ;not again
80$
    POPSP
90$
    RET

;********************************
;
    XDEF    SP.POINT
SP.POINT
      RET

;********************************
;
;   XDEF    SP.PRAY
;
;SP.PRAY
;   RET

*********************************
*
    XDEF    SP.PADDLE
SP.PADDLE
    RET

*********************************
*
* SP.PULL
*
    XDEF    SP.PULL

SP.PULL

; Check for launching the canoe.
    CMP.W   #NCANOE,D0
    BEQ LaunchCanoe
    CMP.W   #NDUNGEON_ROPE,D0       ;pull rope
    BNE.S   90$             ;no
    MOVE.W  #NINNER_HATCH,D0
    GETINFO
    TST_CLOSED              ;closed?
    BEQ.S   10$             ;no
    SCN 555             ;open shutter
    BRA.S   80$
10$
    MSG NHAP
80$
    POPSP
90$
    RET

*--------------------------------
* SP.PRESS
* 
    XDEF    SP.PRESS

    XREF    ROOM,TimesPressed
    XREF    RungBell,GetWet,TIMER9,SETIN

BaseBmess   EQU 310
MaxBmess    EQU BaseBmess+3


SP.PRESS
    CMP.W   #NBUTTON,D0     ;Ringing bell on bus?
    BNE.S   40$
    ST  RungBell(A4)
    SCN 309
* Ding!
    CLR.W   D1
    MOVE.B  TimesPressed(A4),D1
    ADD.W   #BaseBmess,D1
    CMP.W   #MaxBmess,D1
    BLT.S   30$
    MOVE.W  #BaseBmess,D1
    CLR.B   TimesPressed(A4)
30$
    DO  P.SCN
    ADDQ.B  #1,TimesPressed(A4)
*   MOVE.W  #NKEYRING,D0        ;Drop the keys
*   DO  SETOUT          ;Fix tied etc
*   GETINFO
*   MOVE.L  #REL.MASK+NONBUS,6(A0) ;Related so look doesn't spot 'em
    BRA 80$

40$

; Working the airlock ??
    CMP.W   #NFLOOD_BUTTON,D0
    BNE.S   50$
    TEST_B  AirlockFlooded(A4)  ;already?
    BEQ.S   42$         ;no
    SCN 149         ;nothing happens
    BRA 80$
42$
    CALL    AirlockDoorOpen     ;inner door open!! (note:does GETINFO on AIRLOCK_DOOR)
    BEQ.S   43$         ;no
    SCN 134     
* "Oh no! you'll flood the whole place"
45$
    CALL    GetWet
    MOVE.B  #1,TIMER9(A4)
    BRA.S   80$
43$
    TST_LOCKED          ;locked?
    BNE.S   44$         ;yes, ok
    TEST_B  OfficeFlooded(A4)   ;flooded office already?
    BNE.S   44$         ;yes so ok
    SCN 327         ;door is forced open
    DO  OpenDoor        ;open the door and connect net.
    BRA.S   45$         ;and begin the flood
44$
    ST  AirlockFlooded(A4)  ;fill up the airlock
    ST  UnderWater(A4)      ;now underwater
    SCN 132         ;chamber floods
    CALL    GetWet
    BRA.S   80$         ;exit
50$
    CMP.W   #NFLUSH_BUTTON,D0   ;?
    BNE.S   60$
    TEST_B  AirlockFlooded(A4)  ;already?
    BNE.S   52$         ;no
    SCN 149         ;nothing happens
    BRA.S   80$
52$
    MOVE.W  #NHATCH,D0
    GETINFO
    TST_OPEN            ;open?
    BEQ.S   54$         ;no
53$
    SCN 215         ;bubbles but nhap
    BRA.S   80$         ;quit
54$
    TEST_B  OfficeFlooded(A4)   ;office been flooded?
    BEQ.S   55$         ;no, so ok
    CALL_S  AirlockDoorOpen     ;factored out for space reasons
    BNE.S   53$         ;door open, so can't flood airlock
55$
    SCN 133         ;flush water out
    CLR.B   UnderWater(A4)      ;no longer under
    CLR.B   AirlockFlooded(A4)
    BRA.S   80$
60$
    CMP.W   #NOVEN_BUTTON,D0
    BEQ.S   BakeCakes
90$
    RET
80$
    POPSP_RET

; factored from above special since it is used twice
AirlockDoorOpen
    MOVE.W  #NAIRLOCK_DOOR,D0
    GETINFO
    TST_OPEN
    RET

    XDEF    BakeCakes

    XREF    BurntCake,BakerFollows,BakerEject2,CharmInCake

********************************************************************************
* Cope with all the cake ingredients, etc, setting them out, doped/undoped
* cakes, etc. Generally faffy.
*
BakeCakes

; Check the oven is closed, and that cake tin is in oven?
;
    MOVE.W  #NOVEN,D0
    GETINFO
    TST_CLOSED              ; check the oven is closed
    BEQ 90$     ; If oven is open, then nothing happens
    PUSH_L  A0/A5/D0/D4/D5  ;call default cook code
    CALL    CookAllInOven   ;So stuff inside it gets appropriate treatment
    PULL_L  A0/A5/D0/D4/D5
    SCN 189         ; Oven whirrs a bit, then 'ping!'
80$:
    POPSP
90$:
    RET 

*-----------------------------

****************************************************************************
**
** Cook all in the oven
**
** a recursive little routine that will try to cook all that is in the oven.
**
****************************************************************************

    XDEF    GetAllIn
GetAllIn:
* This will really, really, really return a list of all the objects that
* are associated with an object
* (and will return it on the stack)
* Takes D2 as top level (MUST be an object (not a room)).
* returns stack...
    MOVE.L  (A7)+,A3    ; place where we came from...
    MOVE.W  #0,-(A7)
    MOVE.L  A7,A6       ; a6 is our subsiduary pointer
10$
    DO  GETRELN2    ; returns A1 pointing at a list.
15$
    MOVE.W  (A1)+,D0    ; EQ +> fini
    BEQ.S   20$     ; Fini.
    BMI.S   15$     ; hi bit means don't worry
    GETINFO
    TST_OUT         ; out...?
    BNE.S   15$     ; yes we don't want it.
    TST_PSEUDO      ;no pesudos!
    BNE.S   15$     ;next
    TST_ISAROOM     ;no rooms either!
    BNE.S   15$     ;next
    TEST_B  6(A0)       ;check format compatability
    BEQ.S   15$     ;if literal location, lose it
    MOVE.W  D0,-(A7)    ; save away one we want.
    BRA.S   15$
20$
    CMP.L   A7,A6       ; have our pointers met?
    BEQ.S   30$
    MOVE.W  -(A6),D2    ; the next noun
    BRA.S   10$     ; and continue...
30$
    JMP (A3)        ;return

    XREF    DNOUNBUF,GETRELN2,P.LIST3
CookAllInOven:

    MOVE.W  #NOVEN,D2   
    CALL_S  GetAllIn    ; returns a list on the stack.
* returns a 0 terminated list on the stack..
    MOVE.L  A7,A6
    CLR.W   D3          ; used for getting the right byte
    LEA CookTable(PC),A1    ; list of data entries.
    LEA DNOUNBUF(A4),A5
5$
    MOVE.L  A5,A2       ; dnounbuf pointer
    MOVE.L  A7,A6       ; point the stack back where it is
    MOVE.B  (A1),D3     ; the byte number.
10$
    MOVE.W  (A6)+,D0    ; our next noun.
    BMI.S   10$     ; already dealt with ?
    BEQ.S   30$     ; 0 termiated list.
    CALL    CookBread   ; check for dough or bread & cook it if nesc
    BNE.S   15$     ; if bread not cooked, do general cook code
    OR.W    #$8000,-2(A6)   ; flag as done already, so it wont be redone
    BRA.S   10$     ; and do the next item in the list
15$
    GETINFO
    MOVE.B  0(A0,D3.W),D4   ; the byte that we want to and.
    AND.B   1(A1),D4    ; AND the mask
    TEST_B  2(A1)       ; EQ => means just set will do
    BNE.S   18$
    TEST_B  D4
    BEQ.S   10$
    CLR.B   D4
18$
    CMP.B   2(A1),D4    ; now check the compare.
    BNE.S   10$     ; not one we want so continue.
    TEST_B  3(A1)       ; NE => goes out.
    BEQ.S   20$
    DO  SETOUT      ; get it out
20$
    MOVE.L  6(A1),A0    ; call special for this frame for each match
    JSR 0(A4,A0.L)
    OR.W    #$8000,-2(A6)   ; flag as already done
    MOVE.W  D0,(A2)+    ; put it in our buffer
    BRA.S   10$     ; and continue.
30$
* okay we have finished this section.
    CMP.W   A2,A5       ; Did we have any?
    BEQ.S   40$     ; nope so continue onto next frame.
    CLR.W   (A2)+       ; zero terminated list.
    MOVE.L  A5,A0       ; for p.list.
    MOVE.L  A1,-(SP)
    MOVE.L  A0,A1       ; for countlist
    DO  COUNTLIST   ; -> D4.W
    MOVE.L  (SP)+,A1
    TEST_W  D4      ; any in list?
    BEQ.S   40$     ; no so no printing
    DO  P.LIST3     ; print the list.
    CALL    SETPL
    MOVE.W  4(A1),D1    ; scenario message associated.
    DO  P.SCN
40$
    ADD.W   #10,A1      ; next frame
    CMP.W   #-1,(A1)    ; -1 word terminated
    BNE 5$
    MOVE.L  A6,A7       ; reset the stack
    RET

CookTable:
* liquid
    DC.B    1,$F0,$10,-1
    DC.W    201         ; X boils away
    DC.L    SP.CookLiquid
* burns
    DC.B    11,$18,$08,-1
    DC.W    203         ; X burns away
    DC.L    SP.CookBurns
* melts
    DC.B    11,$18,$10,-1
    DC.W    204         ; X melts away
    DC.L    SP.CookMelts
* cooks
    DC.B    11,$e0,$00,-1
    DC.W    202         ; X burns to a crisp
    DC.L    SP.CookCooks
    DC.W    -1          ; terminator

; specials called by CookAllInOven to do anything weird associated with
; whatever happened inside the oven i.e. SETIN(NCRISP)
SP.CookLiquid
SP.CookBurns
SP.CookMelts
    RET

SP.CookCooks
    PUSH_L  D0
    MOVE.W  #NCRISP,D0
    DO  SETIN
    PULL_L  D0
    RET

; called by CookAllInOven to cook the bread if it is in tin in oven
; parameters : D0 = thing do be checked as to whether it is bread
; returns EQ if bread cooked, NE otherwise
CookBread
    CMP.W   #NINGREDIENTS,D0    ; only affect the dough or bread
    BEQ.S   10$
    CMP.W   #NPUDDING,D0
    BNE.S   90$
10$
    PUSH_L  D0      ; remember which
    MOVE.W  #NCAKE_TIN,D0   ; Is the tin in the oven??
    GETINFO
    PULL_L  D0      ; get it back
    CMP.L   #CONT.MASK+NOVEN,6(A0)
    BNE.S   90$     ; Don't affect oven contents at all
    MOVE.L  #CONT.MASK+NCAKE_TIN,D1
    GETINFO
    CMP.L   6(A0),D1
    BNE.S   50$
    DO  P.TN            ; "the X begins to cook"
    SCN 729
    BSET    #7,6(A0)        ; Set it out - but don't tweak volumes
    CMP.W   #NINGREDIENTS,D0    ; was it the bread dough ?
    BNE.S   50$         ; if not, don't muck about with charm
    MOVE.W  #NCHARM4,D0
    GETINFO
    CMP.L   6(A0),D1        ; Is charm in dough
    BNE.S   40$
    DO  SETOUT          ; Set it out  (for real!)
    ST  CharmInCake(A4)
40$:
    MOVE.W  #NPUDDING,D0        ; dough turns into bread
    BRA.S   60$
50$:
    MOVE.W  #NPUDDING2,D0       ; bread turns into burnt bread
60$:
    DO  SETIN           ; Set pudding (or pudding2) IN! 
    SETEQ_RET
90$
    SETNE_RET



;*************************************************
    XDEF    SP.CanoeHole
    XREF    CanoeFilled
SP.CanoeHole
* D0 = direct object (expecting x as in put x)
* D5 = indirect object (expecting hole)
    CMP.W   #NCANOE_HOLE,D5     ; the hole is the important one
    BNE 90$         ; nah - go away
* is the hole already filled?
    TEST_W  CanoeFilled(A4)     ; NE means it's filled
    BEQ.S   10$
    SCN 249         ; hole is as full as it is ever 
    BRA.S   80$         ; go bypassing v.routine
10$
    CMP.W   #NDEAD_MOUSE,D0     ; the dead mouse?
    BNE.S   20$
    SCN 260         ; well i suppose 101 uses..
    BRA.S   32$         ; popsp ret
20$
    CMP.W   #NBUNG,D0       ; just the bung?
    BNE.S   30$
    SCN 593         ; it doesn't quite fit...
    BRA.S   80$         ; bypass v.routine
30$
    CMP.W   #NSOCK,D0       ; the sock...
    BNE.S   40$         ; sniff...nothing is working.
    MOVE.W  #NBUNG,D0
    GETINFO             
    MOVE.W  #NSOCK,D0       ; put the sock back
    CMP.L   #CONT.MASK+NSOCK,6(A0)  ; contained by the sock
    BNE.S   40$         ; put back the sock..
    SCN 594         ; congratulations
    GETINFO             ; for the sock.
32$
    MOVE.W  D0,CanoeFilled(A4)  ; Says what is in it and that is full
    POPSP_RET
40$
    GETINFO
    DO  P.TN            ; print the x.
    MOVE.W  D0,STOPRON(A4)      ; for the @
    MOVE.B  2(A0),D3        ; to get the size.
    LSR.B   #4,D3           ; get it into the bottom bits.
    AND.B   #$F,D3          ; that is all we want.
    CMP.B   #2,D3           ; less than 2 it is too small..
    BHI.S   50$         ; too big.
    SCN 609         ; it falls straight through the hole
    BRA.S   80$         ; bypass v.routine
50$
    SCN 596         ; it is too large
80$
    ADDQ.W  #8,A7           ; bypass special AND v.routine
90$
    RET
**********************************************************************

    XDEF    SP.PutBefore
    XREF    ADVERB,D0SINKS,SINKD0
    XREF    EntryLongLook,BlackHoles

SP.PutBefore
* this special gets called before it does anything (much) with the verb routine
* The verb routine has checked for IN and INTO though and done a chk4pt
* Note that 'fill' gets here too.

    CALL    SP.CanoeHole    ; if anything happened won't come back
    CALL    BlackHoles  ;Put x into black hole?
    BNE 80$     ;=> Did summit, exit

70$:
    CALL    SP.PICKUP
    BNE.S   80$
    CMP.W   #NCANOE,D0
    BNE.S   73$     ; No canoe No launch
    CMP.W   #NLAKE2,D5  ; in lagoon?
    BEQ LaunchCanoe
    CMP.W   #NWATER,D5  ; or in the water
    BEQ LaunchCanoe

73$
***** okay this is an experiment for put stuff in water or 
***** put stuff in lagoon.
    CMP.W   #NWATER,D5      ; put x in water
    BEQ DoWaterLagoon
    CMP.W   #NLAKE2,D5      ; or put x in lagoon
    BEQ DoWaterLagoon       
    CMP.W   #NLAKE1,D5      ; the room laggon?
    BEQ DoWaterLagoon       
    CMP.W   #NSLOT,D5       ; Into the slot?
    BNE.S   90$
    CMP.W   #NAQUITANIAN_FERG,D0    ; Ferg coin?
    BNE.S   90$
    DO  SETOUT
    SCN 223         ; Coin disappears
    TEST_B  TIMER14(A4)     ; Is jukebox on?
    BNE.S   75$         ; yes, so just fod up timer
    SCN 225         ; juke box starts up.
75$:
    MOVE.B  #15,TIMER14(A4)     ; re-start timer life.
*   BRA.S   80$
80$
    POPSP
90$:
    RET


*----------------------------

 
    XREF    IsRoomWet,CONJ,DROP,SEATED,SEATED_RELATOR,R.NOUN
    XDEF    DoWaterLagoon,DoWaterLagoon2
    XREF    FloodObjects
*****
DoWaterLagoon
* It gets here via a put special - on a branch ...Once here
* it can never return.
    POPSP           ; WE Will NEVER get back to v.routine now.
DoWaterLagoon2          ; returns to where called
    PUSH_L  D0/A0
    DO  R.NOUN      ; ->d0
    GETINFO
    TST_WET             ; is it wet
    BNE.S   10$     ; not a wet room.. but with water
    SCN 614     ; you can only SEE the water here.
    PULL_L  D0/A0
    RET
10$
*** The object to be set simply in the room....
    PULL_L  D0/A0
    TST_INTRINSIC               ; is it intrinsic?
    BEQ.S   20$
    SCN 619     ; you must be joking!
    RET
20$
    ;********** EDS Hugh

    MOVE.L  D0,-(A7)    ;arg1
    MOVE.L  #1,-(A7)    ;nargs
    CALL    FloodObjects    ;you will need to flood it as well
    ADDQ.L  #8,A7       ;adjust

    ;**********

    DO  SETOUT      ; get rid of it for now.
    MOVE.W  ROOM(A4),8(A0)
    CLR.W   6(A0)       ; the object is simply in the room.
    DO  P.TN        ; the x
    DO  ISARE       ; is
    SCN 616     ; now in the water
    RET

*--------------------------------
*
* SP.PUT - CALLED AT THE *END* 
*          OF PUT - ONCE ACTION
*          HAS HAPPENED *MUST*
*          RETURN TO VERB ROUTINE
*
*--------------------------------

    XDEF    SP.PUT,SP.PUT2
    XREF    D0SINKS,P.PRN,TIMER14

SP.PUT
    CMP.W   #NFLOUR_SACK,D5     ; put into sack ?
    BNE.S   90$
    PUSH_L  D1          ; putting liquid into sack ?
    MOVE.B  1(A0),D1
    AND.B   #$F0,D1
    CMP.B   #$10,D1
    PULL_L  D1
    BNE.S   90$
    SCN 541         ; "You'd make a terrible mess"
80$
    POPSP
90$
    RET

    XREF    STOPRON,DROPHERE

SP.PUT2

* These exchanges have been inserted wrt a discrepency
* In the parameters for SP.PUT2
*
* The code in PUT.elt had been modified to pass
* std parameters whereas the sp.put2 needs them in the
* reverse order (for the guild etc).

    EXG D0,D5
    EXG A0,A5
    CMP.W   #NBELL,D5   ; into bell?
    BNE.S   10$
    CMP.W   #NSOCK,D0   ; sock?
    BEQ.S   10$     ; then ok
    MOVE.W  D0,STOPRON(A4)
    SCN 366     ; the X falls out
    CALL    DROPHERE    ; onto the ground
    BRA.S   80$
10$
* Are we in the Dungeon?
    CMP.W   #NCANDLE,D0
    BNE.S   90$
    CMP.W   #RNDUNGEON,ROOM(A4) ; well?
    BNE.S   90$
    CMP.W   ObjectUnderRope(A4),D5  ; is it the object that is under hat?
    BNE.S   90$
* okay putting the candle in the object that is under the rope ... so
    EXG D5,D0
    EXG A0,A5
    BRA DoUnder
80$
    POPSP
90$
    RET

;*******************************************************************************
; Sieving the flour for the charm?
;
    XREF    GotCharm4,BakerEject
    XDEF    SP.SIEVE

SP.SIEVE
    CMP.W   #NFLOUR,D0      ; Sieving flour??
    BNE.S   90$
    CMP.W   #NSIEVE,D5      ; ... with sieve??
    BNE.S   90$
    SCN 533         ; you sieve the flour ...
    TEST_B  GotCharm4(A4)       ; got charm alread?
    BNE.S   80$
    SCN 532         ; You find a charm!
    ST  GotCharm4(A4)
    MOVE.W  #NCHARM4,D0
    DO  SETIN
    TEST_B  BakerFollows(A4)    ; Is baker here???
    BEQ.S   80$
    CALL    BakerEject      ; baker chucks you out!
80$:
    POPSP
90$:
    RET
*--------------------------------
;   XDEF    SP.PLANT
; 
;SP.PLANT
;   RET                     

*-------------------------------

    XDEF    SP.PICKUP
; Called by other specials whenever player does soemthing involving picking up
; objects ( or potentialy doing so )

    XREF    WHOOPS,FlagRunover,SEATED,FlagBull
    XREF    TIMER11
    XREF    LoseTicketState
    XREF    ROOM,DomeHasShattered,FodLongLook

    XREF    CanoeFilled,CanoeLaunched
SP.UnBung
* getting the bits from the canoe whilst it is not launched..
    CMP.W   #NBUNG,D0       ; the bung?
    BNE.S   10$         ; nope
    CMP.W   #NSOCK,CanoeFilled(A4)  ; and the sock is in the hole...
    BNE.S   10$         ; nope
* they are trying to get the bung out of the sock when the sock is in the
* hole..... I don't like this one little tiny bit....
    SCN 613
    BRA.S   20$         ; popsp and ret
10$
    CMP.W   CanoeFilled(A4),D0  ; is it us?
    BNE.S   90$
    TEST_B  CanoeLaunched(A4)       ; for safety.?
    BEQ.S   30$
* it really shouldn't get here..
    SCN 612
20$
    SETNE_RET
30$
    CLR.W   CanoeFilled(A4)     ; clear it coz it is going.
90$
    SETEQ_RET

TicketPosTable

    DC.B    RNPLATFORM2
    DC.B    RNPLATFORM3
    DC.B    RNRAILWAY_TRACK
    DC.B    0
    ALIGN

    XREF    FodLongLook
    XREF    Do_Keys
    XREF    LadderLeantAgainst,NRT_RUNGSLOOK,NRT_RUNGSEXAMINE
    XREF    ISIN,OpenCloseHatches,TIMERAFTER8

SP.PICKUP
    CALL    OpenCloseHatches
    BEQ 80$
    CMP.W   ObjectUnderRope(A4),D0  ; the object that is under the rope?
    BNE.S   011$
    CLR.W   ObjectUnderRope(A4) ; clear it...
011$
    CMP.W   #NWATER,D0      ;water in lake?
    BEQ.S   001$
    CMP.W   #NWATER2,D0     ;some more water (under airlock)
    BNE.S   002$
001$
    SCN 368         ;no chance matey!
    BRA 80$
002$
    CALL_S  SP.UnBung       ; trying to unbung the canoe
    BNE 80$         ; NE=> something happened

    CMP.W   #NLADDER,D0     ;Manipulate ladder?
    BNE.S   006$
    CLR.W   LadderLeantAgainst(A4)  ;Fod it's relationship
    BSET    #7,NRT_RUNGSLOOK(A4)    ;and Noreaches
    BSET    #7,NRT_RUNGSEXAMINE(A4)
    BRA 90$

006$
    CMP.W   #NSTOOL,D0      ;Manipulate stool?
    BNE.S   010$
    MOVE.B  3(A0),D1        ;Get texture etc
    AND.B   #$F,D1          ;fod the etc
    CMP.B   #TEXTURE.HOT,D1     ;Hot?
    BNE 90$         ;Nope, let 'em have it
    MOVE.W  #NGLOVES,D0     ;bastard may try it with gloves
    DO  ISIN            ;Gloves still in? (A0 -> data)
    BNE.S   008$
    TST_WORNBYPLAYER        ;worn?
    BEQ.S   008$            ;=> nope
    DO  SETOUT          ;Fod them
    DO  P.TN            ;The gloves
    MSG BURNAW          ;'burn away.'
008$
    SCN 799
* The stool is too hot to hold and you drop it quickly.

    BRA 80$

010$
    CMP.W   #NSOLUTION,D0       ; trying to manipulate the solution
    BNE.S   012$
    SCN 626         ; no fucking way...
    BRA 80$
012$
    CMP.W   #NCHARM2,D0         ;get dragon charm
    BNE.S   013$
    TST_EXPLORED                ;is it explored?
    BNE.S   013$                ;yes then dont bother
    MOVE.W  #NCHANDELIER,D0
    DO  FodLongLook         ;remove longlook
    MOVE.W  #NTOP_CHANDELIER,D0
    DO  FodLongLook         ;remove longlook
    MOVE.W  #NCHARM2,D0         ;restore
013$
    CMP.W   #NGLASS_FRAGMENTS,D0
    BNE.S   014$
    SCN 520             ;you daren't
    BRA 80$
014$
    CMP.W   #NBRACELET,D0           ;get bracelet?
    BNE.S   01$
    TEST_B  DomeHasShattered(A4)        ;covered in glass
    BEQ.S   01$
    MOVE.W  #NGLOVES,D0
    GETINFO
    TST_WORNBYPLAYER                ;worn?
    BNE.S   02$             ;yes
    SCN 564             ;you prick your finger.
    TEST_B  REMASTER(A4)        ; remaster does not insist you use gloves.
    BNE.S   03$
    BRA 80$             ;and exit
02$
    SCN 565             ;gloves protect u
03$    
    CLR.B   DomeHasShattered(A4)        ;incase drop it
    MOVE.W  #NBRACELET,D0           ;better set it up again
    DO  FodLongLook
    GETINFO
    BRA 90$             ;and exit.
01$
; ticket & breeze problem
    CMP.W   #NTRAIN_TICKET,D0
    BNE.S   09$
    TEST_B  LoseTicketState(A4)     ;is the breeze on?
    BEQ.S   09$
    MOVE.B  LoseTicketState(A4),D1      ;get it
    SUBQ.B  #1,D1
    EXT.W   D1
    LEA TicketPosTable(PC),A1
    MOVEQ   #0,D2               ;zeros in most of loc
    MOVE.B  0(A1,D1.W),D2           ;next room tobe
    BEQ.S   08$             ;if end leave it
    MOVE.L  D2,6(A0)            ;fod the ticket there
    SCN 473             ;ticket moves
    ADDQ.B  #1,LoseTicketState(A4)      ;and inc counter
    BRA 80$
08$
                                ;End of special! 
    TEST_B  REMASTER(A4)
    BNE.S   07$
    CLR.B   LoseTicketState(A4)     ;for remaster, we keep the state for the puzzle assistant
07$

    TEST_B  TIMERAFTER8(A4)         ;train only leaves if it hasnt!
    BEQ 90$             ;if already then normal
    TEST_B  REMASTER(A4)
    BNE     90$                 ; doesnt leave for remaster either.
    PUSH    D0
    SCN 474              get ticket & miss train
    ARGL    0               ;dont print leaving message
    CALLS   4,TrainLeaves
    MOVE.W  #RNPLATFORM3,ROOM(A4)
    POP D0
    GETINFO
    CLR.L   6(A0)           ;youve got it
    BRA 80$         ;exit
09$

    CALL    Do_Keys         
* If individual key referenced, this will put NKEYRING in D0 and do
* a GETINFO. Also does an ST WHOOPS. 

    CMP.W   #NKEYRING,D0
    BNE.S   20$

    XREF    FlagHassleFromFuckHead

    TEST_B  FlagHassleFromFuckHead(A4)  ;been thrown off bus?
    BEQ.S   20$

; Spot picking up keys or keyring while in lane.
    TEST_B  FlagRunover(A4)     ;Been run over yet?
    BNE.S   20$
****
*
* Deal with being runover
*
*
    XREF    FlagNothingDoing,EXTRA,RELOBJ,BakerEject
    XREF    SP.LiftPhone
    


    ST  WHOOPS(A4)      ;kill multiple objects
    CLR.L   6(A0)           ;Give player keyring
    SCN 306             ;Bus bears down on you spiel

    ST  FlagNothingDoing(A4)    ;to kill DECODE+start event
    CLR.W   EXTRA(A4)

    BRA 80$
*
*
*********

20$
    CMP.W   #NROSES,D0
    BEQ.S   22$
    CMP.W   #NMAGAZINES,D0
    BNE.S   30$
22$
    SCN 80
* They're only here for decoration!
    BRA 80$
30$:
    TEST_B  BakerFollows(A4)    ; Baker following you around??
    BEQ.S   50$
    MOVE.W  #NCHARM4,D2
    CALL    RELOBJ          ; Is charm related to object
    BNE.S   50$
    CMP.W   D0,D2           ; or IS it the charm?
    BNE.S   45$
    MOVE.W  D0,STOPRON(A4)      ; 395 has an @ in it
    SCN 395         ; Taking the charm, eh?
    BRA.S   47$
45$:
    BSET    #14,D0
    MOVE.W  D0,STOPRON(A4)      ; no trailing space...
    SCN 485         ; rummages in @ for charm...
47$:
    CALL    BakerEject
    BRA.S   80$         ; Throws you out!
50$:
    CMP.W   #NFLOUR,D0
    BNE.S   70$
    SCN 541         ; 'You'd make a terrible mess!
    BRA.S   80$
70$
    CMP.W   #NASH,D0
    BNE.S   75$
    PUSH_L  D1
    MOVE.B  3(A0),D1        ; get texture
    AND.B   #15,D1
    CMP.B   #3,D1           ; is it hot ?
    PULL_L  D1
    BNE.S   90$         ; nope, so ok
    SCN 210
* rrrrrRRRRRRrrr! Imminent finger burning situation Captain!
* Action aborted due to extreme pain!
    BRA.S   80$
75$
    CALL    SP.LiftPhone        ; getting the phone?
    BEQ.S   90$
80$:
    SETNE_RET
90$:
    SETEQ_RET           ; Don't abort

*------------------------------------------

    XREF    CONNECT2,TIMERAFTER7,TIMERAFTER8
    XDEF    TrainLeaves
    XREF    GETLITERALS,FodList
    XREF    Waiting

TrainLeaves ;(printmode.l)
 
* printmode <=> produce message

    LINK_   A6,#0
    PUSH_W  D0-D2
    TEST_L  8(A6)       ;output mode?
    BEQ.S   10$     ;no messages
    SCN 490     ;the train leaves
    CLR.W   Waiting(A4)
    CMP.W   #RNRAILWAY_TRACK,ROOM(A4)
    BNE.S   10$
    SCN 353     ;you move
    MOVE.W  #RNPLATFORM3,D1
    DO  MovePlayer
10$
    CLR.W   D2      ;nowhere
    MOVE.W  #DIR_E,D1
    MOVE.W  #RNPLATFORM1,D0
    DO  CONNECT2
    MOVE.W  #RNPLATFORM2,D0
    DO  CONNECT2
    MOVE.W  #RNPLATFORM3,D0
    DO  CONNECT2
    MOVE.W  #DIR_W,D1
    MOVE.W  #RNON_TRAIN,D0
    DO  CONNECT2
    MOVE.W  #DIR_D,D1
    MOVE.W  #RNPLATFORM_BRIDGE,D0
    DO  CONNECT2
    MOVE.W  #DIR_U,D1
    MOVE.W  #RNTRAIN_ROOF,D0
    DO  CONNECT2
    MOVE.B  #3,TIMERAFTER7(A4)  ;train leaving delay
    CLR.B   TIMERAFTER8(A4)     ;no default leaving
    MOVE.W  ROOM(A4),D1     ;save your room
    MOVE.W  #RNRAILWAY_TRACK,ROOM(A4)
    CALL    GETLITERALS     ;->a1 all in room
    CALL    FodList         ;zap them
    MOVE.W  D1,ROOM(A4)     ;restore
    PULL_W  D0-D2
    UNLINK_ A6
    RTD_    #4

**************************************************************
*
* SP.POST (mail&send)
*
*

    XDEF    SP.POST
    XREF    Postage,PostItem,PostDest,P.MHAVSTOP,CARRIED
    XREF    P.SMALLNUM,DracItem
    XREF    WeirdAttire,TIMERAFTERE,TIMERAFTER1A


*   POST Thingy TO place
*   D0 = Thingy, D5 = place, on entry to sp.post

SP.POST
    MOVE.L  D0,-(A7)
    MOVE.W  #NPOSTMISTRESS,D0   ; is postmistress here ?
    DO  VALID.NP
    PULL_L  D0
    BNE 90$
    CALL    WeirdAttire ; wearing flippers, wetsuit etc.
    BEQ.S   3$
    MOVE.W  #HI+NPOSTMISTRESS,STOPRON(A4) ; no leading space
    SCN 363
* @ cracks up laughing.
    BRA 80$     ; done
3$
    TEST_W  PostItem(A4)    ; Something being posted??
    BEQ.S   10$
    TEST_B  TIMERAFTERE(A4) ; paid for it?
    BEQ.S   4$
    SCN 380     ; "I'm busy"
    BRA 80$
4$
    SCN 483     ; 'I Haven't posted this one yet!'
    BRA 80$
10$:
; Can only post things you're carrying (check intrinsic after this bit...)
15$:
    MOVE.L  A1,-(SP)
    LEA Denotation_CHARMS(A4),A1
    DO  MEMBEROFSET
    PULL_L  A1
    BNE.S   16$     ;no so dont worry
    MOVE.W  D0,-(SP)
    MOVE.W  #NPOSTMISTRESS,D0
    DO  P.TN
    MOVE.W  (SP)+,D0
    SCN 365
* PM says, "what are you doing with that!" and nicks the charm
    DO  SETOUT      ;not having it back either
    BRA.S   80$
16$
    CMP.W   DracItem(A4),D0 ;is it coming straight back?
    BNE.S   17$
    MOVE.W  D0,STOPRON(A4)
    SCN 13      ;she won't take @ from you
    BRA.S   80$     ;so don't return to the verb
17$:
    CALL    ValidDest   ; Is it a valid destination
    BEQ.S   20$
    SCN 502     ; You can't send this there!
    BRA.S   80$
20$:
    MOVE.W  D0,STOPRON(A4)
    BSET    #6,STOPRON(A4)  ; no trailing space...
    SCN 484     ; Weighs it... etc.
    MOVE.W  D0,PostItem(A4) ; Posting this
    CLR.B   TIMERAFTER1A(A4)    ;Fod the prompt msg.
    DO  FIXCONT     ;incase contained
    MOVE.L  #REL.MASK+NPOSTMISTRESS,6(A0)   ; relate it to her.
    BSET    #6,5(A0)    ; Tell the world that npc was given it.
                ; (so you can ask for it back!)
    CALL    CALCWHT     ;->d3
; Items of weight <=5 cost 1 ferg, the rest cost weight-4 fergs!
    MOVEQ   #1,D1
    SUBQ.W  #4,D3       ; Weight of saddle....(less 2 = cost!)
    BLE.S   30$
    MOVE.W  D3,D1       ; Set up the cost... ( currently = weight?)
30$:
    MOVE.W  D1,Postage(A4)
    CALL    P.SMALLNUM  ; ' five'
    BCLR    #6,STOPRON(A4)  ; clear bit 14 of STOPRON to have trailing sp
    SCN 506     ; 'thargs please!' plus rest of spiel
80$:
    POPSP
90$:    
    RET

ValidDest
    LEA PostTable(PC),A1
10$:
    MOVE.B  (A1)+,D1        ;hi
    LSL.W   #8,D1
    MOVE.B  (A1)+,D1        ;get dest object
    ADDQ.L  #1,A1           ;adjust to next entry
    TEST_W  D1          ;any?
    BEQ.S   15$
    CMP.W   D1,D5           ;this one?
    BNE.S   10$         ;no, continue

    MOVEQ   #0,D1           ;clear d1
    MOVE.B  -1(A1),D1
    MOVE.W  D1,PostDest(A4)     ; Save the 'real' destination
    SETEQ_RET
15$
    SETNE_RET

PostEntry   MACRO           ;item, room of destination
        DC.B    (N\1>>8)&$ff    ;hi byte & low bute
        DC.B    N\1&$ff
        DC.B    RN\2
        ENDM

* if you post x to first noun it goes to second noun (ok room)

PostTable

    PostEntry   PLAYER,COUNTRYLANE2
    PostEntry   XAM,FRONTGARDEN2
    PostEntry   BAKER,BAKERY
    PostEntry   BAKERY,BAKERY
    PostEntry   PUB,PUB
    PostEntry   PUBLICAN,PUB
    PostEntry   POST_DEPOT,POST_DEPOT
    PostEntry   TRAIN_STATION,POST_DEPOT
    PostEntry   STATION_MASTER,POST_DEPOT
    DC.W        0
    ALIGN



*-----------------------------------
TallContainers

    DC.W    NTOP_HAT
    DC.W    NJAR
    DC.W    NDRINK
    DC.W    NMILKBOTTLE 
    DC.W    0

SP.PUTUNDER
    XDEF    SP.PUTUNDER

    XREF    CandleUnderRope,BRKD0QUIET,STOPRON,GET_CONT
    XREF    TIMERAFTER11,FIXCONT,RopeTied,ObjectUnderRope

    CMP.W   #NDUNGEON_ROPE,D5   ;put x under rope?
    BNE 90$         ;no, forget it
    TEST_B  RopeTied(A4)        ;is it tied taught
    BNE.S   01$
    SCN 559         ;no space
    BRA 80$
01$
    TEST_W  ObjectUnderRope(A4) ; is there something already there?
    BEQ.S   02$         ; nope
    MOVE.W  ObjectUnderRope(A4),D0  ; put it into our noun reggie.
    DO  P.BTN           ; But the x
    GETINFO
    DO  ISARE           ; already
    SCN 397
    BRA 80$
02$
    MOVE.W  D0,ObjectUnderRope(A4)  ; put it under rope
    MOVE.W  D0,STOPRON(A4)
    SCN 557         ;put @ under rope
    DO  FIXCONT
    MOVE.L  #RNDUNGEON,6(A0)    ;change location
    CMP.W   #NCANDLE,D0     ;put candle
    BEQ PutCandleUnder
    MOVE.B  10(A0),D1
    AND.B   #7,D1           ;container vol
    BNE.S   DoUnder         ;not a container forget it
80$
    POPSP
90$
    RET

DoUnder:
    LEA TallContainers(PC),A1   ;list of
10$
    MOVE.W  (A1)+,D1        ;fetch one
    BEQ.S   20$         ;nonesuch too small
    CMP.W   D1,D0           ;this one
    BNE.S   10$         ;nope, try again
20$
    DO  GET_CONT        ;contents ->a1
    SF  D6          ;liquid flag
    MOVE.L  A0,A2           ;save data
    MOVE.W  D0,D2           ;and container.
30$
    MOVE.W  (A1)+,D0        ;get one
    BEQ.S   70$         ;not here quit.
    GETINFO
    MOVE.B  1(A0),D3
    LSR.B   #4,D3           ;hardness
    CMP.B   #1,D3           ;a liquid?
    BNE.S   31$
    ST  D6          ; to say a liquid is in there..
31$
    CMP.W   #NCANDLE,D0     ;candle in it?
    BNE.S   30$         ;no, try next
    TEST_W  D1          ;was this container tall?
    BNE.S   40$         ;yes
35$
    MOVE.W  D2,D0           ;for container.
    DO  P.TN
    DO  ISAREN
    SCN 550         ;the @ is not tall enough
36$
    CALL    BRKD0QUIET      ;easy way to empty it
    BCLR    #2,(A2)         ;mend it again.
    BRA.S   80$
40$
* Is there any liquid in the thing?
    TEST_B  D6          ; any liquid?
    BEQ.S   41$         ; no liquid
    MOVE.W  D2,STOPRON(A4)      ; suprisingly, the x topplex over
    SCN 326
    BRA.S   36$
41$
    SCN 551         ;ok fine.
    ST  CandleUnderRope(A4)
    MOVE.W  #NCANDLE,D0     ;is candle lit?
    GETINFO
    TST_LIT
    BEQ.S   80$
    MOVE.B  #3,TIMERAFTER11(A4) ;set up for burning the rope
*   BRA.S   80$
70$
80$
    POPSP
90$
    RET

PutCandleUnder

    SCN 558         ;but if falls over
    POPSP_RET

*------------------------------------------

    XDEF    SP.P.Refresh
    XREF    R.NOUN,CHEAT

SP.P.Refresh


    PUSH_L  D0/D2/A0
    XREF    SP.DescribeCarousel,AtStation,TIMERAFTER8

    IFNE  YCHEAT
    TEST_B  CHEAT(A4)       ; is debug on?
    BNE.S   90$
    ENDC

    TEST_B  TIMERAFTER8(A4)     ;train left yet?
    BEQ 08$
    CALL    AtStation       ;in the station?
    BNE.S   08$
    SCN 707         ;". the train is in the station"
08$
    CMP.W   #NCAROUSEL,D0       ;look (at caorusel room)
    BNE.S   09$
    CALL    SP.DescribeCarousel ;do the doings
09$

    MOVE.W  #NUNICORN2,D0       ;get unicorn
    GETINFO
    CMP.W   #NOUTSIDE,8(A0)     ;in sky?
    BNE.S   10$         ;no
    DO  VALID.NP        ;is unicorn around though?
    BNE.S   10$         ;no, so ignor it
    SCN 253         ;if so give message
10$

    IFNE    0           ;DO BE SERIOUS !!

    MOVE.W  #NCANOE,D0
    DO  VALID.NP        ; around...?
    BNE.S   90$
    GETINFO
    DO  R.NOUN          ; noun number of current room.
    MOVE.W  D0,8(A0)
    BSET    #0,6(A0)        ; make it related.
    TEST_B  CanoeLaunched(A4)   ; is the canoe launched?
    BNE.S   20$         ; try seated...
    SCN 86          ; lies on it/s side
    BRA.S   90$
20$
    TEST_W  SEATED(A4)      ; are we seated.
    BNE.S   90$         ; we don't want to know then
    SCN 611         ; canoe is just in the water

    ENDC    ;0
90$
    PULL_L  D0/D2/A0
    RET

*------------------------------------------

    XDEF    SP.P.Description

SP.P.Description
    
    RET


*----------------------------


    END
