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
*	@(#)valid.asm	2.12
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*     SRC.ADV.VALID
* 
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"


	XREF    GETDATA,PSUDAT,PLY.TIED,LIGHT,SUBJECT,SUBJECT2
	XREF    TOPLOC,SCANFLAG,GLOBAL.F,CHEAT,P.NFND,ROOM
	XREF    LASTA0,VLST.GO2,VLEN.GO2,NETWORK,VERB

	XREF	XRAY_VLIST

      IFEQ	THE_PAWN

	XREF	NearbyVerbs,VLEN_NBVerb

      ENDC


	XDEF    VALID,VALID.NP,VALID.WD,VALID.WO

*-------------------------------
*
* VALID - SUSSES IF NOUN D0 IS
*         LOCAL, PRINTS ERROR
*         MSG IF D5=0 WHEN NOT
*         LOCAL.
*
* IF GLOBAL.F < 0 THEN
* ALWAYS LOCAL
*
* RETURNS D7 = 0 = LOCAL
*         D7 < 0 = NOT
*
* ALTERNATIVE ENTRIES:
* ====================
*
* VALID.WD - ENTRY POINT TO DO A
*            VALID ON A WORD NO.
*            RATHER THAN A NOUN#
*            *ALWAYS* NO ERRORS
*
* VALID.NP - ENTRY POINT SETS NO ERROR 
*            PRINTING, SAME RETURNS   
*
*------------------------------

VALID.WO 
VALID.WD
 
	ADDQ.W  #1,D0              ;CONVERT WORD# TO NOUN#
	CALL_S  VALID.NP           ;SUSS IF VALID (NO PRINTING)
	SUBQ.W  #1,D0              ;CONVERT BACK TO WORN#
	RET
 
 
VALID.NP
 
	MOVEQ   #1,D5              ;SET NO ERROR PRINTING FLAG
  
*
*  FALL THROUGH TO VALID
*
 
VALID 

	MOVE.W  #5,PLY.TIED(A4)    ;MAX TIMES TO CONSIDER TIED OBJ
 
	PUSH_L  D0-D6/A0-A6

	GETINFO        
	CLR.L   D7                 ;ERR FLG
	
	MOVE.W  D0,D2              ;SAVE, so we know noun# u can's see (VALNO)

	BTST    #7,6(A0)           ;OUT?
	BNE     VALNO              ;NE=YES=NOT VALID
	
	TEST_W  LIGHT(A4)          ;0= DARK
	BNE.S   VALDO
 
* DARK ROOM STUFF..
  
	BTST    #7,(A0)            ;LIT OR 'SLIGHTLY LIT'
	BNE.S   VALDO              ;OK - U CAN SEE IT
	BTST    #0,5(A0)           ;PSEUDO?
	BNE     VALNO              ;CANNOT SEE IT IN DARK
	DO	TOPLOC             ;CARRIED/IN POCKET/ETC?
	MOVE.W  SUBJECT(A4),D1
	EXT.L   D1
	CMP.L   6(A0),D1
	BEQ.S   05$
	MOVE.W  D2,D0              ;GET 'REAL' NOUN# BACK
	GETINFO                    ;AND DATA
	BRA	VALNO              ;PRINT 'CAN'T SEE XYZZY HERE'  
05$
	MOVE.W  D2,D0
	GETINFO        
 
* IF NOT A DARK ROOM COME HERE
  
VALDO

	BTST    #0,5(A0)           ;PSEUDO ?
	BNE     VALPSEUDO          ;YES - WE HAVE A PSEUDO NOUN
	MOVE.B  6(A0),D1           ;GET HB OF LOCATION INFO
	BEQ     VALTOP             ;ONLY CHK ROOM - AT TOP OF NOUN NEST
	BTST    #7,D1              ;OUT?
	BNE.S   VALNO              ;NO GOOD 
	BTST    #6,D1              ;ROOM?
	BNE     VALROOM            ;SEE IF ROOM IS ADJACENT  
	AND.B   #$0C,D1            ;YES
	BNE     VALONU             ;MUST BE INTRINSIC OR WORN (BY U)
	TEST_W  SCANFLAG(A4)       ;HAVE WE COME FROM SCAN?
	BNE.S   20$                ;YES, IGNORE HIDDEN FLAG   
	BTST    #1,6(A0)           ;HIDDEN?
	BNE.S   VALNO              ;NOT IN ROOM (HONEST)   
20$
	MOVE.W  8(A0),D0           ;POINTED NOUN
	MOVE.B  6(A0),D3           ;SAVE SO WE CAN CHECK IF CONTAINED
	GETINFO        
	MOVE.B  10(A0),D1          ;GET CONTAINER VOL
	AND.B   #$0F,D1            ;MASK REST OUT
	BEQ.S   VALDO              ;NOT A CONTAINER OK,BACK AND REPEAT
	MOVE.B  (A0),D1            ;GET STATUS
	AND.B   #$60,D1            ;TEST IF LOCKED OR CLOSED
	BEQ.S   VALDO              ;OK,BACK AND REPEAT
	BTST    #5,D3              ;CONTAINED?
	BEQ.S   VALDO              ;NOPE - IS ALLOWED


* See if verb forces a 'get without opening' condition to apply
* eg BUY BEER ... buys the beer (bottle) although it's not really VALID

	BTST	#6,(A0)			;Locked?
	BNE.S	40$			;XRAY won't work then

	LEA	XRAY_VLIST(A4),A1
30$
	MOVE.B	(A1)+,D1
	BEQ.S	40$
	CMP.B	VERB+1(A4),D1
	BNE.S	30$
	BRA.S	VALDO

40$
	BTST    #3,(A0)            ;BUT IS IT A TRANSPARENT CONTAINER?

	BNE.S   VALDO              ;YES, SO I'L LET YOU OFF...

*
* NOT VALID SINCE IN A CLOSED CONTAINER
*
 
VALNO
	MOVE.W  D2,D0              ;GET BACK ORIGINAL NOUN#
	TEST_W  D5                 ;SUPRESS ERROR PRINTING?
	BNE.S   10$                ;NE = YES = DONT PRINT
	TEST_W  GLOBAL.F(A4)       ;ALLOWED ANYWHERE FLAG SET?
	BNE.S   10$                ;NE = YES = DONT PRINT
 
      IFNE    YCHEAT             ;ASSEMBLING WITH CHEAT MODE?
 
	TEST_W  CHEAT(A4)          ;YES, THERE FOR TEST THE FLAG
	BNE.S   10$                ;HMM - NE = IN CHEAT MODE
 
      ENDC                       ;END OF CONDITIONAL BIT..
 
	CALL    P.NFND             ;PRINT THE ERROR
  
10$
	MOVEQ   #1,D7              ;FLAG THE ERROR
	BRA.S   VALEXIT            ;EXIT
 
VALPSEUDO
 
	LEA     PSUDAT(A4),A1      ;GET LIST OF PSEUDO LOCS
	ADD.W   8(A0),A1           ;ADD OFFSET TO GET FIRST LOC
	MOVEQ   #0,D1              ;SO AS HI BYTE OF LOW WORD IS OK
 
10$
	MOVE.B  (A1)+,D1           ;GET LOC
	BEQ     VALNO              ;NO MORE LOCS = NOT IN ONE OF THEM
	CMP.W   ROOM(A4),D1        ;WE IN LOC FROM LIST?
	BNE.S   10$                ;NOT YET - TRY MORE
 
	MOVE.W  D2,D0              ;GET REAL NOUN#
	GETINFO                    ;AND INFO
	TEST_B  7(A0)              ;SEC. LOC. SET? (NO NESTED REL TO PS.)
	BEQ.S   20$                ;NO - OK AT ALL OF THEM!
	CMP.B   7(A0),D1           ;IS IT AT THIS ONE?
	BNE     VALNO              ;NOPE - NOT VALID!
20$
	BRA.S   VALEXIT 
 
VALTOP
 
	MOVE.W  8(A0),D1
	CMP.W   SUBJECT(A4),D1  
	BNE.S   01$                ;NO
	MOVE.W  6(A0),D0
	CMP.B   SUBJECT2(A4),D0    ;MUST BE 
	BEQ     VALEXIT            ;YES, OK
	
01$
	CMP.W   ROOM(A4),D1        ;IN ROOM?
	BEQ.S   VALEXIT
 
	MOVE.W  D2,D0              ;LAST DITCH - SUSS IF TIED TO A LOCAL
	GETINFO                    ;OBJECT
      
      IFEQ	THE_PAWN

	TEST_W  12(A0)             ;TIED?
	BEQ     VALNearbyRoom2     ;IF NOT is verb such that next room is ok?

      ENDC

	MOVE.W  12(A0),D0          ;TIED?
	BEQ     VALNO              ;IF NOT THEN CERTAINLY NOT LOCAL
	SUBQ.W  #1,PLY.TIED(A4)    ;STOP TIE A TO B THEN B TO A CRASH
	BMI     VALNO              ;TO MANY PLY - ERROR
	GETINFO        
	BRA     VALDO              ;SEE IF TIED TO LOCAL OBJ
 
  
VALEXIT
 
	MOVE.L  6(A0),LASTA0(A4)	;save D0,A0 to suss toploc
	MOVE.W  D0,LASTA0+4(A4)
  
	PULL_L  D0-D6/A0-A6		;retrieve regs  
 
	TEST_W  GLOBAL.F(A4)		;global??
	        
      IFNE    YCHEAT
	BNE.S   10$
	TEST_W  CHEAT(A4)          ;CHEAT MODE?   
	BEQ.S   20$
      ENDC

      IFEQ    YCHEAT
	BEQ.S   20$
      ENDC

10$
	CLR.L   D7                 ;FORCE GLOBAL
20$
	TEST_B	D7		;cos ken thinks it should work this way
	RET
 
VALROOM
 
	MOVE.W  8(A0),D3           ;GET ROOM #
	CMP.W   ROOM(A4),D3        ;LOCAL IF CURRENT ROOM
	BEQ.S   VALEXIT            ;OK ALLOWED
	
	CMP.W   D2,D0              ;ORIGINAL NOUN?

      IFEQ	THE_PAWN

	BNE     VALNearbyRoom      ;No, But is it like shout? 

      ENDC

      IFNE	THE_PAWN

	BNE	VALNO		;NO=No good since only rooms allowed

      ENDC
      
	LEA     VLST.GO2(A4),A1    ;SUSS IF VERB IS A 'GO' TYPE
*                                 ;MISS -1 FIRST VERB
	MOVE.W  #VLEN.GO2,D0       ;NO OF ALLOWED VERBS (-1)
	MOVE.W  VERB(A4),D4        ;GET CURRENT VERB
10$
	CMP.B   (A1)+,D4           ;SCAN LIST FOR VERB
	DBEQ    D0,10$
	BNE     VALNO              ;NOPE - NOT IN LIST   
 
* SINCE WE GOT THIS FAR VERB MUST BE OK


VALAdjacentRoom

* Checks if room in D3 is adjacent to the current room 

	LEA     NETWORK(A4),A1     ;GET ADDR OF NETWORK
	MOVE.W  ROOM(A4),D4

      IFNE    M68000
	MULU    #Sizeof_NET,D4             ;10 BYTES/ROOM
      ENDC

      IFEQ    M68000
	MOVE.W  D4,-(A7)
	LSL.W   #3,D4
	ADD.W   (A7),D4
	ADD.W   (A7)+,D4

* New network contains IN/OUT info too
* But Pawn is still 10 directions
     
       IFNE	Sizeof_NET-10

	ADD.W	-(A7),D4
	ADD.W	(A7)+,D4

       ENDC

      ENDC

	ADD.W   D4,A1			;GET ADDR OF THIS ROOM (+10)
	MOVEQ   #Sizeof_NET-1,D4	;START AT LAST ROOM
20$
	CMP.B   -Sizeof_NET(A1,D4.W),D3	;IS IT ADJACENT?
	BEQ     VALEXIT			;OK - LOCAL
	DBRA    D4,20$
	BRA     VALNO			;NOT ADJACENT
 
VALONU
	
	 TEST_W   SUBJECT(A4)        ;YOU?
	 BEQ      VALEXIT            ;OK, PASS
	 BRA      VALNO              ;TOO BAD IT'S SOMEONE ELSE:


      IFEQ	THE_PAWN

* Is the verb like shout,throw etc.. ie: it can be direct object can be 
* valid in neighbouring room 

VALNearbyRoom2

	MOVE.W	D1,D3			;cos VALTOP looses D0 (urk!)

VALNearbyRoom
	
	LEA	NearbyVerbs(A4),A1	;list of verbs that allow objects to
					;be valid when in adjacent room
	MOVEQ	#VLEN_NBVerb,D0		;number of these verbs (-1)
	MOVE.W	VERB(A4),D4
10$
	CMP.B	(A1)+,D4		;Verb in list?
	DBEQ	D0,10$
	BNE	VALNO			;NE => not in list
	BRA	VALAdjacentRoom		;this wants D3 (room in question)
	
      ENDC

      END
