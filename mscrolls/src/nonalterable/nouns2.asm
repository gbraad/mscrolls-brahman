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
*	@(#)nouns2.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PARSER.NOUNS2
*
*--------------------------------
 
	include	"equates.asm"
	include	"macros.asm"

      XDEF     DO.PRON,MNPH,MNPHREC,DO.EXCEP,GETCONJ

      XREF     ISBITRAN,WSYN2,PRONHIM,PRONHER,OLDBUF,INCNOUN2,GNG2,P.IDFY
      XREF     VALID,P.NFND,DEPTH,W.HASSLE,DATIVE,TEMPBUF,TMPPNTR,GNGRP
      XREF     DODATIVE,NOUN,EXBUFF,BUFFIX,FIXIREL,WSYN,W.BTHE,POSWRD
      XREF     PRTWRD,P.BTN,W.WNHA,LINE,NOUNBUF2,NOUNBUF3,W.4BUFS,NOERR,D72
	  XREF	   ERR4




*--------------------------------
  
DO.PRON
   
	TEST_W  D3                 ;NOUN ALREADY?
	BEQ.S   01$                ;NO, OK
	CALL    ISBITRAN           ;VERB ALLOWED?
	BEQ     WSYN2              ;NO, ERROR
01$
	CMP.B   #4,D5              ;THEM?
	BEQ.S   40$                ;YES
	CMP.B   #6,D5              ;HIM?
	BNE.S   10$
	MOVE.W  PRONHIM(A4),D5
	BEQ.S   70$
	BRA.S   30$                ;DONE
10$
	CMP.B   #5,D5              ;HER
	BNE.S   20$
	MOVE.W  PRONHER(A4),D5 
	BEQ.S   70$  
	BRA.S   30$                ;DONE
20$
	MOVE.W  OLDBUF(A4),D5      ;ELSE
	BEQ.S   70$
30$
	CALL_S  DO.PROD.VALID      ;CHECK VALID
	TEST_W  D7                 ;?
	BNE.S   90$                ;EXIT
	DO	INCNOUN2
	BRA.S   60$ 
40$
	LEA     OLDBUF(A4),A2
	TEST_W  (A2)
	BEQ.S   70$
50$
	MOVE.W  (A2)+,D5
	BEQ.S   60$                ;FIN
	CALL_S  DO.PROD.VALID      ;VALID?
	TEST_W  D7
	BNE.S   90$                ;NO EXIT
	DO	INCNOUN2           ;ADD
	BRA.S    50$
60$
	MOVEQ   #1,D3
	BRA     GNG2
 
70$
	BRA     P.IDFY
90$
	MOVEQ   #2,D7
	RET
  
	
DO.PROD.VALID

	MOVE.W  D0,-(A7)          ;SAVE IT
	MOVE.W  D5,D0
	DO	VALID             ;ONLY IF LOCAL
	BEQ.S   10$               ;OK
	   TEST_W  ERR4(a4)			; mask?
	   BNE.S   10$
	CALL    P.NFND            ;NOT FOUND
10$
	MOVE.W  (A7)+,D0          ;BAK
	RET

*--------------------------------
  
MNPH 
   
* MAJOR NOUN PHRASE (WHOEVER HE IS)

	CLR.W   DEPTH(A4)          ;RECURSION LEVEL

MNPHREC

*RECURSIVE MNPH ENTRY POINT

	CMP.W   #RECLIM,DEPTH(A4)  ;RECUSION LIMIT
	BLT.S   02$                ;OK
	MSG	HASSLE
	DO	NOERR              ;MASK ERRORS 4 NOW
	DOGO	D72
02$
   
	ADDQ.W  #1,DEPTH(A4)       ;INC RECUSION LEVEL
	
	CLR.L   DATIVE(A4)         ;RESET
	MOVE.L  A3,-(A7)
	LEA     TEMPBUF(A4),A3
	MOVE.L  A3,TMPPNTR(A4)
	MOVE.L  (A7)+,A3
	CALL    GNGRP
	MOVE.W  D5,D2
	CLR.W   D5
	MOVE.W  D3,-(A7)		; save 'and' flag
	DO	INCNOUN2           ;ADD 0
	MOVE.W  (A7)+,D3
	MOVE.W  D2,D5              ;BAK
	TEST_L  DATIVE(A4)
	BEQ.S   01$
	CALL    DODATIVE
	BRA     20$                ;EXIT
01$
	CMP.B   #1,D7              ;ERROR?
	BEQ.S   20$                ;END
	BHI.S   90$                ;YES JUST EXIT
	CMP.W   #SECT.SPEC,D6      ;SP?   
	BNE.S   20$                ;ANOTHER
	CMP.W   #09,D5             ;EXCEPT?
	BNE.S   20$                ;ANOTHER
	CMP.W   #4,NOUN(A4)        ;ONLY ONE NOUN SPECIFIED?
	BHI.S   03$                ;NO
	CMP.W   #2,DEPTH(A4)       ;AMBIGUOUS POSITION?
	BHS.S   20$
03$
	MOVE.L  A3,-(A7)
	LEA     EXBUFF(A4),A3
	MOVE.L  A3,TMPPNTR(A4)
	MOVE.L  (A7)+,A3
	CALL_S  DO.EXCEP           ;HMMMMMMMMMMM
	BRA.S   01$
20$
	MOVE.W  D5,-(A7)
	CALL    BUFFIX             ;ACTUALLY SQUASH BUFF THIS TIME
	DO	FIXIREL
	MOVE.W  (A7)+,D5  
90$
	SUBQ.W  #1,DEPTH(A4)       ;GOING UP ONE
	RET
  
DO.EXCEP
 
	TEST_B  D3                 ;NOUN FLG
	BEQ     WSYN               ;AND EXCEPT
	CALL    GNGRP
	CMP.W   #1,D7              ;E O S IS ALLOWED
	BEQ.S   05$
	TEST_B  D7                 ;ERROR?
	BNE.S   90$                ;YES
05$
	MOVE.W  D5,D2
	CLR.W   D5
	DO	INCNOUN2
	MOVE.W  D2,D5
  
* NOW FIX TEMPBUFF CONTENTS
 
	LEA     EXBUFF(A4),A0
10$
	MOVE.W  (A0)+,D0           ;GET CHR FROM EXBUFF
	BEQ.S   90$                ;DT?
	LEA     TEMPBUF(A4),A1
11$
	MOVE.W  (A1)+,D1           ;GET CHR FROM TEMPBUF
	BEQ.S   25$                ;ERROR
	AND.W   #$7FFF,D1
	CMP.W   D0,D1              ;MATCH?
	BNE.S   11$                ;GET NEXT
	EOR.W   #$8000,-(A1)       ;INVERT HI BIT
	BRA.S   10$
25$   
	DO	P.BTN
	MSG	WNHA
	MSG    LINE
	BRA.S   10$
90$
	RET
  
   
GETCONJ
  
	DO	MNPH               ;GET NOUNS
	LEA     TEMPBUF(A4),A0
	LEA     NOUNBUF2(A4),A1
	TEST_W  (A1)               ;IS ALREADY FUL?
	BEQ.S   10$                ;NO
	LEA     NOUNBUF3(A4),A1
	TEST_W  (A1)
	BEQ.S   10$                ;FOR THE 3RD NPH
	MSG	4BUFS            ;I CANT HANDLE THAT MANY NPS
	MOVEQ   #2,D7
	RET
10$
	MOVE.W  (A0)+,(A1)+        ;A NOUN
	BNE.S   10$
	RET

      END

