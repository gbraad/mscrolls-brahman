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
*	@(#)data2.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.DATA2
*
*--------------------------------
 
	include "equates.asm"
	include	"macros.asm"
	include	"roomequ.asm"
	include "nounequ1.asm"
	include "nounequ2.asm"


      XDEF     PSUDAT,NPC.DATA,SNOWLOC,NPC.LIST


PSUDAT
 
01$
              DC.B    RNONPATH3,RNFOREST3   ;32 - DOTTED LINE
              DC.B    RNFOREST4,RNFOREST6
              DC.B    RNPLAIN3,RNSCLIFF,0
02$
              DC.B    RNBRIDGE2,RNONBRIDGE   ;35 - STONE BRIDGE
              DC.B    RNGARDENS,0   
03$ 
              DC.B    RNGATEWAY,RNGARDENS   ;PALACE
              DC.B    RNONBRIDGE,0
04$
              DC.B    RNHILLS,RNHILL        ;38 - TERRAIN  
              DC.B    RNFOREST7,RNPLAIN2
              DC.B    RNNCLIFF,0
05$
              DC.B    RNHILL,RNINHUT,0     ;43 - HUT DESC(IN/OUT)
06$
              DC.B    RNONPATH,RNONPATH2    ;60 - GRAVEL 
              DC.B    RNONPATH3,RNONPATH4 
              DC.B    RNONPATH5,0
07$ 
              DC.B    RNSCLIFF,RNBRIDGE2    ;65 - CLIFF
              DC.B    RNNCLIFF,RNONBRIDGE,0
08$
              DC.B    RNTRACK,RNTRACK2      ;ITEM ?
              DC.B    RNTRACK3,0
09$
              DC.B    RNPLATEAU,RNPLATEAU2  ;135 - ICE TOWER
              DC.B    RNPLATEAU3,RNPLATEAU4
              DC.B    RNINTOWER,RNLANDING
              DC.B    RNJAIL,RNWALL
              DC.B    RNSTORE,0
10$
              DC.B    RNLANDING,RNJAIL,0   ;138 - PRINCESS'S DOOR 
11$
              DC.B    RNVSHAFT,RNCAVERN4   ;148 - LAVA RIVER
              DC.B    RNRIVER,RNCIRCROOM,0
12$
              DC.B    RNENTRANCE,RNCORRIDOR2,0   ;152 - PERSPEX DOORS
13$
              DC.B    RNCORRIDOR2,RNROOM,0        ;155 - ADV. DOOR
14$
*             DC.B    RNBRIDGE3,RNBRIDGE4,0  ;192 - ROPE BRIDGE 
       DS.B     3
15$
              DC.B    RNSHAFT,RNSHAFT2,0      ;188 - SHAFT  
16$
              DC.B    RNSHAFT2,RNLEDGE4,0  ;214 - SCREAMS
17$ 
              DC.B    RNUPTREE,RNTREETRUNK,0   ;272 - TREE DOOR
18$
              DC.B    RNGARDENS,RNINSHED,0     ;273 - SHED DOOR
19$
              DC.B    RNPASSAGE3,RNLOUNGE,0    ;293 - FRONT DOOR
20$
              DC.B    RNPASSAGE4,RNLIFT,0      ;302  - LIFT DOOR
21$
              DC.B    RNGATEWAY         ;THIS IS THE (START OF) THE WALL LIST
              DC.B    RNINHUT,RNINSHED
              DC.B    RNINTOWER,RNJAIL
              DC.B    RNSTORE,RNCAVEMOUTH
              DC.B    RNCAVERN,RNCAVE
              DC.B    RNCAVERN2,RNCORRIDOR
              DC.B    RNVSHAFT,RNCAVERN4
              DC.B    RNENTRANCE,RNCORRIDOR2
              DC.B    RNRIVER,RNROOM
              DC.B    RNPASSAGE,RNLAB
              DC.B    RNROOM2,RNCAVERN5
              DC.B    RNCAVERN6,RNCHAMBER
              DC.B    RNROOM4,RNLEDGE2
              DC.B    RNCHAMBER2,RNWORKSHOP
              DC.B    RNTUNNEL,RNROOM5
              DC.B    RNLEDGE3,RNPASSAGE2
              DC.B    RNLEDGE4,RNTREETRUNK
              DC.B    RNSTAIRCASE,RNLOWCEILING
              DC.B    RNCROSSROADS,RNPASSAGE3
              DC.B    RNLOUNGE,RNKITCHEN
              DC.B    RNPASSAGE4,RNLIFT  
              DC.B    RNROCKFACE,RNWHITEROOM
              DC.B    RNOFFICE,RNBOOTH
              DC.B    RNCHAMBER4,0
 
22$
              DC.B    RNPASSAGE2,RNSHAFT2,0 ;DOUBLE DOORS  - EX PORTCULLIS
23$
SNOWLOC        EQU  *-PSUDAT
              DC.B    RNPLATEAU,RNPLATEAU2
              DC.B    RNPLATEAU3,RNPLATEAU4,0  ;SNOW   
 
              DC.B    RNFOREST2,RNFOREST3,RNCLEARING   ;TREES
              DC.B    RNFOREST4,RNFOREST5
              DC.B    RNFOREST6,RNFOREST7,0
 
              DC.B    RNBRIDGE2,RNNCLIFF  ;RIVER & RAVINE
              DC.B    RNSCLIFF,RNONBRIDGE
              DC.B    RNCHAMBER3
              DC.B    RNLEDGE           ;ROPE BRIDGE STARTS HERE (SHARED)
              DC.B    RNLEDGE2,RNBRIDGE3
              DC.B    RNBRIDGE4
              DC.B    0
 
              DC.B    RNMAZE,RNROOM,RNCLEARING    ;POSTER
              DC.B    RNUPTREE,RNTREETRUNK
              DC.B    RNSTAIRCASE,RNPASSAGE,0
 
 
 
       ALIGN 
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
* BIT #2 - ARTICLE - 
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
 
       DC.B       $00,$00,$03,$00 ; GUARDS
       DC.B       $00,$00,$0B,$00 ; JOHN
       DC.B       $00,$00,$07,$00 ; DONKEY
       DC.B       $00,$00,$03,$00 ; ADVE
       DC.B       $00,$00,$01,$02 ; HORSE
       DC.B       $00,$00,$63,$02 ; GURU
       DC.B       $00,$00,$07,$00 ; BATS
       DC.B       $00,$00,$03,$00 ; DEVIL
       DC.B       $00,$00,$03,$00 ; DEMONS
       DC.B       $00,$00,$BB,$00 ; LEWIS    
       DC.B       $00,$00,$03,$00 ; DRAGON
       DC.B       $00,$00,$F3,$00 ; RABBIT
       DC.B       $00,$00,$93,$00 ; PORTER
       DC.B       $00,$00,$07,$00 ; SNOWMAN
       DC.B       $00,$00,$10,$00 ; PRINCESS
       DC.B       $00,$00,$D7,$00 ; HOBBITS
       DC.B       $00,$00,$03,$00 ; MAGICIAN
       DC.B       $00,$00,$03,$00 ; ALCH
       DC.B       $00,$00,$0B,$00 ; ERIK
       DC.B       $00,$00,$03,$00  ; VOICE 
       DC.L	0

********************************************
*
* NPC.LIST - a go faster feature 
*
* this is a list of the NOUN no.s of npcs
* GETNPCNO uses it to convert noun no.s 
* to npc no.s by scanning it 
*
* this may appear slow but it beats scanning
* the noun data!
*
********************************************

NPC.LIST

       DC.W       NGUARDS
       DC.W       NJOHN
       DC.W       NDONKEY
       DC.W       NADVENT
       DC.W       NHORSE
       DC.W       NGURU
       DC.W       NBATS
       DC.W       NDEVIL
       DC.W       NDEMONS
       DC.W       NLEWIS    
       DC.W       NDRAGON
       DC.W       NRABBIT
       DC.W       NPORTER
       DC.W       NSNOWMAN
       DC.W       NPRINCESS
       DC.W       NSHAPES
       DC.W       NMAGICIAN
       DC.W       NALCH
       DC.W       NERIK
       DC.W       NVOICE 

       DC.W       $8000		;TERMINATOR so CMP (A1)+,D0 exits ok
        

      END

