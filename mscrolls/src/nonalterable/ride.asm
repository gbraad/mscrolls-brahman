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
*	@(#)ride.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.RIDE/DISMOUNT
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"

      XDEF     P.FLOO

      XREF     ADVERB,W.IDFY,SEATED,SP.RIDE,P.BYAN,RIDING,VERB,GO
      XREF     P.SUB,W.YCNDTT,P.TN,W.ARE,W.ALRDY,W.CANTH,V.SIT,V.STAND
      XREF     LINE,W.GIVE,W.FLOOK,PRTTXT,V.RAPE,P.TNSTOP


	IfVerb	RIDE

	XDEF	V.RIDE
	
V.RIDE
	
	MOVE.W  ADVERB(A4),D1
	BMI.S   50$                 ;RIDE (DIRECTION)
10$
	TEST_W  D0                  ;NOUN SPEC?
	BMI.S	52$			;idfy
	TEST_W  SEATED(A4)
	BNE.S   70$                 ;ERROR
	CALL    SP.RIDE
	BTST    #6,4(A0)            ;NPC?
	BEQ.S   60$                 ;OK
	BTST    #1,(A0)             ;Dead NPC?
	BEQ     P.FLOO
	BRA	V.RAPE
50$
	CMP.W   #-1,D1
	BEQ.S   10$                 ;NOT REALLY
	TEST_W  SEATED(A4)
	BNE.S   51$                 ;OK
	BRA     P.BYAN              ;BUT YOU ARE NOT SEATED.
51$
	MOVE.W  SEATED(A4),RIDING(A4)
	MOVE.W  #VNGO,VERB(A4)
	CALL    GO
52$
	MSGGO	IDFY
	        
60$
	BTST    #3,5(A0)            ;SITTABLE?
	BNE.S   90$
	DO	P.SUB
	MSG	YCNDTT
	DOGO	P.TNSTOP
70$
	DO	P.SUB
	CMP.W   SEATED(A4),D0
	BNE.S   71$
	MSG	ARE                ;YOU ARE ALREADY
	MSGGO	ALRDY
71$
	MSGGO	CANTH   
90$
	MOVE.W  #ON,ADVERB(A4)      ;SET AS 'ON'
	BRA     V.SIT

	ENDC
  
*--------------------------------

	IfVerb	DISMOUNT

	XDEF	V.DISMOU
	
V.DISMOU
	
	TEST_W  D0                ;SPEC?
	BMI     V.STAND           ;NONE
	MOVE.W  SEATED(A4),D1
	BEQ.S   20$               ;NOT SEATED
	CMP.W   D0,D1             ;SITTING?
	BNE.S   10$               ;NO GOOD     
20$
	MOVEQ   #-1,D0
	BRA     V.STAND           ;OK
10$
	MSG	YOUMEAN
	MOVE.W  SEATED(A4),D0  
	DO	P.TNSTOP
	MSG    LINE
	BRA.S   20$               ;OK

	ENDC
	
*--------------------------------
	
P.FLOO
	
	DO	P.TN
	MSG	GIVE  
	DO	P.SUB
	MSGGO	FLOOK

*-------------------------------

      END

