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
*	@(#)screen.asm	2.23
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SCREEN1
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"



      IFNE     Graphics

	XREF    PICTURES

      IFNE	C64

	XREF	Cameos

      ENDC

      ENDC

	XREF    GETNPC,P.SCN,GETDATA,VERB,INIT.TAB,W.EH,YESERR
	XREF    W.IDFY,EXTRA,GETVECT,RIDPRT,SCRPNT,R.NOUN,DECODE,PEND2
	XREF    GETPRON,LINE,PRTHEX,ADDNOUN,ADVERB,CONJ,CHEAT,CHEAT2
	XREF    STOPBRK,CHK.OWN,VERBOWN,DBS,P.NORCH,WHENSAT
	XREF    SEATED,VALID,VALID.NP,ISDOFRE,WHOOPS,NOUNBUF,ISTIETOG
	XREF    USE,REP1,TWOPV,W.THERE,W.I2DARK,LIGHT,NPCINRM
	XREF    LASTNPC,GETRELN,DNOUNBUF,P.LIST,CHAROUT,NOUN
	XREF    UTIL.BS,EXTRAPNT,DO.DEATH,DIED,SPECIALS
	XREF    W.ARE,W.HERE,W.IS,ISARE,ONLYREL,ISANYREL,USER.DB
	XREF    OUTMODE,SATMSG,P.NOUN,SINGV.TB,VLN.SING
	XREF    CAPITAL,P.LOC,SP.LIGHT,ROOM,NOTOUT,PRTTXT
	XREF    EVALWHT,ENERGY,CLOCK,BIOS,VLN.DARK,VLT.DARK,FMSG
	XREF	DO.GOIN,PADV,PVERB,INTEXT,DECODE.LAB,SING.EX,P.VERB
	XREF	P.PARTICLE,BIOSA4,P.Refresh,GameFinished


	XDEF    REALBAD,SCREEN,PROCESS,PROC.LAB,SC70
	XDEF    PROC.EX,SINGV,P.INIT,SETPAUSE

	XDEF	FINNPC,KILLNOTNPC,KILLNPC,KILLPSEUDO
	XDEF	FINRELNPC

*--------------------------------
*
* REALBAD   -  HMM IF THE PROG DOES AN
*              EXTRA RET WE HAD BETTER
*              STOP IT CRASHING
*
*  SO FAR ONLY (!?!) FRED ABOUT .....     fixed this! (do I leave fudge in?)
*  SEEMS TO NEED THIS!
*
*--------------------------------
 
REALBAD
 
*       LEA     BIOS-$20(PC),A7    ;PUT STACK @ SENSIBLE PLACE
*       PEA     REALBAD(PC)
 
SCREEN

      IFNE	VAX&YCHEAT

	CMP.L	#$1F400,A7
	BEQ.S	05$
	PRINT	<'Waaaa - unhealthy sp!!!!!!^'>
05$
      ENDC
 
	CALL	BIOSA4        ;SO THAT  *32698G WONT KILL MACHINE! 
  
  
	DO	YESERR             ;INCASE FROZEN
	MSG     LINE
 
	TEST_B  DIED(A4)           ;DID WE DIE LAST MOVE?
	BNE     DO.DEATH           ;YES - PROCESS DEATH
  
	SUBQ.W  #1,ENERGY(A4)      ;LIMIT ON MOVES
	ADDQ.W  #1,CLOCK(A4)       ;MOVE TIMER    
	CALL    EVALWHT
	DO	R.NOUN
	GETINFO        
	CALL    SP.LIGHT           ;! - CALC 'LIGHT' FOR THIS GO

      IFEQ	THE_PAWN

	XREF	SPECIALS_Before

	CALL	SPECIALS_Before

      ENDC


	TEST_W  SCRPNT(A4)		;CHANGED ROOM?
	BEQ.S	10$			;nope, only do top line
	CALL	P.Refresh
	BRA.S   NOTNEWROOM
10$
	DO	R.NOUN
	CALL	P.LOC

NOTNEWROOM

	MSG     LINE
	CALL    SPECIALS           ;do timers, misc specials
	MSG     LINE
	TEST_B  DIED(A4)           ;SPECIALS KILLED U?
	BNE     DO.DEATH           ;YES - GO SUSS HOW DEAD
	TEST_B	GameFinished(A4)	; Is the game over?
	BNE	DO.FINAL


      IFNE    YCHEAT
	
	TEST_W  CHEAT2(A4)         ;'DEBUG BAND' info
	BEQ.S   10$
	MSG     LINE
	MOVE.L  A7,D6              ;print current stack pointer, should ALWAYS
	CALL    PRTHEX             ;be same through here
	MSG     LINE
10$
	
      ENDC
	
	TEST_W  EXTRA(A4)          ;PENDING COMMAND?
	BEQ.S   SC70               ;EQ => no
	MOVE.L  EXTRAPNT(A4),A6    ;Get addr of current posn in command line
	CALL    NOTOUT             ;Entry into DECODE after new cmd line fetch
	BRA     PROCESS            ;Process a single verb
SC70
	DO	DECODE             ;Get new command line and parse first verb
	BRA     PROCESS            ;process the verb



      IFNE	THE_PAWN!YCHEAT
 
*--------------------------------
*
*  Sussinv - print a list of all the items related to a room,
*            ie. not normally listed but which lurk in the background
*
*--------------------------------

	XDEF	SUSSINV
	  
 
SUSSINV
 
	DO	R.NOUN             ;GET ROOM#
	MOVE.W  D0,D2              ;TARG
	DO	GETRELN	           ;REL TO ROOM.
 
	MOVEQ   #'(',D1
	DO	CHAROUT
10$
	MOVE.W  (A1)+,D0
	BMI.S   10$
	BEQ.S   20$
	GETINFO        
	BTST    #0,5(A0)           ;PSEUDO?
	BNE.S   15$                ;FAIL
	MOVE.B  6(A0),D1
	AND.B   #$3B,D1
	BNE.S   10$
15$
	OR.W    #$8000,-2(A1)
	BRA.S   10$
20$
	LEA     DNOUNBUF(PC),A0
	DO	P.LIST
	DO	UTIL.BS
	MOVEQ   #')',D1
	DO	CHAROUT
	MSGGO   LINE
 
      ENDC
	
*--------------------------------
*
* FINRELNPC - FIND RELATED NPC'S
*
* SETS 'NPCINRM' IF THERE ARE
*
* ANY NPC'S RELATED TO CURRENT 
*
* ROOM
*
*--------------------------------
 
FINRELNPC
 
	MOVE.W  NPCINRM(A4),D0
	BMI.S   10$
	DO	VALID.NP	          ;NO ERRORS PRINTED
	CLR.W   D5
	TEST_W  D7
	BEQ.S   90$
	MOVE.W  #-1,NPCINRM(A4)    ;ZAP IY
10$
	DO	R.NOUN             ;GET ROOM#
	MOVE.W  D0,D2              ;TARG
	DO	GETRELN	           ;REL TO ROOM.
20$
	MOVE.W  (A1)+,D0           ;GET N#
	BEQ.S   90$
	GETINFO                    ;ON D0
	TEST_W  6(A0)              ;MUST BE REL
	BEQ.S   20$                ;NO GOOD
	BTST    #6,4(A0)           ;NPC?
	BEQ.S   20$                ;NO
	BTST    #1,(A0)            ;ALIVE?
	BNE.S   20$                ;NO
	MOVE.W  D0,NPCINRM(A4)     ;ITS OK (WOW!)
90$
	MOVE.W  NPCINRM(A4),D0
	CMP.W   LASTNPC(A4),D0
	BEQ.S   99$
	CLR.W   LASTNPC(A4)
99$
	RET
 
 
*--------------------------------
*
* FINNPC - FIND NPC (NOT RELATED)
*
* AS ABOVE BUT ALREADY GOT 
* BUFFER OF NOUNS TO LOOK IN (A1)
*
*--------------------------------
 
FINNPC
	
	MOVE.L  A1,-(A7)
01$
	MOVE.W  (A1)+,D0
	BEQ.S   90$                ;ALL DONE
	GETINFO                    ;POINT TO DATA
	TEST_W  SEATED(A4)
	BNE.S   20$                ;SKIP THIS
   
	TEST_B  6(A0)              ;HB OF LOC
	BNE.S   10$
20$
	DO	VALID.NP	          ;INCASE   (NO ERRORS PRINTED)
	BNE.S   10$                ;OK
	BTST    #6,4(A0)           ;NPC?
	BEQ.S   01$                ;YES
	BTST    #1,(A0)
	BNE.S   01$ 
	TEST_W  NPCINRM(A4)
	BPL.S   01$
	MOVE.W  D0,NPCINRM(A4)    ;FOR SPEECH
	BRA.S   01$
10$
	SUBQ.L  #2,A1
	BSET    #7,(A1)            ;VOID
	ADDQ.L  #2,A1              ;NEXT
	BRA.S   01$
90$
	MOVE.L  (A7)+,A1
	RET
 
*--------------------------------
*
* ALWINDARK - ALLOWED IN THE DARK
*
* Tests verb to see if u can do it in a dark room
*
*--------------------------------
 
 
ALWINDARK
 
	MOVE.L  A1,-(A7)
	TEST_W  LIGHT(A4)          ;LIGHT = 0 => dark room
	BNE.S   90$
	LEA     VLT.DARK(PC),A1    ;get list of allowed in dark verbs
	MOVE.W  #VLN.DARK,D3       ;COUNTER  (BLOODY LINKER!)
	MOVE.W  VERB(A4),D1        ;TARGET
10$
	CMP.B   (A1)+,D1           ;GOT IT?
	DBEQ    D3,10$             ;NEXT.. OR FALL THROUGH (EQ= FOUND)
	BNE.S   90$                ;OK VERB NOT ON LIST = CAN DO IN DARK
	MSG	I2DARK             ;NOPE - CANNOT DO IT IN DARK
	MOVE.L  (A7)+,A1           ;LEVEL STACK
	MOVE.L  (A7)+,A1           ;LOSE RTN ADDR
	DOGO	SCREEN             ;BACK TO SCREEN
90$
	MOVE.L  (A7)+,A1           ;GET BACK A1
	RET


*--------------------------------
* 
*  SRC.ADV.SCREEN2
*
*--------------------------------
 
 
KILLPSEUDO
 
	MOVEQ   #1,D1
	BRA.S   DOKILL
 
KILLNPC
 
	MOVEQ   #2,D1
	BRA.S   DOKILL
 
KILLNOTNPC
 
	MOVEQ   #3,D1
	MOVEQ   #-1,D6        ;MANY?
  
DOKILL
 
	MOVE.L  A1,-(A7)
10$
	MOVE.W  (A1)+,D0
	AND.W   #$7FFF,D0
	BEQ.S   90$
	GETINFO        
	CMP.B   #1,D1         ;KILLPSEUDO
	BNE.S   20$
	BTST    #0,5(A0)      ;PSEUDO?
	BEQ.S   10$
15$
	OR.W    #$8000,-2(A1)
	BRA.S   10$
20$
	CMP.B   #2,D1         ;KILL NPC
	BNE.S   30$
	BTST    #6,4(A0)
	BEQ.S   10$
	BRA.S   15$
30$
	BTST    #6,4(A0)      ;KILL NOT NPC
	BEQ.S   15$
	TEST_W  6(A0)         ;KILL NPC IF NOT EXPLICITLY IN
	BNE.S   15$           ;ROOM
	AND.W   #$7FFF,-2(A1)
	ADDQ.B  #1,D6
	BRA.S   10$
90$
	MOVE.L  (A7)+,A1
	RET
 
*--------------------------------
 
 
PROCESS

	
	XREF	MakeUndoDiff
	CALL	MakeUndoDiff

 
	CLR.W   REP1(A4)           

	MOVE.W  #-1,PEND2(A4)
	CMP.W   #VNUSE,VERB(A4)
	BNE.S   11$              ;NOPE
	CALL    USE              ;FIX THE BUFFERS
11$
	CALL    TWOPV            ;FIX VERB
	CALL    ISTIETOG
	LEA     NOUNBUF(PC),A0
	MOVE.L  A0,NOUN(A4)
	MOVE.W  NOUNBUF(A4),D0
	BEQ     SINGV
 
PROC.LAB
 
10$
	MSG     LINE               ;PUT A LF BETWEEN ATTEMPS AT NOUNS
	CLR.W   D2
	MOVE.W  ADDNOUN(A4),D0
	BEQ.S   19$
	BMI.S   19$
	GETINFO        
	MOVE.W  D0,D2
	MOVE.L  A0,A5
19$
	MOVE.L  NOUN(A4),A0
	MOVE.W  (A0),D0
	BEQ     PROC.EX    
	TEST_W  WHOOPS(A4)          ;VERB ROUTINE FED UP??
	BNE     PROC.EX             ;YEA
	ADDQ.L  #2,NOUN(A4)
	BCLR    #15,D0
	BNE.S   21$
	MOVEQ   #0,D5               ;SO WE GET ERROR MSGS
	DO	ISDOFRE           
	BEQ.S   21$			;must be able to reach em too!
	DO	VALID
	BNE.S   19$
11$
      IFEQ	THE_PAWN

	XREF	NoCanReach,NoCanReachD2

	CALL	NoCanReach		;returns EQ if D0 may be got at from
	BNE.S	20$			;NE => may not reach it 
					;(CanReach over-rides)
	CALL	NoCanReachD2
	BNE	PROC.EX		;NE => can't reach it (D5) with this verb

      ENDC

* are we sitting ?

	TEST_W  SEATED(A4)
	BEQ.S   21$                 ;NO, CARRY ON..

* does this verb work when sitting?

	CALL    WHENSAT             
	BEQ.S   21$                 ;EQ=> yes

20$

      IFNE    YCHEAT

	TEST_W  CHEAT(A4)
	BNE.S   21$

      ENDC

      IFEQ	THE_PAWN

	XREF	CanReach

	CALL	CanReach		;returns EQ if D0 may be got at from
	BEQ.S	21$			;where we are sitting

      ENDC

	CALL    P.NORCH             ;CANT REACH
	BRA.S   19$                 ;NEXT
21$
	CMP.W   D2,D0
	BNE.S   22$

	MOVE.L  NOUN(A4),A1		;is there another to consider?
	MOVE.W  (A1),D0
	BNE	MissVerb		;NE=> yes, think about it

	DO	DBS			;no, complain
	BRA     PROC.EX   
22$
	GETINFO        
	
      IFNE    YCHEAT

	TEST_W  CHEAT(A4)          ;CHEATING?
	BNE.S   23$

      ENDC
	
	CALL    VERBOWN            ;CHKOWN APPLIES TO THIS VERB?
	BEQ.S   23$
	CALL    CHK.OWN
	TEST_B  D7
	BNE     19$                ;NEXT
23$
	CALL    STOPBRK
	MOVE.B  (A0),D1
	MOVE.W  D2,D5
	CALL    SETPAUSE
	CLR.L   D2
	MOVE.W  VERB(A4),D2
	DO	GETVECT
	
      IFNE    YCHEAT
	
	TEST_W  CHEAT2(A4)         ;THIS CHEAT DOES NOT NOBBLE VALID
	BEQ     MISSINFO           ;DONT PRINT DEBUG INFO   
	PUSH_L  D0-D7/A0-A6  ;CAN'T BE TOO SURE..
	PRINT   <'^NOUN1 :'>
	MOVE.W  D0,D6
	MOVE.W  D5,-(A7)            ;PRTHEX KILLS D5 
	CALL    PRTHEX
	PRINT   <'^NOUN2 :'>
	MOVE.W  (A7)+,D6            ;GET SECOND NOUN
	CALL    PRTHEX
	PRINT   <'^VERB :'>
	MOVE.W  VERB(A4),D6
	CALL    PRTHEX
	PRINT   <'^CONJ :'>
	MOVE.W  CONJ(A4),D6
	CALL    PRTHEX
	PRINT   <'^ADVERB :'>
	MOVE.W  ADVERB(A4),D6
	CALL    PRTHEX
	PRINT   <'^NOUN3 :'>
	MOVE.W  ADDNOUN+2(A4),D6
	CALL    PRTHEX
	MSG     LINE
	PULL_L  D0-D7/A0-A6

      ENDC

MISSINFO
	
	
	CALL    ALWINDARK
	CALL    GETPRON            ;SET PRONOUNS - BUT LET VERB ALTER THEM

      IFEQ	THE_PAWN

	XREF	ProcessSpecials		;to be found in special0

	CALL	ProcessSpecials		;check noun interference
	BNE.S	10$			;thou shalt not proceed (john)

      ENDC

	MOVE.B  (A0),D1
	JSR     (A1)
10$	
 
MissVerb
 
*--------------------------------
*
*  THIS PASSED:
*              D0=NOUN NUMBER
*              D1=STATUS
*              A0=ADDR OF DATA
*  TO THE VERB ROUTINE
*
*--------------------------------
  
 
	TEST_W  PEND2(A4)
	BMI.S   85$
	DO	DECODE 
	BRA     PROCESS
85$
 
	MOVE.L  NOUN(A4),A1
	MOVE.W  -2(A1),D0
	GETINFO        
	BTST	#6,6(A0)		;to avoid 'go room' - no desc hassle
	BNE.S	90$			;let screen set explored AFTER desc.
	DO	VALID.NP		;only set explored if its local
	BNE.S	90$
	BSET    #4,4(A0)		;SET EXLORED FLAG
90$
	BRA     PROC.LAB

PROC.EX

	DOGO	SCREEN
 
 
*--------------------------------
  
SINGV
  
	DO	R.NOUN
	GETINFO        
  
	MOVE.W  VERB(A4),D2
	BNE.S   10$                ;0= DO A LOOK
	MOVE.W  #2,SCRPNT(A4)
	CLR.W   RIDPRT(A4)
	BRA.S   50$                ;SCREEN
10$
	CALL	DO.GOIN		  ;trap go in 
	CMP.B   #2,D7		  ;fail
	BEQ	GetSomeNouns_EXIT	;yes, exit.
	TEST_W  ADVERB(a4)	  ;must have no adverb to be an exception
	BPL.S   30$		  ;has one, continue.
	LEA	SING.EX(A4),A1	  ;table of execptions (GET).
20$
	MOVE.B  (A1)+,D0	  ;get a verb
	BEQ.S   30$		  ;end (NB: look tested above)
	CMP.B   D0,D2		  ;verb in table?
	BEQ.S	GetSomeNouns	  ;yes, skip
	BRA.S   20$		  ;next
30$
	MOVEQ   #-1,D0             ;NO NOUN
	LEA     SINGV.TB(PC),A1
	MOVE.W  #VLN.SING,D3       ;NO OF VERBS -1 (LINKER!!)
40$
	CMP.B   (A1)+,D2    
	DBEQ    D3,40$
	BNE.S	GetSomeNouns
	CALL    ALWINDARK
	DO	GETVECT
	JSR     (A1)
50$
	DOGO	SCREEN
 
GetSomeNouns

	MOVE.W	VERB(A4),D0		;get the verb (if any)
	BMI.S	20$			;no, oh well...
	CALL	UseWhere		;returns EQ is want verb where?
	SUBQ.B	#1,D7
	BPL.S	01$
	MSG	SWHAT			;<'What'>
	BRA.S	05$
01$
	SUBQ.B	#1,D7
	BPL.S	02$
	MSG	SWHERE			;<'Where'>
	BRA.S	05$
02$
	MSG	SWHO			;<'Who'>
05$
	MSG	SDOYOUWANT		;<' do you want to '>
	MOVE.W	D0,PVERB(A4)		;stash it.
	MOVE.W	D0,D1			;P.VERB wants D1=verb#
	DO	P.VERB
	MOVE.W	#-1,VERB(A4)		;kill old verb so GO isn't confused

* REMEMBER 'adverbs' are really particles!

	MOVE.W	ADVERB(A4),D1		;and the adverb.
	MOVE.W	D1,PADV(A4)		;P.ADVERB (P.PARTICLE) wants D1
	CMP.W	#-1,D1			;null adverb?
	BEQ.S	10$			;yes
	CALL	P.PARTICLE		;print 'up/down/on/off' etc
10$
	DO	UTIL.BS
	PRINT	<'?'>
	CALL	INTEXT			;get command line
	PEA	PROCESS(PC)		;point to return to.
	BRA	DECODE.LAB		;carry on...
20$
	CMP.B   #2,D7			;ERROR been printed?
	BEQ.S   30$
	MSG	EH
	MSG	IDFY
30$

GetSomeNouns_EXIT

	CLR.W   EXTRA(A4)
	DOGO	SCREEN

UseWhere

	PUSH_L	D0-D1/A0
	MOVE.W	VERB(A4),D0

	MOVEQ	#1,D7
	LEA	VList_Where(PC),A0
	CALL_S	LookList
	BEQ.S	90$

	MOVEQ	#2,D7
	LEA	VList_Who(PC),A0
	CALL_S	LookList
	BEQ.S	90$

	MOVEQ	#0,D7
90$
	PULL_L	D0-D1/A0
	RET

LookList

10$
	MOVE.B	(A0)+,D1
	BEQ.S	80$
	CMP.B	D0,D1
	BNE.S	10$
	BRA.S	90$
80$
	SETNE
90$
	RET

VList_Where

	SVERB	GO
	SVERB	WALK
	SVERB	RUN
	SVERB	LEAN
	SVERB	LISTEN
	SVERB	LIE
	SVERB	SWIM
	SVERB	PISS
 
	DC.B	0			;look is a SINGV so isn't relevent

VList_Who

	SVERB	RAPE
	SVERB	KISS

	DC.B	0

	ALIGN
 
*--------------------------------
  
P.INIT
  
 
      IFNE    YCHEAT
	TEST_W  CHEAT(A4)
	BNE.S   90$
      ENDC
 
	MOVE.W  #-1,VERB(A4)       ;BLOODY ROOMS!!!
	LEA     INIT.TAB(PC),A1
20$
	MOVE.W  (A1)+,D1           ;MSG #
	MOVE.W  (A1)+,D0
	BEQ.S   90$
	GETINFO        
	DO	VALID.NP	          ;SETS D5 ITSELF
	TEST_W  D7
	BNE.S   20$
	BSET    #1,5(A0)
	BNE.S   20$
	DO	P.SCN
	BRA.S   20$
90$
	RET
  
  
*--------------------------------
*
* Set pause - pause an NPC if he's the subject of
*             a verb, eg if u try to trade/fight/talk with an NPC
*             they'll hang around a while
*
*
*--------------------------------


SETPAUSE
  
	BTST    #6,4(A0)
	BEQ.S   10$
	CALL_S  90$
10$
	TEST_W  D5
	BEQ.S   11$
	BTST    #6,4(A5)
	BNE.S   20$
11$
	RET
20$
	CALL_S  99$
	CALL_S  90$
	BRA.S   99$
90$
	DO	GETNPC
	BSET    #0,3(A3)           ;BIT 0=PAUSE
	RET
99$
	EXG     D0,D5
	EXG     A0,A5
	RET

*--------------------------------

	XREF	DO.QUIT
* well chaps - the player has won - we shall see what he wishes to do now

DO.FINAL	
	
	PEA	10$(PC)
10$
	LEA	DO.FINAL(PC),A0
	BRA	DO.QUIT

	END
