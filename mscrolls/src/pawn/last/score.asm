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
*	@(#)score.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.SCORE
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"

     XDEF   INITG.E


     XDEF   SC.GGWAT,SC.LVRBO,SC.GTNOT,SC.GVNOT,SC.GOSHE,SC.CBAND
     XDEF   SC.GOTRE,SC.GTCOL,SC.PASSS,SC.KILLA,SC.KILLK,SC.DEVIL
     XDEF   SC.MIXCO,SC.SPLCO,SC.DNROP,SC.BOZJL,SC.BOZPO,SC.FODAL
     XDEF   SC.LEDAL,SC.GOMIN,SC.GTROP,SC.GTCOI,SC.PLANT,SC.PDRG1
     XDEF   SC.PDRG2,SC.GOPLA,SC.GBKEY,SC.PASSB,SC.PASSR,SC.MAZE
     XDEF   SC.VOTE,SC.GBAL,SC.VTGRI,SC.RESPR,SC.RMVBA,SC_PLANT



SC.GGWAT     DC.B   2    ;GIVE GURU WATER
SC.LVRBO     DC.B   2    ;LEVER BOULDER WI HOE &  RAKE
SC.GTNOT     DC.B   1    ;TAKING NOTE FROM KRONOS
SC.GVNOT     DC.B   1    ;GIVING GUARDS NOTE
SC.GOSHE     DC.B   1    ;GETTING INTO SHED
SC.CBAND     DC.B   2    ;COVERING BAND FOR GURU
SC.GOTRE     DC.B   2    ;GETTING INTO TREE TRUNK
SC.GTCOL     DC.B   1    ;GETTING COLOURS/POUCH
SC.PASSS     DC.B   3    ;GETTING PAST SNOWMAN    (?? - HMMM)
SC.KILLA     DC.B   3    ;KILLING ADV             (?? - HMMM)
SC.KILLK     DC.B   5    ;KILLING KRONOS!
SC.DEVIL     DC.B   1    ;MAKING PACT WI DEVIL
SC.MIXCO     DC.B   2    ;MIXING COLOURS
SC.SPLCO     DC.B   0    ;SPLITTING COLOURS       (?? - HMMM)
SC.DNROP     DC.B   3    ;CLIMBING DOWN ROPE FROM PAPERED ROOM
SC.BOZJL     DC.B   2    ;GIVING BEER TO JERRY LEE LEWIS
SC.BOZPO     DC.B   3    ;PASSING PORTER (KNOCKING & WHISKY)
SC.FODAL     DC.B   2    ;LUNCH FOR ALCHEMISTS
SC.LEDAL     DC.B   4    ;LEAD FOR ALCHEMISTS
SC.GOMIN     DC.B   2    ;GET TO MINE WIOUT LOSING HEAD
SC.GTROP     DC.B   3    ;GET ROPE
SC.GTCOI     DC.B   1    ;FIND COIN
SC.PLANT     DC.B   1    ;PLANT POT PLANT IN PLANT POT
SC.PDRG1     DC.B   4    ;GET PAST DRAGON
SC.PDRG2     DC.B   2    ;GET BACK PAST DRAGON
SC.GOPLA     DC.B   2    ;USE PLATFORM TO GO TO HELL
SC.GBKEY     DC.B   2    ;GET BLUE KEY (LAVA RIVER OR K ADV)
SC.PASSB     DC.B   2    ;PASS (OR LEVER?) BOULDERS
SC.PASSR     DC.B   1    ;CLIMB ROCK/ROCKS/ROCKFALL
SC.MAZE      DC.B   0    ;WELL.. ITS IS IRRELEVENT!
SC.VOTE      DC.B   2    ;VOTE
SC.GBAL      DC.B   2    ;GETTING BALLOT PAPER
SC.VTGRI     DC.B   4    ;VOTING FOR GRINGO
SC.RESPR     DC.B   4    ;RESCUE PRINCESS = VOTING
SC.RMVBA     DC.B   10   ;GET BAND REMOVED
SC_PLANT     DC.B   1    ;PLANT POT PLANT IN PLANT POT WI TROWEL

             ALIGN


INITG.E

* end of initial state of game info

            END

