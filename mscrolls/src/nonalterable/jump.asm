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
*	@(#)jump.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.JUMP/VAULT
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"
        include "msgequref.i"


      XREF     W.BUT,W.GOT,P.TN,ADVERB,SEATED,P.SUB,RIDING,W.LEAP,W.IDFY
      XREF     VERB,CONJ,CHK4PT,DBS,W.YSI,P.MSG,LINE,ISARE,W.2BIG,GETDATA
      XREF     P.BTN,W.INW,BIZARE,W.SNPS,BRKD0,SP.JVTBL,JVTBLEND,W.FUN
      XREF     P.SCN,DEAD,R.NOUN,PRTTXT,GO.N,P.TNSTOP

WantJumpOFF	SET	FALSE

	IfVerb	JUMP
WantJumpOFF	SET	TRUE

	XDEF	V.JUMP

V.JUMP
 
      IFEQ	THE_PAWN
	
	XREF	SP.JUMP

	CALL	SP.JUMP
	CMP.W	#ON,ADVERB(A4)		;jump on = go
	BEQ.S	10$			;GO.N
	CMP.W	#ONTO,ADVERB(A4)	;ditto jump onto
	BEQ.S	10$
	CMP.W	#TO,ADVERB(A4)
	BEQ.S	10$
	CMP.W	#INTO,ADVERB(A4)
	BEQ.S	10$
	CMP.W	#OFF,ADVERB(A4)
	BNE.S	V.VAULT

10$
	BRA	GO.N			;which can deal with specials
					;based on MOVE.TAB etc.

      ENDC


	IFND	VNVAULT

V.VAULT
	MSGGO	IDFY
    
	ENDC

	ENDC

	IfVerb	VAULT
WantJumpOFF	SET	TRUE

	XDEF	V.VAULT 


V.VAULT

	TEST_W  D0                 ;ZERO= NO NOUN
	BEQ     V.JUMPOF
 
	TEST_W  8(A0)
	BNE.S   05$
	BTST    #0,5(A0)
	BNE.S   05$
	MSG	BUT
	CALL    PRTTXT
	DC.W    $8100               ;YOU HAVE
	MSG	GOT
	DOGO	P.TN
 
05$
	TEST_W  ADVERB(A4)
	BMI.S   10$
	CMP.W   #8,ADVERB(A4)      ;8='OVER'
	BEQ.S   10$
	CMP.W   #10,ADVERB(A4)     ;10='ACROSS'
	BEQ.S   10$
	TEST_W  SEATED(A4)         ;WHEN SITTING?
	BEQ.S   11$                ;NOPE
	DO	P.SUB
	CLR.W   SEATED(A4)
	CLR.W   RIDING(A4)
	MSGGO	LEAP
11$
	BTST	#6,6(A0)	;room?
	BNE	V.JUMPOF
	MSGGO	IDFY
10$
	MOVEQ   #M.JMP,D6
	MOVE.W  VERB(A4),D1
	CMP.W   #VNJUMP,D1         ;JUMP?
	BEQ.S   30$
	TEST_W  CONJ(A4)
	BMI.S   20$
	CLR.W   D1                 ;0=WITH
	DO	CHK4PT	            ;WITH WHAT EVER
	MOVE.B  2(A5),D1           ;GET SIZE
	LSR.B   #4,D1
	CMP.B   #5,D1
	BMI     DBS                ;TOO SMALL TO USE FOR VAULTING
	MOVE.B  1(A5),D1           ;GET HARDNESS OF ITEM TO BE USED
	LSR.B   #4,D1
	CMP.B   #5,D1
	BMI     DBS
	CMP.B   #12,D1             ;FLEXIBLE
	BEQ     DBS
	CMP.B   #10,D1
	BEQ     JV.SNAPS
	MOVE.B  1(A5),D1           ;GET STRENGTH
	AND.B   #$0F,D1
	CMP.B   #5,D1
	BMI.S   JV.SNAPS
20$
	MOVEQ   #M.VLT,D6        ;'VAULTING ' FOR LATER USE
  
*--------------------------------
  
  
30$
  
	TEST_B  6(A0)              ;CANNOT VAULT IF HIDDEN/RELATED/WORN ETC
	BNE.S   L50
L35
	MOVE.B  2(A0),D1           ;SIZE &WEIGHT
	LSR.B   #4,D1              ;GET SIZE ONLY
	BEQ     DBS                ;CANNOT JUMP OVER ROOMS ETC
	CMP.B   #5,D1
	BPL.S   40$                ;TOO BIG
 
	DO	P.SUB	             ;YOU/HE
	MSG	YSI
	MOVE.W  D6,D1              ;VAULT OR JUMP
	DO	P.MSG
	DO	P.TNSTOP
	MSGGO     LINE
 
 
40$
	
	DO	P.TN
	DO	ISARE
	MSGGO	2BIG           ;TOO BIG
  
  
*--------------------------------
  
L50
	BTST    #0,6(A0)           ;RELATED ?
	BEQ.S   VAULT.60
	MOVE.W  D0,D1
	MOVE.W  8(A0),D0
	GETINFO
	BTST    #6,6(A0)          ;ROOM?
	BEQ.S   L55
	MOVE.W  D1,D0
	GETINFO
	BRA.S   L35
L55
	DO	P.BTN
	DO	ISARE
	MSGGO	INW            ;IF RELATED THEN ?'BUT XYZ IS IN THE WAY'  
    
VAULT.60

	BTST    #6,6(A0)           ;ROOM
	BNE.S   V.JUMPOF
	BRA     BIZARE

	ENDC


	XDEF	JV.SNAPS		;why?
    
JV.SNAPS
 
	MOVE.W  D5,D0
	DO	P.TN
	MSG	SNPS           ;SNAPS
	BRA     BRKD0


	IFNE	WantJumpOFF

	XDEF	V.JUMPOF
 
 
V.JUMPOF
 
	DO	R.NOUN
	LEA     SP.JVTBL(A4),A0
10$
	CMP.W   (A0),D0
	BEQ.S   20$
	ADDQ.L  #4,A0
	LEA     JVTBLEND(A4),A1
	CMP.L   A0,A1
	BNE.S   10$
	MSGGO	FUN
  
20$
	MOVE.W  2(A0),D1
	DO	P.SCN
	DOGO	DEAD

	ENDC
 
      END
