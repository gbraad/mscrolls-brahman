******************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

* JINXTER

    include "equates.i"
    include "roomequ.i"
    include "verbequ.i"
    include "nounequ1.i"
*   include "nounequ2.i"
    include "macros.i"
    include "scenmacros.i"
    include "scoreref.i"

 
    XREF    ROOM,RIDING,SEATED,P.SUB,W.GETOFF,CARRIED
    XREF    W.ROPED,WHRIDING,CHEAT,USER.DB,NEXITFLG,SETOUT
    XREF    GETDATA,SCRPNT,PRTTXT,VERB,DO.SCN,FULLSTOP,DEAD
    XREF    SEATED_RELATOR,RND.D1

    XREF    TIMER2,TIMER4,TIMER5,TIMER6,TIMER7,TIMER8,TIMER9
    XREF    TIMERB,TIMERF,TIMER10
    XREF    REMASTER

        XREF    KnockPos
    XREF    DoorCreaked,DwarfDone
*   XREF    RoomToBits
    XREF    ISIN,SETIN
    XDEF    SETMODE     ;,NEWROOM,NEWROOM2

    XREF    BeginBusking,DO.SCORE

*--------------------------------
*
* SETMODE - HANDLES SPECIALS
*        THAT CONTROL MOVEMENT
*        BETWEEN LOCATIONS 
*
* NOTE   ROOM TO ENTER IS IN D1.B
*
*     BYTESWISE
*    +++++++++++
*
*--------------------------------
  
  
    XREF    FrozenCloud
OnCloud
* if you are on a cloud (over the fairground...) and try to go in any
*direction; (other than down) it will suggest that you consider the fact that
*you are halfway up the sky...
    CMP.W   #NNICECLOUD,SEATED(A4)  ; are we on the cloud...?
    BNE.S   99$
    CALL    Plummet
99$
    RET

SETMODE
 
    LEA     ROOM(A4),A5     ;SAVE MEM
    AND.W   #$FF,D1         ;URK!! - USUALLY FFXX - XX = ROOM

    CMP.W   #RNWATERFALL,ROOM(A4) ; except over the waterfall
    BEQ.S   02$
    
    CALL_S  OnCloud         ; on the cloud..?
    TEST_W  RIDING(A4)      ;ON A VEHICLE?
    BNE.S   01$         ;OK - DONT GET UP THEN
    TEST_W  SEATED(A4)      ;SITTING ON SOMETHING?
    BEQ.S   01$         ;NOPE
    DO      P.SUB           ;'YOU '
    MSG     GETOFF
    CLR.W   SEATED(A4)
    CLR.W   SEATED_RELATOR(A4)
01$
    CALL    WHRIDING        ;STOPPED COS OF VEHICLE
    TEST_W  D7          ;0= NOT STOPPED
    BEQ.S   02$
    RET
02$
    
    IFNE     YCHEAT
    TEST_W  CHEAT(A4)       ;IF CHEATING THEN FORGET SPECIALS
    BNE     NEWROOM2        ;!!! BEWARE - THIS LETS U WALK OUT OF A
    ENDC                ;ROOM WHILEST TIED TO CONTENTS OF IT!! 
    
 
    TEST_B  USER.DB(A4)     ;IF IN 'USER DEBUG' THEN FORGET SPECIALS
    BNE     NEWROOM         ;BUT TAKE CARE OF 'TIED TO ROOM' BITS
    

*   CALL    RoomToBits      ;Convert room # to ptr + index
*   BTST.L  D2,D7           ;Is the target room on fire?
*   BEQ.S   10$         ;=> Nope
*   PRINT   <'Sorry, that room is temporarily out of order '>
*   PRINT   <'because a raging fire is taking place in there!'>
*   RET
*10$

    CALL    DoRoomSpecials      ;SPECIALS GO HERE!!!!

; We can only get to here if RRENTRYs allow it
 
NEWROOM
  
    TEST_W  NEXITFLG(A4)        ;CAN'T GO TO NEXT ROOM IF TIED TO THIS
    BEQ.S   NEWROOM2

    MSGGO   ROPED           ;ROOM'S CONTENTS

; When we get this far, we're GUARANTEED to move to another room
NEWROOM2

    CALL    AllRoomAfter     
  
    MOVE.W  SEATED(A4),D0       ;can't have got here if not a vehicle
    BEQ.S   10$         ;no vehicle
    GETINFO             ;ok, we have a vehicle so lets move it
    CLR.L   6(A0) 
    MOVE.B  D1,9(A0) 
10$
    MOVE.B  D1,1(A5)        ;NEW ROOM #

*
* this next line is why its called 'SETMODE'
*
80$
    MOVE.W  #1,SCRPNT(A4)       ;LET SCREEN KNOW TO DO DESC & CONTENTS
90$
    RET  

*-------------------------------------------------------------------------
      

* RRENTRY macro for the RoomToRoom table

RRENTRY MACRO   * room1,room2,code

    IFC '\1','any'
    DC.B    0
    ENDC
    IFNC    '\1','any'
    DC.B    RN\1
    ENDC

    IFC '\2','any'
    DC.B    0
    ENDC
    IFNC    '\2','any'
    DC.B    RN\2
    ENDC

    DC.W    \3-*

    ENDM
  
*--------------------------------
*
* DoRoomSpecials - this is setmode done in a sensible(ish) fashion!
*
* On entry A5 points at ROOM (current room no, a WORD)
*       D1 (.B) is the target room #
*
* On exit D1 is new room#
*
* This bit of code uses a table to match up target combinations of 
* source and destination (0 in either case => don't care) and then calls
* some code to deal with it
*

DoRoomSpecials

    CALL    AllRoomSpecials     ;any move does this

    LEA RoomToRoom(PC),A2   ;pointer to our table
10$
    MOVE.W  (A5),D0         ;get current room# (byte really)
20$
    MOVE.B  (A2),D2         ;source room
    BEQ.S   30$         ;EQ=> wildcard
    CMP.B   D2,D0           ;right room?
    BNE.S   50$
30$
    MOVE.B  1(A2),D2        ;destination room
    BEQ.S   40$         ;wild card?
    CMP.B   D2,D1
    BNE.S   50$
40$
    MOVE.W  2(A2),D0        ;wow! a complete match!
    BCLR    #15,D0          ;nobble 'is safe' flag
    

* now call the code concerned
*
* this may turn into a JSR one day...
* Call code with dest room # in D2.B

    JMP 2(A2,D0.W)      ;THIS DEPENDS ON DC.W being PC rel!

* so should be thought of as falling through to here
* SO DON'T STOMP ON A2 !!!!

    ADDQ.L  #4,A2
    BRA.S   10$         ;reload D0+loop

50$
    CMP.B   #-1,D2          ;-1 terminator
    BEQ.S   60$
    ADDQ.L  #4,A2
    BRA.S   20$         ;loop
60$:
    RET             ;bye..

*--------------------------------
*
* RRSAFE - room to room, but safe to makelocal via

RRSAFE  MACRO   * room1,room2,code

    IFC '\1','any'
    DC.B    0
    ENDC
    IFNC    '\1','any'
    DC.B    RN\1
    ENDC

    IFC '\2','any'
    DC.B    0
    ENDC
    IFNC    '\2','any'
    DC.B    RN\2
    ENDC

    DC.W    \3-*

    ENDM
  
*
* SafeSetmode
*
* this susses if a given room -> room move is likely (ish!) to be safe 
* (esp for makelocal)
*
* On entry: D0 = room to leave
*           D1 = room to enter
*
* returns EQ => is safe
* else MAY do nasties
*
       IFNE 0       ; NO makelocal

        

    XDEF    SafeSetmode

SafeSetmode

    PUSH_L  D2/A2
    LEA RoomToRoom(PC),A2   ;pointer to our table
10$
    MOVE.B  (A2),D2         ;source room
    BEQ.S   20$         ;EQ=> wildcard
    CMP.B   D2,D0           ;right room?
    BNE.S   40$
20$
    MOVE.B  1(A2),D2        ;destination room
    BEQ.S   30$         ;wild card?
    CMP.B   D2,D1
    BNE.S   40$
30$
    BTST    #7,2(A2)        ;check 'is safe' flag
    
    BNE.S   50$         ;exit (unsafe)

    ADDQ.L  #4,A2
    BRA.S   10$         ;reload D0+loop

40$
    CMP.B   #-1,D2          ;-1 terminator
    BEQ.S   50$
    ADDQ.L  #4,A2
    BRA.S   10$         ;loop

50$
    PULL_L  D2/A2
    RET             ;bye..

       ENDC
    

*
* RoomToRoom - table of room#'s and code to call if going from first
*         room to second
*
* Format of table:
*
*   RRENTRY     <source room>,<destination room>,<addr. of code>
*
* in addition to rooms 'any' is allowed, matching any room.
* table is terminated by a -1 WORD
*

RoomToRoom

    RRSAFE      FIELD1,ORCHARD,Bull

    RRSAFE      FIELD2,ORCHARD,CutFence
    RRSAFE      WOOD1,ORCHARD,CutFence
    RRSAFE      ORCHARD,FIELD2,CutFence

    RRSAFE      any,CONSERVATORY,StartPhone
    RRSAFE      any,BACKGARDEN2,StartKnocks


    RRSAFE      LAKE1,any,Paddling
    RRSAFE      any,LAKE1,Paddling


    RRSAFE      any,PUB,EnterPub
    RRSAFE      PUB,FAIRGROUND,ExitPub

    RRSAFE      BOATHOUSE,CANOE_MOORING,DwarfLeaves
    RRSAFE      BAKERY,BAKERY_OVEN,EnterKitchen
    RRSAFE      BAKERY_OVEN,BAKERY,LeaveKitchen
    RRSAFE      FAIRGROUND,BAKERY,EnterBakery
    RRSAFE      BAKERY,FAIRGROUND,LeaveBakery

* These all call DoGuardian
    RRSAFE      any,INFINITE1,InfiniteEast
    RRSAFE      INFINITE1,COUNTRYLANE1,InfiniteWest
    RRSAFE      INFINITE2,COUNTRYLANE1,InfiniteEast
    RRSAFE      any,INFINITE2,InfiniteWest
    RRSAFE      COUNTRYLANE1,FRONTGARDEN1,GuardianCheck
*------

    RRSAFE      END_OF_BEAM,FAIRGROUND,Plummet

*   RRSAFE      CLOUD_ROOM,OUTSIDE_STATION,Plummet
    RRSAFE      WATERFALL,any,ArriveAtStation
    
    RRSAFE      GIRDER_ROOM,ON_PLATFORM,LeapGap
    RRSAFE      ON_PLATFORM,GIRDER_ROOM,WontLeapGap

    RRSAFE      ONBUS,ONBUS,LeaveBus

    RRSAFE      any,ONLAKE,OverInfinite

    RRSAFE      any,INF_UNDERWATER,UnderInfinite
    RRSAFE      any,OVER_HATCH,Setover_hatch
    RRSAFE      any,UNDERWATER2,SetUnderwater2
    RRSAFE      UNDERWATER2,UNDERWATER,SetUnderwater

    RRSAFE      POSTOFFICE,FAIRGROUND,LeavePOffice
    RRSAFE      FAIRGROUND,POSTOFFICE,EnterPOffice

    RRSAFE      BEERGARDEN,BOTTOM_WELL,EnterWell
    RRSAFE      INF_UNDERWATER,ONLAKE,OutWater
    RRSAFE      any,OUTSIDE_STATION,GotoBeginBusking
    RRSAFE      TRAIN_STATION,PLATFORM1,SetUpTrain
    RRSAFE      any,ON_TRAIN,SetUpGuard
    RRSAFE      PASSAGE,GAOL,WakeUpGaoler
    RRSAFE      FIELD1,FIELD1,GoHole
    RRSAFE      HALLWAY1,FRONTGARDEN1,StartHorror
    RRSAFE      FRONTGARDEN1,HALLWAY1,StartHorror
    RRSAFE      BACKGARDEN1,COUNTRYLANE2,StartHorror
    RRSAFE      COUNTRYLANE2,BACKGARDEN1,StartHorror
*   RRSAFE      any,KITCHEN2,MouseTrap
    RRSAFE      IN_HATCH,CASTLE_KITCHEN,ScoreOutCell
    RRSAFE      HALLWAY,WITCH_CHAMBER,EnterWitchChamber
    RRSAFE      SPIRAL_STAIRCASE,HALLWAY,Do_Voices
    RRSAFE      WITCH_CHAMBER,BALCONY,OnBalcony
    RRSAFE      CAROUSEL,CLOCKMAKERS,Do_MoveStool
    RRSAFE      CLOCKMAKERS,CAROUSEL,Do_MoveStool
    RRSAFE      CASTLE_PARAPETS,WALLED_GARDEN,UnicornLeaves
    RRSAFE      GIRDER_ROOM,CLOCK,TooFarDown

    DC.W        -1

*******************
*
* This bit will happen for all moves between rooms - AFTER all other
* setmode entries.
*
* NO NO NO NO! This happens BEFORE any other specials are invoked!!!
*
*******************

        XREF    R.NOUN2,BakerFollows,BoozedUp
        XREF    NOSINKMSG,TapDancing
        XREF    SEATED

AllRoomSpecials

    XREF    IsRoomWet

    CMP.B   #RNONLAKE,D1        ;we'l allow them to swim upto here
    BEQ.S   02$         ;ok
    DO  IsRoomWet       ;wet room need flippers
    BEQ.S   02$         ;no
    MOVE.W  #NFLIPPERS,D0
    GETINFO
    TST_WORNBYPLAYER        ;are u wearing them?
    BNE.S   02$         ;fine.
    SCN 160         ;you dont go anywhere
    BRA 80$         ;exit
02$
    TEST_B  TapDancing(A4)
    BEQ.S   01$         ;no
    SCN 449         ;dance there
01$
    CMP.W   #NCANOE,SEATED(A4)  ; On canoe?
    BNE.S   10$
    CMP.W   #VNPADDLE,VERB(A4)
    BEQ.S   20$
    CMP.W   #-1,VERB(A4)
    BEQ.S   20$
    SCN 106     ; Must PADDLE canoe 
    BRA.S   80$     ; .. so don't let him go!
10$:
    CMP.W   #VNPADDLE,VERB(A4)
    BNE.S   30$
20$:
    CMP.W   #RNUNDERWATER,D1
    BEQ.S   21$
    CMP.W   #RNLAKE1,D1 ; TO lake?
    BEQ.S   90$
    CMP.W   #RNLAKE1,ROOM(A4)   ; or FROM lake ?
    BEQ.S   90$
21$
    SCN 101     ; Cant paddle here!
    BRA.S   80$

30$
    MOVE.W  D1,-(SP)
    MOVE.B  BoozedUp(A4),D0 ; Get booze level
    CMP.B   #2,D0       ; Check for more than tipsy...
    BLE.S   35$     ; not pissed, so no trouble.
    MOVE.B  #4,D1
    DO  RND.D1      ; random no  0-3
    SUBQ.B  #2,D0       ; Frig D0
    CMP.B   D1,D0       ; See if too pissed?
    BLE.S   35$
    MOVE.B  #3,D1
    DO  RND.D1
    EXT.W   D1
    ADD.W   #414,D1
    DO  P.SCN
    BRA.S   37$
35$:    
    MOVE.W  (SP)+,D1
    BRA.S   90$
37$:
    MOVE.W  (SP)+,D1
80$:
    ADDQ.W  #8,A7       ;Yes, twice is ok - but watch it!! Paul
                ; Twice???***???
90$:
    RET


*******************************************************************************
*
* AllRoomAfter does what AllRoomSpecials was commented as doing - Namely,
* you only get here IF a move to another room is REALLY imminent. The code
* in here shouldn't really prevent movement by POPSPs. Do that elsewhere!


AllRoomAfter

    ST  NOSINKMSG(A4)   ;prevent things from sinking 1st time
    MOVE.W  D1,D2
    DO  R.NOUN2     ;room# ->D0
    GETINFO
    TST_WET             ;wet room?
    BEQ.S   01$     ;no
    TEST_W  SEATED(A4)  ;are you boating or suchlike?
    BNE.S   01$     ;yes, ok
    CALL    GetWet
01$
    TEST_B  BakerFollows(A4)    ; Is baker following you?
    BEQ.S   20$
*   MOVE.W  D1,-(SP)        ; Save room #
    MOVE.W  #NBAKER,D0
    GETINFO
    MOVE.W  D1,8(A0)        ; move baker here...
    SCN 394         ; print message
; the baker stumps around behind you...
*   MOVE.W  (SP)+,D1        ; restore room #

* Douse the candle if player attempts to wander around outside with it lit.
* Two reasons for this: it's silly; it may cause hassles.
20$:
    MOVE.W  #NCANDLE,D0
    DO  ISIN
    BNE.S   35$
    TEST_W  8(A0)           ;On Inventory?
    BNE.S   35$         ;=> nope
* Must suss if player has stashed it in a bag or summit!
* Not here though - should be caught when he tries to do it coz it's
* silly putting a lit candle in a bag!
    TST_LIT             ;Lit?
    BEQ.S   35$         ;Nope, so okay

    CALL    AreWeOutside        ;Going outdoors? (Takes D1)
    BEQ.S   35$         ;=> Nope
    BCLR    #7,(A0)         ;Candle no longer 'lit'
    CLR.B   TIMER5(A4)      ;Fod the 'CandleLife' timer
    SCN 31
* As you step outside your candle blows out.

35$

* Fall through............


    CALL_S  StartCloud
**** DO NOT APPEND anything to start clock as it is called from outside.
    XDEF    StartClock
    XREF    EventRoom,EventNum,TIMERAFTER13

StartClock

    LEA EventRoomsEnd(PC),A0    ;end of msg,room# table
    MOVEQ   #EventRooms_SIZE-1,D0
10$
    MOVE.B  -(A0),D3        ;we go from end to start (downwards)
    CMP.B   D3,D1           ;Right room?
    DBEQ    D0,10$
    BNE.S   90$

20$
    MOVE.W  D0,EventNum(A4)         ;Stash which event
    MOVE.W  D1,EventRoom(A4)        ;and which room room
    MOVE.B  #10,TIMERAFTER13(A4)        ;Start event timer
90$

    RET

*------------------------------

    XREF    CloudAround,FirstCloud
    XDEF    StartCloud
StartCloud:
* this will put the cloud into the room - if it is around AND if the room is
outside...
* D1 points to room we are entering (or we are in...)
    TEST_B  CloudAround(A4)     ; is the cloud around?
    BEQ.S   99$
    TEST_B  FrozenCloud(A4)     ; a frozen cloud don't go nowhere.
    BNE.S   99$
* are we on the platform..?
    CMP.W   #RNON_PLATFORM,D1   ; THIS is a special case.
    BEQ.S   99$         ; do nothing for now..
    CALL_S  AreWeOutside        ; are we outside (Takes D1)
    BEQ.S   80$         ; nope ... nothing happens.
    TEST_B  FirstCloud(A4)      ; never seen it before.?
    BEQ.S   10$
    CLR.B   FirstCloud(A4)
    CMP.B   #1,CloudAround(A4)  ; really really first time.
    BNE.S   05$
    SCN 57          ; okay guys here we are
    BRA.S   10$
05$
    SCN 578         ; not really first time...
10$
    MOVE.W  #NNICECLOUD,D0
    GETINFO
    EXT.L   D1
    MOVE.L  D1,6(A0)        ; make the cloud simply in room.
80$
* change the state from 1 to 2 (or 2 to 2...)
    MOVE.B  #2,CloudAround(A4)  ; set it up to be around ..more
    
99$
    RET


*--------------------------

* Table and macro for StartClock routine above

DefEventRoom    MACRO   *ROOM name

    DC.B    RN\1

    ENDM

EventRoomsStart

    DefEventRoom        LIBRARY1
    DefEventRoom        CANOE_MOORING
    DefEventRoom        HOLLYBUSH
    DefEventRoom        FRONTGARDEN2
    DefEventRoom        BOATHOUSE
    DefEventRoom        STEPS1
    DefEventRoom        CLOCK
    DefEventRoom        ON_PLATFORM
    DefEventRoom        END_OF_BEAM
    DefEventRoom        PUB
    DefEventRoom        TRAIN_STATION
    DefEventRoom        TOP_MOUNTAIN
    DefEventRoom        DUNGEON
    DefEventRoom        POSTOFFICE
    DefEventRoom        PLATFORM_BRIDGE

EventRoomsEnd

EventRooms_SIZE     EQU EventRoomsEnd-EventRoomsStart

    ALIGN

*----------------------------------------------------

*
*
* AreWeOutside  -   Susses if the room # passed in D1 is an
*           'outside' location.
*
*   RETURNS:        NE if it is
*               EQ if not
*
*
;Surely calling VALID.NP on #NOUTSIDE is more sensible????? - Roddy.
; Nope, coz it's called to find out IF a prospective room change
; would take the player into an 'outside' location. Possibly it
; could take liberties with ROOM but I don't rekon that's nice - Paul

        XDEF    AreWeOutside

        XREF    PSUDAT

AreWeOutside

    PUSH_L  D0/D7/A0
    MOVE.W  ROOM(A4),A0
    MOVE.W  D1,ROOM(A4)
    MOVE.W  #NOUTSIDE,D0
    DO  VALID.NP
    MOVE.W  A0,ROOM(A4)     ; Stomps the flags.
    TEST_B  D7
    EOR #4,CCR          ;put flags right way round
    PULL_L  D0/D7/A0
    RET


    IFNE    0           ;old version

    PUSH_L  D0/A0-A1        ;Scratch
    MOVE.W  #NOUTSIDE,D0        ;Pseudo in all 'outside' locations
    GETINFO
    CLR.W   D0          ;I know #NOUTSIDE isn't > 255... BUT!
    LEA PSUDAT(A4),A1       ;Get base pseudo data
    ADD.W   8(A0),A1        ;Add our offset
10$
    MOVE.B  (A1)+,D0
    BEQ.S   85$         ;=> pseudo list terminated
    CMP.W   D0,D1           ;Is the room we're entering 'outside'?
    BNE.S   10$
80$
    SETNE               ;Flag 'going outside' to caller
85$                 ;EQ if we branch here!!!
    PULL_L  D0/A0-A1
    RET

    ENDC    ;0
*--------------------------


**********************************************************************
*
* Now some of the bits of code...
*
**********************************************************************
*
* This happens if you try and get past bull.
*
    XREF    FlagBull,TIMER1

Bull
    TEST_B  FlagBull(A4)        ; If you havent disposed of bull...
    BNE.S   90$
    TEST_B  TIMER1(A4)      ;Is bull charging right NOW?
    BEQ.S   10$
    SCN 108
* Unfortunately, the bull is coming from that direction!^

    BRA.S   80$
10$
    SCN 21          ; ... then the bull won't let you
80$
    POPSP
90$:    
    RET

*********************************************************************
*
* Trying to get through the fence ??
*
    XREF    FlagFence
CutFence
    TEST_B  FlagFence(A4)       ; If fence isn't cut, then...
    BNE.S   20$
    SCN 32          ; You can't get through
    POPSP_RET
20$:
    SCN 33
    RET

**********************************************************************
*
* Start knocking for the first time...
*
StartKnocks
    TEST_W  KnockPos(A4)        ; If we haven't started it already...
    BNE.S   90$
    MOVE.B  #1,TIMER2(A4)       ; ... then start knocking!
90$:
    RET



********************************************************************************
*
* this happens moving to or from the LAKE room.
*
Paddling
    CMP.W   #RNUNDERWATER,ROOM(A4)  ; From underwater is no problem...
    BNE.S   5$
    CALL    OutWater
    BRA.S   95$
5$
    CMP.W   #NCANOE,SEATED(A4)  ; Otherwise, must be on canoe
    BNE.S   90$
    MOVE.W  #NPADDLE,D0     ; ... with the paddle
    DO  CARRIED
    BEQ.S   97$
    SCN 105         ; You need a paddle
    BRA.S   92$
90$:
    MOVE.W  #NDIVING_SUIT,D0    ; Wearing diving suit?
    GETINFO
    TST_WORNBYPLAYER        ;worn by player?
    BNE.S   95$         ;not equipped
    SCN 102
* You're not suitably equipped for swimming. Besides, it looks very cold.

92$:
    POPSP_RET
95$:


;*********> This is a nono, as AllRoomSpecials may PREVENT you moving if
;*********> you haven't said 'PADDLE', and so you're on dry land, but
;*********> with Underwater SET. 
; P.S. Does riding on canoe mean to have 'Underwater' set anyway????


    CALL    SetUnderwater       ;Wotchit! This ain't wot u fink!
    CMP.B   #RNUNDERWATER,D1    ;are you going down?
    BNE.S   97$         ;no
    ST  UnderWater(A4)      ;otherwise we really r underwater.
97$:
    RET

*------------------------------------------

    XREF    Hooligan,SP.RowdyInPub
    XREF    WeirdAttire

* Start pub closing timer
EnterPub
; you're not allowed to wear Aqualungs in Pub!!
    MOVE.L  D1,-(A7)
    CALL    WeirdAttire
    PULL_L  D1
    BNE.S   20$     ; if clothes weird, then must be rowdy!
10$
    TEST_B  Hooligan(A4)    ; a known hooligan?
    BEQ.S   90$
20$
    POPSP
    BRA SP.RowdyInPub
90$
    RET
    
    XREF    SP.SnatchGlass
; check that player does not take glass from pub
ExitPub
    BRA SP.SnatchGlass  ;swipe the glass if player is stealing it

*------------------------------------------


********************************************************************************
*
* Handle entering the canoe mooring, to see if dwarf leaves the sack.
*
DwarfLeaves
    TEST_B  DwarfDone(A4)   ; Done it already?
    BNE.S   90$
    TEST_B  DoorCreaked(A4) ; No, so has the door creaked?
    BNE.S   10$
    SCN 171     ; No, so dwarf runs away.
    SCORE   OPENBHOUSE  ;Score for surprising dwarf
    BRA.S   80$

; Come here when dwarf does a runner.
10$:
    SCN 170     ; Dwarf scurries off...
    MOVE.W  #NSACK,D0   ; ... taking sack with him.
    TEST_B  REMASTER(A4)
    BEQ.S   20$
    GETINFO
    MOVE.L  #RNSTREAM1,6(A0)    ; bag left at the Spring
    BRA.S   80$
20$    
    DO  SETOUT
80$:
    ST  DwarfDone(A4)   ; Say we've done it once.
90$:
    RET

*******************************************************************************
*
* Make baker descend stairs if you attempt to leave the kitchen.
    XREF    BakerFollows,BakerDescends
LeaveKitchen
    TEST_B  BakerFollows(A4)    ; Is he following?
    BNE.S   10$
    CALL    BakerDescends
    POPSP
10$:
    RET

*
* Check you've asked about job b4 letting you into kitchen

    XREF    AppliedForJob

EnterKitchen
    TEST_B  BakerFed(A4)    ; Once baker is fed, can't re-enter kitchen
    BNE.S   10$
    TEST_B  AppliedForJob(A4)   
    BNE.S   30$     ; Can't go into kitchen 'til job applied for
10$:
    SCN 340
    POPSP_RET

30$:
    MOVE.B  #15,TIMER10(A4)
    
    TEST_B  REMASTER(A4)
    BEQ.S   50$
    MOVE.B  #18,TIMER10(A4)     ; allow more steps for touch version
50$:
    RET
*************************************************************
*
* Infinite bits - to deal with endless lane
*
*

*
* InitLanePos - you can only do this once!
*
* sets up the misc junk needed to fool the player that the
* lane is more that 2 rooms!
*
* takes D3 as x pos (+ve=> east, 0==lane, -ve=>west)
* returns D1 as new room#
*

InitLanePos

    MOVEQ   #RNCOUNTRYLANE1,D1  ;lane at centre
    TEST_W  D3
    BEQ.S   90$         ;do nothing (all junk==0)
    BMI.S   10$
    MOVEQ   #RNINFINITE1,D1
    MOVE.W  #NINFINITE1,D0
    BRA.S   20$
10$
    MOVEQ   #RNINFINITE2,D1
    MOVE.W  #NINFINITE2,D0
20$
    MOVE.W  D0,FGT_Infinite+2(A4)   ;about bus stop
    
    MOVE.W  D3,LanePos(A4)
    MOVE.W  D3,MaxEast(A4)      ;how far east we've been
    NEG.W   D3
    MOVE.W  D3,MaxWest(A4)      ;how far west we have not been
90$
    RET

    XREF    MaxEast,MaxWest,LanePos,FGT_Infinite

* OK, so I could have used LastDirection - but this lot works now...

* going east down lane
InfiniteEast

    MOVEQ   #1,D0               ;dir - going east
    BRA.S   DoInfinite


* going west down lane
InfiniteWest

    MOVEQ   #-1,D0              ;dir - going west
*   BRA.S   DoInfinite          ;Fall through


DoInfinite
    
    CALL    DoGuardian
    BNE.S   08$             ;NE =>guardian won't let u out

    MOVE.W  #8192,D4            ;max in any dir
    SUB.W   D0,D4               ;were we would be leaving

    MOVE.W  LanePos(A4),D3          ;where we were (x)
    BEQ.S   20$             ;coming from lane?
    BPL.S   05$
    NEG.W   D4
05$
    CMP.W   D3,D4               ;we at the end?
    BNE.S   10$
    SCN 279             ;wack - you bang your head
08$
    POPSP_RET
10$
    MOVEQ   #0,D4               ;where we were (y)
    MOVE.W  ROOM(A4),D2
    CALL    RemoveOldStuff          ;D2 = room#
20$
    MOVE.W  D0,D7               ;cos we step on D0
    MOVE.W  #-1,D0          ;no add desc on clane1
                    ;know where bus stop goes...
    ADD.W   D7,LanePos(A4)          ;work out new pos.
    BEQ.S   90$             ;EQ => back to lane
    MOVE.W  #NINFINITE2,D0          ;westward lane
    GETINFO

    MOVE.W  LanePos(A4),D2          ;MI => in west lane
    BMI.S   25$
    MOVE.W  #NINFINITE1,D0          ;eastward lane
    GETINFO

* consider whether we've been here before or not

25$
    CMP.W   MaxEast(A4),D2
    BLE.S   30$
    BCLR    #4,4(A0)
    ADDQ.W  #1,MaxEast(A4)
    BRA.S   40$
30$
    NEG.W   D2
    CMP.W   MaxWest(A4),D2
    BLE.S   40$
    BCLR    #4,4(A0)        ;kill explored so we get room desc
    ADDQ.W  #1,MaxWest(A4)
40$
    MOVE.W  8(A0),D1        ;room# to aim at
    MOVE.W  D1,D2           ;as param for AddNewStuff
    MOVE.W  LanePos(A4),D3

    CALL    AddNewStuff
90$
    MOVE.W  D0,FGT_Infinite+2(A4)   ;which room u look at to see
    PUSH_L  D0/A0           ;the bus stop
    MOVE.W  #NBSTOP,D0      ;the bus stop is in 16n th rooms
    DO  SETIN           ;set it in+gets A0
    MOVE.W  LanePos(A4),D0      ;get current posn
    AND.B   #$0F,D0         ;at 16nth room?
    BEQ.S   95$         ;leave out
    BSET    #7,6(A0)        ;put back out (ok, cos it's a PEUDO)
    MOVE.W  #-1,FGT_Infinite+2(A4)  ;so we don't see message about bstop
95$
    PULL_L  D0/A0
    RET

*------------------------------------------

SetUnderwater
    XREF    Underwater_x,Underwater_y

    PUSH_L  D2-D4
    MOVE.W  #$100,D3    ;x
    MOVE.W  D3,D4       ;y
    MOVE.W  D3,Underwater_x(A4)
    MOVE.W  D4,Underwater_y(A4)
    ; D2 is already set up
    CALL    AddNewStuff
    CALL    GetWet
    PULL_L  D2-D4
    RET

Setover_hatch
    XREF    Underwater_x,Underwater_y,OfficeFlooded

    CMP.W   #RNAIRLOCK,ROOM(A4) ;in the airlock?
    BNE.S   10$
    TEST_B  OfficeFlooded(A4)   ;flood over?
    BNE.S   10$         ;yes, ok
    TEST_B  TIMER9(A4)      ;during flood?
    BEQ.S   10$         ;no, then ok
    SCN 179         ;nocando
    POPSP_RET
10$
    MOVE.W  #$100-1,Underwater_x(A4)
    MOVE.W  #$100+2,Underwater_y(A4)
    BRA GetWet

SetUnderwater2
    XREF    Underwater_x,Underwater_y
    MOVE.W  #$100-1,Underwater_x(A4)
    MOVE.W  #$100+1,Underwater_y(A4)
    RET
*------------------------------------------

UnderInfinite
    XREF    Underwater_x,Underwater_y,ROOM

    PUSH_L  D2-D4/A0
    ST  UnderWater(A4)      ;you are now submerged.
    MOVE.W  Underwater_x(A4),D3
    MOVE.W  Underwater_y(A4),D4 ;set up current pos
    MOVEQ   #RNONLAKE,D2        ;room to remove things from
    CALL    RemoveOldStuff
    CALL    CALCNEWXY       ;mods d3/d4 wi lastdirection
    MOVE.W  D3,Underwater_x(A4) ;update
    MOVE.W  D4,Underwater_y(A4) ;and this
    CALL    DOMAXXY
    LEA UnderWaterTable(PC),A0  ;table of location changes
10$
    MOVE.B  (A0)+,D2        ;get x coord
    CMP.B   #$80,D2         ;end of table?
    BEQ.S   90$         ;yes, quit
    CMP.B   D2,D3           ;correct?
    BNE.S   20$         ;no
    CMP.B   (A0),D4         ;y coord?
    BNE.S   20$         ;no
    CLR.W   D1
    MOVE.B  1(A0),D1        ;set to this room
    BRA.S   90$         ;and quit
20$
    ADDQ.L  #2,A0           ;next entry
    BRA.S   10$         ;again
90$
    PULL_L  D2-D4/A0
    RET

UnderWaterTable
* contains coordinates for changes of location.

    DC.B    -1,2,RNOVER_HATCH
    DC.B    0,0,RNUNDERWATER
    DC.B    -1,1,RNUNDERWATER2
    DC.B    3,1,RNOVER_WRECK
    DC.B    $80
    ALIGN


OverInfinite
    XREF    Underwater_x,Underwater_y,ROOM

    PUSH_L  D2-D4/A0
    CLR.B   UnderWater(A4)      ;not submerged on surface
    MOVE.W  Underwater_x(A4),D3
    MOVE.W  Underwater_y(A4),D4 ;set up current pos
    MOVEQ   #RNONLAKE,D2        ;this is new room
    CALL    AddNewStuff     ;anything here?
    PULL_L  D2-D4/A0
    RET

*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
*
* THESE TWO BITS (AddNewStuff & RemoveOldStuff) WILL NOT LIVE HERE
*
* SO ANYONE EVEN THINKING ABOUT CHANGING THEM GETS FED TO A RABID Ken
*
*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

*
* AddNewStuff
*
* 'reveals' the junk that ought to be kicking about in the target room
*
* D2 = room# D3 =x pos D4 = y pos
*
* x/y pos 14 bits signed ( +/- 8191 )
*

AddNewStuff

    PUSH_L  D0-D6/A0-A1
    PUSH_W  D2/D3       ;save 'real' room# of fake room & save y pos
    MOVE.W  D4,D2           ;search on y-pos first
    MOVEQ   #8,D1           ;target field
    ST  D5          ;wordwise
    ST  D6          ;and non-local
    DO  GETNOUN2        ;suss to A1
    MOVEQ   #0,D2           ;top 16 bits will be simply in room
    PULL_W  D2/D3           ;get D2 (room) & D4 target y-pos
    OR.W    #$C000,D3       ;add x-pos distinguishing bits
    OR.W    #$C000,D4       ;helps avoid out+pseudos etc
10$
    MOVE.W  (A1)+,D0        ;get a pos. noun
    BMI.S   10$         ;MI => to ignore
    BEQ.S   20$         ;EQ => finished
    GETINFO
    CMP.W   6(A0),D3        ;right x-pos?
    BNE.S   10$         ;nope, try next
    CMP.W   8(A0),D4        ;right y-pos?
    BNE.S   10$
    MOVE.L  D2,6(A0)        ;yes, fod into room
    BRA.S   10$         ;next...
20$
    PULL_L  D0-D6/A0-A1
    RET
    

*
* RemoveOldStuff
*
* 'hides' the stuff that was kicking about in the room you are leaving
*
* D2 = room# D3 =x pos D4 = y pos
*
* x/y pos 14 bits signed ( +/- 8191 )
*

RemoveOldStuff

    XREF    GETRELN2

    PUSH_L  D0-D4/A0-A1
    OR.W    #$C000,D3           ;so it looks out
    OR.W    #$C000,D4           ;so we can tell!

    DO  GETRELN2            ;not likely to step on D0/D1/A1
10$
    MOVE.W  (A1)+,D0            ;get a noun#
    BMI.S   10$             ;neg => no no
    BEQ.S   20$             ;EQ => end of table
    GETINFO
    TEST_W  6(A0)               ;only simple-in-room is ok
    BNE.S   10$
    TST_PSEUDO              ;leave pseudos alone
    BNE.S   10$
    MOVE.W  D3,6(A0)            ;save x pos
    MOVE.W  D4,8(A0)            ;save y pos
    BRA.S   10$             ;next please...
20$
    PULL_L  D0-D4/A0-A1
    RET

*----------------------------------------

    XDEF    CALCNEWXY
    XREF    LastDirection

CALCNEWXY
    
* entry
*   d3,d4= current x,y coords

    PUSH_L  D1/D2
    MOVE.W  LastDirection(A4),D1
    BMI.S   90$         ;do nothing

    LEA XCHANGES(PC),A0
    MOVE.B  0(A0,D1.W),D2

**********************************
*   MOVE.B  XCHANGES(PC,D1),D2  Modded to above 2 lines coz 6502 don't work
**********************************

    EXT.W   D2
    ADD.W   D2,D3           ;modify x coordinate

    LEA YCHANGES(PC),A0
    MOVE.B  0(A0,D1.W),D2

**********************************
*   MOVE.B  YCHANGES(PC,D1),D2  Modded to above 2 lines coz 6502 don't work
**********************************

    EXT.W   D2
    ADD.W   D2,D4           ;modify y coordinate
90$
    PULL_L  D1/D2
    RET

XCHANGES
    DC.B    0,1,1,1,0,-1,-1,-1,0,0
YCHANGES
    DC.B    1,1,0,-1,-1,-1,0,1,0,0

DOMAXXY
    XREF    Underwater_x,Underwater_y
    XREF    WaterMax_x,WaterMin_x
    XREF    WaterMax_y,WaterMin_y

    PUSH_L  D0/A0
    MOVE.W  Underwater_x(A4),D0     ;get x coord
    SUB.W   #$100,D0            ;adjust
    CMP.W   WaterMax_x(A4),D0       ;bigger?
    BLE.S   10$             ;no
    MOVE.W  D0,WaterMax_x(A4)       ;update
    BRA.S   80$             ;exit, yes
10$
    CMP.W   WaterMin_x(A4),D0       ;less than min?
    BGE.S   20$             ;no
    MOVE.W  D0,WaterMin_x(A4)       ;update
    BRA.S   80$             ;exit, yes
20$
    MOVE.W  Underwater_y(A4),D0     ;get y coord
    SUB.W   #$100,D0            ;adjust
    CMP.W   WaterMax_y(A4),D0       ;bigger?
    BLE.S   30$             ;no
    MOVE.W  D0,WaterMax_y(A4)       ;update
    BRA.S   80$             ;exit, yes
30$
    CMP.W   WaterMin_y(A4),D0       ;less than min?
    BGE.S   40$             ;no
    MOVE.W  D0,WaterMin_y(A4)       ;update
80$
    MOVE.W  #NINF_UNDERWATER,D0
    GETINFO
    BCLR    #4,4(A0)            ;set as not explored
40$
    PULL_L  D0/A0
    RET
*
*
*
************************************************************************


; On any occasion when you can jump down, use brolly to break your fall.
    XREF    SP.CloudUnreach
    XDEF    Plummet
Plummet:
    CALL    SP.CloudUnreach
    MOVE.W  #NUMBRELLA,D0
    GETINFO
    TEST_L  6(A0)       ; Is it carried??
    BNE.S   90$     ; Nope, so pain
    TST_OPEN        ; is it OPEN
    BNE.S   99$
90$:

; MAYBE WE HAVE A GUARDIAN SPIEL HERE???

    SCN 298     ; Wheee, Ouch!
    RET         ; Sarky message, but you survive.
99$:
    SCN 299     ; You float safely down.
    RET

    XREF    TelephoneRung,RingCount,PhoneAnswered
; Initiate call from XAM when he walks into the house

StartPhone

    TEST_B  REMASTER(A4)
    BEQ.S   80$
    TEST_B  PhoneAnswered(A4)
    BNE.S   99$                 ; dont ring again 
80$    
    BSET    #7,TelephoneRung(A4)
    BNE.S   99$
    SCN 302     ; Your phone starts to ring.
    MOVE.B  #1,RingCount(A4); So we can tell its ringing!
    MOVE.B  #2,TIMER6(A4)   ; Start it ringing.
99$:
    RET

*------------------------------


*
*
* DoGuardian    -   Won't let you go till you've read the document
*
*

;   XDEF    DoGuardian
    XREF    GuardianHere,TIMER12

DoGuardian

    TEST_B  GuardianHere(A4) ; If guardian is here, then he won't let you...
    BEQ.S   90$      ;...leave!
    SCN 377
* The guardian
    SCN 308
* says, Do me a favour. Read the wossname etc etc.

    ADDQ.B  #2,TIMER12(A4)  ;Not two spiels at once thanks

    SETNE           ;Tell caller no room change
90$
    RET


*-----------------

*
*
* GuardianCheck -   Special case not coped with by infinite code's
*           call to DoGuardian when player goes from
*           countrylane1 to frontgarden1
*

GuardianCheck

    CALL_S  DoGuardian
    BEQ.S   90$     ; room change allowed

    POPSP
90$
    RET

*-------------------------------


*
*
* LeaveBus  -   Copes with player getting off the bus.
*
*           Initialises the prospective 'infinite'
*           room number where he gets dumped
*
*   Expects:    WhereToGetOff to contain signed position
*           of infinite room number


    XDEF    LeaveBus
    XREF    WhereToGetOff,SETPRON,DROPHERE,FlagHassleFromFuckHead

LeaveBus

    MOVE.W  WhereToGetOff(A4),D3
    CALL    InitLanePos     ;Tell infinite code where we are

    MOVE.W  D1,ROOM(A4)         ;InitLanePos returns the room #
    MOVE.W  #1,SCRPNT(A4)       ;Not superfluous
    CMP.W   #RNCOUNTRYLANE1,D1  ;Right place?
    BNE.S   10$
    SCORE   LEAVEBUS        ;Score if so
10$
    PUSH_L  D0/A0           ;Safe!
    MOVE.W  #NKEYRING,D0
    GETINFO
    SCN 178
* The inspector calls after you, saying, "Oy! Don't forget these ..."
* He tosses a bunch of keys towards you, but they land on the
* ground.^
    DO  SETOUT          ;Fix tied etc
    CALL    SETPRON         ; make it the pronoun (get it)
    CALL    DROPHERE        ; Drop the keyring HERE!

    SCN 272
* The doors close and the bus pulls away.

    ST  FlagHassleFromFuckHead(A4)  ;let interfering wi keys
                        ;trigger hassle

    PULL_L  D0/A0

    RET             ;Exit

*---------------------------------

    XREF    UnderWater,TIMERB,PUSHBUF,FloodObjects
    XDEF    GetWet
GetWet

* your things get wet in water

    PUSH_L  D0-D2/A0-A2
    TEST_B  TIMERB(A4)  ;is drying timer going?
    BEQ.S   01$     ;no
    MOVE.B  #10,TIMERB(A4)  ;if so, then start a ful 10 moves
01$
    MOVEQ   #0,D2       ;you
    DO  GETRELN     ;all things ->a1
    CALL    PUSHBUF     ;we'l flood them, paramatrize stack
    CALL    FloodObjects    ;recusively get'em
    ADD.L   D2,A7       ;adjust stack
    PULL_L  D0-D2/A0-A2
    RET

*-----------------------------------------
    XDEF    OutWater
    XREF    UnderWater

OutWater
    CLR.B   UnderWater(A4)
    RET

; Leaping to the platform from the girder...
LeapGap
    CMP.W   #VNJUMP,VERB(A4)    ; jumping?
    BNE.S   20$
    SCN 126         ; a daring leap!
    RET
20$:
    SCN 590         ; Can't just walk there
    POPSP_RET

; Leaping to the girder from the platform (or trying to)
WontLeapGap
    PUSH_L  D0/A0
    MOVE.W  #NUMBRELLA,D0
    GETINFO
    TEST_L  6(A0)           ;holding brolly ?
    BNE.S   5$
    TST_OPEN
    BEQ.S   5$
    MOVE.W  #RNCLOCK,ROOM(A4)   ;move player to clock
    PULL_L  D0/A0
    POPSP               ;no mucking about
    BRA Plummet         ;Mary Poppins spiel
5$
    PULL_L  D0/A0
    ESCAPE  DARINGLEAP
    POPSP_RET           ;Leave this in

*----------------------


*
*
* LeavePOffice/EnterPOffice -   Simply sets flags for SPECIALS
*                   to say that we just left or
*                   entered the Post Office.
*
*   LeavePOffice makes the postmistress 'out' so you don't
*   see her if you go back in.
*

    XDEF    LeavePOffice
    XREF    IntroPMistress,PostMistressGone
    XREF    TIMERAFTER1A

EnterPOffice
; Kick off a specials routine to put postmistress in if bell rings.
    TEST_B  PostMistressGone(A4)    ; is she gone for good?
    BNE.S   90$
    MOVE.B  #10,TIMERAFTER1A(A4)    ;Set up prompt timer
    ST  IntroPMistress(A4)  ; set up for specials...
90$
    RET

; When you leave the post office, postmistress goes away...
LeavePOffice
    MOVE.W  #NPOSTMISTRESS,D0
    DO  SETOUT          ; lose her...
    MOVE.W  #NSAFE,D0
    GETINFO
    MOVE.W  #NPOSTOFFICE,8(A0)  ; safe related to room
    MOVE.W  #NSCALES,D0
    GETINFO
    MOVE.W  #NPOSTOFFICE,8(A0)  ; ditto scales.
    RET

*----------------------------------------

; When in the well, he gets random messages.
EnterWell
    MOVE.W  D1,-(SP)    ; save dest. room
    MOVEQ   #5,D1       ; Set up start of well messages...
    DO  RND.D1
    ADD.B   #3,D1
    MOVE.B  D1,TIMERF(A4)   ; Start the timer going...
    MOVE.W  (SP)+,D1    ; restore dest. room 
    RET


LeaveBakery:
; If he's applied for the job, don't let him out until he's shown
; a cake to the baker.
    TEST_B  AppliedForJob(A4)   ; If not applied for job, then no prob.
    BEQ.S   90$
    TEST_B  BakerFed(A4)        ; else, must show cake to baker.
    BNE.S   90$
    SCN 396         ; 'Show me the cake'
    POPSP
90$:
    RET

    XREF    BakerFed,BakerAngry,DemolishedBakery
; Stops you returning to bakery if you've fed baker, or got him angry.
; or if you've blown it up
EnterBakery
    TEST_B  DemolishedBakery(A4)    ;Blown up the bakery?
    BEQ.S   5$          ;=> Nope
    SCN 615
* You blew the bakery up, remember? There's nothing left of it.

    BRA.S   80$
5$
    TEST_B  BakerAngry(A4)      ; The burnt one???
    BEQ.S   10$
    SCN 346         ; 'You again? Get lost!'
    BRA.S   80$

10$:
    TEST_B  BakerFed(A4)        ; Been given a cake??
    BEQ.S   90$
    SCN 345         ; semi-polite...
                    ; 'don't call us, etc...'
80$:
    POPSP
90$:
    RET

*-------------------------------------

GotoBeginBusking

    BRA BeginBusking

*-------------------------------------
    XREF    TrainBitOver,TIMERAFTER8

SetUpTrain

    BSET    #0,TrainBitOver(A4)     ;has it happened & set
    BNE.S   90$                     ;yes, forget it
    MOVE.B  #30,TIMERAFTER8(A4)     ;timeout til train moves
90$
    RET

*------------------------------------------

    XREF    TIMERAFTER9

SetUpGuard

    MOVE.B  #1,TIMERAFTER9(A4)      ;guard hasseles you
    RET

*------------------------------------------

    XREF    ROOM,GaolerAlerted

WakeUpGaoler

    TEST_B  GaolerAlerted(A4)
    BEQ.S   01$         ;ok
    SCN 540         ;meet him again
    BRA.S   WakeUpGaoler.DO
01$
    MOVE.W  #NFLIPPERS,D0
    GETINFO
    TST_WORNBYPLAYER                ;begin worn
    BNE.S   10$         ;yes unlucky
    MOVE.W  #NTAPDANCE_SHOES,D0 ; or the shoes
    GETINFO
    TST_WORNBYPLAYER                ;wearing these
    BEQ.S   90$         ;no, ok just exit
10$
    SCN 539         ;wakes up & chucks u in cell
    SCN 598         ; and says you cunt...(ajs)
    BRA.S   WakeUpGaoler.DO
90$
    RET

WakeUpGaoler.DO

    XDEF    WakeUpGaoler.DO
    XREF    XamRescued

    MOVE.W  #RNDUNGEON,ROOM(A4) ;u r there
    MOVE.W  #NXAM,D0
    DO  SETOUT          ;no xams there today
    ST  GaolerAlerted(A4)   ;doesnt fall asleep next time
    ST  XamRescued(A4)      ;effectively
    MOVE.W  #NSOLID_DOOR,D0     ;shut the cell door
    DO  CloseDoor
    POPSP_RET
    
*---------------------------

*
* GoHole    -   Stops player trying to get into hole that
*           the bull fell into.

GoHole

* NB We have to check the verb since CROSS or GO will
* go through setmode with From and To room as the same thing.
* Er, not any more apparently, so this code is remmed out.

*   CMP.W   #VNCROSS,VERB(A4)   ;Trying to cross the field?
*   BEQ.S   90$
*   CMP.W   #VNGO,VERB(A4)      ;or a silly GO field from the field
*   BEQ.S   90$
    SCN 107
* In case you hadn't noticed, there's this incredibly annoyed bull
* down there and I'm not going to extend the definition of adventurous
* to include foolhardy.

    POPSP
90$
    RET
    
*-----------------------------------


    XREF    HorrorPlace
StartHorror:
* if you enter of leave the part of the adventure from frontgarden
* to backgarden AND the phone has rung it sets off this nasty chain of 
* events....
    NOT.B   HorrorPlace(A4)     ; EQ => NE
    RET



*---------------------------------
*
* 1 in 3 times  when you walk into the messy kitchen (kitchen2)
* IF the mousetrap has been set AND the cheese is on it
* there is a dead mouse on the trap
*
* ps - the trap must be in the kitchen!
*

*   XREF    FlagTrapSet,FlagMouseDead
*
*MouseTrap
    
*   MOVE.L  D1,-(A7)            ;RND.D1 stomps this!
*   MOVE.W  #NMOUSE_TRAP,D0
*   GETINFO
*   CMP.L   #RNKITCHEN2,6(A0)
*   BNE.S   90$
*   MOVE.W  #NCHEESE,D0
*   GETINFO
*   CMP.L   #REL.MASK+NMOUSE_TRAP,6(A0)
*   BNE.S   90$             ;no cheese on trap
*   MOVEQ   #3,D1               ;1 in 3 prob
*   DO  RND.D1
*   TEST_B  D1
*   BNE.S   90$
*   DO  SETOUT              ;zap cheese
*
*   TEST_B  FlagTrapSet(A4)
*   BEQ.S   90$ ;EQ =>u didn't set the trap - the mouse gets the cheese!
    
*   ST  FlagMouseDead(A4)
*   MOVE.W  #NDEAD_MOUSE,D0
*   DO  SETIN
*80$
*   SF  FlagTrapSet(A4)
*90$
*   MOVE.L  (A7)+,D1
*   RET

*--------------------------------------

*
* Hmm, you score as you crawl out of the food hatch into the kitchen
*

    XREF    DO.SCORE

ScoreOutCell

    SCORE   ENTERKITCHEN            ;Escaped dungeon
    RET


*--------------------------

InitFuture
    
    XREF        FutureBits

** initialiser futures bits...

    ;; this code used to be in INIT
    ;; but is now called the first time you enter the witch chamber.
    ;; This is so that we can depend on a random number not set at the start

    PUSH_L  D1/D2
    MOVEQ   #4,D1
    DO      RND.D1
    LSL.W   #2,D1       ; mulu by 4 for the rotate.
    MOVE.W  FutureBits(A4),D2
    ROR.W   D1,D2   ; so they are in the new order.
    MOVE.W  D2,FutureBits(A4)
    PULL_L  D1/D2
    RET
    
*--------------------------

    XREF    LUCK_,RestartGame,DO.SCORE,FGT_Doors
    XREF    TIMERAFTER17

EnterWitchChamber

    MOVE.W  D1,D2               ; d1 => target room.
    CALL    DoExploredRoom          ; ne => been here.
    BNE.S   25$             ; tut tut shouldn't come

    ;; first time in, we decide the witch future
    CALL_S  InitFuture

    TEST_B  REMASTER(A4)
    BNE.S   10$             ; in remaster mode always select the right door
    CMP.W   #4,LUCK_(A4)    ;Got all you're luck?
    BEQ.S   10$             ;=> yep
    SCN 755
* Look's like you're luck's run out. You picked the wrong door. As you
* step through the doorway, you black out......

    BRA RestartGame         ;Hmmmmmm!

10$
    BSET    #7,FGT_Doors+2(A4)      ; never seen tehm again
    SCORE   SEEWITCH            ;Well, you get some points
    MOVE.W  #NWITCH_DOOR,D0         ;So, which door did he use?
    GETINFO
    TST_CLOSED              ;Closed?
    BNE.S   20$             ;=> yep
    MOVE.W  #NWITCH_DOOR1,D0        ;Must be the other one
20$
    DO  SETOUT              ;FOD this door
* If you have just entered the room. (What happens.)?
    TEST_B  XamRescued(A4)      ; did we rescue Xam..?
    BNE.S   30$
* you haven't rescued Xam therefore there is no reason for the witch to be
* on the balcony giving him a bollocking.
25$
    TEST_B  FlyzzedFuture(A4)
    BNE.S   27$
    MOVE.B  #1,TIMERAFTER17(A4)     ; set up that timer
27$
    RET
30$
    SCN 768
    MOVE.B  #4,TIMERAFTER17(A4)     ; you have four moves to get out

    TEST_B  REMASTER(A4)
    BEQ.S   90$
    MOVE.B  #20,TIMERAFTER17(A4)     ; you have 20 remastered moves to get out, enough to explore
90$    
    RET


*--------------------------------------

*
* Do_voices -   This susses wether or not you can hear
*           Xam and the witch insoide the witch chamber
*           while you're outside.

    XREF    XamRescued,TIMERAFTER16,DoExploredRoom

Do_Voices

    TEST_B  XamRescued(A4)      ;Managed to free Xam?
    BEQ.S   90$         ;=> nope, exit (no voices)
    MOVEQ   #RNWITCH_CHAMBER,D2 ;Have we been to the witches room b4?
    CALL    DoExploredRoom      ;sets Z on explored bit
    BNE.S   90$         ;=> been there, no voices
    MOVE.B  #1,TIMERAFTER16(A4) ;Yuk, there should be an easier way!
90$
    RET


*-------------------


    XDEF    ScenVanquished,Vanquished
    XREF    STOPRON,MovePlayer,P.Refresh,DecLuck,FlyzzedFuture,FlagHearth

ScenVanquished
* Takes D2 as the room number....
    SCN 756             ; set up stopron

SP.Vanquished:
*** Where end of game happens (ie back to start)...

    TEST_B  REMASTER(A4)
    BEQ.S   10$
    SCN     829
    CALL_S  VanquishReset
    RET
10$ 
    SCN 767

Vanquished
    TEST_B  REMASTER(A4)
    BEQ.S   10$
    SCN     830
    CALL_S  VanquishReset
    BRA     P.Refresh
10$
    BRA RestartGame

    XREF    WitchPlace
    
VanquishReset
    MOVE.W  #0,FlyzzedFuture(A4)
    MOVE.W  #0,FlagHearth(A4)
    MOVEQ   #RNCASTLE_KITCHEN,D1
    DO      MovePlayer          ; remaster vanquished to kitchen
    CALL    DecLuck

    MOVE.W  #NWITCH,D0
    GETINFO
    MOVE.W  #RNBALCONY,8(A0)        ; okay she is there
    CLR.W   WitchPlace+2(A4)
    RET    

*--------------------------------------

    XREF    NRT_STOOLTHROW

Do_MoveStool

    MOVE.W  #NSTOOL,D0
    GETINFO
    CMP.W   #RNCAROUSEL,(A5)        ;At the carousel?
    BNE.S   10$             ;=> Going shop to carousel

    ;; carousel -> shop
    CMP.L   #REL.MASK+NPSEUDOCMAKERS,6(A0)  ;In original pos?
    BNE.S   90$             ;=> no, exit
    MOVE.L  #REL.MASK+NCLOCKMAKERS,6(A0)    ;related to cmakers shop
    MOVE.W  #NCMAKER_FIRE,D0
    GETINFO
    TST_OUT
    BNE.S   05$
    MOVE.L  #REL.MASK+NCLOCKMAKERS,6(A0)
    BSET    #7,NRT_STOOLTHROW(A4)
05$
    BRA.S   90$             ;...and exit

10$
    ;; shop -> carousel
    CMP.L   #REL.MASK+NCLOCKMAKERS,6(A0)    ;Still related to shop?
    BNE.S   90$             ;Nope, exit
    MOVE.L  #REL.MASK+NPSEUDOCMAKERS,6(A0)  ;Put it back...
    MOVE.W  D0,NRT_STOOLTHROW(A4)
    MOVE.W  #NCMAKER_FIRE,D0
    GETINFO
    TST_OUT
    BNE.S   90$
    MOVE.L  #REL.MASK+NPSEUDOCMAKERS,6(A0)
90$
    RET

*---------------------------

    XREF    FlyzzedFuture

OnBalcony:


    TEST_B  FlyzzedFuture(A4)   ; have we flyzzed the future.?
    BEQ SP.Vanquished
    RET

*------------------------------------------

UnicornLeaves

    MOVE.W  #NUNICORN2,D0
    GETINFO
    BSET    #7,6(A0)        ;if unicorn is out, then journey over
    BNE.S   90$         ;end
    SCN 352         ;unicorn flies away
90$
    RET
    
*------------------------------------------

TooFarDown

    SCN 452
* Look, it's a twenty five foot drop. Things can't be that bad.
    RET

*------------------------------------------

* when you arrive at station on the cloud
    
ArriveAtStation    

    TEST_B  REMASTER(A4)
    SNE     FrozenCloud(A4)     ; remaster works like frozen
    TEST_B  FrozenCloud(A4) ; if he is frozen ... leave him there.
    BEQ.S   17$
    SCN 738
    MOVEQ   #RNOUTSIDE_STATION,D1
    MOVE.W  #NNICECLOUD,D0 
    GETINFO
    MOVE.L  D1,6(A0)    ; the cloud is in the  room...
    CLR.W   SEATED(A4)
    BRA.S   90$
17$
    SCN 194     ; and he is gone leaving you in a puddle.
    MOVE.W  #NPUDDLE,SEATED(A4) ; no longer seated
    MOVE.W  #IN,SEATED_RELATOR(A4)
90$    
    RET

*------------------------------------------    

    END


