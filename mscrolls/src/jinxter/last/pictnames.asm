******************************************************
*
*	%W%
*	%G%
*
******************************************************

******************************************************
*
*      Pictnames.asm
*
*      Included by Datafile for the names of the piccies.
*
******************************************************


		IFEQ	TITLEPIC
MAXNAMES:       EQU     37
		ENDC

		IFNE	TITLEPIC
MAXNAMES	EQU	38
		ENDC


Okok	SET	FALSE


    IFNE    PROD!PROD_TEST
Okok	SET	TRUE
		ENDC

	IFNE	AMIGA512!Okok

NAMETAB:

	DC.W	BRIDGE-NAMETAB
	DC.W	FRGARDEN-NAMETAB
	DC.W	XAMHOUSE-NAMETAB
	DC.W	CAROUSEL-NAMETAB
	DC.W	NMAGPIE-NAMETAB
	DC.W	CHEESE-NAMETAB
	DC.W	BULL-NAMETAB
	DC.W	CONSERVE-NAMETAB
	DC.W	STATION-NAMETAB
	DC.W	NIAGRO-NAMETAB
	DC.W	SCHLOSS5-NAMETAB
	DC.W	BLUELAKE-NAMETAB
	DC.W	DININGRM-NAMETAB
	DC.W	DUNGEON-NAMETAB
	DC.W	WRECK-NAMETAB
	DC.W	CLOCK-NAMETAB
	DC.W	ARCHWAY-NAMETAB
	DC.W	LIBRARY1-NAMETAB
	DC.W	ONBUS-NAMETAB
	DC.W	WITCHCH-NAMETAB
	DC.W	PAGODA-NAMETAB
	DC.W	BAKERY-NAMETAB
	DC.W	BANQUET-NAMETAB
	DC.W	LIBRARY4-NAMETAB
	DC.W	CLOCKSHOP-NAMETAB
	DC.W	ORCHARD-NAMETAB
	DC.W	JEWELRM-NAMETAB
	DC.W	BEER-NAMETAB
	DC.W	GUARDEN-NAMETAB
    DC.W    ONBUS2-NAMETAB
    DC.W    LAGOON-NAMETAB
    DC.W    CWOOD-NAMETAB
    DC.W    MKITCHEN-NAMETAB
    DC.W    CARCH-NAMETAB
    DC.W    UNDERWATER-NAMETAB
    DC.W    NLANE-NAMETAB
    DC.W    SACRISTY-NAMETAB
    DC.W    VGREEN-NAMETAB
    DC.W    WLODGE1-NAMETAB
    DC.W    WLODGE2-NAMETAB
    DC.W    WLODGE3-NAMETAB
    DC.W    WLODGE4-NAMETAB
    DC.W    WLODGE5-NAMETAB

	IFNE	TITLEPIC
	DC.W	TITLE-NAMETAB
	ENDC
    
LASTENTRY:

	IFEQ	AMIGA512

BRIDGE		DC.B	'BRIDGE.FIN',0
FRGARDEN	DC.B	'FRGARDEN.FIN',0
XAMHOUSE	DC.B	'XAMHOUSE.FIN',0
CAROUSEL	DC.B	'CAROUSEL.FIN',0
NMAGPIE		DC.B	'NMAGPIE.FIN',0
CHEESE		DC.B	'CHEESE.FIN',0
BULL		DC.B	'BULL.FIN',0
CONSERVE	DC.B	'CONSERVE.FIN',0
STATION		DC.B	'STATION.FIN',0
NIAGRO		DC.B	'NIAGRO.FIN',0
SCHLOSS5	DC.B	'SCHLOSS5.FIN',0
BLUELAKE	DC.B	'BLUELAKE.FIN',0
DININGRM	DC.B	'DININGRM.FIN',0
DUNGEON		DC.B	'DUNGEON.FIN',0
WRECK		DC.B	'WRECK.FIN',0
CLOCK		DC.B	'CLOCK.FIN',0
ARCHWAY		DC.B	'ARCHWAY.FIN',0
LIBRARY1	DC.B	'LIBRARY1.FIN',0	;Your library
ONBUS		DC.B	'ONBUS.FIN',0
WITCHCH		DC.B	'WITCHCH.FIN',0
PAGODA		DC.B	'PAGODA.FIN',0
BAKERY		DC.B	'BAKERY.FIN',0
BANQUET		DC.B	'BANQUET.FIN',0
LIBRARY4	DC.B	'LIBRARY4.FIN',0
CLOCKSHOP	DC.B	'CLOCKS.FIN',0
ORCHARD		DC.B	'ORCHARD.FIN',0
JEWELRM		DC.B	'JEWEL.FIN',0
BEER		DC.B	'BEER.FIN',0
GUARDEN		DC.B	'GUARDEN.FIN',0
ONBUS2		DC.B	'ONBUS2.SVG',0
LAGOON		DC.B	'LAGOON1.SVG',0
CWOOD		DC.B	'CWOOD1.SVG',0
MKITCHEN	DC.B	'MKITCHEN1.SVG',0
CARCH   	DC.B	'ARCH1.SVG',0
UNDERWATER  DC.B    'UNDERWATER1.SVG',0
NLANE       DC.B    'NLANE1.SVG',0
SACRISTY    DC.B    'SAC1.SVG',0
VGREEN      DC.B    'VGREEN1.SVG',0 
WLODGE1  	DC.B	'WLODGE1.SVG',0 ; no water
WLODGE2  	DC.B	'WLODGE2.PNG',0 ; animated low water
WLODGE3  	DC.B	'WLODGE3.PNG',0 ; animated mid water
WLODGE4  	DC.B	'WLODGE4.PNG',0 ; animated high water
WLODGE5  	DC.B	'WLODGE5.JPG',0 ; room full
    
	ENDC

	IFNE	AMIGA512

BRIDGE		DC.B	'J3',0
FRGARDEN	DC.B	'J4',0
XAMHOUSE	DC.B	'J5',0
CAROUSEL	DC.B	'J6',0
NMAGPIE		DC.B	'J7',0
CHEESE		DC.B	'J8',0
BULL		DC.B	'J9',0
CONSERVE	DC.B	'J10',0
STATION		DC.B	'J11',0
NIAGRO		DC.B	'J12',0
SCHLOSS5	DC.B	'J13',0
BLUELAKE	DC.B	'J14',0
DININGRM	DC.B	'J15',0
DUNGEON		DC.B	'J16',0
WRECK		DC.B	'J17',0
CLOCK		DC.B	'J18',0
ARCHWAY		DC.B	'J19',0
LIBRARY1	DC.B	'J20',0	;Your library
ONBUS		DC.B	'J21',0
WITCHCH		DC.B	'J22',0
PAGODA		DC.B	'J23',0
BAKERY		DC.B	'J24',0
BANQUET		DC.B	'J25',0
LIBRARY4	DC.B	'J26',0
CLOCKSHOP	DC.B	'J27',0
ORCHARD		DC.B	'J28',0
JEWELRM		DC.B	'J29',0
BEER		DC.B	'J30',0
GUARDEN		DC.B	'J73',0

		ENDC

		IFNE	TITLEPIC
TITLE:		DC.B	'TITLE.FIN',0
		ENDC

		ENDC
