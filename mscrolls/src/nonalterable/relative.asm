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
*	@(#)relative.asm	2.10
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.RELATIVE1
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"

	XREF   RelBuf,RelPnt,RelPnt2

	XREF   VERB,EXAMPLE,W.BUT,W.THERE,ISAREN,GETDATA,P.NONLY
	XREF   MATCHBUF,DERR,VLST2.P,P.IDFY,GETWRD,TYPE,WORD_
	XREF   SECTION_,TEMPBUF,POSWRD,MNPHREC,YESERR,NOERR
	XREF   ERR1,ERR4,CLRADJ,RELSTK,GNGTEMP,DATIVE,POSSESS
	XREF   TMPPNTR,NOUN,PULFAIL9,CMDPNT


	XDEF   PULMCH,PULREST,PUSH,PULFAIL,RELPH,RELWRD,IS2PART,ISTN

	
PUSH
	
*PUSH A STACK FRAME FOR RELATIVE
*EXPRESSION EVAL
	
*FORMAT:
* A6.L
* D3.W
* GNGTEMP.W
* DATIVE.L
* POSSESS.W       ;POSSESSIVE USEAGE
* TMPPNTR.L       ;BUFFER PNTR IN EXC
* POSWRD.L
* CMDPNT
* TYPE.W
* NOUN.W
* SECTION_.W
* WORD_.W
* TEMPBUF
* MATCHBUF
	
* USES A1,A2 & 'STACK'
	
	MOVE.L  (A7)+,RELSTK(A4)                 ;SAVE RET
	MOVE.L  A6,-(A7)                         ;SAVE TXT PNTR
	MOVE.W  D3,-(A7)
	MOVE.W  GNGTEMP(A4),-(A7)
	MOVE.L  DATIVE(A4),-(A7)
	MOVE.W  POSSESS(A4),-(A7)
	MOVE.L  TMPPNTR(A4),-(A7)
	MOVE.L  POSWRD(A4),-(A7)  
	MOVE.L  CMDPNT(A4),-(A7)
	MOVE.W  TYPE(A4),-(A7)
	MOVE.W  NOUN(A4),-(A7)                   ;INDEX
	MOVE.W  SECTION_(A4),-(A7)
	MOVE.W  WORD_(A4),-(A7)
	LEA     TEMPBUF(A4),A1
	MOVE.L  A1,A2
30$
	TEST_W  (A1)+                       
	BNE.S   30$                              ;TO END
40$                                       
	MOVE.W  -(A1),-(A7)                      ;PUSH ONE ENTRY
	CMP.L   A1,A2
	BNE.S   40$
	LEA     MATCHBUF(A4),A1
	MOVE.L  A1,A2
10$
	TEST_L  (A1)+
	BPL.S   10$                             ;TO END
20$
	MOVE.L  -(A1),-(A7)                     ;PUSH MATCH ON STACK
	CMP.L   A1,A2                           ;BEGINING?
	BNE.S   20$
	MOVE.L  RELSTK(A4),-(A7)
	RET
	
*--------------------------------
	
PULMCH
	
*PUL MATCHBUF ONLY FROM STACK
*FRAME
	
*USES A1
	
	MOVE.L  (A7)+,RELSTK(A4)
	LEA     MATCHBUF(A4),A1
10$
	MOVE.L  (A7)+,(A1)+
	BPL.S   10$
	MOVE.L  RELSTK(A4),-(A7)
	RET
	
*--------------------------------
	
PULREST
	
*PUL MAIN BODY OF STACK FRAME
*EXCEPT TXT PNTR
	
*USES A1
	
	MOVE.L  (A7)+,RELSTK(A4)
	LEA     TEMPBUF(A4),A1
10$
	MOVE.W  (A7)+,(A1)+
	BNE.S   10$
	MOVE.W  (A7)+,WORD_(A4)
	MOVE.W  (A7)+,SECTION_(A4)               ;RETURN CHOSEN NOUN
	MOVE.W  (A7)+,NOUN(A4)                   ;AND GET INDEX
	MOVE.W  (A7)+,TYPE(A4)
	MOVE.L  (A7)+,CMDPNT(A4)
	MOVE.L  (A7)+,POSWRD(A4)
	MOVE.L  (A7)+,TMPPNTR(A4)
	MOVE.W  (A7)+,POSSESS(A4)
	MOVE.L  (A7)+,DATIVE(A4)
	MOVE.W  (A7)+,GNGTEMP(A4)
	MOVE.W  (A7)+,D3
	MOVE.L  RELSTK(A4),-(A7)
	RET
	
*--------------------------------
	
PULFAIL
	
*PUL ENTIRE STACK FRAME
*PRESERVE TXT PTR
	
	MOVE.L  (A7)+,PULFAIL9(A4)
	CALL    PULMCH
	CALL    PULREST
	MOVE.L  (A7)+,A6                ;RET OLD TXT PNTR
	MOVE.L  PULFAIL9(A4),-(A7)
	RET
	
*--------------------------------
*
* SRC.ADV.RELATIVE2
*
*--------------------------------
 
 
RELPH
	
*PROCESS RELATIVE CLAUSE
*USES D4,A1,A2,D0,A0,D1
*EQ= PHRASE FAIL
*NE= PHRASE PASS
	
	DO	CLRADJ   
	CALL    PUSH                     ;SAVE PARSE STATE

	LEA     MATCHBUF(A4),A1          ;find last set of matches
	LEA     RelBuf(A4),A3            ;get buffer for nouns 'in play'
	ADD.W   RelPnt(A4),A3            ;and our last known position in it
	MOVE.W  RelPnt(A4),RelPnt2(A4)   ;setup our tempory offset
10$
	MOVE.L  (A1)+,D0                 ;Get section and word
	BMI.S   20$                      ;end of buffer
	CMP.W   #SECT.NOUN,-4(A1)        ;only want to know about knows
	BNE.S   10$
	
	MOVE.W  D0,(A3)+                 ;save non-adjusted noun #
	ADDQ.W  #2,RelPnt2(A4)           ;remember new position in case we do
	                                 ;go deeper
	BRA.S   10$
20$

	TEST_W  ERR1(A4)                ;ERROR SITUATION?
REL1
	BEQ.S   03$                     ; ERROR
	DO	GETWRD	                 ;NEXT, FOR RELATIVISER
	CALL    RELWRD
	BRA.S   02$
03$
	MOVE.W  ERR4(A4),-(A7)
	DO	NOERR
	DO	GETWRD
	CALL    RELWRD
	MOVE.W  (A7)+,ERR4(A4)
	DO	YESERR
02$
	TEST_W  D7
	BNE     80$                     ;FAIL
	
	CMP.B   #SECT.CONJ,D6
	BNE     80$                     ;FAIL
	CMP.B   #2,D5                   ;FROM?
	BNE.S   10$
	MOVEQ   #$21,D4                 ;MASK AS BOTH
	BRA.S   20$
10$
	CMP.B   #3,D5                   ;ON?
	BEQ.S   12$
	CMP.B   #$9,D5                  ;UNDER?
	BNE.S   11$                     ;NO
12$
	MOVEQ   #1,D4
	BRA.S   20$
11$
	CMP.B   #5,D5                   ;IN
	BNE     80$                     ;SORRY, FAIL
	MOVEQ   #$20,D4
20$
	MOVE.W  D4,-(A7)                ;SAVE RELMASK

	LEA     RelBuf(A4),A3           ;maintain nouns stack
	ADD.W   RelPnt2(A4),A3
	OR.W    #$FF00,D4               ;flag end of one 'lump' of nouns
	MOVE.W  D4,(A3)                 ;and save relationship info
	ADDQ.W  #2,RelPnt2(A4)
	MOVE.W  RelPnt2(A4),RelPnt(A4)  ;so as next time through we know where
	                                ;we are

* RelPnt only gets updated here, IE if we REALLY DO try to process a RELative PHrase
* this way, if there is any lack of adjective hassle we can look back at the
* previous level of nouns (ie the related objects) and try to resolve the
* problem - ps the non-recursive entry to MNPH zeros RelPnt and RelPnt2
* pps and DEPTH - but that's another story...

	CALL	MNPHREC                 ;ENTRY POINT FOR RECURSION

	LEA     RelBuf(A4),A3           ;get RelBuf 'noun stack' pointer
	SUBQ.W  #2,RelPnt(A4)           ;adjust for terminator
	ADD.W   RelPnt(A4),A3
205$
	SUBQ.W  #2,RelPnt(A4)           ;remove the nouns we just stacked in
	TEST_W  -(A3)                   ;RelBuf
	BPL.S   205$

	ADDQ.W  #2,RelPnt(A4)           ;move past the terminator we just found

	CMP.B   #1,D7                   ;EOL
	BEQ.S   21$                     ;OK
	MOVE.L  POSWRD(A4),A6
	SUBQ.L  #1,A6                   ;ADJUST
21$
	MOVE.W  (A7)+,D4                ;RET MASK
	CMP.B   #2,D7                   ;ERROR TYPE?
	BEQ     80$                     ;YES, FALL THROUGH
	CALL    PULMCH                  ;PUL BACK OLD MATCHBUF
	LEA     MATCHBUF(A4),A1
25$
	MOVE.L  (A1)+,D0                ;GET AN ENTRY
	BMI.S   30$                     ;DONE
	SWAP    D0
	CMP.W   #SECT.NOUN,D0           ;ONLY APPLIES TO NOUNS
	BNE.S   25$
	SWAP    D0                      ;BACK
	ADDQ.W  #1,D0                   ;ADJ
	GETINFO                         ;ON THIS NOUN
	MOVE.B  6(A0),D1                ;GET RELLOC
	BTST	#7,D1			;out?
	BNE.S	25$			;yes, throw away.
	AND.B   D4,D1                   ;WI MASK
	BEQ.S   25$                     ;TOO BAD
	LEA     TEMPBUF(A4),A2          ;WE GOT ONE
26$
	MOVE.W  (A2)+,D1                ;GET SECOND NOUNS
	BEQ.S   25$                     ;IF AL, TRY NEXT
	CMP.W   8(A0),D1                ;IS REL TO THIS NOUN?
	BNE.S   26$                     ;NO, ANY MORE?
	BSET    #7,-2(A1)               ;YES!!, FLAG AS OK
	BRA.S   25$                     ;ALOW MORE IF MAKES SENSE.

30$
	CALL    PULREST                 ;GET OLD TEMPBUF & INDEX
	CALL    ISTN                    ;TO D0
	MOVE.W  D0,EXAMPLE(A4)          ;INCASE ERROR PROCESSING

	CLR.W   D4                      ;FLAG AS ANY OR NOT
	LEA     MATCHBUF(A4),A1
	MOVE.L  A1,A0                   ;TARGET
40$
	MOVE.L  (A1)+,D0                ;GET MATCHBUF AGAIN....
	BMI.S   50$                     ;DONE
	TEST_W  D0                      ;FLGGED?
	BPL.S   40$                     ;NO, ANY??
	ADDQ.B  #1,D4                   ;INC
	BCLR    #15,D0                  ;NO HI-BIT
	MOVE.W  D0,D5
	MOVE.L  D0,(A0)+                ;INTO BUFFER
	BRA.S   40$                     ;NEXT
50$
	MOVE.W  #-1,(A0)                ;TERMINATE
	MOVEQ   #SECT.NOUN,D6 
	TEST_W  D4                      ;DID RELPH HAPPEN?
	BEQ.S   60$                     ;NO
	MOVE.W  D5,WORD_(A4)
	MOVE.W  D6,SECTION_(A4)
52$
	MOVE.L  (A7)+,A1                ;DUMP TXT
	BRA.S   90$
60$
	MOVE.W  D6,(A0)                 ;IF FAIL, UNTERMINATE MATCHBUF (NB IT MUST
	                                ;              BE A NOUN SO THIS IS OK)
	CALL    IS2PART                 ;CAN'T TAKE A PREP PHRASE?
	MOVE.L  (A7)+,A6                ;TOO BAD, GET BACK OLD TEXT
90$
	TEST_W  D4                      ;SET FLAGS
	RET
	
80$
	CALL    PULFAIL                 ;GET THE LOT
	CLR.W   D4
	BRA.S   90$                     ;SET EQ & EXIT
	
*--------------------------------
	
RELWRD
	
*PROCESS RELATIVE PHRASE STARTING
*WITH:
* THAT,WHO,WHAT,WHICH IS
	
*ENTRY:
*D5,D6 ALREADY SET-UP
	
	CMP.B   #SECT.REL,D6
	BEQ.S   10$                              ;YES, DEFINITE
	CMP.B   #SECT.QUEST,D6
	BNE.S   90$                              ;NO, FAIL
	CMP.B   #1,D5                            ;WHO
	BNE     90$
10$
	CMP.B   #2,TYPE(A4)                      ;APOSTROPHE?
	BNE.S   11$
	CMP.B   #'S',TYPE+1(A4)                  ;'S?
	BEQ     GETWRD
11$
	DO	GETWRD	                          ;NEXT?
	CMP.B   #SECT.TOBE,D6
	BEQ     GETWRD
	CMP.B   #2,D7                            ;ERROR?
	BEQ.S   90$                              ;EXIT
	BRA     P.IDFY
90$
	RET
	        
	
	
	
*--------------------------------
	
IS2PART
	
*IF THE VERB IS 2PART ONLY
*ANY RELPH MUST BE A RELPH
* EG
* LOOK (BOTTLE IN CART)
* IS NEVER...
	
* LOOK (BOTTLE) IN CART
	
*EXIT:
* EQ= IS2PART ONLY
* NE= NO, CAN BE 4PART, FAIL
	
	
	LEA     VLST2.P(A4),A1
10$
	MOVE.B  (A1)+,D1                ;GET ENTRY
	CMP.B   #-1,D1                  ;TERM?
	BEQ.S   80$                     ;FAIL, NOT IN LIST
	CMP.B   VERB+1(A4),D1
	BNE.S   10$                     ;TOO BAD
90$
	TEST_W  ERR1(A4)                ;ERROR OFF?
	BNE.S   91$                     ;YES
      
	MSG	BUT
	MSG	THERE
	MOVE.W  EXAMPLE(A4),D0
	GETINFO
	DO	ISAREN
	CALL    P.NONLY                 ;ANY/A NOUN
	MSG	THERE
91$
	MOVEQ   #2,D7                   ;EROR PRINTED
	SETNE                           ;FAIL
80$
	RET

*--------------------------------
	
ISTN
	
*EXTRACT THE 1ST NOUN FROM MATCHBUF
	
*EXIT:
*NOUN IN D0 (ADJUSTED)
	
	LEA     MATCHBUF(A4),A1
10$
	MOVE.L  (A1)+,D0
	BMI.S   90$                              ;WOOPS
	SWAP    D0
	CMP.B   #SECT.NOUN,D0                    ;A NOUN?
	BNE.S   10$                              ;TOO BAD
	SWAP    D0
	ADDQ.W  #1,D0                            ;ADJUST
	RET                                      ;OK
90$
	BRA     DERR                             ;DATA ERROR

*--------------------------------


	END

