* 
* 
*     _    __        _      __                           
*    | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
*    | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
*    | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
*    |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
*                                                        
*  Copyright (c) 2016-2017 Voidware Ltd.  All Rights Reserved.
* 
*  This file contains Original Code and/or Modifications of Original Code as
*  defined in and that are subject to the Voidware Public Source Licence version
*  1.0 (the 'Licence'). You may not use this file except in compliance with the
*  Licence or with expressly written permission from Voidware.  Please obtain a
*  copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
*  it before using this file.
* 
*  THIS IS NOT FREE SOFTWARE.
*  
*  The Original Code and all software distributed under the Licence are
*  distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
*  OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
*  WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
*  PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
* 
*  Please see the Licence for the specific language governing rights and 
*  limitations under the Licence.
* 
*  contact@strandgames.com
* 
*  This software was originally written and owned By Magnetic Scrolls Ltd.
*  It is being released under a Public Source Licence for historical reference
*  and educational purposes.
*


******************************************************
*
*	@(#)adjectiv.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.ADJECTIVES1
*
*--------------------------------
  
	include "equates.asm"
	include	"macros.asm"

      XDEF        ADJ.TBL

ADJ.TBL
  
 
*ON THE PATH 
       DC.B       $00
*ON THE PATH
       DC.B       $00
*ON THE PATH
       DC.B       $00
       DC.B       $05,$06 
*VOID
       DC.B       $00
       DC.B       $07,$08,$09,$0A
*FOREST
       DC.B       $00
       DC.B       $07,$08,$09,$0A
*FOREST
       DC.B       $00
       DC.B       $DB        ;FOREST
*CLEARING
       DC.B       $00
       DC.B       $07,$08,$09,$0A  
*FOREST
       DC.B       $00
       DC.B       $07,$08,$09,$0A   
*FOREST
       DC.B       $00
       DC.B       $07,$08,$09,$0A  
*FOREST
       DC.B       $00
       DC.B       $07,$08,$09,$0A  
*FOREST
       DC.B       $00
*ON THE PATH
       DC.B       $00
*ON THE PATH
       DC.B       $00
       DC.B       $0B,$0C,$AB
*PLAIN
       DC.B       $00
       DC.B       $0B,$0C,$AB
*WILDERNESS
       DC.B       $00
       DC.B       $0B,$0C,$AB
*PLAIN
       DC.B       $00
*BANK
       DC.B       $00
       DC.B       $0D
*BRIDGE
       DC.B       $00
*CLIFF
       DC.B       $00
*ON THE BRIDGE
       DC.B       $00
       DC.B       $0F,$B3
*GARDENS
       DC.B       $00
*GATEWAY
       DC.B       $00
       DC.B       $7B
*MAZE
       DC.B       $00
       DC.B       $11
*HILLS
       DC.B       $00
       DC.B       $12,$AD
*HILLS
       DC.B       $00
       DC.B       $48
*HILL
       DC.B       $00
       DC.B       $AC
*HILLS
       DC.B       $00
*FOOTHILLS
       DC.B       $00
*INSIDE THE HUT
       DC.B       $00
*IN THE SHED
       DC.B       $00
       DC.B       $7E,$32
*SHIRT
       DC.B       $00
       DC.B       $13,$37
*LINE
       DC.B       $00
       DC.B       $14
*BOARD
       DC.B       $00
*WRITING
       DC.B       $00
       DC.B       $0D
*BRIDGE
       DC.B       $00
       DC.B       $AE
*POT
       DC.B       $00
       DC.B       $AF
*SHED
       DC.B       $00
       DC.B       $16,$18
*TERRAIN
       DC.B       $00
*ARCHWAY
       DC.B       $00
       DC.B       $19,$B0 
*GUARDS
       DC.B       $00
*EMBLEMS
       DC.B       $00
       DC.B       $1A
*PIKE
       DC.B       $00
       DC.B       $1B,$B1,$2A,$B2
*HUT
       DC.B       $00
       DC.B       $1C
*MUD
       DC.B       $00
       DC.B       $17
*PLANT
       DC.B       $00
       DC.B       $1D,$1F,$32
*JEANS
       DC.B       $00
       DC.B       $1E
*ARMOUR (ADVENS)
       DC.B       $00
*PONCHO
       DC.B       $00
*STETSON
       DC.B       $00
*CHEROOT
       DC.B       $00
*MAGNUM
       DC.B       $00
       DC.B       $20
*JOHN
       DC.B       $00
       DC.B       $1A
*CART
       DC.B       $00
       DC.B       $21,$2A
*DONKEY
       DC.B       $00
       DC.B       $22
*WALL
       DC.B       $00
       DC.B       $23
*WIRE
       DC.B       $00
       DC.B       $24
*FOUNTAIN
       DC.B       $00
       DC.B       $25,$26
*ADVENTURER 
       DC.B       $00
       DC.B       $06
*HORSE
       DC.B       $00
       DC.B       $7E
*GRAVEL
       DC.B       $00
*HOE
       DC.B       $00
       DC.B       $28,$B4
*POUCH
       DC.B       $00
*TROWEL
       DC.B       $00
*RAKE
       DC.B       $00
       DC.B       $B5
*CLIFF
       DC.B       $00
*WHEELBARROW
       DC.B       $00
       DC.B       $1F
*BLUE
       DC.B       $00
       DC.B       $30
*GREEN
       DC.B       $00
       DC.B       $37
*RED
       DC.B       $00
       DC.B       $29,$2A
*GURU
       DC.B       $00
       DC.B       $D4,$2B
*ROBE
       DC.B       $00
       DC.B       $B6,$2C,$33
*BOWL
       DC.B       $00
       DC.B       $2D
*BOULDER
       DC.B       $00
       DC.B       $2E
*BORDERS
       DC.B       $00
       DC.B       $2F
*BUSHES
       DC.B       $00
       DC.B       $1A
*WORKBENCH
       DC.B       $00
       DC.B       $31,$32
*WRISTBAND
       DC.B       $00
       DC.B       $33,$85 
*WATERFALL
       DC.B       $00
       DC.B       $35,$36
*FOG
       DC.B       $00
       DC.B       $04
*PATH
       DC.B       $00
       DC.B       $38,$39
*RICE
       DC.B       $00
       DC.B       $7E
*ROSE
       DC.B       $00
       DC.B       $0E
*TRACK
       DC.B       $00
       DC.B       $0E
*TRACK
       DC.B       $00
       DC.B       $0E
*TRACK
       DC.B       $00
       DC.B       $05,$42,$3C    
*PLATEAU
       DC.B       $00
       DC.B       $05,$42,$3C    
*PLATEAU
       DC.B       $00
       DC.B       $05,$42,$3C    
*PLATEAU
       DC.B       $00
       DC.B       $05,$42,$3C    
*PLATEAU
       DC.B       $00
*INSIDE THE TOWER
       DC.B       $00
*LANDING
       DC.B       $00
*ROOM OF INCARCERATION
       DC.B       $00
       DC.B       $B7 
*WALL
       DC.B       $00
*STORE
       DC.B       $00
       DC.B       $22,$9E
*CAVEMOUTH
       DC.B       $00
       DC.B       $48,$49
*CAVERN
       DC.B       $00
       DC.B       $B8
*CAVERN
       DC.B       $00
       DC.B       $33,$4B
*CAVE   
       DC.B       $00
       DC.B       $6F,$38
*CORRIDOR
       DC.B       $00
       DC.B       $5E     ;VERTICAL
*VERTICAL SHAFT
       DC.B       $00
* CAVERN (NORTH SIDE OF LAVA)
       DC.B       $00
       DC.B       $0E
*CORRIDOR
       DC.B       $00
       DC.B       $38,$B9
*CORRIDOR
       DC.B       $00
*RIVER
       DC.B       $00
       DC.B       $BA
*ROOM
       DC.B       $00
       DC.B       $53,$54
*PASSAGE
       DC.B       $00
*LABORATORY
       DC.B       $00
       DC.B       $27
*ROOM
       DC.B       $00
       DC.B       $33  
*CAVERN
       DC.B       $00
*HELL
       DC.B       $00
*CAVERN
       DC.B       $00
       DC.B       $06
*CHAMBER
       DC.B       $00
       DC.B       $1B
*ROOM
       DC.B       $00
       DC.B       $33
*ROOM
       DC.B       $00
       DC.B       $5E
*SHAFT
       DC.B       $00
       DC.B       $22   ;HIGH
*LEDGE
       DC.B       $00
       DC.B       $63,$62,$2A 
*BRIDGE
       DC.B       $00
       DC.B       $63,$62,$2A
*BRIDGE
       DC.B       $00
       DC.B       $22   ;HIGH   
*LEDGE
       DC.B       $00
       DC.B       $58,$64
*CHAMBER
       DC.B       $00
*WORKSHOP
       DC.B       $00
       DC.B       $1B,$0D
*PLATFORM
       DC.B       $00
       DC.B       $69
*TUNNEL
       DC.B       $00
*ROOM
       DC.B       $00
*LEDGE
       DC.B       $00
       DC.B       $6E,$6F
*PASSAGE 
       DC.B       $00
       DC.B       $3B,$3E,$5E
*SHAFT
       DC.B       $00
*LEDGE
       DC.B       $00
       DC.B       $41
*CHAMBER
       DC.B       $00
       DC.B       $0E
*TRACK
       DC.B       $00
*ROCKFALL
       DC.B       $00
*CAVE
       DC.B       $00
*STAIRWAY
       DC.B       $00
       DC.B       $05,$42,$3C    
*PLATEAU
       DC.B       $00
       DC.B       $3D,$3E,$3F
*TOWER
       DC.B       $00
       DC.B       $05,$06,$3C,$46
*SNOW
       DC.B       $00
       DC.B       $45,$9C
*SLOPE
       DC.B       $00
       DC.B       $40,$1A  
*DOOR
       DC.B       $00
       DC.B       $72   
*STRAPS
       DC.B       $00
       DC.B       $44,$8D
*BED
       DC.B       $00
       DC.B       $73
*BOOTS
       DC.B       $00
*ROPE
       DC.B       $00
*BATS
       DC.B       $00
       DC.B       $4A
*POOL
       DC.B       $00
*STALAGTITES
       DC.B       $00
*STALAGMITES
       DC.B       $00
*REM
       DC.B       $00
       DC.B       $4E  
*RIVER
       DC.B       $00
       DC.B       $74
*LAVA
       DC.B       $00
       DC.B       $4F,$0D,$C2,$1F
*PEDESTAL
       DC.B       $00
*NOTICE
       DC.B       $00
       DC.B       $50
*DOORS
       DC.B       $00
       DC.B       $75
*LAMPS
       DC.B       $00
       DC.B       $52  
*WINDOWS
       DC.B       $00
       DC.B       $BB,$55
*DOOR
       DC.B       $00
*PAPER
       DC.B       $00
*LISTING
       DC.B       $00
       DC.B       $48
*FAN
       DC.B       $00
*ASH
       DC.B       $00
*COMPUTERS
       DC.B       $00
*PERIPHERALS
       DC.B       $00
       DC.B       $81,$82
*MAT
       DC.B       $00
       DC.B       $52
*LIMBECKS
       DC.B       $00
       DC.B       $52
*FLASKS
       DC.B       $00
*--------------------------------
*
*  SRC.ADV.ADJECTIVES2
*
*--------------------------------
 
 
       DC.B       $55
*LIQUID
       DC.B       $00
*TUBES
       DC.B       $00
*VAPOUR
       DC.B       $00
       DC.B       $33
*BOXES
       DC.B       $00
       DC.B       $4D
*JARS
       DC.B       $00
       DC.B       $76
*CHEST
       DC.B       $00
       DC.B       $35
*BLACKNESS
       DC.B       $00
       DC.B       $51
*BOTTLE
       DC.B       $00
       DC.B       $33
*GEYSERS
       DC.B       $00
*VITRIOL
       DC.B       $00
       DC.B       $56,$BC
*SCYTHES
       DC.B       $00
       DC.B       $59
*DEVIL
       DC.B       $00
       DC.B       $58
*THRONE
       DC.B       $00
       DC.B       $57,$5A
*EYES
       DC.B       $00
       DC.B       $5B
*DEMONS
       DC.B       $00
       DC.B       $5C
*CORPSES
       DC.B       $00
*NOOSES
       DC.B       $00
*FRIDGE
       DC.B       $00
*HEADS
       DC.B       $00
       DC.B       $1A
*KEY
       DC.B       $00
*CRYSTALS
       DC.B       $00
*PIANO
       DC.B       $00
*LEWIS
       DC.B       $00
       DC.B       $3E,$5E,$3B
*SHAFT
       DC.B       $00
       DC.B       $5F
*LADDER
       DC.B       $00
       DC.B       $66,$61
*CLOUD
       DC.B       $00
*LEDGE
       DC.B       $00
       DC.B       $62,$2A,$63
*BRIDGE
       DC.B       $00
       DC.B       $1A
*PLANKS
       DC.B       $00
       DC.B       $60,$65
*DRAGON
       DC.B       $00
       DC.B       $67
*TREASURE
       DC.B       $00
       DC.B       $66
*SHADOWS
       DC.B       $00
       DC.B       $72
*KEY
       DC.B       $00
       DC.B       $6A
*LAMP
       DC.B       $00
       DC.B       $68,$48,$BD
*TABLE
       DC.B       $00
       DC.B       $7C
*HAT
       DC.B       $00
       DC.B       $78,$7A,$06
*RABBIT
       DC.B       $00
       DC.B       $2A,$69
*BOOKCASES
       DC.B       $00
       DC.B       $2A,$69
*TOMES
       DC.B       $00
*NICHE
       DC.B       $00
       DC.B       $1B,$0D
*PLATFORM
       DC.B       $00
*WATER
       DC.B       $00
       DC.B       $1A
*POSTS
       DC.B       $00
*CUPBOARD
       DC.B       $00
*HOOK
       DC.B       $00
*PASTE
       DC.B       $00
       DC.B       $BE
*WALLPAPER
       DC.B       $00
       DC.B       $84
*PORTCULLIS
       DC.B       $00
       DC.B       $6D
*PORTER
       DC.B       $00
       DC.B       $57
*SCREAMS
       DC.B       $00
       DC.B       $9D   ;LOW
*WALL
       DC.B       $00
*RIVER
       DC.B       $00
*WATER
       DC.B       $00
*SNOWMAN
       DC.B       $00
       DC.B       $32
*POCKET
       DC.B       $00
       DC.B       $79
*PRINCESS
       DC.B       $00
*GRAFFITI
       DC.B       $00
*PAPER
       DC.B       $00
*FLAME
       DC.B       $00
       DC.B       $85,$3C
*WATER
       DC.B       $00
*SHAPES
       DC.B       $00
*AEROSOUL
       DC.B       $00
       DC.B       $7F
*MAGICIAN
       DC.B       $00
       DC.B       $7D
*HAT
       DC.B       $00
       DC.B       $7E,$80
*CLOAK
       DC.B       $00
       DC.B       $BF
*RACK
       DC.B       $00
*WAND
       DC.B       $00
       DC.B       $BD
*CHEST
       DC.B       $00
*NICHE
       DC.B       $00
       DC.B       $24,$0D
*RATS
       DC.B       $00
       DC.B       $3D,$47
*ICE TABLE
       DC.B       $00
*ALCHEMISTS
       DC.B       $00
*ROCKS
       DC.B       $00
       DC.B       $43
*STUMP
       DC.B       $00
*WANDS
       DC.B       $00
       DC.B       $C0,$70
*NOTE
       DC.B       $00
       DC.B       $15
*RAVINE
       DC.B       $00
*WHISKY
       DC.B       $00
*CUPBOARD
       DC.B       $00
       DC.B       $C1
*ERIK
       DC.B       $00
       DC.B       $52
*PRISM
       DC.B       $00
       DC.B       $1F
*KEY
       DC.B       $00
       DC.B       $C3,$87
*WATER
       DC.B       $00
       DC.B       $8F
*SYMBOLS
       DC.B       $00
       DC.B       $06
*DRESS
       DC.B       $00
*SEAL
       DC.B       $00
       DC.B       $5F,$8A,$39
*RATIONS
       DC.B       $00
       DC.B       $A9        ;FIRST
       DC.B       $7C        ;TOP
*BUTTON
       DC.B       $00
       DC.B       $C4,$31
*ARMOUR
       DC.B       $00
       DC.B       $C5
*BOTTLE
       DC.B       $00
       DC.B       $30
*ROSE
       DC.B       $00
       DC.B       $8D,$8E
*COIN
       DC.B       $00
       DC.B       $8C,$91
*ALE
       DC.B       $00
       DC.B       $8B
*BULLETS   
       DC.B       $00
       DC.B       $90
*POSTER
       DC.B       $00
       DC.B       $4B,$32   ;BARE
*HAND
       DC.B       $00
*FEET
       DC.B       $00
       DC.B       $32
*ARMS
       DC.B       $00
       DC.B       $32
*FIST
       DC.B       $00
       DC.B       $E0,$32
*HEAD (MY)
       DC.B       $00
       DC.B       $88
*BELT
       DC.B       $00
       DC.B       $30,$3B  
*BOTTLE
       DC.B       $00
*QUILT
       DC.B       $00
       DC.B       $91
*BOTTLE
       DC.B       $00
*DUST
       DC.B       $00
       DC.B       $94,$95,$96,$97
*TREE
       DC.B       $00
*UP THE TREE
       DC.B       $00
       DC.B       $1A,$99,$43
*DOOR
       DC.B       $00
       DC.B       $C9,$9A,$2A
*DOOR
       DC.B       $00
       DC.B       $D7,$43,$D8
*TREE TRUNK ROOM
       DC.B       $00
       DC.B       $D9  
*STAIRCASE
       DC.B       $00
       DC.B       $DA,$9D,$9E
*LOW CEILINGED ROOM
       DC.B       $00
*CROSSROADS
       DC.B       $00
*PASSAGE
       DC.B       $00
*LOUNGE        
       DC.B       $00
*KITCHEN
       DC.B       $00
*PASSAGE
       DC.B       $00
*LIFT
       DC.B       $00
*ROCKFACE
       DC.B       $00
       DC.B       $06
*WHITE ROOM
       DC.B       $00
*OFFICE
       DC.B       $00
       DC.B       $9F
*VOTTING BOOTH
       DC.B       $00
       DC.B       $A0
*CHAMBER
       DC.B       $00
       DC.B       $A1,$1A,$D4
*FLOORBOARDS
       DC.B       $00
       DC.B       $2A,$1A
*STAIRCASE
       DC.B       $00
       DC.B       $05,$06   ;PIPE WAS  A2A3
*WHITE  (WAS PIPE)
       DC.B       $00
*TREE (SO AS ENTRE TREE WORKS) WAS SPIKE #1
       DC.B       $00
       DC.B       $75
*WRITING
       DC.B       $00
       DC.B       $CA,$CB,$37
*DOOR
       DC.B       $00
*SETEE
       DC.B       $00
*CUSHIONS
       DC.B       $00
       DC.B       $CC,$CB,$61
*HAT
       DC.B       $00
       DC.B       $CD
*HOOKS
       DC.B       $00
       DC.B       $CE,$CF
*STOVE 
       DC.B       $00
       DC.B       $76,$D0,$92,$D1
*POT
       DC.B       $00
*CARROT
       DC.B       $00
       DC.B       $D2
*MESSAGE
       DC.B       $00
       DC.B       $DD        ;LIFT
*DOOR
       DC.B       $00
*WORKTOP
       DC.B       $00
       DC.B       $1A        ;METAL
*PLAQUE
       DC.B       $00
       DC.B       $AA          ;SECOND
       DC.B       $DE          ;BOTTOM
*BUTTON
       DC.B       $00
       DC.B       $A4          ;LEAD
*LEAD LUMP (!!??)
       DC.B       $00
*DESK
       DC.B       $00
       DC.B       $A5,$A6        ;????
*SAFE
       DC.B       $00
       DC.B       $83        ;BALLOT   
*PAPER
       DC.B       $00
       DC.B       $48,$83        ;BALLOT
*BOX 
       DC.B       $00
       DC.B       $1A
*TABLE
       DC.B       $00
       DC.B       $2A        ;OLD
*RUG
       DC.B       $00
       DC.B       $6A
*LANTERN
       DC.B       $00
*TREES
       DC.B       $00
*LEDGE
       DC.B       $00
       DC.B       $72
*PLAYER - WAS SPIKE #2
       DC.B       $00
*SPELL 
       DC.B       $00
*PALACE (PSEUDO)
       DC.B       $10        ;GOLDEN
       DC.B       $00
       DC.B       $10
*PALACE
       DC.B       $00
       DC.B       $A7,$A8
*HEAD
       DC.B       $00
       DC.B       $33,$83   ;SMALL , BALLOT
* BALLOT BOX (SMALL)
       DC.B       $00
       DC.B       $DF    ;AEROSOUL
*BUTTON (AEROSOUL)
       DC.B       $00
       DC.B       $D5
*CUPBOARD DOORS
       DC.B       $00
       DC.B       $0D,$06   ;STONE
* STONE WALLS
       DC.B       $00
       DC.B       $DC   ;PAPER
*PAPER WALLS
       DC.B       $00
*WINDOW (INSIDE JAIL)
       DC.B       $00
*WINDOW (OUTSIDE)
       DC.B       $00
       DC.B       $DD    ;LIFT
*LIFT BUTTON
       DC.B       $00
* VOICE
* GOOD
       DC.B       $00
       DC.B       $00
* EVIL
       DC.B       $00 
* LIFE 
       DC.B       $00  
* DEATH  
       DC.B       $00  
* GRINGO
       DC.B       $00  
* GNOMES
       DC.B       $00   
* KEROVNIA
       DC.B       $00    
       DC.B       $7F
* EVIL PERSON
       DC.B       $00
* FERGS
       DC.B       $00
* GOLD
       DC.B       $00
* ALCHEMY
       DC.B       $00
       DC.B       -1

       ALIGN

       END

