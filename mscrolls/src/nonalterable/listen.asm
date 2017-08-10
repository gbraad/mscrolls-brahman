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
*	@(#)listen.asm	2.15
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.LISTEN/FIND/GRAB/RUB
*
*--------------------------------
   
	include	"equates.asm"
	include "macros.asm"
	include	"verbequ.asm"


	XREF    ADVERB,W.IDFY,P.SUB,W.CNTHEA,SP.LISTE,P.TN,ISARE,P.TNSTOP
	XREF    W.SILNT,W.ATMOMT,VERB,SCAN,W.FNOI,VALID,P.PRN,W.HERE
	XREF    P.SCN,W.CNTSEE,P.NQEN,GET_,V.TOUCH,W.BUT,W.NHAP,VALID.NP
	XREF	CHK4PT,CONJ,ISAREN,PRTTXT,BSFULLSTOP
	XREF	TBL.LISTEN,MessageFromTable

	IfVerb	LISTEN

	XDEF	V.LISTEN

V.LISTEN

	CALL	SP.LISTE

      IFEQ	THE_PAWN

	LEA     TBL.LISTEN(A4),A1	;LIST OF LISTENABLE (!?) ITEMS
	CALL	MessageFromTable
	BEQ.S	05$			;If we output nothing, then EQ
	RET
05$ 

      ENDC

	CMP.W   #TO,ADVERB(A4)		;Listen to object?
	BEQ.S   20$                 	
	CMP.W   #AT,ADVERB(A4)		;Listen at xyzzy?
	BEQ.S   10$
	MSGGO	IDFY
10$
	DO	P.SUB
	MSGGO	CNTHEA
20$
	BTST    #6,4(A0)
	BEQ.S   10$
	DO	P.TN
	DO	ISARE
	MSG	SILNT
	MSGGO	ATMOMT


	ENDC
  
*--------------------------------

	IfVerb	FIND

	XDEF	V.FIND
   
V.FIND
 
	CMP.W   #VNFIND,VERB(A4)
	BEQ.S   90$
	CMP.W   #FOR,ADVERB(A4)		;search for?
	BEQ.S   90$
	CALL    SCAN
	TEST_B  D7			;1= SOMETHING FOUND
	BNE.S   99$
	DO	P.SUB
	MSGGO	FNOI
90$
	
      IFD	VNMAKELOCAL

	XREF	V.MAKELOCAL

	BRA	V.MAKELOCAL
      
      ENDC

      IFND	VNMAKELOCAL

	DO	VALID.NP
	BNE.S   91$
	DO	P.PRN
	DO	ISARE
	MSGGO	HERE
91$
	DO	P.SUB
	MSG	CNTSEE
	CALL    P.NQEN
	DO	BSFULLSTOP

      ENDC
99$
	RET


	ENDC
 
*--------------------------------

	IfVerb	GRAB

	XDEF	V.GRAB
   
V.GRAB

	BTST    #6,4(A0)		;npc?
	BEQ     GET_			;no - do a get

	ENDC

	IfVerb	TOUCH

GRABorRUB

	MOVE.W  #VNTOUCH,VERB(A4)
	MOVE.W  #$8008,ADVERB(A4)	;up?
	BRA     V.TOUCH

	ENDC

 
*--------------------------------

	IfVerb	RUB

	XDEF	V.RUB
   
V.RUB
 
	BTST    #6,4(A0)		;npc?
	BNE.S   GRABorRUB

      IFNE	THE_PAWN	

	MSG	BUT
	MSGGO	NHAP

      ENDC

      IFEQ	THE_PAWN

	XREF	SP.RUB,SP.RUB2

* new all singing all dancing rub code

	CALL	SP.RUB			;simple special
	MOVE.W	CONJ(A4),D1		;get with/into/on etc
	BEQ.S	30$			;rub x with y?
	CMP.W	#ON,D1			;rub y on x
	BEQ.S	20$
	CMP.W	#INTO,D1		;rub y into x
	BEQ.S	20$
	MOVEQ	#0,D1			;with
	BTST	#4,3(A0)		;moveable
	BEQ.S	10$			;if moveable do 'On what?'
	MOVEQ	#ON,D1			;else do 'with what?'
10$
	DO	CHK4PT			; will not return!!!!!!!!!!!

* swap arguments for 'rub y on x' to 'rub x with y'

20$
	EXG	A0,A5
	EXG	D0,D5

* call special

30$
	CALL	SP.RUB2
	DO	P.TN		;failed - 'the x is not effected by the y'
	DO	ISAREN
	MSG	EFFECTEDBY	;<' effected by '>
	EXG	D0,D5
	DOGO	P.TNSTOP

      ENDC

      ENDC




      END
