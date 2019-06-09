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
*	@(#)parser3.asm	2.14
*	5/16/87
*
******************************************************

	SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PARSER3     
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"

FIXNUM	EQU		4


	XDEF	INCNOUN,SYNONYM,STCONJ,GETADJPH,STOTXT,RCLTXT,INITCOM,INTEXT
	XDEF	DONEB4,SAVEANS,ZAPWHFIX,EDITCMD

	XREF	NOUNBUF,NOUN,SYN_L,PEND2,OLDBUF,PEND,VERB,VERB2,OLDVERB,NOTWSYN
	XREF	GETWRD,ERR4,NOADJ,PICKONE,POSWRD,FROMWCH,ADJECTIV
	XREF	OLDTEXT,INBUFF,BRKWCH,D72,WORD_,CONJBUF,EXNUM,ADDBUF,SECTION_
	XREF	ADJBUF,ADJPNTR,ADDNOUN,CONJ,CONJPNTR,NOUNBUF2,NOUNBUF3
	XREF	GETNUM,DEPTH,WHOOPS,ADVERB,NEG_,DBSFLG,IGNORQ,UNPAUSE,PRTTXT
	XREF	CHEAT,LINE,CHEAT2,USER.DB,UTIL.BS,INPUT,INFORMAT,ADDPNTR,DO.ADJ
	XREF	CMDPNT,NOERR,YESERR,OLDWRDNUM,WHICHFIX,FNDWRD,WDEPTH
	XREF	QUIETWH,RELTXT,DO.INONLY,SCREEN,PADV,PVERB,OLDADVERB

INCNOUN
  
	LEA     NOUNBUF(A4),A3
	ADD.W   NOUN(A4),A3
	MOVE.W  D5,(A3)
	ADDQ.W  #$2,NOUN(A4)
	RET
 
*--------------------------------
*
* SYNONYM - CONVERTS IF A SYN.
*
* D5 = WORD,  D6 = SECTION
*
*
*--------------------------------
  
  
SYNONYM
  
	CMP.B   #SECT.SYN,D6 		;IN SYN LIST
	BNE.S   90$
	MOVE.L  A0,-(A7)
	LSL.W   #1,D5        		;*4
	LEA     SYN_L(A4),A0 		;START
	MOVE.W  0(A0,D5),D5  		;GET WORD & SECTION
	MOVE.W  D5,D6
	AND.W   #$1F,D6      		;SECT NUMBER
	LSR.W   #5,D5        		;GET WORDNUMLOW
     
	MOVE.L  (A7)+,A0 
90$
	RET
 
*--------------------------------
   
STCONJ
 
	TEST_W  PEND2(A4)
	BMI.S   20$
	PUSH_L  A0/A1
	LEA     OLDBUF(A4),A0
	LEA     NOUNBUF(A4),A1
10$
	MOVE.W  (A0)+,(A1)+
	BNE.S   10$
	PULL_L  A0/A1
	BRA.S   30$
20$
	CALL	DO.INONLY
	CMP.W   #2,D7			;couldnt go there
	BNE.S	30$
	DOGO	SCREEN
30$
	MOVE.W  #1,PEND(A4)
	MOVE.W  OLDVERB(A4),VERB(A4)
	MOVE.W	OLDADVERB(A4),ADVERB(A4)
	BRA     NOTWSYN
 
*--------------------------------
    
   
GETADJPH

	CALL	DONEB4			;has it already been resolved?
	MOVE.L  A6,-(A7)
	TEST_W  WDEPTH(A4)
	BNE.S   01$
	MOVE.L  A6,OLDWRDNUM(A4) 	;save this as original position
01$
	DO	GETWRD
	CMP.B   #4,D7			;which one?
	BEQ	EDITCMD			;insert new text.
	TEST_B  D7
	BNE.S   90$
	CMP.B   #SECT.ADJ,D6
	BNE.S   90$
	CALL    DO.ADJ
	BNE.S   90$
	CMP.B   #4,D7         		;suss problems with small sphere?
	BEQ	EDITCMD
	TEST_W  ERR4(A4)     		;ESCAPE?
	BNE.S   90$          		;YES

* this mod (poping 4 rather than 8 made 4/12/86 to stop 'put all on adj'
* crash - side effects not totally clear yet

*	PULL_L  D0/D1
	POPSP

	BRA     NOADJ
90$
	MOVE.L  (A7)+,D0
91$
	RET

*--------------------------------
  
STOTXT
 
	LEA     INBUFF(A4),A0
	LEA     OLDTEXT(A4),A1
10$
	MOVE.B  (A0)+,(A1)+
	BNE.S   10$
	RET

*--------------------------------

SAVETXT
 
	LEA     INBUFF(A4),A0
	LEA     RELTXT(A4),A1
10$
	MOVE.B  (A0)+,(A1)+
	BNE.S   10$
	RET
  
*--------------------------------
   
RCLTXT
 
	LEA     INBUFF(A4),A0
	LEA     OLDTEXT(A4),A1
10$
	MOVE.B  (A1)+,(A0)+
	BNE.S   10$
	CLR.B   (A0)
	CLR.B   1(A0)
	CLR.B   2(A0)  
	RET

*--------------------------------

RCLSAVE
 
	LEA     INBUFF(A4),A0
	LEA     RELTXT(A4),A1
10$
	MOVE.B  (A1)+,(A0)+
	BNE.S   10$
	CLR.B   (A0)
	CLR.B   1(A0)
	CLR.B   2(A0)  
	RET
 
*--------------------------------
* INITCOMMAND
* RESETS VARIBLES FOR NEW COMMAND
*--------------------------------
 
 
INITCOM
 
	MOVEQ   #-1,D1
	CLR.W   WORD_(A4)
	MOVE.W  VERB(A4),OLDVERB(A4)
	MOVE.W  D1,VERB(A4)
	MOVE.W  D1,VERB2(A4)  	;incase VERB = use
	MOVE.L  D1,CONJBUF(A4)
	CLR.W   NOUN(A4)
	CLR.W   EXNUM(A4)
	CLR.W   ADDBUF(A4)
	CLR.B   SECTION_(A4)
	LEA     ADJBUF(A4),A0
	MOVE.L  A0,ADJPNTR(A4)
	LEA     ADDBUF(A4),A0  
	MOVE.L  A0,ADDPNTR(A4)
	LEA     CONJBUF(A4),A0  
	MOVE.L  A0,CONJPNTR(A4)
	MOVE.L  D1,ADDNOUN(A4)
	MOVE.L  D1,CONJ(A4)
	CLR.L   NOUNBUF(A4)
	CLR.L   NOUNBUF2(A4)
	CLR.L   NOUNBUF3(A4)
	CLR.W   ADJBUF(A4)
	CLR.W   GETNUM(A4)      	;GET COUNTER
	CLR.W   DEPTH(A4)       	;ZERO RECUSTION DEPTH
	CLR.W   BRKWCH(A4)
	CLR.W   WHOOPS(A4)
	MOVE.W  D1,ADVERB(A4)
	CLR.W   PEND(A4)
	CLR.W   NEG_(A4)
	CLR.W   DBSFLG(A4)
	MOVE.L  D1,IGNORQ(A4)		;CLR BOTH
	CALL	ZAPWHFIX
	CLR.W   WDEPTH(A4)		;for which nesting
	MOVE.W  D1,PADV(A4)		;clear pending adverb
	MOVE.W  D1,PVERB(A4)		;clear pending verb

    IFEQ	THE_PAWN
	XREF	NearNPC,FoundTO
	MOVE.W	D1,NearNPC(A4)		;fod semi-local NPC
	SF	FoundTO(A4)		;kill flag for 'use x TO verb y'
					;as used in GW07/VerbOrNoun (same code)
    ENDC


	BRA     UNPAUSE 
      
*--------------------------------
  
INTEXT

    XREF	MakeUndoDiff
	CALL	MakeUndoDiff

	MSG    LINE
01$
	
	IFNE     YCHEAT
	
	TEST_W  CHEAT(A4)
	BEQ.S   02$
	PRINT   <'---'>
02$
	TEST_W  CHEAT2(A4)
	BEQ.S   03$
	CALL    PRTTXT
	DC.B     '< ',0
	ALIGN

	BRA.S   04$
03$
	ENDC

    IFNE	THE_PAWN
	TEST_B  USER.DB(A4) 	;IN 'USER' DEBUG MODE?   
	BEQ.S   06$         	;EQ=NOPE
	PRINT   <'] '>        	;YES - CHANGE PROMPT
	BRA.S   04$
06$
    ENDC

	CALL    PRTTXT
	DC.B    '> ',0
	ALIGN
04$

	DO	UTIL.BS
	CALL    INPUT
	TEST_B  INBUFF(A4)
	BEQ.S   01$
	CALL    INFORMAT
	TEST_B  D7
	BNE.S   INTEXT
	RET

*--------------------------------------

GSILENT
*
*	get a word silently, for use in recursive getadjph(s)
*	if it is not an adj carry on..
*
	MOVE.L	A6,-(A7)
	MOVE.L	POSWRD(A4),-(A7)	; save this lot.
	MOVE.W	ERR4(A4),-(A7)
	ST	ERR4(A4)		;non-zero
	ST  	QUIETWH(A4)
	ADD.W	#1,WDEPTH(A4)		;keep track of depth
	DO	GETWRD			;ok go for it
	SUB.W	#1,WDEPTH(A4)
	MOVE.W  (A7)+,ERR4(A4)		;restore error condition
	CLR.W	QUIETWH(A4)
	CMP.W	#4,D7			;nested which?
	BEQ.S	10$			;recurse for it.
	ADD.L	#8,A7			;loose above pushes
	RET
10$
	MOVE.L	(A7)+,POSWRD(A4)
	MOVE.L	(A7)+,A6		;get these back.
	ST	FROMWCH(A4)		;since cleared above.
	ADD.W	#1,WDEPTH(A4)		;keep track of depth
	DO	GETADJPH		;try the main routine
	SUB.W	#1,WDEPTH(A4)
	RET

*--------------------------------------

DONEB4
*	doesnt come back if successful
*	keys on a6 and wdepth
* 	info packet:
*	wrdnum.l,wdepth.w,result.w,newa6.L

	PUSH_L	D5/A0
	LEA	WHICHFIX(A4),A0		;start of list
10$
	MOVE.L	(A0)+,D5			
	BEQ.S	90$			;no, end of list
	CMP.L	A6,D5
	BNE.S	30$			;no, try another
	MOVE.W	(A0)+,D5		;get depth
	CMP.W	WDEPTH(A4),D5		;must be at the same depth
	BEQ.S	20$			;yep, a match!
	ADDQ.L	#6,A0			;skip
	BRA.S	10$			;next
30$
	ADDQ.L	#8,A0			;skip other bytes
	BRA.S	10$			;next
20$
	MOVE.W	(A0)+,D5		;get noun
	MOVE.L	(A0),A6			;then new text position.
	MOVEQ	#SECT.NOUN,D6		;set up as done.
	ADD.W	#$0C,A7			;forget this routine & 2longs
	RET
90$
	PULL_L	D5/A0
	RET				;continue.

*----------------------------------------- 

SAVEANS
*	
*	entry:
*	d5.w= result noun
*	cmdpnt contains resultant text position
*

	PUSH_L	D0/D1/A0		;save to be neat
	MOVE.L	CMDPNT(A4),A0
	CMP.L	OLDWRDNUM(A4),A0	;must be bigger
	BLS.S	20$			;no, fail
	LEA	WHICHFIX(A4),A0		;start
	MOVEQ	#0,D1			;safety counter.
10$
	ADDQ.W	#1,D1			;inc counter.
	TEST_L	(A0)			;find end
	BEQ.S	30$			;found a space
	ADD.L	#$0C,A0			;next
	BRA.S	10$			;try another
30$
	CMP.W	#FIXNUM,D1
	BHI.S	20$			;too high, something is wrong.
	MOVE.L	OLDWRDNUM(A4),(A0)+
	MOVE.W	WDEPTH(A4),(A0)+	;save depth
	MOVE.W	D5,(A0)+
	MOVE.L	CMDPNT(A4),(A0)		;save after pos as wel
20$
	PULL_L  D0/D1/A0		;restore
	RET

*----------------------------------------- 

ZAPWHFIX

	MOVE.L	A0,-(A7)
	MOVE.W	#FIXNUM-1,D0
	LEA	WHICHFIX(A4),A0
10$
	CLR.L	(A0)+
	CLR.L	(A0)+
	CLR.L	(A0)+			;clear 12 bytes
	DBRA	D0,10$
	MOVE.L	(A7)+,A0
	RET

*------------------------------------------

ADJUSTTEXT

*	fix the whichfix list, since extra text has been inserted
*	start at CMDPNT and add 'a1'
*	d0 is the point at which the adjective is to be inserted.

	PUSH_L	D1/D2/A3
	LEA	WHICHFIX(A4),A3		;start of table (each is 12 bytes)
	MOVE.L	A1,D2
10$
	MOVE.L	(A3),D1			;start adress
	BEQ.S	90$			;end
	CMP.L	D0,D1
	BCS.S	20$			;before, not affected.
	ADD.L	D2,(A3)+		;adjust
	ADDQ.L	#4,A3			;to end pointer
	ADD.L	D2,(A3)+		;next
	BRA.S	10$			;anymore?
20$
	ADD.W	#$0C,A3			;skip to next
	BRA.S	10$
90$
	PULL_L	D1/D2/A3
	RET

	
*--------------------------------
EDITCMD

*  perform the text edit on the command line of inserting
*  adjectives for 'which' resolution.

*  NB: code factored from getadjph.


	TEST_W  ERR4(A4)     		;NO WHICHES?
	BEQ.S   11$          		;ok, do which.
	TEST_W  QUIETWH(A4)		;supress pickone (muck about)
	BNE     90$			;yes, just quit!
	CALL    PICKONE			;CHOOSE ONE ARBITRARILY
	MOVEQ   #0,D7
	BRA     90$			;EXIT
11$
	TEST_W	WDEPTH(A4)		;only on top level
	BNE.S   13$
	CALL    STOTXT
	MOVE.L  A6,CMDPNT(A4)		;save command point
13$
	MOVE.L  POSWRD(A4),-(A7)
	MOVE.W  D3,-(A7)
	CALL	SAVETXT			; seconadary buffer used for rels
	CALL    INTEXT
	MOVE.W  (A7)+,D3
	ST      FROMWCH(A4)		;FLAG TO USE DATA.NOUN.BUF ALSO
	CALL	GSILENT			;get right type sneakily
	CLR.W   FROMWCH(A4)
	TEST_B  D7
	BNE     25$
	CMP.B   #SECT.ADJ,D6
	BEQ.S   39$
	CMP.B   #SECT.NOUN,D6		;IF NOUN ACCEPT IT
	BNE     20$			;EXIT AS ERROR
	CALL    SAVEANS			;is a single noun, save it
	TEST_W  WDEPTH(A4)
	BNE.S   14$
	CALL    RCLTXT			;recal text incase of a relph
	MOVE.L  CMDPNT(A4),A6		;back
14$
	MOVE.L  (A7)+,POSWRD(A4)
	ADDQ.L  #4,A7			;dump old A6
	
	RET
39$
	MOVE.L  (A7)+,A6
	CALL	RCLSAVE			;get back second buffer
	MOVE.L  A6,A5
	LEA     ADJECTIV(A4),A1
	MOVE.W  D5,D0
	BEQ.S   30$
40$
	TEST_B  (A1)+
	BPL.S   40$
	SUBQ.W  #1,D0
	BNE.S   40$
30$
	MOVE.L	A6,D0			;save for use in adjusttext
	MOVE.B  (A1)+,(A6)+
	BPL.S   30$         		;COPY ADJECTIVE
	MOVE.B  #$20,(A6)
	BCLR    #7,-(A6)    		;ZAP HI-BIT
	ADDQ.L  #2,A6

	TEST_W  WDEPTH(A4)		;top level?
	BNE.S   60$			;no.
	MOVE.L  A6,A1
	SUB.L   A5,A1			;length of adjective
	CALL	ADJUSTTEXT		;fix the WHICHFIX list
60$
	ADD.L   #RELTXT,A5
	SUB.L   #INBUFF,A5
70$
	MOVE.B  (A5)+,(A6)+ 		;REST
	BNE.S   70$
	CLR.B   (A6)			;an additional zero (weird?)
	MOVE.L  (A7)+,A6    		;OLD PHRASE
	DOGO	GETADJPH
20$
	PULL_L  D0/D1 			;RET A6, POSWRD
	ST      BRKWCH(A4)  		;BREAK OUT OF CURRENT CONTEXT
	DOGO	D72
 
25$
	PULL_L  D0/D1
	DOGO	D72

90$
	MOVE.L	(A7)+,D0
	RET
 
*--------------------------------

	END
