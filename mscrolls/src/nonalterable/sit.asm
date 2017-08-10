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
*	@(#)sit.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.SIT
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"


      XREF     VERB,ADVERB,W.IDFY,SP.RIDE,GETDATA,SEATED,P.SUBV,W.NSO,P.TN
      XREF     P.SUB,W.CSOT,W.CSJA,FNDSEAT,W.NOLIE,W.SITUP,W.ASO,PRTTXT
      XREF	CARRIED,DBS,BSFULLSTOP,P.TNSTOP

	IfVerb	SIT

	XDEF	V.SIT

V.SIT
	
	MOVE.W  #VNSIT,VERB(A4)      ;ENFORCE
	MOVE.W  ADVERB(A4),D1        ;ANY? 
	
	TEST_W  D0                   ;NOUN?
	BMI     VSIT2                ;NO NOUN SPECIFIED
	CMP.W   #-1,D1
	BEQ.S   40$                  ;OK
	EXT.W   D1
	BMI.S   30$                  ;UP/DOWN  
	CMP.W   #ON,D1                ;ON
	BEQ     40$                  ;OK
	CMP.W   #AT,D1               ;AT?
	BEQ     40$                  ;OK
30$
	MSGGO	IDFY
40$
	CALL    SP.RIDE
	CALL    GETDATA              ;ON D0
	BTST    #3,5(A0)             ;SITTABLE?
	BEQ.S   41$                  ;NO
	TEST_W  SEATED(A4)           ;ALREADY?
	BNE     SIT18                ;ALREADY
	DO	CARRIED		     ;cant sit on things that are carried!
	BEQ     DBS		     ;may prefer a special message

	IFEQ	THE_PAWN

	XREF	SP.SIT
	CALL	SP.SIT		     ;Specials for sitting - popsp to abort

	ENDC

	MOVE.W  D0,SEATED(A4)        ;SITTING
	DO	P.SUBV
	MSG	NSO

      IFEQ	THE_PAWN

	BTST	#0,(A0)			;plural?
	BEQ.S	10$
	PRINT	<'one of '>
10$
      
	ENDC

	DOGO	P.TNSTOP
41$
	DO	P.SUB
	MSGGO	CSOT
	
VSIT2
	CMP.W   #-1,D1              ;ANY?
	BNE.S   11$                 ;YES
	DO	P.SUB
	MSGGO	CSJA
11$
	EXT.W   D1
	BMI.S   12$                 ;UP/DOWN
17$
	CALL    FNDSEAT
	MOVE.W  #3,ADVERB(A4)       ;SET TO 'ON'
	TEST_W  D7                  ;FOUND?
	BEQ     V.SIT               ;YES, CONTINUE
	RET
12$
	CMP.W   #DIR_U,D1               ;UP
	BNE.S   13$
	TEST_W  SEATED+2(A4)        ;LIED DOWN?
	BNE.S   14$                 ;YES
	DO	P.SUBV
	MSGGO	NOLIE
14$
	DO	P.SUB
	MSG	SITUP
	CLR.W   SEATED+2(A4)
	RET
13$
	CMP.W   #10,D1			;?????????????????????????????
	BNE.S   SIT16               ;BAD?
	TEST_W  SEATED(A4)
	BEQ.S   17$                 ;NOPE
SIT18
	DO	P.SUBV
	MSG	ASO
	MOVE.W  SEATED(A4),D0
	GETINFO
	DO	P.TN
	DOGO	BSFULLSTOP
SIT16
	PRINT  <'(HELP)'>
SIT90
	RET

	ENDC

*--------------------------------

      END

