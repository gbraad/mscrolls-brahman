******************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALS
*
*--------------------------------


    include "equates.i"
    include "verbequ.i"
    include "nounequ1.i"
*   include "nounequ2.i"
    include "roomequ.i"
    include "macros.i"
    include "scenmacros.i"
*   include "scoreref.i"
    include "scenequ.i"


    XDEF    SPECIALS

    XDEF    SPECIALS_Before

    XREF    Weather
    XREF    SP.LIGHT,TIMER,FULLSTOP,CHKPAUSE,LINE,GETDATA
    XREF    P.SCN,DEAD,PRTTXT,DO.SCN
    XREF    SCRPNT,ROOM,CLOCK,SCORE_,LIGHT,ADVERB
    XREF    SETOUT,CARRIED,SUBJECT,P.TN,P.TNIN,P.TNSTOP
    XREF    CHEAT,SETPRON
    XREF    OPEN,CLOSE,VERB,PRTNUM,M.ON
    XREF    AquaWorn,AquaTank
    XREF    TAirFlooded,BAirFlooded
    XREF    TIMER_AFTER,NPCINRM,HouseHorror
    XREF    REMASTER

*
* SPECIALS - this is called by screen each move to let the specials suss
*       if they should do spontaneous things (as opposed to specials
*       triggered by verbs)
*



SPECIALS

    IFNE  YCHEAT
    TEST_W  CHEAT(A4)
    BNE.S   90$
    ENDC

                    ;This is where TIMER used to get
    CALL    TIMER_AFTER     ;called. Since it randomly moved
                    ;one day - here's another set.
    CALL    Do_Worms
    CALL    Do_Dracmazza
    CALL    SP.Drown
    CALL    SP.AquaLung
    CALL    Weather
*   CALL    Fire            ;Think we'll ditch this
    CALL    ClosePODoorBehind

* SP.LIGHT should be one of the first things that SPECIALS calls so
* that VALID will work properly in subsequent stuff. Like TIMER, it
* has randomly gone walkies.
* Putting it back MAY screw some of the above calls subtly - so it'll
* have to stay here unless anyone can say for SURE that they don't
* rely on this position.
    CALL    SP.LIGHT        ;suss if the player can see
    CALL    SetUpDrying
    CALL    HouseHorror
    CALL    SP.MOUSE
    CALL    SP.Disappear        ; should objects disappear
                                ; ie chimney && well?

    CALL    SP.TrainWaiting
    
    IFNE  YCHEAT&VAX
    CALL    SP.Inform
    ENDC
90$
    RET

*
* SPECIALS_Before, called before deciding if we've changed room
*
* thus if stuff called here sets SCRPNT room desc happens
*

SPECIALS_Before

    IFNE  YCHEAT
    TEST_W  CHEAT(A4)
    BNE.S   90$
    ENDC
    
    CALL    TIMER           ;do timers - BEFORE SP.LIGHT!!!
    CALL    LINE
*   CALL    SP.SCORE        ;Not if I can help it

    CALL    LINE

    CALL    FodSinkers
    CALL    SinkAllInRoom       ;items sink in wet rooms
90$
    RET


;********************************************************
; Check if Player is drowning...

    XREF    UnderWater,TIMERAFTER1
    XREF    AquaLive

SP.Drown:
    CALL    Aquaok          ;In trouble?
    BEQ.S   90$         ;=> nope
    TEST_B  TIMERAFTER1(A4)     ;Already coping?
    BNE.S   90$         ;=> yep
    ESCAPE  DROWNING        ;saved by the guardian
90$
    RET


SP.AquaLung
    CLR.B   AquaLive(A4)
    MOVE.W  #NAQUALUNG,D0
    GETINFO
    TST_WORNBYPLAYER                ;are you wearing it?
    BEQ.S   90$     ;no
    SUBQ.W  #1,AquaTank(A4) ;running out....
    BPL.S   10$
    CLR.W   AquaTank(A4)    ;So it doesn't get -ve
    BRA.S   90$
10$
    SNE     AquaLive(A4)        ; using tank
    BNE.S   90$
    SCN 152     ; Tanks now empty
90$:
    RET

*
* Aquaok    -   Checks for drowning problems.
*
*   RETURNS:    NE  In trouble
*           EQ  All ok
*

    XDEF    Aquaok

Aquaok
    TEST_B  UnderWater(A4)      ;player submerged?
    BEQ.S   90$         ;no, exit, all ok
    MOVE.W  #NAQUALUNG,D0       ;need the aqualung
    GETINFO
    TST_WORNBYPLAYER                ;player wearing it?
    BEQ.S   80$         ;then drowning
    TEST_W  AquaTank(A4)        ;any air left?
    BNE.S   90$         ;=> ok
80$
    SETNE_RET           ;Flag problems
90$
    SETEQ_RET

*************************************



*
* This copes with putting dracmazza'd objects back on your inventory.
*

    XREF    DracItem,SetValue

Do_Dracmazza

    MOVE.W  DracItem(A4),D0     ;Anything been dracmazza'd?
    BEQ.S   90$         ; No - so just quit
    GETINFO
    DO  CARRIED         ;Player tried to dispose of it?
    BEQ.S   90$         ;=> nope
    CMP.W   #NAQUITANIAN_FERG,D0    ;you can have the coin back even..
    BEQ.S   10$         ;if it is out
    TST_OUT             ;you cant have it back if it is out
    BEQ.S   10$
    CLR.W   DracItem(A4)        ;don't bother me again!
    BRA.S   90$
10$
*   MOVE.W  DracItem(A4),D0     ;Get magic item...
    DO  SETOUT          ;...volumes correct
* I have done a GETINFO above so this ain't nesc - Doug.
*   GETINFO             ; (setout saves/restores A0!)
    CLR.L   6(A0)           ; back on inventory
    DO  P.TN
    SCN 197         ; item returns.
    TEST_B  REMASTER(A4)
    BEQ.S   20$
    BCLR    #7,(A0)         ; remove glowing
    CLR.W   DracItem(A4)    ; remove spell after use
20$    
    CMP.W   #NAQUITANIAN_FERG,D0    ;Ferg coin?
    BNE.S   90$         ;=> nope
    MOVE.W  D0,D5           ;Ferg to D5 (for SetValue)
    MOVEQ   #1,D1           ;Value 1
    CALL    SetValue
90$
    RET

************************************

*
* Do_Worms  -   Since there are probably a million and one
*           ways of ending up with worms on the floor
*           this checks their location every move (Yuk)
*

    XREF    ODT_DIRTMOUND

Do_Worms

    MOVE.W  #NWORMS,D0
    GETINFO             ;NO, I DON'T want an ISIN call
    TST_OUT             ;Alive and kicking?
    BNE.S   90$         ;=> nope, exit fast
    CMP.L   #RNSTEPS1,6(A0)
    BEQ.S   05$
    CMP.W   #NDIRTMOUND,8(A0)   ;In the right place?
    BNE.S   10$         ;=> nope, but maybe they escaped!
    TEST_W  6(A0)       ;incase we got confused with a room
    BEQ.S   10$
05$    
    SCN 570
* The worms burrow into the pile of dirt. After a few moments of frantic
* activity, the dirt mound looks much looser.

    BCLR    #7,ODT_DIRTMOUND(A4)    ;Other description
    DO  SETOUT          ;Fod the worms
    BRA.S   90$

10$
    TST_CONTAINED           ;Contained elsewhere?
    BNE.S   90$         ;=> yep, so let it be
    TEST_W  6(A0)
    BNE.S   90$
    MOVE.W  ROOM(A4),D0
    CMP.W   8(A0),D0
    BNE.S   90$
    SCN 573
* Released from confinement, the worms wriggle away.^

    TEST_B  REMASTER(A4)
    BEQ.S   90$
    DO  SETOUT          ;Fod the worms (not for remaster)

90$
    RET

*---------------------------


**********************************************************************
*
* ScanSpecials
*
*
* Called from SCAN to enable people to give a more interesting message
* than "You find the xyzzy" D0 = noun number of discovered object
* returns EQ => stop scan printing
*         NE => let scan print "You find the xyzzy"
*
**********************************************************************

    XDEF    ScanSpecials
    XREF    FGT_Debris

ScanSpecials

    CMP.W   #NSPARE_TANK,D0     ; so I can fod the FGT_Debris
    BNE.S   10$
    BCLR    #7,FGT_Debris(A4)
    PUSH_L  D0/A0
    MOVE.W  #NSHIPWRECK,D0      ; zap the shipwreck initial state
    GETINFO
    BSET    #1,5(A0)
    PULL_L  D0/A0
10$
    SETNE_RET

**********************************************************************
*
* ContextSpecials - called from CONTEXT before it tries too hard to
*                   suss out what to throw out.
*                    
* Context specials looks/hacks matchbuf, no 'real' params
* but should leave regs as found (bar D4, if successful == 1)
* returns EQ => i resolved it, don't do any more
*
**********************************************************************

    XDEF    ContextSpecials
    XREF    VERB,MATCHBUF,VerbInList,Denotation_COINS


ContextSpecials
*
* First do the doings about matches

    MOVE.L  A1,-(A7)
    LEA LightTypeVerbs.Tbl(PC),A1
    CALL    VerbInList          ;is our verb one of these?
    PULL_L  A1              ;restore
    BEQ SP.Context.Light        ;gfi

** with buy x with coin
*** if you are referring to any one of the coins it will
* look it up and default to the one found on your inventory (or ones)!
    CMP.W   #VNBUY,VERB(A4)     ; Is it our verb?
    BNE.S   99$
    PUSH_L  D0-D3/D5-D7/A0-A6
    LEA MATCHBUF(A4),A1     ; where they are all stored...
    MOVE.L  A1,A2           ; secondry pointer.
    CLR.W   D6
10$
    MOVE.L  (A1)+,D0        ; Long because of section no.
    BMI.S   60$
20$
    ADDQ.W  #1,D0           ; for carried's sake
    MOVE.L  A1,-(A7)
    LEA Denotation_COINS(A4),A1
    DO  MEMBEROFSET     ;one of the coins?
    PULL_L  A1
    BNE.S   10$         ;none.
30$
* okay we have found one
* so is it carried?

    DO  CARRIED         ; returns EQ if it is.
    BNE.S   10$         ; not carried not to worry
    SUBQ.W  #1,D0
    MOVE.L  D0,(A2)+        ; secondary pointer.
    ADDQ.W  #1,D6           ; our counter.
    BRA.S   10$         ; and around again....
60$
* okay we got here. Do we have anything interesting?
    TEST_W  D6          ; EQ => boring
    BEQ.S   90$         ; go away.
    MOVE.L  D0,(A2)         ; we KNOW at this point it contains -1
    MOVE.W  D6,D4
    PULL_L  D0-D3/D5-D7/A0-A6
    SETEQ_RET
90$
    PULL_L  D0-D3/D5-D7/A0-A6
99$
    SETNE_RET


************************************



********
*
* ProcessSpecials - called from PROCESS immediately before a verb routine
*
* primary use is to spot random references to given nouns. This may be
* triggered here rather than in ALL the verb routines/specials
*
* Returns NE if something is up
*
* WARNING: 
*
*    Only verbs going via process (not SINGV/say types) are hit.
*
*    THIS SHOULD NOT BE ABUSED - side effects intoduced here could
*                                be nasty!
*
* format of table:
*
*   DC.W    Indirect obj,Direct obj [ +HI ]
*   DC.L    code
*
* Will call code if EITHER D.O. OR I.O. match
*
* If dobj has hi-bit set then 'code' is taken as a scen msg#
* and for all verbs not in { look, examine }
* msg# is printed and attempt at that command is aborted.

    XDEF    ProcessSpecials


ProcessSpecials

    DO  VALID.NP    ;is item valid?
    EOR #4,CCR      ;invert return value
    BEQ.S   99$     ;no quit straight away with EQ

    PUSH_L  D1-D2/A1/A2
    CALL    AllPrSpec   ;non noun specific ones, ret EQ => nothing done
    BNE.S   90$

    LEA PS_TAB(PC),A1
10$
    MOVE.L  (A1)+,D1
    BEQ.S   90$
    MOVE.L  (A1)+,A2
    CMP.W   D0,D1
    BEQ.S   20$
    SWAP    D1
    CMP.W   D5,D1
    BEQ.S   20$
    SWAP    D1      ;set back again.
    BCLR    #15,D1      ;any hi bit?
    CMP.W   D1,D0       ;special case?
    BNE.S   10$     ;no, try another.
    MOVE.W  VERB(A4),D1 ;fetch verb.
    BEQ.S   90$     ;verb is look, ok, exit with EQ.
    CMP.W   #1,D1       ;=examine?
    BEQ.S   90$     ;same story.
    MOVE.W  A2,D1       ;get msg# (not an address after all)
    DO  P.SCN       ;print moan message.
    SETNE           ;flag as fail
    BRA.S   90$     ;and exit
20$
    MOVE.W  VERB(A4),D1 ;so SP.xyzzy doesn't need to do VERB(A4) lots
    JSR 0(A4,A2.L)  ;should return EQ => do verb
90$
    PULL_L  D1-D2/A1/A2
99$
    RET

    XREF    WearCheck
    XREF    BoozedUp,RND.D1,STOPRON
*   XREF    SP.MEDDLE
    XREF    VerbInList

AllPrSpec

    PUSH_L  D1-D7/A1-A6

    MOVE.W  D0,STOPRON(A4)  ;This line is VERY dangerous!!!!!!!!!

    MOVE.B  BoozedUp(A4),D2 ; Get booze level
    CMP.B   #2,D2       ; Check for more than tipsy...
    BLE.S   90$     ; not pissed, so no trouble.
    MOVEQ   #4,D1
    DO  RND.D1      ; random no  0-3
    SUBQ.B  #2,D2
    CMP.B   D1,D2       ; See if too pissed?
    BLE.S   90$
    MOVEQ   #5,D1       ; Generate message number
    DO  RND.D1
    EXT.W   D1
    OR.W    #$8000,STOPRON(A4)  ;to kill initial space
    ADD.W   #414,D1
    DO  P.SCN
80$
    PULL_L  D1-D7/A1-A6
    SETNE_RET
90$
    PULL_L  D1-D7/A1-A6
    SETEQ_RET

*------------------------------------------



PS_TAB

* Note that first word is INDIRECT object, second is DIRECT object!
*
*   DC.W    NHOORAY,NHOORAY
*   DC.L    SP.HOORAY

    DC.W    NBELL,-1
    DC.L    SP.TweakBell
    DC.W    NNUKE_DEVICE,NNUKE_DEVICE
    DC.L    SP.NUKE

******** These catch references to our intrinsic spells..... *******
    DC.W    NINT_FLYZZ,NINT_FLYZZ
    DC.L    SP.NoWay
    DC.W    NINT_AQUA,NINT_AQUA
    DC.L    SP.NoWay
    DC.W    NINT_SOL,NINT_SOL
    DC.L    SP.NoWay
    DC.W    NINT_DRACMAZZA,NINT_DRACMAZZA
    DC.L    SP.NoWay
    DC.W    NINT_FREEZE,NINT_FREEZE
    DC.L    SP.NoWay
********                             *******

    DC.W    NUNICORN2,NUNICORN2
    DC.L    SP.UNICORN2
    DC.W    NCHEESE,NCHEESE
    DC.L    SP.ManipulateCheese ; when the cheese is on the trap
    DC.W    -1,NCHANDELIER+HI
    DC.L    604         ;message (not an address)
    DC.W    NMIXEDOIL,NMIXEDOIL
    DC.L    SP.OtherOil

    DC.L    0           ;Terminator


;********************************************



*--------------------------------------

    XREF    TIMERAFTERE

SP.TweakBell

    TEST_B  TIMERAFTERE(A4)     ; bending down?
    BNE.S   90$
    NVALID  POSTMISTRESS        ;is she around?
    BNE.S   90$         ; no so no message
    SCN 512         ; Postmistress won't let you
    SETNE_RET
90$:
    SETEQ_RET
*
* only allow look and examine with the nuke
* else message about dangerous
*

    XREF    FlagTrapSet

SP.ManipulateCheese         ; when the cheese is on the trap

    TEST_B  FlagTrapSet(A4)     ; if flag isn't set anything can happen
    BEQ.S   90$
                    ; it shouldn't be manipulated
    DO  IsVerbLookOrExamine ; that is okay
    BEQ.S   90$
    SCN 234         ; I am not going anywhere near that trap
    SETNE
90$
    RET

SP.OtherOil
    DO  IsVerbLookOrExamine
    BEQ.S   90$
    SCN 381
    SETNE
90$
    RET
SP.NUKE

    XREF    IsVerbLookOrExamine

    DO  IsVerbLookOrExamine
    BEQ.S   90$         ;yes
    CMP.W   #VNLISTEN,VERB(A4)  ;hoho
    BEQ.S   90$

      IFD   VNMAKELOCAL

    CMP.W   #VNMAKELOCAL,VERB(A4)
    BEQ.S   90$
    CMP.W   #VNFIND,VERB(A4)
    BEQ.S   90$
      
      ENDC

    SCN 292         ;leave it alone!
    SETNE               ;don't do verb
90$
    RET



*--------------------------------

*
*
* SP.NoWay  Should catch ALL references (you'll be lucky!) to
*       the spell names which are intrinsic.
*
*


    XREF    WHOOPS

SP.NoWay
    CMP.W   #VNCAST,VERB(A4)    ;ONLY cast allowed!
    BEQ.S   90$
    ST  WHOOPS(A4)      ;Kill multiples
    DO  DBS
    SETNE
90$
    RET

*-------------------------------

********
*
* DecodeSpecials - called from DECODE after INITCOM so we can fool
*           the parser about some of the things it thinks about
*
*   Prime use is to fod NearNPC so gatekeeper/miller
*   may be shouted at
*
*   THIS SHOULD NOT BE ABUSED - side effects intoduced here could
*                        be really nasty!!!!!!!!!!
*

    XDEF    DecodeSpecials
    XREF    DO.SING,SP.NothingDoing,FlagNothingDoing,P.LIST3

DecodeSpecials

    TEST_B  FlagNothingDoing(A4)    ;stop them doing anything but
    BEQ.S   90$         ;save/load/brief etc?

* When we get top here we are REALLY commited - no going back!
    
    POPSP               ;back to DECODE's stack level

    CLR.W   D3          ;kill noun flag (well decode does it!)
    DO  NOERR
    DO  GETADJPH        ;to D5=word# D6=noun#
    DO  YESERR

    MOVE.W  #-1,VERB(A4)        ;in case we don't find a verb
    CMP.W   #SECT.CONT,D6       ;is it control (ie save/load etc)
    BEQ DO.SING         ;yes => off to debug/control bits
    CMP.W   #SECT.DIR,D6
    BNE.S   10$
    MOVEQ   #VNRUN,D5       ;fake up that we hit run verb
    MOVEQ   #SECT.VERB,D6
10$
    CMP.W   #SECT.VERB,D6       ;did we hit a verb?
    BNE.S   20$
    MOVE.W  D5,VERB(A4)
20$
    CALL    SP.NothingDoing     ;go think about the event
    POPSP               ;back to SCREEN's stack level
    DOGO    SCREEN          ;to avoid process...

90$
    RET

*----------------------------------------

SinkAllInRoom

    XREF    ROOM,D0SINKS,LIST3,SETPL,DNOUNBUF
    XREF    EntryLongLook,NOSINKMSG,IsRoomWet
    XREF    COUNTLIST

* if the room is wet- all sinks

    PUSH_L  D0-D2/A0-A3
    DO  IsRoomWet       ;is this a wet room?
    BEQ 90$         ;no, exit
    MOVE.W  ROOM(A4),D2     ;target
    DO  GETRELN         ;all associated wi room ->a1
    MOVE.L  A1,A2           ;pointer in a2
    MOVE.W  #0,-(A7)        ;terminate sinker list
10$
    MOVE.W  (A2)+,D0        ;get one
    BEQ.S   20$         ;done all, now print them
    GETINFO
    TEST_W  6(A0)           ;any other relation but in room?
    BNE.S   10$         ;doesnt count
    TST_PSEUDO              ;pseudo?
    BNE.S   10$         ;no pseudos!
    MOVE.W  D0,-(A7)        ;push item to sink
    BRA.S   10$         ;next
20$
    MOVE.L  A7,A0           ;save buffer start
21$
    MOVE.W  (A0)+,D0        ;fetch each one again
    BEQ.S   30$         ;done them
    CALL    D0SINKS         ;does it sink?
    BEQ.S   21$         ;yes, leave it
    MOVE.W  #323,D1         ;floating message
    SWAP    D1
    MOVE.W  -2(A0),D1       ;and the noun
    DO  EntryLongLook
    BSET    #7,-2(A0)       ;flag as void
    CALL    FloatAway
    BRA.S   21$         ;next
30$
    MOVE.L  A7,A0           ;back again
    LEA DNOUNBUF(A4),A1     ;start of list
31$
    MOVE.W  (A0)+,(A1)+     ;move back onto buffer
    BNE.S   31$         ;not end, continue
    LEA DNOUNBUF(A4),A1     ;start of list
    DO  COUNTLIST       ;->d4
    TEST_W  D4
    BEQ.S   40$         ;quit
    MOVE.L  A1,A0
    TEST_B  NOSINKMSG(A4)       ;supress sink message?
    BNE.S   40$
    DO  P.LIST3         ;the list
    CALL    SETPL
    SCN 764         ;'sink@.^'
40$
    MOVE.W  (A7)+,D0        ;get item to sink
    BEQ.S   90$         ;done
    BMI.S   40$         ;not if flagged out
    CALL    SINKD0          ;sink if possible
    BRA.S   40$         ;next
90$
    CLR.B   NOSINKMSG(A4)       ;ok, from now on
    PULL_L  D0-D2/A0-A3
    RET

*----------------------------------------

FodSinkers
    XREF    LongLookTable

    PUSH_L  D0/A0
01$
    LEA LongLookTable(A4),A0
10$
    MOVE.L  (A0)+,D0        ;get one
    BEQ.S   90$         ;done
    SWAP    D0
    CMP.W   #323,D0         ;floating?
    BNE.S   10$         ;no, next
20$
    MOVE.L  (A0),-(A0)      ;move rest up
    BEQ.S   01$         ;try again
    ADDQ.L  #8,A0           ;skip 2
    BRA.S   20$         ;next
90$
    PULL_L  D0/A0
    RET

*--------------------------------------------

    XREF    CALCNET,ROOM,SETOUT,R.NOUN2
;   XDEF    SINKD0

SINKD0

* actually sink d0

    PUSH_L  D0/D2/a0
    DO  CALCNET         ;network ->a0
    CLR.W   D2          ;uses word wise
    MOVE.B  DIR_D(A0),D2        ;get down room
    BMI.S   50$         ;stays here
    BEQ.S   50$         ;stays here too
    CMP.B   ROOM+1(A4),D2       ;wrap around?
    BNE.S   10$         ;goes there
    DO  SETOUT          ;fod it
    BRA.S   90$         ;exit
50$
    MOVE.W  ROOM(A4),D2     ;room to go in
10$
    GETINFO             ;data
    DO  R.NOUN2         ;room ->d0
    MOVE.W  #$100,6(A0)     ;related
    MOVE.W  D0,8(A0)        ;in room
90$
    PULL_L  D0/d2/A0
    RET

*-------------------------
*
*
* ClosePODoorBehind Called each move via SPECIALS
*           Closes the Post Office door behind you
*           if just entered or left Post Office.
*
*

    XREF    ShutTheDoor,BellMuffled,IntroPMistress,SETIN
    XREF    HandlePos
    XDEF    ClosePODoorBehind

ClosePODoorBehind

    BCLR    #7,ShutTheDoor(A4)  ;Setmode or timer told us to do it?
    BEQ 90$         ;Nope, exit
    MOVE.W  #NPODOOR,D0
    GETINFO
    TST_CLOSED          ;Already closed?
    BNE.S   10$
    DO  CloseDoor       ; Make door closed
    DO  VALID.NP        ; Can you hear anything??
    BNE 90$             ; No, so no messages!
    CALL    BellMuffled     ;Bell been silenced?
    BEQ 80$         ;=> yep
    SCN 175
* The Post Office door closes behind you, ringing the bell.^
10$
    BCLR    #7,IntroPMistress(A4)   ;Need to intro Postmistress?
    BEQ 90$         ;Nope
    MOVE.W  #NPOSTMISTRESS,D0
    GETINFO
    TST_OUT             ; already here ?
    BEQ 90$         ; yes so do nothing
    CMP.W   #RNPOSTOFFICE,ROOM(A4)  ; are we in there?
    BNE 90$         ; nope don't print message
    SCN 176
* A postmistress enters from a rear door and takes her position behind
* the counter.

; has that smumbag player been mucking about with the safe
    MOVE.W  #NSAFE,D0
    GETINFO
    TST_OPEN            ; safe open?
    BEQ.S   30$         ; nope so ok (or at least LOOKS ok)
    MOVE.W  #NCHARM1,D0     ; charm in safe?
    GETINFO
    CMP.W   #NSAFE,8(A0)
    BNE.S   20$
    SCN 361
* says "whats that safe doing open?" and closes, locks it.
    CLR.B   HandlePos(A4)       ; all handles UP
    MOVE.W  #NSAFE,D0
    GETINFO
    AND.B   #$CF,(A0)       ; clear closed and open bits
    OR.B    #(1<<6),(A0)        ; set locked bit
    BRA.S   30$         ; normal PM stuff
20$
    SCN 362
* says "Some thief has stolen my stuff!! Right! That does it! I'm closing!"
    MOVEQ   #RNFAIRGROUND,D1    ; chuck out player
    DO  MovePlayer
    MOVE.W  #NPODOOR,D0     ; make PO door locked
    GETINFO
    BSET    #6,(A0)         ; now locked
    BCLR    #5,(A0)         ; no longer closed
    BRA.S   90$
30$
    MOVE.W  #NPOSTMISTRESS,D0   ; Postmistress now IN
    MOVE.W  D0,NPCINRM(A4)      ; set 'in room'
    DO  SETIN
    CALL    SETPRON
    MOVE.W  D0,D1
    MOVE.W  #NSAFE,D0       ; safe related to her now
    GETINFO
    MOVE.W  D1,8(A0)
    MOVE.W  #NSCALES,D0     ; ditto scales
    GETINFO
    MOVE.W  D1,8(A0)
    
    BRA.S   90$
80$
    SCN 173
* The Post Office door closes behind you.

90$
    RET



*------------------
    XREF    UnderWater,TIMERB,IsRoomWet

SetUpDrying

    
    TEST_B  UnderWater(A4)      ;player submerged?
    BNE.S   10$         ;yes
    DO  IsRoomWet
    BNE.S   10$         ;if set then set timer
    TEST_B  TIMERB(A4)      ;if dry then timer in progress?
    BNE.S   90$         ;yes, leave it
                    ;otherwise set it up
10$
    MOVE.B  #10,TIMERB(A4)      ;set it up for drying
90$
    RET

*-------------------------------------
    XDEF    SetBuskFlags

    XREF    HatForCollecting,CoinContainer,CoinUsed

SetBuskFlags

* evaluate and set the busking flags

    CLR.B   HatForCollecting(A4)    ;false to begin
    CLR.W   CoinContainer(A4)
    MOVE.W  #NTOP_HAT,D0        ;only hat in game   
    GETINFO
    TEST_W  6(A0)           ;in a room?
    BNE.S   10$         ;next flag
    MOVE.W  ROOM(A4),D0     ;current room
    CMP.W   8(A0),D0        ;this room?
    BNE.S   10$         ;no, flag is false then
    ST  HatForCollecting(A4)    ;is true then
10$
    MOVE.W  #NAQUITANIAN_FERG,D0    ;where is the coin
    GETINFO
    TST_CONTAINED           ;contained?
    BEQ.S   15$         ;no, tough
* EDITED BY AJS
    TST_OUT             ; is it out
    BEQ.S   17$         ; nope we have one...
15$    
    MOVE.W  #NTWOFERG,D0        ; the other coin should work
    GETINFO
    TST_CONTAINED           ;contained?
    BEQ.S   20$         ;no, tough
    TST_OUT             ; is it out?
    BNE.S   20$         ; yes. we don't want it.
17$
    MOVE.W  D0,CoinUsed(A4)     ; the coin that is used.
* END of AJS edits.
    MOVE.W  8(A0),D0        ;get container
    GETINFO             ;on container.
    TEST_W  6(A0)           ;container simply in a room?
    BNE.S   20$         ;no too bad
    MOVE.W  8(A0),D1        ;get room location
    CMP.W   ROOM(A4),D1     ;this room?
    BNE.S   20$         ;somewhere else!
    MOVE.W  D0,CoinContainer(A4)    ;otherwise here it is.
20$
    RET

*---------------------------------------

FloatAwayRooms

    DC.B    RNINF_UNDERWATER
    DC.B    RNUNDERWATER
    DC.B    RNUNDERWATER2
    DC.B    RNOVER_HATCH
    DC.B    0
    ALIGN

FloatAway

* items float to the surface from the inf_underwater rooms
* Entry;
*   item in d0

    MOVE.L  A1,-(A7)
    LEA FloatAwayRooms(PC),A1
    CALL    LookUpRoomInTable       ;valid room?
    BNE.S   90$             ;no, quit
    MOVE.L  A0,-(A7)
    GETINFO
    MOVE.L  #RNONLAKE,6(A0)         ;float up to the lake
    MOVE.L  (A7)+,A0            ;restore
90$
    MOVE.L  (A7)+,A1
    RET

    
***************************************************
*
* SP.MOUSE
*
* it seems that a mouse should scurry out of it's hole
* in the messy kitchen (RNKITCHEN2) from time to time
*
    XREF    FlagMouseDead

SP.MOUSE
    
    CMP.W   #RNKITCHEN2,ROOM(A4)
    BNE.S   90$
    TEST_B  FlagMouseDead(A4)   ;dead mice don't walk
    BNE.S   90$

    MOVEQ   #5,D1           ;1 move in 5 the mouse looks out!
    DO  RND.D1          ;returns 0-(n-1)
    TEST_B  D1
    BNE.S   90$
    SCN 455         ;the mouse looks out msg
90$
    RET

*------------------------------------------

    XREF    ROOM
    XDEF    LookUpRoomInTable

LookUpRoomInTable

* looks ROOM up in A1

    MOVE.L  D1,-(A7)
10$
    MOVE.B  (A1)+,D1
    BEQ.S   80$     ;exit
    CMP.B   ROOM+1(A4),D1
    BNE.S   10$ 
    BRA.S   90$
80$     
    SETNE
90$
    PULL_L  D1
    RET

*------------------------------------------

    XREF    IsVerbLookOrExamine

SP.UNICORN2

* Can you reach the unicorn

    PUSH_L  D0/A0
    MOVE.W  #NUNICORN2,D0
    DO  IsVerbLookOrExamine
    BEQ.S   90$         ;ok
    GETINFO
    CMP.W   #NOUTSIDE,8(A0)     ;it it in the sky?
    BNE.S   10$         ;no, then ok (wil be local)
    SCN 599         ;do nasty message (too far away)
    SETNE
    BRA.S   90$         ;and exit.
10$
    SETEQ
90$
    PULL_L  D0/A0
    RET

*------------------------------------------


    IFNE    YCHEAT&VAX

*
* Inform player if score changes    - only on vax coz no top line
*


    XREF    OldScore

SP.Inform

    MOVE.W  SCORE_(A4),D0
    CMP.W   OldScore(A4),D0
    BEQ.S   90$
    PRINT   <'^Your score just went '>
    SUB.W   OldScore(A4),D0
    BLT.S   10$
    PRINT   <'up by '>
    BRA.S   20$
10$
    NEG.W   D0
    PRINT   <'down by '>
20$
    MOVE.W  D0,D6       
    CALL    PRTNUM
    PRINT   <' points.^'>
    MOVE.W  SCORE_(A4),OldScore(A4)
90$
    RET

    ENDC

*-----------------------------------------------

;Call this when the player has been rowdy in the Pub
    XDEF    SP.RowdyInPub,SP.SnatchGlass
    XREF    Hooligan,Waiting

SP.RowdyInPub
    SCN 788     ;I'll have no rowdyism in My pub!
    TEST_B  REMASTER(A4)
    BNE.S   10$
    ST  Hooligan(A4)    ;now a known hooligan
10$    
    CALL_S  SP.SnatchGlass  ;get glass if he is carrying it
    CLR.W   Waiting(A4)
    MOVEQ   #RNFAIRGROUND,D1    ; Move room...
    DOGO    MovePlayer

SP.SnatchGlass
    PUSH_L  D0/A0
    MOVE.W  #NDRINK,D0      ; is player carrying glass
    DO  CARRIED
    BNE.S   10$         ; nope
    GETINFO
    MOVE.L  #REL.MASK+NPUBLICAN,6(A0) ; move it to publican
    MOVE.W  #NBEER,D0
    DO  SETOUT          ; so BUY won't fall over
    MOVE.W  #NPUBLICAN,D0
    DO  P.TN
    SCN 795         ; I'll have that!
10$
    PULL_L  D0/A0
    RET

*-----------------------------------------------
    XREF    GLOBAL.F,FlagHearth
SP.Disappear
* this will cope with any rooms where if an object gets left lying
* around it will disappear from view.

    CLR.W   D7          ; a random flag we are using.
    CMP.W   #RNBOTTOM_WELL,ROOM(A4) ; is it the well?
    BEQ.S   10$
    CMP.W   #RNCHIMNEY,ROOM(A4)
    BNE.S   99$         ; none of them we go away.
    ST  D7
10$
    ST  GLOBAL.F(A4)        ; in case we are in the dark.
    DO  GETRELN         ; returns a list pointed to by a1.
    SF  GLOBAL.F(A4)        ; no more GLOBAL.F stuff
20$
    MOVE.W  (A1)+,D0        
    BMI.S   20$         ; hi bit set means n/a
    BEQ.S   99$         ; ZERO terminated
    GETINFO
    BTST    #0,5(A0)        ; is it a pseudo?
    BNE.S   20$         ; yup - next one please
    TEST_W  12(A0)          ; is it tied?
    BNE.S   20$         ; YUP go away
    TEST_B  6(A0)           ; must be just lying around.
    BNE.S   20$         ; get the next one
    TEST_B  D7
    BNE.S   30$
    DO  SETOUT          ; and now set it out.
25$
    TEST_B  LIGHT(A4)       ; if there is no light we print nothing
    BEQ.S   20$
    DO  P.TN            ; print the xyzzy
    SCN 792         ; disappears from view.
    BRA.S   20$         ; and continue....
30$
* okay it is the chimney.
    MOVE.L  #REL.MASK+NHEARTH,6(A0) ; put it in the hearth.
    ST  FlagHearth(A4)
    BRA.S   25$
99$
    RET             

*------------------------------------------

SP.TrainWaiting

    XREF    TIMERAFTER8

    ;; for remaster we keep the train waiting timer waiting
    ;; unless it's close to trigger
    ;; this prevents the train from leaving the station until ready

    TEST_B  REMASTER(A4)
    BEQ.S   90$
    CMP.B   #20,TIMERAFTER8(A4)
    BLT.S   90$                 ; timer < 20, continue to zero
    MOVE.B  #30,TIMERAFTER8(A4) ; otherwise reset to 30
90$    
    RET

*------------------------------------------

LightTypeVerbs.Tbl

    SVERB   LIGHT
    SVERB   STRIKE
    SVERB   IGNITE
    SVERB   BURN
    DC.B    -1
    ALIGN

*------------------------------------------

    XREF    TestSpent,MATCHBUF,Denotation_MATCHES
    XREF    NOUNBUF

SP.Context.Light

* pre- suppose matchbuf contains nouns only (hmm)

* if one of the matches in the list is sensible for lighting
* we must find it.
*
* Must check for not-spent match (ie live) and not contained.

* if matchbuf contains at least one, non-spent (also non-lit) match
* then change buffer to contain only those matches.

    PUSH_L  D0-D2/A0-A2
    LEA MATCHBUF(A4),A2
    LEA Denotation_MATCHES(A4),A1
    CLR.W   D2          ;counter
    MOVE.L  #-1,-(A7)   ;terminator in stack
10$
    MOVE.L  (A2)+,D0    ;get entry
    BMI.S   20$         ;end
    ADDQ.W  #1,D0       ;adjust to noun#
    DO  MEMBEROFSET     ;a match?
    BNE.S   10$         ;nope.
    GETINFO             ;on our match
    TST_CONTAINED       ;cant light contained matches
    BNE.S   10$         ;tough, lose this one

    TEST_W  NOUNBUF(A4)
    BEQ.S   15$         ; direct object

    ;; indirect object
    TST_LIT
    BEQ.S   10$         ; must be lit
    BRA.S   18$         ; if lit, add to list
15$    
    CALL    TestSpent   ;a spent match?
    BEQ.S   10$         ;yep.
18$
    SUBQ.W  #1,D0       ;adjust back
    MOVE.L  D0,-(A7)    ;a LIVE match (oh no)
    ADDQ.W  #1,D2       ;inc counter.
    BRA.S   10$         ;next
20$
    CMP.W   #1,D2       ;how  many live ones?
    BLT.S   50$         ;none, ok quit
    LEA MATCHBUF(A4),A2 ;at start.
30$
    MOVE.L  (A7)+,(A2)+ ;move ultimate buffer
    BPL.S   30$         ;until done.
    MOVE.W  D2,D4       ;set to 1
    BRA.S   90$         ;quit.
50$
    TEST_L  (A7)+       ;junk stack buffer
    BPL.S   50$         ;until end.
90$
    SETNE               ;set as fail for both cases
    PULL_L  D0-D2/A0-A2
    RET
    
*------------------------------------------
    

    END
