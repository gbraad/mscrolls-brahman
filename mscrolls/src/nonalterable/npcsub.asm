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
*	@(#)npcsub.asm	2.12
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.NPC.SUB1
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"


	 XREF   GETRELN,P.BTN,ISARE,W.DEAD,GETNPC,W.UNCON,R.NOUN2,QuoteFlag
	 XREF   W.ASLEP,W.IDNATA,RNDGEN,P.SCN,VERB,GETDATA,P.TN,CHAROUT
	 XREF   REP1,SUBJECT,W.NOTMY,P.NOUN,SCAN.REP,ERR1,BSFULLSTOP
	 XREF   W.WNTLT,P.SUBOB,SCAN.SUB,SCAN.OBJ,IUFLAG
	 XREF   ERR4,GETADJPH,TYPE,ISARETYP,KYWRD
	 XREF   SCANVERB,NEG_,NPC.DATA,CONVNPC,W.ONEOF,SP.SAY
	 XREF   W.SAYS,INSULT,P.TAB,ERR3,GLOBAL.F,UTIL.BS
	 XREF   SP.ODREP,W.WPRI,P.PRN,W.COULD,PRTTXT


	 XDEF   CHKNPC,COPOUT,CHK.OWN,SCANNER
	 XDEF   BLD.NVN,SCANSUB,SC.SHORT,CHKPAUSE
	 XDEF   UNPAUSE,P.SAY2,P.SAY,P.KYWRD
	 XDEF   YESERR,NOERR,GLOBAL,NPC.OK,IFANIMAL


      IFNE	THE_PAWN

	XDEF	FNDWEAPO
    
*  D3=BEST DEFENCE
*  D2=DEFENCE STRENGTH
  
FNDWEAPO
 
	PUSH_L  D0-D1/D5/A0/A1
	MOVE.W  D0,D2
	DO	GETRELN	
	CLR.W   D2
	CLR.W   D3
20$
	MOVE.W  (A1)+,D0
	BEQ.S   10$
	GETINFO
	BTST    #6,6(A0)
	BNE.S   20$
	BTST    #7,3(A0)
	BEQ.S   30$
	MOVE.B  2(A0),D1
	LSR.B   #4,D1     ;SIZE
	OR.B    #$80,D1   ;SET HI
	BRA.S   40$       ;CONT
30$
	MOVE.B  1(A0),D1  ;H & S
	LSR.B   #4,D1
	CMP.B   #10,D1    ;BRITTLE
	BEQ.S   20$
	CMP.B   #12,D1    ;FLEX
	BEQ.S   20$
	CMP.B   #3,D1
	BLS.S   20$
	MOVE.B  1(A0),D1
	AND.B   #$0F,D1
40$
	CMP.B   D2,D1
	BLS.S   20$
	MOVE.W  D0,D3     ;SAVE ITEM
	MOVE.B  D1,D2     ;BEST
	BRA.S   20$
10$
	CMP.B   #3,D2
	BHI.S   11$
	MOVE.B  #4,D2
	CLR.W   D3
11$
	PULL_L  D0-D1/D5/A0/A1
	RET

      ENDC
 
*--------------------------------
*
* 	CHKNPC Pops a spurious longword from the stack if the
*       NPC is NOT ok. This is meant to return an extra level
*       of routine, assuming that a return address is on the stack
*
*       NB Also corrupts A2 if the NPC is dead/asleep/etc.

CHKNPC
	PUSH_L  D0/D1/A0/A3
	MOVE.W  D5,D0
	MOVE.L  A5,A0
	BTST    #1,(A0)
	BEQ.S   05$
	DO	P.BTN
	DO	ISARE
	MSG	DEAD
	BRA.S   90$
05$
	DO	GETNPC
	MOVE.B  2(A3),D1
	CMP.B   #5,D1
	BNE.S   10$
	DO	P.BTN
	DO	ISARE
	MSG	UNCON
	BRA.S   90$
10$
	CMP.B   #4,D1
	BEQ.S   20$
	PULL_L  D0/D1/A0/A3        ; This is the CLEAN exit.
	RET
20$
	DO	P.BTN
	DO	ISARE
	MSG	ASLEP

* ... and this is the grubby one

90$     
	PULL_L  D0/D1/A0/A3/A2     ; Ugly pop of an RET as well!
	RET
  
*--------------------------------
   
COPOUT

	MOVE.L  D2,D1		;get list of copouts 
	BNE.S	10$
	MSGGO	IDNATA        ;eq=>none ->I DON'T KNOW ANYTHING ABOUT THAT
10$
	CALL    RNDGEN		;random num into d0
	AND.W   #3,d0		;just from 0-3
	ADDQ.B  #1,d0		;from 1-4
16$
	SUBQ.B  #1,D0
	BEQ.S   20$
	ROR.L   #8,D1
	BRA.S   16$
15$
	ROR.L   #8,D1
20$
	TEST_B  D1
	BEQ.S   15$		;until something
	AND.W   #$FF,D1
	DOGO	P.SCN
 
*--------------------------------
  
CHK.OWN
 
	PUSH_L	D0-D2/A0
	CLR.W   D7
	MOVE.W  VERB(A4),D1
	CMP.W   #VNEXAMINE,D1		;look or examine?
	BLS     99$

      IfVerb	MAKELOCAL

	CMP.W	#VNMAKELOCAL,D1		;npc can't stop u till u get there
	BEQ	99$

      ENDC

	MOVE.B  6(A0),D2            ;SAVE INITIAL RELATION
	AND.B   #$3B,D2
	BEQ	99$                 ;NOT INIT
05$
	MOVE.B  6(A0),D1            ;REL?
	AND.B   #$3B,D1
	BEQ.S   99$                 ;OK
	MOVE.W  8(A0),D0
	GETINFO
	BTST    #6,4(A0)
	BEQ.S   05$
	CALL    NPC.OK
	BNE.S   05$
20$
	DO	GETNPC	          ;TO A3
	BTST    #1,3(A3)         ;LETS U?
	BEQ.S   22$  		 ;NO
	CMP.B   #1,D1            ;MUST BE SIMPLY RELATED
	BNE.S   22$ 		 ;NO
	TEST_B  6(A0)
	BEQ.S   99$
	BRA.S   05$
22$ 
	TEST_W  REP1(A4)
	BNE.S   21$                ;SKIP
	ST      REP1(A4)
	CMP.W   SUBJECT(A4),D0
	BNE.S   23$ 		 ;OK
	MSG	NOTMY
	PULL_L  D0-D2/A0
	DO	P.NOUN
	MOVEQ   #1,D7
	RET
23$

      IFEQ	THE_PAWN

	XREF	ListNPCAllow

	LEA	ListNPCAllow(A4),A3
10$
	MOVE.L	(A3)+,D2
	BEQ.S	12$			;zero long terminator
	CMP.W	D0,D2			;right npc?
	BNE.S	10$
	SWAP	D2
	CMP.W	VERB(A4),D2		;format dc.w verb,npc
	BNE.S	10$
	BRA.S	99$			;yes, allowed then
12$

      ENDC

	DO	P.BTN		;but the <npc>
	MSG	WNTLT		;won't let
	CALL    P.SUBOB		;you
	DO	BSFULLSTOP	;.
21$
	MOVEQ   #1,D7
99$
	PULL_L	D0-D2/A0
	RET
	        
 
	
*--------------------------------
*
*   THE SCANNER
*--------------------------------
   
*PRIORITY:
*  QUESTION
*  'THINK'
*  'WANT'
*  N.V.N.
*  NOT
   
	
*--------------------------------
	 
*BIT FLAGS IN IUFLAG:
	
* 0=YOU, 1=I,  2=WANT,LIKE...
* 3=THINK, KNOW..
* 4=INSULT
* 5=IS/ARE PRES.
    
SCANNER

	CALL_S  SCANSUB
	BRA     SCAN.REP

SCANSUB
  
	DO	NOERR                 ;FREZE ERROR
	CLR.L   SCAN.SUB(A4)
	CLR.L   SCAN.OBJ(A4)
	CLR.L   IUFLAG(A4)
	CLR.W   ISARETYP(A4)
	LEA     IUFLAG(A4),A1

BLD.NVN

	ST      ERR4(A4)               ;NO WHICHES?
*	SUBQ.L  #1,A6			;taken care of in back.wrd b4 call
	MOVE.L  A6,A5
	CLR.W   D7
30$
	CMP.W   #1,D7
	BEQ     92$
	PUSH_L  A1/A5
	DO	GETADJPH
	PULL_L  A1/A5
	CMP.B   #2,D7
	BEQ.S   30$
	CALL    SC.SHORT
	CMP.W   #SECT.NOUN,D6
	BNE.S   31$                    ;NONE
	ADDQ.W  #1,D5                  ;ADJ
	TEST_W  SCAN.SUB(A4)
	BNE.S   60$
	
	MOVE.W  D5,SCAN.SUB(A4)
	BRA     30$
60$
	TEST_W  SCAN.OBJ(A4)
	BNE.S   30$
	MOVE.W  D5,SCAN.OBJ(A4)
	BRA     30$
31$
	CMP.B   #1,TYPE(A4)            ;END QUOTE
	BEQ     92$
	CMP.B   #10,D6                  ;TO BE
	BNE.S   23$
	BSET    #5,(A1)
	MOVE.W  D5,ISARETYP(A4)
	BRA.S   20$
23$
	CMP.W   #2,D6                ;SPECIAL
	BNE.S   24$
	CMP.W   #$11,D5              ;ABOUT,MUCH,MANY
	BLS.S   24$
	MOVE.W  D5,ISARETYP(A4)
	BRA.S   30$
24$
	CMP.B   #13,D6                ;WANT,LIKE WISH
	BNE.S   21$
	BSET    #2,(A1)
21$
	CMP.B   #14,D6                 ;THINK, KNOW..
	BNE.S   22$
	BSET    #3,(A1)
22$
	CMP.B   #16,D6
	BNE.S   20$                    ;INSULT
	BSET    #4,(A1)
	MOVE.W  D5,KYWRD(A4)
	BRA     30$
20$
	CMP.W   #SECT.VERB,D6           ;VERB?
	BNE.S   50$
	CMP.W   #VNINVENTORY,D5         ;I
	BNE.S   90$
	BSET    #$1,(A1)
	BRA     30$
90$
	TEST_W  SCANVERB(A4)
	BNE.S   50$
	MOVE.W  D5,SCANVERB(A4)
	BRA     30$  
70$
50$
	CMP.B   #12,D6                 ;YOU
	BNE.S   80$
	BSET    #0,(A1)
80$
	CMP.B   #$F,D6
	BNE.S   81$
	NOT.W   NEG_(A4)
	BRA     30$
	
81$
	CMP.B   #SECT.QUEST,D6
	BNE     30$                    ;BACK
	MOVEQ   #3,D7                  ;FLAG AS SPECIAL CASE
	MOVE.L  (A7)+,D0               ;DUMP RET
92$
	CLR.W   ERR4(A4)
	RET
	
*--------------------------------
	
SC.SHORT
	
	CMP.B   #2,TYPE(A4)        ;APOSTROPHE TERMINATOR
	BNE.S   90$
	MOVE.B  TYPE+1(A4),D0      ;GET NEXT CHR
	CMP.B   #'T'+32,D0         ;NOT?
	BNE.S   10$                ;NO
	NOT.W   NEG_(A4)            ;NEGATE
	BRA.S   90$                ;DONE
10$
	CMP.B   #'R'+32,D0         ;RE?
	BEQ.S   20$  ;OK
	CMP.B   #'M'+32,D0
	BNE.S   90$                ;SORRY
20$
	BSET    #5,(A1)            ;TO BE EXITS
90$
	RET

*--------------------------------

* BNE: PAUSED
   
   
CHKPAUSE

	PUSH_L  D0/D1/A0/A3
	BTST    #1,(A0)
	BNE.S   50$
	DO	GETNPC
	MOVE.B  2(A3),D1
	LSR.B   #4,D1
	CMP.B   #5,D1
	BEQ.S   50$
	CMP.B   #4,D1
	BEQ.S   50$
	BTST    #0,3(A3)           ;PAUSED?
	BEQ.S   10$
50$
	MOVEQ   #1,D1
10$
	PULL_L  D0/D1/A0/A3
	RET



*--------------------------------
*
*  SRC.ADV.NPC.SUB2
*
*--------------------------------
	
	
		IFNE	THE_PAWN
	
*--------------------------------
*
* MOVENPC      LETS NPC'S WALK
*              ABOUT
*
* ON ENTRY:
*
*   D0 - NOUN NO.
*   A0 - POINTER TO NOUN DATA
*   A6 - POINTER TO TIME MAP
*        (ACTUALLY TO CUR. LOC.)
*
*
* ON RETURN
*
*   D1/D2 STEPPED ON
*   8(A0), (A6), 2(A6) UPDATED
*   LOC    CLOC  OFST
*
*
*--------------------------------

	XDEF	MOVENPC
 
MOVENPC
 
	CALL    CHKPAUSE            ;NOT DOING ANYTHING ELSE
	BNE.S   90$        
	TEST_W  (A6)                ;IF CUR  LOC=0 THEN STAY STILL
	BEQ.S   90$
	CLR.L   D2
	MOVE.W  2(A6),D2            ;OFSET INTO MAP
	CLR.L   D1
	MOVE.B  4(A6,D2),D1         ;NEW ROOM NO.
	BEQ.S   10$                 ;ROOM #0 MEANS STOP OR LOOP
	MOVE.W  D1,(A6)
	MOVE.W  D1,D2               ;FOR R.NOUN2
	MOVE.L  D0,-(A7)            ;SAVE D0
	DO	R.NOUN2
	MOVE.W  D0,8(A0)            ;NEW ROOM NOUN#
	MOVE.W  #$0100,6(A0)        ;RELATED
	MOVE.L  (A7)+,D0            ;RECOVER D0
	ADD.W   #1,2(A6)            ;NEXT LOC
	RET
 
10$
	TEST_B  5(A6,D2)            ;00 00 00 MEANS STOP
	BNE.S   20$
	TEST_B  6(A6,D2)
	BNE.S   20$
	CLR.W   (A6)                ;SET CUR LOC TO ZERO = STOP
	RET
 
20$
	MOVE.B  5(A6,D2),2(A6)      ;HIGH BYTE NEW OFFSET
	MOVE.B  6(A6,D2),3(A6)      ;LOW BYTE
90$
	RET



	ENDC


*--------------------------------
   
UNPAUSE
	MOVE.L  A0,-(A7)
	LEA     NPC.DATA(A4),A0
	CLR.B   3(A0)
10$
	TEST_L  (A0)+
	BEQ.S   90$
	BCLR    #0,3(A0)
	BRA.S   10$
90$
	MOVE.L  (A7)+,A0
	RET
 
*--------------------------------
*
*  P.SAY2 - PRINTS 'XYZ SAYS'
*             OR 'ONE OF THE
*             XYZS SAYS'
*
* (IF XYZ IS PLURAL)
*
* HANDLES STUFF LIKE
* THE GURU, BETWEEN FITS OF...
* VIA SP.SAY
*
*--------------------------------
 
 
P.SAY2
 
	PUSH_L  D0/D1/A0         ;COS P.SAY HAS TO
	BRA.S   P.SAY.SUB
 
P.SAY
 
	PUSH_L   D0/D1/A0
	MOVE.W   CONVNPC+4(A4),D0
 
P.SAY.SUB
  
	GETINFO
	BTST    #0,(A0)              ;PLURAL?
	BEQ.S   10$                  ;NO
	MSG	ONEOF
10$
	DO	P.TN
	CALL    SP.SAY               ;SPEECH MODIFIER 
*			(ie: The guru, between fits of laughter, says... )
	MSG	SAYS
      
      IFEQ	THE_PAWN

	BCHG	#0,QuoteFlag(A4)	;use test and set to allow
	BNE.S	20$			;'the xyzzy says nothing.'
	CALL	UTIL.BS			; to print the xyzzy says{, "}willies"
	PRINT 	<', '>			;change to msg one day?
	MSG     QUOTE
	MOVEQ	#-1,D1			;to tell CHAROUT about capitals
	DO	CHAROUT
20$
      
      ENDC

	PULL_L	D0/D1/A0
	RET
 
*--------------------------------
  
P.KYWRD
	
	MOVE.W  KYWRD(A4),D1
	LEA     INSULT(A4),A1
	BRA     P.TAB

*--------------------------------
  
   
  
YESERR
	CLR.L   ERR1(A4)            ;ZAP BOTH
	MOVE.W  #1,ERR3(A4)         ;NORMAL
	CLR.W   GLOBAL.F(A4)      ;LIKE 'CHEAT'
	RET
NOERR
	MOVE.L  #$00010001,ERR1(A4) ;SET BOTH
	MOVE.W  #2,ERR3(A4)
	ST      ERR4(A4)
	RET
GLOBAL
	ST      GLOBAL.F(A4)      ;LIKE 'CHEAT'
	RET
*--------------------------------
	
NPC.OK
	
*DETERMINE WHETHER NPC IS CONSCIOUS
*ENTRY:
*D0,A0 NPC INFO
*EXIT:
*EQ = OK
*NE = NOT OK
	
	PUSH_L   D1/A3
	BTST    #1,(A0)            ;DEAD
	BNE.S   99$                ;EXIT
	DO	GETNPC	            ;GETDATA
	MOVE.B  2(A3),D1
	AND.B   #$F0,D1            ;JUST HI-NIBLE
	CMP.B   #$50,D1            ;UNCON
	BEQ.S   98$                ;NO GOOD
	CMP.B   #$40,D1            ;ALSEEP
98$
	EOR     #4,CCR             ;INVERT
99$
	PULL_L   D1/A3
	RET

*--------------------------------
	
IFANIMAL
	
*IF TRYING TO TALK TO AN ANIMAL
	
*ENTRY:
*D0=NPC
	
*EXIT:
*EQ = IS NON-ANIMAL
*NE = IS ANIMAL, MESSAGE PRINTED.
	                        
	CALL    SP.ODREP         ;DO ODD REPLY TYPE
	DO	GETNPC	          ;TO A3
	BTST    #2,2(A3)         ;ANIMAL?
	BEQ.S   90$              ;NO
	DO	P.TN
	MSG	WPRI           ;WOULD PROBABLY REPLY IF
	GETINFO
	DO	P.PRN
	MSG	COULD
	SETNE
90$
	RET

*--------------------------------

	END

