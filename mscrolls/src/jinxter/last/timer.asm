******************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*  SRC.ADV.TIMER1
*
*--------------------------------

*   JINXTER !!!!!!!!!!!!!!!!!!!!!!

    include "nounbits.i"
    include "nway.i"
    include "equates.i"
    include "roomequ.i"
    include "nounequ1.i"
*   include "nounequ2.i"
    include "macros.i"
    include "scenmacros.i"
    include "scoreref.i"

    include "scen.i"

    XREF    TIMER2,TIMER3
    XREF    TIMER6,TIMER7,TIMER8,TIMER9,TIMERA,TIMERB
    XREF    TIMERF,TIMER11,TIMER12

    XREF    TIMERS_AFTER,TIMERAFTER1,TIMERAFTER2,TIMERAFTER13
    XREF    TIMERAFTER14,TIMERAFTER12

    XREF    CHEAT,TIMERS,GETDATA,ROOM,P.TN,ISARE,DO.SCN,DEAD,STOPRON
    XREF    W.REBUIL,GETNOUNS,P.SCN,SETPRON,TIMERA
    XREF    CHKPAUSE,R.NOUN,SEATED,SETOUT,C.STATUS,RND.D1
    XREF    PRTTXT,LINE,PRTHEX,VALID.NP
    XREF    YNHANDLR,KnockPos,KnockP2
    XREF    FloodPos,AirlockFlooded,OfficeFlooded
    XREF    DryOut,SETIN,MovePlayer,DO.SCORE,TIMERAFTER19
    XREF    TIMERAFTER1A
    XREF    REMASTER

    XDEF    TIMER,TIMER_AFTER,NumTimersAfter
*
* Handle all the kicking-off of various timer routines.
*

NumTimers   EQU 23      ;# of top timer. (Bottom one is 0) 
NumTimersAfter  EQU 28      ;Ditto for TIMER_AFTER

 
*
*
* TIMER     -   Called from Specials_Before.
*           Happens BEFORE room description.
*

TIMER
 
    MOVEQ   #NumTimers-1,D4
    LEA TIMERS(A4),A1
    LEA TimeTable(PC),A2
    BRA.S   TIMERCOMMON

*
*
* TIMER_AFTER   -   Called from top of SPECIALS.
*           Happens AFTER room description
*

TIMER_AFTER

    MOVEQ   #NumTimersAfter-1,D4
    LEA TIMERS_AFTER(A4),A1
    LEA TimeTable_After(PC),A2

*   BRA.S   TIMERCOMMON         ;Fall through...


*
*
* TIMERCOMMON   -   Updates running timers and calls relevant
*           code for any expired timer.
*

TIMERCOMMON

    IFNE    YCHEAT
    TEST_W  CHEAT(A4)       ;Timers don't happen in debug mode
    BNE.S   99$
    ENDC

10$
* First find a running timer...
    TEST_B  1(A1,D4)
    DBNE    D4,10$          ;Loop till we find a running timer
    BEQ.S   99$         ;=> none at all - Exit

* Decrement it and call code if it expired...
    SUBQ.B  #1,1(A1,D4)     ;Dec this timer
    BNE.S   20$             ;If non-0 go do next
    PUSH_L  D4/A1-A2        ;Stash essentials
    LSL.W   #1,D4           ;Index please
    MOVE.W  2(A2,D4),D4     ;Get relative offset for this timer
      
    IFNE    YCHEATMORE
    BEQ.S   BadTimer        ;no code to call?
    ENDC

    JSR     0(A2,D4)        ;Call the code concerned
    PULL_L  D4/A1-A2        ;Recover
20$
    DBRA    D4,10$          ;Next please - if any
99$
    RET


    IFNE    YCHEATMORE
BadTimer

    PRINT   <'^Waaa: bad timer - no code for timer #'>
    PULL_L  D4/A1-A2        ;Exit with a sensible stack!
    MOVE.W  D4,D6           ;PRTHEX takes D6
    ADDQ.W  #1,D6
    CALL    PRTHEX
    BRA LINE
    ENDC

*
* TimeTable -   Table that TIMERCOMMON looks at to suss what to call.
*
*           Entries in this table happen before room desc.
*
*   Format:     1 zero word of padding followed by wordwise
*           offsets from start of table to routine.
*

TimeTable

    DC.W    0               ;padding
    DC.W    BullCharge-TimeTable        ;Bull in field
    DC.W    Knocking-TimeTable      ;Postman knocking
    DC.W    PostMan-TimeTable       ;Second postman
    DC.W    MatchLife-TimeTable     ;Match life
    DC.W    CandleLife-TimeTable        ;Candle Life
    DC.W    PhoneCall-TimeTable     ;Librarian does his stuff?
    DC.W    MouseTrap-TimeTable     ;used to be Publican closes pub
    DC.W    ClosePODoor-TimeTable       ; Post office door closes
    DC.W    OfficeFloods-TimeTable      ;TIMER 9
    DC.W    DOPOISON-TimeTable      ;timer A CALLED FROM SYSTEM!!
    DC.W    DryOutGame-TimeTable        ;timer B
    DC.W    Do_Bus-TimeTable        ;timer C
    DC.W    CoolKey-TimeTable       ;timer D
    DC.W    0               ;timer E used to do siphon
    DC.W    WellMsg-TimeTable       ;timer F - things in well
    DC.W    BakerDescends-TimeTable     ;timer 10
    DC.W    Hospital1-TimeTable     ;timer 11
    DC.W    Patience-TimeTable      ;timer 12
    DC.W    SoberUp-TimeTable       ;timer 13
    DC.W    DoNothing-TimeTable     ;timer 14(NE = juke box on)
    DC.W    Expiresolution-TimeTable    ; timer 15
    DC.W    CloudGone-TimeTable     ;timer 16
    DC.W    NextTimeAppears-TimeTable   ; timer 17

*--------------------------------

*
* TimeTable_After - Table that TIMERCOMMON looks at to suss what to call.
*
*           Entries in this table happen after room desc
*           but before SPECIALS.
*
*   Format:     1 zero word of padding followed by wordwise
*           offsets from start of table to routine.
*

TimeTable_After

    DC.W    0               ;Padding
    DC.W    GuardianDrown-TimeTable_After   ;TIMERAFTER1
    DC.W    GuardianFalling-TimeTable_After ;TIMERAFTER2
    DC.W    Dancing-TimeTable_After     ;timerafter 3
    DC.W    Stationmaster-TimeTable_After   ;timerafter4
    DC.W    LoseTicket-TimeTable_After  ;timerafter5
    DC.W    Tourists-TimeTable_After    ;timerafter6
    DC.W    TooLateForTrain-TimeTable_After ;timerafter7
    DC.W    TrainWaitsAtStation-TimeTable_After ;timerafter8
    DC.W    TrainGuard-TimeTable_After  ;timerafter9
    DC.W    TrainJourney-TimeTable_After    ;timeraftera
    DC.W    TrainArrives-TimeTable_After    ;timerafterb
    DC.W    Hospital2-TimeTable_After   ;TIMERAFTERC
    DC.W    TrainArrivesHome-TimeTable_After    ;timerafterd
    DC.W    DoPost-TimeTable_After      ;TIMERAFTERE (post object)
    DC.W    GuardChuckOut-TimeTable_After   ;timerafterf
    DC.W    UnicornGoesMountain-TimeTable_After ;timerafter10
    DC.W    CandleBurnsRope-TimeTable_After ;timerafter11
    DC.W    StuckInOven-TimeTable_After ;TIMERAFTER12
    DC.W    ValidateEvent-TimeTable_After   ;TIMERAFTER13
    DC.W    EndEvent-TimeTable_After    ;TIMERAFTER14
    DC.W    CloseUpStation-TimeTable_After  ;timerafter15
    DC.W    HearVoices-TimeTable_After  ;TIMERAFTER16
    DC.W    WitchInChamber-TimeTable_After  ; timerafter17
    DC.W    MoveWitch-TimeTable_After   ; timerafter18
    DC.W    AnswerDoor-TimeTable_After  ;TIMERAFTER19
    DC.W    PostMistressPrompt-TimeTable_After  ;TIMERAFTER1A
    DC.W    0               ;TIMERAFTER1B
    DC.W    0               ;TIMERAFTER1C

*-----------------------------------------

*
*
* DOPOISON  -   Hard wired timer! Deep in the depths of eat/taste
*           or whatever, this timer gets set if the player
*           consumes something poisonous. Since we don't
*           have anything poisonous in this game, I'm
*           remming out the code.
*           Paul

DOPOISON 
    RET

*   CMP.B   #1,C.STATUS(A4)     ;FATAL POISON?
*   BNE.S   10$
*   FIXED_SCN Poison3       ;OK U DIE NOW
*   DOGO    DEAD
*10$
*   CMP.B   #2,C.STATUS(A4)     ;JUST POISONOUS
*   BNE.S   20$
*   FIXED_SCN Poison2       ;OK VERY ILL  (MAYBE SOMETHIN U ATE)
*   MOVE.B  #1,C.STATUS(A4)     ;OK DIE NEXT TIME
*   MOVE.B  #3,TIMERA(A4)
*   RET
*20$
*   FIXED_SCN Poison1       ;DONT FEEL TO GOOD..
*   RET
*

*-------------------------------


; Handle the charging of the bull
BullCharge
    CMP.W   #RNFIELD1,ROOM(A4)  ; Still in bull's room?
    BNE.S   99$         ; Naw! he ran away!
    ESCAPE  BULLCHARGE

99$:
    RET

*-------------------------------

    XDEF    OfficeFloods

OfficeFloods
    LEA FloodRooms(PC),A1   ;list of effective rooms
    MOVE.W  FloodPos(A4),D0
    BNE.S   05$         ;not the first
    CALL    SetWaterThere       ;put water in these rooms
05$
    MOVE.L  (A1)+,D1        ;fetch room & code
    BEQ.S   40$         ;not in list, continue flood
    CMP.W   ROOM(A4),D1     ;this one?
    BNE.S   05$         ;no, try next
    SWAP    D1          ;code to lo word
    ADD.W   D1,D0           ;adjust by code
    ASL.W   #1,D0

    LEA FloodList(PC),A0
    MOVE.W  0(A0,D0.W),D1

*************************************
*        MOVE.W FloodList(PC,D0.W),D1Modded to above 2 lines coz 6502 don't work
*************************************

    BPL.S   10$         ;not the end yet
    Connect LIBRARY2,TOPOF_LIBRARY2,U
    ST  AirlockFlooded(A4)
    ST  OfficeFlooded(A4)   ;flag as completely flooded
    CALL    SubmergeOffice
    BRA.S   90$         ;exit
10$:
    CMP.W   #146,D1
    BEQ.S   30$
20$:
    MOVE.W  D1,-(SP)
    SCN 139     ; Water is up to your...
    MOVE.W  (SP)+,D1
30$:
    DO  P.SCN       ; Knees/ankles/waist/whatever
40$:
    MOVE.B  #1,TIMER9(A4)
    ADDQ.W  #1,FloodPos(A4)
90$:
    RET

MaxFMessage EQU 7   ;# of messages

FloodList:
    DC.W    140,141,142,143,144,145,146,-1,-1
FloodRooms
    DC.W    0,RNAIRLOCK
    DC.W    0,RNCORRIDOR1
    DC.W    1,RNOFFICE
    DC.W    1,RNLIBRARY2
    DC.L    0

; Postman knocking for the first time...
Knocking
* Eds - Paul

    MOVE.W  KnockPos(A4),D0
    ASL.W   #1,D0           ; Get message number
;   MOVE.W  KnockList(PC,D0.W),D1
    LEA KnockList(PC),A1
    MOVE.W  0(A1,D0.W),D1
    BMI.S   10$         ; All done ?
    MOVE.B  #1,TIMER2(A4)       ; No, so restart timer
    ADDQ.W  #1,KnockPos(A4)     ; Increment message

    CALL    HouseMsg        ; Print message if in Xams house
    BRA.S   90$

10$
    MOVE.W  #NPOSTAL_NOTE,D0    ; Last time.... so
    DO  SETIN           ; put the postal note 'in'

90$
    RET

KnockList
    DC.W    40,41,42,-1

KnockList2
    DC.W    40,41,111,180,-1

*---------------

    XREF    MaxWaits

; Second visit - When he delivers package
PostMan

    MOVEQ   #-1,D6          ;Set up 'no spiel' flag
    CMP.B   #10,MaxWaits(A4)    ;Postman won't wait forever
    BLT.S   5$
    MOVE.W  #42,D1
    CALL    HouseMsg        ;Footsteps into distance
    BRA.S   72$         ;Go put parcel in mbox if poss

5$
    CLR.L   D6          ;Fod flag
    ADD.B   #1,MaxWaits(A4)     ;Bump count
    CMP.W   #RNKITCHEN2,ROOM(A4)    ;In Xam's kitchen?
    BNE.S   10$

    MOVE.W  #NKITCHEN_DOOR,D0   ;With open door?
    GETINFO
    TST_OPEN                ;Is it open?
    BEQ.S   10$
    SCN 52
* The postman stands in the doorway.

    BRA.S   70$         ; Go and do postman stuff
10$
    CMP.W   #RNFRONTGARDEN2,ROOM(A4)    ;Or in Xam's front garden?
    BEQ.S   60$

    MOVE.W  KnockP2(A4),D0      ; He's not met him yet, so next message
    ASL.W   #1,D0
;   MOVE.W  KnockList2(PC,D0.W),D1
    LEA KnockList2(PC),A1
    MOVE.W  0(A1,D0.W),D1
    BMI.S   15$         ;Just hang on last one
    ADDQ.W  #1,KnockP2(A4)      ; Increment message

    CALL    HouseMsg        ; Print message
15$
    MOVE.B  #1,TIMER3(A4)       ; Restart timer...
20$
    RET

60$
    SCN 51
* The postman walks into the garden from the orchard.

70$
    SCN 53
* "Oh," He says, a little taken aback. "I was expecting Xam to be here. This
* parcel is addressed to him, and it's more than my job's worth to give it
* to a stranger... Ah, I know, I'll pop it in his mailbox."^ Having resolved
* his dilemma, the postman bids you good-day and makes his way to the
* mailbox

72$
    MOVE.W  #NMAILBOX1,D0       ;Lock the mailbox
    GETINFO
    BSET    #6,(A0)         ;Lock it
    BNE.S   75$         ;=> already locked
    BCLR    #4,(A0)         ;!open
    TEST_W  D6          ;Want messages?
    BNE.S   74$         ;=> nope
    SCN 321
* . He places the package in the mailbox, closes it, and trots off into
* the distance.

74$
    MOVE.W  #NPARCEL,D0     
    DO  SETIN           ;Set the parcel 'in'
    BRA.S   90$

75$
    TEST_W  D6          ;Want messages?
    BNE.S   90$         ;=> nope
    SCN 322
* . Finding the mailbox locked, he shrugs his shoulders and saunters off with
* the package under his arm.

90$
    RET


*--------------------------

; cope with lit objects expiring completely...

MatchLife
    XREF    Denotation_MATCHES

* Cause all lit matches to expire

    PUSH_L  D0/A0
    LEA Denotation_MATCHES(A4),A0
10$
    MOVE.W  (A0)+,D0
    BEQ.S   90$
    MOVE.L  A0,-(A7)
    CALL    Expires
    MOVE.L  (A7)+,A0
    BRA.S   10$     ;next (if any)
90$
    PULL_L  D0/A0
    RET


CandleLife

    TEST_B  REMASTER(A4)
    BEQ.S   10$
    RET                         ; candle does not expire in remaster
10$    
    MOVE.W  #NCANDLE,D0
*   BRA.S   Expires     ;Fall through

Expires     
    GETINFO
    TST_LIT             ;
    BEQ.S   90$     ; Branch if not lit
    TST_OUT             ;already out?
    BNE.S   90$     ;yes, forget it
    DO  VALID.NP    ; See if player can see it...
    BNE.S   10$
    DO  P.TN        ; Yes, so 'The xyzzy'       
    SCN 72      ; ... 'has burnt away to nothing.'
10$:
    DO  SETOUT      ; Lose candle/match
90$:
*   RET         ; might as well save 2 bytes...

; Just used as a dummy timer routine, when TIMERn(A4) is used as a flag
DoNothing
    RET


; PhoneCall handles the ringing of the telephone...
    XREF    RingCount,PhoneAnswered,TelephoneRung

MaxRings    EQU 15

PhoneCall
    TEST_B  RingCount(A4)       ; If it's been answered, stop the timer.
    BEQ.S   90$
    ADDQ.B  #1,RingCount(A4)    ; Increment number of rings
    CMP.B   #MaxRings,RingCount(A4) ; All done ?
    BGE.S   10$         ; Yes, so another message, and abort
    MOVE.B  #1,TIMER6(A4)       ; Restart timer.
    MOVE.W  #303,D1         ; 'A telephone is ringing'
    BRA.S   20$
10$:
    MOVE.W  #304,D1         ; 'I hope it wasn't important'
    CLR.B   RingCount(A4)       ; Stop it ringing...

    TEST_B  REMASTER(A4)
    BEQ.S   30$
    CLR.B   TelephoneRung(A4)   ; it can ring again
    BRA.S   20$                 ;for remaster, we do not mark the phone answered, so timer6 is triggered again
30$    
    ST  PhoneAnswered(A4)   ; the phone has happened!
20$:
    CALL_S  HouseMsg1
90$:
    RET


* Eds - Paul

HouseMsg1
; print a message if we're around your house
    MOVE.W  #NYOUR_HOUSE,D0
    BRA.S   Msg1Entry

HouseMsg
; print a message if we're around Xam's house
    MOVE.W  #NXAMS_HOUSE,D0

Msg1Entry

    XREF    Waiting
    
    DO  VALID.NP    ; is item valid??
    BNE.S   10$     ; nope, so no message
    DO  P.SCN       ; do the message
    CLR.W   Waiting(A4) ;stop waiting
10$:
    RET



*
*
* Do_Bus    -   Handles the opening sequence on the bus.
*           Called by TIMERS.
*           First call is 3 moves into game.
*

    XREF    TIMERC,TimesCalled,AskedNTimes,RungBell,DoorsOpen
    XREF    NPCINRM
    XREF    TimesPressed,WhereToGetOff,LeaveBus

    XREF    BaseTMess       ;This is an EQUATE in scenrw.i



Do_Bus
    CMP.W   #RNONBUS,ROOM(A4)   ;Are we on the bus?
    BNE 90$         ;If not, straight out - never to return
    MOVE.B  TimesCalled(A4),D1  ;How many times we been here?
    BNE.S   10$         ;=> at least once!
* First call does this....

    ADDQ.B  #1,TimesCalled(A4)  ;Bump to keep track of events

    SCN 29
* Bus stops and the inspector gets on.

    MOVE.W  #NINSPECTOR,D0
    DO  SETIN           ;Set the inspector 'in'
    MOVE.W  D0,NPCINRM(A4)      ;So conversation gets to him
    DO  SETPRON         ;Set pronouns

    MOVE.B  #1,TIMERC(A4)       ;Prime timer for next time
    BRA 90$         ;Bye bye - till next move

10$
    SUBQ.B  #1,D1           ;Second time through?
    BNE.S   40$         ;=> nope
* Second call does this.... And keeps doing it until we've seen the ticket
* or been chucked off the bus.

    MOVE.W  AskedNTimes(A4),D1  ;We've asked for the ticket n times
    BMI.S   20$         ;=> We've been shown the ticket
    CMP.W   #BaseTMess+4,D1     ;Run out of spiel?
    BEQ.S   25$         ;=> that's it, chuck him off.
    DO  P.SCN           ;Do relevant message
    ADDQ.W  #1,AskedNTimes(A4)
    MOVE.B  #1,TIMERC(A4)       ;Prime for next move
    MOVE.W  #NTICKET,D0
    DO  SETPRON         ;So we can 'show him it'
    BRA 90$

20$
    MOVE.B  #2,TimesCalled(A4)  ;Force the loop
    MOVE.B  #2,TIMERC(A4)       ;Bus stop in 2 moves!
    BRA.S   85$

25$
    MOVE.W  #16,WhereToGetOff(A4)   ;Chucked off 1 stop from home
30$
    SCN 117
* Get thrown off the bus for not having a ticket
* or for fare-dodging

    BRA LeaveBus        ;LeaveBus lives in setmode


* Once ticket has been seen by the inspector, we loop around this bit
* until the player get's off the bus - voluntarily or otherwise!
40$

    TEST_B  RungBell(A4)        ;Have we rung the bell?
    BEQ.S   50$         ;=> Nope
    TEST_B  DoorsOpen(A4)       ;Doors open?
    BNE.S   45$         ;=> Yep
    SCN 118
* Bus stops, doors open

    MOVE.W  #NAUTODOORS,D0      ;Fix up doors
    DO  OpenDoor
    ST  DoorsOpen(A4)       ;Flag this fact
    MOVE.B  #1,TIMERC(A4)       ;If we're still here in 1 move,
                    ;doors close and we're off again!
    BRA.S   90$

45$
    SCN 272
* The doors close and the bus pulls away
    MOVE.W  #NAUTODOORS,D0
    DO  CloseDoor
    CLR.B   DoorsOpen(A4)       ;Tell the world
    CLR.B   RungBell(A4)        ;Got to ring it again for next stop
    CLR.B   TimesPressed(A4)    ;So SP.PRESS knows where it is
    BRA.S   55$

50$
    SCN 273
* Bus sails past a bus-stop

55$
    TEST_W  WhereToGetOff(A4)   ;Gone past our stop?
    BMI.S   30$         ;Miles back - chuck him off
    BNE.S   60$         ;=> Not there yet
    SCN 77
* Excuse me, you just missed your stop, mate. Better get off at the next.

60$
    MOVE.B  #5,TIMERC(A4)       ;5 moves till next stop!
85$
    SUB.W   #16,WhereToGetOff(A4)   ;Keep track of lane position
90$
    RET

*----------------------------


*
*
* CoolKey   -   Puts the plastic key's texture back to
*           hard rather than warm or hot.
*
*   NB  It's possible the key may be 'out' when this happens.
*       Bear this in mind when editing.
*

CoolKey
    MOVE.W  #NKEY1,D0
    GETINFO
    AND.B   #$F0,3(A0)      ;Save sharp etc
    OR.B    #TEXTURE.HARD,3(A0) ;Now hard again
    RET

*----------------------------

    XREF    GetWet,ROOM,R.NOUN2,FloodRoom
    XREF    NOSINKMSG

SubmergeOffice

    XREF    LibraryFlooded,UnderWater

    PUSH_L  D2/D4/A0-A1
    MOVE.W  #NRED_DOOR,D0       ;see if library floods as wel
    GETINFO
    TST_OPEN                ;is it open?
    SNE LibraryFlooded(A4)  ;if so library floods
    LEA FloodRooms(PC),A1
    CLR.L   D4          ;nargs
10$
    MOVE.L  (A1)+,D2        ;get a room
    BEQ.S   90$         ;exit
    CMP.W   ROOM(A4),D2     ;are you here?
    BNE.S   15$
    ST  UnderWater(A4)      ;and you are under water
    CALL    GetWet          ;you get wet
15$
    DO  R.NOUN2         ;the the room noun# ->d0
    GETINFO
    BSET    #5,3(A0)        ;becomes wet
    MOVE.W  D2,-(A7)        ;push room#
    CLR.W   -(A7)
    ADDQ.W  #1,D4           ;inc nargs
    BRA.S   10$         ;next room
90$
    SCN 324         ;office submerges
    MOVE.L  D4,-(A7)        ;and nargs
    CALL    FloodRoom       ;these rooms submerge
    ADDQ.W  #1,D4
    ASL.L   #2,D4           ;*4 for size
    ADD.L   D4,A7           ;adjust stack
    ST  NOSINKMSG(A4)       ;dont say that all items sank
    PULL_L  D2/A0-A1
    RET
    
*------------------------

*
*
* ClosePODoor   -   Tells specials to shut door.
*           Called one move after door opened.
*
*

    XREF    ShutTheDoor,IntroPMistress

ClosePODoor

    BSET    #7,ShutTheDoor(A4)      ;Don't fod this to a ST
    BSET    #7,IntroPMistress(A4)
    RET


WellMsg
    CMP.W   #RNBOTTOM_WELL,ROOM(A4) ; Check player is still in well...
    BNE.S   90$
    MOVEQ   #4,D1       
    DO  RND.D1
    LSL.B   #1,D1           ; convert to index.
    LEA WellMsgTable(PC),A1 ; get table address.
    EXT.W   D1          ; wordwise
    MOVE.W  0(A1,D1.W),D1       ; get message no.
    BEQ.S   80$
    DO  P.SCN           ; print it!
    XREF    Waiting
    CLR.W   Waiting(A4)
80$
    MOVEQ   #10,D1          ; re-set timer
    DO  RND.D1          ; in 10 moves...
    ADDQ.B  #7,D1           ; after at least 3
    MOVE.B  D1,TIMERF(A4)       
90$
    RET

WellMsgTable
    DC.W    385,386,387,388


**************************************

DryOutGame

* called when it is time for objects in the game to dryout
* GRRRR, cant just put:
*   DryOut-TimeTable, in the time table.

    BRA DryOut

; Which file is DryOut in, Hugh???
; it's in dryout.elt !!!! - Hugh.



    XREF    ISIN,BakerAngry,BakerFollows,GotCharm4,BakerFed
    XREF    QCARRIED,DROPHERE
    XDEF    BakerDescends
; This handles baker stomping down stairs into kitchen/larder
BakerDescends
    TEST_B  BakerAngry(A4)
    BNE.S   90$     ; Already turfed you out ??
    TEST_B  BakerFollows(A4); Already following???
    BNE.S   90$     ; if so, no action
    TEST_B  BakerFed(A4)
    BNE.S   90$     ; ... or if you fed him, then skip!
    SCN 390     ; The baker clumps down the stairs

    TEST_B  GotCharm4(A4)   ; got the charm yet?
    BEQ.S   20$     ; Yes, so baker has a scan round!
    SCN 391     ; Searches around...
    MOVE.W  #NCHARM4,D0 ; is the charm carried?
    CALL    QCARRIED    ; Carried?
    BEQ.S   BakerEject  ; No, so it's easy...
    SCN 393     ; Doesn't find it

20$:
    MOVE.W  #NBAKER,D0
    CALL    DROPHERE    ; Baker simply in room.
    ST  BakerFollows(A4); Baker now following you around
90$:
    RET


    XDEF    BakerEject,BakerEject2
    XREF    WHOOPS

BakerEject
    MOVE.W  #NCHARM4,D0
    MOVE.W  D0,STOPRON(A4)      ; theres an @ in SCN 392
    SCN 392         ; 'You thief! Get out of here!'
    DO  SETOUT          ; Lose charm
BakerEject2
    ST  WHOOPS(A4)      ; Lose anything else...
    ST  BakerAngry(A4)      ; Baker won't let you back
    CLR.B   BakerFollows(A4)    ; Stop him following you
    MOVEQ   #RNFAIRGROUND,D1    ; Chuck to fairground...
    DOGO    MovePlayer      ;put player there


;   XDEF    TOPOBJ
; TOPOBJ will return the top-level OBJECT (not room) that D0 is 
; related/contained by, etc
;TOPOBJ
;   GETINFO
;   TEST_B  6(A0)       ; Out??
;   BMI.S   85$
;   BEQ.S   99$
;   TEST_W  8(A0)       ; On inv?
;   BEQ.S   90$
;   BTST    #6,6(A0)
;   BNE.S   80$
    
;   MOVE.W  D0,D1
;   MOVE.W  8(A0),D0
;   BRA.S   TOPOBJ      ; No, so loop
;80$
;   MOVE.W  D1,D0       ; Set up last object...
;   RET
;85$
;   MOVEQ   #-1,D0      ; Out.
;   RET
;90$:
;   CLR.W   D0      ; On inv.
;99$:
;   RET


; RELOBJ will return TRUE (EQ) if object D2 is related/contained/nested, ... 
; to object D0
    XDEF    RELOBJ

RELOBJ
    PUSH_L  D0/D2/A0
    EXG D0,D2
10$
    CMP.W   D2,D0
    BEQ.S   90$
    GETINFO
    TEST_L  6(A0)       
    BLE.S   80$     ; Out, or on inventory?
    TST_ISAROOM     ; is it a room??
    BNE.S   80$
    MOVE.W  8(A0),D0
    BRA.S   10$     ; No, so loop
80$
    SETNE   
90$
    PULL_L  D0/D2/A0
    RET

*----------------------------

*******************************************************************************
    XREF    TIMER13,BoozedUp

; This handles sobering up the player after he's been drinking.
SoberUp
    SUBQ.B  #1,BoozedUp(A4)     ; Decrement the booze level
    BEQ.S   90$         ; If zero, he's now sober
    MOVE.B  #10,TIMER13(A4)     ; else restart timer
90$:
    RET

*******************************************************************************
*
*
* Patience  -   Guardian gives random spiel if you hang around
*           instead of reading document.
*
*   This gets initialized by whatever it is that gives you the document
*   and is cleared by whatever it is that removes the guardian.
*
*   NB Relies on no room change while guardian is about.
*

        XREF    LastSpiel

Patience

     IFNE   YCHEAT
    

* So that we don't get hassled if we use tele!

    CMP.W   #RNCOUNTRYLANE1,ROOM(A4)
    BEQ.S   5$
    CMP.W   #RNINFINITE1,ROOM(A4)
    BEQ.S   5$
    CMP.W   #RNINFINITE2,ROOM(A4)
    BNE.S   90$
5$
     ENDC

10$
    MOVEQ   #10,D1          ;RND.D1 rets with # 0 to n-1
    DO  RND.D1          ;Think of a number between 0 and 9...
    MOVE.B  D1,TIMER12(A4)      ;...and set this timer with it.
    ADDQ.B  #2,TIMER12(A4)      ;add two (NOT just for luck!)
    EXT.W   D1          ;Wordwise
    ASL.W   #1,D1           ;index please
    LEA Patience.TBL(PC),A1
    MOVE.W  0(A1,D1),D1
    CMP.W   LastSpiel(A4),D1    ;Same as last time?
    BEQ.S   10$         ;=> yeah, reject it, get another
    SCN 377         ;Do the preliminaries
* The guardian 

    MOVE.W  D1,LastSpiel(A4)    ;Remember for next time
    DO  P.SCN           ;...and print it!
    MOVE.W  #NDOCUMENT,D0       ;"read the wossname", "l wossname"
    DO  SETPRON

90$
    RET



* List of SCN #'s containing guardian's impatience spiel
* NB must be 10 entries - else change #10 above accordingly.

Patience.TBL

    DC.W    378,308,400,401,402,403,404,405,406,407

*------------------------------------------------------

*
*
* GuardianDrown     -   Copes with the drowning sequence where
*               the guardian rescues you.
*
*   Don't fiddle with this coz there are a million ways the player
*   can hassle this piece of code.
*

        XDEF    GuardianDrown

        XREF    EventClock,UnderWater,Aquaok
        XREF    EDROWNING,AquaTank,WEAR
        XREF    WearCheck,OutWater,GETWOSSNAME,OLDROOM
        XREF    REMASTER

GuardianDrown

    CALL    Aquaok
    BEQ.S   5$
    SCN 137
* You appear to be drowning.^

    BRA.S   8$
5$
    TEST_W  EventClock(A4)      ;If the bastard just took it off...
    BEQ ExitDrown       ;for one move, pretend nuthin happened
8$
    CALL    GETWOSSNAME     ;Always, since we may have moved
    BEQ.S   10$         ;=> same room as last call
    SCN 420
* The guardian cruises up alongside you.^

10$
    CALL    Aquaok          ;Has he fixed his probs?
    BEQ EscapedAlone        ;=> yep, player escaped alone
    MOVE.W  EventClock(A4),D1   ;How many times we been here?
    BNE.S   20$         ;=> already happened at least once
    SCN 421
* "Problems, chief?" he says, nonchalantly.

    BRA.S   ResetTimerAfter1    ;Reset timer for next move

20$
    TEST_W  AquaTank(A4)        ;Is there any air left?
    BEQ.S   25$         ;=> nope, he's in trouble
    MOVE.W  #NAQUALUNG,D0
    DO  SETOUT          ;Fix tied etc just in case
    GETINFO
    BCLR    #7,6(A0)        ;..and back again
    SCN 442
* "Helps if you use this!" says wossname, helping you on with the aqualung.

    CALL    WEAR            ;Easy way! (This MUST come back!)
* WEAR cops out if it's fixed - hence above. Apart from that it should
* all just work!!!
    BRA.S   ExitDrown       ;Clean up and ret

25$
    SUBQ.W  #1,D1           ;Dec count of calls
    BNE.S   30$
* This happens on the second call
    SCN 432
* Guardian spiel about your face turning blue

    BRA.S   ResetTimerAfter1
30$
* Third call...

    SCN 11
* Tucking you under his arm, the guardian makes his way back to the
* surface. He drops you at the pagoda, dives gracefully back into the
* lagoon and swims off into the distance.

    TEST_B  REMASTER(A4)
    BEQ.S   35$
    SCN     825
    MOVE.W  #100,AquaTank(A4)   ; for remaster refill the aquatank after guardian to the rescue
35$
    CALL    OutWater        ;Clears the U/water flag
    MOVE.W  #EDROWNING,D0       ;Tell ExitCommon how to exit
    MOVEQ   #RNPAGODA,D1
    CLR.W   OLDROOM(A4)     ;In case player jumps straight back in
    BRA ExitCommon      ;Tidy up (Implied RET)

ResetTimerAfter1
    ADDQ.W  #1,EventClock(A4)   ;Bump # of calls
    MOVE.B  #1,TIMERAFTER1(A4)
    RET


EscapedAlone

* If we get here it means the player got out of it all by himself!

    SCN 422
* (Guardian spiel along the lines of "saves me the bother of rescuing you")

ExitDrown
    CLR.W   OLDROOM(A4)
    CLR.W   D0          ;Tell ExitCommon how to exit
    CLR.W   D1
    CLR.W   D2
    BRA ExitCommon      ;Exit neatly (implied RET)

*-----------------------------

*
*
* GuardianFalling   -   Copes with player falling from girder
*               in clock tower and being rescued by
*               wossname.
*               Probably end up as general 'falling'
*               escape.

    XREF    EDARINGLEAP


GuardianFalling

    MOVE.W  EventClock(A4),D1   ;How many times this has happened
    BNE.S   10$         ;=> at least once
    CALL    GETWOSSNAME     ;My hero
    MOVE.W  #NPARACHUTE,D0
    DO  SETIN
    SCN 444
* A familiar figure, dangling below the canopy of a parachute, swings
* into view beside you. "Long way down, innit?" He remarks helpfully.
    BRA.S   ResetTimerAfter2

10$
    SUBQ.W  #1,D1           ;Dec call times
    BNE.S   20$
    SCN 445
* "Interest you in a parachute?"

    BRA.S   ResetTimerAfter2

20$
    SCN 446
* "Naw, only kidding" (Straps a parachute to your back)
* You land, parachute disappears.

    MOVE.W  #NPARACHUTE,D0
    DO  SETOUT
    MOVE.W  #EDARINGLEAP,D0     ;Tell ExitCommon how to exit
    MOVEQ   #RNCLOCK,D1
    BRA ExitCommon      ;Clean up neatly (Implied RET)

ResetTimerAfter2
    SCN 443
* ^The ground is getting closer.

    MOVE.B  #1,TIMERAFTER2(A4)  ;Prime for next move.
    ADDQ.W  #1,EventClock(A4)
90$
    RET

*-----------------------------------

*
*
* Hospital  -   ESCAPE mechanism for bashing your head
*           on the low tunnel as the train goes under
*           it.
*

    XREF    TIMER11,TIMERAFTERC,TIMERAFTER3

Hospital1               ;This is a TIMER......

    CLR.B   TIMERAFTER3(A4)     ;Incase we're tapdancing!
    CLR.B   TapDancing(A4)
;   CLR.W   DracItem(A4)        ;This is done b4 we get here now
* Wotchit! There may be other nasties like this!!

    CALL    GETWOSSNAME     ;My hero
    SCN 522
* You wake up expecting to find yourself dead etc

    MOVE.B  #2,TIMERAFTERC(A4)  ;Kick off hospital2
    RET

*----------------

Hospital2               ;...And this is a TIMERAFTER

    MOVE.W  EventClock(A4),D1   ;How many times this has happened
    CMP.W   #4,D1           ;Done max message
    BLT.S   10$         ;=> not yet
    CALL_S  20$         ;Do last message

* Bung player back at station and clean up
    SCN 536
* Indeterminate amount of time later, you regain consciousness.^
    CALL    SetTrainBack        ;Re-init the train stuff

    MOVE.W  #EHITTUNNEL,D0
    CLR.W   D1          ;no player moving as SetTrainBack did it
    BRA.S   ExitCommon      ;Implied RET


10$
    MOVE.B  #1,TIMERAFTERC(A4)  ;Prime for next move
    ADDQ.W  #1,EventClock(A4)   ;Bump count of calls to routine
20$
    ADD.W   #523,D1         ;Base msg #
    CMP.W   #526,D1         ;Yuk!
    BNE.S   30$
    MOVE.W  #NWASWOSSNAME,D0    ;So 'look wossname' works!
    DO  SETPRON
30$
    DO  P.SCN
    CALL    WeirdNoises     ;machine goes bleep?
    RET

*--------------------------

*
* ExitCommon    -   Common exit code for guardian timers.
*
*   TAKES:      D0 as Event identifier (0=> don't dec luck for event)
*              NB This means event 0 won't do it!!!!
*           D1 as room # to move to (0 => no room change)
*

        XREF    DecLuck,GetTableEntry,P.Refresh

ExitCommon

    TEST_W  D1          ;Need a room change?
    BEQ.S   10$         ;=> nope
    DO  MovePlayer      ;& change location
10$
    TEST_W  D0          ;Do luck decrement?
    BEQ.S   20$         ;=> nope
    CALL    GetTableEntry       ;Original LUCK.TBL entry to D1
    CALL    DecLuck         ;Lose appropriate luck points
20$
    CALL_S  FODWOSSNAME     ;Bye bye
    CLR.W   EventClock(A4)      ;Fod multi-purpose counter
    RET

*---------------------------------

    XDEF    FODWOSSNAME

FODWOSSNAME

    MOVE.W  #NGUARDIAN,D0       ;Lose wossname
    DO  SETOUT
    RET

*---------------------------------



WeirdNoises

    MOVE.W  D1,-(A7)        ;Scratch
    MOVEQ   #4,D1
    DO  RND.D1
    MOVE.W  D1,D2
    BEQ.S   90$
    SCN 537
* ^The machine next to the operating table goes 

    ADD.W   #527,D1         ;Beep beep messages
10$
    DO  P.SCN
    SUBQ.W  #1,D1
    DBNE    D2,10$
    DO  BSFULLSTOP
    MSG LINE
90$
    MOVE.W  (A7)+,D1        ;Recover
    RET

*########################################################


    XREF    TapDancing

Dancing
    SCN 468     ;spell wears off
    CLR.B   TapDancing(A4)
    RET
*------------------------------------

    XDEF    Stationmaster
    XREF    TapDancing,PlayHarmonica
    XREF    BuskableRoom
    XREF    Stationmaster_givein,GivenCoin
    XREF    TIMERAFTER15,TriedToBuy

Stationmaster

    TEST_B  Stationmaster_givein(A4)    ; redone
    BNE.S   90$
    TEST_B  GivenCoin(A4)       ;been given coin?
    BEQ.S   90$         ;no, dont close just yet
    TEST_B  TapDancing(A4)
    BEQ.S   80$         ;loose it
    TEST_B  TriedToBuy(A4)      ;have you attempted to buy?
    BEQ.S   80$         ;if not he doesnt come out
    CALL    BuskableRoom        ;room ok
    BNE.S   80$         ;too bad
    SCN 469         ;station master cant take it
    ST  Stationmaster_givein(A4)
80$ 
    TEST_B  REMASTER(A4)
    BNE.S   90$                 ; does not close shop for remaster
    MOVE.B  #7,TIMERAFTER15(A4) ;he will close anyway in 7 moves
90$
    RET

*---------------------------------------

    XREF    LoseTicketState

LoseTicket

    SCN 472         ;wind blows ticket
    MOVE.W  #NTRAIN_TICKET,D0
    GETINFO
    CLR.W   6(A0)
    MOVE.W  #RNPLATFORM1,8(A0)  ;it is now on the platform
    MOVE.B  #1,LoseTicketState(A4)  ;1st point
    RET
*-----------------------------------------

    XREF    BeginBusking,BuskableRoom
Tourists

    CALL    BuskableRoom        ;this room ok?
    BNE.S   90$         ;nope
    ARGL    1           ;always print mode
    CALLS   4,BeginBusking
    CALL    Stationmaster
90$
    RET

*-----------------------------------------

    XREF    AtStation,TIMERAFTERA,TrainMovesOff
    XREF    Waiting

TooLateForTrain

    MOVE.B  #5,TIMERAFTERA(A4)      ;begin journey
    CALL    AtStation   ;only get message if player is around
    BNE.S   90$     ;no exit
    CALL    AboardTrain
    BEQ.S   99$     ;if so forget it
    SCN 491     ;too late, uve missed the train
    CLR.W   Waiting(A4)
90$
    MOVE.W  #NLOCOMOTIVE,D0
    DO  SETOUT
99$
    ST  TrainMovesOff(A4)
    RET

*-----------------------------------------

    XREF    AtStation,TrainLeaves

TrainWaitsAtStation

* When triggerd, the player has hung around too long for the train
* and it will have to leave

    CLR.W   D0          ;print mode
    CALL    AtStation   ;we'll let the player know if he is
                        ;anywhere near the station
    BNE.S   10$         ;no, then no-print mode
    ST  D0              ;otherwise printmode on
10$
    ARGD    D0          ;set print mode
    CALLS   4,TrainLeaves
    RET

*-----------------------------------


AboardTrainRooms

    DC.B    RNON_TRAIN
    DC.B    RNTRAIN_ROOF
    DC.B    RNGUARDS_VAN
    DC.B    0
    ALIGN

    XDEF    AboardTrain
    XREF    LookUpRoomInTable

AboardTrain

    MOVE.L  A1,-(A7)
    LEA AboardTrainRooms(PC),A1
    CALL    LookUpRoomInTable
    MOVE.L  (A7)+,A1
    RET


*---------------------------------------  

    XREF    GuardWantsTicket,P.SCN
    XREF    TIMERAFTER9,GuardSeenTicket
    XREF    TIMERAFTERD,TrainMovesOff

TrainGuard

    PUSH_L  A0-A1
    LEA GuardWantsTicket(A4),A1
    CALL    AboardTrain
    BEQ.S   03$         ;ok
    CLR.B   (A1)            ;start again
    BRA.S   90$         ;now exit
03$
    TEST_B  GuardSeenTicket(A4)
    BNE.S   90$         ;seen it
    CLR.W   Waiting(A4)
    TEST_B  (A1)            ;first time?
    BNE.S   01$         ;no
    SCN 492         ;train guard arrives
    MOVE.W  #NGUARD,D0
    DO  SETPRON         ; make him it/him
    GETINFO
    MOVE.L  #REL.MASK+NON_TRAIN,6(A0); related to this location
    BRA.S   02$         ;inc counter and set-up asking

01$
    MOVE.W  #493-1,D1   ;get base message
    ADD.B   (A1),D1     ;get message
    CMP.B   #3,(A1)     ;done all
    BHS.S   10$     ;yes, throws you off into van
    DO  P.SCN       ;print it
02$
    ADDQ.B  #1,(A1)     ;inc it
    MOVE.B  #1,TIMERAFTER9(A4)  ;keep going
    BRA.S   90$     ;done
10$
    TEST_B  TrainMovesOff(A4)   ;is train moving?
    BNE.S   11$     ;yes throw u into van
    SCN 519     ;pushes you off train
    MOVEQ   #RNPLATFORM1,D1 ;u r on plat again
    CALL    MoveAndRefresh
    MOVE.B  #1,(A1)     ;set to start again+1
    BRA.S   90$     ;exit
11$
    SCN 496         ;throws you into van
    CALL    ThrowYouInVan
90$
    PULL_L  A0-A1
    RET

ThrowYouInVan

    MOVE.B  #5,TIMERAFTERD(A4)  ;before train arrives back
    MOVEQ   #RNGUARDS_VAN,D1
    BRA MoveAndRefresh      ;& put player there
*---------------------------------------

    XREF    TrainOnRoute,P.SCN
    XREF    LieDownOnTrain,ROOM,TapDancing

TrainJourney

* each time this timer is triggered is signifies the next stage of
* the train journey.
* The stage is held in TrainOnRoute

    CMP.W   #RNTRAIN_ROOF,ROOM(A4)
    BNE.S   90$
    CLR.W   Waiting(A4)
    TEST_B  TapDancing(A4)      ;tap dancing on train roof
    BNE DancingOnTrain
    CLR.W   D1
    MOVE.B  TrainOnRoute(A4),D1
    CMP.B   #3,D1           ;do you hit or go under
    BHI.S   30$         ;next stage
    BEQ.S   10$         ;find out
    ADD.W   #515,D1         ;get message
    DO  P.SCN
09$
    ADDQ.B  #1,TrainOnRoute(A4) ;next message
    MOVE.B  #1,TIMERAFTERA(A4)  ;and come back next move
    BRA.S   90$         ;exit
10$
    TEST_B  LieDownOnTrain(A4)  ;are you face down?
    BEQ.S   20$         ;you die(a bit)
    SCN 518         ;under tunnel
    BRA.S   09$         ;stil continue (for bend)
90$
    RET

20$
    ESCAPE  HITTUNNEL       ;whack!!
    BRA.S   90$         ;exit clean

30$
    CMP.B   #5,D1           ;out of tunnel
    BNE.S   31$
    SCN 181         ;whoosh, out of tunnel
    BRA.S   09$         ;next
31$
    CMP.B   #8,D1           ;a few moves gap.
    BLT.S   09$         ;wait a while
    ADD.W   #185-8,D1       ;to base
    MOVE.W  D1,-(A7)
    DO  P.SCN           ;print the message
    MOVE.W  (A7)+,D1
    CMP.W   #187,D1         ;last message?
    BLT.S   09$         ;no, continue

40$         ;when u fall off, all things are lost

    CALL    FodStuffHere
    ESCAPE  FALLOFFTRAIN
    BRA.S   90$         ;ret
    
*------------------------------------
    XREF    TIMERAFTERF,Waiting,StartClock

TrainArrives

    CLR.W   Waiting(A4)
    CMP.W   #RNGUARDS_VAN,ROOM(A4)  ;r u in the van
    BNE.S   10$         ;no
    SCN 241         ;guard lets u out
    MOVEQ   #RNTOP_MOUNTAIN,D1
    CALL    StartClock      ;coz this doesn't go through setmode
    CALL    MoveAndRefresh
*   MOVE.W  #RNTOP_MOUNTAIN,ROOM(A4)    ;u r out
    BRA.S   20$
10$
    SCN 188         ;arrive on mountain
    MOVE.B  #1,TIMERAFTERF(A4)  ;guard chucks u out
20$
    Connect     ON_TRAIN,TOP_MOUNTAIN,W
    Connect     TOP_MOUNTAIN,ON_TRAIN,E
    RET

TrainArrivesHome

    CLR.W   Waiting(A4)
    SCN 208         ;boots you off

SetTrainBack

    XDEF    SetTrainBack

    XREF    ROOM,GuardSeenTicket,GuardWantsTicket
    XREF    TIMERAFTER8,PlayerInVan,TrainOnRoute
    XREF    TrainMovesOff

    MOVE.W  #NLOCOMOTIVE,D0 
    DO  SETIN           ;set in (back at station)_
    Connect PLATFORM1,ON_TRAIN,E
    Connect ON_TRAIN,PLATFORM1,W
    MOVE.W  #RNTRAIN_ROOF,ROOM(A4)  ;fake u r on roof
    CALL    FodStuffHere
    MOVE.B  #10,TIMERAFTER8(A4) ;set train to leave again
    CLR.B   GuardSeenTicket(A4)
    CLR.B   GuardWantsTicket(A4)
    CLR.B   PlayerInVan(A4)
    CLR.B   TrainOnRoute(A4)
    CLR.B   TrainMovesOff(A4)
    MOVEQ   #RNPLATFORM1,D1 ;you are back
    BRA MoveAndRefresh

;******************************************************************************
; Make things be posted!

    XREF    PostItem,PostDest,PUTD0IND5
DoPost
    CMP.W   #RNPOSTOFFICE,ROOM(A4)
    BNE.S   10$
    MOVE.W  PostItem(A4),STOPRON(A4)    
    SCN 500
10$
    MOVE.W  PostItem(A4),D0     
    GETINFO
    MOVEQ   #0,D5           ; clear top 16 bits
    MOVE.W  PostDest(A4),D5     ; stick the dest room in low.
    CMP.B   #RNCOUNTRYLANE2,D5  ;to mailbox
    BNE.S   20$
    MOVE.W  #NMAILBOX,D5
15$
    XREF    PUTD0IND5_2
    CALL    PUTD0IND5_2     ;if it fits
    BRA.S   80$         ;quit
20$
    CMP.B   #RNFRONTGARDEN2,D5
    BNE.S   30$
    MOVE.W  #NMAILBOX1,D5
    BRA.S   15$
30$
; Simple destinations...
    MOVE.L  D5,6(A0)        ;there!

; score for the saddle
    CMP.W   #NSADDLE,D0     ;If posting saddle to station..
    BNE.S   80$         ;then score.
    CMP.W   #RNPOST_DEPOT,D5
    BNE.S   80$
    SCORE   POSTSADDLE
80$:
    CLR.W   PostItem(A4)        ; nothing to be posted...
    RET

*---------------------------------------

    XDEF    GuardChuckOut
    XREF    ROOM

GuardChuckOut

    CMP.W   #RNON_TRAIN,ROOM(A4)    ;r u in the train?
    BNE.S   90$         ;no, exit
    CLR.W   Waiting(A4)
    SCN 250     ;guard chucks you out
    MOVEQ   #RNTOP_MOUNTAIN,D1
    CALL    StartClock      ;Coz this doesn't go through setmode
    CALL    MoveAndRefresh
90$
    RET

*----------------------------------------

    XREF    ROOM,EntryLongLook,Waiting
    
UnicornGoesMountain

    MOVE.L  #RNTOP_MOUNTAIN,D1      ;target room
    CMP.W   ROOM(A4),D1         ;are u there too?
    BNE.S   10$             ;nope.
    SCN 255             ;unicorn lowers & grazes.
    CLR.W   Waiting(A4)
10$
    MOVE.W  #NUNICORN2,D0
    GETINFO
    MOVE.L  D1,6(A0)            ;move unicorn to mountain
    MOVE.L  #256<<16+NUNICORN2,D1       ;longlook entry
    CALL    EntryLongLook           ;uncrn is grazing nearby.
    RET

*----------------------------------------

    XDEF    CandleBurnsRope

CandleBurnsRope

    XREF    ROOM,CandleUnderRope
    XREF    ObjectUnderRope,FGT_Rope
    XREF    RopeTied,CloseDirection
    XREF    TIMERAFTER11

    MOVE.W  #NCANDLE,D0
    GETINFO
    TST_LIT             ;is it alight
    BEQ.S   90$         ;no, nothing happens
    TEST_B  CandleUnderRope(A4) ;is it underneath?
    BEQ.S   90$         ;no, nhap
    TEST_B  REMASTER(A4)
    BEQ.S   05$
    CMP.W   #RNIN_HATCH,ROOM(A4)    ;crawled into the hatch?
    BEQ.S   05$         ;yes, go ahead
    MOVE.B  #2,TIMERAFTER11(A4) ;no, keep burning timer running
    BRA.S   90$
05$
    SCN 552         ;rope burns
    CLR.W   ObjectUnderRope(A4)
    MOVE.W  #NDUNGEON_ROPE,D0
    DO  SETOUT          ;fod it
    MOVE.W  #-1,FGT_Rope+2(A4)
    TEST_B  RopeTied(A4)        ;is rope tied?
    BEQ.S   90$         ;no then tough!
    CMP.W   #RNDUNGEON,ROOM(A4) ;stil in the dungeon
    BNE.S   10$         ;no
    SCN 553         ;hatches closes
    MOVE.W  #NINNER_HATCH,D0
    MOVE.W  #nway_hatch,D2
    CALL    CloseDirection      ;close it
    BRA.S   90$         ;exit
10$
    CMP.W   #RNIN_HATCH,ROOM(A4)    ;crawled into the hatch?
    BNE.S   90$         ;no, exit
    SCN 554         ;hatch opens
    MOVE.W  #NINNER_HATCH,D0
    MOVE.W  #nway_hatch,D2
    CALL    CloseDirection      ;close it
    MOVE.W  #NOUTER_HATCH,D0
    DO  OpenDoor        ;you are free
90$
    RET

*------------------------------------

SetWaterThere

    PUSH_L  D0/A0
    MOVE.W  #NWATER2,D0
    DO  SETIN
    PULL_L  D0/A0
    RET

*-------------------------------

    XREF    EOVEN,DemolishedBakery

BaseOvenMSG EQU 714     ;contiguous messages
NumOvenMSGs EQU 5

    XREF    BakeryLook.MSG

StuckInOven

    MOVE.W  EventClock(A4),D1
    ADD.W   #BaseOvenMSG,D1         ;Plus message base
    CMP.W   #BaseOvenMSG+NumOvenMSGs-1,D1   ;Done 'em all?
    BLE.S   10$             ;=> nope, do it
    SCN 719
* You land on the guardian. He crawls away

    ST  DemolishedBakery(A4)        ;Bakery exploded
    MOVE.W  #644,BakeryLook.MSG(A4)     ;change desc.
    MOVE.W  #EOVEN,D0           ;Tell exitcommon how to exit
    MOVEQ   #RNFAIRGROUND,D1
    ST  D2
    BRA ExitCommon          ;Implied RET

10$
    BNE.S   20$
    MOVE.W  #RNFALLING,ROOM(A4)     ;In mid air
20$
    DO  P.SCN
30$
    ADDQ.W  #1,EventClock(A4)       ;Bump
    MOVE.B  #1,TIMERAFTER12(A4)     ;Reset timer for nxt move
    RET

*------------------------------------------

DancingOnTrain
    SCN 162     ;he throws you into the van
    CALL    ThrowYouInVan
    RET


*------------------------------------------

    XREF    EventRoom,EventNum,Do_Event

ValidateEvent

    MOVE.W  EventRoom(A4),D2    ;Room where this event was trigerd
    CMP.W   ROOM(A4),D2     ;Still there?
    BNE.S   90$         ;=> Nope, musta moved, exit
    MOVEQ   #2,D1           ;Will need tweeking
    DO  RND.D1          ;0-1 in D1.B
    CMP.B   #0,D1           ;Gonna use it?
    BNE.S   90$         ;=> nope, exit
    MOVE.W  EventNum(A4),D0     ;This event's id
    CALL    Do_Event        ;Go do the event
90$
    RET

*----------------------------------

*
* EndEvent  -   Multi-purpose timer to terminate guardian
*           events. Susses which one it's trying to
*           terminate and does it.

    XREF    EndEventMSG,FodLongLook

EndEvent

    CLR.L   D1          ;Yuk
    MOVE.W  ROOM(A4),D1
    MOVE.W  #NGUARDIAN,D0
    GETINFO
    CMP.L   6(A0),D1        ;Make sure he's here
    BNE.S   10$         ;Nope, maybe player moved or summit
    MOVE.W  EndEventMSG(A4),D1  ;Message #
    DO  P.SCN           ;Print the message
10$
    DO  FodLongLook     ;Lose trailing text
    CALL    FODWOSSNAME     ;Adios amigo
    RET


*----------------------------------

    XREF    StationMasterCloses

CloseUpStation

    BRA StationMasterCloses

*------------------------------------------

FodStuffHere

    XREF    GETLITERALS,FodList

* removes all the objects literally in the ROOM


    CALL    GETLITERALS     ;->a1
    CALL    FodList
    RET

*------------------------------------------
    XREF    SP.FreezeBottle,GetAllIn
Expiresolution
* once the fridge has been 'closed' for 20 moves with solution in it
* it get's here.
    MOVE.W  #NFRIDGE,D0
    GETINFO         ; make sure it is still closed.....
    TST_CLOSED              ; NE => closed
    BEQ.S   99$     ; no no no go away.
    MOVE.W  D0,D2
    CALL    GetAllIn    ; get all in returns a stack...
    SF  D7
10$
    CMP.W   #NSOLUTION,(A7) ; solution...?
    BNE.S   20$
    ST  D7      ; flag to say okay..
20$
    TEST_W  (A7)+
    BNE.S   10$     ; not fini.
    TEST_B  D7
    BNE SP.FreezeBottle
99$
    RET

    XREF    CloudAround,TIMER16,FrozenCloud
    XDEF    CloudGone
CloudGone:
* simply removes the cloud...
    TEST_B  FrozenCloud(A4)     ; if the cloud is frozen
    BNE.S   99$         ; don't do anything.
    CMP.W   #NNICECLOUD,SEATED(A4)  ; are we seated ont he cloud?
    BNE.S   05$
    MOVE.B  #1,TIMER16(A4)      ; again...
    RET
05$
    CLR.B   CloudAround(A4)     ; the cloud is now gone here..
    MOVE.W  #NNICECLOUD,D0
    DO  VALID.NP        ; are we local.
    BNE.S   10$         ; not local
    SCN 87          ; cloud disappears
10$
    DO  SETOUT          ; get rid of the cloud...
99$
    RET
    XREF    SP.Vanquished,TIMER17,COUNTEXIT,SEATED_RELATOR,R.NOUN2
    XREF    ScenVanquished
NextTimeAppears:
*** the next time you either get out of a hiding place...
*** Or. you open a door. the witch appears and vanquishes you.
* okay are you hidden?
* under?
    CMP.W   #UNDER,SEATED_RELATOR(A4)   ; if we are under something.
    BEQ.S   80$             ; we are hidden.
    CMP.W   #BEHIND,SEATED_RELATOR(A4)  ; or if we are behind...
    BEQ.S   80$             ; we are hidden
    CMP.W   #RNCHIMNEY,ROOM(A4)     ; in the chimney?
    BEQ.S   80$             ; not there...
    MOVE.W  #NWITCH,D0          ; get the witches room no.
    GETINFO
    MOVE.W  8(A0),D0        
    MOVE.W  D0,D5               ; save it away
* does the room we are in have any exits?
    CALL    COUNTEXIT
    BEQ.S   80$             ; no exits...
    BRA ScenVanquished          ; you are vanquished
80$
    MOVE.B  #1,TIMER17(A4)          ; you have no chance..
90$
    RET
    

*-------------------------

*
* Hearvoices    -   Simply prints a scn msg


HearVoices

    SCN 757
* The sound of raised voices can be heard beyond the doors.

    RET

*-------------------------
* This will trigger off the witch vanquishing you after you have got into
* the chamber... (This is assuming that you are still in the chamber...)
WitchInChamber:

*** okay we got here.. now what (Witch is assumed on the balcony).
* Are we in the Witches chamber still...
    CMP.W   #RNWITCH_CHAMBER,ROOM(A4)   ; are we still in there.

    BEQ.S   10$
* we are not here. if we havn't sussed the puzzle in 30 moves the witch
* will kill you
    MOVE.B  #30,TIMER17(A4)     ; get's cleared by other puzzle.
* Okay if we are not here any more let us put the witch there...
5$
    RET
10$
    TEST_W  SEATED(A4)      ; if we are seated good chance we hide
    BEQ.S   20$         ; yuppie. go and do a next time...
    MOVE.B  #1,TIMER17(A4)      ; set up the next time appears time
    RET
20$
    MOVEQ   #RNBALCONY,D2       ; for the @
    BRA ScenVanquished      ; .... you are gone...


    XREF    WitchPlace,STOPRON,SEATED_RELATOR,VALID.NP
    XREF    TIMERAFTER18,CurrentFuture,FutureEvents
    XREF    WitchBeen,Vanquished
    
MoveWitch:
    
    MOVE.W  WitchPlace+2(A4),D2 ;witch place index.
    LEA WitchBizz(PC),A3    ; start of table
    ADD.W   D2,A3           ;add offset.
10$
*** A3 points at the witch.
    CLR.W   D2          ; our new room.
    MOVE.B  (A3)+,D2        ; the room.
    MOVE.W  #NWITCH,D0
    GETINFO
    MOVE.W  D2,8(A0)        ; okay she is there
    DO  VALID.NP        ; NE = > not around
                    ; EQ = > here.
    BNE.S   20$         ; not here...
    CMP.W   #BEHIND,SEATED_RELATOR(A4)  ; are we behind things?
    BEQ.S   20$
    CMP.W   #UNDER,SEATED_RELATOR(A4)   ; or under things
    BNE ScenVanquished          ; You are dead.
20$
*** we get here if you are either in the room and hidden or
*** you are not here...
*   NOW How about that ball?
    MOVE.W  #NCRYSTALBALL,D0
    GETINFO
    CMP.W   8(A0),D2            ; simply in the room?
    BNE.S   30$             ; not around
    CALL    SETOUT
* okay you are going to die guys....
25$
    MOVE.B  #1,TIMER17(A4)          ; next timer appears...
    RET
30$
** No ball Maybe you - now what chaps...
* in the Kitchen (end of all things...)
    CMP.B   #RNCASTLE_KITCHEN,D2
    BNE.S   40$
    MOVE.B  #2,TIMER17(A4)          ; soon... to soon
    RET
40$
** in the witches chamber?
    CMP.B   #RNBANQUET_HALL,D2      ; in the banquet hall...
    BEQ.S   50$
45$
    MOVE.L  A0,-(A7)
    LEA WitchBizz(PC),A0
    SUB.L   A0,A3
    MOVE.L  A3,WitchPlace(A4)       ; set it back up
    MOVE.L  (A7)+,A0
    MOVE.B  #1,TIMERAFTER18(A4)
    RET
50$
*** Ouch we have stumbled into the banquet hall......
* is the future the fireplace?
    MOVE.W  CurrentFuture(A4),D5            
    LEA FutureEvents(A4),A5     ; pointer to nouns.
    MOVE.W  0(A5,D5.W),STOPRON(A4)      ; set up for @
    BSET    #6,STOPRON(A4)          ; NO trailing space
    CMP.W   #8,D5               ; our fireplace...?
    BEQ.S   60$
55$
    CALL    CrashInKitchen
    TEST_B  D7
    BNE.S   45$             ; you aren't around.
    SCN 771             ; she ....
    BRA.S   45$             ; and continue..
60$
*** It is the fireplace...
* now are you in the chimney
    CALL_S  AshesonHearth
    BNE.S   55$             ; no ashes go as normal
    CMP.W   #RNCHIMNEY,ROOM(A4) 
    BEQ.S   70$
* you are not in the chimney.....
    BSET    #0,WitchBeen(A4)        ; been here before?
    BNE.S   62$             ; yes...
    TEST_B  D7              ; around?
    BNE.S   61$
    SCN 510             ; she feels around
61$
    CALL    CrashInKitchen
    SUBQ.L  #1,A3               ; for next go.
    BRA.S   45$
62$
* been here before.
    TEST_B  D7
    BNE.S   45$             ; not around to see actioin
    SCN 769
    BRA.S   45$
70$
** okay we are in the chimney.....
    BSET    #0,WitchBeen(A4)        ; we have been here...?
    BNE.S   80$             ; ohoh you are gonna die.
    MOVE.W  #NWITCH_HAND,D0
    DO  SETIN
    SCN 773             ; a grisly hand appears.
    BRA.S   61$
80$
    SCN 774             ; tough - you go. your luck runs out!
    BRA Vanquished          ; gone......
    

AshesonHearth

* EQ => YES

    PUSH_L  D1-D2/D5/A1
    TEST_B  REMASTER(A4)
    BEQ.S   50$                 ; old version
    MOVE.W  #NHEARTH,D2
    DO  GETRELN                 ; related to hearth -> A1
10$
    MOVE.W  (A1)+,D0            ; item
    BMI.S   10$                 ; void
    BEQ.S   20$                 ; end
    GETINFO
    TST_PSEUDO      
    BNE.S   10$                 ; ignore any pseudos
    CMP.B   #1,6(A0)            ; be sure simply related
    BEQ.S   90$                 ; yes, EQ=> OK, return

20$

    ;;  nothing on hearth
    SETNE
    BRA.S   90$
    
50$    
    MOVE.W  #NASH,D0
    GETINFO
    CMP.B   #1,6(A0)            ;related?
    BNE.S   90$
    CMP.W   #NHEARTH,8(A0)          ;test the state...
90$
    PULL_L  D1-D2/D5/A1
    RET
    
WitchBizz:  DC.B    RNWITCH_CHAMBER,RNHALLWAY,RNSPIRAL_STAIRCASE
        DC.B    RNTOWER_ENTRANCE
        DC.B    RNBANQUET_HALL,RNCASTLE_KITCHEN

        ALIGN

*-------------------------

*
* AnswerDoor    -   Copes with clockmaker answering shop door
*           one move after knocking.
*

    XREF    ODT_CMAKERS_WINDOW

AnswerDoor

    CMP.W   #RNCAROUSEL,ROOM(A4)        ;Right place still?
    BNE.S   10$
    SCN 787
* You hear a click as the door is unlocked. The door is opened and the
* clockmaker peers at you. "My life," he says. "Can't you read, my boy?
* Put a notice on the door and they still come knocking." He points at
* the sign on the door before closing it and locking up again. Through
* the window, you see him enter the back room and resume his work.

10$
    MOVE.W  #NCLOCKMAKER,D0         ;Clockmaker back please
    DO  SETIN
    BSET    #7,ODT_CMAKERS_WINDOW(A4)   ;Fod ODT
    RET

*-----------------------------
    XREF    NRT_KITCHEN_FANTHROW
CrashInKitchen:
* are we hear?
    NVALID  KITCHEN_FAN     ;fan here?
    BNE.S   10$
    SCN 384         ; the kitchen fan falls on your head.
10$
    BSET    #7,NRT_KITCHEN_FANTHROW(A4)
    RET

*------------------------------------------

    XREF    AndRefresh.Refresh

; TimerAfters can call this

MoveAndRefresh
    DO  MovePlayer
    BRA P.Refresh

*------------------------------------------


PostMistressPrompt

    MOVE.W  #NPOSTMISTRESS,D0
    DO  VALID.NP        ;She local?
    BNE.S   90$         ;=> Nope, so no txt
    SCN 576
* Postmistress prompts you about posting summit.

90$
    RET

    XREF    FlagTrapSet,FlagMouseDead,TIMER7

MouseTrap
    
* is the trap still around?
    MOVE.W  #NMOUSE_TRAP,D0
    GETINFO
    CMP.L   #RNKITCHEN2,6(A0)
    BNE.S   90$
    MOVE.W  #NCHEESE,D0
    GETINFO
    CMP.L   #REL.MASK+NMOUSE_TRAP,6(A0)
    BNE.S   90$             ;no cheese on trap
    MOVEQ   #3,D1               ;1 in 3 prob
    DO  RND.D1
    TEST_B  D1
    BEQ.S   10$
5$
    MOVE.B  #4,TIMER7(A4)       ; reset it..
    RET
10$
* Are you in the kitchen?
    CMP.W   #RNKITCHEN2,ROOM(A4)
    BEQ.S   5$
    DO  SETOUT              ;zap cheese

    TEST_B  FlagTrapSet(A4)
    BEQ.S   90$ ;EQ =>u didn't set the trap - the mouse gets the cheese!
    
    ST  FlagMouseDead(A4)
    MOVE.W  #NDEAD_MOUSE,D0
    DO  SETIN
80$
    SF  FlagTrapSet(A4)
90$
    RET



    END
