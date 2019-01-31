******************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

************************************************
*
*  PICTURES  module to deal with all the code
*            needed to suss whether a picture
*            is required in this room
*
*
***********************************************



* Jinxter version.......


    include "roomequ.i"
    include "equates.i"
    include "macros.i"



Pict    MACRO   * pictno., room identifier, version
    DC.B    \1,RN\2,\3
    ENDM


    XDEF     PICTURE_INDEX,ClocksPicture
    XDEF     LodgePicture


PICTURE_INDEX

***** The waterfall is not a room -> it happens by specials. 
***** Ditto clockmakers
***** IF YOU ARE IN DOUBT SEE ANITA.... And she'll show you her pigeon
*                   impression.
*WaterPicture   EQU 10      ***** NUMBER FOR WATERFALL PICTURE

* It is a room now! (hugh2018)

ClocksPicture   EQU 25
LodgePicture    EQU 39
    
    Pict    2,FRONTGARDEN1,0    ;Deck-chair
    Pict    7,FIELD1,0          ;Bull
    Pict    3,FRONTGARDEN2,0    ;Xam's House
    Pict    13,DININGROOM1,0    ;Xam's Dining room
    Pict    6,CELLAR2,0         ;Cheese cellar (Hmmmmmm)
    Pict    5,HOLLYBUSH,0       ;Magpie
    Pict    1,BRIDGE1,0         ;Bridge!
    Pict    12,CANOE_MOORING,0  ;Lake
    Pict    4,CAROUSEL,0        ;Carousel!
    Pict    14,DUNGEON,0        ;Dungeon!
    Pict    8,CONSERVATORY,0
    Pict    9,TRAIN_STATION,0
    Pict    11,TOP_MOUNTAIN,0
    Pict    15,OVER_WRECK,0     ;Ship wreck
    Pict    16,ON_PLATFORM,0    ;Clock
    Pict    17,TOWER_ENTRANCE,0 ;Castle archway
    Pict    18,LIBRARY1,0       ;Your library
    Pict    30,ONBUS,1          ; onbus version2
    Pict    19,ONBUS,0          ;The bus
    Pict    20,WITCH_CHAMBER,0  ;Witch's bedroom
    Pict    25,DUMMY1,0         ;clockmakers shop

    IFEQ    IBM!MAC512!ST520
    Pict    21,STEPS1,0         ;The pagoda
    ENDC

    Pict    22,BAKERY_OVEN,0    ;Bakers Kitchen
    Pict    23,BANQUET_HALL,0   ;Castle hall
    Pict    24,LIBRARY4,0       ;Witches library
    Pict    26,ORCHARD,0
    Pict    27,JEWEL_ROOM,0
    
    IFEQ    IBM!MAC512!ST520!ATARI
    Pict    28,BEERGARDEN,0
    ENDC

    Pict    10,WATERFALL,0      ;over the waterfall
    
    Pict    29,WALLED_GARDEN,0
    Pict    31,LAKE1,1          ;on the lagoon
    Pict    32,WOOD2,1          ;conifer wood
    Pict    33,KITCHEN2,1       ;messy kitchen
    Pict    34,ARCHWAY,1        ;castle archway
    Pict    35,INF_UNDERWATER,1 ; underwater
    Pict    35,UNDERWATER2,1    ; underwater
    Pict    35,UNDERWATER,1     ; underwater
    Pict    36,INFINITE1,1      ; neverending lane
    Pict    36,INFINITE2,1      ; other neverending lane
    Pict    37,OFFICE,1         ; sacristy
    Pict    38,FAIRGROUND,1     ; village green
    Pict    39,LIBRARY2,1       ;green witch's lodge#1
    Pict    40,LIBRARY2,1       ;green witch's lodge#2
    Pict    41,LIBRARY2,1       ;green witch's lodge#3
    Pict    42,LIBRARY2,1       ;green witch's lodge#4
    Pict    43,LIBRARY2,1       ;green witch's lodge#5

    DC.W    -1          ;Terminator

       END

