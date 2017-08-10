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
*	@(#)search.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SEARCH
*
*--------------------------------

	include	"equates.asm"
	include "macros.asm"


	XREF    TBL.WRD,TYPE,ADJFLG,MATCHBUF,POSWRD,ADJ.TBL
	XREF    ADJBUF,SYNONYM,SYN_L

	XDEF    SEARCH,SEARCH2



      IFEQ    FastBits                  ;ho ho ho


      IFNE	THE_PAWN		;new search?

SEARCH
	
	LEA     TBL.WRD(A4),A3
	CLR.W   D6
 
SEARCH2
 
	CLR.W   D5
	CLR.W   TYPE(A4)
	CLR.W   ADJFLG(A4)
	LEA     MATCHBUF(A4),A2
	LEA     ADJ.TBL(A4),A0
	MOVE.L  A6,POSWRD(A4)
	MOVE.L  A6,A5                ;STORE START OF WRD
	
10$
	CLR.B   D0                   ;LETTER COUNT
11$
	ADDQ.B  #1,D0                ;INC LETTER
	MOVE.B  (A6),D1              ;GET CHR FROM INBUF
	BEQ.S   23$                  ;ZERO, LINE END THUS MATCG
	CMP.B   #$20,D1              ;SPACE?
	BEQ.S   23$                  ;ALSO MATCH
*	CMP.B   #'"',D1              ;QUOTES?
*	BEQ.S   21$                  ;MATCH
	CMP.B   #39,D1               ;APOSTROPHE?
	BEQ.S   22$                  ;ALSO, MATCH
	ADDQ.L  #1,A6     ;NEXT
*	CMP.B   #$60,D1   
*	BLS.S   12$
	AND.B   #$5F,D1              ;TO UPPER CASE
12$
	MOVE.B  (A3)+,D2             ;CHR FROM TEXT
	BMI.S   30$                  ;DONE, HI BIT SET
*--------------------------------
	 
*	CMP.B   #$60,D2 
*	BLS.S   13$
	AND.B   #$5F,D2              ;TO UPPER CASE
13$
*--------------------------------
	
	CMP.B   D1,D2                ;SAME?
	BEQ.S   11$                  ;YES, UNTIL DONE.
17$
	CMP.B   #SECT.NOUN,D6
	BNE.S   19$                  ;SKIP
18$
	TEST_B  (A0)+                ;ADJ=0?
	BNE.S   18$                  ;UNTIL NEXT
19$
	ADDQ.W  #1,D5                ;NEXT 
15$
	TEST_B  (A3)+                ;SKIP UNTIL END OF WRD
	BPL.S   15$
	CMP.B   #$82,(A3)            ;END OF SECTION?
	BNE.S   16$                  ;NO
	ADDQ.B  #1,D6                ;NEXT SECTION
	ADDQ.L  #1,A3                ;SKIP FLAG
	CLR.W   D5                   ;ZERO START
	CMP.B   #$81,(A3)	;END OF TABLE???
	BNE.S   16$
	RET                       ;YES!
16$
	MOVE.L  A5,A6                ;RESTORE
	BRA.S   10$                  ;TRY AGAIB;N
	
21$
	MOVE.B  #1,TYPE(A4)          ;FLG AS QUOTES
	BRA.S   23$
22$
	MOVE.B  #2,TYPE(A4)          ;'
	MOVE.B  (A6),TYPE+1(A4)      ;SAVE NEXT CHR
	
23$
	CMP.B   #SIGCHAR,D0          ;ENOUGH?
	BLS.S   17$                  ;NO, FAIL
40$
	CMP.B   #SECT.NOUN,D6
	BNE.S   41$                  ;NO
	LEA     ADJBUF(A4),A1        ;START OF SPEC LIST
	MOVE.L  A0,D4                ;STORE START OF REAL LIST
42$
	MOVE.W  (A1)+,D0             ;GET ONE
	BEQ.S   41$                  ;ALL DONE, PASS
	MOVE.L  D4,A0                ;RESTORE START
44$
	MOVE.B  (A0)+,D1             ;GET ONE
	BEQ.S   43$                  ;FAIL, NOT IN LIST
	SUBQ.B  #3,D1                ;ADJ
	CMP.B   D0,D1                ;INLIST ?
	BNE.S   44$                  ;NO, TRY ANOTHER
	BRA.S   42$                  ;YES, FINISH THOSE SPECIFIED.
41$
	CALL    SYNONYM 
	MOVE.W  D6,(A2)+
	MOVE.W  D5,(A2)+
	BRA.S   17$                  ;FORCE FAIL, ANY MORE??
43$
	MOVE.W  #1,ADJFLG(A4)
	BRA.S   19$                  ;FAIL
	
30$
	SUBQ.L  #1,A3                ;BACK1
	BCLR    #7,D2                ;RESET HI-BIT
*	CMP.B   #$60,D2
*	BLS.S   31$
	AND.B   #$5F,D2              ;TO UPPER
31$
	CMP.B   D1,D2                ;MATCH?
	BNE     17$                  ;NO, FAIL    
	MOVE.B  (A6),D1              ;NEXT CHAR
	BEQ.S   40$                  ;OK
	CMP.B   #$20,D1              ;SPACE?
	BEQ.S   40$                  ;OK
*	CMP.B   #'"',D1              ;QUOTES
*	BEQ.S   40$  ;OK
	CMP.B   #39,D1
	BNE.S   23$
	MOVE.B  #2,TYPE(A4)
	MOVE.B  1(A6),TYPE+1(A4)
	BRA.S   40$

      ENDC

      IFEQ	THE_PAWN

	XREF	NewSearch

SEARCH
	
	LEA     TBL.WRD(A4),A3
	CLR.W   D6

SEARCH2


	LEA     ADJBUF(A4),A1
	LEA     MATCHBUF(A4),A2
	LEA     ADJ.TBL(A4),A0
	MOVE.L  A6,POSWRD(A4)

	LEA	SYN_L(A4),A5	;so vax seach can suss synonyms

	MOVE.L	#NewSearch,D0
	JSR	0(A4,D0.L)

	MOVE.W  D0,TYPE(A4)
	MOVE.W  D1,ADJFLG(A4)

	MOVE.L  #-1,(A2)            ;terminator in MATCHBUF
	LEA     MATCHBUF(A4),A2
10$
	MOVE.W (A2),D6              ;Section#
	MOVE.W 2(A2),D5             ;word#
	BMI.S  20$                  ;hit the terminator?
	CALL   SYNONYM              ;convert if necessary

	MOVE.W D6,(A2)+             ;then place back in matchbuf
	MOVE.W D5,(A2)+
	BRA.S  10$

20$
	MOVE.W  -4(A2),D6
	MOVE.W  -2(A2),D5

	RET

      ENDC
      ENDC

******************************************************************************


      IFNE    FastBits

SEARCH
	
	LEA     TBL.WRD(A4),A3
	CLR.W   D6

SEARCH2


	LEA     ADJBUF(A4),A1
	LEA     MATCHBUF(A4),A2
	LEA     ADJ.TBL(A4),A0
	MOVE.L  A6,POSWRD(A4)

	LEA	SYN_L(A4),A5		;so vax seach can suss synonyms

	DC.W    $A0FF               ; Pseudo-op to do the SEARCH

	MOVE.W  D0,TYPE(A4)
	MOVE.W  D1,ADJFLG(A4)

	MOVE.L  #-1,(A2)            ;terminator in MATCHBUF
	LEA     MATCHBUF(A4),A2
10$
	MOVE.W (A2),D6              ;Section#
	MOVE.W 2(A2),D5             ;word#
	BMI.S  20$                  ;hit the terminator?
	CALL   SYNONYM              ;convert if necessary

	MOVE.W D6,(A2)+             ;then place back in matchbuf
	MOVE.W D5,(A2)+
	BRA.S  10$

20$
	MOVE.W  -4(A2),D6
	MOVE.W  -2(A2),D5

	RET

      ENDC

	END

