***************************************************************************
*
*
*	fred23jr output from file '../guild'*
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

	INFO24	2,MOORING
	INFO24	3,JETTY
	INFO24	2,JETTY
	INFO24	4,JETTY
	INFO24	1,SCRUB1
	INFO24	1,SCRUB1
	INFO24	1,SCRUB2
	INFO24	1,SCRUB2
	INFO24	3,SCRUB2
	INFO24	3,SCRUB3
	INFO24	3,SCRUB3
	INFO24	1,SCRUB3
	INFO24	1,FOREST1
	INFO24	3,FOREST2
	INFO24	2,FOREST2
	INFO24	5,FOREST4
	INFO24	1,FOREST4
	INFO24	3,FIELD
	INFO24	3,FIELD
	INFO24	4,HGROUND
	INFO24	3,HGROUND
	INFO24	2,HGROUND
	INFO24	3,HGROUND
	INFO24	1,DRAWBRIDGE
	INFO24	1,DRAWBRIDGE
	INFO24	1,DRAWBRIDGE
	INFO24	1,MILL1
	INFO24	1,GATEHOUSE
	INFO24	1,BEDROOM1
	INFO24	1,KITCHEN
	INFO24	1,HALL1
	INFO24	1,YARD
	INFO24	1,YARD
	INFO24	1,PATH
	INFO24	2,PATH
	INFO24	1,STABLE
	INFO24	1,LOUNGE
	INFO24	3,GALLERY
	INFO24	3,ROOM1
	INFO24	3,ROOM1
	INFO24	1,PASSAGE1
	INFO24	1,PASSAGE1
	INFO24	1,CORRIDOR1
	INFO24	1,CORRIDOR1
	INFO24	1,CORRIDOR1
	INFO24	1,CORRIDOR2
	INFO24	1,CORRIDOR2
	INFO24	1,CORRIDOR2
	INFO24	1,CORRIDOR3
	INFO24	1,CORRIDOR3
	INFO24	1,CORRIDOR3
	INFO24	1,CORRIDOR4
	INFO24	1,CORRIDOR4
	INFO24	6,BATHROOM
	INFO24	5,BATHROOM
	INFO24	1,BEDROOM3
	INFO24	3,ROOM3
	INFO24	3,ACHAM
	INFO24	2,ACHAM
	INFO24	3,ACHAM
	INFO24	1,GDN1
	INFO24	3,TEMPLE
	INFO24	2,STAIR2
	INFO24	4,STAIR3
	INFO24	3,SWAY1
	INFO24	3,SWAY1
	INFO24	2,ORGANROOM
	INFO24	1,CRYPT
	INFO24	1,CRYPT
	INFO24	1,CRYPT
	INFO24	1,CRYPT
	INFO24	1,CRYPT
	INFO24	1,SHRINE
	INFO24	1,SHRINE
	INFO24	6,SHRINE
	INFO24	5,WELL3
	INFO24	2,BANK
	INFO24	6,CAVE2
	INFO24	4,FACE
	INFO24	1,ENTR
	INFO24	3,PSG2
	INFO24	3,PSG2
	INFO24	3,PSG2
	INFO24	3,GDN2
	INFO24	1,GDN2
	INFO24	1,GDN2
	INFO24	1,GDN2
	INFO24	2,HSE
	INFO24	1,HSE
	INFO24	1,OFFIS
	INFO24	1,OFFIS
	INFO24	1,PSG7
	INFO24	1,PSG7
	INFO24	1,CAGE
	INFO24	1,CAGE
	INFO24	1,SHOP1
	INFO24	1,SHOP2
	INFO24	5,SHOP2
	INFO24	6,SHOP2
	INFO24	1,CEMTR
	INFO24	3,RRM
	INFO24	3,RRM
	INFO24	5,RVR
	INFO24	3,TPWFL
	INFO24	3,TPWFL
	INFO24	6,TPWFL
	INFO24	3,RMUD
	INFO24	3,RMUD
	INFO24	3,RMUD
	INFO24	3,RMUD
	INFO24	5,FNRM
	INFO24	3,FNRM
	INFO24	3,STPS
	INFO24	3,STPS
	INFO24	3,STPS
	INFO24	6,BHSE
	INFO24	3,BHSE
	INFO24	1,DRM1
	INFO24	1,DRM2
	INFO24	1,DRM3
	INFO24	1,DRM4
	INFO24	1,DRM5
	INFO24	4,BEAR
	INFO24	4,BEAR
	INFO24	1,SPIDER
	INFO24	1,SPIDER
	INFO24	5,SPIDER
	INFO24	5,SNAKE
	INFO24	1,SNAKE
	INFO24	3,RIVER
	INFO24	3,MOAT
	INFO24	2,HILL
	INFO24	3,HILL
	INFO24	2,TREES
	INFO24	1,LAMP
	INFO24	1,JAR
	INFO24	1,CHALICE
	INFO24	3,CHALICE
	INFO24	1,BUCKET
	INFO24	2,COAL
	INFO24	1,FOSSIL
	INFO24	1,PAINTING2
	INFO24	1,NOTE
	INFO24	1,NOTE
	INFO24	1,BOTTLE4
	INFO24	1,PAPER
	INFO24	1,BED2
	INFO24	1,POT
	INFO24	1,DRESS
	INFO24	1,DRESS
	INFO24	1,BOOKSHELF
	INFO24	1,RING
	INFO24	1,STOOL
	INFO24	1,STOOL
	INFO24	1,BOARD
	INFO24	1,BOARD
	INFO24	1,ALTAR
	INFO24	1,GRID
	INFO24	1,GRID
	INFO24	1,STATUETTE
	INFO24	1,BOOTS
	INFO24	1,BOOTS
	INFO24	1,DOOR7
	INFO24	1,BERRIES
	INFO24	1,DIE5
	INFO24	1,SUIT
	INFO24	5,ABSTRACT
	INFO24	1,BED2UP
	INFO24	2,HOLE
	INFO24	5,ZOOPAPERS
	INFO24	5,ZOOPAPERS
	INFO24	5,ZOOPAPERS
	INFO24	6,ZOOPAPERS
	INFO24	5,BOARD2
	INFO24	6,LUNCHSIGN
	INFO24	1,KBANK
	INFO24	2,MANAGER
	INFO24	1,RUBBAG
	INFO24	3,STREAM
	DC.W	0		;terminate list (for printing)

	END
* produced on Fri Nov 10 15:15:36 2017

* checksum = 9762edc7
