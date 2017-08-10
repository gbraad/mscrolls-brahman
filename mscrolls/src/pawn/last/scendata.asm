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
*	@(#)scendata.asm	2.1
*	9/19/86
*
******************************************************

*--------------------------------
*
*      SCENDATA.ASM
*
* stuff that used to be in COMMON.ASM 
* noun included by it so as COMMON really is 
* common across scenarios
*
*--------------------------------

   
*--------------------------------
*
* COMMON ROUTINES - *NOT* SAVED
* WITH SAVE/LOAD GAME
*
* NOT TO BE MODIFIED BY PROG!!
*
*--------------------------------
  
  
*--------------------------------
*
* INIT.TAB - LIST OF INITIAL STATE
*            DESCRIPTIONS FOR SOME
*            NOUNS
*
* FORMAT :    DC.W    MSG#,NOUN#
*
* MSGS ARE SCENARIO ONES
*
*--------------------------------
 
INIT.TAB
  
 
              DC.W    106,NGATEWAY
              DC.W    107,NFOOTHILLS
              DC.W    108,NHILL
              DC.W    109,NGURU
              DC.W    110,NINHUT
*             DC.W    111,NHORSE     
              DC.W    114,NSHAFT
*             DC.W    112,NPWALL
*             DC.W    118,NRABBIT  
              DC.W    45,NPLAIN2
              DC.W    289,NFLOORBOARD 
              DC.W    377,NHARDHAT
              DC.W    125,NPRINCESS
              DC.W    375,NROOM    
              DC.L    0
  
*--------------------------------
*
* MLISTNGO -  LIST OF MESSAGES
*             USED FOR CAN'T GO
*             THAT WAY SITUATIONS
*
* FIRST WORD NEVER USED
*
*--------------------------------
 
MLISTNGO
       
              DC.W    0        ;PADDING
              DC.W    170      ;$FF - CLOSED DOOR
              DC.W    26       ;$FE - UNDERGROWTH
              DC.W    68       ;$FD - LAVA 2 HOT FOR HORSE
              DC.W    167      ;$FC - TREES TOO SLIPPERY
              DC.W    168      ;$DB - UNDERGROWTH AGAIN
              DC.W    169      ;$FA - NO WINGS
              DC.W    170      ;$F9 - DOOR (JAMMED)
              DC.W    171      ;$F8 - WALL
              DC.W    172      ;$F7 - BREAK LEGS
              DC.W    173      ;$F6 - MOUNTAINSIDE
              DC.W    174      ;$F5 - LAVA NOT TO INVITING
              DC.W    175      ;$F4 - LOCKED DOOR
              DC.W    176      ;$F3 - SNOWMAN
              DC.W    177      ;$F2 - GUARDS
              DC.W    178      ;$F1 - 6 SHARP...
              DC.W    305      ;$F0 - LIFT WILL NOT GO THAT WAY
              DC.W    306      ;$EF - DOTTED LINE
              DC.W    307      ;$EE - BOLDER
              DC.W    308      ;$ED - ROCKFALL
              DC.W    309      ;$EC - TOWER IN WAY
              DC.W    310      ;$EB - ICE TOO SLIPPERY
              DC.W    311      ;$EA - NO FOOT HOLDS
 
 
 
   
*--------------------------------
*
* MOVE.TAB - UNCONDITIONAL
*            'TELEPORTS' TO A ROOM
*            IF 'GO XYZZY'
*
*--------------------------------
   
MOVE.TAB
 
              DC.W    NHUT
              DC.B    DIR_W,RNINHUT

              DC.W    NCAVE2
              DC.B    DIR_E,RNCAVEMOUTH

              DC.W    NSTAIRWAY
              DC.B    DIR_U,RNPLATEAU

              DC.W    NROCKFALL
              DC.B    DIR_U,RNTRACK2

              DC.W    NROCKS
              DC.B    DIR_U,RNTRACK2    ;LIKE ROCKFALL

              DC.W    NTREE
              DC.B    DIR_U,RNUPTREE

              DC.W    NSLOPE
              DC.B    DIR_U,RNLANDING ;SLOPE    

              DC.W    NWINDOW1
              DC.B    DIR_E,RNWALL

              DC.W    NWINDOW2
              DC.B    DIR_U,RNJAIL

              DC.W    NTOWER
              DC.B    DIR_NE,RNINTOWER

              DC.W    NPLATFORM2
              DC.B    DIR_N,RNPLATFORM    ;WORKSHOP=PLATFORM

              DC.W    NBRIDGE2
              DC.B    DIR_E,RNONBRIDGE

              DC.L    0
 
 
*--------------------------------
*
* PP.OP.CL   -   LIST OF NOUNS
*                TO GET OPENED
*                OR CLOSED IF
*                PUSHED OR PULLED
*
* (PUSH/PULL.OPEN/CLOSE)
*
*  NO TERMINATOR - DOORS TABLE
*  IS PART OF IT 
*
*  FORMAT -   DC.W    NOUN#,-1
*
*  BEWARE OF THE PADDING!!
*
*--------------------------------
  
PP.OP.CL
 
              DC.W    NHANDLES,-1
              DC.W    NCUPBOARD,-1
              DC.W    NWINDOW1,-1         ;in/outside princess's room
              DC.W    NWINDOW2,-1
 
  
*--------------------------------
*
*  DOORS - DIRECTIONS TO PATCH
*          INTO NETWORK WHEN DOOR
*          OPENED
*
*          ALSO AS ABOVE BUT 
*          CONDITIONAL & BIDIRECTIONAL
*          (DOOR MUST BE OPEN)
* 
*
*  FORMAT:
*              /NOUN# OF DOOR,        IE.  DC.W  NOUNNO
*              #DIR IN,#DIR OUT            DC.B  DIR_IN,DIR_OUT
*
*--------------------------------
 
 
  
DOORS
 
              DC.W    NDOOR

              DC.B    DIR_N,DIR_S 
              DC.W    NDOORS

              DC.B    DIR_S,DIR_N
              DC.W    NDOOR2

              DC.B    DIR_S,DIR_N
              DC.W    NTREEDOOR

              DC.B    DIR_E,DIR_W
              DC.W    NETREE

              DC.B    DIR_E,DIR_W    ;NOT A DOOR - USES TREE DOOR
              DC.W    NFRONTDOOR

              DC.B    DIR_W,DIR_E
              DC.W    NLIFTDOOR

              DC.B    DIR_N,DIR_S
              DC.W    NSHEDDOOR

              DC.B    DIR_SW,DIR_NE
              DC.W    NSHED

              DC.B    DIR_SW,DIR_NE   ;NOT A DOOR - USES SHEDDOOR
              DC.W    NDDOORS

              DC.B    DIR_E,DIR_W
              DC.L    0
        
       
       
*--------------------------------
*
*  TBL.DOOR    (TBL.DOORS)
*
*  FORMAT: /DOORN#,/KEYN#
*             ..     ..
*          /0
*
* REALLY TABLE OF THINGS THAT 
* CAN BE UNLOCKED
*
*--------------------------------
 
TBL.DOOR
 
*             DC.W    NDOOR,NKEY       ;NOW BLUE KEY WORKS FOR ANY DOOR ONCE!
              DC.W    NDOOR,-1         ;BUT MUST BE IN TABLE TO GET RIGHT MSG 
              DC.W    NSAFE,-1         ;DITTO SAFE
              DC.W    NSHEDDOOR,NMKEY    
              DC.W    NTREEDOOR,NWKEY
              DC.W    NDOORS,-1        ;DOORS WI NO KEY MUST BE HERE
              DC.W    NDDOORS,-1       ;SO THAT LOCK/UNLOCK R OK
              DC.W    NFRONTDOOR,-1    ;& GIVE RIGHT ERROR MSG
              DC.W    NDOOR2,-1
              DC.L    0
 
*--------------------------------
*
* SP.JVTBL      - TABLE OF MSG 
*              IN CASE OF DEATH
*              BY JUMPING
*
* FORMAT:      /ROOM#,/SCN.MSG#
*
*
*--------------------------------
 
  
SP.JVTBL
  
              DC.W    NSCLIFF,36
              DC.W    NNCLIFF,36
              DC.W    NBRIDGE,36 
              DC.W    NONBRIDGE,36 
 
              DC.W    NLANDING,90
              DC.W    NWALL2,89
    
              DC.W    NVSHAFT,91
              DC.W    NCAVERN4,91  
              DC.W    NRIVER,91
              DC.W    NROOM3,91 
 
              DC.W    NLEDGE,36
              DC.W    NBRIDGE3,36
              DC.W    NBRIDGE4,36
              DC.W    NLEDGE2,36 
              DC.W    NLEDGE3,144
              DC.W    NSHAFT2,144
              DC.W    NLEDGE4,144
 
JVTBLEND


* no END this is an INCLUDE'd file

