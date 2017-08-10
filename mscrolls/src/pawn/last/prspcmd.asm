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
*	@(#)prspcmd.asm	2.2
*	2/8/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.PRSPCMD
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"macros.asm"


	XREF   VERB,CHEAT,CONVNPC,SEATED,RND.D1,SUBJECT
	XREF   SUBJECT2,NOUN,NOUNBUF,NPCCMDPT,PR20,NPCQUEST
	XREF   NPCMODEL,NPC.LBF1,P.SAY,P.BOTN,F_GURU
	XREF   UTIL.BS,SPACE,DO.SCN,PRTTXT

	XDEF   PRSPCMD,PR.SPEC,PRSPDO,SP.ODREP,SP.SAY


	
PRSPCMD
	
	CMP.B   #SECT.DIR,D6         ;IS A STRIAGHT DIRECTION?
	BNE.S   04$
	MOVE.W  #VNGO,VERB(A4)
	BRA.S   02$
  
	
04$
	MOVE.W  D5,VERB(A4)
	IFNE    YCHEAT
	TEST_W  CHEAT(A4)
	BNE.S   02$
	ENDC
	
	LEA     L.CMD(PC),A5         ;ALLOWED VERBS
01$
	MOVE.B  (A5)+,D1
	BMI.S   90$                  ;FAIL, NOT IN LIST
	CMP.B   D1,D5                ;WEL??
	BNE.S   01$                  ;NOPE
02$
	MOVE.L  CONVNPC(A4),A0
	MOVE.W  CONVNPC+4(A4),D0   
	CMP.W   #NHORSE,D0           ;TO HORSE?
	BNE.S   90$                  ;NOPE
	CMP.W   SEATED(A4),D0        ;ON HORSE?
	BNE.S   90$
	MOVEQ   #4,D1
	CALL    RND.D1
	TEST_W  D1
	BNE.S   10$
	SCN    213                   ;DONT ORDER ME
	BRA.S   80$                  ;FAIL
10$
	SCN     302                  ;OKAY
	MOVE.W  D0,SUBJECT(A4)
	MOVE.B  #1,SUBJECT2(A4)
	MOVE.L  (A7)+,D0             ;GET IT
     
	CLR.W   NOUN(A4)             ;ZAP NOUNBUFF FOR NEW CMD
	CLR.W   NOUNBUF(A4)
	BRA     NPCCMDPT
	
80$
	MOVE.L  (A7)+,D0             ;DOWN
	BRA     PR20
90$
	RET
      
*--------------------------------
	
*ALOWABLE VERBS FOR THE HORSE
 
L.CMD
 
	       DC.B    VNGO
	       DC.B    VNWALK
	       DC.B    VNENTER
	       DC.B    VNRUN
	       DC.B    VNEXIT
	       DC.B    VNCLIMB
	       DC.B    VNGET
	       DC.B    VNTAKE
	       DC.B    VNCARRY
	       DC.B    VNDROP
	       DC.B    VNLOOK
	       DC.B    VNEXAMINE
	       DC.B    VNKISS
	       DC.B    -1
 
	ALIGN
 
*--------------------------------
	        
PR.SPEC
	
* SPECIALS FOR NPC INTERACTION
* LIVE HERE
	
*ENTRY:
* D2=PERSON
* CONVNPC+4 = NPC IN CONVERSATION
* CONVNPC   = DATA ON NPC
	
	
	
	LEA     L.PRSP(PC),A0
02$
	MOVE.W  (A0)+,D1
	BEQ.S   90$                  ;NOT IN LIST
	CMP.W   CONVNPC+4(A4),D1
	BEQ.S   10$                  ;OK
	ADD.L   #12,A0               ;SKIP TO NEXT SECTION
	BRA.S   02$                  ;AGAIN
10$
	MOVE.W  (A0)+,D1
	BMI.S   15$                  ;SKIPP
	CMP.W   NPCQUEST(A4),D1
	BNE.S   02$                  ;FAIL
15$
	MOVE.W  (A0)+,D1             ;GET MODEL
	BMI.S   20$                  ;OPTIONAL
	CMP.W   NPCMODEL(A4),D1      ;SAME?
	BNE.S   02$                  ;FAIL
20$
	MOVE.W   (A0)+,D1            ;GET MAIN NOUN
	BMI.S   30$                  ;OPTIONAL
	CMP.W   #2,D1                ;3RD OR NOUN?
	BLT.S   25$                  ;NO, IS PERSON CHK
	CMP.W   NPC.LBF1(A4),D1   ;AS IN BUFFER?
	BNE.S   02$                  ;NO, FAIL
	BRA.S   30$                  ;OK PASS
25$
	CMP.W   D2,D1                ;AS PERSON?
	BNE.S   02$                  ;FAILK
30$
	MOVE.W  (A0)+,D1             ;GET VERB
	BMI.S   40$                  ;OPTIONAL
	CMP.W   VERB(A4),D1
	BNE.S   02$                  ;FAIL
40$
	MOVE.W  (A0)+,D1             ;INDIRECT NOUN
	BMI.S   50$                  ;OPTIONAL
	CMP.W   NOUNBUF(A4),D1
	BNE.S   02$                  ;NOPE
50$
*
*
*  SUCESS!
*
	MOVE.W  (A0),D1              ;DISP
	LEA     PRSPDO(PC),A0
	ADD.W   D1,A0
	JSR     (A0)
	MOVE.L  (A7)+,D0             ;GET RET
	BRA     PR20                 ;EXIT
	
90$
	RET
 
*--------------------------------
*
*
*
* FORMAT FOR SP.SPECIALS
*
* /NPC (ESSENTIAL) 
* /QUEST (OPTIONAL=-1) ,
* /MODEL (OPTIONAL=-1),
* /DIRECT OBJECT (OPTIONAL=-1)
*         0=IST PERSON
*         1=2ND PERSON
*      NOUN= MAIN NOUN
* /VERB (OPTIONAL =-1)
* /INDIRECT OBJECT (OPTIONAL=-1)
* /ADDR OF HANDLER (REL TO PRSPDO)
*
*--------------------------------
	
L.PRSP
      
* TABLE OF SPECIALS
	
	       DC.W    NGURU
	       DC.W    4                   ;WHY?
	       DC.W    -1                  ;NO MODEL
	       DC.W    1                   ;YOU
	       DC.W    VNLAUGH             ;LAUGHING
	       DC.W    -1                  ;NONE
	       DC.W    PRSP1-PRSPDO
     
	        
	       DC.W    0                   ;TERMINATE
	
*--------------------------------
	
PRSPDO
	
*ROUTINES TO HANDLE SPECIAL 
*QUESTIONS
	
PRSP1
*ASK GURU WHY ARE YOU LAUGINH?
	
	CALL    P.SAY
	MOVE.W  #NBAND,D0
	BRA     P.BOTN             ;BECAUSE OF (THE NOUN)
 
*--------------------------------
	
	
SP.ODREP 
	
*NON-STANDARD REPLYS
	
	CMP.W   #NSNOWMAN,D0
	BNE.S   90$                  ;NO
	SCN     24 
	MOVE.L  (A7)+,D0             ;AND SET NE
90$
	RET
  
*--------------------------------
*
*  SP.SAY, CALLED BY P.SAY(2)
*          DOES STUFF LIKE
*          GURU, BETWEEN FITS..
*
* ACCTUAL STUFF GOES HERE
* 
*--------------------------------
  
SP.SAY
	
	CMP.W   #NGURU,D0            ;GURU IS TALKING?
	BNE.S   90$                  ;NO, FAIL
	CMP.W   #1,F_GURU(A4)        ;HAVNT COVERED THE BAND?
	BNE.S   90$                  ;NO
	CALL    UTIL.BS              ;LOSE SPACE AFTER P.TN
	SCN     312                  ;BETWEEN FITS OF LAUGHTER
	CALL    SPACE                ;SHOULD BE IN TEXT!
90$
	RET
	
*--------------------------------

	END

