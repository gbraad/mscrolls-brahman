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
*	@(#)context.asm	2.18
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.CONTEXT
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include "macros.asm"


      XREF  QCARIED2,WORD_,VERB,VERBTYPE,VALID.NP,VALID.WO,RND.D1
      XREF  MATCHBUF,SECTION_,GETDATA,ISDOFRE

      XDEF  CONTEXT
    

*
* ContextHelp
*
* This susses if the verb is of the sort context can't help out
* such as LOOK/EXAMINE/PUT
*
* Returns NE if context CAN help
*

ContextHelp

	LEA	VList_ContextHelp(PC),A0
	MOVE.W	VERB(A4),D0
10$
	MOVE.B	(A0)+,D1
	CMP.B	#-1,D1			;-1 term cause LOOK is in list
	BEQ.S	80$			;end of list
	CMP.B	D1,D0			;in list?
	BNE.S	10$
	RET
80$
	SETNE
	RET


VList_ContextHelp

	SVERB	LOOK
	SVERB	PUT
	SVERB	EXAMINE
      
	SVERB	OPEN			;not too sure

	SVERB	CLOSE

	DC.B	-1


	ALIGN

*--------------------------------
*
*  CONTEXT.SUB - GETS A WORD_
*                FROM MATCHBUF
*                (A1) AND SUSSES
*  IF ITS ON INVENT
*
*  ENTRY: A1 - POINTS AT BUFFER
*    
*  EXIT:  D6.B = -1 = ON INV
*
*  IF END OF BUFFER REACHED THEN
*  RETURNS NORMALLY, ELSE RETURNS
*  +2
*
* IE.  CALL    CONTEXT.SUB
*      BRA.S   END.OF.BUFFER
*      ....    NOT.END 
*
*--------------------------------
 
 
CONTEXT.SUB
 
10$
 
	MOVE.L  (A1)+,D0           ;WORD_ #
	BMI.S   20$                ;END OF MATCHBUF
	ADDQ.W  #1,D0              ;CONVERT WORD_# TO NOUN#
	MOVE.W  #$C800,D2          ;MASK ROOM/INTRINSIC/OUT
	CALL    QCARIED2          ;RET NE => ON INVE
	SEQ     D6                 ;NOW, D6.B = FF => ON INVE
	ADDQ.L  #2,(A7)            ;RET TO NORM+2
20$
	RET
 
 
*--------------------------------
*
*  CONTEXT
*
* DEDUCES WHICH NOUN TO USE IF
* MORE THAN ONE OF SAME ROOT WORD_
* PRESENT,
*
* EG.  G KEY = ONE NOT CARRIED
*
*      DR KEY = ONE CARRIED
*
* THEN ZAPS MATCHBUF AS APROP.
*
*--------------------------------
 
 
CONTEXT

 
	PUSH_L  D0-D3/D5-D7/A1 	;DONT SAVE D4!

	DO	ISDOFRE			;does the verb allow GLOBAL nouns?
	BEQ.S	05$			;EQ => yes, so don't fod non-local
	CALL	MurderNonLocal
        
        IFNE    Remastered
        
        TEST_B  D7                      ; were any objects local?
        BEQ.S   05$                     ; no
        CALL_S  KILLROOMS
        CMP.B   #1,D4
        BLS.S   99$                     ; resolved
        ENDC
05$

      IFEQ	THE_PAWN

	XREF	ContextSpecials		;in special0 (probably!)

* Context specials looks/hacks matchbuf, no 'real' params
* but should leave regs as found (bar D4, if successful == 1)
* returns EQ => i resolved it, don't do any more

	CALL	ContextSpecials		;returns EQ => i fixed it folks!
	BEQ	99$			;so ignore rest

      ENDC

	CALL	ContextHelp		;can context (Kens bit) help wi
	BEQ	90$			;this verb? EQ=> nope
 
	LEA     MATCHBUF(PC),A1    ;POINT A1 AT MATCH BUFFER
	MOVEQ   #0,D3              ;NO. OF NOUNS ON INV
	MOVEQ   #0,D1              ;NO. OF NOUNS NOT ON INVE
10$
	CALL    CONTEXT.SUB
	BRA.S   40$                ;END OF BUFFER (*MUST* BE 2 BYTES)
	TEST_B  D6                 ;NE = ON INVE
	BNE.S   20$
	ADDQ.W  #1,D1              ;INC NOT ON INV COUNT
	BRA.S   10$
20$
	ADDQ.W  #1,D3              ;INC ON INV COUNT
	BRA.S   10$
 
40$
	TEST_W  D3                 ;ANY ON INVE?
	BEQ.S   90$                ;NOPE = CANNOT RESOLVE
	TEST_W  D1                 ;ANY NOT ON INVE?
	BEQ.S   90$                ;NOPE = CANNOT HELP
	
	CALL    VERBTYPE           ;RETS D7 = 1 = ON INV
	TEST_B  D7
	SNE     D3                 ;D3.B = FF = ON INV
 
	LEA     MATCHBUF(PC),A1
	MOVE.L  A1,A2              ;COPY POINTER
	MOVEQ   #0,D4              ;NO. OF MATCHES
50$
	CALL    CONTEXT.SUB        ;GET A NOUN
	BRA.S   60$                ;END OF BUFF (*MUST* BE SHORT)
	CMP.B   D3,D6              ;NOUN SAME AS VERB?
	BNE.S   50$                ;NOPE, GET ANOTHER
55$
	MOVE.L  -4(A1),(A2)+       ;YES, PUT IT BACKINTO MATCHBUF
	ADDQ.W  #1,D4              ;KEEP COUNT
	BRA.S   50$                ;NEXT NOUN..
60$
	MOVE.W  #-1,(A2)+          ;TERMINATE MATCHBUF
	CMP.W   #1,D4              ;DID WE RESOLVE?
	BNE.S   90$                ;NOPE
 
	MOVE.W  MATCHBUF(A4),SECTION_(A4)
	MOVE.W  MATCHBUF+2(A4),WORD_(A4)
 
90$
					;case where 2 n exist only try to
					;open the 'openable one'
	CALL_S	PREVERBCONTEXT		;with verbs like open in a 

      IfVerb	MAKELOCAL

	CALL	ChoseNoun		;if verb has FREDO then chose any
					; (random) noun

      ENDC

99$
	PULL_L  D0-D3/D5-D7/A1
	RET

        IFNE    Remastered

* called if we have some local objects in MATCHBUF
* MurderNonLocal will have removed everything else
* This fixes problems like "which one the Cliff or the cliff"

KILLROOMS

        CMP.W   #1,d4                   ; more than 1 object?
        BEQ.S   99$                     ; nothing to do
        
	PUSH_L	D0/D6/A0-A2
       	LEA	MATCHBUF(A4),A2
	MOVE.L	A2,A1			;target buffer

10$
	MOVE.W	(A2)+,D6		;section
	BMI.S	30$			;-1 terminator
	MOVE.W	(A2)+,D0		;word#

        ADDQ.W  #1,D0                   ;to object
        GETINFO                         ;A0 = data
        BTST    #6,6(A0)                ; room?
        BEQ.S   20$                     ; no, keep it
        SUBQ.W  #1,D4                   ; adjust counter
        BRA.S   10$
20$

; object not a room put it back
        SUBQ.W  #1,D0
	MOVE.W	D6,(A1)+
	MOVE.W	D0,(A1)+
	BRA.S	10$

30$
	MOVE.L	#-1,(A1)		;terminate matchbuf
	PULL_L	D0/D6/A0-A2
99$
        RET

        ENDC                    ;remastered

*********************************************************************
*
*  This nifty bit of code will:
*  try to put some intelligence into the context with verbs relating
*  to data of an object.
*  ie: 2 door 1 open 1 closed
*  open door should not return which one?
*  it should automatically assume the open door
*
*  this will work by a byte compare and a mask compare
*  which is set up by the verb.
*
*
*  it will never return zero
*  if there are two doors, both open, and you type open door
*  it will prompt you for which door - even though the answer
*  is gonna be you can't do that to the door
*  this is to allow some hope for open etc.. specials
* 
*  this will also call valid on all items..(yawn)!
*
*  this bit of code was written by AJS
*  it is therefore likely that bugs relating to context
*  should be first looked for here.....
*
*********************************************************************

* open entry d4 => number of things in matchbuf
* matchbuf contains SECT# WORD# -1.w terminated

* on exit d4= number of possibilities
* matchbuf containing them SECT# WORD#

PREVERBCONTEXT

	CMP.W	#1,D4			;only one worth considering?
	BEQ  	99$			;YEAH GOaway
	MOVE.W	VERB(A4),D0		;the verb we are using
	LEA	VERBCONTLIST(PC),A2	;the list of verbs
	CLR.W	D1			;the index...
10$
* let us see if we are handling a verb we can do something with...

	CMP.W	#-1,0(A2,D1.W)		;is it a terminator?
	BEQ	99$			;Yeah no verb...
	CMP.W	0(A2,D1.W),D0		;Is it our verb?
	BEQ.S	20$
	ADDQ.W	#2,D1			;point it to next verb..
	BRA.S	10$
20$
	LEA	VERBCONTDATA(PC),A2	;the data that accompanies this verb
	CLR.W	D6
	CLR.W	D7
	MOVE.B	0(A2,D1.W),D7		;the byte number
	MOVE.B	1(A2,D1.W),D6		;the mask...
	MOVE.W	D7,A3			;a3 points at byte number!
	LEA	MATCHBUF(A4),A1
	CLR.W	D3

* now we scan through the matchbuf finding all the possibilities...
* and counting them up in d3
* d4 still contains original number...(and must continue to do so!)

30$
	MOVE.L	(A1)+,D0		;The word number (always a noun - Ken)
	BMI.S	40$			;all finished for now..(-1 term)
	ADDQ.W	#1,D0			;Converting word no to noun no
	GETINFO				;and get associated info on it
	MOVE.B	0(A0,A3.W),D2		;the appropriate byte
	AND.B	D6,D2			;and it with the mask...
	BEQ.S	30$
	ADDQ.B	#1,D3			;we've found one!
	BSET	#7,-4(A1)		;flag it for future knowledge
	BRA.S	30$			;and retry...
40$
* okay guys how many do we have.../
	TEST_B	D3			;EQ=> go away
	BEQ.S	99$
	LEA	MATCHBUF(A4),A2		;first pointer
	MOVE.L	A2,A1			;Second pointer
50$
	CMP.W	#-1,(A1)		;reached end?
	BEQ.S	60$
	ADDQ.W	#4,A1
	BCLR	#7,-4(A1)		;was it flagged?
	BEQ.S	50$
	MOVE.L	-4(A1),(A2)+		;move it into the matchbuf
	BRA.S	50$
60$
	MOVE.W	D3,D4			;new number of things...
	MOVE.W	#-1,(A2)+		;terminate matchbuf
	CMP.W	#1,D4
	BNE.S	99$
	MOVE.W  MATCHBUF(A4),SECTION_(A4)
	MOVE.W  MATCHBUF+2(A4),WORD_(A4)
99$
	RET


VERBCONTLIST:
	DC.W	VNOPEN
	DC.W	VNCLOSE
	DC.W	VNUNLOCK
	DC.W	VNLOCK
	DC.W	VNGET		; moveability
      
      IfVerb	MAKELOCAL
	DC.W	VNMAKELOCAL
      ENDC

	DC.W	-1

* The format is BYTE # of data
* And bit mask to be ANDED with 
* ie:
* if you want something TO BE closeable
* Bit 4 of BYTE 0 must be 1 in the MASK
* (the opened bit!)

VERBCONTDATA:
	DC.B	0
	DC.B	%01100000
	DC.B	0
	DC.B	%00010000
	DC.B	0
	DC.B	%01000000
	DC.B	0
	DC.B	%00100000
	DC.B	3			; get
	DC.B	%00010000		; if in doubt moveable..

      IfVerb	MAKELOCAL
	DC.B	4
	DC.B	%00010000
      ENDC



*
* MurderNonLocal
*
* this kills everything in MATCHBUF except local NOUNS
* hopefully it will let CONTEXT etc avoid doing the same thing twice
*
* D7 = 0 => we didn't apply 
* D7 != 0 => we did something (at least one thing was local)

MurderNonLocal

	PUSH_L	D0-D3/D5-D6/A0-A6	;not D4 - count of words
	MOVE.L	D4,-(A7)

	LEA	MATCHBUF(A4),A2
	MOVE.L	A2,A1			;target buffer
	MOVEQ	#0,D4			;'good' count

10$

* get a word from matchbuf

	MOVE.W	(A2)+,D6		;section
	BMI.S	20$			;-1 terminator
	MOVE.W	(A2)+,D0		;word#
	CMP.W	#SECT.NOUN,D6
	BNE.S	10$

	DO	VALID.WO		;valid given WORD no.
	TEST_W	D7
	BNE.S	10$

	ADDQ.W	#1,D4

* put a valid word back

	MOVE.W	D6,(A1)+
	MOVE.W	D0,(A1)+
	BRA.S	10$

20$
	TEST_W	D4			;were any valid?
	BEQ.S	90$			;no, forget whole deal

	POPSP				;kill old D4 on stack
	MOVE.L	#-1,(A1)		;terminate matchbuf
        ST      D7                      ;flag that we did something
	BRA.S	99$

90$
        CLR.B   D7                      ;flag didnt apply
	MOVE.L	(A7)+,D4
99$
	PULL_L	D0-D3/D5-D6/A0-A6
	RET


*
* ChoseNoun - so makelocal room won't give too much hassle
*

ChoseNoun

	DO	ISDOFRE
	BNE.S	99$

	MOVE.L	A2,-(A7)	
	MOVE.W	D4,D1		;no of possible nouns
	RND_D1
	LEA	MATCHBUF(A4),A2
10$
	CMP.W	#SECT.NOUN,(A2)
	BEQ.S	20$
	ADDQ.W	#4,A2
	BRA.S	10$
20$
	MOVE.L	(A2)+,D0
	DBMI	D1,10$
	BMI.S	90$
	MOVE.L	D0,MATCHBUF(A4)
	MOVE.L	#-1,MATCHBUF+4(A4)
	MOVEQ	#1,D4
90$
	MOVE.L	(A7)+,A2
99$
	RET
       
       END
