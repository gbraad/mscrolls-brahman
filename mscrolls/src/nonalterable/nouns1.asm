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
*	@(#)nouns1.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PARSER.NOUNS1
*
*--------------------------------
 
	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"


      XDEF     GNGRP,BICHK,MULT.WRD,WSYN2,INCNOUN2,GNG1,GNG2

      XREF     CLRADJ,NOUN,GNGTEMP,WRDPNT,GETADJPH,DO.ALL,DO.PRON,DO.CONTE
      XREF     ISBITRAN,PERSON,ERR4,MULT.TAB,MATCHBUF,VALID,P.NOTANY,RESADJ
      XREF     RELPH,W.WRSYN,TMPPNTR,ERR1,D72,ISDOFRE

      IFEQ	THE_PAWN

      XREF	Scenery

      ENDC


*--------------------------------
*
* GET NOUN GROUP
*
* NOUN = POINTER TO INPUT BUFFER
*   D3 = FLAG
*   D7 = EXIT FLAGS
*     
*    0 = OK, CONTINUE
*    1 = END OF SENTENCE
*    2 = ERROR, MSG DONE
*
*--------------------------------
 
 
GNGRP
	DO	CLRADJ               ;kill pending adjectives
	MOVE.W  #0,NOUN(A4)          ;no. of nouns in this noun group
	ST      GNGTEMP(A4)          ;NULL
GNG1
	CLR.W   D3                   ;'AND' FLAG
GNG2
	MOVE.L  A6,WRDPNT(A4)        ;UPDATE
	DO	GETADJPH
	TEST_W  D7
	BNE.S   90$                  ;EXIT
	CMP.B   #SECT.SPEC,D6        ;SPEC
	BNE.S   10$
	TEST_W  D5                   ;AND?
	BEQ.S   GNG1                 ;LTS NOT WORRY ABOUT TOO MANY..
	CMP.W   #$8,D5               ;= THEN?
	BHS.S   01$                  ;YES = MISS BITRANS TEST
	CALL    BICHK                ;TEST BITRANS IF 'SPECIAL' IS A NOUN
*                                   ;GROUP EG IT,THEM,ALL... (1-6)
01$
	CMP.B   #2,D5                ;ALL?
	BLS     DO.ALL               ;YES
	CMP.B   #6,D5                ;PRON
	BLS     DO.PRON
	CMP.W   #$A,D5               ;CONT?
	BEQ     DO.CONTE
13$
	CLR.W   D7                   ;OK
90$
	RET
10$
	CMP.W   #SECT.NOUN,D6        ;NOUN?
	BNE.S   12$
	TEST_W  D3                   ;AND FLAG?
	BEQ.S   21$
	CMP.W   GNGTEMP(A4),D5       ;SAME AS B4?
	BEQ.S   GNG2
	CALL    ISBITRAN             ;BITRANSITIVE?
	BEQ     WSYN2                ;NOT, THUS ERROR
21$
	MOVE.W  D5,GNGTEMP(A4)       ;STO
	ADDQ.W  #1,D5
	DO	INCNOUN2
	BRA.S   GNG2
12$
	CMP.W   #SECT.MULT,D6        ;MULTIPLE
	BNE.S   19$
	CALL    BICHK
	BRA     MULT.WRD
19$
	CMP.W   #SECT.YOU,D6         ;YOU?
	BNE.S   14$
	MOVEQ   #1,D5                ;SECOND PERSON
	BRA.S   16$                  ;DUMY
14$
	CMP.W   #SECT.VERB,D6
	BNE.S   20$
	CMP.W   #VNINVENTORY,D5      ;I (SPECIAL CASE)
	BNE.S   13$                  ;NO, FAIL
	MOVEQ   #0,D5                ;1ST PERSON
16$
	CMP.W   #2,PERSON(A4)        ;ALREADY?
	BNE.S   17$                  ;FAIL, DONE ALREADY
	MOVE.W  D5,PERSON(A4)
17$
	
	
	TEST_W  ERR4(A4)
	BNE.S   18$               ;I IS TREATED AS A NOUN IN THIS CONTEXT
	BRA.S   13$               ;SET TO FAIL
20$
	CMP.B   #SECT.NOT,D6      ;BUT NOT...
	BNE.S   13$               ;NO, FAIL
	BRA     GNG1              ;SKIP WRD, CLR SYNTAX
18$
	
	CLR.W   D5
	DO	INCNOUN2        ;DUMMY
	BRA     GNG2
	
*--------------------------------
	
BICHK
	
	TEST_W  D3
	BEQ.S   90$            ;GO
	CALL    ISBITRAN       ;ALLOWED?
	BNE.S   90$            ;OK
	MOVE.L  (A7)+,D0       ;DUMP
	BRA     WSYN2
90$
	RET
	
*--------------------------------
   
MULT.WRD 
  
	CLR.B   D4
	LEA     MULT.TAB(A4),A0
	TEST_W  D5
	BEQ.S   08$
05$
	MOVE.W  (A0)+,D0
	BNE.S   05$
	SUBQ.W  #1,D5
	BNE.S   05$
08$
	LEA     MATCHBUF(A4),A1
	MOVEQ   #SECT.NOUN,D0
	DO	ISDOFRE			;does verb allow completely anything?
	SNE	D5			;flag EQ => don't call valid
*	ST      D5             ;SUPRESS ERR IN VALID - see above
	SWAP    D0
	MOVE.W  D7,-(A7)
	
10$
	MOVE.W  (A0)+,D0
	BEQ.S   20$
	TEST_B	D5			;is D0 free?
	BEQ.S	15$			;eq => yes, don't use valid
	DO	VALID
	BNE.S   10$
15$
	SUBQ.W  #1,D0          ;ADJUST TO PRENOUN#
	MOVE.L  D0,(A1)+       ;ONTO MATCHBUF
	ADDQ.B  #1,D4
	BRA.S   10$
20$
	MOVE.W  (A7)+,D7
	MOVE.W  #-1,(A1)        ;TERMINATE MATCHBUF
    
	TEST_B  D4
	BNE.S   30$
21$
	BRA     P.NOTANY
30$
	CALL    RESADJ
	TEST_W  D7
	BNE.S   34$
	CALL    RELPH           ;PROCESS POSSIBLE REL PHRASE
34$
	CMP.B   #1,D7           ;ERROR TYPE?
	BHI     D72             ;PRINTED
	BEQ.S   33$             ;WANT OND BUFFER BACK
	TEST_W  D4              ;ANY?
	BNE.S   31$             ;SOME
33$
	MOVE.W  #SECT.NOUN,MATCHBUF(A4)    ;A CUNNING AND DEVIOUS WAY..
31$
	LEA     MATCHBUF(A4),A0
32$
	MOVE.L  (A0)+,D5        ;GET ONE
	BMI     GNG2            ;DONE
	ADDQ.W  #1,D5
  
	DO	INCNOUN2
   
	BRA.S   32$             ;NEXT
90$
	RET
 
*--------------------------------
  
WSYN2
	MOVEQ   #2,D7   
	TEST_W  ERR1(A4)        ;ERRORS BEEN NOBBLED?
	BNE.S   99$

	IFEQ	THE_PAWN

	CMP.W   #SECT.SCEN,D6	;scenery noun?
	BEQ     Scenery

	ENDC

	MSG	WRSYN         ;NOPE - GIVE ERROR
99$
	RET
 
*--------------------------------
 
INCNOUN2
   
* NOUN=PNTR TO INPUT BUFFER
  
	MOVE.L  TMPPNTR(A4),A3
	ADD.W   NOUN(A4),A3
	MOVE.W  D5,(A3)+
	ADDQ.W  #2,NOUN(A4)
	CLR.W   (A3)
	MOVEQ   #1,D3
     
	DOGO	CLRADJ         ;CLR RESIDUAL ADJ IN BUF
   
*--------------------------------

      END

