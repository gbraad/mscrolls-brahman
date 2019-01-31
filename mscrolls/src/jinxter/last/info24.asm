***************************************************************************
*
*
*	fred23jr output from file '../jinxter'*
*
***************************************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* INFO24.ASM	- location+type info for scenary nouns
*
*--------------------------------
 
	include "equates.i"
	include	"nounequ1.i"

	XDEF	Info24Table

INFO24	MACRO	* type,location
	DC.W	\1<<12!N\2
	ENDM

Info24Table

	INFO24	1,ONBUS
	INFO24	3,ONBUS
	INFO24	3,DECKCHAIR
	INFO24	3,HAND
	INFO24	3,KITCHEN1
	INFO24	3,KEY1
	INFO24	5,CONSERVATORY
	INFO24	3,CONSERVATORY
	INFO24	1,COUNTRYLANE2
	INFO24	5,FIELD1
	INFO24	3,FIELD1
	INFO24	3,FIELD2
	INFO24	3,FENCE
	INFO24	3,FENCE
	INFO24	6,STREAM1
	INFO24	5,STREAM1
	INFO24	6,STREAM1
	INFO24	1,FRONTGARDEN2
	INFO24	3,KITCHEN2
	INFO24	5,KITCHEN2
	INFO24	5,KITCHEN2
	INFO24	3,CANDLE
	INFO24	1,STUDY1
	INFO24	3,STUDY1
	INFO24	6,TRAPDOOR1
	INFO24	1,CELLAR2
	INFO24	1,CELLAR2
	INFO24	3,HOLLYBUSH
	INFO24	3,HOLLYBUSH
	INFO24	3,BOATHOUSE
	INFO24	6,FISHINGBOAT
	INFO24	6,FISHINGBOAT
	INFO24	6,CORRIDOR1
	INFO24	6,CORRIDOR1
	INFO24	6,BOTTOM_WELL
	INFO24	2,BOTTOM_WELL
	INFO24	1,POUCH
	INFO24	3,DIVERS_ROOM
	INFO24	3,DIVERS_ROOM
	INFO24	4,LIBRARY2
	INFO24	1,CAROUSEL
	INFO24	1,CAROUSEL
	INFO24	1,UNICORN
	INFO24	1,UNICORN
	INFO24	5,KERULGIAN_DRAGON
	INFO24	1,KERULGIAN_DRAGON
	INFO24	5,PELICAN
	INFO24	5,PELICAN
	INFO24	5,PELICAN
	INFO24	1,PELICAN
	INFO24	1,STATE_CARRIAGE
	INFO24	1,STATE_CARRIAGE
	INFO24	1,BAKER
	INFO24	6,NICECLOUD
	INFO24	6,NICECLOUD
	INFO24	1,BEAM
	INFO24	3,RAIN_WEATHERMAN
	INFO24	3,RAIN_WEATHERMAN
	INFO24	3,SUN_WEATHERMAN
	INFO24	3,PUB
	INFO24	3,PUB
	INFO24	3,BAR
	INFO24	3,BAR
	INFO24	1,BEERGARDEN
	INFO24	1,BEERGARDEN
	INFO24	6,OUTSIDE_STATION
	INFO24	4,TRAIN_STATION
	INFO24	4,TRAIN_STATION
	INFO24	4,TRAIN_STATION
	INFO24	6,LOCOMOTIVE
	INFO24	5,LOCOMOTIVE
	INFO24	1,DUNGEON
	INFO24	1,TRESTLE_TABLE
	INFO24	3,CHIMNEY
	INFO24	6,CHIMNEY
	INFO24	1,DOOR
	INFO24	3,BANQUET_TABLE1
	INFO24	3,KEY4
	INFO24	3,POSTOFFICE
	INFO24	3,SAFE
	INFO24	5,TOP_CHANDELIER
	INFO24	1,POST_DEPOT
	INFO24	3,MACHINE
	INFO24	3,MACHINE
	INFO24	3,MACHINE
	INFO24	3,GARMENT
	INFO24	3,GARMENT
	INFO24	3,GARMENT
	INFO24	3,PLATFORM_BRIDGE
	INFO24	1,OTHER_PLATFORM
	INFO24	3,CASTLE_KITCHEN
	INFO24	3,CASTLE_KITCHEN
	INFO24	3,JEWEL_ROOM
	INFO24	3,WITCH
	INFO24	3,HOLE
	INFO24	3,HOLE
	INFO24	6,MOUSE_TRAP
	INFO24	1,CANOE_HOLE
	INFO24	1,KITCHEN_FAN
	INFO24	6,LECTERN
	DC.W	0		;terminate list (for printing)

	END
