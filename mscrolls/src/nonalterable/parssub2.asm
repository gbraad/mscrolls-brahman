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
*	@(#)parssub2.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.PARSER.SUB2
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"


      XDEF     BUFFIX,FIXIREL,PARVERB,DOYESNO

      XREF     KILLDOUB,TEMPBUF,NOUN,VERB,FIXSYS,GETWRD,VERB2,NOERR,PARSTO
      XREF     ADVERB,CLRADJ,YESERR,YNHANDLR,P.SAY,W.IAGREE,GO,ERR4,PR20
	        
BUFFIX
  
	CALL    KILLDOUB
 
* ZAP DUPLICATE ENTRIES IN TEMPBUF
*
* REMOVE ALL NOUNS W HI BIT SET
* FROM TEMPBUF  
  
05$
	LEA     TEMPBUF(A4),A1
10$
	TEST_W  (A1)+
	BEQ.S   90$                ;DONE
	BPL.S   10$                ;OK, NEXT
15$
	MOVE.W  (A1),-(A1)         ;BACK
	BEQ.S   20$                ;END?
	ADDQ.L  #4,A1              ;EXT
	BRA.S   15$
20$
	SUB.W   #2,NOUN(A4)
	BRA.S   05$ 
90$
  
	RET
 
*--------------------------------
 
FIXIREL 
 
	MOVE.L  A0,-(A7)           ;TO BE SAFE..
	CMP.W   #VNEXAMINE,VERB(A4)
	BLS.S   90$
	CLR.W   D5
	LEA     TEMPBUF(A4),A1
10$
	MOVE.W  (A1)+,D0
	BEQ.S   90$                ;DONE
	DO	FIXSYS             ;A3
	MOVE.L  A3,A0
	ADDQ.L  #2,A3
	TEST_W  (A3)
	BEQ.S   10$
	SUBQ.L  #2,A3
	MOVE.L  A1,A2              ;SAVE
70$
	MOVE.W  (A1)+,D0           ;NEXT #
	BEQ.S   20$
40$
	MOVE.W  (A3)+,D1
	BEQ.S   30$                ;DONE
	CMP.W   D0,D1
	BNE.S   40$                ;REST
	MOVE.L  A1,A5
60$
	MOVE.W  (A1),-(A1)
	BEQ.S   50$
	ADDQ.L  #4,A1
	BRA.S   60$
50$
	MOVE.L  A5,A1
	SUBQ.L  #2,A1
30$
	MOVE.L  A0,A3
	BRA.S   70$
20$
	MOVE.L  A2,A1
	BRA.S   10$
90$
	MOVE.L  (A7)+,A0
	RET
 
*--------------------------------
	
	
PARVERB
	
*PARSE A VERB
	
*ENTRY:
*D6,D5 ALREADY VERB FOUND
	
      IFNE    YCHEAT
	CMP.W   #VNLOOP,D5
	BNE.S   01$                ;NO
	DO	GETWRD	            ;WIL BE A VERB
	MOVE.W  D5,VERB2(A4)
01$
      ENDC
	
	DO	NOERR
	MOVE.L  A6,PARSTO(A4)
	DO	GETWRD	            ;NEXT
	TEST_W  D7
	BNE.S   20$
	CMP.W   #SECT.DIR,D6       ;FOR UP/DOWN
	BNE.S   10$                ;NO, CHK FOR CONJ TYPE
	
	BSET    #15,D5             ;FLAG AS DIR
	MOVE.W  D5,ADVERB(A4)      ;INCASE IT COMES BACK
	MOVE.L  (A7)+,A3           ;ADJust stack?
	BCLR    #15,D5
	CALL    GO
	MOVE.L  A3,-(A7)           ;RET, RET

	MOVE.L  A6,PARSTO(A4)
	DO	GETWRD	            ;OK, NEXT
10$
	CMP.B   #SECT.VERB,D6
	BNE.S   11$
	CMP.W   #VNEXIT,D5         ;SPECIAL CASE
	BEQ.S   12$
11$
	CMP.W   #SECT.CONJ,D6      ;CONJ/
	BNE.S   20$                ;NO, OK RET POSN
12$
	MOVE.W  D5,ADVERB(A4)      ;SAVE IT
	BRA.S   90$
20$
	MOVE.L  PARSTO(A4),A6
	DO	CLRADJ
90$
	CLR.W   ERR4(A4)
	DOGO	YESERR  
  
*--------------------------------
	        
	        
DOYESNO
	        
	PEA     PR20(PC)
  
	TEST_L  YNHANDLR(A4)       ;REQUEST?
	BEQ.S   10$                ;NO
	MOVE.L  YNHANDLR(A4),A0
	JMP     (A0)               ;GO TO THAT ROUTINE
10$
	DO	P.SAY
	MSGGO	IAGREE
  
*--------------------------------

      END

