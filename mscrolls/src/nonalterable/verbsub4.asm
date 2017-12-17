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


*****************************************************
*
*	@(#)verbsub4.asm	2.18
*	9/17/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.VERBSUB4
*
*--------------------------------
 
	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"


	XREF    ERRHOLD,ERR1,ERR4,VERB,GLV.L_,W.BTINA,W.NO.TNG,PLURAL
	XREF    CHAROUT,W.FERG,W.BECOF,SUBJECT,TOPNPC,LASTA0,VALID.NP
	XREF    W.ICNDTW,PROC.LAB,YCANT,BRK.VERB,SUBOBJ,D72,FULLSTOP
	XREF    W.ARE,W.IT,W.IS,W.THEM,W.THEY,W.HE,W.SHE,W.HIM,W.HER,W.THE
	XREF    GETNPC,ADJECTIV,LIST.N,ADJ.TBL,INITNUM,W.CNTSEE,W.HERE
	XREF    WASMOD2,NOTRAIL,UTIL.SP,LISTWRD,UTIL.DEL,LINE
	XREF    FMSG,CAPITAL,LISWRD,GETDATA,PRTTXT,W.NT,W.YOU,W.AN,SCANFLAG
	XREF    P.DESC,P.LIST,P.LIST2,P.LOR,P.QN,P.NQN,SP3.FLAG,GETRELN
	XREF    P.NOUN,P.NOUN2,NONLY,P.TN,P.QUANT,P.PRN,P.OBJPRN,ISARE
	XREF    P.PV,P.VQN,P.SUBOB,P.SUB,DERR,STOPBRK,YCANTW,CARRIED
	XREF    P.BOTN,P.XFERGS,P.NO.TNG,P.NOTANY,ISDOFRE,HOLDE,UNHOLDE
	XREF    W.FIND,ROOM,DBSFLG,P.NODO,GETNOUN2,LOOPBUF,L.DIREC
	XREF    MATCHBUF,FIXSYS,W.NEASY,W.CON,P.CONJ,NETWORK
	XREF    UTIL.BS,EXBUFF,SEARCH2,W.FHERE,W.NORCH,VLST.GO2
	XREF    P.SUBV,P.NFND,P.NQEN,SCANSV,DBS,OK,CALCWHT,P.BRAKTS
	XREF    GET_CONT,R.NOUN,R.NOUN2,SPACE,ROOTNUM,P.MSG,P.SCN,P.NPCMSG
	XREF    SETA1,P.CNTO,P.FROM,P.WITH,CONNECT,CONNECT2,SCAN
	XREF    W.NWHERE,W.TSOFT,GETNOUNS,PRONHER,PRONHIM,OLDBUF,SEATED
	XREF    TOPLOC,L.WHSAT,RIDING,W.ON,NOUNBUF,NOUNBUF2,NOUNBUF3
	XREF    INTERROR,W.YCNDTT,W.JUSONE,W.NOTIE,CONJ,NOUN,ADVERB
	XREF    SCREEN,SCORE_,VERB2,ADDNOUN,DNOUNBUF,PRTHEX,CHEAT
 
	XDEF    P.EXITS,CALCNET,CALCNET2,PCOK,MIXD0D5,FIXCHK,SETPRON
	XDEF    IFLIQUID,WHENSAT,SATMSG,ONLYREL,TIETOG,ISTIETOG
	XDEF    SETADN,USE,BUFSHUF,MOVBUF,DOSCORE2
	XDEF    DOSCORE,DO.SCORE,P.NORCH,DO.SCN,ISANYREL

	XDEF	DO.NPCMSG
	XDEF	DO.MSG
 

      IFEQ	THE_PAWN

	XDEF	COUNTEXIT,CanReach,NoCanReach,NoCanReachD2,GetValue,SetValue
	XDEF	MessageFromTable

	XREF	ReachTable,NoReachTable,ValueTable,P.Description

      ENDC

 
*--------------------------------
*
* DO.SCN - PRINTS THE SCN MSG IN NEXT WORD
*          AFTER CALL - USED BY SCN 
*
*--------------------------------
 
DO.SCN
	
	MOVE.L  A2,-(A7)           ;SAVE A2
	MOVE.L  4(A7),A2           ;GET ADDR OF SCN MSG #
	MOVE.L  D1,-(A7)           ;SAVE D1
	MOVE.W  (A2)+,D1           ;GET MSG#
	DO	P.SCN	             ;PRINT MSG
	MOVE.L  (A7)+,D1           ;RESTORE D1
	MOVE.L  (A7)+,A2           ;AND A2
	ADDQ.L  #2,(A7)            ;UPDATE PC
	RET

*--------------------------------
*
* DO.MSG - PRINTS THE PROG MSG IN NEXT WORD
*          AFTER CALL - USED BY MSG 
*
*--------------------------------

DO.MSG
	
	MOVE.L  A2,-(A7)           ;SAVE A2
	MOVE.L  4(A7),A2           ;GET ADDR OF SCN MSG #
	MOVE.L  D1,-(A7)           ;SAVE D1
	MOVE.W  (A2)+,D1           ;GET MSG#
	DO	P.MSG	             ;PRINT MSG
	MOVE.L  (A7)+,D1           ;RESTORE D1
	MOVE.L  (A7)+,A2           ;AND A2
	ADDQ.L  #2,(A7)            ;UPDATE PC
	RET

*--------------------------------
*
* DO.NPCMSG - PRINTS THE NPC MSG IN NEXT WORD
*          AFTER CALL - USED BY NPCMSG 
*
*--------------------------------

 
DO.NPCMSG
	
	MOVE.L  A2,-(A7)           ;SAVE A2
	MOVE.L  4(A7),A2           ;GET ADDR OF SCN MSG #
	MOVE.L  D1,-(A7)           ;SAVE D1
	MOVE.W  (A2)+,D1           ;GET MSG#
	CALL    P.NPCMSG              ;PRINT MSG
	MOVE.L  (A7)+,D1           ;RESTORE D1
	MOVE.L  (A7)+,A2           ;AND A2
	ADDQ.L  #2,(A7)            ;UPDATE PC
	RET
 
P.EXITS
 
	CLR.W   D4
	DO	CALCNET			;A0 POINTS TO CUR ROOM IN NETWORK
	LEA     L.DIREC(A4),A1		;A1 POINTS TO TEXT IN WORDTABLE
	MOVEQ   #DIR_D+1,D0		;Literal directions not in/out
20$
	MOVE.B  (A0)+,D1		;GET A DIREC
	BEQ.S   50$			;ZERO = CAN'T GO THAT WAY


      IFEQ     THE_PAWN

	CMP.B   #MAXROOM,D1
	BCC.S   50$

      ENDC


      IFNE     THE_PAWN

	BMI.S   50$                ;NO WAY - BUT SPECIAL MSG

      ENDC

	ADDQ.B  #1,D4               ;FLAG FOR NO EXITS

10$
	CMP.B   #1,D4		;first dir?
	BEQ.S   14$		;yes, don't print a comma,
	PRINT	<', '>

14$
      IFNE	YCHEAT
	
	TEST_B	CHEAT(A4)
	BEQ.S	15$
	PUSH_L	D0-D7/A0-A6
	PRINT	<'^('>
	MOVEQ	#0,D2
	MOVE.B	D1,D2
	MOVE.W	D2,-(A7)		;save room# to be printed
	DO	R.NOUN2
	DO	P.NOUN
	MOVE.W	(A7)+,D6		;get room#
	CALL	PRTHEX
	PRINT	<') '>
	PULL_L	D0-D7/A0-A6

      ENDC

15$
	MOVE.B  (A1)+,D1            ;PRINT DIR TEXT
	BMI.S   16$                 ;LAST CHAR
	DO	CHAROUT
	BRA.S   15$
16$
	DO	CHAROUT
	BRA.S   30$
50$
	MOVE.B  (A1)+,D1
	BPL.S   50$ 
	ADDQ.L  #1,D1
30$
	SUBQ.B  #1,D0
	BNE.S   20$
	TEST_B  D4
	BNE.S   40$
	MSG     NWHERE          ;D4=0 = NO EXITS
	BRA.S   55$
40$
	MOVEQ   #'.',D1
	DO	CHAROUT
55$
      IFNE	YCHEAT
       
       IFEQ	THE_PAWN
	
	TEST_B	CHEAT(A4)
	BEQ.S	99$
	PUSH_L	D0-D7/A0-A6
	PRINT	<'^in - '>
	MOVEQ	#0,D2
	MOVE.B	(A0)+,D2
	BEQ.S	101$
	CMP.B	#MAXROOM,D2
	BCC.S	101$
	MOVE.L	D2,-(A7)
	DO	R.NOUN2
	MOVE.L	(A7)+,D6
	CALL	PRTHEX
101$
	PRINT	<'^out - '>
	MOVEQ	#0,D2
	MOVE.B	(A0)+,D2
	BEQ.S	102$
	CMP.B	#MAXROOM,D2
	BCC.S	102$
	MOVE.L	D2,-(A7)
	DO	R.NOUN2
	MOVE.L	(A7)+,D6
	CALL	PRTHEX
102$
	PULL_L	D0-D7/A0-A6

       ENDC
      ENDC

99$
	MSGGO     LINE
	
	
	

      IFEQ	THE_PAWN

COUNTEXIT:
*
* This will count the number of exits available from ROOM
* it will return the result in D7 
* A0 will return the pointer into the network
* and d0 will be stomped!
* this shall also test d7 before it returns to inform you 
* if EQ
* and d1 will be stomped!

	DO	CALCNET		;returns A0 pointing to start of network
	MOVE.W	#Sizeof_NET-1,D0	;amount of rooms
	CLR.W	D7
10$
	MOVE.B	0(A0,D0.W),D1   	;is it greater than maximum?
	BEQ.S	20$
	CMP.B	#MAXROOM,D1
	BCC.S	20$			;not an exit
	ADDQ.W	#1,D7			;add one to our counter
20$
	DBRA	D0,10$			;To finish
	TEST_B	D7			;for room count
	RET

      ENDC

*--------------------------------
*
* CALCNET - calculates pointer to network data for current room
*
* on entry: ROOM = room we want to know about
* on exit: A0 -> network info
*          D2 stomped
* 

      IFNE    M68000

CALCNET

	CLR.L   D2
	MOVE.W  ROOM(A4),D2

CALCNET2

	LEA     NETWORK(A4),A0
	SUBQ.W  #1,D2
	MULU    #Sizeof_NET,D2
	ADD.L   D2,A0
	RET

      ENDC

      IFEQ    M68000
 
CALCNET

	MOVE.W  ROOM(A4),D2

CALCNET2

	LEA     NETWORK(A4),A0
	SUBQ.W  #1,D2
	ADD.W   D2,A0
	ADD.W   D2,A0

* consider different network sizes (norm+in/out =12)

       IFNE	Sizeof_NET-10

	ADD.W	D2,A0
	ADD.W	D2,A0

       ENDC

	ASL.W   #3,D2
	ADD.W   D2,A0
	RET

      ENDC

*--------------------------------
  
PCOK

* TEST D5,A5 for Physical Characteristics ok

        ;;  return d1 == 1 => failed, d1 == 0, ok
        ;; ok if (hardness > 3, != 12, != 0 and item not broken)
        
	PUSH_L  D0/A0
	MOVE.B  1(A5),D1
	LSR.B   #4,D1             ;get hardness
	BNE.S   05$
	DO	DBS               ;hardness void
	BRA.S   30$               ;EXIT W ERROR
05$
	CMP.B   #3,D1             ; <=3 => too soft
	BHI.S   10$

	BeSure	M.TSOFT
20$
	MOVE.W	#M.TSOFT,D1          ;IS NOT SUITABLE
25$
	MOVE.W  D5,D0
	GETINFO        
	DO	P.TN
	DO	ISARE
	DO	P.MSG
	DO	FULLSTOP
30$
	MOVEQ   #1,D1
	BRA.S   99$
10$
	CMP.B   #12,D1           ; flexible, also too soft
	BEQ.S   20$
	MOVEQ	#0,D1

	BTST	#2,(A5)		;broken ?
	BEQ.S	99$
	
	BeSure	M.BROKEN
	MOVE.W	#M.BROKEN,D1
	BRA.S	25$

99$
	PULL_L  D0/A0
	RET

*--------------------------------
   
MIXD0D5

	SWAP    D0
	MOVE.W  D5,D0
	RET
 
*--------------------------------
*
*  FIXCHK - SUSSES WHETHER ITEM
*           IS FIXED TO ANYTHING
*
*  PASSED   D0/A0 - NOUN NO. ETC
*
*  RETURNS  ZERO FLAG
*
*  BNE  (NOT SET, Z=0)  FIXED
*  BEQ  (SET, Z=1) NOT FIXED
*
* D1 a noun tied to noun which is D0
* A1 a pointer to a buffer of such nouns (zero terminated)
*
*--------------------------------
 
   
FIXCHK
 
	PUSH_L  D2/D5/A0      ;SAVE SCRATCH
	GETINFO                     ;MAKE *SURE*
	MOVE.W  12(A0),D1           ;FIX DATA
	AND.W   #$3FFF,D1           ;IS THIS TIED (DIRRECTLY)ND
	BNE.S   10$                 ;YES, THEN 10$
	MOVE.W  D0,D2               ;LOOK FOR NOUN NO.
	MOVEQ   #12,D1              ;AT BYTE 12
	MOVEQ   #1,D5               ;&13
	DO	GETNOUNS            ;GO LOOK
	MOVE.W  (A1),D1             ;TEST F TIED (BUFFER)
	BRA.S	90$
10$
	LEA	DNOUNBUF(A4),A1		;fake up a GETNOUNS return
	CLR.W	2(A1)
	MOVE.W	D1,(A1)
90$
	PULL_L  D2/D5/A0
	RET                         ;BACK TO CALLER
  
*--------------------------------
 
*--------------------------------
*
* SETPRON: 
*
* SET HIM/HER PRONOUNS TO BE D0
* 
* WHEN D0 IS A NPC, NO ERROR
* 
* RETURNS, STOMPS A0,A3
*
*--------------------------------
 
SETPRON
 
	GETINFO        
	BTST    #6,4(A0)            ;NPC?
	BEQ.S   20$                 ;0= NOT NPC, SO SET 'IT'
	DO	GETNPC	             ;GET NPC DATA, A3
	BTST    #0,2(A3)            ;1=MALE, 0=FEMALE
	BNE.S   10$
	MOVE.W  D0,PRONHER(A4)      ;SET PRONOUN
	RET
10$
	MOVE.W  D0,PRONHIM(A4)
	RET
20$
	MOVE.W  D0,OLDBUF(A4)       ;IT/THEM BUFFER
	RET
*--------------------------------
	
ISANYREL
	
*ARE ANY NOUNS RELATED SEPARABLY?
	
*ENTRY:
*A1=NOUNS LIST
	
*EXIT:
*EQ =NONE  
*NE = AT LEAST ONE
*D0=ITEM FOUND
	
	PUSH_L  A0-A1
10$
	MOVE.W  (A1)+,D0
	BEQ.S   90$
	BMI.S   10$                 ;NOT REALLY?
	GETINFO                     ;TO A0
	CMP.B   #1,6(A0)
	BNE.S   10$
	SETNE
90$
	PULL_L  A0-A1
	RET
 
 
*--------------------------------
*
*  IFLIQUID - IF DO IS A LIQUID
*             THEN FIND ITS
*             CONTAINER, SET IT AS DO
*             AND A0
*
* NEEDS D0/A0 ON ENTRY
*
*--------------------------------
 
IFLIQUID
 
	MOVE.L  D1,-(A7)
	MOVE.B  1(A0),D1           ;GET PHYS CHAR
	LSR.B   #4,D1              ;ONY WANT HARDNESS
	CMP.B   #1,D1              ;LIQUID?
	BNE.S   90$                ;NOT = FORGET
	BTST    #5,6(A0)           ;CONTAINED?
	BEQ.S   90$                ;NOPE = DON'T GET CONTAINER!!
	MOVE.W  8(A0),D0
	GETINFO        
90$
	MOVE.L  (A7)+,D1
	RET


*--------------------------------
*
*  SRC.ADV.VERBSUB5
*
*--------------------------------
 
 
WHENSAT
	
*CHECKS FOR CERTAIN VERBS ONLY WHEN SEATED
*USES D1
*EXIT:
*EQ = OK
*NE = CAN'T REACH IT FROM HERE.
	
	TEST_L  SUBJECT(A4)         ;U?
	BEQ.S   01$                 ;OK
	CLR.W   D7                  ;NO ERROR 
	RET
	
01$
	PUSH_L  D0/D2/D5/A0-A1/A5
	GETINFO        
	LEA     SEATED(A4),A1
	CMP.W   (A1),D0             ;SAME NOUN?
	BEQ.S   99$                 ;OK
	MOVE.W  (A1),D1             ;SEATED NOUN
	MOVE.L  A0,A5
	MOVE.W  D0,D5
03$
	MOVE.B   6(A0),D2 
	AND.B   #$33,D2             ;MASK FOR CONTINUE
	BEQ.S   20$                 ;NO, CHK VERB INCASE
	CMP.B   #1,6(A0)            ;SIMPLY RELATED?
	BNE.S   02$                 ;NO, TRY NEXT?
	CMP.W   8(A0),D1            ;TO THIS ITEM?
	BEQ.S   99$                 ;YES, OK
02$
	MOVE.W  8(A0),D0
	GETINFO        
	BRA.S   03$
	
20$
	MOVE.L  A5,A0
	MOVE.W  D5,D0                ;NOVE BAK
	BTST    #0,5(A0)             ;PSEUDO?
	BNE.S   90$                  ;NO-GO
	DO	TOPLOC
	CMP.W   SUBJECT(A4),D0       ;ON INVENT?
	BEQ.S   99$                  ;OK
	LEA     L.WHSAT(A4),A0
	TEST_W  RIDING(A4)           ;IF RIDING?
	BEQ.S   10$
	LEA     VLST.GO2(A4),A0
	CLR.W   D0
10$
	MOVE.B  (A0)+,D0
	CMP.B   #-1,D0
	BEQ.S   90$                  ;END
	CMP.W   VERB(A4),D0          ;IN LIST?
	BNE.S   10$                  ;NO, NEXT
99$
	PULL_L  D0/D2/D5/A0-A1/A5
	RET                          ;ITS OK
90$
	SETNE
	BRA.S   99$                  ;EXIT
 
*--------------------------------
	        
	
SATMSG
	        
*PRINTS OUT SITTING ON X...
*WHEN SAT
	
	TEST_W  SEATED(A4)
	BEQ.S   90$                  ;NO
	PUSH_L  D0-D1/A0
	MOVE.W  SEATED(A4),D0
	MOVEQ   #'(',D1
	DO	CHAROUT
	MSG     ON
	DO	P.TN
	DO	UTIL.BS
	MOVEQ   #')',D1
	DO	CHAROUT
	PULL_L  D0-D1/A0
90$
	MSGGO     LINE
  
*--------------------------------
	
	
ONLYREL
	
*REMOVE NOUNS FROM LIST NOT JUST
*SIMPLY RELATED
	
*ENTRY:
*A1=START OF LIST
	
	PUSH_L  D0/A0-A1
10$
	MOVE.W  (A1)+,D0            ;GET ONE
	BEQ.S   90$                 ;DONE
	GETINFO                     ;ON D0
	BTST    #0,5(A0)            ;PSUDO?
	BNE.S   20$                 ;NOPE
	CMP.B   #1,6(A0)            ;REL SIMPLE?
	BEQ.S   10$                 ;OLK
20$
	OR.W    #$8000,-2(A1)       ;SET HI
	BRA.S   10$                 ;NEXT
90$
	PULL_L  D0/A0-A1
	RET
 
*--------------------------------
	
	
TIETOG
	
*PROCESS FOR TIE TOGETHER
	
	CLR.W   D7                  ;ERROR FLAG
	LEA     NOUNBUF(A4),A1      ;NOUNS
	CLR.W   D4                  ;NON TIEABLE COUNTER
	CLR.W   D2                  ;NOUN COUNTER
	MOVE.W  (A1),D5             ;SET INITALLY
10$
	MOVE.W  (A1)+,D0            ;GET A NOUN
	BEQ.S   50$                 ;ALL DNE
	ADDQ.W  #1,D2               ;INC
	GETINFO                     ;TO A0
	MOVE.B  1(A0),D1            ;HARD&STRENGTH
	AND.B   #$F0,D1             ;HARDNESS
	CMP.B   #$C0,D1             ;FLEXI?
	BEQ.S   10$                 ;YES, NEXT
	ADDQ.W  #1,D4               ;INC NON-FLEXY
	MOVE.W  D0,D5               ;SAVE
	BRA.S   10$                 ;UNTIL ALL
50$
	CMP.B   #1,D2               ;HOW MANY?
	BLT     INTERROR            ;HUH?
	BNE.S   60$                 ;OK
	MSG     YCNDTT
	MSG     JUSONE
	BRA     80$                 ;FAIL ,EXIT
60$
	CMP.B   #1,D4               ;HOW MANY?
	BLS.S   61$                 ;OK
	TEST_W  NOUNBUF2(A4)        ;ANOTHER?
	BNE.S   61$                 ;HUMM, LET TIE SORT IT OUT! 
	MSG     NOTIE
	BRA.S   80$                 ;FAIL,EXIT
61$
	LEA     NOUNBUF(A4),A1      ;START AGAIN/.
62$
	MOVE.W  (A1)+,D0            ;GET A NOUN
	BEQ     INTERROR            ;BUT I THOUGHT THERE WAS A MATCH???
	CMP.W   D0,D5               ;FOUND IT YET?
	BNE.S   62$                 ;NO
63$
	MOVE.W  (A1),-(A1)          ;COLLAPSE BUFFER 
	BEQ.S   64$                 ;DONE?
	ADDQ.L  #4,A1               ;NXT
	BRA.S   63$
64$
	LEA     NOUNBUF2(A4),A0
	LEA     NOUNBUF3(A4),A1
	CALL    MOVBUF              ;INTO 3
	
	LEA     NOUNBUF2(A4),A1     ;SECOND BUFFER
70$
	EXT.L   D5                  ;ASSUME NO HI BIT.
	SWAP    D5                  ;ZEROS LOW
	MOVE.L  D5,(A1)             ;INTO
	MOVE.L  #$00010000,CONJ(A4) ;FUDGE IN TO
	SUBQ.W  #2,NOUN(A4)
90$
	RET
80$ 
	MOVEQ   #1,D7   
	BRA.S   90$
  
*--------------------------------
	
	
ISTIETOG
	
	CMP.W   #VNTIE,VERB(A4)
	BEQ.S   10$                ;OK
	CMP.W   #VNFIX,VERB(A4)
	BNE.S   90$                ;NO EXIT
10$
	CMP.W   #TOGETHER,ADVERB(A4)    ;TOGETHER?
	BEQ.S   20$                ;NO EXIT
	CMP.W   #TOGETHER,CONJ(A4)
	BNE.S   90$                ;FAIL
20$
	CALL    TIETOG             ;SHUFFLE BUFFERS
	TEST_W  D7                 ;ERROR?
	BNE.S   80$                ;FAIL  
	BRA.S   SETADN
90$
	RET
80$
	MOVE.L  (A7)+,D0  DUMP STACK
	DOGO	SCREEN
*--------------------------------
	
	
SETADN
	MOVE.W  NOUNBUF2(A4),ADDNOUN(A4)
	MOVE.W  NOUNBUF3(A4),ADDNOUN+2(A4) 
	RET
	
*--------------------------------
	
USE
	CALL_S  BUFSHUF               ;SHUFFLE THEM THERE BUFFERS (DEALER)  
	MOVE.W  CONJ+2(A4),CONJ(A4)
	BPL.S   20$                   ;YES
	CLR.W   CONJ(A4)
	BRA.S   21$
20$
	CLR.W   CONJ+2(A4)
21$
	MOVE.W  VERB2(A4),VERB(A4)
	CMP.W   #-1,ADVERB(A4)        ;ANY?
	BNE.S   10$                   ;YEP
	MOVE.W  CONJ+2(A4),ADVERB(A4)
10$
	
	BRA.S   SETADN
      
*--------------------------------
	
BUFSHUF
	
	LEA     NOUNBUF(A4),A0     ;SOURCE
	LEA     EXBUFF(A4),A1      ;DEST
	CALL_S  MOVBUF             ;MOVEIT
	LEA     NOUNBUF2(A4),A0
	CALL_S  MOVBUF             ;MOVE
	LEA     NOUNBUF3(A4),A0
	TEST_W  (A0)               ;ANY?
	BEQ.S   50$                ;ONLY 2 BUFFS
	CALL_S  MOVBUF             ;MOVE
50$
	LEA     EXBUFF(A4),A0      ;SOURCE
     
MOVBUF
	MOVE.L  A0,-(A7)
91$
	MOVE.W  (A0)+,(A1)+
	BNE.S   91$                ;UMOVE IT
	MOVE.L  (A7)+,A1           ;BAK
	RET
 
 
*--------------------------------
*
* DOSCORE2 - CALL WITH A2 = &SC.XYZ
*             STOMPS D1
*

* WARNING!!!! These  are now alink compatible !!!
  
DOSCORE2
 
	MOVE.B  (A2),D1
	EXT.W   D1
	ADD.W   D1,SCORE_(A4)
	CLR.B   (A2)         ;SO IT ONLY HAPPENS ONCE
	RET
*
* DOSCORE - CALL WITH NEXT WORD IN PROG = ADDR OF SCORE_ BYTE
*
*      CALL    DOSCORE
*      SCORE_  FRED
*
 
DO.SCORE 
DOSCORE
	
	MOVE.L  A2,-(A7)           ;SAVE A2
	MOVE.L  4(A7),A2           ;GET ADDR OF SCORE_ OFFSET
	PUSH_L  D0/D1
	MOVE.W  (A2),D0		;PC relative (alink!!)
	EXT.L   D0
      
      IFEQ	THE_PAWN
	XREF	ScoreList
	LEA	ScoreList(A4),A2	;so we avoid pc-rel range hassles
      ENDC

	ADD.L   D0,A2
	CALL_S  DOSCORE2
	PULL_L  D0/D1
	MOVE.L  (A7)+,A2
	ADDQ.L  #2,(A7)            ;UPDATE PC
	RET

*--------------------------------
	        
P.NORCH
	        
	DO	P.SUB
	MSG     NORCH   ;CANT REACH FROM HERE
	DO	P.TN
	MSG	FHERE
	MSG	LINE		
	BRA	SETPRON		;setup pronouns as used in error msg

      IFEQ	THE_PAWN


****************************************************************
*
* CanReach - susses if u can reach this object while seated 
* 
* On entry: D0 = noun# in question
*           SEATED = noun# of object u'r sitting on
*
* uses ReachTable in scenrw.asm
*
* format is:    object,seat
*               long word -1 terminated
*
* In exit: EQ => can reach it
*

CanReach

	PUSH_L	D1/A0

	LEA	ReachTable(A4),A0
10$
	MOVE.L	(A0)+,D1
	BMI.S	90$

	TEST_W	D1			;MI => is a verb#
	BPL.S	20$
	CMP.B	VERB+1(A4),D1		;to allow stuff like 'feed bear'
	BEQ.S	30$
	BRA.S	10$
20$
	CMP.W	SEATED(A4),D1		;bottom word is object we're on?
	BNE.S	10$
30$
	SWAP	D1
	CMP.W	D0,D1
	BNE.S	10$

90$
	PULL_L	D1/A0
	RET

****************************************************************
*
* NoCanReachD2 - checks if this verb doesn't want to let D2 go
*                by without checking reach/npc ownership
*
* takes D2 = noun# of indirect object
*
* returns NE => fail
*

	XREF	VLIST.NOREACHD5,VERBOWN,CHK.OWN

NoCanReachD2

	PUSH_L	D0/A0
	LEA	VLIST.NOREACHD5(A4),A0
10$
	MOVE.B	(A0)+,D0
	CMP.B	#-1,D0
	BEQ.S	90$			;EQ => end of list
	CMP.B	VERB+1(A4),D0
	BNE.S	10$

	MOVE.W	D2,D0

	CALL_S	NoCanReach
	BEQ.S	20$			;NE => fail, can't reach it
	CALL	CanReach
	BEQ.S	20$
	CALL	P.NORCH
	SETNE
	BRA.S	90$
20$
	CALL	VERBOWN
	BEQ.S	90$
	GETINFO				;waaa - CHK.OWN is takky
	CALL	CHK.OWN
	TEST_B	D7
90$
	PULL_L	D0/A0
	RET


****************************************************************
*
* NoCanReach - susses if u cannot reach this object
* 
* On entry: D0 = noun# in question
*           VERB = verb in use
*
* uses NoReachTable in scenrw.asm
*
* format is:    verb,object
*               long word ZERO terminated
*
* In exit: EQ => can reach it
*

NoCanReach

	CALL	TranspCont		;returns NE => cannot get at obj
	BNE.S	99$			;with current verb

	PUSH_L	D1/D2/A0
	LEA	NoReachTable(A4),A0	;get table of objects not normally
	MOVEQ	#0,D2			;reachable
10$
	MOVE.L	(A0)+,D1		;table format DC.W verb#,noun#
	BEQ.S	90$

	CMP.W	D0,D1			;check noun#
	BNE.S	10$

	MOVEQ	#-1,D2			;return code -1 => cannot reach

	SWAP	D1

	CMP.W	VERB(A4),D1		;bottom word is object we're on?
	BNE.S	10$
	MOVEQ	#0,D2			;verb is allowed - can reach
90$
	TEST_B	D2
	PULL_L	D1/D2/A0
99$
	RET

*************************************************
*
* TranspCont - returns NE is obj (D0) is in a CLOSED
*              transp. container and the verb can't
*              get into it.
*
* Uses TRANS_VLIST and things

	XREF	TRANS_VLIST

TranspCont

	PUSH_L	D0-D1/A0-A1

	LEA	TRANS_VLIST(A4),A1	;list of allowed verbs
10$
	MOVE.B	(A1)+,D1
	CMP.B	#-1,D1			;-1 term cos look is in list!
	BEQ.S	20$			;not in list so must see if ok
	CMP.B	VERB+1(A4),D1		;is the verb in our list?
	BNE.S	10$			;not yet, try another
	BRA.S	90$			;exit ok
20$
	GETINFO
	BTST	#0,5(A0)		;pseudos can't be contained
	BNE.S	80$
	MOVE.B	6(A0),D1
	AND.B	#$23,D1			;is it contained/hidden/related?
	BEQ.S	90$			;nope, not a pointed noun
	BTST	#5,6(A0)		;contained?
	BNE.S	30$			;NE => yes, better consider if its
					;closed & transp.
	MOVE.W	8(A0),D0
	BEQ.S	90$			;just in case!
	GETINFO
	BRA.S	20$
30$
	MOVE.W	8(A0),D0
	BEQ.S	90$			;just in case!
	GETINFO
	BTST	#5,(A0)			;closed?
	BNE.S	40$
	BTST	#6,(A0)			;locked?
	BEQ.S	20$			;neither locked nor closed, loop up

40$
	BTST	#3,(A0)			;transp container?
	BNE.S	90$			;yes - bad exit!
	BRA.S	20$			;no, keep going - may be some later
	
80$
	SETEQ
90$
	PULL_L	D0-D1/A0-A1
	RET



      ENDC 

      IFEQ	THE_PAWN

*********************************************
*
* Bits dealing with the value of objects
*
*
* GetValue - returns the value of D0 in D1.B,
*            also Z flag set according to D1.B
*
* rest of D1 is undefined
*
* format of ValueTable is DC.W value,noun# - long word zero terminated

GetValue

	PUSH_L	A0/D2
	MOVEQ	#0,D1			;zap incase no entry found
	LEA	ValueTable(A4),A0
10$
	MOVE.L	(A0)+,D2
	BEQ.S	90$			;EQ => no value
	CMP.W	D0,D2
	BNE.S	10$
	SWAP	D2
	MOVE.B	D2,D1
90$
	PULL_L	A0/D2
	RET


*
* SetValue - sets the value of object D5 to D1.B (sets Z on D1.B on exit)
*

SetValue

	PUSH_L	A0/D2
	LEA	ValueTable(A4),A0
10$
	MOVE.L	(A0)+,D2
	BEQ.S	90$		;EQ => no value - cannot set - call it null
	CMP.W	D5,D2
	BNE.S	10$
	MOVE.B	D1,-3(A0)
90$
	PULL_L	A0/D2
	RET


************************************************************
* 
* This routine scans a table (pointed to by A1) for the first
* match with noun D0. Three different table entries are allowed:
*
* a) Print desc. of noun
*	DC.W	NNOUN+HI
*
* b) Print Scenario message
*	DC.W	NNOUN,SCNMSG
*
* c) Print desc. of other noun
*	DC.W	NNOUN,NOTHERNOUN+HI
*
* A word of zero terminates the list.
*
* Roddy - Jan,'87
* 
*********************************************************

MessageFromTable:

	PUSH_L	D0-D1/A1
10$:
	MOVE.W	(A1)+,D1	; Get first table word?
	BEQ.S	90$		; end of table ? - only jump to 90$ with EQ
	ANDI.W	#$7FFF,D1	; Strip top bit...
	CMP.W	D1,D0		; does it match the one we want?
	BEQ.S	20$		
	TEST_W	-2(A1)		; Nope - do we need to skip another word?
	BMI.S	10$		; If not, go and try next word.
	ADDQ.W	#2,A1		; Yes, skip extension word...
	BRA.S	10$		; ..and try next match

20$:
	TEST_W	-2(A1)		; We've got a match, so was top bit set?
	BMI.S	70$		
	MOVE.W	(A1)+,D1	; No, so get extension wd.
	BMI.S	30$		; Was ITS top bit set? (i.e. noun desc)
	DO	P.SCN		; No,so treat it as a scenario message
	BRA.S	80$		; ... and exit
30$:
	AND.W	#$7FFF,D1	; Its a noun number, so clear top bit
70$:	
	MOVE.W	D1,D0		; put it in the right place...
	CALL	P.Description	; and print its description
80$:
	SETNE
90$:
*	SETEQ			; GUARANTEED EQ when we get here via 90$
				; ... or else we're in the brown stuff.
	PULL_L	D0-D1/A1
	RET

      ENDC

	END

