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
*	@(#)nouns3.asm	2.10
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PARSER.NOUNS3
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"

	XREF    DEPTH,RelBuf,RelPnt

	XREF    GETWRD,WSYN2,WRDPNT,GETADJPH,P.IDFY,OLDBUF
	XREF    GETDATA,GET_CONT,W.NO.TNG,W.IN,P.TN,INCNOUN2
	XREF    P.BTN,ISAREN,W.NCONT,GNG2,TEMPBUF,TMPPNTR
	XREF    NOUN,DATIVE,VERB,MATCHBUF,ADJ.TBL,ADJBUF
	XREF    P.NOTANY,FROMWCH,DNOUNBUF,OLDBUF
	XREF	P.TNSTOP

	XREF    SECTION_,WORD_

	XDEF    DO.CONTE,DO.ONE,RESADJ,KILLDOUB,ISBITRAN,MUST4,RESREL

DO.CONTE
 
	DO	GETWRD
	CMP.W   #SECT.SPEC,D6
	BNE     WSYN2
	CMP.W   #$B,D5                  ;'OF'
	BNE     WSYN2
	MOVE.L  A6,WRDPNT(A4)           ;UPDATE
	DO	GETADJPH
	CMP.B   #1,D7
	BHI     98$
	BEQ     99$
	CMP.W   #SECT.NOUN,D6
	BEQ.S   07$
	CMP.W   #SECT.SPEC,D6
	BNE.S   03$
	CMP.W   #3,D5                    ;IT?
	BEQ.S   05$
03$
	BRA     P.IDFY
05$
	MOVE.W  OLDBUF(A4),D5
	SUBQ.W  #1,D5
07$
	MOVE.W  D5,D0   
	ADDQ.W  #1,D0
	GETINFO        
	DO	GET_CONT
	BCS.S   20$
	TEST_W  (A1)               ;NONE
	BNE.S   10$
	MSG	NO.TNG
	MSG	IN
	DO	P.TNSTOP
	BRA.S   98$
10$
	MOVE.W  (A1)+,D5
	BEQ.S   99$
	BMI.S   10$		;did BTST #15,D5  BNE 10$ 
	DO	INCNOUN2
	BRA.S   10$
20$
	DO	P.BTN
	GETINFO        
	DO	ISAREN
	MSG	NCONT
98$
	MOVEQ   #2,D7
	RET
99$
	MOVEQ   #1,D3              ;'AND' FLAG
	BRA     GNG2
 
*--------------------------------
*
* KILLDOUB -   IF NOUN IS IN
*              TEMPBUF TWICE
*              THEN SECOND ONE
*              IS NOBBLED
*
* SO 'L X,X' DOES 'L X'
*
*--------------------------------
 
KILLDOUB
 
	LEA     TEMPBUF(A4),A1      ;GET START OF BUFFER
10$
	MOVE.W  (A1)+,D1            ;GET FIRST ENTRY
	BMI.S   10$                 ;ALREADY NOBBLES?
	BEQ.S   90$                 ;END OF BUFFER?
	MOVE.L  A1,A2               ;NOPE - GET POINTER TO REST OF BUFFER
20$
	MOVE.W  (A2)+,D2            ;GET A NOUN TO COMPARE AGAINST
	BMI.S   20$                 ;NOBBLED?
	BEQ.S   10$                 ;FINISHED (FOR THIS NOUN)
	CMP.W   D1,D2               ;OK - DO COMPARE
	BNE.S   20$                 ;DIFFERENT - NEXT..
	OR.W    #$8000,-(A2)        ;SAME - NOBBLE
	BRA.S   20$                 ;NEXT ...
90$
	RET                         ;LET BUFFIX TIDY UP

*--------------------------------
	
L.BITRANS
	
*LIST OF BITRANSITIVE VERBS
	
*EFFECTIVELY NO 'AND' IS REQUIRED
*BETWEEEN NOUNS, IF THE VERB IS OF
*THIS TYPE.
	
		SVERB	OFFER
		SVERB	FEED
		SVERB	PAY
		SVERB	GIVE
		SVERB	SHOW

		DC.B    0

      		ALIGN 
*--------------------------------
	
ISBITRAN       
	
*IS VERB BITRANSITIVE
*EXIT:
*EQ =NO
*NE = YES
	
	PUSH_L  A0/D1 
	LEA     L.BITRANS(PC),A0
10$
	MOVE.B  (A0)+,D1           ;GET ONE
	BEQ.S   90$                ;NOT IN LIST
	CMP.B   VERB+1(A4),D1      ;SAME?
	BNE.S   10$                ;NO
	MOVE.L  TMPPNTR(A4),A0
	ADD.W   NOUN(A4),A0
	MOVE.L  A0,DATIVE(A4)
90$
	PULL_L   D1/A0
	RET

*--------------------------------


MUST4
	
*IN AMBIGUOUS CASES SUCH AS
*PUT ALL IN X  (INTO WHAT?)
	
*SOMETIMES A 4PART MUST BE THE CASE
	
*EXIT:
*EQ= 4PART REQUIRED
*NE= NOT-NECESSARILY
	
	CLR.W   D0
	LEA     MUST4_L(PC),A1
10$
	MOVE.B  (A1)+,D0
	CMP.B	#-1,D0
	BNE.S	20$			;not at end of list
	SETNE				;fail
	RET
20$
	CMP.W   VERB(A4),D0        ;IN LIST?
	BNE.S   10$
99$
	RET


*--------------------------------
	
MUST4_L

	SVERB   LOOK
	SVERB   EXAMINE
	SVERB   GET
	SVERB   TAKE
	SVERB   CARRY
	SVERB   CAST
	SVERB   PLANT
	SVERB   MIX
	SVERB   ASK
	SVERB   SAY

	DC.B    -1

	ALIGN
	
*--------------------------------
	
	
RESREL
	
*RESOLVE  RelBuf

	CMP.W   #1,DEPTH(A4)      ;are we in the middle of a relphrase?
	BGT.S   05$               ;EQ => no, so do normally
	RET

05$
	PUSH_L  D0-D3/A0-A1       ;Beware stompings

	MOVEQ   #0,D3             ;count (very) good relations

	LEA     RelBuf-2(A4),A1   ;'noun stack'  (-2 to miss current term.)
	ADD.W   RelPnt(A4),A1     ;and pointer (well ok, so it's an offset
	                          ;really
	MOVE.B  1(A1),D1          ;get relation type required
10$
	TEST_W  -(A1)             ;Find start of relevant entry
	BPL.S   10$
	ADDQ.L  #2,A1             ;move past terminator and relation type
20$
	MOVE.W  (A1)+,D0          ;Get a noun
	BMI.S   50$               ;end of entry on stack
	ADDQ.W  #1,D0             ;convert word# to noun#
	GETINFO        
	MOVE.B  6(A0),D0          ;get relation type
	AND.B   D1,D0             ;mask (since insep. n/a compare is no good)
	BEQ.S   20$               ;EQ => no good, wrong relationship

* now lets see if this noun is related to any of the nouns in MATCHBUF

	MOVE.W  8(A0),D0          ;location noun#
	SUBQ.W  #1,D0             ;convert to word#
	LEA     MATCHBUF(A4),A0
30$
	MOVE.W  (A0)+,D2          ;Get an entry from MATCHBUF
	BMI.S   20$               ;MI=> end of matches, try next noun on 'stack'
	CMP.W   #SECT.NOUN,D2     ;gotta be a noun
	BNE.S   40$
	CMP.W   (A0),D0           ;any good?
	BNE.S   40$
	BSET    #7,(A0)           ;YES! - flag word# hi bit
	ADDQ.W  #1,D3             ;count good ones
40$
	ADDQ.L  #2,A0             ;next entry in MATCHBUF
	BRA.S   30$

	
* ok, lets see where that got us..

50$

	  LEA      MATCHBUF(A4),A0
60$
	  MOVE.L   (A0)+,D0
	  BMI.S    90$
	  BCHG     #7,-2(A0)         ;CLEANUP wants bitset=> no good
	  TEST_W   D3
	  BNE.S    60$
	  BCLR     #7,-2(A0)         ;if we didnt find ANY matches then do a
	                             ;normal RESADJ
	  BRA.S    60$

90$
	  CALL     CLEANUP           ;lose hi bit set noun# from MATCHBUF
	  PULL_L   D0-D3/A0-A1

	  LEA      MATCHBUF(A4),A0
	  MOVEQ    #0,D4             ;Count
95$
	  TEST_L   (A0)+
	  BMI.S    99$
	  ADDQ.W   #1,D4             ;return D4 = no of matches still left
	  BRA.S    95$
99$
	  RET
*
* RESolve ADJectives -	removes nouns in match buf which suffer adjective
*			clashes	
*

RESADJ
 
	LEA     MATCHBUF(A4),A0
10$
	MOVE.L  (A0)+,D0          ;GET A NOUN
	BMI.S   90$               ;DONE, EXIT
	LEA     ADJ.TBL(A4),A1
	AND.W   #$7FFF,D0         ;loose bit we may have set above

	IFEQ	FastBits

	SUBQ.W  #1,D0             ;ADJUST FOR DBRA
	BMI.S   20$               ;SKIP IF 1ST ONE
15$
	TEST_B  (A1)+             ;END OF LIST?
	BNE.S   15$               ;NO
	DBRA    D0,15$            ;UNTIL CORRECT

	ENDC

	IFNE	FastBits

	BEQ.S	20$

	DC.W	$A0F1		;Find Nth zero;

	ENDC
20$
	LEA     ADJBUF(A4),A2
30$
	MOVE.L  A1,A3             ;SAVE START
	MOVE.W  (A2)+,D0          ;GET ADJ
	BEQ.S   10$               ;DONE, A MATCH, NEXT
	ADDQ.B  #3,D0
32$
	MOVE.B  (A3)+,D1          ;GET ADJ IN TABLE
	BNE.S   31$               ;NOT END
	BSET    #7,-2(A0)         ;FLAG NOUN AS FAIL
	BRA.S   10$               ;TRY NEXT
31$
	CMP.B   D0,D1             ;ADJ MATCH?
	BNE.S   32$               ;TRY REST OF LIST
	BRA.S   30$               ;OK, NEXT ADJ IN BUFFER
90$
	CALL_S  CLEANUP           ;DELETE FLAGGED NOUNS
	TEST_L  MATCHBUF(A4)      ;ANY?
	BPL.S   99$               ;YES
	CALL    P.NOTANY          ;THERE ISNT ANY
99$
	RET
	
*--------------------------------
	
CLEANUP
	
* CLEAN UP ALL HI-BIT FLAGGED WORDS
* IN MATCHBUF
	
	LEA     MATCHBUF(A4),A0
	MOVE.L  A0,A1
10$
	MOVE.L  (A0)+,D0
	BMI.S   90$                      ;DONE
	TEST_W  D0
	BMI.S   10$                      ;FORGET THIS WORD
	MOVE.L  D0,(A1)+                 ;INTO BUFF AGAIN
	BRA.S   10$                      ;NEXT
90$
	MOVE.L  D0,(A1)                  ;AS TERMINATOR
	RET


*--------------------------------

	
DO.ONE
	
	LEA     MATCHBUF(A4),A0
	TEST_W  FROMWCH(A4)              ;FROM WHICH ONE??
	BEQ.S   01$                      ;NO
	LEA     DNOUNBUF(A4),A1
	CLR.W   FROMWCH(A4)              ;NOT NEXT TIME
	BRA.S   02$
01$
	LEA     TEMPBUF(A4),A1
02$
	TEST_W  (A1)
	BNE.S   05$
	LEA     OLDBUF(A4),A1
05$
	CALL_S  80$
	CALL    RESADJ                  ;RESOLVE
	BRA.S   COUNT                   ;AND SET D4
80$
	MOVE.W  #SECT.NOUN,(A0)+
	MOVE.W  (A1)+,(A0)
	BEQ.S   81$
	SUBQ.W  #1,(A0)+
	BRA.S   80$
81$
	SUBQ.L  #2,A0
	MOVE.W  #-1,(A0)
90$
	RET

*--------------------------------

	
COUNT
	LEA     MATCHBUF(A4),A0 
	MOVE.W  (A0),SECTION_(A4)
	MOVE.W  2(A0),WORD_(A4)     ;SET UP
	CLR.W   D4
10$
	TEST_L  (A0)+
	BMI.S   90$
	ADDQ.W  #1,D4
	BRA.S   10$
90$
    
	RET

*--------------------------------


	END
