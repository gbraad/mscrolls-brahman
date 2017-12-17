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
*	@(#)verbsub6.asm	2.20
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.VERBSUB6
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"nounequ2.i"       ;JUST FOR NOUNNUM



	XREF    WRDTABLE,CHAROUTL,ST_,POSWRD,CHEAT,W.NOTAKE,W.WNTF
	XREF    W.NORIDE,W.WHAK,W.FALOFF,STACK,W.IDFY,PEND2,W.NT
	XREF    WHOOPS,CONJST,W.NOT,NMOVEFLG,W.ACON,W.BUT
	XREF    ERRHOLD,ERR1,ERR4,VERB,GLV.L_,W.BTINA,W.NO.TNG,PLURAL
	XREF    CHAROUT,W.FERG,W.BECOF,SUBJECT,LASTA0,VALID.NP
	XREF    W.ICNDTW,PROC.LAB,BRK.VERB,SUBOBJ,D72
	XREF    W.ARE,W.IT,W.IS,W.THEM,W.THEY,W.HE,W.SHE,W.HIM,W.HER,W.THE
	XREF    GETNPC,ADJECTIV,LIST.N,ADJ.TBL,INITNUM,W.CNTSEE,W.HERE
	XREF    WASMOD2,NOTRAIL,UTIL.SP,LISTWRD,UTIL.DEL,LINE
	XREF    FMSG,CAPITAL,LISWRD,GETDATA,W.WT,W.YOU,W.AN,SCANFLAG
	XREF    P.DESC,P.LIST,P.LIST2,P.LOR,P.QN,P.NQN,SP3.FLAG,GETRELN
	XREF    P.NOUN,P.NOUN2,NONLY,P.TN,P.QUANT,P.PRN,P.OBJPRN,ISARE
	XREF    P.PV,P.VQN,P.SUBOB,P.SUB,DERR,STOPBRK,YCANT,YCANTW,CARRIED
	XREF    P.BOTN,P.XFERGS,P.NO.TNG,P.NOTANY,ISDOFRE,HOLDE,UNHOLDE
	XREF    W.FIND,ROOM,DBSFLG,P.NODO,GETNOUN2,LOOPBUF,L.DIREC
	XREF    MATCHBUF,W.CON,NETWORK,W.MTMAGI
	XREF    UTIL.BS,EXBUFF,SEARCH2,W.FHERE,W.NORCH,VLST.GO2
	XREF    P.SUBV,P.NFND,P.NQEN,SCANSV,DBS,OK,CALCWHT,P.BRAKTS
	XREF    GET_CONT,R.NOUN,R.NOUN2,SPACE,ROOTNUM,P.MSG,P.SCN
	XREF    SETA1,P.CNTO,P.FROM,P.WITH,CONNECT,CONNECT2,SCAN
	XREF    W.NWHERE,W.TSOFT,GETNOUNS,PRONHER,PRONHIM,OLDBUF,SEATED
	XREF    L.WHSAT,RIDING,W.ON,NOUNBUF,NOUNBUF2,NOUNBUF3
	XREF    INTERROR,W.YCNDTT,W.JUSONE,W.NOTIE,CONJ,NOUN,ADVERB
	XREF    SCREEN,VERB2,ADDNOUN,STOPRON,OLDADVERB
	XREF    P.EXITS,CALCNET,PCOK,MIXD0D5,FIXCHK,SETPRON
	XREF    IFLIQUID,WHENSAT,SATMSG,ONLYREL,TIETOG,ISTIETOG
	XREF    SETADN,USE,BUFSHUF,MOVBUF,DOSCORE2
	XREF    DOSCORE,P.NORCH,DO.SCN,WTAB_VERBS,W.ARDY,PSUDAT


	XDEF	TOPLOC,PUTD0IND5
	XDEF    TOPNPC,SETOUT,SETOUT2,PRTWRD,PRTWRD2,WHRIDING,P.CONJ
	XDEF    PRTNUM,PRTTXT,PRTBYTE,PRINTWRD,P.ANOUN
	XDEF    P.NONLY,P.IDFY,P.CWHT,CHK4PT,CHK6PART
	XDEF    P.TAB,ISAREN,FIXCONT,FNDEND,FIXSYS,P.BTN
	XDEF	P.PARTICLE,P.VERB,FULLSTOP,BSFULLSTOP
	XDEF	P.VERBING,P.BTNI,P.TNSTOP,P.LISTSTOP

*
* P.LISTSTOP
* 
* does a P.LIST followed by a BSFULLSTOP

	XREF	NOTRAIL2

P.LISTSTOP

	ST	NOTRAIL2(A4)
	DO	P.LIST
	SF	NOTRAIL2(A4)
	BRA.S	FULLSTOP

*
* P.TNSTOP - prints 'the xyyyz.'
*
* prints D0
*

P.TNSTOP

	ST	NOTRAIL(A4)
	DO	P.TN
	SF	NOTRAIL(A4)
	BRA.S	FULLSTOP

* BSFULLSTOP
* 
* prints a BS then a '.' ! - D1 preserved

BSFULLSTOP

	DO	UTIL.BS

* FULLSTOP	prints just a '.'

FULLSTOP

	MOVE.L	D1,-(A7)
	MOVEQ	#'.',D1
	DO	CHAROUT
	MOVE.L	(A7)+,D1
	RET
	
TOPNPC
	
*FINDS NEXT NPC
*(CF TOPLOC, VALID)
*PRESEVES D0/A0
*RETURN NPC, OR ITEM IN D4 
	
	PUSH_L  D0-D1/A0     ;SAVE
	BTST    #6,4(A0)           ;NPC AT START
	BNE.S   20$                ;DONT CHK
10$
	BTST    #6,4(A0)           ;NPC?
	BNE.S   99$                ;YES
20$
	MOVE.B  6(A0),D1  
	AND.B   #$3D,D1            ;MASK
	BEQ.S   99$                ;DONE
	MOVE.W  8(A0),D0           ;NEXT ITEM
	GETINFO
	BRA.S   10$                ;NEXT
99$
	MOVE.W  D0,D4
	PULL_L  D0-D1/A0
	RET
	
*--------------------------------
 
 
*--------------------------------
*
* SETOUT - CLEAN UP AFTER A ZAP
*
* SET'S BIT 7,BYTE 6 (OUT FLAG)
* THEN CLEANS UP FIXTURES
* AND CONTAINERS
*
* SO STUFF LIKE TIE SHIRT TO RED
* MIX COLOURS,DR SHIRT,L RED
* DONT SUSS US!!
*
*
*--------------------------------
 

SETOUT
 
	PUSH_L  D0-D2/D5/D6/A0-A1/A3
	GETINFO
	BSET    #7,6(A0)           ;SET IT TO BE 'OUT'
	DO	FIXCONT            ;FIX ANY CONTAINERS

	MOVEQ	#0,D5			;tell fixsys to be quiet
	DO	FIXSYS			;get buffer of tied obj's in A3
02$
	MOVE.W	(A3)+,D0
	BMI.S	02$
	BEQ.S	90$
	GETINFO
	CLR.W	12(A0)
	BRA.S	02$

	IFNE	0

	MOVE.W  D0,D2              ;TARGET
	MOVEQ   #1,D5              ;FLAG - LOOKING FOR WORD
	MOVEQ   #12,D1             ;STARTING WI BYTE 12 (TIED)
    
	DO	GETNOUNS           ;BUFFER RETURNED AT A1
 
10$
	MOVE.W  (A1)+,D0
	BEQ.S   90$
	GETINFO
	CLR.W   12(A0)             ;UNTIE
	BRA.S   10$

	ENDC
 
90$
	PULL_L  D0-D2/D5/D6/A0-A1/A3
	RET

*
*
* SETOUT2 - to avoid hassle from things which are contained being put 'out'
*           and annoying the relph stuff we have 'SETOUT2' which sets the
*           noun out then moves it to room #1 (still with the OUT
*           bit set).
*

SETOUT2

	MOVE.L	A0,-(A7)
	DO	SETOUT			;do the work!
	GETINFO
	MOVE.L	#$80000001,6(A0)	;frig to room 1
	MOVE.L	(A7)+,A0
	RET

*
*
* PUTD0IND5 - move D0 into D5, fixing all cont. vols etc
*
* this does no checking!
*

PUTD0IND5

	PUSH_L	D0/D2/D3/D5/A0/A5
	GETINFO
	DO	FIXCONT			;fix volume of container removed from
	MOVE.W  D5,8(A0)		;point to container  
	MOVE.W	#$2000,6(A0)		;set as simply contained
	MOVE.B	2(A0),D2
	LSR.B	#4,D2			;get size
	EXG     D0,D5
	GETINFO
	MOVE.B  10(A0),D3		;cap
	SUB.B   D2,D3			;with new cap
	MOVE.B  D3,10(A0)		;update

	PULL_L	D0/D2/D3/D5/A0/A5
	RET

	
  
*--------------------------------
*
* PRTWRD - PRINTS WORD A (POSWRD)
*
* ENDS ON ZERO,SPACE OR HI BIT SET
*
*--------------------------------
 
PRTWRD
 
	MOVE.L  POSWRD(A4),A5

PRTWRD2

	MOVE.B  (A5)+,D1
	BEQ.S   90$                ;END
	BMI.S   80$                ;END WI A SPACE
	CMP.B   #$20,D1
	BEQ.S   90$
	DO	CHAROUT
	BRA.S   PRTWRD2
	
80$
	DO	CHAROUT
90$
	MSGGO     SPACE
 
*--------------------------------
	
WHRIDING
	
	CLR.W   D7                 ;NO ERROR YET.
	
	IFNE    YCHEAT
	TEST_W  CHEAT(A4)
	BNE     99$   
	ENDC
 
	TEST_W  RIDING(A4)         ;RIDING AT ALL?
	BEQ     99$                ;NO, EXIT OK.
	MOVE.W  SEATED(A4),STOPRON(A4)
	MOVE.W  D1,D2
	DO	R.NOUN2
	GETINFO            ;ROOM NOUN IN D0.A0
	MOVE.B  2(A0),D1           ;SIZE & WIGHT
	LSR.B   #4,D1              ;GET SIZE
	BEQ     90$                ;NULL, OK
	CMP.B   #1,D1              ;MUST DROP ALL
	BEQ.S   11$                ;SAME AS 2 FOR THE TIME BEING..
10$
	CMP.B   #2,D1              ;TOO SMALL TO FIT (HORSE) IN.
	BNE.S   20$
11$
	TEST_W  SUBJECT(A4)
	BNE.S   15$
	DO	P.SUB
	MSG     NOTAKE
	BRA     80$               ;FAIL.
15$
	MOVE.W  SEATED(A4),D0
	DO	P.TN
	MSG     WNTF           ;WONT FIT
	BRA.S   80$
20$
	CMP.B   #3,D1              ;NOT RIDING.
	BNE.S   30$
	TEST_W  SUBJECT(A4)
	BNE.S   15$
	DO	P.SUB
	MSG     NORIDE
	BRA.S    80$               ;FAIL.
30$
	CMP.B   #$A,D1             ;TOO LOW.
	BNE.S   40$
	MSG     WHAK
35$
	CLR.W   RIDING(A4)
	CLR.W   SEATED(A4)
	BRA     80$
40$
	CMP.B   #$B,D1             ;STUMBLES.
	BNE.S   50$
	MSG     FALOFF
	BRA.S   35$
50$
	CMP.B   #$C,D1             ;FAR TOO SMALL.
	BNE.S   60$
	MSG     MTMAGI
	BRA.S   80$
60$
90$
	MOVE.W  D2,D1 
99$
	RET
80$
	MOVEQ   #1,D7
	BRA.S   90$                ;EXIT
	
	
*--------------------------------
 
PRTNUM
	PUSH_L   A0/D3
	SF      D3
	MOVE.L  D6,D2
	LEA     DECTAB(PC),A0
10$:
	MOVE.W  (A0)+,D0
	BEQ.S   PRTOUT
	CLR.B   D1
20$:
	SUB.W   D0,D2
	BMI.S   30$
	ADDQ.B  #1,D1
	BRA.S   20$
30$:
	ADD.W   D0,D2
	TEST_B  D1
	BNE.S   40$
	TEST_B  D3
	BEQ.S   10$
40$:
	ST      D3
	ADD.B   #'0',D1
	DO	CHAROUT
	BRA.S   10$

PRTOUT:
	MOVE.B  D2,D1
	ADD.B   #'0',D1
	DO	CHAROUT
	PULL_L   A0/D3
	RET
 
DECTAB:  DC.W 10000,1000,100,10,0

PRTTXT
 
	MOVE.L  (A7),STACK(A4)    ;PULL DATA ADDRESS
	PUSH_L  D0-D7/A0-A6
	MOVE.L  STACK(A4),A3       ;REPLACE
10$
	MOVE.B  (A3)+,D4           ;GET 1 CHARS.
	BEQ.S   90$                ;FINISH
	CALL_S  PRTBYTE
	BRA.S   10$
90$
	ADDQ.L  #1,A3
	MOVE.L  A3,STACK(A4)
	BCLR    #0,STACK+3(A4)
	PULL_L  D0-D7/A0-A6
	MOVE.L  STACK(A4),(A7)    ;PUSH RETURN ADDRESS
	RET
	
PRTBYTE
 
	TEST_B  D4                 ;CHAR OR WORD ?
	BMI.S   PRINTWRD
	MOVE.B  D4,D1              ;BYTE TO SEND
	BRA     CHAROUTL           ;FUDGE TO LOWER CASE
	
PRINTWRD
  
	DO	P.SUB
	AND.W   #$7F,D4            ;GEN OFFSET INTO WORD ADDRESS TABLE
	LSL.W   #$01,D4            ;*2
	LEA     WRDTABLE(A4),A2    ;START OF TABLE
	ADDA.W  D4,A2              ; ADD OFFSET TO START
	LEA     ST_(A4),A1         ;OF TABLE
	ADD.W   (A2)+,A1           ; GET ADDRESS OF STRING
	TEST_W  SUBJECT(A4)
	BEQ.S   10$                ;OK - ITS NORMAL
30$
	TEST_B  (A1)+              
	BNE.S   30$                ;SKIP FIRST MSG
	
10$
	MOVE.B  (A1)+,D1
	BEQ.S   20$                ;DONE
	DO	CHAROUT
	BRA.S   10$                ;NEXT
20$
	RET
 
*--------------------------------
	        
	
P.ANOUN
	
*PRINT NOUN IN SINGULAR FORM
*THIS ROUTINE IS REALLY, REALLY BAD!
	
	GETINFO
	BTST    #0,(A0)            ;PL?
     
	BEQ     P.TN             ;AS NORMAL, THE (NOUN)
	PRINT   <'a '>
	DO	P.NOUN	          ;QUANTIFIER + NOUN
	CALL    UTIL.DEL         ;DELETE
	BRA     UTIL.DEL
	
*--------------------------------
	        
P.NONLY
	
*PRINT JUST QP+ NOUN
	
*ENTRY:
* D0= NOUN#
	
* USES D1
* PRESERVES D0/A0
	
	PUSH_L  D0/D3/A0/A5
	GETINFO              ;TO A0
	MOVE.L  A0,A5
	BTST    #6,4(A0)             ;NPC?
	BEQ.S   01$                  ;NO
	DO	GETNPC	              ;TO A3
01$
	LEA     LIST.N(A4),A0
	SUBQ.W  #1,D0
	BEQ.S   09$                  ;IST ONE
05$
	IFEQ	FastBits

	TEST_B  (A0)+
	BPL.S   05$                  ;SKIP ONE WORD
	SUBQ.W  #1,D0
	BNE.S   05$                  ;UNTIL TXT FOUND

	ENDC

	IFNE	FastBits

	EXG	A0,A1		;WORDSCAN wants A1
	EXG	D0,D2		;and D2

	DC.W	$A0FB

	EXG	A0,A1
	EXG	D0,D2

	ENDC
09$
	MOVE.B  (A0),D1              ;GET 1ST

	BeSure	M.ANY

	MOVE.W  #M.ANY,D3            ;INCASE PLURAL
	CALL    P.QUANT
10$
	MOVE.B  (A0)+,D1             ;GET CHAR 
	BMI.S   20$                  ;LAST ONE
	DO	CHAROUT
	BRA.S   10$                  ;NEXT
20$
	DO	CHAROUT
	PULL_L  D0/D3/A0/A5
	TEST_B  NOTRAIL(A4)          ;REQ A SPACE?
	BEQ     SPACE                ;YES
	RET
	
*--------------------------------
	
P.IDFY
	
*SIMILAR TO ROUTINE IN TOPSUBS
	
	TEST_W  ERR1(A4)             ;ERROR?
	BNE.S   90$                  ;NO
	MSG     IDFY
	MOVEQ   #2,D7
90$
	RET
*--------------------------------


*--------------------------------
*
*  SRC.ADV.VERBSUB7
*
*--------------------------------
 
 
*--------------------------------
	
P.CWHT
	
* PRINT CONJ WHAT, SET PEND   
* DOES NOT RETURN
	
	MOVE.W  D1,PEND2(A4)
	MOVE.W	ADVERB(A4),OLDADVERB(A4)	;so we can maintain it 
	CALL    P.CONJ
	MSG     WT               ;'CONJ WHAT?'
	MSG    LINE
	MOVE.L  (A7)+,D7           ;AND SET NE
	RET
	
*--------------------------------
*
*  CHECK FOUR PART
*
* SUSSES IF THE SUPPLIED CONJ IS
* AS PER TYPED (AND SO OK FOR 
* THE VERB) AND THAT THERE ARE
* REQUIRED NO. OF NOUNS (2)
*
* DOESNT RETURN IF BAD CONJ/NOUNS
* IT SETS UP LIFE,UNI,ETC TO KNOW
* ABOUT COMMING 'WITH WHAT?'
*
*--------------------------------
  
CHK4PT
 
* D1=RELEVENT CONJ
 
	MOVEQ   #0,D7
	CMP.W   CONJ(A4),D1
	BEQ.S   40$
	TEST_W  CONJ(A4)
	BMI     P.CWHT
	ST      WHOOPS(A4)
	MSG     IDFY
	MSG    LINE
	MOVE.L  (A7)+,D7
	BRA.S   90$
40$
	TEST_W  ADDNOUN(A4)
	BEQ     P.CWHT
90$
	RET
 
*--------------------------------
	
   
CHK6PART
 
* D1=CONJ'S      D7=FORCE 4 PART
  
	CMP.L   CONJ(A4),D1
	BEQ.S   10$
	SWAP    D1
	CMP.L   CONJ(A4),D1
	BEQ.S   30$
	TEST_B  D7
	BNE.S   20$
	RET
20$
	CMP.W   CONJ(A4),D1        ;SAME?
	BNE     P.CWHT
	BRA.S   CHK4PT
30$
	PUSH_L  D0/A0        ;SAVE
	SWAP    D1                 ;KEEP CONJ CONSISTENT INCASE
	MOVE.L  D1,CONJ(A4)
	MOVE.W  ADDNOUN+2(A4),D0
	GETINFO
	MOVE.L  ADDNOUN(A4),D0
	MOVE.L  A0,A5              ;NEW DATA
	MOVE.W  D0,D5              ;NEW
	SWAP    D0
	MOVE.L  D0,ADDNOUN(A4)
	PULL_L  D0/A0
     
10$
	MOVE.L  ADDNOUN(A4),D1
	TEST_W  D1
	BEQ.S   40$
	SWAP    D1
	TEST_W  D1
	BEQ.S   40$
	RET
40$
	MOVE.W  CONJ(A4),D1
	BRA     P.CWHT
  
*--------------------------------
   
P.CONJ
 
	MOVE.L  A1,-(A7)
	LEA     CONJST(A4),A1
	CALL    P.TAB
	MOVE.L  (A7)+,A1
	RET
  
*--------------------------------
*
* P.TAB - print a word from the word table
*
* takes: A1 points as start of section
*	 D1 word# to print
*
* On exit D1 is the last character in the word (hi-bit set)
*
* NB puts a space on the end of the word
*

   
P.TAB
 
	MOVE.L  D0,-(A7)           ;SAVE
	TEST_W  D1   
	BEQ.S   20$                ;DONE ALREADY

	IFEQ	FastBits

10$
	MOVE.B  (A1)+,D0           ;GET 
	BPL.S   10$                ;UNTIL END   
	SUBQ.W  #1,D1              ;DC     
	BNE.S   10$                ;UNTIL CORRECT

	ENDC

	IFNE	FastBits

	EXG	D1,D2		;WORDSCAN wants D2 and A1
	DC.W	$A0FB	
	EXG	D1,D2

	ENDC


20$    MOVE.B  (A1)+,D1
	BMI.S   30$
	CALL	CHAROUT		;call for speed
	BRA.S   20$
30$
	CALL	CHAROUT		;call for speed
	MSG    SPACE
	MOVE.L  (A7)+,D0           ;BACK
	RET

*******************************************************
*
* P.VERB - print a verb 
*
*
* takes D1 is the verb# (starts at 0 = look)
*

P.VERB

	PUSH_L	D1/A1
	LEA	WTAB_VERBS(A4),A1
	CALL_S	P.TAB
	PULL_L	D1/A1
	RET

*******************************************************
*
* P.PARTICLE - print a particle  (things stored in ADVERB!)
*
*
* takes D1 is the particle no.
* 
* Hi bit set => up/down - ie DIRECTION section
*

P.PARTICLE

	TEST_W	D1		;MI => up/down
	BPL	P.CONJ		;else just a normal 'conj' prep.

	PUSH_L	D1/A1
	LEA	L.DIREC(A4),A1
	AND.W	#$7FFF,D1	;fod hi bit
	CALL_S	P.TAB
	PULL_L	D1/A1
	RET

 
*--------------------------------
*
* ISAREN - prints is/are not
*
* assumes PLURAL is set up 
*

  
ISAREN
 
	DO	ISARE
	MSGGO	NOT
 
*--------------------------------
* 
* FIXCONT - FIXS THE 'SIZE' OF
*           A CONTAINER IF
*           SOMETHING IS REMOVED
*           FROM IT
*
* REMOVE ITEM IN D0
*
*
* STOMPS ON A2
*
*--------------------------------
  
FIXCONT
 
	PUSH_L  D0/D1/A0/A2
	GETINFO
	MOVE.L  A0,A2
	BTST    #5,6(A0)           ;CONTAINED?
	BEQ.S   90$
	MOVE.W  8(A0),D0           ;GET CONTAINER
	GETINFO		            ;TO A0
	MOVE.B  10(A0),D0          ;HB OF DATA
	AND.B   #7,D0              ;LOW 3
	MOVE.B  2(A2),D1           ;S&WEIGHT
	LSR.B   #4,D1              ;SIZE
	ADD.B   D1,D0              ;INC
	CMP.B   #7,D0
	BHI     DERR
	MOVE.B  10(A0),D1          ;AGAIN
	AND.B   #$F8,D1            ;TOP 5
	OR.B    D0,D1              ;COMBINE
	MOVE.B  D1,10(A0)          ;UPDATE
90$
	PULL_L  D0/D1/A0/A2
	RET
 
*--------------------------------

FNDEND

* FIND THE END OF A STRING OF CONNECTED 
* PARTICLES

	GETINFO
	LEA     LOOPBUF(A4),A1
	CLR.W   (A1)               ;INIT
10$:
	MOVE.W  12(A0),D1          ;FIX DATA
	AND.W   #$3FFF,D1          ;MASK TOP 2
	BEQ.S   90$                ;THE END
	MOVE.L  A1,A2              ;SAVE
	LEA     LOOPBUF(A4),A1
20$
	MOVE.W  (A1)+,D3           ;GET ENTRY
	BEQ.S   50$                ;SCANNED
	CMP.W   D3,D1              ;SCAN FOR MATCH
	BNE.S   20$                ;NONE, OK
	LEA     LOOPBUF(A4),A1
	CLR.W   D0
11$
	MOVE.W  (A1)+,D1
	BEQ.S   90$
	CMP.W   D1,D0
	BHI.S   11$
	MOVE.W  D1,D0
	BRA.S   11$
90$
	RET
50$    
	MOVE.L  A2,A1              ;BACK
	MOVE.W  D1,(A1)+           ;ONTO BUF
	CLR.W   (A1)
	MOVE.W  D1,D0
	GETINFO
	BTST    #4,3(A0)           ;MOVEABLE?
	BNE.S   80$                ;ZERO=NO
	ST      NMOVEFLG(A4)
80$
	BRA.S   10$                ;DO REST

*--------------------------------
  
FIXSYS
	PUSH_L  D0-D7/A0-A6        ;STASH
	CLR.W   NMOVEFLG(A4)       ;CLEAR NOT MOVEABLE FLAG
	LEA     EXBUFF(A4),A3      ;TARGET FOR SYS
	CALL    FNDEND             ;OF CHOICE
	MOVE.W  D0,D4              ;SAVE
	MOVE.W  #NOUNNUM,D2        ;COUNTER
05$    
	IFNE	FastBits

* GETTIED finds the previous noun which has (12(A0) & 0x3FFF) !=0
* it takes and returns D2.W
* if a tied noun IS found CARRY is set, else cleared

	DC.W	$A0F2		;GETTIED
	BCC.S	20$

	ENDC

	MOVE.W  D2,D0
	CALL    FNDEND             ;OF EACH
	CMP.W   D0,D4              ;FIXED TO END?
	BNE.S   10$                ;NO
	MOVE.W  D2,(A3)+           ;YES, ONTO BUF
10$
	SUBQ.W  #1,D2
	BNE.S   05$                ;DO ALL
20$
	CLR.W   (A3)               ;DT=0
	TEST_B  D5
	BEQ.S   FX90
FX20
	LEA     EXBUFF(A4),A0
	MOVEQ   #1,D5
	CALL	P.LIST2
	MSG     ACON
FX90
	PULL_L  D0-D7/A0-A6        ;RESTORE 
	LEA     EXBUFF(A4),A3
	RET

*--------------------------------
  
P.BTN

	MSG     BUT
	DOGO	P.TN
   

*--------------------------------

*******************************************************************
*
*  P.VERBING
*  Prints out the verb with 'ing' tagged on the end
*
*  This routine is DUMB ie: it will not cope with 
*  any irregularity!
*
*  On entry D1 = verb# to print
*
*******************************************************************

P.VERBING

	DO	P.VERB			;prints the verb
	DO	UTIL.BS			;backspace over space
	PRINT 	<'ing '>		;add on the ing!
	RET				;And return


*--------------------------------
*
* P.BTNISALDY
*
* PRINT BUT THE NOUN IS/ARE ALREADY
* MSG# D1
*
*--------------------------------
 
P.BTNI
 
	DO	P.BTN
	DO	ISARE
	MSG	ARDY
	DOGO	P.MSG

****************************************************************************
*
*  TOPLOC you pass it d0 as object
*  returns D0 as room number of the top location
*  won't work on pseudos and d0=0 means inventory
*
****************************************************************************

 
TOPLOC 
 
	GETINFO	
	MOVE.B  6(A0),D1
	AND.W   #$BF,D1                  ;ALLOW ROOMS
	BEQ.S   10$
	MOVE.B	7(A0),D1		;I know top byte of D1.W is zero
	BNE.S	30$
	MOVE.W  8(A0),D0
	BRA.S   TOPLOC
30$
	MOVE.W	D1,D0
	BRA.S	20$
10$
	MOVE.W  8(A0),D0
20$
	RET

*
* FindRoom - find the room# of a given noun.
*
* entry:   D0 = noun#
*
* exit:    D0 = room#, D7 = status
*
* if the object is OUT                         D7 =-1
* if it is carried by the player               D7 = 4
* if it may be got to (ie none of the above)   D7 = 0
*
* if the object is a pseudo the first location it's in is returned
* if it's related to one & secondary location is zero ditto
* if it's related but only in one place, thats what u get

	XDEF	FindRoom

FindRoom

	PUSH_L	D1/A0		;trying not to stomp too much
10$
	GETINFO
	BTST	#0,5(A0)		;pseudo?
	BNE.S	50$

	MOVE.W	6(A0),D1
	BTST	#7+8,D1			;out?
	BNE.S	30$

	BTST	#6+8,D1			;room?
	BNE.S	35$

	TEST_B	D1			;check secondary loc.
	BNE.S	60$			;is related(ish) to one occurance of
					;a pseudo

	AND.W	#$3300,D1		;contained/insep/hidden/related allowed
	BEQ.S	20$			;else player must have it

	MOVE.W	8(A0),D0		;next noun up
	BRA.S	10$

* get here => player has it - so give 'em player's room#

20$
	MOVE.W	8(A0),D0		;really in a room?
	BNE.S	40$			;NE => yes, so pass
	MOVE.W	ROOM(A4),D0		;setup room#
	MOVEQ	#4,D7			;flag it's on the player
	BRA.S	90$


* get here => it's out or associated with a pseudo

30$
	MOVEQ	#-1,D7
	BRA.S	90$

* get here if it's in a room, then pass

35$
	MOVE.W	8(A0),D0

* get here => pass!!

40$
	MOVEQ	#0,D7
	BRA.S	90$

* get here if a pseudo is involved

50$
	MOVE.W	8(A0),D1			;get pseudo offset
	LEA	PSUDAT(A4),A0			;and start of data
	MOVEQ	#0,D0				;kill odd bits
	MOVE.B	0(A0,D1.W),D0
	BRA.S	40$				;pass

* get here if the secondary loc. is non zero

60$
	MOVEQ	#0,D0
	MOVE.B	D1,D0		;get room#
	BRA.S	40$		;pass

90$
	PULL_L	D1/A0
	RET
	
    

	END

