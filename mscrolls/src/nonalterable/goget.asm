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
*	@(#)goget.asm	2.12
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
* 
*  SRC.ADV.GO/GET/DROP
*
*--------------------------------
  
	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"



      XREF     SP.GO.N,V.SIT,SETMODE,P.SUB,WHOOPS,W.CGTW,PRTTXT
      XREF     ADVERB,W.IDFY,CONJ,REMOVE,DBS,SUBJECT
      XREF     FIXCHK,P.SCN,P.TN,W.STEADF,WCARR,P.BTN,ISAREN,W.OPN,CALCWHT
      XREF     W.DBR,W.CNTGT,P.NQEN,W.CTMA,GETNUM,W.NOW,W.GOT,SP.MOVE.
      XREF     SCANSV,FIXCONT,SUBJECT2,P.NOUN,W.TAKEN,ISARE,W.THEVY
      XREF     LINE,W.TTR,FIXSYS,SETA1,GETDATA,W.THEMAL,P.SUBV,GETRELN
      XREF     P.WITH,W.NTCAR,W.ASCONT,V.EXIT,V.PUT,CARRIED,NMOVEFLG,W.DROPD
      XREF     W.ALCAR,SEATED,IGNORAJ,VERB,SP.DROP,P.SEPA,BRKD0
      XREF     W.BUT,DO.FIX,MOVE.TAB,DOORS,PSUDAT,ROOM,CALCNET,MLISTNGO

      XREF	FULLSTOP,BSFULLSTOP,P.TNSTOP,SP.SCORE.NOUN,LastDirection


WantGoLeave	SET	FALSE

      IfVerb	GO		;hmmmm will this every be missing?
WantGoLeave	SET	TRUE

	XDEF	GO.N

GO.N
  
* HANDLE 'GO NOUN' (ALSO 'ENTER NOUN' ETC)
  
	CALL    SP.GO.N              ;TEST FOR SHED
	BTST    #3,5(A0)             ;SITTABLE?
	BNE     V.SIT                ;NO
	
	BTST    #6,6(A0)             ;ROOM?
	BEQ.S   10$                  ;NO  
	MOVE.W  8(A0),D1
	MOVE.W	#-1,LastDirection(A4)	;'cos we went 'go xyzzy' not 'go dir
	BRA     SETMODE
10$
	CALL    GO.LEAVE            ;RETURNS IF CANNOT GO NOUN
	DO	P.SUB	              ;'YOU CAN'T GO THAT WAY'
	ST      WHOOPS(A4)          ;AVOID MULTIPLE MSGS
	MSGGO	CGTW


	ENDC
 
*--------------------------------
*
*  GET ROUTINE
* 
*--------------------------------

	IfVerb	GET

	XDEF	GET_
	XREF	SP.GET
 
GET_
 
	CALL    SP.GET               ;CATCH 'G SNOW'

	MOVE.W  ADVERB(A4),D1
	CMP.W   #IN,D1                ;IN
	BEQ     GO.N
	TEST_W  D0                   ;ANY?
	BPL.S	01$
	MSGGO	IDFY
01$
	CMP.W   #FROM,CONJ(A4)          ;FROM? 
	BNE.S   02$                  ;DO REMOVE
	MOVEQ   #0,D1
	BRA     REMOVE
02$
	BTST    #0,5(A0)
	BNE     GT16
	BTST    #6,6(A0)		;A ROOM?
	BNE     GT16
	BTST    #3,6(A0)            ;INTRINSIC?
	BNE     DBS
	MOVE.W  8(A0),D1             ;LOC
	CMP.W   SUBJECT(A4),D1       ;CARRIED?
	BEQ     P.ALCARRY
10$
	DO	FIXCHK
	BNE     GETMANY
	BTST    #4,3(A0)             ;MOVEABLE?
	BEQ.S   05$                  ;NO
	BTST    #4,6(A0)             ;INSEPARABLE?
	BEQ.S   06$                  ;NO
	DOGO	DBS
05$
	DO	P.TN	              ;NOPE CAN'T GET IT
	MSGGO	STEADF
06$
	MOVE.B  1(A0),D1
	LSR.B   #4,D1
	CMP.B   #1,D1                ;HARDNESS
	BEQ     GETWTH               ;GET WITH

      IFEQ	THE_PAWN

	MOVE.B	2(A0),D1
	AND.B	#$0F,D1			;get weight 15=> no can get
	CMP.B	#15,D1
	BEQ	TOOHVY

       ENDC

	MOVE.W  D0,D2
11$
	MOVE.B  6(A0),D1
	AND.B   #$21,D1
	BEQ.S   12$
	MOVE.W  8(A0),D0
	GETINFO
	BRA.S   11$
12$
	MOVE.W  8(A0),D1
	CMP.W   SUBJECT(A4),D1
	BNE.S   GT13
	MOVE.W  D2,D0
	MOVE.L  A2,A0
	MOVE.W  WCARR(A4),D2
	BRA     GT30
GT13
	BTST    #5,6(A0)             ;CONTAINED?
	BEQ.S   GT15
	MOVE.W  8(A0),D0             ;CONTAINER
	GETINFO        
	MOVE.B  (A0),D1
	MOVE.L  A2,A0                ;BACK
	AND.B   #$18,D1              ;BITS 3&4 ONLY
	BNE.S   GT15                 ;OK
	DO	P.BTN
	DO	ISAREN
	MSGGO	OPN
GT15
	MOVE.W  D2,D0
	CALL    CALCWHT              ;TO D3
	CMP.B   #MAXWGHT,D3          ;TOO HEAVY?
	BHI     TOOHVY               ;BRANCH = YES
	TEST_B  D3                   ;ZERO ?
	BNE.S   GT20                 ;VOID
GT16
	MSG	DBR              ;DONT BE SILLY MSG
	DO	P.SUB	             ;FOR ITEMS WI ZERO WEIGHT
	MSG	CNTGT
	CALL    P.NQEN
	DOGO	BSFULLSTOP
GT20
	MOVE.W  WCARR(A4),D2         ;WEIGHT CARRIED 
	ADD.B   D3,D2                ;SUM
	CMP.B   #MAXWGHT,D2              ;LIMIT
	BLS     GT30
	DO	P.SUBV
	MSG	CTMA
	DOGO	P.TNSTOP
GT30
	MOVE.W  GETNUM(A4),D1
	BEQ.S   GT31
	CALL_S  TAKEN                 ;DO MANY
	BRA.S   LBL32
GT31
	CALL    PRTTXT     
	DC.W    $8100			;p.sub has/have
	MSG	NOW
	MSG	GOT

      IFNE	THE_PAWN

	DO	P.TN

      ENDC

      IFEQ	THE_PAWN

	DO	P.TNSTOP
      
      ENDC

	CALL    SP.MOVE.          ;ANYTHING REVIELED  ?
	CALL    SCANSV
LBL32
	MOVE.W  D2,WCARR(A4)         ;SUPERCEEDED BY EVALWHT
	DO	FIXCONT              ;FIX CONTAINER VOL IF NEC.
	GETINFO
	MOVE.W  SUBJECT2(A4),6(A0)   ;ENTIRE LOCATION INFO
	MOVE.W  SUBJECT(A4),8(A0)
	BCLR    #0,11(A2)            ;IF WORN
	ADDQ    #1,GETNUM(A4)

      IFEQ	THE_PAWN

	CALL	SP.SCORE.NOUN
      
      ENDC

	MSGGO     LINE
TAKEN
	DO	P.NOUN
	MSG	TAKEN
	CALL    SP.MOVE.          ;CATCHES 'G MAT' ETC

      IFEQ	THE_PAWN

	CALL	SP.SCORE.NOUN
      
      ENDC

	BRA     SCANSV
TOOHVY   
	DO	P.TN
	GETINFO        
	DO	ISARE
 
TOOHVY1.5
 
	MSG	THEVY
	MSGGO     LINE
  
TOOHVY2
 
	MSG	TTR              ;TOGETHER THEY ARE
	BRA.S   TOOHVY1.5            ;FOR THE SAKE OF 2 BYTES
  
*--------------------------------
 
GETMANY
  
	MOVEQ   #0,D5                ;FLG=DO NOT PRINT
  
	DO	FIXSYS
	CLR.W   D2
	CALL    SETA1
	MOVE.L  A1,A2
51$
	MOVE.W  (A1)+,D0
	BEQ.S   52$
	GETINFO        
	BTST    #4,3(A0)             ;MOVEABLE?
	BEQ.S   51$                  ;NOPE, GET NEXT ONE
	MOVE.B  2(A0),D1  
	AND.B   #$0F,D1              ;GET WIEGHT  
	BEQ     DBS                  ;ZERO TUT TUT!  
      
      IFEQ	THE_PAWN

	CMP.W	#15,D1
	BEQ	TOOHVY
    
      ENDC

	CALL    CALCWHT
	ADD.W   D3,D2
	BRA.S   51$
52$
	CMP.W   #MAXWGHT,D2
	BHI.S   TOOHVY2
	ADD.W   WCARR(A4),D2
	CMP.W   #MAXWGHT,D2
	BLS.S   53$
	DO	P.SUB
	MSG	CTMA
	MSG	THEMAL
	BRA	BSFULLSTOP
53$
	MOVE.W  (A2)+,D0
	BEQ.S   90$
	GETINFO        
	BTST    #4,3(A0)               ;MOVEABLE?
	BEQ.S   53$                    ;NO
	DO	FIXCONT			;in case it's contained
	CLR.W   6(A0)
	MOVE.W  SUBJECT(A4),8(A0)
	CALL    TAKEN
	MSG    LINE
	BRA.S   53$
90$
	RET


	ENDC

*--------------------------------
	
P.ALCARRY
	
	MSG    LINE
	MSG	BUT
	DO	P.SUBV
	MSG	ALCAR
	DOGO	P.TNSTOP
	
*--------------------------------

	IfVerb	GET
	
GETWTH                         
	
	DO	CARRIED	             ;ALREADY?
	BEQ.S   P.ALCARRY
01$
	TEST_W  D5                   ;WITH SOMETHING?
	BEQ.S   10$                  ;NO
	TEST_W  CONJ(A4)
	BEQ.S	02$
	MSGGO	IDFY
02$
	MOVE.W  #5,CONJ(A4)          ;CHANG TO 'IN'
	BRA     V.PUT                ;DO AS PUT X IN Y
10$
	PUSH_L  D0/A0
	MOVE.W  SUBJECT(A4),D2
	DO	GETRELN	             ;REL TO ME
20$
	MOVE.W  (A1)+,D0
	BEQ.S   30$                  ;DONE
	GETINFO                      ;TO A1
	BTST    #0,5(A0)
	BNE.S   20$                  ;NOT PSUDOS
	MOVE.W  SUBJECT2(A4),D1
	CMP.W   6(A0),D1             ;MUST BE RELEVENT REALTION
	BNE.S   20$                  ;NO GOOD
	MOVE.B  10(A0),D1            ;CONTAINER VOL
	AND.B   #$7,D1               ;VOL
	CMP.B   #$2,D1
	BLT.S   20$                  ;NO-GO
	MOVE.W  D0,D2                ;FOR FROM ROUTINE
	CALL    P.WITH               ;WITH TN
	MOVE.L  A0,A5
	PULL_L  D0/A0    
	MOVE.W  D2,D5
	BRA.S   02$
30$
	                          
	PULL_L  D0/A0
	DO	P.SUBV
	MSG	NTCAR
    
	MSG	ASCONT           ;A SUITABLE CONTAINER
	DOGO	FULLSTOP


	ENDC
	
*--------------------------------
	
WantDrop	SET	FALSE
	
	IfVerb	LEAVE
WantGoLeave	SET	TRUE
WantDrop	SET	TRUE

	XDEF	LEAVE

 
LEAVE

      IFEQ	THE_PAWN

	XREF	SP.LEAVE
	CALL	SP.LEAVE

      ENDC
 
	BTST    #6,6(A0)		;ROOM?
	BNE     V.EXIT
	CALL    GO.LEAVE		;DO A GO NOUN IF POSSIBLE
*					;TO ALLOW LEAVE SHED

	ENDC

*
* WARNING!!!!! beware of the fall through!
*

	IfVerb	DROP
WantDrop	SET	TRUE
	ENDC

	IFNE	WantDrop

	XDEF	DROP,DRP.PNT2
   
DROP
	TEST_W  CONJ(A4)             ;DROP IN?
	BPL     V.PUT                ;DO AS PUT
	BTST	#4,6(A0)		;inseperable?
	BNE.S	05$
	BTST    #4,3(A0)             ;MOVEABLE?
	BNE.S   07$
05$
	DO	P.TN
	MSGGO	STEADF
07$
	DO	CARRIED	             ;CARRIED?
	BEQ.S   10$                  ;YES
	MSG	BUT
	DO	P.SUBV
	MSG	NTCAR
	DOGO	P.TNSTOP
10$
	DO	FIXCHK
	BNE     DRP.MANY   
	CLR.W   NMOVEFLG(A4)         ;JUST IN CASE...
 
DRP.PNT
 
DROP.DO
 
	DO	P.NOUN
	BTST    #3,6(A0)             ;INTRINSIC?      
	BNE     DBS
	MSG	DROPD
	MSG	LINE
  
DRP.PNT2
  
	DO	FIXCONT
	MOVE.W  SEATED(A4),8(A0)     ;INCASE
	BEQ.S   10$                  ;NOT REALLY
	TEST_W  SUBJECT(A4)
	BNE.S   10$
	MOVE.B  #1,6(A0)             ;SET AS RELATED ONLY
	BRA.S   12$                  ;OK
10$
	CLR.W   6(A0)                ;NO HB
	MOVE.W  ROOM(A4),8(A0)       ;ON FLOOR
12$
	CALL    SP.DROP

      IFNE	THE_PAWN

	CMP.W   #VNDROP,VERB(A4)     ;MUST ME DROP TO BREAK
	BNE.S   14$    
	CMP.W   #1,IGNORAJ(A4)       ;DROP CAREFULLY?
	BEQ.S   14$ 
	MOVE.B  1(A0),D1             ;H&S
	LSR.B   #4,D1                ;H
	CMP.B   #1,D1                ;IS A LIQUID?
	BNE.S   13$
	BRA     P.SEPA               ;AND DO IT
13$
	CMP.B   #10,D1               ;BRITTLE?
	BEQ     BRKD0                ;BREAK D0

      ENDC
14$
	RET
 
DRP.MANY
  
	LEA     DRP.PNT(PC),A6
	BRA     DO.FIX


	ENDC
  
*--------------------------------
*
* GO.LEAVE   (GO.LEAVE.SUB)
*
*      DOES A GO NOUN IF IT
*      IS A DOOR OR A VECTORED
*      NOUN (EG SHED)
*
*  ELSE IT RETURNS
*
*--------------------------------


	IFNE	WantGoLeave

	XDEF	GO.LEAVE
 
GO.LEAVE
 
	LEA     MOVE.TAB-2(A4),A1    ;GET 'TELEPORT' TYPE NOUNS
10$
	ADDQ.L  #2,A1
	MOVE.W  (A1)+,D1             ;GET A NON FROM LIST
	BEQ.S   60$                  ;END OF LIST?
	CMP.W   D0,D1                ;IS IT THIS NOUN?
	BNE.S   10$                  ;NOPE - GET ANOTHER
	MOVE.B  1(A1),D1             ;GET TARGET ROOM#
20$
	MOVE.L  (A7)+,A1             ;POP STACK
	BRA     SETMODE              ;SEE IF WE CAN GO THERE
60$
	LEA     DOORS(A4),A1         ;GET DOORS LIST (REALY PSEUDOS)
65$
	CMP.W   (A1)+,D0             ;IS IT THIS ONE IN LIST?
	BEQ.S   70$                  ;YES - SUSS WHERE OTHER SIDE IS
	TEST_W  (A1)+                ;END OF LIST?
	BNE.S   65$                  ;NO - TEST NEXT
67$
	RET                          ;NOT A DOOR SO CANNOT GO THERE - RET
70$
	LEA     PSUDAT(A4),A2
	ADD.W   8(A0),A2             ;GET POINTER TO PSEUDO DATA
	CLR.L   D1
	MOVE.W  ROOM(A4),D0          ;GET CURRENT ROOM
	CMP.B   (A2),D0              ;WHICH SIDE OF DOOR?
	BEQ.S   75$                  ;OK WE'RE AT SIDE A
	MOVE.B  1(A1),D1             ;GET DIR B-A
	BRA.S   80$
75$
	MOVE.B  (A1),D1              ;GET DIR A-B
80$
	DO	CALCNET              ;A0 POINTS TO NETWORK
	MOVE.B  0(A0,D1),D1          ;GET ROOM NO.
	BEQ.S   67$                  ;CAN'T GO THAT WAY
	BPL.S   20$                  ;OK NEW ROOM..
	NEG.B   D1                   ;SORRY DOOR CLOSED..
	AND.L   #$FF,D1
	LSL.W   #1,D1
	LEA     MLISTNGO(A4),A1
	ADD.L   D1,A1
	MOVE.W  (A1),D1


	POPSP	                ;POP stack - don't return if it was a
				;closed door.
	DOGO	P.SCN

	ENDC


      END

