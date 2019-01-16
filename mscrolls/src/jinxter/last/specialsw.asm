************************************************************
*
*   %W%
*   %G%
*
************************************************************

    SECTION "ADV",code

    include "equates.i"
    include "macros.i"
    include "scenmacros.i"
    include "roomequ.i"

    include "nounequ1.i"
    include "scenequ.i"
    include "scoreref.i"


    XDEF    SP.WEAR
    XREF    SP.PICKUP
    XREF    REMASTER
*   XREF    SP.MEDDLE


; Two handy routines for setting items in, and testing if they're in
    XDEF    SETIN,ISIN,DROPHERE

    XREF    ROOM,DropHere

SETIN
    GETINFO         ; Get noun data
    BCLR    #7,6(A0)    ; set it IN
    RET

ISIN
    GETINFO
    TST_OUT             ; Return EQ if it's IN
    RET

DROPHERE
    GETINFO         ; get info... Add some hassle to people
    DOGO    DropHere    ;using A0 as a pointer to something else

*----------------------------------

SP.WEAR
    CALL    SP.PICKUP
    BEQ.S   01$
    POPSP_RET
01$

*        !!!!!!!!!!!!   WARNING    !!!!!!!!!!!!
* IF D0 is NAQUALUNG then DO NOT POP the stack as there's a call to the
* WEAR verb from inside the scenario code which expects it to return.
* Is it stands this code is ok.

    CMP.W   #NAQUALUNG,D0
    BNE.S   90$
    SCN 320                     ; now breathing through aqualung
90$:
    RET

*--------------------------------

    XDEF    SP.UNWEA
    XDEF    SP.WIND
    XREF    TapDancing
   
SP.UNWEA
SP.WIND
    RET

*--------------------------

    XDEF    SP.WAIT
    XREF    TIMERAFTER8

SP.WAIT

    CMP.W   #RNTRAIN_ROOF,ROOM(A4)  ;waiting for train
    BNE.S   01$
    TEST_B  TIMERAFTER8(A4)     ;and train is waiting to leave?
    BEQ.S   01$         ;no
    MOVE.B  #1,TIMERAFTER8(A4)  ;otherwise happens
    BRA.S   80$         ;exit
01$
    MOVE.W  #NKEY1,D0
    DO  ISIN            ; Is it IN?
    BNE.S   90$         ;=> nope, Exit
    MOVE.W  D0,STOPRON(A4)      ;Set up the '@'
    MOVE.B  3(A0),D1        ;Get texture
    AND.B   #$F,D1          ;Mask sharp etc
    CMP.B   #TEXTURE.HOT,D1     ;Hot now?
    BNE.S   10$
    CMP.B   #4,TIMERD(A4)
    BNE.S   90$
    CMP.W   #NCANDLE,WarmedWith(A4)
    BNE.S   90$
    PUSH_L  D0/A0
    MOVE.W  #NCANDLE,D0
    GETINFO
    TST_LIT             ;candle still alight?
    PULL_L  D0/A0
    BEQ.S   90$         ;no so doesn't melt
    SCN 56
* The key melts forming a shapeless plastic blob.

    DO  SETOUT          ;Lose the key
    MOVE.W  #NBLOB,D0
    DO  SETIN           ;Set blob 'in'
*   CLR.B   TIMERD(A4)      ;Superfluous since key is 'out'
    BRA.S   80$

10$
    CMP.B   #5,TIMERD(A4)       ;Last move was 'warm key with candle'?
    BNE.S   90$         ;=> nope, Exit
    CMP.W   #NCANDLE,WarmedWith(A4) ;Was it the candle?
    BNE.S   90$         ;nope, exit


* Must be warm if we get here

    AND.B   #$F0,3(A0)      ;Mask texture
    OR.B    #TEXTURE.HOT,3(A0)  ;Make it hot
    SCN 55
* Almost at melting point, @ begins to lose it's shape.

80$
    POPSP
90$
    RET

*---------------------------

    XDEF    SP.WAITUNTIL

SP.WAITUNTIL

;* D2 Arrives with KST - please set it up if you want to use the
;* verb wait until afterwards...

    RET

    XDEF    SP.WARM
    XREF    TIMERD,WarmedWith,BurnRope

*
* !!!! NB SP.MELT BRA's here coz only the key is considered. If summit !!!!
* !!!! else gets added to this special then make SP.MELT check for the !!!!
* !!!! key first.                              !!!!
*
 
SP.WARM

    CMP.W   #NKEY4,D0       ;heat the second key?
    BEQ.S   09$         ;yes, it melts straight away!
    CMP.W   #NKEY1,D0       ;Warm plastic key?
    BNE.S   40$
    MOVE.W  D0,STOPRON(A4)      ;Set up the '@'
    MOVE.W  D5,WarmedWith(A4)   ;Wot we used
    MOVE.B  #TEXTURE.HOT,D2
    MOVE.B  3(A0),D1
    AND.B   #$F,D1          ;Mask sharp etc
    CMP.B   D2,D1           ;Already hot?
    BNE.S   10$         ;=> nope
09$
    MOVE.W  D0,STOPRON(A4)      ;Set up the '@'
    TEST_B  REMASTER(A4)
    BEQ.S   08$
    SCN 824                 ; leave it alone...
    BRA.S 80$
08$
    DO  SETOUT          ;If it is then lose it and...
    SCN 56
* The heat becomes too much for @ and it melts, forming a blob.

    MOVE.W  #NBLOB,D0
    DO  SETIN           ;Set blob in
    BRA.S   80$

10$
    CMP.B   #TEXTURE.WARM,D1    ;So... Is it already warm?
    BEQ.S   20$         ;=> Yep, go make it hot
    MOVE.B  #TEXTURE.WARM,D2    ;If not, it soon will be
    SCN 54
* The heat warms @ making it a little more malleable.

20$
    MOVE.B  3(A0),D1
    AND.B   #$F0,D1         ;mask old texture
    OR.B    D2,D1           ;Give it our new one
    MOVE.B  D1,3(A0)
    CMP.B   #TEXTURE.WARM,D2    ;We just made it warm?
    BEQ.S   30$         ;=> Yep, so we've had the spiel
    SCN 55
* Almost at melting point, @ begins to lose it's shape.
30$
    TEST_B  REMASTER(A4)
    BNE.S   80$
    MOVE.B  #6,TIMERD(A4)       ;But not for long!!
    BRA.S   80$
40$
    CMP.W   #NICE,D0        ; melting the ice?
    BNE.S   45$
    DO  P.TN
    SCN 367         ; "melt@."
    BSET    #7,6(A0)        ; set out, don't adjust volumes
    MOVE.W  #NICEWATER,D0       ; set in the water
    DO  SETIN           ; does GETINFO;BCLR #7,6(A0);RET
    BRA.S   80$
45$
    CMP.W   #NKEY2,D0     ; added for remaster
    BNE.S   46$
    SCN     827
    BRA.S   80$
46$
    CMP.W   #NKEY3,D0
    BNE.S   50$
    SCN     827
    BRA.S   80$
50$
    CMP.W   #NDUNGEON_ROPE,D0   ; is it the rope?
    BEQ BurnRope
80$
    POPSP
90$
    RET

*
* !!!! NB SP.MELT BRA's here coz only the key is considered. If summit !!!!
* !!!! else gets added to this special then make SP.MELT check for the !!!!
* !!!! key first.                              !!!!
*

******************************************************************
*
*   Weather - code to think about the weather
*
*   called each move via SPECIALS
*
* The idea is this - 'TypeOfWeather' has 8 bits, each represents
* one sort of weather that's happening NOW (yes, so it can rain+shine at
* the same time!)
*
* each type of weather is related to a PSEUDO noun which is in all the rooms
* that it applies to, eg rain is in all outside rooms so is related to OUTSIDE
*
* if the 'weather' is active then it will be local, otherwise it is set out
* the pseudo is never out, so several types of weather (eq rain+bird) can
* use the same pseudo (which might have a LONG list of locations!)

    XREF    TypeOfWeather,WeatherTime,STOPRON
    XDEF    Weather,WT_WRAIN,StartRain,EndRain

c_weathertype   SET 0

WEntry  MACRO   *code to start,code to end,prob of event,type
WT_\4       EQU c_weathertype
c_weathertype   SET c_weathertype+1
        DC.W    \3/8+1
        DC.W    N\4
        DC.W    \1-WeatherTable
        DC.W    \2-WeatherTable
        ENDM




* the prob. of event is the (approx) no. of move before an event is expected

WeatherTable

* windy?
    WEntry  StartWind,EndWind,100,WWIND0

* raining?
    WEntry  StartRain,EndRain,100,WRAIN

* stormy?
    WEntry  StartCloud,EndCloud,100,WCLOUD

* lightning?
    WEntry  StartLightning,EndLightning,100,WLIGHTNING

* Guardian
    WEntry  RandGuardian,RandGuardian,16,GUARDIAN   ;noun irrel

* 8 bits in TypeOfWeather
    DC.L    0,0 


Weather

    ADDQ.B  #1,WeatherTime(A4)  ;only consider weather every 8 moves

    AND.B   #7,WeatherTime(A4)
    BNE.S   90$         ;NE => not time yet

    LEA WeatherTable(PC),A1
    MOVE.L  A1,A2
    MOVEQ   #-1,D6          ;weather type
10$
    ADDQ.W  #1,D6
    MOVE.W  (A1)+,D1        ;get probability of an event
    BEQ.S   90$         ;EQ=>finished
    DO  RND.D1              ;returns a BYTE!!!
    CMP.B   #1,D1           ;this time?
    BEQ.S   20$         ;EQ=> yes
    ADDQ.L  #6,A1
    BRA.S   10$

* now call relevant stuff

20$
    MOVE.W  (A1)+,D0        ;get noun in question
    GETINFO
    MOVE.W  (A1)+,D1        ;get offset to first event
    BCHG    D6,TypeOfWeather(A4)
    BEQ.S   30$         ;EQ=> use first
    MOVE.W  (A1),D1         ;get offset to second event
30$
    PUSH_L  D6/A1/A2
    JSR 0(A2,D1.W)      ;do the event
    PULL_L  D6/A1/A2
    ADDQ.L  #2,A1
    BRA.S   10$
90$
    RET
    

*****
**
** Lightning    - Only allowed when cloudy and raining
**
**
****

StartLightning

* Are the stormclouds about?
    BTST    #WT_WCLOUD,TypeOfWeather(A4)
    BEQ.S   90$         ;=> Nope, so no lightning
* AND is it raining?
    BTST    #WT_WRAIN,TypeOfWeather(A4)
    BEQ.S   90$         ;=> Nope, so no lightning
    DO  SETIN           ;Set lightning 'in'
    DO  VALID.NP        ;Are we in a room where it matters?
    BNE.S   90$
    SCN 261

* Lightning arcs it's way across the stormy sky.^

90$
    RET


EndLightning
    
    DO  SETIN           ;Is the lightning really 'in'?
    BNE.S   90$         ;=> Nope, so exit
    DO  VALID.NP        ;we in a room where it matters?
    BNE.S   10$         ;NE=> nope
    SCN 262
* The stormy atmosphere lifts as the lightning stops.^

10$
    BSET    #7,6(A0)        ;set lightning 'out'
90$
    RET


*****
**
** Rain -   Returns NE if rain can be observed and EQ if not
**
**
****
    XREF    SP.SunManOut,SP.RainManOut

StartRain
    
    CLR.W   D6          ;NE/EQ  return flag
    DO  SETIN           ;set rain 'in'
    DO  VALID.NP        ;we in a room where it matters?
    BNE.S   90$         ;NE=> nope
    SCN 263
* The heavens open and rain begins to fall.^

    MOVEQ   #-1,D6
90$
    CALL    SP.RainManOut
    RET

EndRain
    
    CLR.W   D6          ;NE/EQ return flag
    DO  VALID.NP        ;we in a room where it matters?
    BNE.S   10$         ;NE=> nope
    SCN 264

* The rain ceases as abruptly as it began.

    MOVEQ   #-1,D6          ;Flag visible change
10$
    BSET    #7,6(A0)        ;set rain 'out'
90$
    CALL    SP.SunManOut
    RET


*****
**
** Wind -   There are four different winds!
**      And they can combine to give variations!
**
****


StartWind
    DO  SETIN           ;Set the appropriate wind 'in'
    DO  VALID.NP        ;Need the speil?
    BNE.S   90$         ;=> Nope
* Is it raining?
    BTST    #WT_WRAIN,TypeOfWeather(A4)
    BEQ.S   20$         ;=> Nope

* A sudden wind turns the shower into driving rain.^
    SCN 265
    BRA.S   90$
20$
* The wind picks up.^
    SCN 266
90$
    RET



EndWind
    BSET    #7,6(A0)        ;Set appropriate wind 'out'
    DO  VALID.NP        ;we in a room where it matters?
    BNE.S   90$         ;=> nope
    SCN 267
* The wind dies down somewhat.^

90$
    RET

*------------------------------------

    XREF    FlagRoomDone,LIGHT,Waiting


RandGuardian

    TEST_B  LIGHT(A4)       ;check for a light source.  
    BEQ.S   90$         ;EQ => dark

    LEA GuardianTableEnd(PC),A1 ;end of msg,room# table
    MOVEQ   #GuardianTable_SIZE-1,D0
10$
    MOVE.L  -(A1),D1        ;we go from end to start (downwards)
    CMP.W   ROOM(A4),D1     ;Right room?
    DBEQ    D0,10$
    BNE.S   90$
    SWAP    D1
    BSET    D0,FlagRoomDone(A4) ;Flag that this is done
    BNE.S   90$
    CLR.W   Waiting(A4)     ; autowaiters
    DOGO    P.SCN
90$
    RET


GUARDMAC    MACRO   *MESS,ROOMNO.
        DC.W    \1,RN\2
        ENDM


GuardianTable

    GUARDMAC     595,KITCHEN2

GuardianTableEnd

GuardianTable_SIZE  EQU (GuardianTableEnd-GuardianTable)/4


*--------------------------------

*
*
* TransWind -   Return Current wind direction in DIR_ format
*
*   On Entry:   TypeOfWeather contains weather info
*
*   On Exit:    D0.B contains the DIR_ wind direction
*           or $FF if it maps to an illegal combination
*           CurrentWind contains current wind DIR_ #
*           A0 corrupted
*

*WindMask   EQU $F
*DIR_ILLEG  EQU $FF


*TransWind
*   LEA TransTable(PC),A0
*   MOVE.B  TypeOfWeather(A4),D0
*   AND.W   #WindMask,D0
*
*   IFNE    YCHEAT
*   BNE.S   10$         ;There should ALWAYS be some
*                   ;wind about if this is called!
*   PRINT   <'Help! Transwind called with no wind bits!'>
*   ADDQ.B  #1,D0           ;Just so we don't miss the table
*10$
*   ENDC
*
*   SUBQ.B  #1,D0           ;Go from 0 upwards
*   ADD.W   D0,A0
*   MOVE.B  (A0),D0         ;Get the corresponding DIR_ #
*   BMI.S   90$
*   MOVE.B  D0,CurrentWind(A4)  ;Stash valid DIR's
*90$
*   RET             ;NB Caller expects N flag intact!


* Yes, there's probably an easier way to do this!!!

*TransTable
*   DC.B    DIR_E
*   DC.B    DIR_W
*   DC.B    DIR_D           ;East + West = DOWN!
*   DC.B    DIR_N
*   DC.B    DIR_NE
*   DC.B    DIR_NW
*   DC.B    DIR_ILLEG       ;East+West+North not allowed!
*   DC.B    DIR_S
*   DC.B    DIR_SE
*   DC.B    DIR_SW
*   DC.B    DIR_ILLEG       ;East+West+South not allowed!
*   DC.B    DIR_U           ;North + South = UP!
*   DC.B    DIR_ILLEG       ;North+South+East not allowed!
*   DC.B    DIR_ILLEG       ;North+South+West not allowed!


*-------------------------

*****
**
** Stormclouds
**
**
****

StartCloud
    
    DO  SETIN           ;set clouds 'in'
    DO  VALID.NP        ;we in a room where it matters?
    BNE.S   90$         ;NE=> nope
* Is it raining?
    BTST    #WT_WRAIN,TypeOfWeather(A4)
    BEQ.S   10$         ;nope it is not raining...
    SCN 268

* Storm clouds are forming overhead, goaded on by the rain.^

    BRA.S   90$
10$
    SCN 269
* Large storm clouds gather on the horizon.^

90$
    RET


EndCloud
    
    DO  VALID.NP        ;we in a room where it matters?
    BNE.S   10$         ;NE=> nope
* is there rain around?
    BTST    #WT_WRAIN,TypeOfWeather(A4)
    BEQ.S   5$          ;no rain.
    SCN 270
* The clouds disperse, taking the potency out of the rain.^

    BRA.S   8$
5$
* there is no rain so...

    SCN 271
* The stormclouds drift away.^

8$
    MOVE.L  A0,-(A7)        ;Preserve cloud info ptr
    MOVE.W  #NWLIGHTNING,D0     ;See if we need lightning message
    GETINFO
    DO  ISIN
    BNE.S   9$          ;=> lightning 'out' so don't
    SCN 262
* The stormy atmosphere lifts as the lightning stops.^
9$
    MOVE.L  (A7)+,A0
10$
    BSET    #7,6(A0)        ;set clouds 'out'

* Set lightning 'out' wether it was there or not!
    MOVE.W  #NWLIGHTNING,D0
    GETINFO
    BSET    #7,6(A0)
90$ 
    RET

****************************************************************************
* HouseHorror
* randome things will happen after the phone has rung in your house...
* 
****************************************************************************
    XDEF    HouseHorror
    XREF    HorrorPlace,PhoneAnswered,RND.D1,P.SCN,HTable
    XREF    SEATED


HouseHorror
    TEST_B  HorrorPlace(A4)
    BEQ.S   99$
    TEST_B  PhoneAnswered(A4)   ; and has the phone gone?
    BEQ.S   99$
    MOVE.L  #20,D1          ; one in thirty chance
    DO  RND.D1
    CMP.B   #1,D1           ; the one we want
    BNE.S   99$
    MOVEQ   #6,D1
    DO  RND.D1
    CMP.B   #4,D1           ; 3 or less we do this..
    BEQ DoHMess
    CMP.B   #3,D1
    BEQ DoHDoors
    CMP.B   #5,D1
    BEQ DoHCollapse
    MOVE.W  ROOM(A4),D0     ; room we are in.
    LEA HTable(A4),A0
10$
    TEST_B  (A0)            ;eq= FINI
    BEQ.S   99$         ; Nothing happening
    CMP.B   (A0),D0
    BEQ.S   20$
    ADDQ.W  #2,A0           ; after scen.
    BRA.S   10$
20$
    MOVE.B  #-1,(A0)+       ; fod it so it doesn't appear again
    CLR.W   D1
    MOVE.B  (A0),D1
    DOGO    P.SCN           ; Print scenario message
99$
    RET
DoHMess:
    MOVEQ   #2,D1           ; random number of messages
    DO  RND.D1
    LEA HorrorMess(PC),A0
    LSL.W   #1,D1
    MOVE.W  0(A0,D1.W),D1
    DOGO    P.SCN
DoHDoors:
    MOVEQ   #5,D1           ;number of doors
    DO  RND.D1
    LEA HorrorDoors(PC),A0  ; get the doors
    LSL.W   #1,D1           ; mulu by 2
    MOVE.W  0(A0,D1.W),D0
    GETINFO
    TST_CLOSED              ; closed?
    BNE.S   99$         ; it is CLOSED.
    SCN 157         ; you hear a door slamming
    DOGO    CloseDoor       ; and close it....
99$
    RET

    XREF    HCollapse,DESITTABLE
DoHCollapse:

    ;; remaster fridge can no longer collapse.
    
    MOVEQ   #5,D1
    DO      RND.D1
    LEA     HCollapse(A4),A0
    LSL.W   #1,D1
    MOVE.W  0(A0,D1.W),D0       ; Object to collapse
    BEQ.S   99$                 ; Done allready!
    CMP.W   SEATED(A4),D0       ; hmm sad but unfortunetly...
    BEQ.S   99$
    CLR.W   0(A0,D1.W)
    GETINFO
    BSET    #2,(A0)             ; broken
    SCN 454
    DOGO    DESITTABLE      ; get rid of it
99$
    RET

    
HorrorMess:
    DC.W    138
    DC.W    147 
HorrorDoors:
    DC.W    NGREEN_DOOR
    DC.W    NFRONT_DOOR
    DC.W    NBATHROOMDOOR
    DC.W    NKITCHENDOOR
    DC.W    NLEATHERDOOR
*--------------------------------------------------

    IFD wantSP.WAVE
    XDEF    SP.WAVE
* Eds - Paul
    XREF    SP.SHAKE
SP.WAVE
    BRA SP.SHAKE

    ENDC    ;wantSP.WAVE
*--------------------------------------------------

    XDEF    SP.WAKE
    XREF    WakeUpGaoler.DO
    XREF    ReleaseXam

SP.WAKE

    CMP.W   #NGAOLER,D0
    BNE.S   10$         ;no, exit
    SCN 539         ;wakes up
    SCN 598         ;and chucks u into a cell
    BRA WakeUpGaoler.DO     ;and modify (popsp ret)
10$ 
    CMP.W   #NXAM,D0        ;wake xam
    BNE.S   90$         ;no
    CALL    WakeXam         ;wake him wi msg
80$
    POPSP
90$
    RET

*----------------------------------------------------

    XDEF    CurrentPlayerClothing
; OR the clothing masks of all the Player's clothing together into D2.B
CurrentPlayerClothing
    PUSH_L  D0/A0-A1
    CLR.W   D2          ;target == player
    DO  GETRELN2        ;all things, asoc player ->a1
10$
    MOVE.W  (A1)+,D0        ;fetch one
    BEQ.S   90$         ;done
    GETINFO
    TST_WORNBYPLAYER        ;r u wearing it?
    BEQ.S   10$         ;nope
    TST_OUT             ;no 'out' clothing (today)
    BNE.S   10$
    OR.B    10(A0),D2       ;include these covers
    BRA.S   10$         ;anymore?
90$
    AND.B   #$F8,D2         ;zap container vol (if exists)
    PULL_L  D0/A0-A1
    RET

    XDEF    SP.WEAR_Before
    XREF    WEAR.DO

SP.WEAR_Before
; can't wear wetsuit if you are already wearing
; something which intersects its clothing mask
    PUSH_L  D1/D2/A1
    CMP.W   #NDIVING_SUIT,D0    ; the diving suit ?
    BNE.S   10$
    MOVE.B  10(A0),D1       ; get the clothing mask
    AND.B   #$F8,D1
    CALL    CurrentPlayerClothing   ; get player's current clothing into D2
    AND.B   D2,D1           ; intersect them
    BEQ 90$         ; EQ => no intersection so ok
5$
    SCN 98          ; something prevents you
    BRA 80$         ; don't bother with verb
10$
    CMP.W   #NFLIPPERS,D0       ; can't wear shoes & flippers
    BNE.S   15$
    PUSH_L  D0/A0
    MOVE.W  #NTAPDANCE_SHOES,D0
    GETINFO
    TST_WORNBYPLAYER
    PULL_L  D0/A0
    BNE.S   5$          ; wearing shoes, so moan
15$
    CMP.W   #NTAPDANCE_SHOES,D0
    BNE.S   16$
    PUSH_L  D0/A0
    MOVE.W  #NFLIPPERS,D0
    GETINFO
    TST_WORNBYPLAYER
    PULL_L  D0/A0
    BNE.S   5$          ; wearing flippers, so moan
16$
    CMP.W   #NAQUALUNG,D0       ;wear aqualung
    BNE.S   50$         ;no
    TST_WORNBYPLAYER        ;already
    BNE.S   90$
    TEST_W  12(A0)          ;fixed?
    BNE.S   90$
20$
    DO  FIXCHK          ;anything random fixed to it?
    BEQ.S   90$         ;no, ok let verb do it
30$
    MOVE.W  (A1)+,D1        ;get attached item
    BEQ.S   40$         ;end ok.
    CMP.W   #NSPARE_TANK,D1     ;exception?
    BEQ.S   30$         ;ok
    BRA.S   90$         ;else verb stops it
40$
    CALL    WEAR.DO
    BRA.S   80$
50$
; if clothing mask of item includes chest, check you aren't wearing aqualung
    BTST    #3,10(A0)
    BEQ.S   60$
    PUSH_L  D0/A0
    MOVE.W  #NAQUALUNG,D0
    GETINFO
    TST_WORNBYPLAYER
    PULL_L  D0/A0
    BNE 5$          ; prevents u
60$
    CMP.W   #NSOCK,D0       ;Wear sock?
    BNE.S   90$
    SCN 627
* After a quick consultation with your feet, you decide not to.

80$
    PULL_L  D1/D2/A1
    POPSP_RET
90$
    PULL_L  D1/D2/A1
    RET

*------------------------------------------
    IFD wantSP.WEIRDATTIRE

    XDEF    SP.WEIRDATTIRE
    XREF    IsRoomWet

SP.WEIRDATTIRE

    DO  IsRoomWet       ;aqua equipment is ok here
    BEQ.S   90$         ;no, exit
    CLR.B   D1          ;no weirdness here
90$
    RET
    ENDC    ;wantSP.WEIRDATTIRE
*------------------------------------------

    XDEF    WakeXam

WakeXam
    SCN 597         ;gaoler wakes & grabs you
    SCN 598         ;chucks u into a cell
    BRA ReleaseXam

*------------------------------------------

    XDEF    SP.WASH

SP.WASH

    CMP.W   #NHAND,D0       ;wash hands?
    BNE.S   90$         ;no
    CMP.W   #NBASIN,D5      ;in basin?
    BNE.S   90$         ;no
    SCN 624         ;you wash hands
    POPSP
90$     
    RET

*------------------------------------------

    XDEF    SP.AutomaticWait,AutoWaitTable
    XREF    LookUpRoomInTable,Waiting

AutoWaitTable

    DC.B    RNON_TRAIN
    DC.B    RNTOP_MOUNTAIN
    DC.B    RNRAILWAY_TRACK
    DC.B    RNPLATFORM1
    DC.B    RNPLATFORM2
    DC.B    RNPLATFORM3
    DC.B    0
    ALIGN


SP.AutomaticWait

    LEA AutoWaitTable(PC),A1        ;super wait rooms
    CALL    LookUpRoomInTable
    BNE.S   90$             ;exit, fail
    MOVE.W  #15,Waiting(A4)         ;default 15 moves
    SETEQ                   ;set to success
90$
    RET

*------------------------------------------


*
*
* WonGame   -   This gets called when you've won!
*
*

    XDEF    WonGame
    XREF    FlagNothingDoing,FlagGameOver,GetRidOfObjects
    XREF    ForceRefresh,EXTRA,WHOOPS,FlagRunover,DracItem


WonGame
    SCN 791
* Big spiel about slapping on bracelet

* the original did not award the winning points for killing the
* witch, but only when you "die". Unfortunately you can never see
* your score!
    
    TEST_B  REMASTER(A4)
    BEQ.S   10$
    SCORE   WINNING         ;And the final point!
10$    
    MOVE.W  #RNCOUNTRYLANE1,D1
    DO  MovePlayer
    MOVEQ   #0,D7           ;Flag to routine
    CALL    GetRidOfObjects     ;Lose stuff on inventory
    MOVE.W  #NOVERCOAT,D0
    GETINFO
    MOVE.L  #WORN.MASK,6(A0)    ;Give it to player
    MOVE.W  #NSANDWICH,D0
    GETINFO
    CLR.L   6(A0)           ;and the sandwich
    CALL    ForceRefresh        ;Force room desc
    CLR.W   DracItem(A4)        ;Waa, can't have summit coming back!
    CLR.B   FlagRunover(A4)     ;Use this flag again
    ST  FlagGameOver(A4)
    ST  FlagNothingDoing(A4)    ;And let Nothing-Doing take over
    CLR.W   EXTRA(A4)
    ST  WHOOPS(A4)
    RET


*-------------------------------------------

    XDEF    SP.WEIGH
    XREF    CALCWHT,DBS,P.SMALLNUM

SP.WEIGH

    TEST_W  D5      ;any iobj specified?
    BEQ.S   10$     ;no
    CMP.W   #NSCALES,D5 ;if so, must be the scales
    BEQ.S   20$     ;fine.
    CALL    DBS
    BRA.S   80$     ;and exit
10$
    NVALID  SCALES      ;are scales nearby?
    BNE.S   90$     ;ret to verb => nothing suitable
20$
    NVALID  POSTMISTRESS    ;is she around?
    BNE.S   21$     ;ok
    SCN 512     ;hands off!
    BRA.S   80$     ;exit
21$
    SCN 508     
    CALL    CALCWHT
    MOVE.W  D3,D1
    CALL    P.SMALLNUM  ; 'fifteen' 
    SCN 509     ; 'thargs exactly!'
80$
    POPSP
90$
    RET


*-------------------------------------------

    XDEF    SP.WHICHONE

SP.WHICHONE

    XREF    Denotation_SPELLS,WhichOneFlag

* This is incredibly    DANGEROUS!!!
*
* DONT, change this or put anymore code here without consultation.

* EXIT
*   EQ <=> special did something (generated some text)
*       so dont print the which one message
*       but continue.


    MOVE.L  A1,-(A7)
    LEA Denotation_SPELLS(A4),A1
    DO  MEMBEROFSET     ;which spell?
    BNE.S   90$         ;no, continue (ret NE)
    SCN 466         ;which spell?
    ST  WhichOneFlag(A4)    ;need to tell the parser
    SETEQ               ;flag as done something
90$
    MOVE.L  (A7)+,A1        ;restore wo flags
    RET

*------------------------------------------


    END

