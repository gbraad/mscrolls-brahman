***************************************************************************
*
*
*	fred23jr output from file '../guild'*
*
***************************************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.ADJECTIVES1
*
*--------------------------------
  
	include "equates.i"
	include	"macros.i"
	include	"adjequ.i"

      XDEF        ADJ.TBL

ADJ      MACRO    * adjname,adjname....

         IFC      '','\1'
         DC.B     0
         MEXIT
         ENDC
         DC.B     ADJ_\1
         ADJ      \2,\3,\4,\5,\6

         ENDM




ADJ.TBL




* boat
	XDEF	ADJ.BOAT
ADJ.BOAT
	ADJ	
* mooring
	XDEF	ADJ.MOORING
ADJ.MOORING
	ADJ	mooring,three,four,six
* jetty
	XDEF	ADJ.JETTY
ADJ.JETTY
	ADJ	
* scrub1
	XDEF	ADJ.SCRUB1
ADJ.SCRUB1
	ADJ	
* scrub2
	XDEF	ADJ.SCRUB2
ADJ.SCRUB2
	ADJ	
* scrub3
	XDEF	ADJ.SCRUB3
ADJ.SCRUB3
	ADJ	
* forest1
	XDEF	ADJ.FOREST1
ADJ.FOREST1
	ADJ	
* forest2
	XDEF	ADJ.FOREST2
ADJ.FOREST2
	ADJ	forest
* forest3
	XDEF	ADJ.FOREST3
ADJ.FOREST3
	ADJ	forest
* forest4
	XDEF	ADJ.FOREST4
ADJ.FOREST4
	ADJ	forest
* field
	XDEF	ADJ.FIELD
ADJ.FIELD
	ADJ	
* hground
	XDEF	ADJ.HGROUND
ADJ.HGROUND
	ADJ	
* drawbridge
	XDEF	ADJ.DRAWBRIDGE
ADJ.DRAWBRIDGE
	ADJ	
* mill1
	XDEF	ADJ.MILL1
ADJ.MILL1
	ADJ	
* gatehouse
	XDEF	ADJ.GATEHOUSE
ADJ.GATEHOUSE
	ADJ	
* bedroom1
	XDEF	ADJ.BEDROOM1
ADJ.BEDROOM1
	ADJ	
* quarters
	XDEF	ADJ.QUARTERS
ADJ.QUARTERS
	ADJ	
* kitchen
	XDEF	ADJ.KITCHEN
ADJ.KITCHEN
	ADJ	castle
* hall1
	XDEF	ADJ.HALL1
ADJ.HALL1
	ADJ	dining
* hall2
	XDEF	ADJ.HALL2
ADJ.HALL2
	ADJ	entrance
* yard
	XDEF	ADJ.YARD
ADJ.YARD
	ADJ	
* path
	XDEF	ADJ.PATH
ADJ.PATH
	ADJ	
* stable
	XDEF	ADJ.STABLE
ADJ.STABLE
	ADJ	
* lounge
	XDEF	ADJ.LOUNGE
ADJ.LOUNGE
	ADJ	
* gallery
	XDEF	ADJ.GALLERY
ADJ.GALLERY
	ADJ	
* room1
	XDEF	ADJ.ROOM1
ADJ.ROOM1
	ADJ	drawing
* passage1
	XDEF	ADJ.PASSAGE1
ADJ.PASSAGE1
	ADJ	gloomy
* sway5
	XDEF	ADJ.SWAY5
ADJ.SWAY5
	ADJ	
* cellar
	XDEF	ADJ.CELLAR
ADJ.CELLAR
	ADJ	wine
* room2
	XDEF	ADJ.ROOM2
ADJ.ROOM2
	ADJ	junk
* corridor1
	XDEF	ADJ.CORRIDOR1
ADJ.CORRIDOR1
	ADJ	wooden,tiled
* corridor2
	XDEF	ADJ.CORRIDOR2
ADJ.CORRIDOR2
	ADJ	wooden,tiled
* corridor3
	XDEF	ADJ.CORRIDOR3
ADJ.CORRIDOR3
	ADJ	wooden,tiled
* corridor4
	XDEF	ADJ.CORRIDOR4
ADJ.CORRIDOR4
	ADJ	wooden,tiled
* library
	XDEF	ADJ.LIBRARY
ADJ.LIBRARY
	ADJ	castle
* bathroom
	XDEF	ADJ.BATHROOM
ADJ.BATHROOM
	ADJ	
* bedroom2
	XDEF	ADJ.BEDROOM2
ADJ.BEDROOM2
	ADJ	main
* bedroom3
	XDEF	ADJ.BEDROOM3
ADJ.BEDROOM3
	ADJ	spare
* lab
	XDEF	ADJ.LAB
ADJ.LAB
	ADJ	secret
* room3
	XDEF	ADJ.ROOM3
ADJ.ROOM3
	ADJ	billiard
* room4
	XDEF	ADJ.ROOM4
ADJ.ROOM4
	ADJ	music
* acham
	XDEF	ADJ.ACHAM
ADJ.ACHAM
	ADJ	
* gdn1
	XDEF	ADJ.GDN1
ADJ.GDN1
	ADJ	temple
* temple
	XDEF	ADJ.TEMPLE
ADJ.TEMPLE
	ADJ	
* stair1
	XDEF	ADJ.STAIR1
ADJ.STAIR1
	ADJ	
* stair2
	XDEF	ADJ.STAIR2
ADJ.STAIR2
	ADJ	
* stair3
	XDEF	ADJ.STAIR3
ADJ.STAIR3
	ADJ	
* stair4
	XDEF	ADJ.STAIR4
ADJ.STAIR4
	ADJ	
* sway1
	XDEF	ADJ.SWAY1
ADJ.SWAY1
	ADJ	
* sway2
	XDEF	ADJ.SWAY2
ADJ.SWAY2
	ADJ	
* sway3
	XDEF	ADJ.SWAY3
ADJ.SWAY3
	ADJ	
* sway4
	XDEF	ADJ.SWAY4
ADJ.SWAY4
	ADJ	
* organroom
	XDEF	ADJ.ORGANROOM
ADJ.ORGANROOM
	ADJ	organ
* crypt
	XDEF	ADJ.CRYPT
ADJ.CRYPT
	ADJ	
* shrine
	XDEF	ADJ.SHRINE
ADJ.SHRINE
	ADJ	
* library2
	XDEF	ADJ.LIBRARY2
ADJ.LIBRARY2
	ADJ	black
* well1
	XDEF	ADJ.WELL1
ADJ.WELL1
	ADJ	
* well2
	XDEF	ADJ.WELL2
ADJ.WELL2
	ADJ	
* well3
	XDEF	ADJ.WELL3
ADJ.WELL3
	ADJ	
* tunnel
	XDEF	ADJ.TUNNEL
ADJ.TUNNEL
	ADJ	
* sump
	XDEF	ADJ.SUMP
ADJ.SUMP
	ADJ	
* bank
	XDEF	ADJ.BANK
ADJ.BANK
	ADJ	sandy
* shaft1
	XDEF	ADJ.SHAFT1
ADJ.SHAFT1
	ADJ	thin
* crab1
	XDEF	ADJ.CRAB1
ADJ.CRAB1
	ADJ	
* cave2
	XDEF	ADJ.CAVE2
ADJ.CAVE2
	ADJ	claustrophobic,small
* face
	XDEF	ADJ.FACE
ADJ.FACE
	ADJ	rock
* cave1
	XDEF	ADJ.CAVE1
ADJ.CAVE1
	ADJ	
* cave3
	XDEF	ADJ.CAVE3
ADJ.CAVE3
	ADJ	craggy
* entr
	XDEF	ADJ.ENTR
ADJ.ENTR
	ADJ	cave
* passage3
	XDEF	ADJ.PASSAGE3
ADJ.PASSAGE3
	ADJ	
* room9
	XDEF	ADJ.ROOM9
ADJ.ROOM9
	ADJ	junction
* psg2
	XDEF	ADJ.PSG2
ADJ.PSG2
	ADJ	hot
* psg4
	XDEF	ADJ.PSG4
ADJ.PSG4
	ADJ	
* tjunc1
	XDEF	ADJ.TJUNC1
ADJ.TJUNC1
	ADJ	
* tjunc2
	XDEF	ADJ.TJUNC2
ADJ.TJUNC2
	ADJ	
* tjunc3
	XDEF	ADJ.TJUNC3
ADJ.TJUNC3
	ADJ	
* avre
	XDEF	ADJ.AVRE
ADJ.AVRE
	ADJ	
* gdn2
	XDEF	ADJ.GDN2
ADJ.GDN2
	ADJ	hot
* hse
	XDEF	ADJ.HSE
ADJ.HSE
	ADJ	insect,reptile,jungle
* offis
	XDEF	ADJ.OFFIS
ADJ.OFFIS
	ADJ	zoo
* psg7
	XDEF	ADJ.PSG7
ADJ.PSG7
	ADJ	white
* cage
	XDEF	ADJ.CAGE
ADJ.CAGE
	ADJ	
* frk
	XDEF	ADJ.FRK
ADJ.FRK
	ADJ	
* shop1
	XDEF	ADJ.SHOP1
ADJ.SHOP1
	ADJ	
* shop2
	XDEF	ADJ.SHOP2
ADJ.SHOP2
	ADJ	shop
* shop3
	XDEF	ADJ.SHOP3
ADJ.SHOP3
	ADJ	
* cemtr
	XDEF	ADJ.CEMTR
ADJ.CEMTR
	ADJ	
* rrm
	XDEF	ADJ.RRM
ADJ.RRM
	ADJ	circular,echo
* rvr
	XDEF	ADJ.RVR
ADJ.RVR
	ADJ	
* tpwfl
	XDEF	ADJ.TPWFL
ADJ.TPWFL
	ADJ	
* rmud
	XDEF	ADJ.RMUD
ADJ.RMUD
	ADJ	muddy
* fnrm
	XDEF	ADJ.FNRM
ADJ.FNRM
	ADJ	
* stps
	XDEF	ADJ.STPS
ADJ.STPS
	ADJ	
* bhse
	XDEF	ADJ.BHSE
ADJ.BHSE
	ADJ	
* drm1
	XDEF	ADJ.DRM1
ADJ.DRM1
	ADJ	blue
* drm2
	XDEF	ADJ.DRM2
ADJ.DRM2
	ADJ	red
* drm3
	XDEF	ADJ.DRM3
ADJ.DRM3
	ADJ	white
* drm4
	XDEF	ADJ.DRM4
ADJ.DRM4
	ADJ	green
* drm5
	XDEF	ADJ.DRM5
ADJ.DRM5
	ADJ	yellow
* thief
	XDEF	ADJ.THIEF
ADJ.THIEF
	ADJ	master
* miller
	XDEF	ADJ.MILLER
ADJ.MILLER
	ADJ	
* gatekeeper
	XDEF	ADJ.GATEKEEPER
ADJ.GATEKEEPER
	ADJ	castle,gate
* bear
	XDEF	ADJ.BEAR
ADJ.BEAR
	ADJ	grizzly
* guest
	XDEF	ADJ.GUEST
ADJ.GUEST
	ADJ	old,frail
* spider
	XDEF	ADJ.SPIDER
ADJ.SPIDER
	ADJ	jumper,king
* gravedigger
	XDEF	ADJ.GRAVEDIGGER
ADJ.GRAVEDIGGER
	ADJ	
* snake
	XDEF	ADJ.SNAKE
ADJ.SNAKE
	ADJ	ice,constricting
* boat2
	XDEF	ADJ.BOAT2
ADJ.BOAT2
	ADJ	small
* jetty2
	XDEF	ADJ.JETTY2
ADJ.JETTY2
	ADJ	rickety,old
* river
	XDEF	ADJ.RIVER
ADJ.RIVER
	ADJ	
* water
	XDEF	ADJ.WATER
ADJ.WATER
	ADJ	river
* wheatfield
	XDEF	ADJ.WHEATFIELD
ADJ.WHEATFIELD
	ADJ	golden
* moat
	XDEF	ADJ.MOAT
ADJ.MOAT
	ADJ	castle
* castle
	XDEF	ADJ.CASTLE
ADJ.CASTLE
	ADJ	homely
* copse
	XDEF	ADJ.COPSE
ADJ.COPSE
	ADJ	
* wood
	XDEF	ADJ.WOOD
ADJ.WOOD
	ADJ	
* hill
	XDEF	ADJ.HILL
ADJ.HILL
	ADJ	
* trees
	XDEF	ADJ.TREES
ADJ.TREES
	ADJ	healthy,strong
* fishingsign
	XDEF	ADJ.FISHINGSIGN
ADJ.FISHINGSIGN
	ADJ	
* counter
	XDEF	ADJ.COUNTER
ADJ.COUNTER
	ADJ	polished,highly,beautifully
* lake
	XDEF	ADJ.LAKE
ADJ.LAKE
	ADJ	
* wall1
	XDEF	ADJ.WALL1
ADJ.WALL1
	ADJ	
* sky
	XDEF	ADJ.SKY
ADJ.SKY
	ADJ	
* bag1
	XDEF	ADJ.BAG1
ADJ.BAG1
	ADJ	swag,large,black
* sweatshirt
	XDEF	ADJ.SWEATSHIRT
ADJ.SWEATSHIRT
	ADJ	striped
* jeans
	XDEF	ADJ.JEANS
ADJ.JEANS
	ADJ	black
* lamp
	XDEF	ADJ.LAMP
ADJ.LAMP
	ADJ	
* pocket1
	XDEF	ADJ.POCKET1
ADJ.POCKET1
	ADJ	jeans
* mill2
	XDEF	ADJ.MILL2
ADJ.MILL2
	ADJ	wind
* vanes
	XDEF	ADJ.VANES
ADJ.VANES
	ADJ	
* chair1
	XDEF	ADJ.CHAIR1
ADJ.CHAIR1
	ADJ	easy
* chaff
	XDEF	ADJ.CHAFF
ADJ.CHAFF
	ADJ	
* lute
	XDEF	ADJ.LUTE
ADJ.LUTE
	ADJ	
* plectrum
	XDEF	ADJ.PLECTRUM
ADJ.PLECTRUM
	ADJ	silver,solid
* gum
	XDEF	ADJ.GUM
ADJ.GUM
	ADJ	chewing
* lever
	XDEF	ADJ.LEVER
ADJ.LEVER
	ADJ	
* bed1
	XDEF	ADJ.BED1
ADJ.BED1
	ADJ	small
* tub
	XDEF	ADJ.TUB
ADJ.TUB
	ADJ	small,plastic
* maggot
	XDEF	ADJ.MAGGOT
ADJ.MAGGOT
	ADJ	lonely
* cabinet1
	XDEF	ADJ.CABINET1
ADJ.CABINET1
	ADJ	antique
* key1
	XDEF	ADJ.KEY1
ADJ.KEY1
	ADJ	golden
* sink
	XDEF	ADJ.SINK
ADJ.SINK
	ADJ	kitchen
* cupboard
	XDEF	ADJ.CUPBOARD
ADJ.CUPBOARD
	ADJ	
* worktop
	XDEF	ADJ.WORKTOP
ADJ.WORKTOP
	ADJ	
* poison
	XDEF	ADJ.POISON
ADJ.POISON
	ADJ	rat
* jar
	XDEF	ADJ.JAR
ADJ.JAR
	ADJ	jam
* jam2
	XDEF	ADJ.JAM2
ADJ.JAM2
	ADJ	fly_infested
* jam
	XDEF	ADJ.JAM
ADJ.JAM
	ADJ	
* cage3
	XDEF	ADJ.CAGE3
ADJ.CAGE3
	ADJ	cubic,golden
* table1
	XDEF	ADJ.TABLE1
ADJ.TABLE1
	ADJ	long
* chalice
	XDEF	ADJ.CHALICE
ADJ.CHALICE
	ADJ	platinum
* rug
	XDEF	ADJ.RUG
ADJ.RUG
	ADJ	vast
* cage4
	XDEF	ADJ.CAGE4
ADJ.CAGE4
	ADJ	bird
* cheque
	XDEF	ADJ.CHEQUE
ADJ.CHEQUE
	ADJ	
* rat1
	XDEF	ADJ.RAT1
ADJ.RAT1
	ADJ	black
* rat2
	XDEF	ADJ.RAT2
ADJ.RAT2
	ADJ	white
* rat3
	XDEF	ADJ.RAT3
ADJ.RAT3
	ADJ	grey
* rat4
	XDEF	ADJ.RAT4
ADJ.RAT4
	ADJ	brown
* bone
	XDEF	ADJ.BONE
ADJ.BONE
	ADJ	snake
* straw
	XDEF	ADJ.STRAW
ADJ.STRAW
	ADJ	
* flies
	XDEF	ADJ.FLIES
ADJ.FLIES
	ADJ	live
* deadflies
	XDEF	ADJ.DEADFLIES
ADJ.DEADFLIES
	ADJ	dead
* carpet
	XDEF	ADJ.CARPET
ADJ.CARPET
	ADJ	shagpile
* fire1
	XDEF	ADJ.FIRE1
ADJ.FIRE1
	ADJ	coal
* grate
	XDEF	ADJ.GRATE
ADJ.GRATE
	ADJ	
* bucket
	XDEF	ADJ.BUCKET
ADJ.BUCKET
	ADJ	black,iron
* coal
	XDEF	ADJ.COAL
ADJ.COAL
	ADJ	black
* fossil
	XDEF	ADJ.FOSSIL
ADJ.FOSSIL
	ADJ	
* painting1
	XDEF	ADJ.PAINTING1
ADJ.PAINTING1
	ADJ	oil
* painting2
	XDEF	ADJ.PAINTING2
ADJ.PAINTING2
	ADJ	
* painting3
	XDEF	ADJ.PAINTING3
ADJ.PAINTING3
	ADJ	gaudy
* safe
	XDEF	ADJ.SAFE
ADJ.SAFE
	ADJ	
* couch
	XDEF	ADJ.COUCH
ADJ.COUCH
	ADJ	antique,red,leather
* cushion
	XDEF	ADJ.CUSHION
ADJ.CUSHION
	ADJ	
* foam
	XDEF	ADJ.FOAM
ADJ.FOAM
	ADJ	
* note
	XDEF	ADJ.NOTE
ADJ.NOTE
	ADJ	five_ferg,five,ferg
* door1
	XDEF	ADJ.DOOR1
ADJ.DOOR1
	ADJ	wooden
* rats
	XDEF	ADJ.RATS
ADJ.RATS
	ADJ	
* steps
	XDEF	ADJ.STEPS
ADJ.STEPS
	ADJ	
* racks
	XDEF	ADJ.RACKS
ADJ.RACKS
	ADJ	tall,wooden
* ratwater
	XDEF	ADJ.RATWATER
ADJ.RATWATER
	ADJ	
* system
	XDEF	ADJ.SYSTEM
ADJ.SYSTEM
	ADJ	drainage
* pipe
	XDEF	ADJ.PIPE
ADJ.PIPE
	ADJ	lead
* stopcock
	XDEF	ADJ.STOPCOCK
ADJ.STOPCOCK
	ADJ	
* wine1
	XDEF	ADJ.WINE1
ADJ.WINE1
	ADJ	red
* champagne
	XDEF	ADJ.CHAMPAGNE
ADJ.CHAMPAGNE
	ADJ	
* bottle1
	XDEF	ADJ.BOTTLE1
ADJ.BOTTLE1
	ADJ	wine,red
* bottle4
	XDEF	ADJ.BOTTLE4
ADJ.BOTTLE4
	ADJ	champagne
* ruby
	XDEF	ADJ.RUBY
ADJ.RUBY
	ADJ	superb,beautiful,red
* junk
	XDEF	ADJ.JUNK
ADJ.JUNK
	ADJ	old,broken
* book1
	XDEF	ADJ.BOOK1
ADJ.BOOK1
	ADJ	
* desk1
	XDEF	ADJ.DESK1
ADJ.DESK1
	ADJ	simple,reading
* book3
	XDEF	ADJ.BOOK3
ADJ.BOOK3
	ADJ	
* device
	XDEF	ADJ.DEVICE
ADJ.DEVICE
	ADJ	yellow,pale
* paper
	XDEF	ADJ.PAPER
ADJ.PAPER
	ADJ	toilet,yellow,pale
* toilet
	XDEF	ADJ.TOILET
ADJ.TOILET
	ADJ	yellow,pale
* bed2
	XDEF	ADJ.BED2
ADJ.BED2
	ADJ	king_size,king,size,sized
* pot
	XDEF	ADJ.POT
ADJ.POT
	ADJ	china,chamber
* cabinet2
	XDEF	ADJ.CABINET2
ADJ.CABINET2
	ADJ	walnut
* brooch
	XDEF	ADJ.BROOCH
ADJ.BROOCH
	ADJ	platinum
* blackboard
	XDEF	ADJ.BLACKBOARD
ADJ.BLACKBOARD
	ADJ	black
* bed3
	XDEF	ADJ.BED3
ADJ.BED3
	ADJ	
* wardrobe
	XDEF	ADJ.WARDROBE
ADJ.WARDROBE
	ADJ	large
* dress
	XDEF	ADJ.DRESS
ADJ.DRESS
	ADJ	designer,black,chiffon
* box1
	XDEF	ADJ.BOX1
ADJ.BOX1
	ADJ	sewing
* needle
	XDEF	ADJ.NEEDLE
ADJ.NEEDLE
	ADJ	sharp
* cotton
	XDEF	ADJ.COTTON
ADJ.COTTON
	ADJ	
* cauldron
	XDEF	ADJ.CAULDRON
ADJ.CAULDRON
	ADJ	cast,iron
* bookshelf
	XDEF	ADJ.BOOKSHELF
ADJ.BOOKSHELF
	ADJ	book
* book2
	XDEF	ADJ.BOOK2
ADJ.BOOK2
	ADJ	old,leather
* sachet
	XDEF	ADJ.SACHET
ADJ.SACHET
	ADJ	paper
* powder
	XDEF	ADJ.POWDER
ADJ.POWDER
	ADJ	magic,electric,blue
* cue
	XDEF	ADJ.CUE
ADJ.CUE
	ADJ	billiard
* table2
	XDEF	ADJ.TABLE2
ADJ.TABLE2
	ADJ	billiard
* ball1
	XDEF	ADJ.BALL1
ADJ.BALL1
	ADJ	white,billiard,cue
* ball2
	XDEF	ADJ.BALL2
ADJ.BALL2
	ADJ	red,billiard
* ball3
	XDEF	ADJ.BALL3
ADJ.BALL3
	ADJ	spot,billiard
* rack1
	XDEF	ADJ.RACK1
ADJ.RACK1
	ADJ	cue
* ring
	XDEF	ADJ.RING
ADJ.RING
	ADJ	diamond
* piano
	XDEF	ADJ.PIANO
ADJ.PIANO
	ADJ	polished,highly
* stool
	XDEF	ADJ.STOOL
ADJ.STOOL
	ADJ	piano
* sheet
	XDEF	ADJ.SHEET
ADJ.SHEET
	ADJ	
* bag2
	XDEF	ADJ.BAG2
ADJ.BAG2
	ADJ	plastic
* board
	XDEF	ADJ.BOARD
ADJ.BOARD
	ADJ	notice
* altar
	XDEF	ADJ.ALTAR
ADJ.ALTAR
	ADJ	
* statue
	XDEF	ADJ.STATUE
ADJ.STATUE
	ADJ	
* burner
	XDEF	ADJ.BURNER
ADJ.BURNER
	ADJ	incense
* hive
	XDEF	ADJ.HIVE
ADJ.HIVE
	ADJ	
* bees
	XDEF	ADJ.BEES
ADJ.BEES
	ADJ	
* gloves
	XDEF	ADJ.GLOVES
ADJ.GLOVES
	ADJ	
* honey
	XDEF	ADJ.HONEY
ADJ.HONEY
	ADJ	
* door2
	XDEF	ADJ.DOOR2
ADJ.DOOR2
	ADJ	black
* grid
	XDEF	ADJ.GRID
ADJ.GRID
	ADJ	
* bluesquarep
	XDEF	ADJ.BLUESQUAREP
ADJ.BLUESQUAREP
	ADJ	blue
* orangesquarep
	XDEF	ADJ.ORANGESQUAREP
ADJ.ORANGESQUAREP
	ADJ	orange
* yellowsquarep
	XDEF	ADJ.YELLOWSQUAREP
ADJ.YELLOWSQUAREP
	ADJ	yellow
* violetsquarep
	XDEF	ADJ.VIOLETSQUAREP
ADJ.VIOLETSQUAREP
	ADJ	violet
* redsquarep
	XDEF	ADJ.REDSQUAREP
ADJ.REDSQUAREP
	ADJ	red
* greensquarep
	XDEF	ADJ.GREENSQUAREP
ADJ.GREENSQUAREP
	ADJ	green
* indigosquarep
	XDEF	ADJ.INDIGOSQUAREP
ADJ.INDIGOSQUAREP
	ADJ	indigo
* blacksquarep
	XDEF	ADJ.BLACKSQUAREP
ADJ.BLACKSQUAREP
	ADJ	black
* whitesquarep
	XDEF	ADJ.WHITESQUAREP
ADJ.WHITESQUAREP
	ADJ	white
* blacksquare
	XDEF	ADJ.BLACKSQUARE
ADJ.BLACKSQUARE
	ADJ	black
* whitesquare
	XDEF	ADJ.WHITESQUARE
ADJ.WHITESQUARE
	ADJ	white
* redsquare1
	XDEF	ADJ.REDSQUARE1
ADJ.REDSQUARE1
	ADJ	red
* redsquare2
	XDEF	ADJ.REDSQUARE2
ADJ.REDSQUARE2
	ADJ	red
* orangesquare1
	XDEF	ADJ.ORANGESQUARE1
ADJ.ORANGESQUARE1
	ADJ	orange
* orangesquare2
	XDEF	ADJ.ORANGESQUARE2
ADJ.ORANGESQUARE2
	ADJ	orange
* yellowsquare1
	XDEF	ADJ.YELLOWSQUARE1
ADJ.YELLOWSQUARE1
	ADJ	yellow
* yellowsquare2
	XDEF	ADJ.YELLOWSQUARE2
ADJ.YELLOWSQUARE2
	ADJ	yellow
* greensquare1
	XDEF	ADJ.GREENSQUARE1
ADJ.GREENSQUARE1
	ADJ	green
* bluesquare1
	XDEF	ADJ.BLUESQUARE1
ADJ.BLUESQUARE1
	ADJ	blue
* bluesquare3
	XDEF	ADJ.BLUESQUARE3
ADJ.BLUESQUARE3
	ADJ	blue
* indigosquare1
	XDEF	ADJ.INDIGOSQUARE1
ADJ.INDIGOSQUARE1
	ADJ	indigo
* indigosquare2
	XDEF	ADJ.INDIGOSQUARE2
ADJ.INDIGOSQUARE2
	ADJ	indigo
* indigosquare3
	XDEF	ADJ.INDIGOSQUARE3
ADJ.INDIGOSQUARE3
	ADJ	indigo
* violetsquare1
	XDEF	ADJ.VIOLETSQUARE1
ADJ.VIOLETSQUARE1
	ADJ	violet
* violetsquare2
	XDEF	ADJ.VIOLETSQUARE2
ADJ.VIOLETSQUARE2
	ADJ	violet
* redsquare3
	XDEF	ADJ.REDSQUARE3
ADJ.REDSQUARE3
	ADJ	red
* orangesqure3
	XDEF	ADJ.ORANGESQURE3
ADJ.ORANGESQURE3
	ADJ	orange
* greensquare2
	XDEF	ADJ.GREENSQUARE2
ADJ.GREENSQUARE2
	ADJ	green
* yellowsquare3
	XDEF	ADJ.YELLOWSQUARE3
ADJ.YELLOWSQUARE3
	ADJ	yellow
* squaredeath
	XDEF	ADJ.SQUAREDEATH
ADJ.SQUAREDEATH
	ADJ	
* sarcophagus
	XDEF	ADJ.SARCOPHAGUS
ADJ.SARCOPHAGUS
	ADJ	huge,stone
* skull
	XDEF	ADJ.SKULL
ADJ.SKULL
	ADJ	
* eye
	XDEF	ADJ.EYE
ADJ.EYE
	ADJ	
* statuette
	XDEF	ADJ.STATUETTE
ADJ.STATUETTE
	ADJ	obsidian
* fakemiller
	XDEF	ADJ.FAKEMILLER
ADJ.FAKEMILLER
	ADJ	
* door4
	XDEF	ADJ.DOOR4
ADJ.DOOR4
	ADJ	shrine,shiny,black
* pamphlet
	XDEF	ADJ.PAMPHLET
ADJ.PAMPHLET
	ADJ	
* key2
	XDEF	ADJ.KEY2
ADJ.KEY2
	ADJ	ebony,black
* ivorykey
	XDEF	ADJ.IVORYKEY
ADJ.IVORYKEY
	ADJ	ivory,white
* sand
	XDEF	ADJ.SAND
ADJ.SAND
	ADJ	crunchy
* boots
	XDEF	ADJ.BOOTS
ADJ.BOOTS
	ADJ	wetsuit
* rope
	XDEF	ADJ.ROPE
ADJ.ROPE
	ADJ	wet
* mineral
	XDEF	ADJ.MINERAL
ADJ.MINERAL
	ADJ	semi_precious
* pick
	XDEF	ADJ.PICK
ADJ.PICK
	ADJ	
* bars
	XDEF	ADJ.BARS
ADJ.BARS
	ADJ	iron
* signpost
	XDEF	ADJ.SIGNPOST
ADJ.SIGNPOST
	ADJ	
* gate
	XDEF	ADJ.GATE
ADJ.GATE
	ADJ	toll
* slot1
	XDEF	ADJ.SLOT1
ADJ.SLOT1
	ADJ	
* macaw
	XDEF	ADJ.MACAW
ADJ.MACAW
	ADJ	kerovnian
* bird3
	XDEF	ADJ.BIRD3
ADJ.BIRD3
	ADJ	mynah
* cage5
	XDEF	ADJ.CAGE5
ADJ.CAGE5
	ADJ	large
* tree1
	XDEF	ADJ.TREE1
ADJ.TREE1
	ADJ	palm,coconut
* succulents
	XDEF	ADJ.SUCCULENTS
ADJ.SUCCULENTS
	ADJ	
* coconut
	XDEF	ADJ.COCONUT
ADJ.COCONUT
	ADJ	brown,hairy
* cage6
	XDEF	ADJ.CAGE6
ADJ.CAGE6
	ADJ	small
* cage7
	XDEF	ADJ.CAGE7
ADJ.CAGE7
	ADJ	large
* door5
	XDEF	ADJ.DOOR5
ADJ.DOOR5
	ADJ	white
* desk2
	XDEF	ADJ.DESK2
ADJ.DESK2
	ADJ	old
* drawer1
	XDEF	ADJ.DRAWER1
ADJ.DRAWER1
	ADJ	
* spade
	XDEF	ADJ.SPADE
ADJ.SPADE
	ADJ	small
* foods
	XDEF	ADJ.FOODS
ADJ.FOODS
	ADJ	
* magazine2
	XDEF	ADJ.MAGAZINE2
ADJ.MAGAZINE2
	ADJ	zoological
* door6
	XDEF	ADJ.DOOR6
ADJ.DOOR6
	ADJ	small
* door7
	XDEF	ADJ.DOOR7
ADJ.DOOR7
	ADJ	front
* window
	XDEF	ADJ.WINDOW
ADJ.WINDOW
	ADJ	frosted
* casket
	XDEF	ADJ.CASKET
ADJ.CASKET
	ADJ	
* till
	XDEF	ADJ.TILL
ADJ.TILL
	ADJ	
* button
	XDEF	ADJ.BUTTON
ADJ.BUTTON
	ADJ	till
* piece
	XDEF	ADJ.PIECE
ADJ.PIECE
	ADJ	two_ferg,two,ferg
* grave1
	XDEF	ADJ.GRAVE1
ADJ.GRAVE1
	ADJ	small
* grave2
	XDEF	ADJ.GRAVE2
ADJ.GRAVE2
	ADJ	medium
* grave3
	XDEF	ADJ.GRAVE3
ADJ.GRAVE3
	ADJ	large
* headstone1
	XDEF	ADJ.HEADSTONE1
ADJ.HEADSTONE1
	ADJ	small
* headstone2
	XDEF	ADJ.HEADSTONE2
ADJ.HEADSTONE2
	ADJ	medium
* headstone3
	XDEF	ADJ.HEADSTONE3
ADJ.HEADSTONE3
	ADJ	large
* tree2
	XDEF	ADJ.TREE2
ADJ.TREE2
	ADJ	yew
* berries
	XDEF	ADJ.BERRIES
ADJ.BERRIES
	ADJ	yew
* skeleton
	XDEF	ADJ.SKELETON
ADJ.SKELETON
	ADJ	
* finger
	XDEF	ADJ.FINGER
ADJ.FINGER
	ADJ	finger
* chest
	XDEF	ADJ.CHEST
ADJ.CHEST
	ADJ	wooden
* heart
	XDEF	ADJ.HEART
ADJ.HEART
	ADJ	
* spike
	XDEF	ADJ.SPIKE
ADJ.SPIKE
	ADJ	
* ladder
	XDEF	ADJ.LADDER
ADJ.LADDER
	ADJ	rope
* beam
	XDEF	ADJ.BEAM
ADJ.BEAM
	ADJ	thin
* pool
	XDEF	ADJ.POOL
ADJ.POOL
	ADJ	muddy
* wax
	XDEF	ADJ.WAX
ADJ.WAX
	ADJ	
* gem
	XDEF	ADJ.GEM
ADJ.GEM
	ADJ	
* mirror
	XDEF	ADJ.MIRROR
ADJ.MIRROR
	ADJ	
* coals
	XDEF	ADJ.COALS
ADJ.COALS
	ADJ	hot
* cube2
	XDEF	ADJ.CUBE2
ADJ.CUBE2
	ADJ	ivory
* cube1
	XDEF	ADJ.CUBE1
ADJ.CUBE1
	ADJ	wooden,red
* podium1
	XDEF	ADJ.PODIUM1
ADJ.PODIUM1
	ADJ	red
* podium2
	XDEF	ADJ.PODIUM2
ADJ.PODIUM2
	ADJ	green
* podium3
	XDEF	ADJ.PODIUM3
ADJ.PODIUM3
	ADJ	blue
* podium4
	XDEF	ADJ.PODIUM4
ADJ.PODIUM4
	ADJ	yellow
* podium5
	XDEF	ADJ.PODIUM5
ADJ.PODIUM5
	ADJ	white
* die1
	XDEF	ADJ.DIE1
ADJ.DIE1
	ADJ	red
* die2
	XDEF	ADJ.DIE2
ADJ.DIE2
	ADJ	green
* die3
	XDEF	ADJ.DIE3
ADJ.DIE3
	ADJ	blue
* die4
	XDEF	ADJ.DIE4
ADJ.DIE4
	ADJ	yellow
* die5
	XDEF	ADJ.DIE5
ADJ.DIE5
	ADJ	plastic
* case
	XDEF	ADJ.CASE
ADJ.CASE
	ADJ	opaque,plastic
* slot2
	XDEF	ADJ.SLOT2
ADJ.SLOT2
	ADJ	first,red
* slot3
	XDEF	ADJ.SLOT3
ADJ.SLOT3
	ADJ	second,green
* slot4
	XDEF	ADJ.SLOT4
ADJ.SLOT4
	ADJ	third,blue
* slot5
	XDEF	ADJ.SLOT5
ADJ.SLOT5
	ADJ	fourth,yellow
* privdoor
	XDEF	ADJ.PRIVDOOR
ADJ.PRIVDOOR
	ADJ	office,zoo
* shelf
	XDEF	ADJ.SHELF
ADJ.SHELF
	ADJ	dusty
* rhino
	XDEF	ADJ.RHINO
ADJ.RHINO
	ADJ	ivory,carved
* blackbooks
	XDEF	ADJ.BLACKBOOKS
ADJ.BLACKBOOKS
	ADJ	black,leather_bound
* ladder2
	XDEF	ADJ.LADDER2
ADJ.LADDER2
	ADJ	rope
* insects
	XDEF	ADJ.INSECTS
ADJ.INSECTS
	ADJ	
* reptiles
	XDEF	ADJ.REPTILES
ADJ.REPTILES
	ADJ	
* flora
	XDEF	ADJ.FLORA
ADJ.FLORA
	ADJ	tropical,wonderful,weird
* outside
	XDEF	ADJ.OUTSIDE
ADJ.OUTSIDE
	ADJ	
* wrain
	XDEF	ADJ.WRAIN
ADJ.WRAIN
	ADJ	dreary
* wcloud
	XDEF	ADJ.WCLOUD
ADJ.WCLOUD
	ADJ	feathery
* wwind
	XDEF	ADJ.WWIND
ADJ.WWIND
	ADJ	howling
* suit
	XDEF	ADJ.SUIT
ADJ.SUIT
	ADJ	
* inside
	XDEF	ADJ.INSIDE
ADJ.INSIDE
	ADJ	
* wratrace
	XDEF	ADJ.WRATRACE
ADJ.WRATRACE
	ADJ	
* wetplaces
	XDEF	ADJ.WETPLACES
ADJ.WETPLACES
	ADJ	
* wdrip
	XDEF	ADJ.WDRIP
ADJ.WDRIP
	ADJ	
* organ
	XDEF	ADJ.ORGAN
ADJ.ORGAN
	ADJ	
* billiards
	XDEF	ADJ.BILLIARDS
ADJ.BILLIARDS
	ADJ	
* igate
	XDEF	ADJ.IGATE
ADJ.IGATE
	ADJ	iron
* ckey
	XDEF	ADJ.CKEY
ADJ.CKEY
	ADJ	grotty,dirty
* flap
	XDEF	ADJ.FLAP
ADJ.FLAP
	ADJ	counter
* shards
	XDEF	ADJ.SHARDS
ADJ.SHARDS
	ADJ	sharp
* mill3
	XDEF	ADJ.MILL3
ADJ.MILL3
	ADJ	wind
* lfish
	XDEF	ADJ.LFISH
ADJ.LFISH
	ADJ	live
* dfish
	XDEF	ADJ.DFISH
ADJ.DFISH
	ADJ	dead
* pfish
	XDEF	ADJ.PFISH
ADJ.PFISH
	ADJ	dead
* plaque1
	XDEF	ADJ.PLAQUE1
ADJ.PLAQUE1
	ADJ	
* legs
	XDEF	ADJ.LEGS
ADJ.LEGS
	ADJ	hairy
* feet
	XDEF	ADJ.FEET
ADJ.FEET
	ADJ	
* arms
	XDEF	ADJ.ARMS
ADJ.ARMS
	ADJ	
* player
	XDEF	ADJ.PLAYER
ADJ.PLAYER
	ADJ	
* fist
	XDEF	ADJ.FIST
ADJ.FIST
	ADJ	
* hand
	XDEF	ADJ.HAND
ADJ.HAND
	ADJ	
* plaque2
	XDEF	ADJ.PLAQUE2
ADJ.PLAQUE2
	ADJ	
* upbutton
	XDEF	ADJ.UPBUTTON
ADJ.UPBUTTON
	ADJ	top,first
* downbutton
	XDEF	ADJ.DOWNBUTTON
ADJ.DOWNBUTTON
	ADJ	bottom,second
* abstract
	XDEF	ADJ.ABSTRACT
ADJ.ABSTRACT
	ADJ	abstract
* bed2up
	XDEF	ADJ.BED2UP
ADJ.BED2UP
	ADJ	kingsize,king,size,sized
* hole
	XDEF	ADJ.HOLE
ADJ.HOLE
	ADJ	dark,black
* guild
	XDEF	ADJ.GUILD
ADJ.GUILD
	ADJ	
* spell
	XDEF	ADJ.SPELL
ADJ.SPELL
	ADJ	
* chips
	XDEF	ADJ.CHIPS
ADJ.CHIPS
	ADJ	mineral
* trunk
	XDEF	ADJ.TRUNK
ADJ.TRUNK
	ADJ	heavy
* sash
	XDEF	ADJ.SASH
ADJ.SASH
	ADJ	white
* banquet
	XDEF	ADJ.BANQUET
ADJ.BANQUET
	ADJ	
* race
	XDEF	ADJ.RACE
ADJ.RACE
	ADJ	rat
* staircase
	XDEF	ADJ.STAIRCASE
ADJ.STAIRCASE
	ADJ	
* popening
	XDEF	ADJ.POPENING
ADJ.POPENING
	ADJ	
* pentrance
	XDEF	ADJ.PENTRANCE
ADJ.PENTRANCE
	ADJ	
* ppassage
	XDEF	ADJ.PPASSAGE
ADJ.PPASSAGE
	ADJ	
* pair
	XDEF	ADJ.PAIR
ADJ.PAIR
	ADJ	
* pdust
	XDEF	ADJ.PDUST
ADJ.PDUST
	ADJ	
* pcorner
	XDEF	ADJ.PCORNER
ADJ.PCORNER
	ADJ	
* pface
	XDEF	ADJ.PFACE
ADJ.PFACE
	ADJ	
* pwheat
	XDEF	ADJ.PWHEAT
ADJ.PWHEAT
	ADJ	golden
* baron
	XDEF	ADJ.BARON
ADJ.BARON
	ADJ	
* spade2
	XDEF	ADJ.SPADE2
ADJ.SPADE2
	ADJ	gravedigger__s
* zoopapers
	XDEF	ADJ.ZOOPAPERS
ADJ.ZOOPAPERS
	ADJ	official,zoo
* board2
	XDEF	ADJ.BOARD2
ADJ.BOARD2
	ADJ	
* lunchsign
	XDEF	ADJ.LUNCHSIGN
ADJ.LUNCHSIGN
	ADJ	out_to_lunch
* lchairs
	XDEF	ADJ.LCHAIRS
ADJ.LCHAIRS
	ADJ	comfortable
* lfloor
	XDEF	ADJ.LFLOOR
ADJ.LFLOOR
	ADJ	
* lrocks
	XDEF	ADJ.LROCKS
ADJ.LROCKS
	ADJ	
* lceiling
	XDEF	ADJ.LCEILING
ADJ.LCEILING
	ADJ	
* lsurface
	XDEF	ADJ.LSURFACE
ADJ.LSURFACE
	ADJ	
* oneroom
	XDEF	ADJ.ONEROOM
ADJ.ONEROOM
	ADJ	cubical
* tworoom
	XDEF	ADJ.TWOROOM
ADJ.TWOROOM
	ADJ	cubical
* threeroom
	XDEF	ADJ.THREEROOM
ADJ.THREEROOM
	ADJ	cubical
* fourroom
	XDEF	ADJ.FOURROOM
ADJ.FOURROOM
	ADJ	cubical
* wmachine
	XDEF	ADJ.WMACHINE
ADJ.WMACHINE
	ADJ	weighing
* pillar
	XDEF	ADJ.PILLAR
ADJ.PILLAR
	ADJ	short,white
* signpost3
	XDEF	ADJ.SIGNPOST3
ADJ.SIGNPOST3
	ADJ	white
* fiveroom
	XDEF	ADJ.FIVEROOM
ADJ.FIVEROOM
	ADJ	cubical
* sixroom
	XDEF	ADJ.SIXROOM
ADJ.SIXROOM
	ADJ	cubical
* dummy
	XDEF	ADJ.DUMMY
ADJ.DUMMY
	ADJ	cubical
* cardbox
	XDEF	ADJ.CARDBOX
ADJ.CARDBOX
	ADJ	wooden,small,card
* card5
	XDEF	ADJ.CARD5
ADJ.CARD5
	ADJ	playing,joker
* card4
	XDEF	ADJ.CARD4
ADJ.CARD4
	ADJ	playing,diamond
* card3
	XDEF	ADJ.CARD3
ADJ.CARD3
	ADJ	playing,club
* card2
	XDEF	ADJ.CARD2
ADJ.CARD2
	ADJ	playing,heart
* card1
	XDEF	ADJ.CARD1
ADJ.CARD1
	ADJ	playing,spade
* hooray
	XDEF	ADJ.HOORAY
ADJ.HOORAY
	ADJ	
* dbox1
	XDEF	ADJ.DBOX1
ADJ.DBOX1
	ADJ	night
* dbox2
	XDEF	ADJ.DBOX2
ADJ.DBOX2
	ADJ	night
* dbox3
	XDEF	ADJ.DBOX3
ADJ.DBOX3
	ADJ	night
* dbox4
	XDEF	ADJ.DBOX4
ADJ.DBOX4
	ADJ	night
* horseshoe
	XDEF	ADJ.HORSESHOE
ADJ.HORSESHOE
	ADJ	horse
* stabledoor
	XDEF	ADJ.STABLEDOOR
ADJ.STABLEDOOR
	ADJ	stable,old,mangy
* kbank
	XDEF	ADJ.KBANK
ADJ.KBANK
	ADJ	
* bsign
	XDEF	ADJ.BSIGN
ADJ.BSIGN
	ADJ	
* oakdoor
	XDEF	ADJ.OAKDOOR
ADJ.OAKDOOR
	ADJ	bank,oak,solid,looking,stout
* bnotice
	XDEF	ADJ.BNOTICE
ADJ.BNOTICE
	ADJ	
* inbank
	XDEF	ADJ.INBANK
ADJ.INBANK
	ADJ	
* antecube
	XDEF	ADJ.ANTECUBE
ADJ.ANTECUBE
	ADJ	
* queue1
	XDEF	ADJ.QUEUE1
ADJ.QUEUE1
	ADJ	shorter,short,nearer,first
* queue2
	XDEF	ADJ.QUEUE2
ADJ.QUEUE2
	ADJ	longer,long,other,further,second
* mandoor
	XDEF	ADJ.MANDOOR
ADJ.MANDOOR
	ADJ	office
* counter2
	XDEF	ADJ.COUNTER2
ADJ.COUNTER2
	ADJ	bank,wooden
* manoffis
	XDEF	ADJ.MANOFFIS
ADJ.MANOFFIS
	ADJ	manager__s
* vaultdoor
	XDEF	ADJ.VAULTDOOR
ADJ.VAULTDOOR
	ADJ	vault,steel
* desk3
	XDEF	ADJ.DESK3
ADJ.DESK3
	ADJ	managers,office
* papers
	XDEF	ADJ.PAPERS
ADJ.PAPERS
	ADJ	
* chair2
	XDEF	ADJ.CHAIR2
ADJ.CHAIR2
	ADJ	comfortable,reclining
* dcabinet
	XDEF	ADJ.DCABINET
ADJ.DCABINET
	ADJ	drinks
* till1
	XDEF	ADJ.TILL1
ADJ.TILL1
	ADJ	
* wall2
	XDEF	ADJ.WALL2
ADJ.WALL2
	ADJ	castle
* till2
	XDEF	ADJ.TILL2
ADJ.TILL2
	ADJ	bank
* staff
	XDEF	ADJ.STAFF
ADJ.STAFF
	ADJ	bank
* teller
	XDEF	ADJ.TELLER
ADJ.TELLER
	ADJ	bank
* dress1
	XDEF	ADJ.DRESS1
ADJ.DRESS1
	ADJ	cheap,imitation
* finbank
	XDEF	ADJ.FINBANK
ADJ.FINBANK
	ADJ	
* key5
	XDEF	ADJ.KEY5
ADJ.KEY5
	ADJ	bank,plastic
* bglass
	XDEF	ADJ.BGLASS
ADJ.BGLASS
	ADJ	bullet,proof
* people
	XDEF	ADJ.PEOPLE
ADJ.PEOPLE
	ADJ	
* manager
	XDEF	ADJ.MANAGER
ADJ.MANAGER
	ADJ	bank,mr,mister
* manthing
	XDEF	ADJ.MANTHING
ADJ.MANTHING
	ADJ	
* white
	XDEF	ADJ.WHITE
ADJ.WHITE
	ADJ	bright
* rubbag
	XDEF	ADJ.RUBBAG
ADJ.RUBBAG
	ADJ	rubbish,black
* keyhole
	XDEF	ADJ.KEYHOLE
ADJ.KEYHOLE
	ADJ	key
* stream
	XDEF	ADJ.STREAM
ADJ.STREAM
	ADJ	
* temple1
	XDEF	ADJ.TEMPLE1
ADJ.TEMPLE1
	ADJ	
* form
	XDEF	ADJ.FORM
ADJ.FORM
	ADJ	


	include "cdadj.i"
	include	"cloneadj.i"

	DC.B       -1

 	ALIGN

	END


* produced on Tue Jun 12 09:38:16 2018

* checksum = 60f691fc
