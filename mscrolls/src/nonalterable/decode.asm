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
*	@(#)decode.asm	2.10
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.DECODE
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include "verbequ.asm"


	XDEF    DECODE,NOTOUT
	XDEF    NPCCMDPT,DEC.N,CLRADJ,NEWST,CONVSTART,DECODE.LAB

	XREF    SUBJECT,CHEAT2,PRTHEX,YESERR,INTEXT,INBUFF
	XREF    ERR4,INITCOM,EXTRA,GETADJPH,STCONJ
	XREF    AGAIN,STOTXT,EXTRAPNT,GO,SCREEN,DO.SING
	XREF    PEND2,DIRECT,VERB,CONVSE.V,SPEECH,PARVERB
	XREF    EXTRAFIX,MNPH,DATIVE,TEMPBUF,NOUNBUF,DO.THEN
	XREF    DO.CONJ,DO.THEN2,MVERB_L,ADVERB,GETWRD,DSTAND
	XREF    W.CTEXT,ADJBUF,ADJPNTR,BRKWCH,INCNOUN,SPIEL,NPCINRM
	XREF	WRDPNT,BACK.WRD,NOERR
	
      IFEQ	THE_PAWN

	XREF	DoScenery

      ENDC

     IFNE     BookProtection

	XREF    PROTECT
     
     ENDC
 
*--------------------------------
* 
* DECODE - 
*
* CONVERTS  INBUFF (ASCII WORDS)
* 
* ON EXIT
*
* NOUN    = # IN NOUNBUFFER
* CONJ    = CONJNUM # 
* ADDNOUN = EXTRA NOUN
*
*
*--------------------------------

DECODE
 
	CLR.L   SUBJECT(A4)
 
      IFNE    YCHEAT
	TEST_W  CHEAT2(A4)
	BEQ.S   10$
	MOVE.L  A7,D6
	CALL    PRTHEX
10$
	
      ENDC
 
	DO	YESERR


      IFNE      BookProtection
	CALL    PROTECT          ;This does the word protection with books
	                         ;this can be moved to anywhere else if
	                         ;it gets yukky.!
      ENDC


	CALL    INTEXT
   
NOTOUT
 
	CLR.W   ERR4(A4)
	CALL    INITCOM

      IFEQ	THE_PAWN

	XREF	DecodeSpecials

	CALL	DecodeSpecials		;things that want to tweek parser's
					;view of the world (like NearNPC)
      ENDC

	CLR.W   EXTRA(A4)
 
DECODE.LAB
 
10$
 
	CLR.B   D3                 ;NOUN FLAG
	DO	GETADJPH
	TEST_W	BRKWCH(A4)	   ;break context- direct which?
	BEQ.S	15$		   ;no- carry on
	CLR.L	D7		   ;zap error.
15$
	TEST_B  D7
	BNE.S   DECODE
	CMP.B   #SECT.CONJ,D6
	BEQ     STCONJ
	CMP.B   #SECT.SPEC,D6
	BNE.S   11$
	CMP.B   #$07,D5
	BEQ     AGAIN
	TEST_W  D5                 ;AND?
	BEQ.S   DECODE.LAB
	CMP.W   #8,D5              ;THEN
	BEQ.S   DECODE.LAB
11$
	CALL    STOTXT

NPCCMDPT

	CMP.B   #SECT.DIR,D6       ;DIRECTION
	BNE.S   13$
	TEST_W  D7                 ;END?
	BNE.S   14$                ;NO

	MOVE.L  A6,EXTRAPNT(A4)
	ST      EXTRA(A4)          ;FLAG
14$
	CALL    GO
	DOGO	SCREEN
13$
	CMP.B   #SECT.CONT,D6      ;CONTROL
	BEQ     DO.SING
	CMP.B   #SECT.VERB,D6
	BEQ.S   12$
	   CALL	   CONVSTART			;must it be conversation anyway?
	   BEQ	   SPIEL				;yes,
	TEST_W  PEND2(A4)
	BMI     DIRECT
	MOVE.W  PEND2(A4),D5
	LEA     INBUFF(PC),A6
	BRA     STCONJ
12$
	MOVE.W  D5,VERB(A4)
	CALL    CONVSE.V           ;SAY GROUP
	BEQ     SPEECH
	CALL    PARVERB            ;PARSE A VERB
	CMP.W   #1,D7
	BEQ     EXTRAFIX           ;ENMD
DEC.N
	MOVEQ   #-1,D6             ;INCASE OF E O S
	DO	MNPH    
	TEST_L  DATIVE(A4)
	BNE     EXTRAFIX
 
	LEA     TEMPBUF(PC),A0
	LEA     NOUNBUF(PC),A1
	
 
10$
	MOVE.W  (A0)+,(A1)+
	BNE.S   10$
11$
	CMP.B   #1,D7              ;MATCH NOT FOUND.
	BHI     NEWST 
	CMP.B   #15,D6             ;NOT
	BNE.S   20$
	TEST_W  D5
	BEQ     DECODE
20$
	CMP.B   #SECT.SPEC,D6
	BNE.S   30$                ;NOT SPECIAL
	CMP.B   #$08,D5
	BEQ     DO.THEN            ;PROCESS MULTIPLE COMMAND
	TEST_W  D3			  ;hanging 'and' ?
	BEQ.S   30$			  ;yes,
	TEST_W  d7			  ;can be end?
	BNE.S   30$
	TEST_W  D5			  ;and?
	BEQ     DO.THEN	   		 ;yes, multiple command
30$
	CMP.B   #SECT.CONJ,D6  
	BEQ     DO.CONJ            	;MEETS CONJUNCTION  
	CMP.B   #SECT.VERB,D6      	;VERB
	BNE.S   33$			;nope
	CMP.W   #VNEXIT,D5		;out? (get key out)
	BNE  	DO.THEN2		;no, it is an ordinary verb
	TEST_W	D3			;get key and out?
	BEQ   	DO.THEN2		;yes, proc as 2 commands
	BRA.S 	34$			;next word & continue
33$
	CMP.B   #SECT.DIR,D6
	BNE.S   BadContext
	CMP.B   #8,D5
	BLT     DO.THEN2           	;IF A SENSIBLE DIRECTION
	CMP.W   #$FFFF,ADVERB(A4)	;no adj? (NB: can't BMI)
	BNE	DO.THEN2		;yes, an adj, assume a new command
	TEST_W  D3			;cant be a PUT X DOWN if AND exists  
	BEQ	   DO.THEN
	LEA     MVERB_L(PC),A1
32$
	MOVE.B  (A1)+,D1           ;GET A VERB
	CMP.B   #-1,D1             ;TERM?
	BEQ     DO.THEN2           ;NOT IN LIST  
	CMP.B   VERB+1(A4),D1      ;SAME?
	BNE.S   32$                ;NO, TRY ANOTHER
	BSET    #15,D5             ;SET HI-BIT
	MOVE.W  D5,ADVERB(A4)      ;SET AS PART OF VERB 
34$
	DO	GETWRD
	TEST_W  D7                 ;END?
	BEQ     11$
    
BadContext

	TEST_W  D7                 ;E O S
	BNE     EXTRAFIX

     IFEQ	THE_PAWN

	CMP.W	#SECT.SCEN,D6		;scenary noun?
	BEQ	DoScenery

     ENDC

	CALL    DSTAND
	MSG	CTEXT
	DOGO	DECODE
 
CLRADJ
 
	MOVE.L  A0,-(A7)
	LEA     ADJBUF(A4),A0
	MOVE.L  A0,ADJPNTR(A4)
	MOVE.L  (A7)+,A0
	CLR.W   ADJBUF(A4)
	RET
 
*--------------------------------
	
NEWST
	
*START ANOTHER COMMAND WO 
*PROPERLY FINISHING OLD ONE
      
	TEST_W  BRKWCH(A4)         ;did we get a verb in response to a 'which..'
	BEQ     DECODE             ;EQ => nope, so carry on as per normal
	LEA     INBUFF(A4),A6      ;POINT TO START
	BRA     NOTOUT             ;PROCESS NEXT SENTANCE
   
*--------------------------------

CONVSTART
*
*	checks first word to see if it must be conversation
*	directly from the command line.
*	entry:
*		expects (d5,d6) to be current word
*	exit:
*		EQ=> yes
*		NE=> no

	TEST_W	NPCINRM(A4)		;npc around?
	BMI.S	80$			;nope, fail
	CMP.W	#SECT.QUEST,D6		;question type
	BEQ.S	90$			;yes, ok
	CMP.W	#SECT.INSULT,D6		
	BEQ.S	90$
	CMP.W	#SECT.YOU,D6
	BEQ.S	90$
	CMP.W	#SECT.HELLO,D6
	BEQ.S	90$
80$
	SETNE				;fail
	RET
90$
	MOVE.L	D5,-(A7)
	MOVE.W	NPCINRM(A4),D5
	CALL 	INCNOUN
	CALL	BACK.WRD		;move back, to adjust for SPEECH
	MOVE.L	A6,WRDPNT(A4)
	MOVE.L	(A7),D5
	MOVE.W	#VNSAY,VERB(A4)		;fake as say 
	DO	NOERR			;gag parser for later
	CLR.L   (A7)+			;set eq	
	RET


*--------------------------------

	END
