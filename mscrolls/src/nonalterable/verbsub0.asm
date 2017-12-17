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
*	@(#)verbsub0.asm	2.17
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*     SRC.ADV.VERBSUB0
* 
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"

      IFNE     Graphics
	 XREF    PICTURES
      ENDC


	 XREF    ERRHOLD,ERR1,ERR4,VERB,GLV.L_,W.BTINA,W.NO.TNG,PLURAL,BSFULLSTOP
	 XREF    CHAROUT,SPACE,W.FERG,W.BECOF,SUBJECT,TOPNPC,LASTA0,VALID.NP
	 XREF    P.SCN,W.ICNDTW,PROC.LAB,BRK.VERB,SUBOBJ,D72,FULLSTOP
	 XREF    W.ARE,W.IT,W.IS,W.THEM,W.THEY,W.HE,W.SHE,W.HIM,W.HER,W.THE
	 XREF    GETNPC,P.MSG,ADJECTIV,LIST.N,ADJ.TBL,ROOTNUM,INITNUM
	 XREF    WASMOD2,SETPRON,NOTRAIL,UTIL.SP,LISTWRD,UTIL.DEL,LINE
	 XREF    FMSG,CAPITAL,LISWRD,GETDATA,PRTTXT,W.NT,W.YOU,W.AN
	 XREF	PRTNUM,STOPRON,NOTRAIL2,UTIL.BS

	 XDEF    P.Description,P.DESC,P.LIST,P.LIST2,P.LOR,P.QN,P.NQN
	 XDEF    P.NOUN,P.NOUN2,NONLY,P.TN,P.QUANT,P.PRN,P.OBJPRN,ISARE
	 XDEF    P.PV,P.VQN,P.SUBOB,P.SUB,DERR,STOPBRK,YCANTW,YCANT,CARRIED
	 XDEF    P.BOTN,P.XFERGS,P.NO.TNG,P.NOTANY,ISDOFRE,HOLDE,UNHOLDE
	XDEF	YCANTNOW,YCANTWSTOP

	 IFNE      ABSOLUTELY_THE_LAST_LINK

	  XDEF    CHK_FRIG
	  XREF    CHK.FRIG


CHK_FRIG    BRA      CHK.FRIG


	 ENDC


***************************************************************************
*
*  FirstGlance
* 
* This bit of code will append a description onto a look the first time that
* the object is looked at.  If the Initial State bit is EQ and the noun 
* number is in the table then it will print the description and fod the bit
* 
* When it arrives here A0 points at the noun data
*                      D0 contains the noun number
*
* A2/D1 are used
*
***************************************************************************

      IFEQ	THE_PAWN

	XREF	FirstGlanceTable
	XDEF	FirstGlance

	include	"nounbits.asm"

FirstGlance

	LEA	FirstGlanceTable(A4),A2		

* This Table is:
* SCN  # (.W)
* NOUN # (.W)
* ZERO terminated

10$
	MOVE.L	(A2)+,D1
	BEQ.S	99$
	CMP.W	D0,D1			;does noun number correspond?
	BNE.S	10$
	SWAP	D1			;Put Scenario number in low word
	BCLR	#15,D1			;if hi bit of msg# then ALWAYS use
	BNE.S	80$
	BSET	#INIT_BIT,SPARE_BYTE(A0)	;Fod it once only!
	BNE.S	99$				;NE => been done once
80$
	DO	P.SCN
99$
	RET

      ENDC
	



*********************************************************************
*
*  P.Description - This routine checks that the noun in D0 prints
*  	           the noun description by looking it up in the 
*		   table (OtherDescTable) if it is there it prints
*	           the corresponding scenario msg.  Else it calls
*                  the routine P.DESC
*		   The table contains info as:
*	           #Scn #Noun Zero Terminated.
*
*********************************************************************

      IFEQ	THE_PAWN

	XREF	OtherDescTable
	XDEF	OtherDescription

OtherDescription

	PUSH_L	A1/D1	
	LEA	OtherDescTable(A4),A1	;The table containing Noun Numbers.
10$
	MOVE.L	(A1)+,D1		
	BEQ.S	90$			;Zero Terminated
	CMP.W	D0,D1			;Is it our noun?
	BNE.S	10$

* Okay guys we have one now print the scen message of it.

	SWAP	D1			;get Scen number in low word
	DO	P.SCN

	SETNE
	
90$
	PULL_L	D1/A1
	RET

      ENDC

P.Description

      IFEQ	THE_PAWN	;The Pawn does not know about different descs

	CALL	OtherDescription
	BEQ.S	10$			;kill rest
	RET
10$

      ENDC	


***********************************************************************
***********************************************************************
* 
*  WARNING - Do not seperate these routines.  
*
***********************************************************************
***********************************************************************

*--------------------------------
*
* P.DESC - PRINTS THE DESC. OF
* THE NOUN IN D0. IF A ROOM IT 
* PUTS A <CR AFTER THE NOUN NAME
*
*--------------------------------
 
P.DESC
 
	PUSH_L  D1/A0
	GETINFO        
	BTST    #6,6(A0)           ;ROOM?
	BEQ.S   05$

      IFNE     Graphics
	CALL    PICTURES           ;Suss if we should display a picture
      ENDC

	ST      CAPITAL(A4)        ;FORCE CAPS FOR ROOM NAME/ADJ
	DO	P.NOUN
	MSG	LINE

      IFND	WindUp
	BRA.S   10$
      ENDC
05$
      IFND	WindUp
      IFEQ	THE_PAWN
	BTST	#1,4(A0)		;suppress 'the' xyzzy?
	BNE.S	10$			;NE => yes
      ENDC

	DO	P.TN
      ENDC
10$
	MOVE.W	D0,STOPRON(A4)
	CALL    FMSG

     IFEQ	THE_PAWN
	XREF	UntilDisturbed
	CALL	FirstGlance
	CALL	UntilDisturbed
     ENDC

	DO	FULLSTOP
	PULL_L  D1/A0
	RET
  
*--------------------------------
*
*  P.LIST
*
* PRINT LIST OF NOUNS POINTED TO
* BY A0, HIBIT SET = IGNORE
* ZERO TERMINATES
* 
* P.LIST2 ALLOWS LIST TO USE
* 'THE' RATHER THAN 'A'/'AN'/'SOME'
* (D5<0 = USE THE) 
*
* STOMPS D5, SAVES ALL REST
*
*--------------------------------
 
  
P.LIST
  
	MOVEQ   #0,D5              ;FLAG TO USE QUANTIFIERS

	BeSure	M.AND

	MOVE.W  #M.AND,LISWRD(A4)
 
P.LIST2
 
	CALL_S  P.LISUB
	TEST_B	NOTRAIL2(A4)
	BEQ	SPACE
	RET
 
P.LOR
	
	ST      D5                 ;WANT 'THE'

	BeSure	M.OR

	MOVE.W  #M.OR,LISWRD(A4)
  
	
  
P.LISUB
  
	ST      NOTRAIL(A4)        ;LOSE P.NOUN'S TRAILING SPACES
 
	PUSH_L  D0-D7/A0-A6
	MOVEQ   #0,D7
10$
	MOVE.W  (A0)+,D0           ;HI BIT SET = IGNORE
	BMI.S   10$
	MOVE.W  D0,D1              ;SAVE
	BEQ.S   80$                ;NOTHING
	MOVE.L  A0,A1              ;SAVE
11$
	MOVE.W  (A0)+,D0
	BMI.S   11$
 
	MOVE.L  A1,A0
	EXG     D1,D0              ;BACK
	TEST_W  D1
	BEQ.S   70$                ;END
	CALL_S  20$                ;PRINT 'QUANT NOUN' OR 'THE NOUN'
	PRINT   <', '> 
	ADDQ.W  #1,D7   
	BRA.S   10$
70$
	TEST_W  D7                 ;NONE (OR 64K!!)
	BEQ.S   75$
	CALL    UTIL.DEL           ;STOMP LAST COMMA AND SPACE
	CALL    UTIL.DEL           ;BY MOVING LEFT TWICE
	CALL    UTIL.SP            ;FORCE A SPACE
	MOVE.W  LISWRD(A4),D1
	DO	P.MSG
75$
	CALL_S  20$                ;DO LAST NOUN
	BRA.S   99$
80$
	MSG     NT
	TEST_B	NOTRAIL2(A4)
	BEQ.S	99$
	DO	UTIL.BS
99$
	PULL_L  D0-D7/A0-A6
	CLR.B   NOTRAIL(A4)        ;INCASE FINISHED WI A 'NOTHING'
	RET
20$
	TEST_B  D5                 ;SUBROUTINE TO DECIDE WHETHER TO
	BNE     P.TN               ;PRINT 'QUANT NOUN' (D5=0)
;      BRA.S   P.QN               ;OR 'THE NOUN' (D5<=0)
; Fall through...  
*--------------------------------
P.QN

	BeSure	M.SOME
  
	MOVE.W	#M.SOME,D3
;      BRA	P.QN.SUB
; Fall through....
*--------------------------------
  
P.QN.SUB
  
	MOVE.L  D3,-(A7)
	BSET    #15,D3
	CALL_S	P.NOUN2
	MOVE.L  (A7)+,D3
	RET
 
*--------------------------------
  
P.NQN
  
	BeSure	M.ANY

	MOVE.W  #M.ANY,D3
	BRA	P.QN.SUB
 
*--------------------------------
  
P.NOUN
 
	CLR.W   D3
   
P.NOUN2
   
	TEST_W  D0                 ;PRINT NOUN #0?
	BNE.S   05$

      IFNE    YCHEATMORE
	PRINT   <'WHOOPS NOUN # ZERO!!^'>
      ENDC

	RET

05$
	PUSH_L  D0-D7/A0-A1/A3/A5
	GETINFO        
	BTST    #6,4(A0)           ;NPC?
	BEQ.S   10$                ;NO - DON'T SET 'IT'
	CALL    SETPRON            ;ALSO FIXES A0,A3 (IF REQUIRED)
10$
	MOVE.L  A0,A5
	MOVE.B  (A0),D5
	MOVE.B  WASMOD2(A4),D4
	BTST    #4,5(A0)           ;FORCE ADJ?
	BNE.S   NONLY
	MOVE.B  INITNUM(A4),D4     ;NO. IN LOCALITY
	BTST    #7,5(A0)           ;CONSIDER IF 1 IN 1 LOC POS?
	BEQ.S   NONLY              ;NOPE - NOT POS   
	CALL    ROOTNUM            ;CALC NO. IN LOC - D4
   
*  BE CAREFUL HERE
*  DO NOT CALL NONLY UNLESS YOU PRESET THE STACK
*  IT IS NOT A SELF-CONTAINED ROUTINE.
*  
*  DO NOT CHANGE THE STACK LEVEL IN THIS ROUTINE
*   

NONLY
	LEA     ADJ.TBL(A4),A1
	LEA     LIST.N(A4),A0      ;GET TAB
	SUBQ.W  #1,D0              ;DEC NOUN#
	BEQ.S   10$                ;FIRST

      IFNE   FastBits 

	DC.W    $A0FC

      ENDC

      IFEQ    FastBits

15$
	MOVE.B  (A0)+,D1           ;GET CHAR
	BPL.S   15$                ;UNTIL END 
16$
	MOVE.B  (A1)+,D1           ;COUNT ADJS
	BNE.S   16$                ;ADJ LIST STILL 00 TERM
 
	SUBQ.W  #1,D0              ;DEC NOUN#
	BNE.S   15$                ;UNTIL CORRECT POS

      ENDC

10$
	MOVE.B  (A1)+,D1           ;GET ADJ
	BNE.S   20$                ;OK NON 0 = WE HAVE ONE
11$
	BTST    #15,D3
	BEQ.S   31$
	MOVE.B  (A0),D1
	CALL    P.QUANT
	BRA.S   31$
20$
	CMP.B   #1,D4 		   ;do we need to print adj?
	BLS.S   11$		   ; LS => nope
	SUBQ.B  #3,D1              ;FOR CONTROL DISP.
	LEA     ADJECTIV(A4),A1
	MOVEQ   #0,D2
	MOVE.B  D1,D2              ;D2=AD
	SUBQ.B  #1,D2      
	BEQ.S   27$                ;FIRST?

      IFNE    FastBits

	DC.W    $A0FB              ;word table scanner

      ENDC


      IFEQ   FastBits 
25$
	MOVE.B  (A1)+,D1           ;GET CHAR
	BPL.S   25$                ;UNTIL END OF WRD  
	SUBQ.B  #1,D2              ;DEC COUNTER
	BNE.S   25$

      ENDC


27$
	BTST    #15,D3
	BEQ.S   28$
	MOVE.B  (A1),D1
*	BTST    #6,4(A5)
*	BEQ.S   29$
*	CMP.B   #1,D4
*	BHI.S   28$
29$
	CALL    P.QUANT
28$
	MOVE.B  (A1)+,D1           ;GET CHAR
	CALL	CHAROUT			;is a CALL not a DO for speed
	TEST_B  -1(A1)
	BPL.S   28$                ;PRINT WRD

	CALL    SPACE			;is a CALL not a MSG for topline!
31$
	MOVE.B  (A0)+,D1   
	CALL	CHAROUT			;CALL for SPEED
	TEST_B  -1(A0)
	BPL.S   31$
90$
	PULL_L  D0-D7/A0/A1/A3/A5
	TEST_B	NOTRAIL2(A4)		;insist lose space?
	BNE.S	99$			;NE => yes
	TEST_B  NOTRAIL(A4)        ;LOSE TRAILING SPACE?
	BEQ     SPACE              ;NOPE - SO PRINT ONE
99$
	RET
 
 
*--------------------------------
  
P.TN
	PUSH_L  D1/D3/A0/A3
	GETINFO        

      IFEQ	THE_PAWN
	BTST	#6,6(A0)		;is it a room?
	BEQ.S	05$
	BTST	#7,10(A0)		;rooms cannot be clothing
					;so see if article killed
	BNE.S	20$			;NE => it is, loose 'the'
					;cos 'the Upon Westminster Bridge'
      ENDC				;looks really silly!

05$
	BTST    #6,4(A0)                ; NPC?
	BEQ.S   10$
	CALL	GETNPC	   		;CALL for SPEED
	BTST    #3,2(A3)           ;MISS ARTICLE?
	BNE.S   20$
10$
	MSG     THE
20$
	CALL	P.NOUN			;call for speed
	PULL_L   D1/D3/A0/A3
	RET
 
*--------------------------------
*
* warning!!! noun data is in A5 here!
    
P.QUANT

      IFEQ	THE_PAWN

	BTST	#6,6(A5)		;room?
	BEQ.S	05$
	BTST	#7,10(A5)		;kill article
	BNE.S	10$			;loose quant

      ENDC
05$ 
	BTST    #6,4(A5)           ;NPC?
	BEQ.S   20$
	BTST    #3,2(A3)           ;SUPPRESS ARTICLE?
	BEQ.S   20$
10$
	RET
20$
 
	BTST    #0,(A5)            ;PLURAL ?
	BNE.S   50$                ;YES PRINT SOME/ANY
	MOVE.B  1(A5),D5           ;LIQUID ?
	LSR.B   #4,D5
	CMP.B   #1,D5
	BEQ.S   50$                ;YES PRINT SOME/ANY
	BTST    #5,5(A5)           ;FORCED SOME/ANY
	BNE.S   50$                ;YES - THEN 50$
	AND.B   #$5F,D1            ;FIX CASE
	CMP.B   #'A',D1            ;STARTS WITH A VOWEL ?
	BEQ.S   30$                ;YES - PRINT AN...
	CMP.B   #'E',D1            ;REST OF VOWELS
	BEQ.S   30$
	CMP.B   #'I',D1
	BEQ.S   30$
	CMP.B   #'O',D1
	BEQ.S   30$
	CMP.B   #'U',D1
	BEQ.S   30$
	PRINT   <'a '>
	RET
30$
	MSGGO	AN
50$
	MOVE.W  D3,D1
	BCLR    #15,D1
	DOGO	P.MSG
 
*--------------------------------
 
P.PRN
 
	CLR.W   D5
	BRA     PRNSUB
 
 
P.OBJPRN
 
	MOVE.W  D5,-(A7)
	MOVEQ   #6,D5               ;DISP
	CALL    PRNSUB
	MOVE.W  (A7)+,D5
	RET
 
*--------------------------------
 
PRNSUB
 
	PUSH_L  A0/A3
	BTST    #0,(A0)            ;PL
	BNE.S   70$                ;YES
	BTST    #6,4(A0)           ;ALIVE?
	BNE.S   50$                ;YES
	MSG     IT
	BRA.S   90$
50$
	DO	GETNPC	            ;TO A3
	BTST    #0,2(A3)           ;MALE OR FEMALE
	BNE.S   60$                ;MALE
	TEST_B	D5
	BNE.S	55$
	MSG     SHE
	BRA.S   90$
55$
	MSG     HER
	BRA.S   90$
60$
	TEST_B	D5			;flag NE => object prons
	BNE.S	65$
	MSG     HE
	BRA.S   90$
65$
	MSG     HIM
	BRA.S   90$
70$
	TEST_B	D5
	BNE.S	75$
	MSG     THEY
	BRA.S   90$
75$
	MSG     THEM
90$
	PULL_L   A0/A3
	RET

*--------------------------------
  
P.PV
 
	DO	P.PRN
;      DOGO	ISARE
; Fall through...  
*--------------------------------

ISARE
 
* PRINTS "IS " OR "ARE "
* ON DATA AT A0
  
	BTST    #0,(A0)            ;PLURAL
	BEQ.S	10$
	MSGGO	ARE              ;YES
10$
	MSGGO	IS 
 
*--------------------------------
  
P.VQN
 
	CALL_S  ISARE
	DOGO	P.QN

*--------------------------------
*
*     SRC.ADV.VERBSUB1    
* 
*--------------------------------
	
P.SUBOB
	
*PRINTS OBJECTIVE PRONOUN
*THIS ROUTINE COULD BE MADE SMALLER
*BY MAKING IT PATCH P.SUB
*HOWEVER...
	
	ST      SUBOBJ(A4)
	DO	P.SUB
	CLR.W   SUBOBJ(A4)
	RET
	
	
*--------------------------------
  
P.SUB
  
	TEST_W  SUBJECT(A4)        ;ZERO= 'PLAYER'
	BEQ.S   10$
	MOVE.L  D0,-(A7)           ;SAVE IT!
	MOVE.W  SUBJECT(A4),D0
	GETINFO        
	BTST    #6,4(A0)           ;ALIVE
	BEQ.S   DERR               ;DATA ERROR
	TEST_W  SUBOBJ(A4)
	BEQ.S   20$
	DO	P.OBJPRN
	BRA.S   30$
20$
	DO	P.PRN              ;HE OR SHE
30$
	MOVE.L  (A7)+,D0           ;RET
	RET
10$
	MSG     YOU
	MOVE.W  #1,PLURAL(A4)
	RET
   
*--------------------------------
*
* DERR - DATA ERROR LOTS OF MISC
*        ROUTINES COME HERE WHEN THEY
*        SUSS THAT THERE IS SOME 
*        IMPOSSIBLE DATA, STUFF
*        LIKE CONTENTS OF OBJ
*        BEING GREATER THAN IT MAX VOL
*        ETC..
*
*--------------------------------
 
*--------------------------------
* 
* IF PROD THEN JUST RETURNS
* IF DEMO THEN PRINTS 'DATA...
* THEN RETURNS
* IF NOT DEMO THEN TRAP #6
*
*--------------------------------
  
DERR
 
      IFNE    YCHEAT
	PRINT   <'^DATA ERROR^'>
      ENDC

	RET
 
*--------------------------------
*
* STOPBRK - STOPS THE USE OF CERTAIN
*           VERBS WI A BROKEN NOUN
*        
* ON ENTRY D0 - NOUN #
*          A0 - NOUN DATA
*
* VERB IN VERB(A4)
*
* IF VERB NOT ALLOWED & OBJ BROKEN
* THEN DOESN'T RETURN 
* IE. IT IGNORES THE NOUN
*
*--------------------------------
 
STOPBRK
 
	BTST    #2,(A0)            ;BROKEN?
	BEQ.S   20$                ;EQ= NO = FORGET IT
	MOVE.W  VERB(A4),D1        ;GET THE VERB NO
	LEA     BRK.VERB(A4),A1    ;LIST OF NON ABLLOWED VERBS
10$
	MOVE.B  (A1)+,D3           ;VERB IN LIST?..
	BEQ.S   20$                ;0= FINISHED, OK (LOOK = V#0)
	CMP.B   D3,D1              ;MATCH
	BNE.S   10$                ;NEXT...
   
	DO	YCANT              ;'YOU CANNOT DO THAT TO THE....
	MOVE.L  (A7)+,A1           ;POP  - NO RETURN IF BAD
	BRA     PROC.LAB           ;CONTINUE IN PROCESS FOR MORE NOUNS
20$
	RET
 

****************************************************
*
* YCANTWSTOP
* PRINTS : you cant do that to the xyzzy{.}
*
****************************************************
YCANTWSTOP
	CALL_S	YCANTW
	BRA	BSFULLSTOP

*--------------------------------
*
* YCANTW - PRINTS 'YOU CAN'T DO THAT WI THE NOUN'&
*
*--------------------------------
   
YCANTW
 
	DO	P.SUB
	MSG     ICNDTW
	DOGO	P.TN
 
*-----------------------------------------------------------
*
* YCANT - prints 'you cant do that to the xyzzy.'
*
*-----------------------------------------------------------

YCANT
	CALL_S	YCANTNFS
	DOGO	BSFULLSTOP

*
* YCANTNFS - as above but no fullstop
*

  
YCANTNFS

	DO	P.SUB
	MSG     YCNDTT
	DOGO	P.TN

*
* YCANTNOW - as above + at the moment
*

YCANTNOW

	CALL_S	YCANTNFS
	MSGGO	ATMOMT

*--------------------------------
* 
* CARRIED - SUSSES IF AN ITEM IS
*           CARRIED
*
* ON ENTRY - D0 = NOUN #
*
* ON EXIT - EQ = IS CARRIED/WORN/INTRINSIC
*
*--------------------------------
 
CARRIED
 
	CLR.W   D7                 ;RETURN FLAG
	PUSH_L  D1/D5        ;SCRATCH
	TEST_W  SUBJECT(A4)        ;LOOKS LIKE A FRUDGE FOR HORSE..
	BNE.S   50$                ;SEPARATE CASE (REKONS - KEN)
	DO	VALID.NP	          ;VALID LEAVES ALLSORTS OF HANDY THINGS
	MOVE.W  LASTA0+2(A4),D1    ;IN 'LASTA0+2' IS 8(A0).W OF 'TOP' ITEM
	BEQ.S   90$                ;OF ALL THE VARIOUS THINGS LEADING TO
	MOVE.B  LASTA0(A4),D1      ;THE NOUN, IN 'LASTA0' IS 6(A0).B
	AND.B   #$C,D1             ;8(A0)=0 = ON INVE= CARRIED
	BNE.S   90$                ;'$0C' MASK ALLOWES WORN&INTRINSIC
10$
	MOVEQ   #1,D7              ;SET 'NOT CARRIED' (ALSO NE) FLAG
90$
	PULL_L  D1/D5        ;RECOVER SCRATCH
	TEST_B  D7                 ;SET EQ/NE FLAG
	RET                        ;EXIT
	
50$
	CALL    TOPNPC             ;TO D4 - FIND NPC 'CARRYING' OBJ
	CMP.W   SUBJECT(A4),D4     ;ON HIS INV?
	BNE.S   10$                ;NO, FAIL
	CMP.W   D4,D0              ;NPC DOES *NOT* CARRY HIMSELF!
	BEQ.S   10$                ;SO D0=D4 = NO GOOD
	BRA.S   90$                ;ELSE OK
  
*--------------------------------
	        
P.BOTN
	
*PRINTS BECAUSE OF (THE NOUN)
 
	MSG     BECOF
	DOGO	P.TN
	
*--------------------------------
	
P.XFERGS
	
*ENTRY:
*D1=NUMBER TO PRINT (SINGLE CHR)
	
	MOVE.W  #2,PLURAL(A4)      ;SET TO PLURAL AGREEMENT
	CMP.B   #1,D1              ;SINGLE?
	BNE.S   10$                ;NO
	SUBQ.W  #1,PLURAL(A4)      ;SET TO 1
10$
	PUSH_L	D0-D3/D6
	MOVE.W	D1,D6

      IFNE	THE_PAWN
	AND.W	#$000F,D6
      ENDC

	CALL	PRTNUM
	PULL_L	D0-D3/D6

	MSG    SPACE
	MSGGO	FERG
	
*--------------------------------
	

P.NO.TNG
	
*PRINTS BUT THERE ARN'T ANY
	
	TEST_W  ERR1(A4)           ;ERRORS ON?
	BNE.S   90$                ;NO, EXIT WITHOUT PRINTING
	MSG     NO.TNG
	DO	D72                ;SET D7 = 2 
90$
	RET
	
*--------------------------------
	
P.NOTANY
	
	TEST_W  ERR1(A4)           ;ERRORS ON?
	BNE.S   90$                ;NO, EXIT
	CMP.W   #2,D7              ;HAS AN ERROR ALREADY HAPPENED?
	BEQ.S   90$                ;YES = DONT PRINT ANOTHER
	MSG     BTINA              ;PRINT ERROR ' BUT THERE AREN7T ANY...
	MOVEQ   #2,D7              ;SET ERROR HAS HAPPEND FLAG
90$
	RET
	    
*--------------------------------
	
ISDOFRE
	
*IS DIRECT OBJECT FREE (= GLOBAL)
* EXIT:
* EQ= IS FREE
* NE= NOT FREE
	
	PUSH_L  D1/A1        ;SCRATCH
	LEA     GLV.L_(A4),A1       ;LIST OF VERBS WI GLOBAL DIR OBJ
10$
	MOVE.B  (A1)+,D1           ;GET A VERB
	BEQ.S   95$                ;EQ=END OF LIST (LOOK NOT ALLOWED)
	CMP.B   VERB+1(A4),D1      ;IN LIST?
	BNE.S   10$                ;NO, TRY NEXT
90$
	PULL_L  D1/A1        ;RECOVER SCRATCH
	RET                        ;BYE..
95$
	SETNE                      ;SET FLAGS TO 'NE'
	BRA.S   90$                ;CLEAR UP..
	
*--------------------------------
	        
HOLDE
	        
	MOVE.L  (A7)+,ERRHOLD(A4)  ;STORE RET
	MOVE.W  ERR1(A4),-(A7)
	MOVE.W  ERR4(A4),-(A7)
	MOVE.L  ERRHOLD(A4),-(A7)  ;PUSH BAK
	ST      ERR1(A4)
	ST      ERR4(A4)  ;MASK ERRORS
	RET
	
*--------------------------------
	
UNHOLDE
	        
	MOVE.L  (A7)+,ERRHOLD(A4)
	MOVE.W  (A7)+,ERR4(A4)
	MOVE.W  (A7)+,ERR1(A4)
	MOVE.L  ERRHOLD(A4),-(A7)
	RET
	
*--------------------------------

	END

