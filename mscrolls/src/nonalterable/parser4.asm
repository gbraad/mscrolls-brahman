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
*	@(#)parser4.asm	2.13
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
* 
*    SRC.ADV.PARSER4
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"

      XDEF     DIRECT,DIRNPC,CONVSE.V,SPEECH,SPIEL,INTERROR,FORWHAT,BACK.WRD
      XDEF     DODATIVE,TWOPV,PICKONE

      XREF     GETDATA,TALKME,W.JCHKG,W.WISLI,DECODE,W.TALKME,INCNOUN,NOERR
      XREF     NPCINRM,WRDPNT,GETWRD,VERB,GETADJPH,MNPH,NOUN,TEMPBUF,W.1ATTIM
      XREF     W.MAD,VALID,IFANIMAL,YESERR,ERR4,GLOBAL,NOTWSYN,QUOTES
      XREF     W.NOONE,SCREEN,W.ERROR,W.ONEOF,P.TN,W.SAYS,W.IDNK,W.GOTNON
      XREF     STOPRON,TOPNPC,W.BIDHIT,P.NQEN,P.MSG,W.NOCHAN,W.NO,P.PRN
      XREF     ISARE,W.MINE,W.NONEED,SUBJECT,POSWRD,INBUFF,DATIVE,NOUNBUF2
      XREF     NOUNBUF,MOVBUF,CONJ,SETADN,TWOPV_L,ADVERB,ADDNOUN,VALID.WO
      XREF	GETNPC,CHKNPC,MATCHBUF,DEC.N,VALID.NP,NOTRAIL
      XREF	PADV,PVERB,YNHANDLR,DOYESNO

   
DIRECT
 
	CMP.B   #SECT.NOUN,D6      ;ADDRESSING SOMEONE
	BNE.S   01$                ;NOT TO SOMETHING
	ADDQ.W  #1,D5              ;ADJUST
	MOVE.W  D5,D0
	GETINFO        
	BTST    #6,4(A0)           ;NPC?
	BNE.S   DIRNPC             ;ADDRESSING SOMEONE
01$
	CMP.B   #SECT.YESNO,D6     ;YES OR NO DIRECT?
	BNE.S   10$
	TEST_W  TALKME(A4)

	IFEQ	THE_PAWN

	BNE.S	16$		   ;NOT DONE YET
	TEST_L	YNHANDLR(A4)
	BNE	DOYESNO		   ;suss out yes/no bits
*
* this may cause major stack traumas!!!!!!!!!!!
*

	ENDC

	BEQ.S	10$			;do first pass talking bits


16$
	TEST_W  D5                 ;YES?
	BNE.S   20$                ;NO
	MSG     JCHKG              ;JUST CHECKING
	BRA.S   99$                ;EXIT
20$
	CMP.B   #1,D5              ;NO
	BNE.S   10$
	MSG	WISLI              ;WELL IT SOUNDED LIKE IT
99$                              
	CLR.W   TALKME(A4)
	DOGO	DECODE
10$
	TEST_W	PVERB(A4)		;pending verb?
	BMI.S	11$			;no.
	MOVE.W	PVERB(A4),VERB(A4)
	MOVE.W	PADV(A4),ADVERB(A4)	;replace old values.
	LEA	INBUFF(A4),A6		;start again
	BRA	DEC.N
11$
	MSG	TALKME
	ST      TALKME(A4)
	DOGO	DECODE

*--------------------------------
	
DIRNPC
	
	CLR.W	NOUN(A4)

	CALL    INCNOUN            ;INTO BUFFER
	DO	NOERR              ;MASK ERRORS
	MOVE.W  D0,NPCINRM(A4)     ;SET AS FOUND
	MOVE.L  A6,WRDPNT(A4)      ;TEXT START IF NOT THE CASE
	DO	GETWRD	            ;FOR COMMA
	CMP.B   #SECT.SPEC,D6 
	BNE.S   20$                ;NO COMMA
	TEST_W  D5                 ;0= AND?
	BNE.S   20$                ;NO 
	MOVE.L  A6,WRDPNT(A4)      ;UPDATE SINCE COMMA
	DO	GETWRD	            ;NEXT
20$
	MOVE.W  #VNASK,VERB(A4)    ;INCASE of 'kronos, about band'
	BRA     SPIEL
 
*--------------------------------
 
CONVSE.V
 
*ZERO FLAG SET IF VERB IS 'SPEECH' TYPE
	
	CMP.W   #VNASK,D5
	BEQ.S   20$
	CMP.W   #VNQUESTION,D5
	BEQ.S   20$
	CMP.W   #VNTELL,D5
	BEQ.S   20$
	CMP.W   #VNSAY-1,D5
	BLS.S   10$
	CMP.W   #VNCALL,D5        ;CALL
	BHI.S   10$
20$
	OR      #4,CCR
	RET
10$
	MOVE    #0,CCR
	RET
 
*--------------------------------
  
SPEECH
  
* to avoid:
* >g
* what do u want to get?
* say piss off
* bomb!!!!!!!!!!

	CLR.W	NOUN(A4)		;yuk! 

	DO	NOERR
	MOVE.L  A6,WRDPNT(A4)

      IFEQ	THE_PAWN

	XREF	SP.SPEECH

	CALL	SP.SPEECH
	BEQ	SP55			;something happened

      ENDC

	CMP.W   #VNASK,VERB(A4)
	BEQ.S   10$
	CMP.W   #VNTELL,VERB(A4)
	BEQ.S   10$
	DO	GETADJPH
	CMP.B   #SECT.CONJ,D6 
	BNE.S   20$
	CMP.W   #1,D5			;to?
	BNE.S   20$
10$
	CALL	IsNextNoun		;incase MNPH can't cope
	BEQ.S	40$			;EQ => yes
	DO	MNPH
	BRA.S   40$
20$
	MOVE.W  D5,D2
	MOVE.W  NPCINRM(A4),D5
      
      IFNE	THE_PAWN

	BMI     SP50               ;NOBODY THERE

      ENDC

      IFEQ	THE_PAWN

	XREF	NearNPC,NearbyVerbs,VLEN_NBVerb
	
	BPL.S	30$			;we've got an NPC, no worries

* see if the verb allows NPC in next room (there's a case for a vsub here)

	PUSH_L	D0/A0			;shortage of OK regs!
	LEA	NearbyVerbs(A4),A0
	MOVEQ	#VLEN_NBVerb,D0

	MOVE.W	VERB(A4),D5
25$
	CMP.B	(A0)+,D5		;search list for verb
	DBEQ	D0,25$

	PULL_L	D0/A0			;won't kill flags

	BNE	SP50			;NE=> not in list

* ok, so the verb's allowed, but is there an NPC nearby?

	MOVE.W	NearNPC(A4),D5
	BMI	SP50			;MI=> nope
	MOVE.W	D5,NPCINRM(A4)

30$
      ENDC

	CALL    INCNOUN
	MOVE.W  D2,D5
	BRA.S   SPIEL              ;OK NEXT
40$ 
	CLR.W   NOUN(A4)           ;ZAP
	LEA     TEMPBUF(A4),A0
	MOVE.W  (A0)+,D0           ;GET NOUN#
	BEQ.S   20$
	TEST_W  (A0)+
	BEQ.S   50$
	MSG	1ATTIM
	DOGO	DECODE
50$
	GETINFO        
	BTST    #6,4(A0)           ;ALIVE?
	BNE.S   60$                ;OL
	MSG	MAD
	DOGO	DECODE
60$
	MOVE.W  D5,D1			;save D5 (used as a flag to VALID)
	MOVEQ   #0,D5              ;MAKE SURE ERRORS HAPPEN
	DO	VALID
	BNE     DECODE
	MOVE.W  D0,NPCINRM(A4)     ;UPDATE
	MOVE.W  D0,D5
	CALL    INCNOUN
	MOVE.W  D1,D5              ;RET  NEXT TYPE
SPIEL
	MOVE.W  NPCINRM(A4),D0
	CALL    IFANIMAL
	BNE     SP55               ;GO BACK TO SCREEN
	CMP.W   #SECT.CONJ,D6      ;CONJ?  
	BNE.S   40$                ;NOPE
	CMP.W   #$12,D5            ;ABOUT?
	BEQ.S   30$       
	CMP.W   #6,D5              ;FOR?
	BNE.S   40$                ;NOPE
	CALL    FORWHAT            ;DO FOR CONSTRUCT
	DOGO	DECODE
	
30$                                
	DO	YESERR  
	ST      ERR4(A4)           ;EXCPT WHICH
	CALL    GLOBAL             ;EXCEPT 'I DONT SEE..' ERROR
 
	BRA     NOTWSYN            ;DO AS NORMAL
40$
	MOVE.L  WRDPNT(A4),A6
	BRA     QUOTES
SP50
	MSG	NOONE
SP55
	MOVE.L  (A7)+,D0           ;DUMP
	DOGO	SCREEN

*--------------------------------
  
INTERROR
	
	MSG	ERROR
	DOGO	SCREEN            ;HIT THAT STACK!

*--------------------------------
	        
	        
FORWHAT
	        
	
*'FOR' CONSTRUCTION
*ASK X FOR (NOUNS) ..
	
	ST      ERR4(A4)           ;NO WHICH/WHAT PROBLEMS
	MOVE.W  NPCINRM(A4),D0
	MOVE.W  D0,D5
	GETINFO        
	MOVE.L  A0,A5
	DO	GETNPC	            ;TRO A3
	BTST    #0,(A0)            ;PLURAL?
	BEQ.S   09$                ;NO
	MSG	ONEOF
09$
	DO	CHKNPC
	DO	P.TN
	MSG	SAYS
	MOVE.W  D0,-(A7)
	MOVE.L  A3,-(A7)
	
	DO	MNPH               ;DO NP
	MOVE.L  (A7)+,A3
	MOVE.W  (A7)+,D2
	LEA     TEMPBUF(A4),A1
	TEST_W  (A1)               ;ANY?
	BNE.S   10$                ;OK
	MSG	IDNK
	BNE.S	10$
	MSGGO	GOTNON
10$
	MOVE.W  (A1)+,D0           ;GET ITEM 
	BEQ	90$
	CMP.W   D0,D2              ;FOR NPC ITSELF?
	BEQ.S   11$                ; HMM
	GETINFO        
	MOVE.W  D0,STOPRON(A4)
	CALL    TOPNPC
	CMP.W   D4,D2              ;TOP?
	BEQ.S   12$
	MSG	BIDHIT           ;BUT I DONT HAVE
	BRA     P.NQEN
12$
	BTST    #0,11(A0)          ;WORN?
	BEQ.S   20$                ;NO
11$
	BeSure	M.YMBJ

	MOVE.W  #M.YMBJ,D1         ;YOU MUST BE JOKING
	DOGO	P.MSG
20$
	BTST    #3,4(A0)           ;MONEY?
	BEQ.S	2005$
	MSGGO	NOCHAN
2005$
	BTST    #6,5(A0)           ;GIVEN?
	BNE.S   21$                ;K
	BTST    #1,3(A3)           ;NPC IS NICE?
	BEQ.S   22$                ;NO
	CMP.B   #1,6(A0)           ;IS SIMPLE REL?
	BEQ.S   21$                ;YES, OK
22$
	MSG	NO
	DO	P.PRN
	DO	ISARE
	MSG	MINE
	BRA.S   90$
21$
	ST	NOTRAIL(A4)
	MSG	NONEED               ;OK, MESG
	SF	NOTRAIL(A4)
	MOVE.W  SUBJECT(A4),8(A0)      ;BLAT
	CLR.W   6(A0)
	BRA	10$			;ANOTHER
90$
	RET
	
BACK.WRD 
	
*MOVE BACK 1 WORD
	
	MOVE.L  A1,-(A7)
	MOVE.L  POSWRD(A4),A6
	LEA     INBUFF(A4),A1
	CMP.L   A1,A6                  ;AT BEGINING
	BEQ.S   90$                    ;OK
	SUBQ.L  #1,A6                  ;DEC
90$
	MOVE.L  (A7)+,A1
	RET

*--------------------------------
	
DODATIVE
	
	MOVE.L  DATIVE(A4),A2
	LEA     NOUNBUF2(A4),A1
	LEA     TEMPBUF(A4),A0
10$
	MOVE.W  (A0)+,(A1)+
	CMP.L   A0,A2              ;DONE?
	BNE.S   10$
	LEA     NOUNBUF(A4),A1
	CALL    MOVBUF             ;REST
	MOVE.W  #1,CONJ(A4)
	CALL    SETADN
90$
	RET

*--------------------------------
	        
TWOPV
	
*TRANSLATE 2PART VERBS INTO SINGLE
*VERB MEANING
	
	LEA     TWOPV_L(A4),A1
10$
	MOVE.L  (A1)+,D1
	BEQ.S   90$                ;FAIL
	CMP.B   VERB+1(A4),D1
	BNE.S   10$                ;NO, TRY NEXT
	SWAP    D1                 ;MOVE CONJ LOW
	CMP.W   ADVERB(A4),D1
	BEQ.S   20$
	TEST_W  ADDNOUN(A4)        ;SECOND NP?
	BNE.S   10$                ;YES, CONJ IS FOR REAL THEN
	CMP.W   CONJ(A4),D1
	BNE.S   10$                ;NO
	MOVE.W  #-1,CONJ(A4)       ;CLEAR CONJ IF 'VERB NOUN CONJ' MATCH
20$
	ROL.L   #8,D1              ;MOVE NEW VERB LOW
	MOVE.B  D1,VERB+1(A4)
	MOVE.W  #-1,ADVERB(A4)     ;CLEAR ADV
90$
	RET
	
*--------------------------------
	
PICKONE
	
* TRIES TO PICK A LOCAL NOUN FROM
* MATCHBUF
* ELSE PICKS  LAST
* USES A0/D0/D7
	
* EXIT:
* D5,D6 SET ON CHOSEN NOUN
	
	LEA     MATCHBUF(A4),A0
10$
	MOVE.L  (A0)+,D0           ;NEXT
	BMI.S   90$                ;EXIT WI D6/D5 AS B4
	SWAP    D0
	CMP.W   #SECT.NOUN,D0      ;NOUN
	BNE.S   10$                ;FORGET IT
	SWAP    D0                 ;BAK
	DO	VALID.WO           ;BUT IS IT LOCAL??
	MOVE.W  D0,D5
	MOVEQ   #SECT.NOUN,D6
	TEST_W  D7
	BNE.S   10$                ;NO, NEXT
90$                      
	RET

*--------------------------------
*
* IsNextNoun - suss if the next word is an adj/noun and an NPC
*
* this will fake up tempbuf if it is, pretending to be MNPH
* 
* really this is a frig to avoid
*
* >say to bird xyzzy
* Talking to inanimate objects is a sure sign of madness!
*
* which is due to MNPH dying on the bird (adj) xyzzy (crap) and never
* spotting that the bird is a noun too.
*
* returns EQ => it was, A6/POSWRD updated
*         NE => nope, not updated
*

IsNextNoun

	MOVE.L	POSWRD(A4),-(A7)	;just in case..
	MOVE.L	A6,-(A7)

	PUSH_L	D0-D7/A0-A5

	DO	GETWRD
	LEA	MATCHBUF(A4),A2		;not nice!!!
	MOVEQ	#0,D4			;flag found an adj if D4.B <> 0
10$
	MOVE.W	(A2)+,D6		;section
	BMI.S	90$			;failed
	CMP.W	#SECT.ADJ,D6		;problem only if there is an adj
	BNE.S	20$
	ST	D4			;flag there was one
20$
	MOVE.W	(A2)+,D5
	CMP.W	#SECT.NOUN,D6
	BNE.S	10$
	ADDQ.W	#1,D5			;convert to noun#
	MOVE.W	D5,D0
	DO	VALID.NP		;is it valid too?
	BNE.S	10$
	TEST_B	D4			;but was there an adj to confuse too?
	BEQ.S	90$			;EQ => nope, cannot help

* got here means found one

	LEA	TEMPBUF(A4),A0
	MOVE.W	D0,(A0)+		;fake-up buffer
	CLR.W	(A0)+			;+ terminator

	PULL_L	D0-D7/A0-A5
	POPSP
	POPSP
	SETEQ
	RET

90$
	PULL_L	D0-D7/A0-A5
	MOVE.L	(A7)+,A6
	MOVE.L	(A7)+,POSWRD(A4)
	SETNE
	RET


      END
