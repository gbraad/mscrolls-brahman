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
*	@(#)buy.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.BUY
*
*--------------------------------
  
	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"


      XDEF     ONINV,RETURN

      XREF     CHK6PART,CARRIED,P.MHAV,P.TN,W.GIVE,W.FLOOK
      XREF     ISAREN,W.FSALE,ISARE,W.AYOURS,WHOOPS,W.NTENDE,PRTTXT
      XREF     W.STRANG,W.NDEAL,SETOUT,W.NOW,W.GOT,F_TRADE,FIXCONT,WCARR,ROOM
      XREF     W.CNTCAR,P.OBJPRN,W.SO,P.SUB,W.PUT,W.ONGND,SUBJECT,LINE
      XREF     IFLIQUID


	IfVerb	BUY

	XDEF	V.BUY

V.BUY
 
	MOVEQ   #-1,D7             ;FORCE CHK4PART
	MOVEQ   #FROM,D1           ;WITH A FROM B   
	CALL    CHK6PART           ;SUSS ADNOUN ETC
	EXG     D0,D5
	EXG     A0,A5
	DO	CARRIED	           ;MUST HAVE IT
	BNE     P.MHAV             ;TOO BAD
	EXG     D0,D5
	EXG     A0,A5

      IFEQ	THE_PAWN
	XREF	SP.BUY_Before
	CALL	SP.BUY_Before		;if anything done, won't come back
      ENDC

	CALL    IFLIQUID
	BTST    #6,4(A0)           ;ALIVE
	BEQ.S   11$  
	DO	P.TN
	MSG	GIVE               ;CANT BUY LIVE ITEMS! - FUNNY LOOK
	DO	P.SUB
	MSGGO	FLOOK
11$
      IFNE	THE_PAWN
	MOVE.B  4(A0),D1           ;GET VALUE BYTE
	AND.W   #$F,D1             ;BIT3 = MONEY, BITS 0-2 R VALUE
      ENDC

      IFEQ	THE_PAWN
	XREF	GetValue
	CALL	GetValue	;values object in D0, returns D1/Z flag
      ENDC

	BNE.S   10$
	DO	P.TN	              ;ZERO VALUE = NOT FOR SALE
	DO	ISAREN
	MSGGO	FSALE
10$
	DO	CARRIED	           ;CANNOT BUY IF U ALREADY HAVE IT!
	BNE.S   20$
	DO	P.TN
	DO	ISARE
	MSGGO	AYOURS
	
20$
      IFNE	THE_PAWN
	MOVE.B  4(A5),D1
	AND.W   #$0F,D1            ;LOSE JUNK BITS
	BCLR    #3,D1              ;USING MONEY?
      ENDC

      IFEQ	THE_PAWN
	EXG	D0,D5
	CALL	GetValue
	EXG	D0,D5
	BTST	#3,4(A5)		;test money bit
      ENDC

	BNE.S   30$
	DO	P.SUB
	ST      WHOOPS(A4)         ;TELL PROCESS TO GIV UP
	MSGGO	NTENDE           ;NOPE - USELESS!
30$
      IFNE	THE_PAWN
	MOVE.B  4(A0),D2
	AND.W   #$0F,D2
	BCLR    #3,D2
      ENDC

      IFEQ	THE_PAWN
	EXG	D1,D2			;cause result will be in D1
	CALL	GetValue
	EXG	D1,D2
	BTST	#3,4(A0)
      ENDC

	BEQ.S	4909$
	MSGGO	STRANG
4909$
	SUB.W   D2,D1              ;WORTH DOING?
	SMI     WHOOPS(A4)
	BPL.S	50$
	MSGGO	NDEAL            ;NOPE!
50$
      IFNE	THE_PAWN
	OR.B    #8,D1              ;SET TO BEW VALUABLE
	AND.B   #$F0,4(A5)
	OR.B    D1,4(A5)
	AND.W   #3,D1              ;ANY REMAINING VALUE?
      ENDC

      IFEQ	THE_PAWN
	XREF	SetValue
	CALL	SetValue	   ;set the value of obj D5 to D1.B
				   ;returns EQ => no value
      ENDC

	BNE.S   60$
	EXG     D0,D5              ;COS SETOUT WANTS D0!
	DO	SETOUT	            ;USED ALL VALUE OFCOIN = KNOBBLE IT
	EXG     D0,D5
60$
      
      IFEQ	THE_PAWN
	XREF	SP.BUY
	CALL	SP.BUY		;returns EQ => avoid 'you have now...'
	BEQ.S	ONINV
      ENDC

	CALL    PRTTXT
	DC.W    $8100
	MSG	NOW
	MSG	GOT
	DO	P.TN
	MSG    LINE
	ST      F_TRADE(A4)        ;SO SETMODE CAN SUSS IF TRADED IN ROOM
*	BRA.S   ONINV    		;fals through

	ENDC
	
*--------------------------------
   
ONINV
	DO	FIXCONT            ;WAS PROBABLY CONTAINED (IN CART)
	CLR.W   6(A0)
	MOVE.B  2(A0),D1 
	AND.B   #$0F,D1
	ADD.W   WCARR(A4),D1
	CMP.B   #MAXWGHT,D1
	BLS.S   10$
	MOVE.W  ROOM(A4),8(A0)  
	DO	P.SUB
	MSG	CNTCAR
	DO	P.OBJPRN
	MSG	SO
	DO	P.SUB
	MSG	PUT
	DO	P.OBJPRN
	MSGGO	ONGND
10$
	MOVE.W  SUBJECT(A4),8(A0)
 
RETURN
 
	RET

      END

