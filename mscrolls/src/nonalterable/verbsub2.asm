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
*	@(#)verbsub2.asm	2.22
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.VERBSUB2
*
*--------------------------------
 
	include	"equates.asm"
	include	"macros.asm"
	include	"nounequ2.i"      ;FOR NOUNNUM
	include "verbequ.asm"

	 XREF    ERRHOLD,ERR1,ERR4,VERB,W.BTINA,W.NO.TNG,PLURAL
	 XREF    CHAROUT,W.FERG,W.BECOF,SUBJECT,TOPNPC,LASTA0,VALID.NP
	 XREF    W.ICNDTW,PROC.LAB,YCANT,BRK.VERB,SUBOBJ,D72
	 XREF    W.ARE,W.IT,W.IS,W.THEM,W.THEY,W.HE,W.SHE,W.HIM,W.HER,W.THE
	 XREF    GETNPC,ADJECTIV,LIST.N,ADJ.TBL,INITNUM,W.CNTSEE,W.HERE
	 XREF    WASMOD2,SETPRON,NOTRAIL,UTIL.SP,LISTWRD,UTIL.DEL,LINE
	 XREF    FMSG,CAPITAL,LISWRD,GETDATA,PRTTXT,W.NT,W.YOU,W.AN,SCANFLAG
	 XREF    P.DESC,P.LIST,P.LIST2,P.LOR,P.QN,P.NQN,SP3.FLAG,GETRELN
	 XREF    P.NOUN,P.NOUN2,NONLY,P.TN,P.QUANT,P.PRN,P.OBJPRN,ISARE
	 XREF    P.PV,P.VQN,P.SUBOB,P.SUB,DERR,STOPBRK,YCANTW,CARRIED
	 XREF    P.BOTN,P.NO.TNG,P.NOTANY,ISDOFRE,HOLDE,UNHOLDE
	 XREF    W.FIND,ROOM,DBSFLG,P.NODO,GETNOUN2,LOOPBUF,L.DIREC
	 XREF    MATCHBUF,FIXSYS,W.NEASY,W.CON,P.CONJ,NETWORK,W.FIND
	 XREF    UTIL.BS,EXBUFF,SEARCH2,RNOUNRES,RNOUNLST,BSFULLSTOP

	 XREF	DNOUNBUF,P.INIT,SCRPNT,OUTMODE,FINRELNPC,CHEAT,SATMSG
	 XREF	USER.DB,SEATED,RIDPRT,FINNPC,KILLPSEUDO,KILLNPC
	 XREF	ONLYREL,ISANYREL,W.THERE,KILLNOTNPC,W.I2DARK,P.LOC,LIGHT
	 XREF	FULLSTOP,P.TNSTOP


      IFNE	Graphics
	XREF	PICTURES
      ENDC


	 XDEF    P.SUBV,P.NFND,P.NQEN,SCANSV,DBS,OK,CALCWHT,P.BRAKTS
	 XDEF    GET_CONT,R.NOUN,R.NOUN2,SPACE,ROOTNUM,P.NPCMSG,P.MSG,P.SCN
	 XDEF    DO.FIX,SETA1,P.CNTO,P.FROM,P.WITH,CONNECT,CONNECT2,SCAN
	 XDEF	P.Refresh,ForceRefresh

*--------------------------------
*
*  P.SUBV - PRINTS SUBJECT,VERB
*
* EG - 'YOU ARE'
*      'HE IS' (IF SUBJECT = HORSE SAY)
*
* P.S. - VERB IS ALWAYS 'TO BE'
*
*--------------------------------
 
  
P.SUBV
  
	TEST_W  SUBJECT(A4)        ;PLAYER?
	BNE.S   10$                ;NO
	MSG     YOU
	MSGGO	ARE  
10$
	DO	P.SUB
	MSGGO	IS
 
*--------------------------------
*
* P.NFND - prints the 'You can't see an xyzzy here' message
*
* takes D0 = noun#
* sets D7 = 2 to indicate error message been printed, if it was 2 (ie D7==2)
* to start with then no message is printed
*
   
P.NFND
  
	CMP.W   #2,D7			;ERROR ALREADY?
	BEQ.S   90$			;YES = BYE...

	DO	P.SUB			;YOU..
	MSG     CNTSEE			;CANNOT SEE
	CALL_S  P.NQEN			;NEG ERROR NOUN
	MSG     HERE			;HERE'

80$
	MOVEQ   #2,D7			;FLAG ERROR DONE
90$ 
	RET				;EXIT
  



*--------------------------------
*
* P.NQEN -   THIS IS A CLEVER LITTLE NO.
*            WHICH DEALS WI STUFF LIKE
*            SUSSING IF IT SHOULD GIVE U
*            AN ADJ WHEN IT TELLS U IT CANNOT
*            SEE A NON LOCALLY
* 
* DEAD HANDY FOR CASES LIKE A BLUE KEY IN
* A ROOM, PLAYER TYPES 'L METAL KEY'
* A REPLY LIKE 'U CANNOT SEE A KEY HERE'
* IS REAL SILLY, SO IT GIVES 'METAL KEY'
*
*--------------------------------
 
P.NQEN
	
*PRINT NEG QUANTIFYER ERROR NOUN
	
	GETINFO
	MOVE.W  4(A0),-(A7)        ;SAVE SOME DATA
	BCLR    #4,5(A0)           ;NO FORCE
	MOVE.B  #1,INITNUM(A4)     ;SET NO. IN ROOM TO 1
	CALL    P.NQN
	CLR.B   INITNUM(A4)        
	MOVE.W  (A7)+,4(A0)
	RET
	
*--------------------------------
*
* PRINTS MESSAGE NO. IN D0
*
* (ADDS #NOUNNUM THEN CALL
* FMSG TO DECOMPRESS IT)
*
*--------------------------------

	include	"msginfo.i"

	XDEF	MsgMSGBASE		;no of nouns really
	XDEF	MsgSCNBASE		;no of prog messages
	XDEF	MsgNPCBASE		;no of scen msgs and prog msgs


P.NPCMSG

	ADD.W	#MsgNPCBASE,D1
	BRA.S	P.MSG
    
*--------------------------------
*
* P.SCN - FOR SCENARIO MSGS
*
*--------------------------------
 
P.SCN
 
*	ADD.W   #PMSGNUM,D1

	ADD.W	#MsgSCNBASE,D1
   
*--------------------------------
*
* P.MSG - FOR PROGRAM MSGS
*
*--------------------------------
 
P.MSG
 
*	ADD.W   #NOUNNUM,D1
	ADD.W	#MsgMSGBASE,D1

	CMP.W	#NOUNNUM,D0
	BCC.S	10$			;don't try on non-real noun#
	MOVE.L	A0,-(A7)
	GETINFO
	BTST	#0,(A0)			;is noun plural?
	SNE	PLURAL(A4)
	SNE	PLURAL+1(A4)		;horrid eh...
	MOVE.L	(A7)+,A0
10$
	EXG     D0,D1
	CALL    FMSG
	EXG     D0,D1
	RET
 
*--------------------------------
*
* SCAN - FINDS NOUNS WHICH ARE
*        HIDDEN BY NOUN IN D0
*
* EG, PLANT POT ON BENCH
*     COIN ON CUSHION
*
* WANTS D0=NOUN#
* IT PRINTS OUT IF NESS. & SETS
* PRONOUNS AS PER DISCOVERED ITEM
*
*--------------------------------
 
SCAN
	MOVEQ   #0,D7              ;ERR FLG
	MOVE.W  D0,D2              ;TARGET
	ST      SCANFLAG(A4)
	DO	GETRELN	           ;GETS RELATED NOUNS (D5=1,D1=8,GETNOUNS)
	CLR.W   SCANFLAG(A4)
10$
	MOVE.W  (A1)+,D0           ;GET NOUN
	BEQ.S   90$
	GETINFO                    ;ON D0
	BTST    #1,6(A0)           ;HIDDEN
	BEQ.S   10$
	TEST_W  SP3.FLAG(A4)       ;SUPRESS OUTPUT?
	BNE.S   76$
	CALL    SETPRON            ;SET HE,SHE,IT ETC AS REQUIRED

      IFEQ	THE_PAWN

	XREF	ScanSpecials

	CALL	ScanSpecials		;so it can say, u grovel around..
	BEQ.S	76$			;EQ => we said something, so shutup
     
      ENDC

	DO	P.SUB	             ;TELL THE LUCKY PLAYER
	MSG     FIND
	DO	P.QN

      IFEQ	THE_PAWN

	DO	BSFULLSTOP

      ENDC
76$
	BCLR    #1,6(A0)           ;TO ITEM
	MOVE.B  6(A0),D7
	BNE.S   10$                ;EXIT
	MOVE.W  ROOM(A4),8(A0)     ;THEN IS IN ROOM
	MOVEQ   #1,D7              ;DONE
	BRA.S   10$                ;NEXT
90$
	TEST_W  SP3.FLAG(A4)       ;STOP NHAP MES?
	BEQ.S   99$
	MOVEQ   #1,D7              ;FAKE ITEM FOUND
99$:
	RET
  
*--------------------------------
* 
* SCANSV   
*
*   SAVES REGS AND CALLS SCAN
*
*
*--------------------------------
 
 
SCANSV
 
	PUSH_L  D0-D7/A0-A6
	CALL_S  SCAN
	PULL_L  D0-D7/A0-A6
	RET
  
  
*--------------------------------
*
* DBS - PRINTS A DO BE SERIOUS COMMENT
*
* IF THIS IS NOT THE FIRST TIME ITS
* BEEN CALLED OF LATE THEN IT PRINTS NOUN-COMMENT
*
*--------------------------------
 
 
DBS
  
	TEST_W  DBSFLG(A4)
	BEQ.S   10$
	DO	P.NOUN
	PRINT   <' - '>
10$
	CALL    P.NODO
	ADD.W   #1,DBSFLG(A4)
	MSGGO   LINE 
  
*--------------------------------
*
* OK - THIS IS AN ANCIENT ROUTINE
*      WHICH IS ONLY HERE DUE TO 
*      HISTORIC INTEREST
*
*--------------------------------
  
OK
	PRINT   <'OK '>
	RET


*--------------------------------
*
*  SRC.ADV.VERBSUB3
*
*--------------------------------
 
*--------------------------------
*
* CALCWHT - CALCULATES THE WEIGHT
*           OF A NOUN (D0) INCLUDING
*           JUNK LIKE CONTENTS ETC..
*
* ON ENTRY - D0 = NOUN#
* 
* ON EXIT D3 = WEIGHT
*
*--------------------------------
 
CALCWHT
 
	PUSH_L  D0-D2/D5/A0-A1
	CLR.L   D3
	GETINFO
	CALL_S  80$
	MOVE.W  D0,D2
	DO	GETRELN	
10$
	MOVE.W  (A1)+,D0
	BEQ.S   90$
	GETINFO
	MOVE.W  6(A0),D1
	AND.W   #$3300,D1          ;REL,INSEP,CONT,HIDDEN ALLOWED
	BEQ.S   10$
	BTST    #0,5(A0)           ;NOT PSEUD
	BNE.S   10$
	BTST    #7,6(A0)           ;NOR OUT
	BNE.S   10$
	CALL_S  80$
	BRA.S   10$
80$   
	MOVE.B  2(A0),D1
	AND.W   #$0F,D1
	ADD.W   D1,D3
	RET
90$
	PULL_L  D0-D2/D5/A0-A1 
	RET
 
*--------------------------------
*
* GET_CONT -     GETS ALL THE NOUNS
*                WHICH ARE CONTAINED
*                BY D0
*
* LEAVES THE LIST POINTED TO BY A1
* ZERO (WORD) TERMINATED
*
* Warning!!!! - list contains hi-bit set noun#'s of non-contained objs
*
*--------------------------------
 
GET_CONT
  
* OF D0
	PUSH_L  D0-D6/A0/A2-A6
	GETINFO
	MOVE.B  10(A0),D1
	AND.B   #7,D1              ;VOLUME
	OR      #1,CCR
	BEQ.S   99$
	AND     #$FE,CCR   
 
	MOVE.W  D0,D2              ;TARGET
	DO	GETRELN	           ;TO A1
	MOVE.L  A1,A3
10$
	MOVE.W  (A1)+,D0
	BEQ.S   90$                ;DONE
	GETINFO
	BTST    #5,6(A0)           ;CONTAINED?
	BNE.S   10$                ;YES, OK
	SUBQ.L  #2,A1
	BSET    #7,(A1)
	ADDQ.L  #2,A1
	BRA.S   10$
90$
	MOVE.L  A3,A1
99$
	PULL_L  D0-D6/A0/A2-A6
	RET
 
*--------------------------------
*
* R.NOUN - GIVES NOUN NO. OF
*          CURRENT ROOM
* (ROOM# IN D2 IF R.NOUN2)
* 
* THIS ROUTINE IS NOT QUICK (CALLS GETNOUNS)
* AND MIGHT BE IMPROVED SO AS IT REMEMBERS THE
* LAST ROOM NO. U ASKED FOR, SO AS IF U ASK FOR
* IF AGAIN IT DOESNT HAVE TO THINK ABOUT IT
*
*--------------------------------
 
R.NOUN
  
	MOVE.W  ROOM(A4),D2
 
R.NOUN2

	CMP.W   RNOUNLST(A4),D2    ; Is it the same as the last time..?
	BEQ     R.NOUN.EASY        ; Yes, so its an easy one

	MOVE.W  D2,RNOUNLST(A4)    ; save which room we were scanning for
	MOVEQ   #1,D6              ;SO GETNOUNS IGNORES VALID
	PUSH_L  D1/D5/A0/A1
	MOVEQ   #8,D1   
	MOVEQ   #1,D5  
	DO	GETNOUN2
10$
	MOVE.W  (A1)+,D0
	BEQ.S   50$
	GETINFO
	BTST    #6,6(A0)           ;ROOM?
	BEQ.S   10$
	PULL_L  D1/D5/A0/A1
	MOVE.W  D0,RNOUNRES(A4)
	RET
50$
      IFEQ    PROD
	PRINT   <'NO ROOM'>
      ENDC
  
	PULL_L  D1/D5/A0/A1
	BRA     DERR

R.NOUN.EASY

	MOVE.W  RNOUNRES(A4),D0     ;Just re-fetch the last result
	RET

*--------------------------------
*
* SPACE - PRINTS A SPACE!
*
* PRETTY SIMPLE, SAVE REGS
* NO STOMPING
*
*--------------------------------
 
SPACE
 
	MOVE.L  D1,-(A7)  
	MOVEQ   #$20,D1 
	DO	CHAROUT
	MOVE.L  (A7)+,D1  
	RET
 
*--------------------------------
*
* Rootnum - finds no of nouns with the same 'root' (like key)
* that are local
*
* takes D0 = noun#
*
   
ROOTNUM
   
* CALCS NUMBER OF ROOT-WORD
* MATCHES IN LOCALITY
   
	PUSH_L  D0-D3/D5-D7/A0-A6
	LEA     LIST.N(A4),A6

      IFEQ	FastBits

	SUBQ.W  #1,D0
10$
	MOVE.B  (A6)+,D1           ;CHAR
	BPL.S   10$
	SUBQ.W  #1,D0              ;COUNTER
	BNE.S   10$                ;END?

      ENDC

      IFNE	FastBits

	SUBQ.W  #1,D0
	BEQ.S	10$

	MOVE.L	A6,A1
	MOVE.L	D0,D2
	DC.W	$A0FB		;call WORDSCAN
	MOVE.L	A1,A6
10$

      ENDC


20$
	LEA     LOOPBUF(A4),A3
	MOVE.L  A3,A2
21$
	MOVE.B  (A6)+,(A2)+		;copy word from wtable 
	BPL.S   21$
	MOVE.B  #' ',(A2)		;put a ' ' at the end of it
	BCLR    #7,-(A2)		;FOD the hi bit term.
	MOVE.L  A3,A6			;and fake input for SEARCH (2)
	LEA     LIST.N(A4),A3
 
	MOVE.B  L.DIREC(A4),-(A7)	;SAVE FIRST BYTE AFTER NOUNS
 
	MOVE.B  #$81,L.DIREC(A4)	;PUT WORDTABLE TERMINATER IN
 
	MOVE.W  D4,-(A7)		;SAVE
	CLR.W   D6
	DO	SEARCH2			;INTO MATCHBUF
	MOVE.W  (A7)+,D4
 
	MOVE.B  (A7)+,L.DIREC(A4)	;NOTE THAT THIS ADDS 2 TO A7  !!
     
	MOVE.W  #$FFFF,(A2)		;DT OF BUF
	LEA     MATCHBUF(A4),A2

	DO	ISDOFRE			;EQ => always local
	SEQ	D2
30$
	MOVE.W  (A2)+,D1		;SECTION
	BMI.S   50$			;END OF MATCHES
	MOVE.W  (A2)+,D0		;#
	ADDQ.W  #1,D0			;convert to noun#
	TEST_W  D1			;SECTIN #0? (ie NOUN)
	BNE.S   30$			;SKIP
	TEST_B	D2			;was DO free? (NE => yes)
	BNE.S	40$			;in that case don't do a valid
	DO	VALID.NP
	TEST_B  D7
	BNE.S   30$			;FAIL?
40$
	ADDQ.B  #1,D4			;COUNTER
	BRA.S   30$
50$
	PULL_L  D0-D3/D5-D7/A0-A6
	RET
 
*--------------------------------
*
* DO.FIX - loops around through a fixtures buffer calling 
*          a verb routine (such as get/drop/put) on each noun
*
   
DO.FIX

	MOVE.W  D5,-(A7)
	MOVEQ   #0,D5              ;WAS 1 NOW 0 SO AS NOT TO PRINT
	DO	FIXSYS
	CALL_S  SETA1
	MOVE.W  (A7)+,D5
10$
	MOVE.W  (A1)+,D0
	BEQ.S   20$
	GETINFO
	BTST    #4,3(A0)           ;MOVEABLE?
	BNE.S   10$                ;YES, NEXT..
	BSET    #7,-2(A1)          ;NOT ON!
	DO	CARRIED	           ;THE DREADED BAND???
	BNE.S   10$                ;NOPE, NEXT..
	MSGGO	NEASY            ;TUT TUT GOOD TRY..
20$
	CALL_S  SETA1              ;GET EXBUFF
30$
	MOVE.W  (A1)+,D0           ;GET NOUN #
	BMI.S   30$                ;NON MOVEABLE?
	BEQ.S   SETA1              ;FINISGHED?
	GETINFO                    ;NOPE GET NOUN DATA
	MOVE.L  A1,-(A7)           ;SAVE EXBUFF POINTER
	JSR     (A6)               ;CALL VERBROUTINE
	MOVE.L  (A7)+,A1           ;RECOVER EXBUFF POINTER
	BRA.S   30$                ;NEXT...
SETA1
	LEA     EXBUFF(A4),A1
	RET

*--------------------------------
*
* P.CNTO - the <noun1> is/are connected to the <noun2>
*
* D0 = noun1
* D1 = noun2
*
   
P.CNTO

	GETINFO
	MOVE.W  12(A0),D1
	AND.W   #$3FFF,D1
	BEQ.S   90$
	DO	P.TN
	DO	ISARE
	MSG     CON
	MOVE.W  D1,D0
	DO	P.TN
	GETINFO
	TEST_W  D1
90$
	RET

*--------------------------------
  
P.FROM
	
	MOVE.L  D3,-(A7)
	MOVEQ   #2,D3
	BRA.S   WithFrom
	
*--------------------------------
	
P.WITH
	
	MOVE.L  D3,-(A7)
	CLR.W   D3

WithFrom

	CALL_S  P.BRAKTS
	MOVE.L  (A7)+,D3
	RET

*--------------------------------
	
P.BRAKTS
	
	PUSH_L   D0/A0
	MOVE.B  #'(',D1
	DO	CHAROUT
	MOVE.W  D3,D1
	CALL    P.CONJ
	MOVE.W  D2,D0
	DO	P.TN
	DO	UTIL.BS
	MOVE.B  #')',D1
	DO	CHAROUT
	MSG     LINE
	PULL_L   D0/A0
	RET


*
* CONNECT3 - used by Connect macro
*
* picks d0/d1/d2 forom code
*

	XDEF	CONNECT3

CONNECT3
	
	MOVE.L	(A7),A0			;get PC (CONNECT2 steps on A0)
	MOVE.B	(A0)+,D0
	MOVE.B	(A0)+,D1
	MOVE.B	(A0)+,D2
	ADDQ.W	#1,A0
	MOVE.L	A0,(A7)			;restore ret addr
	BRA.S	CONNECT2
  
	
*--------------------------------
*
* CONNECT - PUTS ROOM# IN D2
*           INTO NETWORK AS DIR
*           GIVEN IN D1 FROM ROOM
*           THAT PLAYER IS IN 
*
*--------------------------------
 
   
CONNECT
 
	CLR.L   D0
	MOVE.W  ROOM(A4),D0
 
 
*--------------------------------
*
* CONNECT2 - AS ABOVE BUT FROM 
*            ROOM GIVEN IN D0
*--------------------------------
 
 
CONNECT2
	
	AND.L   #$FF,D1
	AND.W   #$FF,D0            ;ROOM # LESS THAN $7F
	SUBQ.W  #1,D0


      IFNE     M68000

	MULU    #Sizeof_NET,D0

      ENDC

      IFEQ     M68000

	MOVE.W  D0,-(A7)
	LSL.W   #3,D0
	ADD.W   (A7),D0

* Deal with bigger than original network

       IFNE	Sizeof_NET-10

	ADD.W	(A7),D0
	ADD.W	(A7),D0
	
       ENDC

	ADD.W   (A7)+,D0

      ENDC

	ADD.L   D1,D0
	LEA     NETWORK(A4),A0
	ADD.L   D0,A0
	MOVE.B  D2,(A0)            ;FIX
	RET

*************************************************************
*
* ForceRefresh - forces a full description (if u can see)
*                of ROOM
*
* includes all-sorts of bits like lists of npcs/items
*

ForceRefresh

	MOVE.W	#2,SCRPNT(A4)		;fake a 'look'
*	BRA	R.Refresh
	
 
*--------------------------------
*
* P.Refresh - refreshes the view the player is getting
*
* Calls P.LOC/PICTURES/P.DESC/etc
*
* This used to live in SCREEN, but can now be used from anywhere
*
* WARNING - this seems (!?!) to step on VERB, leaving it as FFFF
*           this may cause some embarasment (a la MAKELOCAL!)
*

P.Refresh

	PUSH_L	D0-D7/A0-A6	   ;save registers

	TEST_W  LIGHT(A4)          ;Can people see?
	BNE.S   10$                ;NE=>yes
	MSG	I2DARK		   ;EQ=> too dark to see
	MSG     LINE
	BRA     DoInits            ;miss print room desc etc
10$
	DO	R.NOUN		   ;get noun# of room
	CALL    P.LOC		   ;top line
	
	GETINFO        

	MOVE.W  D1,-(A7)
	MOVEQ   #-1,D1             ;now, to FOD capital call CHAROUT with
	DO	CHAROUT	           ;D1 = -1
	MOVE.W  (A7)+,D1

	DO	P.NOUN

   IFNE	   Graphics&C64
	XREF	CAMEOS
	CALL	CAMEOS		   ;NB takes D0/A0 as current room!
				   ;and wants to look at D1 !! 
				   ;sneeky call to PICTURES
   ENDC

	CALL    SATMSG
	CMP.W   #2,SCRPNT(A4)	   ;did we do a look last go?
	BEQ.S   30$	       	   ;EQ=> yes so force room desc etc.

	BSET    #4,4(A0)           ;been here before (explored bit)
	BEQ.S   20$                ;No, do room desc unless BRIEF mode
	CMP.B   #1,OUTMODE(A4)     ;0=NORMAL
	BNE.S   40$                ;1=VERBOSE

20$         
	CMP.B   #2,OUTMODE(A4)     ;2=BRIEF
	BEQ.S   40$

30$

      IFEQ	THE_PAWN
	XREF	FirstGlance,OtherDescription
	CALL	OtherDescription		;ne => was one, kill others
	BNE.S	35$
      ENDC

	CALL	FMSG			;'real' desc

      IFEQ	THE_PAWN
	CALL	FirstGlance
	CALL	UntilDisturbed
      ENDC

	PRINT	<'. '>		;350 nouns * 1 full stop = lots of memory
35$
	DO	R.NOUN             ;room# ->D0
	GETINFO                    ;      ->A0

      IFNE     Graphics
	CALL    PICTURES
      ENDC
	 
40$

	CALL    FINRELNPC

      IFNE    YCHEAT               ;If able to be in REAL DB mode
	TEST_W  CHEAT(A4)          ;and in it
	BNE.S   50$                ;then do all the (invis) junk anyhow
      ENDC

      IFNE	THE_PAWN!YCHEAT
	XREF	SUSSINV
	TEST_W  USER.DB(A4)        ;THEY WON & 'DBING'?
	BEQ.S   60$                ;EQ => no, don't do (invis) stuff
50$
	CALL    SUSSINV            ;Print a (list) of invis (related) objects
	
      ENDC
	
60$
	MOVE.W  ROOM(A4),D2        ;get current room no.
	TEST_W  SEATED(A4)         ;are we all sitting comfortably?
	BEQ.S   ListOBJ		   ;OK, we're no (eq)
	BSET    #1,RIDPRT(A4)      ;SET AS DONE
	BNE     DoInits            ;ALREADY, SKIP IT
	MOVE.W  SEATED(A4),D2

* First list the objects in the room (simply in room)

ListOBJ

	MSG     LINE
	DO	GETRELN	           ;CREATE BUFFER
	CALL    FINNPC             ;separate lists of NPC's and ordinary nouns
	CALL    KILLPSEUDO         ;don't list pseudo nouns
	TEST_W  SEATED(A4)
	BNE.S   20$                ;do not supress NPC list if riding
	CALL    KILLNPC            ;loose NPC's from list
10$
	MOVE.W  (A1)+,D0
	BMI.S   10$                ;HIGH BIT = NOT REALLY THERE
	BEQ.S   ListNPCs           ;0 = END OF BUFFER
	BRA.S	30$
20$
	CALL    ONLYREL            ;Remove RELATED objects from list
	CALL    ISANYREL
	BEQ.S   ListNPCs           ;if none related forget list entirely
30$
	GETINFO                    ;
	MSG	THERE              ;
	DO	ISARE	             ;  } print list of non-NPC nouns
	LEA     DNOUNBUF(A4),A0    ;
	DO	P.LIST             ;
	MSG	HERE               ;

* Now list the NPCS hanging around

ListNPCs

	LEA     DNOUNBUF(A4),A1
	CALL    KILLNOTNPC
10$
	MOVE.W  (A1)+,D0
	BMI.S   10$
	BEQ.S   DoInits
	LEA     DNOUNBUF(A4),A0    ;
	DO	P.LIST             ;
	GETINFO                    ;
	BTST    #0,(A0)            ;
	BNE.S   20$                ;
	TEST_B  D6                 ;
	BNE.S   20$                ;   } print list of NPC's in room
	MSG	IS                 ;
	BRA.S   30$                ;
20$
	MSG	ARE                ;
30$
	MSG	HERE               ;

* do the initial states, fod flag forcing description (SCRPNT)

DoInits

	CLR.W   SCRPNT(A4)         ;Only give one description of room!
	CALL	P.INIT             ;do initial state descriptions

	PULL_L	D0-D7/A0-A6	   ;recover
	RET

	

      IFNE	TimeCode

*******************************************************************
*
* GetTime - get a no. from the 'input line' (shadow NumBuf really)
*
* Returns D2=time in Kerovnian Standard Time (KST)
*
*******************************************************************

	XDEF	GetTime,GetNumber
	XREF	WhichNum,NumBuf

GetTime

	MOVE.L	D3,-(A7)	;else will get stomped

	CALL	GetNumber	;returns D2/D3 (before/after ':')
	CMP.L	#24,D2
	BGE.S	BadTime
	CMP.L	#60,D3
	BGE.S	BadTime

	LSL.L	#2,D2		;*4
	MOVE.L	D2,-(A7)
	LSL.L	#4,D2		;and *16 = 64
	SUB.L	(A7)+,D2	;less 4* the number you first thought of...
	ADD.L	D3,D2		;give KST !!! (simple eh?)

* THIS IS WHERE KST MODIFICATIONS GO
	
	MOVE.L	(A7)+,D3
	RET	

BadTime

	MOVEQ	#-1,D2			;ouch bad time!
	MOVE.L	(A7)+,D3
	RET

    
*
* GetNumber
*
*
* returns D2 as LHS, D3 as RHS of a xyzzy:plugh format time
*
* if no ':' is found D3 will be 0
*

GetNumber

	PUSH_L	D1/D6/A3

	MOVEQ	#0,D1			;kill D1+D2+D3
	MOVE.L	D1,D2
	MOVE.L	D1,D3

	MOVE.W	WhichNum(A4),D6
	LEA	NumBuf(A4),A3
	ADD.W	D6,A3			;get addr. of cur. pos in buffer
	MOVE.B	(A3)+,D1
	BEQ.S	90$			;no number
	ADDQ.W	#1,WhichNum(A4)
10$
	MOVE.B	(A3)+,D1
	BLE.S	90$			;end of no.
	ADDQ.W	#1,WhichNum(A4)
	CMP.B	#':',D1
	BEQ.S	20$
	BGT.S	80$
	CMP.B	#'0',D1
	BLT.S	80$
	AND.W	#$0F,D1
	ADD.B	D2,D2
	MOVE.L	D2,-(A7)
	LSL.L	#2,D2
	ADD.L	(A7)+,D2
	ADD.L	D1,D2
	BRA.S	10$
20$
	MOVE.B	(A3)+,D1
	BLE.S	90$			;end of no.
	ADDQ.W	#1,WhichNum(A4)
	CMP.B	#'9',D1
	BGT.S	80$
	CMP.B	#'0',D1
	BLT.S	80$
	AND.B	#$0F,D1
	ADD.B	D3,D3			;*2
	MOVE.L	D3,-(A7)
	LSL.L	#2,D3			;and *4 == *8
	ADD.L	(A7)+,D3		;2+8 = 10 so *10
	ADD.L	D1,D3
	BRA.S	20$
80$
	MOVEQ	#-1,D2			;ouch bad number
	MOVEQ	#-1,D3

90$
	PULL_L	D1/D6/A3
	RET


      ENDC

      IFEQ	THE_PAWN

************************************************************
*
* UntilDisturbed
*
* Called when doing a room desc. Will produce a SCN msg (which appears
* tagged onto the main body of the room desc) for nouns in its list
* which are related to the room and NOT explored. If they are explored
* then it will 'de-relate' them and drop them in the room ONLY IF the hi-bit
* of the msg# is set, otherwise it will keep doing the msg as long as
* the object is there. (phew...)
*
* this stuff is vaguely safe since people can't (hopes) relate things to ROOMS
*
* format of table:
*                   DC.W noun#,noun# of obj it's related to,scn msg#
*                   word zero term
*
* ON ENTRY D0 = noun# of room
*
***************************************************************************

	XDEF	UntilDisturbed
	XREF	UntilDisturbedTable,STOPRON

UntilDisturbed

	PUSH_L	D0-D3/A0-A1

	MOVE.W	D0,D2				;save for comparision
	LEA	UntilDisturbedTable(A4),A1
10$
	MOVE.W	(A1)+,D0			;noun#
	BEQ.S	90$
	MOVE.W	(A1)+,D3			;what we should be looking at
	MOVE.W	(A1)+,D1			;message no.
	GETINFO
	CMP.W	D2,D3
	BNE.S	10$
	CMP.W	8(A0),D3			;original & right noun?
	BNE.S	10$
	MOVE.W	D0,STOPRON(A4)			;so noun name can vary ( @ )
	MOVE.W	6(A0),D0			;location info
	AND.W	#$EFFF,D0			;don't worry about insep.
	CMP.W	#$0100,D0			;check it's ONLY related
	BNE.S	10$
	TEST_W	D1
	BPL.S	30$
	BTST	#4,4(A0)			;explored?
	BEQ.S	20$
	CLR.W	6(A0)				;put object in room
	MOVE.W	ROOM(A4),8(A0)
	BRA.S	90$				;bye
20$
	AND.W	#$7FFF,D1			;fod flag in msg#
30$
	DO	P.SCN
	BRA.S	10$

90$
	PULL_L	D0-D3/A0-A1
	RET

      ENDC



	END

