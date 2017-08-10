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
*	@(#)parsnpc2.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PARSER.NPC2
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"

      XDEF     DO.CONJ2,NPC.WSYN,NOTWSYN2,CONJ11.2,PARSEIT,DOHOW,FLUSHVAR
      XDEF     NESTSPK,ONECOP,HOW.VAL

      XREF     WSYN2,PFAIL,CONJPNTR,GETCONJ,VERB,VERB2,NOUNBUF2,NOUNBUF3
      XREF     ADDNOUN,PEND,DO.THEN2,EXTRAPNT,EXTRA,PEND2,ERR4,NPC.WRD
      XREF     PARSEFOU,PARSETHR,DEC.N2,NPCABOUT,MNPH,NPC.LBF1,TEMPBUF
      XREF     PARSEONE,NPC.TOBE,NPCMODEL,NPC.LBF2,PERSON,P.SAY,W.TRICKQ
      XREF     DO.THEN,GETWRD,CONJ,PRTTXT,P.MSG,RND.D1

DO.CONJ2
 
	TEST_B  D3
	BNE.S   NOTWSYN2

NPC.WSYN

	CALL    WSYN2
	BRA     PFAIL
 
NOTWSYN2
 
	MOVE.L  CONJPNTR(A4),A0
	MOVE.W  D5,(A0)
	ADDQ.L  #2,CONJPNTR(A4)
 
CONJ11.2
 
	CALL    GETCONJ
	CMP.B   #1,D7
	BEQ.S   20$
	BHI     PFAIL 
	CMP.B   #SECT.VERB,D6       ;VERB?
	BNE.S   10$
	CMP.W   #VNUSE,VERB(A4)     ;USE?   
	BNE.S   10$
	MOVE.W  D5,VERB2(A4)        ;ALSO
	BRA     CONJ11.2
10$
	CMP.W   #SECT.CONJ,D6       ;CONJ?
	BEQ.S   NOTWSYN2
	
	CMP.W   #SECT.VERB,D6       ;VERB??
	BNE     NPC.WSYN  
  
	MOVE.W  NOUNBUF2(A4),ADDNOUN(A4)
	MOVE.W  NOUNBUF3(A4),ADDNOUN+2(A4)
 
	BRA     DO.THEN2
 
20$
	MOVE.W  NOUNBUF2(A4),ADDNOUN(A4)
	MOVE.W  NOUNBUF3(A4),ADDNOUN+2(A4)
	TEST_W  PEND(A4)
	BEQ     DO.THEN
	MOVE.L  A6,EXTRAPNT(A4)
	MOVE.W  #1,EXTRA(A4)
	MOVE.L  CONJPNTR(A4),A0
	TEST_W  PEND2(A4)
	BMI.S   30$
	SUBQ.L  #2,A0
30$
	MOVE.W  (A0),CONJ(A4)
	RET
 
*--------------------------------
	
PARSEIT
	        
*PERFORM THE PARSE.
*EXIT VARIABLES ARE LEFT INTH
*FOLLOWING AREAS:
	
*NPC.LBFBUF1
*NPC.LBFBUF2   ;AUX NP USED ONLY FOR "MANY"
*THE USUAL DATA AREAS.
* AND THE VARABLES DEFINED ABOVE.
	
*ASSUME 1STWRD IS GOT.
	
	ST      ERR4(A4)           ;MASK NASTIES
	MOVE.L  A6,NPC.WRD(A4)
	CALL    FLUSHVAR
	CALL    PARSEFOU           ;EG. WHERE CAN..
	TEST_W  D7                 ;ERROR?
	BEQ.S   90$                ;DONE.
	CALL    PARSETHR           ;JUST
	TEST_W  D7
	BEQ.S   90$                ;DONE
	MOVE.L  NPC.WRD(A4),A6
90$
	CLR.W   ERR4(A4)           ;LET THEM OFF  
	RET
*--------------------------------
	
DOHOW
	
*DEAL WITH THE EXCEPTIONAL COMBINATIN
*HOW MUCH...
*HOW MANY...
	
*ENTRY:
*WRD DATA SET UP ALREADY
	
*EXIT:
*NPCABOUTT BIT FLAGS SET AS FOLLOWS..
*0 = ABOUT
*2 = MANY
*2 = MUCH
	
	
	CMP.W   #SECT.VAL,D6      ;MUCH OR MANY?
	BEQ.S   HOW.VAL
	CMP.W   #SECT.CONJ,D6
	BNE     PFAIL
	CMP.W   #$12,D5           ;ABOUT?
	BNE     PFAIL
	MOVE.B  #1,NPCABOUT(A4)   ;FLAG IT
	CALL    DEC.N2
	CLR.W   D7                ;OK
	RET
HOW.VAL
	TEST_W  D5                ;MUCH?
	BEQ.S   10$
	CMP.W   #1,D5             ;MANY?
	BNE     PFAIL
	BSET    #2,NPCABOUT(A4)   ;FLAG
	DO	MNPH
	TEST_W  D7
	BNE     PFAIL
	LEA     NPC.LBF1(A4),A0
	LEA     TEMPBUF(A4),A1
20$
	MOVE.W  (A1)+,(A0)+
	BNE.S   20$                ;SAVE NP
10$
	BSET    #1,NPCABOUT(A4)    ;FLAG
	DO	GETWRD
	CALL    PARSEONE           ;(BE)+N+S
	TEST_W  D7
	BEQ.S   99$                ;EXIT
	CALL    PARSETHR           ;(MODEL)+N+S
	TEST_W  D7
	BNE     PFAIL
99$
	RET
*--------------------------------
	
FLUSHVAR
	        
*CLEAR NPC QUESTION VARIABLES
*AND ZAP BUFFERS
	
	MOVE.L  D0,-(A7)
	MOVEQ   #-1,D0
	MOVE.L  D0,NPC.TOBE(A4)
	MOVE.W  D0,NPCMODEL(A4)
	CLR.W   NPCABOUT(A4)
	MOVE.W  D0,NPC.LBF1(A4)
	MOVE.W  D0,NPC.LBF2(A4)
	MOVE.L  (A7)+,D0
	MOVE.W  #2,PERSON(A4)       ;DEFAULT IS 3RD
	RET

*--------------------------------
	
NESTSPK
	
	IFEQ     PROD
	PRINT    <'THIS HASN''T BEEN DONE YET'>
	ENDC
	BRA     PFAIL

*--------------------------------
	        
	        
ONECOP
	        
	DO	P.SAY
05$
	MOVEQ	#8,D1
	RND_D1			;random 0-7
	TEST_W	D1
	BNE.S   10$
	MSG	TRICKQ
	BRA.S   90$
10$
	CMP.W   #2,D1			;not suitable
	BEQ.S	05$

	BeSure	M.IHNI

	ADD.W	#M.IHNI-1,D1		;I have ni idea -1
	DO	P.MSG
90$
	MOVEQ   #2,D7               ;FLAG AS DONE
	MOVE.L  (A7)+,D1            ;DUMP
	RET
	        
*--------------------------------

      END

