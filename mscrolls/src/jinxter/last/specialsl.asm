******************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSL
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


 
*--------------------------------
    XDEF    SP.LEVER  
SP.LEVER
   
 
 
*--------------------------------
    XDEF    SP.LOWER
SP.LOWER
    RET
    

*--------------------------------

    XDEF    SP.LIFT
    XREF    REMASTER

SP.LIFT

    CALL_S  SP.LiftPhone
    BEQ.S   90$         ;Not a phone, exit to verb
    POPSP
90$
    RET

*--------------------

    XDEF    SP.LiftPhone
    XREF    RingCount,PhoneAnswered
; 'Phone in Xam's house gives a dialling tone, but yours may be ringing
SP.LiftPhone

    CMP.W   #NRECEIVER1,D0
    BEQ.S   15$
    CMP.W   #NTELEPHONE1,D0
    BEQ.S   15$

    CMP.W   #NRECEIVER,D0
    BEQ.S   10$
    CMP.W   #NTELEPHONE,D0
    BNE.S   90$

10$                 
    TEST_B  RingCount(A4)       ; Is it ringing?
    BNE.S   20$
15$
    SCN 305         ; Dialling tone
    BRA.S   80$
20$
    CALL_S  AnswerPhone     ;Xam's call and set relevant flags
80$
    SETNE_RET           ; something happened for sp.get etc
90$
    SETEQ_RET   

*-------------------------

    XDEF    AnswerPhone

AnswerPhone

    SCN 50          ;Xam's call.
    CLR.B   RingCount(A4)       ;Stop it ringing.
    ST  PhoneAnswered(A4)   ;So that we know that the phone...
    RET             ;...has been answered

*-------------------------------

    XDEF    SP.LOOK_After
    XREF    AquaTank,PRTNUM,P.TNSTOP,GET_CONT,P.LIST,SETPL
    XREF    SETIN
    XREF    COUNTLIST,FlyzzedFuture,CurrentFuture,FutureBits
    XREF    FutureEvents,P.TNSTOP

; Get here from SP.LOOK or SP.LOOKAFTER in a subtle fashion...
; So we can catch L IN BARREL as well as L BARREL
SP.LOOKIN
** Crystal Ball.
    CMP.W   #NCRYSTALBALL,D0    ; looking in the crystal ball
    BNE.S   90$
    TEST_B	REMASTER(A4)
    BEQ.S	20$
    TEST_W	FutureBits(A4)		; for remaster keep futures cycling
    BNE.S	20$
    MOVE.W	#%1000011001000010,FutureBits(A4)	
20$
    MOVE.W  FutureBits(A4),D5
    BEQ.S   40$         ; done all futures.
    TEST_B  FlyzzedFuture(A4)   ; flyzzed a future?
    BNE.S   40$
    MOVE.W  D5,D6           ; for lsr.
    AND.W   #%0000000000001111,D5   ; extract current one.
    LSR.W   #4,D6
    MOVE.W  D6,FutureBits(A4)   
    SCN 770         ; crystal ball clouds over...
    LEA FutureEvents(A4),A0
    MOVE.W  0(A0,D5),D0
    DO  P.TNSTOP
    MOVE.W  D5,CurrentFuture(A4)    ; and save away our future.
    BRA.S   80$
40$
    SCN 776         ; no future speel.

80$:
    POPSP
90$:
    RET


*-------------------------------

    XREF    NRT_RUNGS_EXAMINE,NRT_RUNGSLOOK,LadderOrientation

SP.LOOK_After.LADDER

    MOVE.W  LadderLeantAgainst(A4),D0
    BEQ.S   90$             ;=> no relator
    GETINFO
    TST_OUT                 ;Randomly been fodded?
    BEQ.S   10$             ;=> Nope
    CLR.W   LadderLeantAgainst(A4)      ;Fod relator
    BSET    #7,NRT_RUNGSLOOK(A4)        ;and NoReaches
    BSET    #7,NRT_RUNGSEXAMINE(A4)
    CLR.B   LadderOrientation(A4)
    BRA.S   90$
10$
    SCN 357
* It is leaning against 

    DO  P.TNSTOP            ;The x
90$
    POPSP_RET

    XREF    FlagTrapSet,FlagHearth

SP.LOOK_After

    CMP.W   #NCAROUSEL,D0       ;look @ carousel?
    BNE.S   002$
    POPSP               ;lose ret
    BEQ SP.DescribeCarousel
002$
    CMP.W   #NLADDER,D0
    BEQ SP.LOOK_After.LADDER

    CMP.W   #NHEARTH,D0     ; looking the hearth.
    BNE.S   001$
    TEST_B  FlagHearth(A4)
    BNE.S   001$
    SCN 772
001$
    CMP.W   #NMOUSE_TRAP,D0
    BNE.S   05$
    TEST_B  FlagTrapSet(A4)
    BNE.S   01$
    SCN 233         ;mouse trap not set msg.
    BRA.S   05$
01$
    SCN 232         ;mouse trap is set

05$
; Looking at aqualung??
    CMP.W   #NAQUALUNG,D0   ; L at aqualung tells you air levels.
    BNE.S   90$
    SCN 153     ; 'The tanks are '
    MOVE.W  AquaTank(A4),D6
    BNE.S   25$     ; If they're empty, then ...
    SCN 155     ; 'empty'
    BRA.S   90$
25$:
    CALL    PRTNUM      ; '55'
    SCN 154     ; '% full.'
90$:
    BRA SP.LOOKIN   ; Do look 'in' special bits!

*-------------------------------

    XDEF    SP.LOOK

    XREF    ADVERB,HandlePos,PrintSquareEntry,ObjectUnderRope
    XREF    ClockMakerGone

SP.LOOK

    IFNE Graphics
    XREF    Do_PictureD0,ClocksPicture

    CMP.W   #NCMAKERS_WINDOW,D0     ;Look cmakers window?
    BNE.S   10$
    TEST_B  ClockMakerGone(A4)
    BNE.S   10$                     ; but not after fire started
    MOVE.W  #ClocksPicture,D0       ;ClocksPicture,D0
    CALL    Do_PictureD0
    MOVE.W  #NCMAKERS_WINDOW,D0
    RET
10$
    ENDC

    CMP.W   #NLAVATORY,D0
    BNE.S   20$
    CMP.W   #IN,ADVERB(A4)
    BNE.S   20$

    TST_CLOSED              ;NOT if it's closed, please!!!! -Paul
    BNE.S   20$
    SCN 163     ; Such filthy habits!
20$:
    CMP.W   #NBLOCK,D0
    BNE.S   30$
    CALL    PrintSquareEntry
    BRA.S   80$
30$
    CMP.W   #IN,ADVERB(A4)
    BEQ SP.LOOKIN
    CMP.W   #INTO,ADVERB(A4); Or into...
    BEQ SP.LOOKIN
    CMP.W   #NDUNGEON_ROPE,D0   ; look under rope?
    BNE.S   40$
    CMP.W   #UNDER,ADVERB(A4)   ; under it?
    BNE.S   40$
    MOVE.W  ObjectUnderRope(A4),D2
    BEQ.S   40$         ; nothing there...
    MOVE.W  D2,D0           ; set up do...
    DO  P.TN
    DO  ISARE
    SCN 398
    BRA.S   80$
40$
    CALL    IsHandle    ; One of the safe handles?
    BNE.S   90$
    CALL    PrHandle    ; say whether it's up or down.
80$:
    POPSP
90$:
    RET

    XDEF    PrHandle
PrHandle
    DO  P.TN
    SUB.W   #NTLHANDLE,D0
    BTST    D0,HandlePos(A4); Test handle pos.
    BNE.S   45$
    SCN 477     ; 'up.'
    BRA.S   80$
45$:
    SCN 478     ; 'down!'   
80$:
    RET

    XDEF    IsHandle
; Returns true if D0 is a safe handle.
IsHandle
    XREF    Denotation_HANDLES
    PUSH_L  A1
    LEA Denotation_HANDLES(A4),A1
    DO  MEMBEROFSET
    PULL_L  A1
    RET

;********************************************
    XDEF    SP.LEAVE

*   XREF    SP.MEDDLE

    XREF    GO.LEAVE,ROOM

SP.LEAVE

* This makes 'leave bus' work...
    CMP.W   #NBUS,D0
    BNE.S   90$
    CMP.W   #RNONBUS,ROOM(A4)   ;Are we on the bus?
    BNE.S   90$

* Hmm, this is a bit sneaky
    MOVE.W  #NAUTODOORS,D0      ;Fool verb routine
    GETINFO
*   BRA.S   90$         ;Let the verb routine do the work!

90$
    RET


;********************************************
    XDEF    SP.LEAN



SP.LEAN

    TEST_W  D5          ;IO?
    BNE.S   90$         ;Yep must be lean x on/against y
    MOVE.B  11(A0),D1
    AND.B   #$7,D1          ;Clothing is silly
    BNE.S   10$
    MOVE.B  2(A0),D1        ;Get size and weight
    LSR.B   #4,D1           ;Consider size
    CMP.B   #4,D1           ;4 or more is prob half reasonable
    BLT.S   10$         ;=> Too small to lean on
    SCN 351
* You are now leaning on 

    DO  P.TN
    BRA.S   80$
10$
    DO  DBS
80$
    POPSP
90$
    RET

    XDEF    SP.LEAN_After
    XREF    LadderLeantAgainst,LadderOrientation
    XREF    NRT_RUNGSLOOK,NRT_RUNGSEXAMINE


SP.LEAN_After

    TEST_W  D5          ;COULD be NASTY
    BEQ.S   90$
    CMP.W   #NLADDER,D0     ;Lean ladder?
    BNE.S   90$
    MOVE.B  2(A5),D1        ;Make sure it's half sensible
    LSR.B   #4,D1           ;Get size
    CMP.B   #4,D1           ;Min 4
    BGE.S   10$         ;=> ok, should be alright!
    DO  DBS         ;Else, it's silly
    BRA.S   80$
10$
    EXG D0,D5
    DO  CARRIED         ;Nasty!
    EXG D0,D5
    BNE.S   20$
    DO  DBS
    BRA.S   80$
20$
    CMP.W   LadderLeantAgainst(A4),D5   ;Already is?
    BNE.S   25$
    MSG BUT         ;'but'
    DO  P.TN            ;'the ladder'
    EXG D0,D5
    SCN 12
* is already leaning against 

    DO  P.TNSTOP
    BRA.S   80$

25$
    MOVE.W  D5,LadderLeantAgainst(A4)
    CLR.W   6(A0)           ;Simply..
    MOVE.W  ROOM(A4),8(A0)      ;In room (Hmmmmmmm!)
    DO  P.TN            ;The ladder
    SCN 17
* is now leaning against
    EXG D0,D5
30$
    DO  P.TNSTOP        ;The Y.
    TEST_B  LadderOrientation(A4)   ;Which way up is it?
    BNE.S   80$         ;Wrong way, can reach rungs
    MOVE.W  #NRUNGS,NRT_RUNGSLOOK(A4)   ;Set noreach
    MOVE.W  #NRUNGS,NRT_RUNGSEXAMINE(A4)
80$
    POPSP
90$
    RET

;********************************************
    XDEF    SP.LIGHT

    XREF    R.NOUN,GETDATA,LIGHT,VALID.NP,CHEAT,CHEAT2,CHEAT.LI,SCRPNT

List.Light
    DC.W    NCANDLE
    DC.W    NMATCH
    DC.W    NMATCH2
    DC.W    NMATCH3
    DC.W    NMATCH4
    DC.W    0       ;terminator
 
SP.LIGHT
    MOVE.W  LIGHT(A4),-(SP)
    DO      R.NOUN      ;ROOM NOUN# = D0
    GETINFO         ;ROOM INFO = A0
    TST_LITROOM     ; a lit room?
    SNE     LIGHT(A4)
    BNE.S   90$
 
; If wearing magic spectacles, you can see in the dark!

    MOVE.W  #NSPECTACLES,D0
    GETINFO
    TST_WORNBYPLAYER
    SNE LIGHT(A4)   
    BNE.S   90$

07$
    LEA List.Light(PC),A1
10$
    MOVE.W  (A1)+,D0
    BEQ.S   90$
    DO  VALID.NP
    BNE.S   10$
    GETINFO
    TST_LIT             ;
    BEQ.S   10$
    ST  LIGHT(A4)
90$
    
    IFNE     YCHEAT
    MOVE.W  CHEAT(A4),D0       ;VARIOUS CHEAT WAYS TO GET LIGHT
    OR.W    CHEAT2(A4),D0
    OR.W    CHEAT.LI(A4),D0
    OR.B    D0,LIGHT(A4)
    ENDC
 
    MOVE.W  (SP)+,D0    ; Pull old LIGHT
    CMP.W   LIGHT(A4),D0
    BEQ.S   99$
    MOVE.W  #1,SCRPNT(A4)   ; If now light in room, do a description
99$ 
    RET
  
*--------------------------------
* Verb routine scans a table for things to listen to...
    XDEF    SP.LISTE

    XREF    GotSpecs,TIMER14,SETPRON,DO.SCORE,XamRescued,DoExploredRoom
* 
SP.LISTE
SP.LISTEN           ; Just for Roddy!

**********
* !!!!!! NB VERB has done exactly what this piece of code !!!!!!
* !!!!!! does just before we get here.            !!!!!!

*   TEST_W  D0      ; If just 'listen' then 'listen to room'
*   BPL.S   10$
*   DO  R.NOUN
*   GETINFO
*10$
**********

    
    CMP.W   #NONBUS,D0  ;Listen to bus? effectively. 'listen'
    BNE.S   10$
    NVALID  INSPECTOR   ;is he around?
    BNE 90$
    SCN 276
    BRA 80$

10$
    TEST_B  RingCount(A4)   ; ... and the 'phone is ringing...
    BEQ.S   15$     ; then exit with no message, as timer gives
                ; correct message
    EXG D0,D5
    MOVE.W  #NYOUR_HOUSE,D0 ; If in your house...
    DO  VALID.NP    
    EXG D0,D5
    BEQ.S   80$     ; then just get the 'you hear a phone ringing'

15$:
    CMP.W   #NMAGPIE,D0 ; Listening to magpie gives you the specs.
    BEQ.S   30$
    CMP.W   #NHOLLYBUSH,D0  ; catches 'listen' by itself in this room.
    BNE.S   40$
30$
    TEST_B  GotSpecs(A4)    ; done already?
    BNE.S   90$     ; Let ListenTable do it.
    SCN 79      ; Magpie spiel
    ST  GotSpecs(A4)    ; give specs to player
    MOVE.W  #NSPECTACLES,D0 ; spectacles...
    DO  SETIN       ; in...
    DO  SETPRON     ; and IT
    SCORE   GETSPECS
    BRA.S   80$

40$:
    CMP.W   #NHALLWAY,D0        ;Listen in hallway?
    BNE.S   50$
    TEST_B  XamRescued(A4)      ;Xam been released?
    BEQ.S   42$         ;=> nope, can't hear anything
    MOVE.W  #RNWITCH_CHAMBER,D2
    CALL    DoExploredRoom      ;!!! NB stomps D0 !!!!
    BEQ.S   44$         ;=> nope, so listen works ok
42$
    DO  P.SUB           ;'You'
    MSG CNTHEA          ;'can't hear anything.'
    BRA.S   80$

44$
    SCN 758
* you can hear Xam and the witch arguing.

    BRA.S   80$

50$:
    CMP.W   #NCAROUSEL,D0   ; to carousel
    BEQ.S   55$
    CMP.W   #NJUKEBOX,D0
    BNE.S   90$
55$:
    TEST_B  TIMER14(A4) ; Is the jukebox/carousel running?
    BEQ.S   90$     ; No, so silence
    SCN 226     ; 'band music, etc..'
80$
    POPSP
90$:
    RET


*-------------------------------------------


*
*
* GetTableEntry -   Gets an original entry from LUCK.LST
*
*   Takes:      D0.W    -   Event #
*   Returns     D1.L    -   Long word entry from table
*

    XDEF    GetTableEntry

GetTableEntry

    LEA LUCK.LST(PC),A1
10$
    MOVE.L  (A1)+,D1        ;Get nth table entry
    DBRA    D0,10$
    RET

*------------------------------


*
*
* DecLuck   -   Decrements luck points
*
*

    XDEF    DecLuck
    XREF    LUCK_

DecLuck
    SUBQ.W  #1,LUCK_(A4)    ;Lose some luck
    BPL.S   10$     ;=> still some left (NB may be 0)
    CLR.W   LUCK_(A4)   ;Went past the limit - make it 0
10$
    SCN 240
* ^Somehow, you don't feel quite as lucky as you did.^

    RET

*--------------------------------


*
*
* LuckyB    - Central 'get out of anything' code.
*         All Guardian intervention type stuff
*         comes here first.
*
*   Takes   D2  -   Event Number
*
*

    XDEF    LuckyB

LuckyB

    MOVE.L  A6,-(A7)        ;Stash A6
    MOVE.L  4(A7),A6        ;Get ptr to our DC.W
    ADDQ.L  #2,4(A7)        ;Bump ret address
    PUSH_L  D0-D7/A0-A5     ;Stash everything under the sun bar A6!
    MOVE.W  (A6),D2         ;Get the event #
    CLR.L   D7          ;Ret code to say if we dec luck
    LEA LUCK.LST(PC),A1
10$
    MOVE.L  (A1)+,D1        ;Get address;Event/message
    CMP.B   D1,D2           ;Found the event?
    BNE.S   10$         ;No, try next
    SWAP    D1          ;Get addr/msg
    BCLR    #15,D1          ;Hi-bit?
    BEQ.S   20$         ;=> no, must be code
    DO  P.SCN           ;it's a msg, do it
    BRA.S   30$
20$
    JSR -4(A1,D1.W)     ;Go do the event
30$
    TEST_W  D7          ;Do we dec the luck now?
    BNE.S   40$         ;=> nope
    CALL    DecLuck         ;Decrement the luck points

40$
    PULL_L  D0-D7/A0-A5     ;Pull everything...
    MOVE.L  (A7)+,A6        ;and A6
    RET



*------------------------------------------


Event       SET 0

LuckEntry   MACRO   *Label,points,Event  <OR>  scn#,points,Event,TXT

E\3     EQU Event
Event       SET Event+1
        IFEQ    NARG-4
        DC.W    \1+HI
        ENDC
        IFNE    NARG-4
        DC.W    \1-*
        ENDC
        DC.B    \2
        XDEF    E\3
        DC.B    E\3
        ENDM


*
* List of events for which there is code to cope or text to print.
*
* Format as follows:
*
* (1)   For code entries:
*
*   LuckEntry       Routine name,# of points to lose,event name.
*
* (2)   For text entries:
*
*   LuckEntry       SCN #,# of points to lose,event name,TXT
*
* NB Code addresses are laid down as PC rel DC.W's
*

LUCK.LST

    LuckEntry   Guard.BullCharge,100,BULLCHARGE
    LuckEntry   Guard.LeapToGirder,100,DARINGLEAP
    LuckEntry   Guard.Drowning,100,DROWNING
    LuckEntry   Guard.Train,100,HITTUNNEL
    LuckEntry   Guard.Train1,100,FALLBRIDGE
    LuckEntry   Guard.Train2,100,FALLOFFTRAIN
    LuckEntry   36,50,FENCE,TXT
    LuckEntry   Guard.Oven,100,OVEN

    DC.L    0           ;Long word 0 terminator


*-------------------------------------------------------------------

*###################################################################
* Specials associated with events go HERE 
*
* On entry they can expect D3.B to say how many times this event has
* occurred.
*
* Note      D1.L     =  # of points to lose;Event #
*                   (in case we want it).
*###################################################################

    XREF    FlagBull,NRT_HOLELOOK

Guard.BullCharge

* This happens if the bull is charging

    SCN 538
* The ground opens up and the bull falls in.

    SCN 542
* Crash! Bull hits solid ground and Grunchkev miner shouts at you.
    MOVE.W  #NHOLE,D0       ;Give 'em a hole to look at
    DO  SETIN
    MOVE.W  #NBULL,D0
    GETINFO
    MOVE.L  #CONT.MASK+NHOLE,6(A0)  ;Put bull in hole
    MOVE.W  D0,NRT_HOLELOOK(A4) ;Make the bull only lookable...
    ST  FlagBull(A4)        ;Tell the world - esp setmode
    Connect FIELD1,FIELD1,D     ;Give 'em an exit to TRY (setmode
                    ;will stop 'em)
    RET

*------------------------

    XREF    TIMERAFTER2

Guard.LeapToGirder

* This happens if you attempt to leap from the platform to the girder in
* the clock. NB The call is from setmode so we can do a room change
* etc.

    SCN 128
* Land on girder and fall....

    MOVE.W  #RNFALLING,ROOM(A4)
    MOVE.W  #1,SCRPNT(A4)
    MOVE.B  #1,TIMERAFTER2(A4)  ;Kick off the falling stuff
    MOVEQ   #-1,D7          ;Flag 'no luck decrement yet'
    RET

*------------------------

    XREF    TIMERAFTER1

Guard.Drowning

    SCN 156
    MOVE.B  #1,TIMERAFTER1(A4)  ;Kick off the rescue mission
90$
    MOVEQ   #-1,D7          ;Flag to say don't dec luck
    RET

*--------------------------

    XREF    TIMER11,SEATED,ForceRefresh

Guard.Train1

    SCN 534
* You jump off the bridge and fall....

    MOVE.W  #RNFALLING,ROOM(A4)
    CALL    ForceRefresh        ;Caution, stomps VERB to -1
    SCN 535
* Hit the ground andand pass out...

Guard.Train2
    BRA.S   Guard.Common



    

Guard.Train

    SCN 531
* Slamming into the tunnel wall, you pass out...

    BRA.S   Guard.Common        ;Fall through


    XDEF    GetRidOfObjects

GetRidOfObjects:
* D7 = 0 means do Pauls stuff (ie drop stuff && put D6 into 6(a0).L
* D7 = -1 means do Anita's stuff (ie check out size...)

    CLR.W   D2
    MOVE.L  D6,-(A7)
    DO  GETRELN         ;-> A1
    MOVE.L  (A7)+,D6        ;restore
    BRA.S   10$
5$
    BSET    #7,(A1)
7$
    ADDQ.W  #2,A1
10$
    MOVE.W  (A1),D0         ;Get one
    BMI.S   7$          ;Ignore (Hmm, need this?)
    BEQ.S   40$         ;=> end of list
    GETINFO
    TST_PSEUDO          ;No pseudos (just in case!)
    BNE.S   5$          ;try another
    TEST_L  6(A0)           ;Simply carried?
    BEQ.S   20$         ;=> yep,drop it
    CMP.B   #4,6(A0)        ;only allow clothing.
    BNE.S   5$          ;=> nope, must be intrinsic or summit
20$
    TEST_B  D7
** paul's stuff.
    BEQ.S   35$         ;next
30$
** right size...?
    MOVE.B  2(A0),D3    ; Get the size.
    LSR.W   #4,D3
    CMP.B   #4,D3
    BMI.S   5$                  ; 0 to 3 OK.
    TEST_B  REMASTER(A4)
    BEQ.S   31$
    CMP.B   #4,D3
    BEQ.S   5$                  ; allow size=4 for remaster (eg saddle)
31$    
    DO  SETOUT      ; fixing tied etc?
    DO  SETIN       ; and get it back
35$
    MOVE.L  D6,6(A0)    ;Simply in the station (or wherever)
    BRA.S   7$      ; flag it...
40$
    RET

    XREF    DracItem

Guard.Common

* Lose all player's possessions
    SF  D7          ; clear it means for paul's stuff
    MOVE.L  #RNPLATFORM1,D6     ; means the place to leave stuff
    CALL_S  GetRidOfObjects
30$
    SCN 536
* Some time later, you regain consciousness.^

    CLR.W   DracItem(A4)        ;Do this b4 we hit SPECIALS!!!!
    MOVE.W  #RNTHEATRE,ROOM(A4)
    MOVE.W  #NOPTABLE,SEATED(A4)    ;(on the operating table)
    MOVE.B  #1,TIMER11(A4)      ;Kick off the timer
    MOVEQ   #-1,D7          ;Flag no luck decrement yet
    RET


*--------------------------------



    XREF    TIMERAFTER12,TIMER10,BakerFollows

Guard.Oven

    SCN 60
* You clamber inside the oven. Baker locks it and switches it on

    CLR.B   TIMER10(A4)     ;Fod Baker timer
    CLR.B   BakerFollows(A4)
    MOVE.W  #NOVEN,D0
* Get all that is in contained by the oven
    DO  GET_CONT        ;-> A1
10$
    MOVE.W  (A1)+,D0
    BMI.S   10$         ;=> ignore
    BEQ.S   20$         ;=> done
    GETINFO
    MOVE.L  #RNINOVEN,6(A0)     ;...in room.
    BRA.S   10$         ;next
20$
    MOVE.W  #NCRISP,D0      ;move crisp into oven
    GETINFO
    TST_OUT             ;don't bother if it is out
    BNE.S   25$
    MOVE.L  #RNINOVEN,6(A0)
25$
    MOVE.W  #RNINOVEN,ROOM(A4)  ;Put 'em in the oven
    MOVE.B  #1,TIMERAFTER12(A4) ;Kick off event
    MOVEQ   #-1,D7          ;No luck dec yet
    RET



*----------------------------------------------------------------

        XDEF    SP.LIE_DOWN
        XREF    ROOM,LieDownOnTrain
        XREF    SEATED_RELATOR,SEATED

SP.LIE_DOWN
    CMP.W   #NROOF,D0       ;lie on roof
    BEQ.S   20$
    CMP.W   #NLOCOMOTIVE,D0
    BNE.S   90$
20$
    CMP.W   #RNTRAIN_ROOF,ROOM(A4)      ;must be on the roof
    BNE.S   90$
    TEST_B  LieDownOnTrain(A4)      ;already?
    BEQ.S   10$             ;no
    SCN 513             ;already
    BRA.S   80$             ;exit
10$
    SCN 514             ;you lie down
    ST  LieDownOnTrain(A4)      ;set flag on it
    MOVE.W  #ON,SEATED_RELATOR(A4)
    MOVE.W  #NROOF,SEATED(A4)
80$
    POPSP
90$
    RET

*---------------------------------------------
    XDEF    SP.LIGHT_After

SP.LIGHT_After

    XREF    SP.MATCHES,Denotation_MATCHES
    
    MOVE.L  A1,-(A7)
    LEA Denotation_MATCHES(A4),A1
    DO  MEMBEROFSET     ;D0 a match?
    PULL_L  A1
    BNE.S   90$         ;no
    BRA SP.MATCHES
90$
    RET

*---------------------------------------------

    XDEF    SP.DescribeCarousel
    XREF    ONLYREL.SUB,P.LongList2

SP.DescribeCarousel

    SCN 355     ;initial messge
    MOVE.W  D0,D2       ;target
    DO  GETRELN2    ;all asoc wi them->a1
    MOVEQ   #$11,D1     ;insep related
    CALL    ONLYREL.SUB ;remove all not insep related
    DO  COUNTLIST   ;# ->d4
    BNE.S   10$     ;some
    SCN 329
    BRA.S   90$     ;quit
10$
    CALL    P.LongList2 ;print list
    SCN 354
90$
    RET

*------------------------------------------
