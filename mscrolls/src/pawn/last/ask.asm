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
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.ASK
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"macros.asm"

      XDEF     V.ASK,V.TELL,REPLY

      XREF     DBS,SP.ASK,VERB,NOUNBUF2,W.IDFY,CHKNPC,P.SAY2,WHOOPS,GETDATA
      XREF     COPOUT,P.SCN,SP.ASK2,P.TN,W.GLARE,P.SUBOB,RND.D1,SCREEN,EXTRA
      XREF	P.IDFY
 
V.ASK
 
	BTST    #6,4(A0)
	BEQ     DBS
	CALL    SP.ASK
	CMP.W   #VNTELL,VERB(A4)
	BEQ     V.TELL
	TEST_W  NOUNBUF2(A4)        ;INCASE NOTHING SPEC 
	BNE.S   10$
	ADDQ.L  #4,A7
	CLR.W   EXTRA(A4)
	CALL	P.IDFY
	BRA     SCREEN
 
10$
	MOVE.W  D0,D5
	MOVE.L  A0,A5
	CALL    CHKNPC
	CALL    P.SAY2
	LEA     NOUNBUF2(A4),A1
20$
	MOVE.W  (A1)+,D0            ;ITEM
	BEQ.S   30$                 ;DONE
	TEST_B  WHOOPS(A4)          ;ONLY ONE COPOUT PLEASE!
	BNE.S   30$
	GETINFO
	MOVE.L  A1,-(A7)
	CALL    ASKMAIN
	MOVE.L  (A7)+,A1
	BRA.S   20$
30$
	RET
  
ASKMAIN
 
	LEA     TABLE.ASK(PC),A1    ;GET ADDR. OF TABLE OF OFFSETS
10$
	TEST_W  (A1)
	BEQ.S   30$                 ;NOT IN TABLE
	CMP.W   (A1)+,D5            ;CORRECT NOUN#?
	BEQ.S   20$                 ;YES!
	ADDQ.L  #6,A1               ;NOPE
	BRA.S   10$                 ;NEXT PLEASE..
 
20$
	MOVE.L  (A1)+,D2            ;COPOUT LIST
	ADD.W   (A1),A1             ;REPLY LIST ADDR 
	BRA.S   REPLY
30$
	ST      WHOOPS(A4)          ;AVOID LOTS OF COPOUTS
	CLR.L   D2
	BRA     COPOUT
 
*--------------------------------
  
REPLY
 
	MOVE.L  (A1)+,D1
	BEQ.S   90$
	CMP.W   D1,D0
	BNE.S   REPLY
	SWAP    D1
	BRA     P.SCN
90$
	CALL    SP.ASK2             ;COUNT COPOUTS (DEV,PRINCESS)
	ST      WHOOPS(A4)          ;AVOID LOST OF COPOUTS
	BRA     COPOUT
 
*--------------------------------
*
* TABLE.ASK - INFO ON EACH NPC'S
*             REPLIES & COPOUTS
*
* FORMAT      NOUN NO. (WORD)
*             COPOUTS  (LONG WRD)
*             NPC.REP-* (WORD)   
* 
* ZERO WORD TERMS LIST
*
*--------------------------------
 
TABLE.ASK
 
	DC.W     NGURU
	DC.L     $10508
	DC.W     GURU.REP-*
 
	DC.W     NJOHN
	DC.L     $8C8D8E
	DC.W     JOHN.REP-*
 
	DC.W     NKRONOS
	DC.L     $21
	DC.W     KRON.REP-*
 
	DC.W     NPRINCESS
	DC.L     $8382
	DC.W     PRIN.REP-*
 
	DC.W     NGUARDS
	DC.L     $A6
	DC.W     GUARD.REP-*
 
	DC.W     NADVENT
	DC.L     $00
	DC.W     ADV.REP-*
	
	DC.W     NHORSE
	DC.L     $9DA0A2
	DC.W     HORSE.REP-*
 
	DC.W     NPORTER
	DC.L     $00
	DC.W     PORT.REP-*
 
	DC.W     NDEVIL
	DC.L     $00
	DC.W     DEVIL.REP-*
	
	DC.W     NALCH
	DC.L     $00
	DC.W     ALCH.REP-*
 
 
	DC.L     0
 
*--------------------------------
   
GURU.REP
 
	DC.W     6,NSNOWMAN
	DC.W     11,NADVENT
	DC.W     12,NMAGICIAN
	DC.W     13,NPRINCESS
	DC.W     14,NPALACE
	DC.W     7,NPOUCH
	DC.W     7,NBLUE
	DC.W     15,NJOHN
	DC.W     16,NHORSE
	DC.W     17,NTOWER
	DC.W     18,NDEVIL
	DC.W     20,NBAND
	DC.W     102,NERIK
	DC.W     301,NFERGS
	DC.W     287,NGRINGO
	DC.W     10,NHUT
	DC.W     188,NKEROVNIA
	DC.W     198,NLIFE
	DC.L     0

*--------------------------------

KRON.REP

*      DC.W     30,NBAND
	DC.W     31,NERIK
	DC.W     296,NFERGS
	DC.W     32,NEVIL
	DC.W     54,NPRINCESS
	DC.W     53,NTOWER
	DC.W     55,NHORSE
	DC.W     56,NADVENT
	DC.W     143,NGRINGO
	DC.L     0

*-------------------------------

PRIN.REP

	DC.W     103,NKRONOS
	DC.W     104,NADVENT
	DC.W     105,NHORSE
	DC.W     121,NROSE2
	DC.W     123,NTOWER
	DC.W     124,NGURU
	DC.W     126,NERIK
	DC.W     127,NSNOWMAN
	DC.W     128,NBELT
	DC.W     129,NPALACE
	DC.W     132,NGARDENS
	DC.W     300,NFERGS
	DC.L     0

*--------------------------------
   
JOHN.REP

	DC.W     133,NGURU
	DC.W     134,NKRONOS
	DC.W     135,NERIK
	DC.W     136,NPALACE
	DC.W     137,NPRINCESS
	DC.W     138,NADVENT
	DC.W     139,NHORSE
	DC.W     295,NFERGS
	DC.W     295,NCOIN
	DC.W     298,NGRINGO
	DC.W     46,NARMOUR2      ;HJ ARMOUR
	DC.W     46,NARMOUR       ;SO 'ASK ABOUT ARMOUR' IS OK
	DC.W     47,NBOTTLE2      ;WHISKY BOTTLE
	DC.W     48,NBOTTLE4      ;ALE BOTTLE
	DC.W     49,NBOTTLE3      ;WATER BOTTLE
	DC.W     50,NRATIONS      ;RATIONS
	DC.L     0

*--------------------------------
   
   
GUARD.REP

	DC.W     145,NERIK
	DC.W     146,NPALACE
	DC.W     147,NGARDENS
	DC.W     148,NPRINCESS
	DC.W     149,NADVENT
	DC.W     299,NFERGS
	DC.W     150,NHORSE
	DC.W     151,NJOHN
	DC.W     298,NGRINGO
	DC.L     0

*--------------------------------
   
ADV.REP

	DC.W     152,NERIK
	DC.W     153,NPALACE
	DC.W     154,NPRINCESS
	DC.W     155,NHORSE
	DC.W     184,NTOWER
	DC.W     298,NGRINGO
	DC.L     0

*--------------------------------
    
HORSE.REP
 
	DC.W     158,NADVENT
	DC.W     161,NPONCHO
	DC.W     163,NPALACE
	DC.W     164,NCHEROOT
	DC.W     164,NSTETSON
	DC.W     156,NMAGNUM
	DC.W     143,NGRINGO
	DC.L     0

*--------------------------------
 
PORT.REP
 
	DC.W     238,NDEVIL
	DC.W     239,NHELL
	DC.W     242,NBOTTLE2
	DC.W     242,NBOTTLE4      ;BEER
	DC.W     242,NWHISKY
	DC.W     243,NSHAFT2
	DC.W     241,NEVIL
	DC.W     297,NFERGS
	DC.L     0
 
*--------------------------------
 
DEVIL.REP
 
	DC.W     260,NJOHN
	DC.W     261,NSNOWMAN
	DC.W     262,NALCH
	DC.W     263,NDEMONS
	DC.W     266,NDRAGON
	DC.W     267,NKRONOS
	DC.W     268,NGRINGO
	DC.W     269,NTOWER
	DC.W     270,NGOOD
	DC.W     271,NEVIL
	DC.W     272,NHELL
	DC.W     273,NPALACE
	DC.W     274,NKEROVNIA
	DC.W     275,NLAVA
	DC.W     276,NGNOMES
	DC.W     251,NPORTER
	DC.W     252,NERIK
	DC.W     253,NPRINCESS
	DC.W     254,NADVENT
	DC.W     255,NGURU
    
	DC.L     0

*--------------------------------
 
ALCH.REP
 
	DC.W     245,NGOLD
	DC.W     246,NALCHEMY
	DC.W     244,NLEAD
	DC.W     247,NKRONOS
	DC.W     248,NHORSE
	DC.W     249,NFLASKS
	DC.W     249,NLIMBECKS
	DC.W     250,NBOTTLE1
 
	DC.L     0

*--------------------------------
	
	
V.TELL
	
	MOVE.W  D0,D5
	MOVE.L  A0,A5
	CALL    CHKNPC
	CALL    P.TN
	BTST    #0,(A0)            ;PL
	BNE.S   50$
	MOVEQ   #2,D1
	CALL    RND.D1
	ADD.W   #303,D1
	BRA     P.SCN
	
50$
	MSG	GLARE
	BRA     P.SUBOB
 
*--------------------------------

      END

