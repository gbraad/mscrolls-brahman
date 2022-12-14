***************************************************************************
*
*
*	fred23jr output from file '../jinxter'*
*
***************************************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.DATA2
*
*--------------------------------
 
	include "equates.i"
	include	"macros.i"
	include	"roomequ.i"
	include	"nounequ1.i"


	XDEF	PSUDAT,NPC.DATA,NPC.LIST

	XDEF	ValueTable




* Pseudo define (starts list of rooms containing object)

PS_DEF      MACRO    * noun identifier

            DC.B     0                    ;terminate previous list
			IFNE	GENERIC
	    DC.B	N\1>>8
	    DC.B	N\1&$FF
			ENDC
P_\1        EQU      *-PSUDAT             ;data to go in noundata
            XDEF     P_\1                 ;for linker

            ENDM

* Pseudo continue (the actual list of rooms)

PS_CONT     MACRO    * room [,room..]     ;NB max 9 rooms per line!

            PS_ROOM  \1
            PS_ROOM  \2
            PS_ROOM  \3
            PS_ROOM  \4
            PS_ROOM  \5
            PS_ROOM  \6
            PS_ROOM  \7
            PS_ROOM  \8
            PS_ROOM  \9

            ENDM

PS_ROOM     MACRO    * room               ;encode for one room (if there)

           IFNC     '\1',''

            DC.B     RN\1

           ENDC

            ENDM


PSUDAT


	PS_DEF	AUTODOORS
	PS_CONT	ONBUS,ONBUS

	PS_DEF	LAWN
	PS_CONT	FIELD1,FRONTGARDEN1,BACKGARDEN1,ORCHARD,TOP_MOUNTAIN

	PS_DEF	GARDENGATE
	PS_CONT	FRONTGARDEN1,COUNTRYLANE1

	PS_DEF	FLOOR
	PS_CONT	KITCHEN2,CLOCK,WITCH_CHAMBER,CASTLE_KITCHEN

	PS_DEF	RAMBLINGSHED
	PS_CONT	BOATHOUSE,BRIDGE1

	PS_DEF	HEARTH
	PS_CONT	BANQUET_HALL,CHIMNEY

	PS_DEF	GREEN_DOOR
	PS_CONT	CONSERVATORY,BACKGARDEN1

	PS_DEF	COUNTRYSIDE
	PS_CONT	DININGROOM1,COUNTRYLANE1,INFINITE1,INFINITE2,FIELD2

	PS_DEF	TREES
	PS_CONT	BACKGARDEN1,ORCHARD,WOOD1,WOOD2

	PS_DEF	FENCE
	PS_CONT	WOOD1,FIELD2,ORCHARD

	PS_DEF	BRIDGE
	PS_CONT	BOATHOUSE,ORCHARD,BRIDGE1

	PS_DEF	PATH
	PS_CONT	FRONTGARDEN2,WOOD2

	PS_DEF	KITCHEN_DOOR
	PS_CONT	KITCHEN2,FRONTGARDEN2

	PS_DEF	WOODEN_DOOR
	PS_CONT	STUDY1,BACKGARDEN2

	PS_DEF	TRAPDOOR1
	PS_CONT	KITCHEN2,CELLAR2

	PS_DEF	HOLLY_BUSH
	PS_CONT	BACKGARDEN2,HOLLYBUSH

	PS_DEF	NATURESTUFF
	PS_CONT	BACKGARDEN2,HOLLYBUSH,FRONTGARDEN1

	PS_DEF	SLIDINGDOOR
	PS_CONT	BOATHOUSE,CANOE_MOORING

	PS_DEF	RED_DOOR
	PS_CONT	OFFICE,LIBRARY2

	PS_DEF	WATER2
	PS_CONT	CORRIDOR1,OFFICE,LIBRARY2,TOPOF_LIBRARY2

	PS_DEF	BAKERYSTEPS
	PS_CONT	BAKERY,BAKERY_OVEN

	PS_DEF	GIRDER
	PS_CONT	CLOCK,GIRDER_ROOM

	PS_DEF	BEAM
	PS_CONT	ON_PLATFORM,END_OF_BEAM

	PS_DEF	MODELS
	PS_CONT	FAIRGROUND,CAROUSEL

	PS_DEF	LOCOMOTIVE
	PS_CONT	PLATFORM1,PLATFORM2,PLATFORM3,RAILWAY_TRACK,TRAIN_ROOF,PLATFORM_BRIDGE,OTHER_PLATFORM

	PS_DEF	CASTLE
	PS_CONT	QUADRANGLE,WALLED_GARDEN,TOP_MOUNTAIN,CASTLE_PARAPETS

	PS_DEF	LAKE2
	PS_CONT	CANOE_MOORING,PAGODA,ONLAKE,WOOD2,INF_UNDERWATER,UNDERWATER,OVER_HATCH

	PS_DEF	WEEDS
	PS_CONT	OTHER_PLATFORM,STREAM1

	PS_DEF	PRECIPICE
	PS_CONT	CASTLE_PARAPETS,TOP_MOUNTAIN

	PS_DEF	WINDOWS2
	PS_CONT	WITCH_CHAMBER,BANQUET_HALL,JEWEL_ROOM

	PS_DEF	WITCH_DOOR1
	PS_CONT	HALLWAY,WITCH_CHAMBER

	PS_DEF	WITCH_DOOR
	PS_CONT	HALLWAY,WITCH_CHAMBER

	PS_DEF	CMAKERS_DOOR
	PS_CONT	CAROUSEL,CLOCKMAKERS

	PS_DEF	OUTSIDE
	PS_CONT	FRONTGARDEN1,BACKGARDEN1,COUNTRYLANE1,COUNTRYLANE2,FIELD1,FIELD2,WOOD1,ORCHARD
	PS_CONT	STREAM1,BRIDGE1,FRONTGARDEN2,BACKGARDEN2,CANOE_MOORING,LAKE1,PAGODA,WOOD2
	PS_CONT	FAIRGROUND,ON_PLATFORM,END_OF_BEAM,BEERGARDEN,TOP_MOUNTAIN,WALLED_GARDEN,CAROUSEL,CASTLE_PARAPETS
	PS_CONT	BALCONY,INFINITE1,INFINITE2,ONLAKE,RAILWAY_TRACK,PLATFORM1,PLATFORM2,PLATFORM3
	PS_CONT	OTHER_PLATFORM,PLATFORM_BRIDGE

	PS_DEF	DOOR
	PS_CONT	ARCHWAY,BANQUET_HALL

	PS_DEF	DOOR1
	PS_CONT	ANTECHAMBER,LIBRARY4

	PS_DEF	YOUR_HOUSE
	PS_CONT	FRONTGARDEN1,KITCHEN1,BEDROOM1,BATHROOM1,LIBRARY1,CONSERVATORY,HALLWAY1,BACKGARDEN1

	PS_DEF	FRONT_DOOR
	PS_CONT	FRONTGARDEN1,HALLWAY1

	PS_DEF	BSTOP
	PS_CONT	COUNTRYLANE1,INFINITE1,INFINITE2

	PS_DEF	XAMS_HOUSE
	PS_CONT	FRONTGARDEN2,KITCHEN2,DININGROOM1,STUDY1,CELLAR2,BACKGARDEN2

	PS_DEF	WALL
	PS_CONT	BEDROOM1,BATHROOM1,LIBRARY1,CONSERVATORY,DININGROOM1,OFFICE,DIVERS_ROOM,LIBRARY2
	PS_CONT	LIBRARY4,BEERGARDEN,OUTSIDE_STATION,TOP_MOUNTAIN,WALLED_GARDEN,DUNGEON,QUADRANGLE,BANQUET_HALL
	PS_CONT	ANTECHAMBER,CONNECTING_PASSAGE,POSTOFFICE,HALLWAY1,KITCHEN1,CASTLE_KITCHEN,AIRLOCK

	PS_DEF	AIRLOCK_DOOR
	PS_CONT	AIRLOCK,CORRIDOR1

	PS_DEF	HATCH
	PS_CONT	OVER_HATCH,AIRLOCK

	PS_DEF	PODOOR
	PS_CONT	POSTOFFICE,FAIRGROUND

	PS_DEF	PUBDOOR
	PS_CONT	FAIRGROUND,PUB

	PS_DEF	BATHROOMDOOR
	PS_CONT	BATHROOM1,BEDROOM1

	PS_DEF	KITCHENDOOR
	PS_CONT	KITCHEN1,BEDROOM1

	PS_DEF	LEATHERDOOR
	PS_CONT	LIBRARY1,CONSERVATORY

	PS_DEF	CHANDELIER
	PS_CONT	LIBRARY2

	PS_DEF	TELEGRAPH_DOOR
	PS_CONT	PLATFORM3,POST_DEPOT

	PS_DEF	WATER
	PS_CONT	STREAM1,CANOE_MOORING,LAKE1,PAGODA,INF_UNDERWATER,UNDERWATER,UNDERWATER2,OVER_HATCH
	PS_CONT	ONLAKE,DUNGEON,WOOD2,AIRLOCK,WATERFALL

	PS_DEF	TOWER
	PS_CONT	HALLWAY,BALCONY,TOWER_ENTRANCE,SPIRAL_STAIRCASE

	PS_DEF	SOLID_DOOR
	PS_CONT	GAOL,DUNGEON

	PS_DEF	INNER_HATCH
	PS_CONT	DUNGEON,IN_HATCH

	PS_DEF	OUTER_HATCH
	PS_CONT	IN_HATCH,CASTLE_KITCHEN

	PS_DEF	PSEUDOCLOCK
	PS_CONT	FAIRGROUND,CAROUSEL,END_OF_BEAM,CLOCK,ON_PLATFORM

	PS_DEF	STAIRCASE3
	PS_CONT	CASTLE_KITCHEN,BANQUET_HALL

	PS_DEF	PSEUDOPO
	PS_CONT	FAIRGROUND,POSTOFFICE


	DC.B	0
	ALIGN
ValueTable
	DC.W	1,NAQUITANIAN_FERG
	DC.W	1,NDRINK
	DC.W	1,NTRAIN_TICKET
	DC.W	1,NSILVER_COIN
	DC.W	2,NTWOFERG


     DC.L    0                   ;long word terminator


*--------------------------------
*
* NPC ADDITIONAL DATA:
*
* BYTE #0 - MOTIVATION MODIFIER
*           FOR NPC, THRESHOLD 7F
*           (NOT USED (YET)) 
*
* BYTE #1 - NO. OF QUESTIONS TILL
*           NPC GETS PISSED OFF
*           (ALSO N/A (YET))
*
* BYTE #2 - EMOTION,ARTICLE,
*           CLOTHING & SEX
*
*     TOP NIBBLE - 0-F
*  
* 0 - NULL     1 - HAPPY
* 2 - ANGRY    3 - VIOLENT
* 4 - ASLEEP   5 - UNCONSIOUS
* 6 - PEACEFUL 7 - EXCITED
* 8 - FRIENDLY 9 - DRUNK
* A - SAD      B - TIRED
* C - SUFFERING
*
*
*     BOTTOM NIBBLE:
*
* BIT #0 - SEX: MALE=1,FEMALE=0
* BIT #1 - CLOTHING: 1=SUPPRESS
* BIT #2 - 1=> is animal
* BIT #3 - ARTICLE - 
*
* 1 - NONE E.G. HONEST JOHN,ERIK
* 0 - NORM E.G. A GURU/THE GURU
*
*
* BYTE #3 - NPC PAUSED (IF <0)
*
*
*--------------------------------
 
   
NPC.DATA
 
       include "npcdata.i"
        
	DC.L	0

NPC.LIST

	include	"npclist.i"
	DC.L	0
      END


