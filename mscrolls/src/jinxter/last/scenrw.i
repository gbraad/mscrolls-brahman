***************************************************************
*
*   @(#)scenrw.i    2.1
*   11/19/87
*
***************************************************************

*
* scenrw.asm - scenario read/write bits
*

* included by data1.asm
    include "roomequ.i"

    IFND    VNLOOK
    include "verbequ.i"
    ENDC



* Green Magic (or whatever!!!!!!!!!!!!!!!!!!!!!)



    XDEF    OtherDescTable,FirstGlanceTable

    XDEF    FGT_Pubcrowd,FGT_Platform,FGT_Beam,FGT_Doors,FGT_Rope

    XDEF    FlagBull,FlagFence,FlagHoldFence
    XDEF    KnockPos,KnockP2,FlagDialled
    XDEF    GotSpecs,GoneCloud
    XDEF    FloodPos    ;,DRoomFlooded
    XDEF    AquaTank,DoorCreaked,DoorOiled,DwarfDone
    XDEF    DracItem
    XDEF    CanoeLaunched,TypeOfWeather,WeatherTime
*   XDEF    CurrentWind,Smoulder,StartFireTable,EndFireTable
    XDEF    TelephoneRung,RingCount

    XDEF    FlagRunover,GuardianHere,ReadDocument

    XDEF    SquareData,RowSize,GapPos,RowData ; For magic square puzzle
    XDEF    SquareDataEnd
    XDEF    HCollapse,WitchPlace,WitchBeen,FlagHearth
    XDEF    PostMistressGone

*********************************************************
*
* TBL.MOVE - info for SP.MOVE. (was SP.MOVE.GET)
*
* table of noun#,addr. of code to cope
*
* format:
*
*   DC.W    noun#
*   DC.L    addr. of code (hassles due to Metacomco/Vax/Amiga assem)
*
*   ..
*
*   DC.W    0,0,0       ;to end...
*

*TBL.MOVE           !!!!    Unused in magic     !!!!

;   DC.W    NCHALICE
;   DC.L    SP.MOVE.CHALICE


*   DC.W    0,0,0

*********************************************************
*
* TBL_FirstGlance - used by LOOK to add bits to the end
*                    of descriptions of nouns the first
*                    time u look at them.
*
*                    if the scn msg# is >$8000 then it is
*                    ALWAYS appended, not just first time
*
* Format:
*       DC.W    Scenario msg#[+HI],noun#
*        ..
*
*       DC.W    0       ;end of table
*
*********************************************************

    XDEF    FGT_Infinite,FGT_HatchOpened
    XDEF    FGT_Debris

FirstGlanceTable

    DC.W    14,NBOOKSHELVES

FGT_Platform
    DC.W    HI+282,NON_PLATFORM
FGT_Beam
    DC.W    HI+283,NEND_OF_BEAM
FGT_Infinite
    DC.W    HI+209,-1   ;-1 gets fodded with infinite1/2/clane1

FGT_HatchOpened
*KEEP THESE two together (ajs)
    DC.W    HI+172,NTICKET_OFFICE   ; the hatch is open
    DC.W    HI+172,NTRAIN_STATION   ; the hatch is open in description
FGT_Pubcrowd
    DC.W    HI+639,NPUB     ; the customers in the pub.
FGT_Debris
    DC.W    HI+379,NSHIPWRECK   ; cylinder glints in debris
FGT_Doors   
    DC.W    HI+389,NHALLWAY     ; the two doors...
FGT_Rope
    DC.W    HI+463,NDUNGEON     ; nearby hangs a rope...

    DC.L    0       ;end of table
 
*********************************************************
*
* TBL_OtherDesc  -   used by LOOK to potentially replace
*            the normal description with the 
*            Scenario message number in this one.
*
*
* Format:
*       DC.W    Scenario msg#,noun#
*        ..
*
*       Zero Terminated!
*
**********************************************************

ODTMacro    MACRO   *msg#,noun#
        DC.W    \1
ODT_\2      DC.W    N\2+HI
        XDEF    ODT_\2
        ENDM


OtherDescTable

*   ODTMacro    252,DOOR7

    ODTMacro    39,FENCE
    ODTMacro    190,BAKER
    ODTMacro    199,RUNNERS
    ODTMacro    205,SLIDINGDOOR
    ODTMacro    301,LIBRARY2
    ODTMacro    592,MATCH
    ODTMacro    592,MATCH2
    ODTMacro    592,MATCH3
    ODTMacro    592,MATCH4
    ODTMacro    731,DIRTMOUND
    ODTMacro    92,CMAKERS_WINDOW
    ODTMacro    122,PUBHEARTH   ; cold hearth

    DC.L    0


*************************************************************
*
* ReachTable
*
* table of objects which may be reached when seated
* (must also be valid)
*
* format    Reach   object noun#,seat noun#
*            ..
*           DC.L    -1      ;long word -1 terminator
*
    XDEF    ReachTable

Reach   MACRO
    XDEF    RT_\1
RT_\1   DC.W    N\1,N\2
    ENDM

ReachTable

*   Reach   UPBUTTON,BED2
    Reach   UMBRELLA,BEAM       ; so you can get brolly from beam
    Reach   LADDER,STOOL        ;so u can climb onto ladder frm stool
    Reach   RUNGS,STOOL

    DC.L    -1          ;terminator


**************************************************************
*
* NoReachTable
*
* table of objects which u cannot reach (above table, ReachTable, overrides this
*
* verb is one which allows u to reach object
*
* format     NoReach    noun#,verb#
*              ..
*            DC.L       0       ;0 terminator

    XDEF    NoReachTable

NoReach MACRO
      IFNC  '\2','-1'
    DC.W    VN\2
    XDEF    NRT_\1\2
NRT_\1\2
      ENDC
      IFC   '\2','-1'
    DC.W    -1
    XDEF    NRT_\1
NRT_\1
      ENDC
    DC.W    N\1
    ENDM

NoReachTable

*   NoReach WAX,LOOK        ;Example

    NoReach CLOCKMAKER,THROW
    NoReach GIRDER,PUT      ;So u can put ladder against it
    NoReach HOLE,LOOK       ;summit unreachable later (yuk)
    NoReach KITCHEN_FAN,THROW
    NoReach NICECLOUD,LOOK
    NoReach OIL_LAMP,THROW
    NoReach STOOL,THROW
    NoReach CMAKER_FIRE,LOOK
    NoReach DISTANT_RAINMAN,LOOK
    NoReach DISTANT_SUNMAN,LOOK

    DC.W    VNLOOK              ;These need to be reachable
    XDEF    NRT_RUNGSLOOK           ;initially.
NRT_RUNGSLOOK       
    DC.W    0               ;Null noun - gets fodded in
    DC.W    VNEXAMINE
    XDEF    NRT_RUNGSEXAMINE
NRT_RUNGSEXAMINE
    DC.W    0               ;Null noun - gets fodded in

    DC.L    0

*--------------------------------
*
*  TBL.ATTA
*
* FORMAT:     /NOUN#,/MSG#
*
* IF MSG# HIGH BIT IS LOW U DIE!
*
*--------------------------------
  
;TBL.ATTA

*-----------------------------------



*
*
* R/W Scenario flags etc
*
*

    XDEF    TimesPressed,PostItem,PostDest,Postage
    XDEF    CanoeFilled,Hooligan,FlagPubHearthCold
    XDEF    AquaLive                                         

PostItem    DC.W    0   ; Item to be posted...
PostDest    DC.W    0   ; ... and its destination
Postage     DC.W    0   ; How much it will cost to post!
KnockPos    DC.W    0   ; Which 'knock' are we doing?
KnockP2     DC.W    0   ; Which 'knock' are we doing (second time)
FloodPos    DC.W    0   ; How flooded is the office?
AquaTank    DC.W    85  ; Air left in tanks of aqualung
DracItem    DC.W    0   ; Item currently dracmaazaed
AquaLive    DC.B    0   ; using aqualung 
FlagBull    DC.B    0   ; NE when bull disposed of!
FlagFence   DC.B    0   ; NE When fence fence has been cut
FlagHoldFence   DC.B    0   ; NE when holding fence?
FlagDialled DC.B    0   ; NE When postman has been called
GotSpecs    DC.B    0   ; NE when specs given to player by magpie

Hooligan    DC.B    0   ; NE when player has been rowdy in Pub
FlagPubHearthCold DC.B  0   ; NE when player has frozen the pub hearth

;DRoomFlooded   DC.B    0   ; NE when divers room is flooded
DoorCreaked DC.B    0   ; NE If the sliding door has creaked
DoorOiled   DC.B    0   ; NE when door has been oiled
DwarfDone   DC.B    0   ; NE When the dwarf bit has been done

TelephoneRung   DC.B    0   ; NE When Phone has started ringing.
RingCount   DC.B    0   ; Number of times phone has rung.

TimesPressed    DC.B    0   ;# of times bus bell has been pressed

FlagRunover DC.B    0   ; bit 0 NE => have been run over!
GuardianHere    DC.B    0   ; NE when guardian is in front garden..
ReadDocument    DC.B    0   ; NE when document has been read
CanoeLaunched   DC.B    0   ; NE when canoe has been launched


TypeOfWeather   DC.B    0   ;up to 8 types of weather bitwise
WeatherTime DC.B    0   ;only consider weather every 8 moves
CanoeFilled DC.W    0   ; contains the noun of what is filled canoe
;CurrentWind    DC.B    0   ;Current wind direction in DIR_ format
;Smoulder   DC.B    0   ;Gets room # where arsonist struck fodded in

;StartFireTable DC.L    0,0,0,0,0,0 ;The firetable represents each room
;EndFireTable               ;on a bit by bit basis  

    XDEF    MaxWest,MaxEast,LanePos
MaxWest     DC.W    0       ;How far west down Nightingale Lane
MaxEast     DC.W    0       ;we've gone (and east)
LanePos     DC.W    0       ;where we are in the lane (signed)

SquareData:
Sqd1:   DC.B    '_+_+_+_+_+_^'
Sqd2:   DC.B    '__-------__^'
Sqd3:   DC.B    '+_:1_8_5:__^'
Sqd4:   DC.B    '+_:6_9__---^'
Sqd5:   DC.B    '+_:7_2_3_4:^'
    DC.B    '__---------^',0

    IFNE    0
Sqd1:   DC.B    ' +-+---+---+-+    ^'
Sqd3:   DC.B    '+:[1] [8] [5]:    ^'
    DC.B    ' :___________:    ^'
Sqd4:   DC.B    '+:[6] [9] [ ]:    ^'
    DC.B    ' :___________:----^'
Sqd5:   DC.B    '+:[7] [2] [3] [4]:^'
    DC.B    ' -----------------^',0
    ENDC
SquareDataEnd

RowSize EQU Sqd4-Sqd3

GapPos: DC.B    (Sqd4+7)-SquareData

SquareRow MACRO
    DC.B    (\1)-SquareData,(\2)-SquareData,(\3)-SquareData,(\4)-SquareData
    ENDM

; Each squareRow macro defines the position of the lamp, and the positions
; of the squares that make up the row.
RowData:
    SquareRow Sqd1+1,Sqd3+3,Sqd4+5,Sqd5+7
    SquareRow Sqd1+3,Sqd3+3,Sqd4+3,Sqd5+3
    SquareRow Sqd1+5,Sqd3+5,Sqd4+5,Sqd5+5
    SquareRow Sqd1+7,Sqd3+7,Sqd4+7,Sqd5+7
    SquareRow Sqd1+9,Sqd3+7,Sqd4+5,Sqd5+3
    SquareRow Sqd3,Sqd3+3,Sqd3+5,Sqd3+7
    SquareRow Sqd4,Sqd4+3,Sqd4+5,Sqd4+7
    SquareRow Sqd5,Sqd5+3,Sqd5+5,Sqd5+7
    DC.B    -1

    XDEF    TimesCalled,AskedNTimes,RungBell,DoorsOpen,WhereToGetOff

TimesCalled DC.B    0       ;Used by Do_Bus to suss what happens
                    ;next.

BaseTMess   EQU 113     ;SCN # of inspector's messages
    XDEF    BaseTMess

AskedNTimes DC.W    BaseTMess
RungBell    DC.B    0
DoorsOpen   DC.B    0

InitGetOff  EQU 32      ;Initial position in infinite lane

WhereToGetOff   DC.W    InitGetOff


    XDEF    PissInLoo,Underwater_x,Underwater_y
    XDEF    WaterMax_x,WaterMin_x
    XDEF    WaterMax_y,WaterMin_y
    XDEF    AirlockFlooded,UnderWater
    XDEF    OfficeFlooded,HandlePos

PissInLoo   DC.W    0
Underwater_x    DC.W    0
Underwater_y    DC.W    0
WaterMax_x  DC.W    0
WaterMin_x  DC.W    0
WaterMax_y  DC.W    0
WaterMin_y  DC.W    0
AirlockFlooded  DC.B    1   ;initially flooded
UnderWater  DC.B    0   ;when player is underwater
OfficeFlooded   DC.B    0

HandlePos   DC.B    0   ; All handles start UP! (1 byte per handle)

;   XDEF    PostMistressAbout
    XDEF    AppliedForJob,BakerFed,BakerAngry
    XDEF    BakerFollows,GotCharm4,CharmInCake
    XDEF    BurntChucked
    XDEF    ShutTheDoor,IntroPMistress

AppliedForJob       DC.B    0   ; NE When player has asked baker 
                    ; about the job
BakerFed        DC.B    0   ; NE if baker has been
BakerAngry      DC.B    0   ; NE If he was fed the burnt cake
BakerFollows        DC.B    0   ; NE If he's following you downstairs
GotCharm4       DC.B    0   ; NE When flour sack opened!
CharmInCake     DC.B    0   ; NE if charm is in the cake.
BurntChucked    DC.B    0   ; burnt bread is chucked out by baker
;PostMistressAbout  DC.B    0   ;NE when she's gone
ShutTheDoor     DC.B    0   ;NE when just left or entered PO
IntroPMistress      DC.B    0   ;NE When she spots you entering


    XDEF    BoozedUp
BoozedUp        DC.B    0   ; >Player starts life sober...
                    ; this increments with each drink.

    XDEF    MaxWaits
MaxWaits        DC.B    0   ;Keeps track of how long p/man waits

    XDEF    LUCK_

LUCK_           DC.W    0   ;Amount of luck you have

    XDEF    LastSpiel

LastSpiel   DC.W    0       ;So guardian doesn't repeat hisself

*********************************************************
*
* LongLookTable
*
* Format of table:
*
*   DC.W    scen msg# [+HI] [+BIT14], noun# [+ HI]
*   DC.W    ...
*   DC.L    0
*
* The nouns wi this message are factored in the list
* and printed as a group wi the one message.
* If the scenario msg is hi-bit set, then the 'isare' is suppressed between
* the name of the objet and the modifying text.
* If the msg# is BIT14 set, then the modifying text is NOT produced 
* when the object is LOOKed at.
*
**********************************************************

    IFD wantLONGLOOK
    XDEF    LongLookTable,LongLookTable.Blank

    ALIGN   


LongLookTable

    DC.W    190,NPOSTMISTRESS+HI
    DC.W    602+HI,NCHANDELIER
    DC.W    603+HI,NTOP_CHANDELIER
    DC.W    332+HI+BIT14,NPELICAN
    DC.W    334+HI+BIT14,NUNICORN
    DC.W    335+HI+BIT14,NKERULGIAN_DRAGON
    DC.W    337+HI+BIT14,NFIRE_ENGINE
    DC.W    336+HI+BIT14,NSTATE_CARRIAGE
    DC.W    86+HI,NCANOE
LongLookTable.Blank
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0
    DC.L    0


    DC.L    0       ;fencepost

    ENDC    ;wantLONGLOOK
************************************************************


    XDEF    NOSINKMSG,FlagNothingDoing

NOSINKMSG       DC.B    0
FlagNothingDoing    DC.B    0   ;NE=> decode won't let you!

    XDEF    EventClock,OLDROOM,OldUnderwater_x,OldUnderwater_y

EventClock      DC.W    0   ;# of times several timers are called
OLDROOM         DC.W    0   ;Last room # guardian was in
OldUnderwater_x     DC.W    0   ;So we can tell if player has...
OldUnderwater_y     DC.W    0   ;moved underwater.

*****
* Flags for the busking puzzle
    XDEF    HatForCollecting,TapDancing,PlayHarmonica
    XDEF    CoinContainer,BuskDoneTable
    XDEF    Stationmaster_givein,LoseTicketState
    XDEF    TriedToBuy
    XDEF    TrainBitOver,GuardWantsTicket
    XDEF    GuardSeenTicket,TrainOnRoute
    XDEF    LieDownOnTrain
    XDEF    PlayerInVan,BuskingOnTrain
    XDEF    TrainMovesOff,GivenCoin
    XDEF    UnicornFlyzzed,UnicornFriendly
    XDEF    GaolerAlerted,CandleLifeLeft,CandleUnderRope
    XDEF    RopeTied,BlockToSlide,DomeHasShattered
    XDEF    XamRescued,HorrorPlace,PhoneAnswered,HTable
    XDEF    CoinUsed,WarmedWith,StomachLined,FutureBits
    XDEF    FlyzzedFuture,CurrentFuture

StomachLined        DC.B    0
HatForCollecting    DC.B    0
TapDancing      DC.B    0
CoinContainer       DC.W    0
CoinUsed        DC.W    0
PlayHarmonica       DC.B    0
Stationmaster_givein    DC.B    0
LoseTicketState     DC.B    0
TriedToBuy      DC.B    0
TrainBitOver        DC.B    0
GuardWantsTicket    DC.B    0
GuardSeenTicket     DC.B    0
TrainOnRoute        DC.B    0
LieDownOnTrain      DC.B    0
PlayerInVan     DC.B    0
BuskingOnTrain      DC.B    0
TrainMovesOff       DC.B    0
GivenCoin       DC.B    0
UnicornFlyzzed      DC.B    0
UnicornFriendly     DC.B    0
GaolerAlerted       DC.B    0
CandleLifeLeft      DC.B    60  ; 60 moves in total
CandleUnderRope     DC.B    0
RopeTied        DC.B    0   ;dungeon rope
BlockToSlide        DC.B    0
DomeHasShattered    DC.B    0
XamRescued      DC.B    0
HorrorPlace     DC.B    0   ; NE => you are in a horror place
PhoneAnswered       DC.B    0   ; You have answered the phone.
                    ; or it has stopped
FlyzzedFuture       DC.B    0
WitchBeen       DC.B    0
FlagHearth      DC.B    0   ; NE => something on hearth
GoneCloud       DC.B    0
CurrentFuture       DC.W    0
WitchPlace      DC.L    0
FutureBits      DC.W    %1000011001000010   ; futures (shuffled)
WarmedWith      DC.W    0   ;What you warmed the key with

    ALIGN

HCollapse:
    DC.W    NTABLE
    DC.W    NBED
    DC.W    NLAVATORY
    DC.W    NBOOKSHELVES
    DC.W    NDESK
    DC.W    NFRIDGE

*** PLEASE NOTE THE BYTE CONVENTION HERE....
HTable:
    DC.B    RNLIBRARY1,82
    DC.B    RNCONSERVATORY,83
    DC.B    RNBEDROOM1,68
    DC.B    RNBATHROOM1,67
    DC.B    RNKITCHEN1,78
    DC.B    RNHALLWAY1,85
    DC.B    RNBACKGARDEN1,91
    DC.B    0

BuskDoneTable
    
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0
    DC.B    0       ;16 states

*----------

    XDEF    DirtGone,MatchesSpent,SpareTank,LibraryFlooded

DirtGone    DC.B    0       ;NE when dirt mound removed
PostMistressGone    DC.B    0   ; NE when she is gone for good
        ALIGN
MatchesSpent
        DC.W    0
        DC.W    0
        DC.W    0
        DC.W    0       ;4 matches
SpareTank   DC.B    50      ;air in spare tank
LibraryFlooded  DC.B    0       ;for opening red door

*--------------------------------------

        XDEF    EventRoom,EventNum,EndEventMSG,FlagRoomDone
        XDEF    FlagEventDone,CloudAround,FirstCloud,FrozenCloud

EventRoom   DC.W    0       ;Room # where event occured
EventNum    DC.W    0       ;Current event number
EndEventMSG DC.W    0       ;SCN # which ends current event
FlagEventDone   DC.L    0       ;Bits set to flag event occured
FlagRoomDone    DC.B    0       ;All bits used by RandGuardian
******
CloudAround DC.B    0       ; EQ=> no cloud around...
FirstCloud  DC.B    0       ; NE=> never seen cloud.
FrozenCloud DC.B    0       ; NE=> frozen.


************************************************

    XDEF    FlagTrapSet,FlagMouseDead,FlagHassleFromFuckHead

FlagTrapSet DC.B    0       ;NE => cheese is in trap + it is set
FlagMouseDead   DC.B    0       ;NE => mouse is deceased
FlagHassleFromFuckHead  DC.B    0   ;NE=> u got thrown off bus

    XDEF    DemolishedBakery

DemolishedBakery    DC.B    0   ;NE=> bakery blown up

*------------------------------------------

    IFNE    YCHEAT&VAX
    XDEF    OldScore
OldScore        DC.W    0   ;For SP.Inform
    ENDC

    XDEF    FlagWitchDoorOpen

FlagWitchDoorOpen   DC.B    0   ;Bit 7 set when one of the doors open

    XDEF    ClockMakerGone,LadderLeantAgainst,LadderOrientation
    XDEF    FlagGameOver,ObjectUnderRope

ClockMakerGone      DC.B    0   ;NE when he's run away
ObjectUnderRope     DC.W    0   ; containes noun number of object under rope
LadderLeantAgainst  DC.W    0   ;Noun # of wot u leant ladder against
LadderOrientation   DC.B    0   ;0=> right way up NE => isn't!
FlagGameOver        DC.B    0   ;NE when u've won!

*------------------------------------------
    
    IFD wantADDITIONALDESCRIPTIONS

    XDEF    AdditionalDescription.TBL

AdditionalDescription.TBL

* Additional descriptions are those that require to be appended to the
* main room (or item) description for specials etc.
*
* Entries are made in the table:    AdditionalDescription.TBL
*
* Format of table
*
*   DC.W    noun#       ; noun being described.
*   DC.W    msg#,noun2# ;print msg# if (noun2#) local
*   ...
*   DC.W    0

* IF noun2# == 0 then msg# is always appended.
* This is so that specials can change the message printing criterion

    DC.L    0

    ENDC    ;wantADDITIONALDESCRIPTIONS

*------------------------------------------


* an obscure bit of a table which ken isn't going to tell people about
* cos then all our games will look like infocom ones
* Please make sure that the object is related to the room.

* UntilDisturbedTable
*

    XDEF    UntilDisturbedTable

UntilDisturbedTable

    DC.W    NTABLECLOTH,NCONSERVATORY,13
    DC.W    NSHEARS,NBACKGARDEN1,15+HI
    DC.W    NGLOVES,NBACKGARDEN1,18
    DC.W    NBULL,NFIELD1,19
    DC.W    NBOOK1,NBATHROOM1,58
    DC.W    NDIRTMOUND,NSTEPS1,96
    DC.W    NUMBRELLA,NRAIN_WEATHERMAN,285
    DC.W    NTOP_HAT,NRAIN_WEATHERMAN,640
    DC.W    NPUBLICAN,NPUB,349
    DC.W    NBAKER,NBAKERY,103
    DC.W    NTWOFERG,NBOTTOM_WELL,629
    DC.W    NJAR,NSTREAM1,620
    DC.W    NFLIES,NWOOD1,797
    DC.W    NSTOOL,NCLOCKMAKERS,796
    DC.W    NPLASTICTUB,NBACKGARDEN2,465
    DC.L    0

*-----------------------------------------------------
*
* table of scn msg's produced  when you look at a neigboughing room

    XDEF    LookNextRoomTable,BakeryLook.MSG

LookNextRoomTable

    DC.W    628,NPUB
    DC.W    641,NBOTTOM_WELL
*   DC.W    642,NPOSTOFFICE     BONG! if door is shut!!!!!!!!
    DC.W    828,NCLOCKMAKERS    ; only valid when door is open

BakeryLook.MSG
    DC.W    643,NBAKERY

    DC.L    0
