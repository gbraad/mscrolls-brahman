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
*	@(#)data3.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.DATA3
*
*--------------------------------


       XDEF    TMAP.ADV,ERR1,ERR2,ERR3,ERR4,GLOBAL.F
       XDEF    REP1,SCAN.SUB,SCANVERB,SCAN.OBJ,SCANMIXN,IUFLAG
       XDEF    KYWRD,ISARETYP,NPC.LBF1,NPC.LBF2,NPC.TOBE,NPCQUEST
       XDEF    NPCMODEL,NPCABOUT,NPC.WRD,PERSON,TENSE
       XDEF    TBL.MOVE

	include	"nounequ1.asm"
	include	"nounequ2.asm"

       XREF    S_FBOARD,S_MAT,S_PEDEST

TBL.MOVE
 
       DC.W     NFLOORBOARD
       DC.L     S_FBOARD          ;DUE TO NEW ASSEMBLER...

       DC.W     NMAT
       DC.L     S_MAT             ;STUFF USING PC=* HAS TO GO ON

       DC.W     NPEDESTAL
       DC.L     S_PEDEST          ;A NEW LINE AS IT TAKES PC FROM START
       DC.L     0                 ;OF LINE, NOT ADDR OF EXPRESSION
       DC.W     0
 
       
*--------------------------------
*
* TIME MAPS
*
*
*
*
*  00 00 00 MEANS STOP        
*          
*  00 AB CD MEANS GOTO ABCD IN TABLE
*
*
*--------------------------------
*
* FORMAT :
*
*
* CURRLOC        DC.W    0   CURRENT ROOM#
*
* OFFSET         DC.W    0   OFFSET INTO
*                   TIME MAP TABLE
*
*
*    HEX   
*
  
TMAP.ADV              DC.W    0    ;CURR LOC
 
                      DC.W    0    ;OFSET
 
 
       DC.B       $5C,$5C,$5C,$5C,$5C,$16,$16,$16
       DC.B       $15,$15,$14,$14,$12,$12,$0F,$0F,$0C,$0C
       DC.B       $0D,$0D,$1C,$1C,$1C,$1C,$1C,$1F
       DC.B       $1F,$20,$20,$21,$21,$22,$22,$23
       DC.B       $23,$24,$24,$24,$24,$24,$24,$24
       DC.B       $24,$24,$24,$25,$25,$22,$22,$21
       DC.B       $21,$20,$20,$2B,$2B,$2B,$2B,$2B
       DC.B       $2C,$2C,$2D,$2D,$2E,$2E,$2E,$36
       DC.B       $36,$37,$37,$37,$37,$37,$36,$36
       DC.B       $2E,$2E,$32,$32,$33,$33,$33,$33
       DC.B       $32,$32,$2E,$2E,$2F,$2F,$34,$34
       DC.B       $34,$34,$31,$31,$31,$31,$31,$31
       DC.B       $34,$34,$2F,$2F,$2E,$2E,$2D,$2D
       DC.B       $2C,$2C,$2B,$2B,$20,$20,$21,$21
       DC.B       $22,$22,$23,$23,$24,$24,$24,$24
       DC.B       $24,$24,$24,$26,$26,$2A,$2A,$2A
       DC.B       $2A,$26,$26,$26,$26,$27,$27,$27
       DC.B       $27,$28,$28,$28,$28,$28,$28,$28
       DC.B       $28,$28,$27,$27,$27,$26,$26,$24
       DC.B       $24,$23,$23,$22,$22,$22,$22,$21
       DC.B       $21,$20,$20,$1F,$1F,$1C,$1C,$0D,$0D
       DC.B       $0C,$0C,$0F,$0F,$12,$12,$14,$14,$15
       DC.B       $15,$16,$16,$5C,$5C,$5C,$5C,$5C
       DC.B       $00,$00,$00,$00
        
       
       
*--------------------------------
*
* THIS IS A WHOLE BILE OF
* JUNK TAKEN OUT OF VARIOUS
* BITS OF CODE TO AVOID LOCAL
* DATA AREAS
*
*--------------------------------
  
*--------------------------------
*
*  NPC.SUB2 LOCAL BITS
*
* REALLY QUITE GLOBAL SINCE
* THESE ARE THE ERROR SUPRESSION
* FLAGS
*
* YUK!
*
*--------------------------------
 
ERR1                  DC.W    0
ERR2                  DC.W    0
ERR3                  DC.W    1
ERR4                  DC.W    0      ;FLAGS NO WHICH (ETC)  
GLOBAL.F              DC.W    0
 
*--------------------------------
*
*  NPC.SUB LOCAL BITS
*
*--------------------------------
  
REP1                  DC.W    0
SCAN.SUB              DC.W    0
SCANVERB              DC.W    0
SCAN.OBJ              DC.W    0
SCANMIXN              DC.W    0
IUFLAG                DC.W    0
KYWRD                 DC.W    0
ISARETYP              DC.W    0
       
*--------------------------------
*
*  PARSER.NPC LOCAL BITS
*
*--------------------------------
 
NPC.LBF1
       
*LOCAL BUFFER FOR NPC PARSE 
       
       DS.B     $20
       
NPC.LBF2
       
       DS.B     $10
       
   
*RESULT FORMAT:
       
NPC.TOBE              DC.W    -1
NPCQUEST              DC.W    -1
NPCMODEL              DC.W    -1
NPCABOUT              DC.W    0
NPC.WRD               DC.L    0
PERSON                DC.W    2
TENSE                 DC.W    0

* 0 = PRES
* 1 = PRES PART
       
       
       
*--------------------------------

      END

