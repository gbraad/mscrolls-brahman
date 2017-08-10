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
*	@(#)parser1.asm	2.12
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PARSER1
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"

      XDEF     FNDWRD,GETWRD,DSTAND,RTSBACK,RTSBACK2,NHAVE,D72

      XREF     INBUFF,W.JNSP,PRTWRD,SEARCHER,MATCHBUF,ISDOFRE,VALID.WO,ERR1
      XREF     ZAP,WORD_,SECTION_,VERB,VERB2,DO.ONE,RELPH,TYPE,NEG_,POSSESS
      XREF     CLRADJ,HOLDE,UNHOLDE,P.IDFY,DO.ADJ,NOADJ,GETDATA,TOPNPC
      XREF     P.NFND,IGNORQ,IGNORAJ,ERR2,ADJFLG,ERR3,ADJNOUN,ERR4,PRTTXT
      XREF     PEND2,CONTEXT,ISTN,W.WHICH,NOTRAIL,DNOUNBUF,VALID.NP
      XREF     POSWRD,P.LOR,CHAROUT,W.NOTIMP,P.BTN,W.DNH,P.NONLY

      XREF     RESREL,FNOUN

*--------------------------------
*
* FINDWORD
*
* FINDS NEXT LETTER IN INBUFF
*
* ON EXIT:
* D7=1 = END OF INPUT LINE
*
*--------------------------------
 
 
FNDWRD
 
	CLR.L   D7
	LEA     INBUFF(A4),A2
	CMP.L   A2,A6
	BEQ.S   20$
	CMP.B   #$20,(A6)
	BEQ.S   20$
10$
	MOVE.B  (A6)+,D0
	BEQ     30$
	CMP.B   #$20,D0
	BNE.S   10$                  ;UNTIL END
20$
	MOVE.B  (A6),D0
	BEQ.S   30$                  ;TEXTEND
	CMP.B   #$20,D0              ;SPACE
	BNE.S   40$
	ADDQ.L  #1,A6
	BRA.S   20$
30$
	MOVEQ  #1,D7
40$
  
LB3  
  
	RET
 
*--------------------------------
 
DSTAND 
 
	CMP.B   #2,D7
	BEQ.S   90$
	MSG	JNSP
	CALL    PRTWRD
90$
	DOGO	D72
  
*--------------------------------
*
* GETWRD
*
* SCANS INBUFF FOR NEXT WORD
* SKIPS WORDS CONTAINED IN WORD
* SECTIONS 0 TO 1 (QUANTIFIERS.)
*
* ON EXIT:
*      D7=1 END OF FINDWORD
*      D7=2 DOSN'T UNDERSTAND
*      D7=4 NOT ENOUGH ADJECTIVES
*           SETS UP FOR A WHICH ?
*
*--------------------------------
 
GETWRD
 
	CALL_S	FNDWRD               ;POP IF NO WORD
	TEST_B  D7
	BNE     LB3                  ;RTS!
	CALL	SEARCHER
	MOVE.W  #-1,(A2)             ;TERMINATE MATCH LIST
	LEA     MATCHBUF(A4),A2
	TEST_W	FNOUN(A4)	     ;force noun?
	BEQ.S	01$
02$
	ADDQ.L	#4,A2		     ;skip 1st entry.
	CMP.W	#SECT.NOUN,(A2)	     ;noun?
	BNE.S	02$		     ;until noun
	CLR.W	FNOUN(A4)	     ;clear:- only work once.
01$
	MOVEQ   #0,D4                ;COUNT NO. MATCHES
	TEST_W  (A2)                 ;NONE?
	BMI     GW51                 ;ERROR
GW05
	MOVE.W  (A2)+,D1             ;GET SECTION #
	BMI     GW50                 ;-1 TERMINATES LIST
	MOVE.W  (A2)+,D0             ;GET WORD
	CMP.W   #SECT.NOUN,D1
	BNE.S   GW07 
	DO	ISDOFRE
	BEQ.S   09$		   ;yes, no need for valid.
	DO	VALID.WO           ;CALL DOES ADD,ERROR SUP. & SUB
	TEST_W  ERR1(A4)
	BEQ.S   10$
09$
	CLR.W   D7
10$ 
	TEST_W  ZAP(A4)			;forced valid ignor.
	BNE.S   20$
	TEST_B  D7
	BNE.S   GW05
20$
	ADDQ.W  #1,D4                ;COUNT
	MOVE.W  D0,WORD_(A4)
	MOVE.W  D1,SECTION_(A4)
	BRA.S   GW05
  
GW07

VerbORNoun

      IFEQ	THE_PAWN
*
* Attempt to allow 'use cue to cue cue ball' to work (see 'use lift' below)
*
*
* set a flag when we see 'TO' after a 'USE'

	XREF	FoundTO

	CMP.W	#SECT.CONJ,D1		;look for 'TO'
	BNE.S	05$
	CMP.W	#TO,D0			;as in 'use x TO verb y'
	BNE.S	05$
	CMP.W	#VNUSE,VERB(A4)
	BNE.S	05$
	ST	FoundTO(A4)
05$

      ENDC

	CMP.W   #SECT.VERB,D1        ;SUSS PLANT THE PLANT....
	BNE.S   GW08
	TEST_W  (A2)                 ;MORE MATCHES?
	BMI.S   GW08                 ;NOPE = USE THIS!
	CMP.W   #VNUSE,VERB(A4)      ;IF USE THEN WE STILL NEED A VERB
	BNE.S   10$                  ;THIS FUCKS 'USE LIFT TO GO DOWN'
*                                    ;BUT MAKES 'USE TROWEL TO PLANT...' OK

      IFEQ	THE_PAWN

	TEST_B	FoundTO(A4)	;got to stage where may be a verb?
	BEQ.S	10$		;EQ => nope, should be a noun/adj

      ENDC

	TEST_W  VERB2(A4)            ;Have we found a second verb yet?
	BMI.S   GW08                 ;PL => yes, use this word as a NOUN
10$
	TEST_W  VERB(A4)             ;Do we already have a verb?
	BPL.S   GW05                 ;Yes = we don't want this one - use as noun
 
GW08
	CMP.B   #SECT.ONE,D1         ;SPECIAL PRONOUN TYPE
	BNE.S   10$
	CALL    DO.ONE               ;GRAB EFFECTIVE MATCHBUF
	CMP.B   #2,D7                ;ERROR HAPPENED?
	BEQ     D72                  ;YES = BYE BYE
	BRA.S   GW50                 ;AND PROC AS A NOUN
10$
	MOVE.W  D1,SECTION_(A4)
	MOVE.W  D0,WORD_(A4)
	BRA     GW55
GW50
	MOVE.W  D4,-(A7)             ;SAVE MATCHES
	CALL    RELPH
	BEQ.S   10$
	MOVE.W  (A7)+,D0             ;DUMP OLD
	BRA.S   20$
10$
	MOVE.W  (A7)+,D4             ;OLD BAK
20$
	CMP.B   #2,D7                ;ERROR?
	BEQ     D72                  ;EXIT
	CMP.B   #1,D4
	BHI     LB4
	BLT     GW51  
GW52
	CMP.B   #2,TYPE(A4)          ;PSIVE
	BNE     GW51
	CMP.B   #'T'+32,TYPE+1(A4)
	BNE.S   10$ 
	NOT.W   NEG_
	BRA     GW51
10$
	MOVE.W  WORD_(A4),D0
	ADDQ.W  #1,D0
	MOVE.W  D0,POSSESS(A4)
  
	DO	CLRADJ
56$
	CALL    HOLDE                ;HOLD ERRORS
	DO	GETWRD
	CALL    UNHOLDE              ;RELEASE
	CMP.B   #2,D7
	BEQ     D72
	CMP.B   #4,D7
	BEQ.S   30$			;non-unique, resolve
	TEST_W  D7			;end of line
	BNE	GW55			;yes, forget the possesion.
	CMP.B   #SECT.NOUN,D6
	BEQ.S   53$			;a straight noun, check possesion & ret
	CMP.B   #SECT.ADJ,D6		;ADJ?
	BNE     P.IDFY
	CALL    HOLDE			;HOLD AGAIN
	CALL    DO.ADJ
	CALL    UNHOLDE			;free errors again.
	TEST_W  D5			;was do.adj successful?
	BNE.S   53$			;yes, is now setup for a noun.
					;no. too bad error.
	CMP.B   #4,D7			;try to suss l small sphere bugs
	BEQ     RTSBACK2
	TEST_W  ERR1(A4)		;errors off?
	BNE     D72			;yes, set d7 and return.
	BRA     NOADJ
53$
	MOVE.W  D5,D0			;check possesion is correct.
	ADDQ.W  #1,D0
	GETINFO
	CALL    TOPNPC
	CMP.W   POSSESS(A4),D4		;yes?
	BNE     NHAVE			;no, but %tn does not have %tn.
	RET				;ok, d6/d5 are ok.
	
30$
	LEA     MATCHBUF(A4),A1		;try to find one from matchbuf
20$
	MOVE.L  (A1)+,D0             ;GET SECT& NUM     
	BMI.S   31$                  ;NOPE, NOT THERE
	SWAP    D0
	CMP.B   #SECT.NOUN,D0        ;NOUN?
	BNE.S   20$                  ;IGNOR
	SWAP    D0                   ;BACK
	ADDQ.W  #1,D0                ;ADJUST
	GETINFO
	CALL    TOPNPC
	CMP.W   POSSESS(A4),D4		;is this noun owned?
    
	BNE.S   20$                  ;NEXT
	MOVE.W  D0,D5			;yes, fake it was straight
	SUBQ.W  #1,D5
	MOVEQ   #SECT.NOUN,D6
	BRA     LBL8                 ;EXIT
	
31$
	MOVE.L  -8(A1),D5		;NB: Matchbuf is non-empty, so ok.
	BRA     NHAVE			;pick any one, and print error.
	
GW51
	TEST_B  D4
	BNE.S   GW55
 
	LEA     MATCHBUF(A4),A2      ;FIND FIRST NOUN IN MATCHBUF
10$
	MOVE.W  (A2)+,D6             ;GET SECTION
	BMI.S   LBL7                 ;NO NOUNS!!
	MOVE.W  (A2)+,D0             ;AND WORD 
	CMP.W   #SECT.NOUN,D6
	BNE.S   10$                  ;FIND A *NOUN*
 
	ADDQ.W  #1,D0                ;ADJUST NOUN #
	CALL    P.NFND
	DOGO	D72
GW55
	MOVE.W  SECTION_(A4),D6
	MOVE.W  WORD_(A4),D5
	CMP.W   #SECT.QUANT,D6       ;SECTION TO IGNORE?  
	BNE.S   10$
	MOVE.W  D5,IGNORQ(A4)
*	DOGO	GETWRD
	BRA.S	18$
10$
	CMP.W   #SECT.ADV,D6         ;SECTION TO IGNORE?
	BNE.S   20$                  ;NOPE, CARRY ON
	MOVE.W  D5,IGNORAJ(A4)       ;YES - SAVE IT FOR LATER (PERHAPS)
18$
	BRA	GETWRD	              ;GET A 'REAL' WORD INSTEAD
20$
LBL8
	CLR.L   D7
	RET
LBL7
	TEST_W  ERR2(A4)
	BNE.S   LBL72 
	TEST_W  ADJFLG(A4)           ;ERROR DUE TO ADJ
	BEQ.S   LBL6
	MOVE.W  ERR3(A4),D7
LBL72
	CLR.W   ADJNOUN(A4)
	RET
LBL6
 
	MOVE.W  #-1,PEND2(A4)        ;KILL PENDING CONJ
	BRA     DSTAND
 
LB4
	CALL    RESREL               ;TRY to resolve adj using previous rels
	CALL    CONTEXT
	CMP.B   #1,D4
	BNE.S   10$                  ;still too many nouns
	MOVE.W  MATCHBUF(A4),SECTION_(A4)
	MOVE.W  MATCHBUF+2(A4),WORD_(A4)
	BRA     GW52
10$
	TEST_W  ERR4(A4)             ;NOERR ON?
	BNE.S   RTSBACK2             ;YUP = DON'T DO 'WHICH XYZ?'
	CALL    ISTN                 ;1ST NOU IN MB
	MSG	WHICH
	ST      NOTRAIL(A4)          ;INHIBIT SPACE
*	PEA     RTSBACK(A4)
*	PUSH_L  D0-A1/A3/A5    ;HORENDOUS ENTRY TO NONLY!
*	CLR.W   D4
*	CLR.W   D3
*	BRA     NONLY                ;BEWARE OF STACKS WHICH BITE!!
* 
RTSBACK
 
	CLR.B   NOTRAIL(A4)        ;RESET 
	PRINT   <', '>
	LEA     DNOUNBUF(A4),A2
	LEA     MATCHBUF(A4),A1
	DO	ISDOFRE
	SEQ	D2			;so D2 ne => DON'T call valid
10$
	MOVE.L  (A1)+,D0
	BMI.S   20$
	SWAP    D0
	CMP.B   #SECT.NOUN,D0
	BNE.S   10$
	SWAP    D0
	ADDQ.W  #1,D0
	TEST_B	D2
	BNE.S	15$
	DO	VALID.NP	            ;LOCAL?
	BNE.S   10$                  ;NO, TOO BAD
15$
	MOVE.W  D0,(A2)+
	BRA.S   10$
20$
	CLR.W   (A2)
	LEA     DNOUNBUF(A4),A0
	MOVE.L  POSWRD(A4),-(A7)
	CALL    P.LOR                ;X,Y OR Z - LIST
	MOVE.L  (A7)+,POSWRD(A4)
	MOVEQ   #'?',D1 
	DO	CHAROUT
 
RTSBACK2
 
	MOVEQ   #4,D7
	RET
 
NHAVE
 
    
	TEST_W  ERR1(A4)           ;ERRORS?
	BNE.S   D70
	MOVE.W  D5,D0
	ADDQ.W  #1,D0
	GETINFO
	BTST    #3,6(A0)           ;IMPLIED POSSESION
	BEQ.S   10$ 
	MSG	NOTIMP
	BRA.S   D72                ;ERROR
10$
	MOVE.W  D0,D5
	MOVE.W  POSSESS(A4),D0
	DO	P.BTN
	MSG	DNH              ;DOES NOT HAVE
	MOVE.W  D5,D0
	
	CALL    P.NONLY
D72
 
	MOVEQ   #2,D7
D70
	RET

      END

