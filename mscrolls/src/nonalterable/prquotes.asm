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
*	@(#)prquotes.asm	2.14
*	7/7/87
*
******************************************************

		SECTION		"ADV",CODE

*-------------------------------
*
*  SRC.ADV.PR.QUOTES1
* 
*--------------------------------
   
	include	"equates.asm"
	include	"verbequ.asm"
	include	"nounequ2.i"    ;JUST FOR NOUNNUM
	include	"macros.asm"


	XREF   W.IYAUPT,P.SUB,SCAN.SUB,SCANVERB,LINE,LASTA0
	XREF   FLUSHVAR,NOUNBUF,GETDATA,DECODE,CHKNPC,CONVNPC,NPC.LBF1
	XREF   GETNPCNO,SAY.HELL,DOYESNO,PARSEIT,NPCQUEST,NPC.TOBE,STOPRON
	XREF   NPCMODEL,POSWRD,SCANNER,W.PLUSE,W.MAD,DEC.N,W.1ATTIM
	XREF   P.SAY,PRSPCMD,W.DEF0,W.MB,NPCABOUT,CHARTSET,P.MSG,P.PRN
	XREF   GETCMSG,W.HYF,W.YURMIS,W.ALWAYS,W.IAM,RND.D1
	XREF   L.DATA2,CHAROUT,ZAP,TYPE,SCREEN,PR.SPEC,VALID,GETWRD
	XREF   PERSON,NOUNBUF2,W.WHAT,W.ABOUT,P.OBJPRN,W.ICDOAN,PRTTXT
	XREF   W.SLIM,W.SHUTUP,IUFLAG,SCAN.OBJ,W.CANCOP,W.IDNATA,W.DWAM
	XREF   W.ONLY,CONJ,RNDGEN,NEG_,P.KYWRD,W.THOUGH,INTERROR,W.EH
	XREF   W.TRICKQ,W.NCONCE,W.SAYTO,W.IMPOLI,W.TNNICE,W.KNOWNO,W.IDNK
	XREF   W.HRD2BE,W.COP6,P.ANOUN,W.COP15,W.COP12,W.FALSEI,GETNPC
	XREF   W.TRUEI,ISARE,W.WORTH,P.XFERGS,W.WLESS,P.TN,ADDNOUN,GETVECT
	XREF   W.PROBAB,W.OFANY,P.NOUN,W.ME,DBS,W.PLESS,SCANSUB,TOPNPC
	XREF	  VALID.NP,BACK.WRD


	XDEF   QUOTES,PR.BACK,PR.INVQ,P.VAGE
	XDEF   CAN.YOU,RNDNOUN,SCAN.REP,NOTNICE
	XDEF   PR.WANT,PR.HMANY
	XDEF   PR.HMUCH,RNDCOP,P.NODO,PR20


QUOTES

	CALL    FLUSHVAR
	LEA     NOUNBUF(A4),A0
	MOVE.W  (A0)+,D0
	BEQ     QU30
	TEST_W  (A0)
	BNE     QU40
	GETINFO
	BTST    #6,4(A0)
	BEQ     QU50
	MOVEQ   #0,D5              ;LET ERROR'S HAPPEN
	DO	VALID              ;IS NPC IN ROOM?
	BNE     DECODE             ;NOPE!
	PEA     DECODE(PC)
	MOVE.L  A0,A5   
	MOVE.W  D0,D5   
	DO	CHKNPC             ;STACK UP
	MOVE.L  (A7)+,D1           ;DUMP
	CLR.W   TYPE(A4)
	MOVE.W  D5,CONVNPC+4(A4)
	MOVE.L  A5,CONVNPC(A4)     ;TEMPORY STOARAGE
	DO	GETWRD

* to avoid 'say' in mill saying - 'where do u want to lie?' - Ken 11/3/87

	CMP.B	#1,D7			;we have some text?
	BEQ	QU60			;EQ => nope - complain

	CMP.W   #SECT.HELLO,D6             ;HELLO?
	BNE.S   20$                ;NOPE
	MOVE.W  CONVNPC+4(A4),D0
	CALL    GETNPCNO           ;TO D4
	EXG     D4,D0
	CALL    SAY.HELL          ;GREET BACK
	DOGO	PR20
20$
	CMP.W   #SECT.YESNO,D6
	BEQ     DOYESNO
	CMP.W   #SECT.VERB,D6
	BEQ     PR.COMMAND
	CMP.B   #SECT.DIR,D6
	BEQ     PR.COMMAND
	CMP.W	#SECT.SPEC,D6	;suss "shout, lower drawbridge" hassle
	BNE.S	30$
	TEST_W	D5			;and?
	BEQ.S	40$
30$
	CMP.W   #SECT.CONJ,D6
	BNE.S   TRYPNT                ;NO
	CMP.W   #1,D5              ;TO (DO SOMETHING)
	BNE.S   TRYPNT                ;NO
40$
	DO	GETWRD	            ;TO START OF COMMAND
	BRA     PR.COMMAND
TRYPNT
	CALL    PARSEIT    
	TEST_W  D7
	BNE.S   10$
	TEST_W  NPCQUEST(A4)
	BPL     PR.QUESTION
	TEST_W  NPC.TOBE(A4)        ;MAIN VERB
	BPL.S   PR.INVQ
	TEST_W  NPCMODEL(A4)
	BPL     PR.QUESTION
10$
	CMP.W   #2,D7               ;DONE?
	BEQ     PR20                ;EXIT
*	MOVE.L  POSWRD(A4),A6		
	CALL    BACK.WRD		;i think this should be this
	CALL    SCANNER
	CMP.B   #3,D7               ;SPECIAL EXIT CONDITION?
	BEQ.S   TRYPNT              ;TRY
	DOGO	PR20
QU30
	MSG     PLUSE		;please use 'say top someone...
	BRA.S   PR.BACK
QU40
	MSG     1ATTIM
	BRA.S   PR.BACK
QU50
	MSG     MAD 
	DOGO	PR20

QU60
	MSG	PLUSE
	DOGO	PR20

*--------------------------------

PR.BACK

	BRA     DEC.N              ;BACK TO PARSER (DECODE)

*--------------------------------

PR.COMMAND

	CMP.W   #VNINVENTORY,D5
	BNE.S   10$
	CALL	BACK.WRD	;adjustment (i hope)
	CALL    SCANNER
	CMP.B   #3,D7		;special case
	BEQ     TRYPNT		;try again
	DOGO	PR20
10$
      IFNE	THE_PAWN

	DO	P.SAY
      
      ENDC

	CALL    PRSPCMD        ;4 BYTES = NOT NEGATIVE!!!!
	MSG     DEF0           ;DEFINITELY NOT
	DOGO	PR20

*--------------------------------

PR.INVQ

	DO	P.SAY
	MOVEQ   #2,D1            ;RANDOM
	RND_D1	          ;SPIN
	TEST_W  D1
	BNE.S   10$              ;NO
	MSG     MB    
	DOGO	PR20
10$
	DO	P.VAGE
	DOGO	PR20
	
*--------------------------------
	
P.VAGE

	MOVEQ   #5,D1     
	RND_D1	               ;SPIN

	BeSure	M.PERHAPS

	ADD.W   #M.PERHAPS,D1
	DOGO	P.MSG

*--------------------------------

PR.QUESTION

* PROCESS WHERE,WHO,WHAT,WHEN,WHY,WHICH
	
	MOVE.W  NPC.LBF1(A4),STOPRON(A4)
	
	MOVE.W  NPCQUEST(A4),D0
	BMI.S   01$
	
	
	CMP.W   #5,D0                    ;HOW???
	BNE.S   20$
	MOVE.B  NPCABOUT(A4),D1          ;FLAGS
	BEQ     PR.HOW
	CMP.B   #1,D1                    ;ABOUT?
	BEQ     PR.HOWAB
	BTST    #2,D1                    ;MANY?
	BNE     PR.HMANY
	BTST    #1,D1                    ;MUCH?
	BNE     PR.HMUCH
	BRA     INTERROR                 ;HUH?
20$
	TEST_W  PERSON(A4)
	BNE.S   01$                     ;SKIP
	
	CALL    CHARTSET                ;SPECIAL?
	TEST_W  D7
	BEQ.S   10$                     ;YEP, MSG# IN D0
01$
	MOVE.W  PERSON(A4),D2
	CALL    PR.SPEC
	CALL    GETCMSG
	TEST_W  D0                      ;NULL MESSAGE?
	BEQ     GOTOASK                 ;MUST BE ASK ABOUT..
10$
	DO	P.SAY

	BeSure	M.NOIDEA

	CMP.W   #M.NOIDEA,D0
	BNE.S   11$
	CALL    PR.DOWHERE
11$
	MOVE.W  D0,D1                   ;INTO D1
	BCLR    #15,D1
	DO	P.MSG	                  ;PRINT MESSAGE
	TEST_W  D0
	BPL     PR20                    ;Exit
	MOVE.W  (A0)+,D0
	BRA.S   11$

*--------------------------------
	
PR.DOWHERE
	
	MOVE.W  D0,-(A7)
	MOVE.W  NPC.LBF1(A4),D0
	BEQ.S   90$                    ;FAIL IF NONE FOUND
	  MOVE.L  D5,-(A7)
	DO	VALID.NP	               ;LOC?
	  MOVE.L  (A7)+,D5
	TEST_W  D7
	BNE.S   90$                    ;NO FAIL
	DO	P.PRN
	DO	ISARE
	MSG     HYF                  ;HERE YOU FOOL
	MOVE.W  (A7)+,D0
	MOVE.L  (A7)+,D0               ;DUMP
	DOGO	PR20
90$
	MOVE.W  (A7)+,D0
	RET

*--------------------------------
   
PR.HOW
	DO	P.SAY
	PEA     PR20(PC)
	MOVE.W  NPC.LBF1(A4),D3
	MOVE.W  PERSON(A4),D1
	MOVE.W  CONVNPC+4(A4),D5
	MOVE.W  NPC.TOBE(A4),D2
	TEST_W  NPCMODEL(A4)
	BPL     PR.HOWQ
	TEST_W  D2
	BMI     PR.HOWCOP             ;NO-TOBE
	TEST_W  D2                    ;AM?
	BNE.S   11$                   ;NOT AM
	TEST_W  D1                    ;I?
	BEQ.S	10$
	MSGGO	EH
10$
	TEST_W  D3
	BNE.S   12$                   ;MISTAKE
	MOVEQ   #2,D1                 ;INDEX
	RND_D1	               ;RANDOM

	BeSure	M.FORYOU

	ADD.W   #M.FORYOU,D1
	DOGO	P.MSG
12$
	MSGGO	YURMIS
11$ 
	CMP.W   #1,D2                 ;ARE?
	BNE.S   13$                   ;'IS'
	CMP.B   #1,D1                 ;YOU?
	BNE     PR.HOWCOP             ;COPOUT
	TEST_W  D3
	BEQ.S   14$                   ;HOW ARE YOU?
	CMP.W   D5,D3
	BNE     PR.HOWCOP
	MSGGO	ALWAYS
14$
	MOVE.W  D5,D0
	DO	GETNPC
	MOVE.B  2(A3),D2
	LSR.B   #4,D2
	BEQ.S   15$                  ;NORMAL
	MSG     IAM                ;I AM
	LEA     L.DATA2(A4),A1
	SUBQ.B  #1,D2
16$
	MOVE.B  (A1)+,D1
	BNE.S   16$
	SUBQ.B  #1,D2
	BNE.S   16$
17$
	MOVE.B  (A1)+,D1
	BEQ.S   18$
	OR.B    #$20,D1
	DO	CHAROUT
	BRA.S   17$
18$
	RET
15$
	MOVEQ   #2,D1
	RND_D1

	BeSure	M.IMFINE

	ADD.W   #M.IMFINE,D1
	DOGO	P.MSG
13$
	CMP.W   #2,D2                ;'IS'
	BNE     PR.HOWCOP
	MOVEQ   #2,D1
	RND_D1

	BeSure	M.IDBELYU

	ADD.W   #M.IDBELYU,D1
	DOGO	P.MSG

*--------------------------------
	
	XREf	QuoteFlag

PR20

	BCLR	#0,QuoteFlag(A4)
	BEQ.S	10$
	MSG	QUOTE
10$
	ST      ZAP(A4)
	DO	GETWRD
	CLR.W   ZAP(A4)
	CMP.W   #1,TYPE(A4)
	BEQ     PR.BACK
	TEST_B  D7
	BEQ.S   PR20
	MOVE.L  (A7)+,D0
	DOGO	SCREEN

*--------------------------------
	
GOTOASK
	
      
*SET UP VARIABLES THEN MOVE TO ASK
	
	MOVE.W  CONVNPC+4(A4),D0   ;MAIN NOUN
	CMP.W   #1,PERSON(A4)      ;YOU?
	BEQ.S   10$                ;WHO ARE YOU!
	GETINFO                    ;ON D0
	LEA     NPC.LBF1(A4),A1
	MOVE.W  (A1),D5            ;ABOUT D5
	MOVE.W  D5,NOUNBUF2(A4)    ;KEEP ASK HAPPY
	MOVE.W  #VNASK,D2          ;VERB#
	DO	GETVECT            ;TO A1
	JSR     (A1)               ;DO IT!
	DOGO	PR20               ;EXIT
10$
	DO	P.SAY
	MSG     COP6             ;HELLO, I AM..
	GETINFO
	DO	P.TN
	DOGO	PR20 
 
*--------------------------------


*--------------------------------
*
*  SRC.ADV.PR.QUOTES2
* 
*--------------------------------
	
	
PR.HOWAB

	PEA     PR20(PC)
	MOVE.W  NOUNBUF(A4),D0       ;ANY FOUND?
	BEQ.S	10$
	MSG     WHAT
	MSG     ABOUT
	GETINFO
	DOGO	P.OBJPRN             ;WHAT ABOUT IT
10$
	MSGGO	IDNK
	
*--------------------------------
	
PR.HOWQ
	
	MOVE.W  NPCMODEL(A4),D5
	CMP.W   #1,D1                ;PERSON?
	BHI     PR.HOWCOP            ;COPOUT
	
	CMP.W   #$1,D5               ;CAN?
	BEQ.S   PR.HOWCAN
 
	CMP.W   #$4,D5               ;SHALL
	BEQ.S   PR.HOWCAN
	CMP.W   #$2,D5               ;MAY
	BEQ.S   PR.HOWMAY
	CMP.W   #$3,D5               ;WILL
	BEQ.S   PR.HOWWILL
	CMP.W   #$4,D5               ;SHOULD
	BEQ.S   PR.HOWSHOULD
	PRINT   <'WHAT ARE YOU TALKING ABOUT?'>
	RET
	
*--------------------------------
	
	
PR.HOWCAN
	
	TEST_W  D1
	BNE.S	10$
	MOVEQ   #4,D1
	RND_D1

	BeSure	M.USEBRAIN

	ADD.W   #M.USEBRAIN,D1
	DOGO	P.MSG
10$
	MSGGO	ICDOAN

*--------------------------------
	
PR.HOWMAY
	
	
	TEST_W  D1
	
	BNE.S   PR.HOWCOP
	MSGGO	SLIM
	
	
*--------------------------------
	
	
PR.HOWWILL
	       
	
	TEST_W  D1
	BNE.S	10$
	MOVEQ   #3,D1
	RND_D1

	BeSure	M.DONTWRY

	ADD.W   #M.DONTWRY,D1
	DOGO	P.MSG
10$
	MSGGO	DWAM

*--------------------------------
	
	
PR.HOWSHOULD
	       
	
	TEST_W  D1
	BEQ.S	10$
	MOVEQ   #3,D1
	RND_D1

	BeSure	M.DEV

	ADD.W   #M.DEV,D1
	DOGO	P.MSG
10$
	MSGGO	SHUTUP
	
	
*--------------------------------
	
PR.HOWCOP
	
	MOVEQ   #7,D1
	RND_D1

	BeSure	M.IHNI

	ADD.W   #M.IHNI,D1
	DOGO	P.MSG
   
*--------------------------------
;       
;PR.CAN
;     
;      CALL    SCANSUB
;      MOVE.B  IUFLAG(A4),D1
;      MOVE.W  SCAN.SUB(A4),D0
;      BNE.S   10$
;      MOVE.W  SCAN.OBJ(A4),D0
;      BNE.S   10$
;      OR.B    #3,D1
;      BEQ     W.CANCOP
;0$
;      BTST    #1,D1
;      BNE.S   CAN.SUBJ
;      BTST    #0,D1
;      BNE.S   CAN.YOU
;      MSG     IDNATA
;      MSG     ONLY
;      GETINFO
;      MOVE.W  D0,D5
;      MOVE.W  A0,A5
;      MOVE.W  CONVNPC(A4),A0
;      MOVE.W  CONVNPC+4(A4),D0
;      GETINFO
;      MOVEQ   #VNASK,D2
;      DO	GETVECT
;      MOVE.W  #$12,CONJ(A4)
;      MOVE.W  D5,ADDNOUN(A4)
;      JSR     (A1)
;      DOGO	PR20
;              
;      
;--------------------------------
;      
;      
;CAN.SUBJ
;      
;      
;      MSG     IYAUPT
;      DOGO	PR20
;      
;      
*--------------------------------
	
	
CAN.YOU
	       
	
	MSG     ICDOAN
	DOGO	PR20
 
	
   
RNDNOUN

	CALL    RNDGEN             ;THIS LOOKS
	AND.W   #$7F,D0            ; *VERY* DODGY!
	CMP.W   #NOUNNUM,D0        ;HMMMM
	BHI.S   RNDNOUN            ;HMMMM!!!
	RET
  
*--------------------------------
   
INSULTSUB
	
	
	TEST_W  NEG_(A4)
	BEQ.S	10$
	MSGGO	THOUGH
10$
	MOVEQ   #3,D1
	RND_D1
	CMP.B   #2,D1
	BEQ.S   20$
	MOVE.W  D1,D2
	CALL    P.KYWRD
	MOVE.W  D2,D1

	BeSure	M.COP7

	ADD.W   #M.COP7,D1
 
	DOGO	P.MSG
20$
	DO	P.SUB
	BRA     P.KYWRD

*--------------------------------
   
SCAN.REP
   
* ASSUME,
* D5,A5 = NPC TALKED TO
   
	MOVE.L  CONVNPC(A4),A5
	MOVE.W  CONVNPC+4(A4),D5
	TEST_W  NPCQUEST(A4)
	BMI.S   02$
	DO	P.SAY
	MSGGO	TRICKQ
02$
	MOVE.B  IUFLAG(A4),D2
	BTST    #1,D2              ;I
	BNE     SUBJECTIVE
	BTST    #0,D2              ;YOU
	BNE     OBJECTIVE
   
* GENERAL STATEMENT
   
	DO	P.SAY
	CMP.W   SCAN.SUB(A4),D5    ;TALKING ABOUT OTHER PERSON
	BNE.S   11$
	BTST    #4,D2
	BNE     INSULTSUB
	MSGGO	NCONCE
11$
	MOVE.W  SCAN.SUB(A4),D0
	BEQ.S   15$                ;NOONE
	GETINFO
	BTST    #6,4(A0)
	BEQ.S   PR.COPOUT
	DO	VALID              ;ABOUT SOMEONE IN ROOM
	BNE.S   12$                ;NO
	MSG     SAYTO
	MOVE.W  D5,D0
	MOVE.L  A5,A0
	DOGO	P.OBJPRN
12$
	BTST    #4,D2
	BNE.S   NOTNICE
	MSGGO	IMPOLI
15$
	BTST    #4,D2              ;INSULT?
	BEQ     PR.COPOUT          ;NO

	MSGGO	PARDON
	
*--------------------------------
   
NOTNICE
 
	MSG     TNNICE
	BRA     INSULTSUB
 
*--------------------------------


PR.COPOUT
 
	CALL    RNDCOP             ;INCASE
	TEST_W  D7                 ;WEL?    
	BNE.S   90$                ;EXIT  
	MOVEQ   #5,D1
	RND_D1

	BeSure	M.COP0

	ADD.W   #M.COP0,D1
	DO	P.MSG
90$
	RET


*--------------------------------
*
*  SRC.ADV.PR.QUOTES3
*
*--------------------------------
 
   
SUBJECTIVE
	
	DO	P.SAY
	BTST    #2,D2              ;WANT?
	BNE.S   PR.WANT
	BTST    #3,D2              ;THINK?
	BNE     PR15 
	BTST    #5,D2
	BEQ.S   10$                ;NOT 'AM'
	TEST_W  NEG_(A4)
	BEQ.S	1001$
	MSGGO	KNOWNO           ;AM NOT
1001$
	TEST_W  SCAN.SUB(A4)
	BEQ.S	1002$
	MSGGO	HRD2BE           ;AM #N
1002$
	TEST_W  SCANVERB(A4)       ;GOING TO DO SOMETHING?
	BEQ.S   09$
	MSGGO	DBSURE		;DONT BE SO SURE OF THAT!
09$
	MOVE.W  D5,D0
	MSG     COP6             ;HELLO I AM
	GETINFO
	BRA     P.ANOUN
10$
	BTST    #4,D2
	BNE.S   PR11
	BRA     PR.COPOUT
 
*--------------------------------
	
PR.WANT
	
	BTST    #3,D2              ;THINK AS WEL
	BEQ.S	10$
	MSGGO	COP15            ;WISHFUL THINKING HUH!
10$ 
	MOVEQ   #3,D1
	RND_D1

	BeSure	M.REALWT

	ADD.W   #M.REALWT,D1
	DOGO	P.MSG
PR11
	MSGGO	COP12
  
*--------------------------------
  
  
PR15
	BTST    #4,D2
	BNE     INSULTSUB
	MOVEQ   #2,D1
	RND_D1
	TEST_W  NEG_(A4)
	BNE.S   PR16

	BeSure	M.COP15

	ADD.W   #M.COP15,D1
	BRA.S   PR17   
PR16

	BeSure	M.COP17

	ADD.W   #M.COP17,D1
PR17
	DOGO	P.MSG

*--------------------------------
   
OBJECTIVE
	
	DO	P.SAY
	BTST    #5,D2              ;AM
	BEQ.S   60$
	BTST    #4,D2
	BNE     INSULTSUB
	TEST_W  NEG_(A4)
	BNE.S   50$                ;ARE NOT
	TEST_W  SCAN.SUB(A4)
	BNE.S   10$
51$
	BTST    #4,D2
	BNE     INSULTSUB
	MSG     FALSEI
	BRA.S   IAM
10$
	CMP.W   SCAN.SUB(A4),D5
	BNE.S   51$
	MOVEQ   #1,D1
	RND_D1

	BeSure	M.COP9

	ADD.W   #M.COP9,D1
	BRA.S   PR17   
50$
	CMP.W   SCAN.SUB(A4),D5
	BEQ.S   51$
	MSG     TRUEI
	BRA.S   IAM
60$
	BTST    #4,D2
	BEQ     PR.COPOUT
	BRA     INSULTSUB
IAM
	MOVE.W  D5,D0
	BRA     P.ANOUN

*--------------------------------
	       
	       
PR.HMANY
	       
	DO	P.SAY
	MSG     IDNK
	DOGO	PR20
	
*--------------------------------
	
PR.HMUCH
	
	CLR.W   CONJ(A4)           ;CLEAR TO FIX ALL PROCESSING
	DO	P.SAY
	MOVE.W  CONVNPC+4(A4),D5
	LEA     NPC.LBF1(A4),A1
	TEST_W  (A1)
	BEQ     80$                ;NONE SPEC.
10$
	MOVE.W  (A1)+,D0           ;GET ONE
	BEQ     90$                ;DONE
	GETINFO                    ;TO A0
	BTST    #6,4(A0)           ;NPC?
	BNE     50$                ;VALUE NPC?

	MOVE.L	D5,-(A7)
	DO	VALID.NP		;if not local will deny knowledge
	MOVE.L	(A7)+,D5		;(D5 just happens to be 0!!! - no err)

*       CMP.W   LASTA0+4(A4),D5    ;TO NPC?

	CALL    TOPNPC             ;find owner of object in D0 returns noun# in
	CMP.W   D4,D5              ;D4 - is this the owner -no=> worthless

	BNE.S   20$                ;NO
15$
	DO	P.TN
	DO	ISARE
      
      IFNE	THE_PAWN
	MOVE.B  4(A0),D1           ;GET VALUE
	AND.B   #$7,D1             ;LOW 3 BITS
      ENDC

      IFEQ	THE_PAWN
	XREF	GetValue
	CALL	GetValue
      ENDC

	BEQ.S	17$			;zero value - not for sale
	MSG     WORTH
	CALL    P.XFERGS
	BRA.S   60$                ;NEXT
17$
	PRINT	<'NOT FOR SALE.^'>
	BRA.S	60$

20$
	TEST_W  D7
	BNE.S   40$                ;NO
	BTST    #3,4(A0)           ;ONLY OF MONEY.
	BNE.S   15$                ;DOIT
	
	DO	P.TN
	DO	ISARE
	MSG     PROBAB
	MSG     WLESS
	BRA.S   60$                ;NEXT
40$
	MSG     IDNK		;not local => i dont know of any xyzzy
	MSG     OFANY
	DO	P.NOUN
	BRA.S   60$
	
50$
	CMP.W   D0,D5              ;ASKING HIMSELF?
	BNE.S   51$  ;NO
	MSG     ME
	DO	DBS
	BRA.S   60$
51$
	DO	P.TN		;print 'the NPC'
	DO	ISARE		;is/are
	MSG     PLESS		;priceless
   
60$
	TEST_W  2(A0)               ;NEXT?
	BEQ     10$                 ;NO, OK
	MSG    LINE
	BRA     10$                 ;ANOTHER
	
80$
	MSG     IDNK
90$
	DOGO	PR20                ;EXIT

*--------------------------------
	
	
RNDCOP 
	
*GENERATE A RANDOM COMMENT
	
	
	CLR.W   D7
	MOVEQ   #$7F,D1            ;LIKELY HOOD
	RND_D1	            ;RANDOMIZE
	CMP.B   #2,D1              ;UPPER LIMIT
	BHI.S   90$                ;NO RANDOM COMMENT

	BeSure	M.OUTWEIRD

	ADD.W   #M.OUTWEIRD,D1     ;START OF TABLE
	DO	P.MSG	             ;PRINT IT
	MOVEQ   #1,D7  
90$
	RET

*--------------------------------
	
P.NODO
	
*MSG WHEN CANT DO SOMETHING
	
	MOVEQ   #6,D1
	RND_D1

	BeSure	M.NODO1
	
	ADD.W   #M.NODO1,D1
	DOGO	P.MSG
	
*--------------------------------

	END
