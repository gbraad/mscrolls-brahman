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
*	@(#)parser2.asm	2.16
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PARSER2
*
*--------------------------------


	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"

	XDEF     NOADJ,DO.CONJ,WSYN,NOTWSYN,EXTRAFIX,CONJ11,DO.BUFFS,DO.ADJ
	XDEF     BIZARE,DO.THEN2,DO.THEN,AGAIN,VERBTYPE,BACK.AWRD
	
	XREF     DECODE,NEWST,W.WHAT,WRDPNT,FNDWRD,POSWRD,PRTWRD2,CLRADJ,UTIL.BS
	XREF     CHAROUT,D72,ERR1,W.WRSYN,CONJPNTR,VERB2,VERB,GETWRD
	XREF     ADVERB,SETADN,PEND,EXTRAPNT,EXTRA,PEND2,CONJ,EXBUFF,EXNUM
	XREF     GETCONJ,NOUNBUF,NOUN,ADJPNTR,ADJNOUN,VALID,P.NFND,W.BZ
	XREF     CONJBUF,RCLTXT,INBUFF,NOTOUT,VLST.INV,ADJBUF,VLEN.INV
	XREF     DSTAND,W.CTEXT,BACK.WRD,GETADJPH,EDITCMD,FNOUN,ISDOFRE

      IFEQ	THE_PAWN

	XREF	DoScenery

      ENDC
 
NOADJ
 
	CMP.B   #2,D7        	;ALREADY ERROR
	BPL     DECODE
	MSG	WHAT
	CLR.W   D0
	MOVE.L  WRDPNT(A4),A6
	SUBQ.L  #1,A6        	;BADHACKS
*	DO	FNDWRD       	;TO START OF WRD
	MOVE.L  A6,A5        	;KEEP
	MOVE.L  POSWRD(A4),A6	;BEGINNING OF LAST WRD
10$
	CMP.L   A6,A5        	;THERE YET?
	BHS.S   20$          	;YES
	CALL    PRTWRD2  	;BEGINNING OF 1ST WRD
	BRA.S   10$          	;ANOTHER
20$
	CALL    PRTWRD2
	DO	CLRADJ
	DO	UTIL.BS
	MOVEQ   #'?',D1
	DO	CHAROUT
  
	DOGO	D72
 
 
*--------------------------------
 
  
DO.CONJ
 
	TEST_B  D3
	BNE.S   NOTWSYN
 
WSYN
 
	TEST_W  ERR1(A4)     	;Error msgs been killed?
	BNE.S   10$
	CMP.B   #2,D7        	;Error been printed, already?
	BEQ.S   10$

      IFEQ	THE_PAWN

	CMP.W	#SECT.SCEN,D6		;scenery noun?
	BEQ	DoScenery		;yes (ps this goes to DECODE)

      ENDC

	MSG	WRSYN
10$
	DOGO	DECODE
 
NOTWSYN
 
	MOVE.L  CONJPNTR(A4),A0
	MOVE.W  D5,(A0)
	ADDQ.L  #2,CONJPNTR(A4)
 
CONJ11
 
	CALL    GETCONJ
	CMP.B   #1,D7
	BEQ.S   20$
	BHI     NEWST        	; was DECODE, now NEWST to solve problem with
*                           	; 'open door with key':Which key?:i
* NEWST tests BRKWCH to decide whether there is a valid input line already
* in INBUFF, so as to avoid asking for another

	CMP.B   #SECT.VERB,D6	;Verb?
	BNE.S   10$
	CMP.W   #VNUSE,VERB(A4)    ;use?   
	BNE.S   10$

	TEST_W	VERB2(A4)	;but do we have a second verb already?
	BPL.S	10$		;PL=> yes
	
	MOVE.W  D5,VERB2(A4) 	;Also
	DO	GETWRD	      	;Another conj (eg use x to pick up y)

	TEST_B  D7           	;Test the error return from GETWRD
	BNE     WSYN

	CMP.B   #SECT.CONJ,D6
	BEQ.S   12$
	CMP.B   #SECT.DIR,D6
	BNE.S   11$          	;Neither
	OR.W    #$8000,D5    	;HI-BIT flg for directons
12$
	MOVE.W  D5,ADVERB(A4)	;Store
	BRA.S   CONJ11
11$
	MOVE.L  POSWRD(A4),A6
	SUBQ.L  #1,A6
	
	BRA.S   CONJ11
10$
	CMP.W   #SECT.CONJ,D6	;Conj?
	BEQ.S   NOTWSYN
	CMP.B   #SECT.DIR,D6 	;Go
	BEQ.S   15$          	;OK
	
	CMP.W   #SECT.VERB,D6	;Verb??
	BEQ.S   15$
	CMP.W   #SECT.SPEC,D6
	BNE     WSYN         	;No good
	CMP.W   #8,D5        	;Then?
	BNE     WSYN         	;Nope
  
15$
	CALL    SETADN
	BRA     DO.THEN2
 
20$
	CALL    SETADN
	TEST_W  PEND(A4)
	BEQ     DO.THEN
	MOVE.L  A6,EXTRAPNT(A4)
	MOVE.W  #1,EXTRA(A4)
	MOVE.L  CONJPNTR(A4),A0
	TEST_W  PEND2(A4)
	BMI.S   30$
	SUBQ.L  #2,A0
30$
	MOVE.W  (A0),CONJ(A4)
	RET
 
EXTRAFIX
 
	CLR.W   EXTRA(A4)
	RET
 
DO.BUFFS
 
	LEA     EXBUFF(A4),A0
	ADD.W   EXNUM(A4),A0
	LEA     NOUNBUF(A4),A1
	ADD.W   NOUN(A4),A1
	RET
 
*--------------------------------
 
DO.ADJ

*
*	entry:
*	 expects a2,a4 to be set up by getwrd
 
	MOVE.L  ADJPNTR(A4),A0
	SUB.L   A4,A0        	;so as next line is ok!!!!!!!!!!
	CMP.L   #ADJBUF,A0   	;THIS WONT WORK!! - KEN (?)
	BNE.S   01$
	MOVE.L  A6,WRDPNT(A4)	;NEW ADJ METHOD!
01$
	ADD.L   A4,A0        	;make up for above
	ADDQ.L  #1,D5
	MOVE.W  D5,(A0)+
	CLR.W   (A0)
	ADDQ.L  #2,ADJPNTR(A4)
	MOVE.W  (A2),D0			;NEXT
	CLR.W   ADJNOUN(A4)
	CMP.B   #SECT.NOUN,D0		;ALSO A NOUN?
	BNE.S   40$			;NO
10$
	CMP.W   #SECT.NOUN,(A2)+
	BEQ.S   20$
	BSET    #15,D0		  ; flag as, was a noun but not local.
	BRA.S   30$
20$
	MOVE.W  (A2)+,D0
	ADDQ.W  #1,D0

	DO	ISDOFRE		;miss valid (global verbs)
	BEQ.S	21$
	DO	VALID
	TEST_W  ERR1(A4)     	;MAKE PRINCESS WORK
	BEQ.S   25$
21$
	CLR.W   D7
25$
	TEST_B  D7
	BNE.S   10$
	SUBQ.W  #1,D0
30$
	MOVE.W  D0,ADJNOUN(A4)
40$    
	DO	GETWRD
	MOVE.L  ADJPNTR(A4),A0		;rather than the stack push earlier.
	CMP.B   #1,D7
	BLS.S   50$
	CLR.W   D5
	CMP.B	#4,D7				 ;resolution failure.
	BEQ.S	70$				 ;exit wo NE.
	BRA.S   52$	 			;think this is the fix?
50$
	TEST_B  D7
	BNE.S   60$
	CMP.W   #SECT.ADJ,D6 		;ANOTHER ADJ?
	BEQ     DO.ADJ       		;YES
	CMP.B   #SECT.NOUN,D6		;NOUN?
	BEQ.S   90$          		;YES, DONE
	CMP.B   #SECT.MULT,D6
	BEQ.S   52$
	CMP.B   #SECT.ONE,D6
	BNE.S   51$
52$
	SETNE
	RET
51$

	CALL    BACK.WRD			;step back 1 word.
60$
	MOVE.W  #SECT.NOUN,D6
	MOVE.W  ADJNOUN(A4),D5
	BEQ.S   91$                		;NONE, EXIT
	BCLR    #15,D5
	BEQ.S   80$
	MOVE.W  D5,D0
	CLR.W   -(A0)
	CALL    P.NFND
	MOVEQ   #2,D7
	BRA.S   70$
80$
	CLR.W   -(A0)			;was not an adj, clear last adj

    IFEQ    THE_PAWN
	CALL	BACK.AWRD		;backtrack another word.
	ST	FNOUN(A4)		;force noun.
	BRA.S	40$			;get noun & exit.
    ENDC
    
90$
	CLR.W   D7
70$
	TEST_W  D5
	RET
91$
	MOVEQ   #0,D5 	; THIS MIGHT NOT WORK
	BRA.S   90$
  
*--------------------------------
  
BIZARE
 
	MSGGO	BZ
  
*--------------------------------
  
DO.THEN2
  
	MOVE.L  POSWRD(A4),A6
	SUBQ.L  #1,A6
  
DO.THEN
  
	MOVE.L  A6,EXTRAPNT(A4)
	MOVE.W  #1,EXTRA(A4)
	MOVE.L  CONJBUF(A4),CONJ(A4)
	RET
 
  
*--------------------------------
*
* AGAIN - copes with repeat last command line please request
*          somewhat outmoded by stuff like ESC in ST etc but
*          only overhead is this code, so here it is...
*
*--------------------------------
  
AGAIN

	MOVE.L  POSWRD(A4),-(A7) 	;so as we can print 'again' eventually
	LEA     INBUFF(A4),A6    	;must be the first command on line
	DO	GETWRD
	CMP.W   #SECT.SPEC,D6
	BNE.S   10$
	CMP.W   #7,D5            	;again?
	BNE.S   10$

	ADDQ.L  #4,A7            	;Loose old POSWRD that's on the stack
	CALL    RCLTXT           	;get old text
	LEA     INBUFF(A4),A6
	BRA     NOTOUT           	;process commands again
10$
	MOVE.L  (A7)+,POSWRD(A4) 	;Get pointer to text of 'AGAIN'
	CALL    DSTAND           	;'I don't understand xyzzy
	MSG	CTEXT  	        	; in that context'
	DOGO	DECODE           	; go get new command line from keyboard
 
*--------------------------------
*
*  VERBTYPE - SUSSES WHETHER TO
*             USE INVENTORY OR
*             ROOM CONTENTS FOR
*             'ALL'  USES LIST
*             VLST.INVV
*
*  RETURNS  D7 = 1 = USE INVENT
*           D7 = 0 = USE ROOM
*
*--------------------------------
 
VERBTYPE
  
	MOVEQ   #0,D7        	;ZAP NOW, SAVES 2 BYTES LATER
	PUSH_L  D0/D1/A0	   ;SAVE SCRATCH
	LEA     VLST.INV(A4),A0    ;LIST OF 'ON INV' VERBS
	MOVE.W  #VLEN.INV,D1 	;NO OF VERBS-1
	MOVE.W  VERB(A4),D0  	;TARGET VERB
10$
	CMP.B   (A0)+,D0     	;ON INVE?
	DBEQ    D1,10$       	;NEXT, OR FALL THROUGH IF EQ OR FINI
	BNE.S   20$          	;COMPLETED LOOK = NOT IN LIST
	MOVEQ   #1,D7        	;RETURN 'ON INVE'
20$
	PULL_L  D0/D1/A0     	;GET SCRATCH
	RET

*--------------------------------

BACK.AWRD

*  Move back another word in the input stream A6.
	PUSH_L	D0/A1		;save temp vars.
	LEA	INBUFF(A4),A1	;start of buffer
03$
	TEST_B	(A6)
	BNE.S	02$		;ok.
	SUBQ.L	#1,A6		;back until e o sentence.
	BRA.S	03$
02$
	MOVE.B	(A6),D0		;get this char.
	CMP.B	#' ',D0		;space?
	BNE.S	01$		;no- at the end of a word.
	SUBQ.L	#1,A6		;backtrack.
	CMP.L	A1,A6		;start?
	BNE.S	02$		;no- again.
	BRA.S	90$		;exit.
01$
	CMP.L	A1,A6		;start?
	BEQ.S	90$		;yes, exit.
	MOVE.B	-(A6),D0	;previous char.
	CMP.B	#' ',D0		;space?
	BNE.S	01$		;no.
90$
	PULL_L	D0/A1		;restore old values.
	RET			;exit.

*--------------------------------
      END
