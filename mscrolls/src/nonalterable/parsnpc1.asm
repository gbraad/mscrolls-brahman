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
*	@(#)parsnpc1.asm	2.5
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*
* SRC.ADV.PARSER.NPC1
*
*--------------------------------


	include	"equates.asm"
	include	"macros.asm"

      XDEF     PFAIL,PARSEONE,PARSETHR,PARSEFOU,NPC.DECO,DEC.N2

      XREF     NPC.TOBE,MNPH,NPC.LBF1,TEMPBUF,NPCMODEL,NPCQUEST,WRDPNT
      XREF     GETWRD,ONECOP,DOHOW,VERB,CONVSE.V,NESTSPK,GETADJPH,ADVERB
      XREF     NOUNBUF,DO.THEN,DO.CONJ2,EXTRAFIX
   
*CONSTRUCT ELEMENTS:
       
*      PHRASE FORMS:
*      
       
*1.    (BE)+N+S(ING)     BASE TYPE 
       
*2.    A+1
       
*3.    B+N+S             BASE TYPE
       
*4.    A+3
       
*WHERE...
       
*A=A MEMBER OF SECT.QUEST
*B=A MEMBER OF SECT.MODEL
       
*--------------------------------
       
PFAIL
       
*GENERALISED FAILURE EXIT FOR
*PARSE CONSTRUCTS
       
       MOVEQ   #1,D7      ;FAIL FLG
       RET
       
*--------------------------------
  
PARSEONE
       
*PARSE A TYPE 1 STRUCTURE.
       
       CMP.W   #SECT.TOBE,D6  
       BNE.S   PFAIL                ;FAIL
       MOVE.W  D5,NPC.TOBE(A4)      ;PARSED
       DO	MNPH                 ;NOUN
PT1
       LEA     NPC.LBF1(A4),A0      ;GET LOCAL BUF
       LEA     TEMPBUF(A4),A1
10$
       MOVE.W  (A1)+,(A0)+
       BNE.S   10$                  ;SHIFT THAT PHRASE
       CALL    NPC.DECO
99$
       MOVEQ   #0,D7      ;NO ERROR
       RET
*--------------------------------
       
PARSETHR
       
*PARSE TYPE 3 CONSTRUCT
       
       CMP.W   #SECT.MODEL,D6 ;MODEL?
       BNE.S   PARSEONE  ;FAIL, TRY OTHER TYPE
       MOVE.W  D5,NPCMODEL(A4)
       DO	MNPH
       BRA.S   PT1               ;CONTINUE FROM TYPE 1

*--------------------------------
       
PARSEFOU
       
*PARSER TYPE 4 CONSTRUCT
       
       CMP.W   #SECT.QUEST,D6 
       BNE.S   PFAIL     ;NO
       MOVE.W  D5,NPCQUEST(A4)
       MOVE.L  A6,WRDPNT(A4)      ;SAVE
       DO	GETWRD	            ;NEXT
       CMP.W   #1,D7              ;EOLINE
       BEQ     ONECOP
       CALL    DOHOW              ;TESTFOR HOW MUCH/MANY..
       TEST_W  D7
       BEQ.S   99$                ;EXIT DID IT!
       MOVEQ   #0,D7              ;OK
       BRA.S   PARSETHR           ;CONTINUE
99$
       RET
*--------------------------------
       
       
NPC.DECO
 
 
       CLR.B   D3                 ;NOUN FLAG
       TEST_B  D7
       BNE.S   PFAIL 
       CMP.B   #SECT.VERB,D6
       BNE     PFAIL
01$
       MOVE.W  D5,VERB(A4)
       CALL    CONVSE.V
       BEQ     NESTSPK
       MOVE.L  A6,WRDPNT(A4)
       DO	GETADJPH
       CMP.B   #1,D7              ;ERROR?
       BHI     PFAIL 
       CMP.B   #SECT.CONJ,D6      ;PRETENDING TO BE ADVERB?
       BNE.S   15$
       MOVE.W  D5,ADVERB(A4)
       BRA.S   DEC.N2
15$
       MOVE.L  WRDPNT(A4),A6

DEC.N2

       MOVEQ   #-1,D6             ;INCASE OF E O S
       DO	MNPH    
 
       LEA     TEMPBUF(A4),A0
       LEA     NOUNBUF(A4),A1
       
 
10$
       MOVE.W  (A0)+,(A1)+
       BNE.S   10$
       CMP.B   #1,D7              ;MATCH NOT FOUND.
       BHI     PFAIL 
20$
       CMP.B   #SECT.SPEC,D6
       BNE.S   30$                ;NOT SPECIAL
       CMP.B   #$08,D5
       BEQ     DO.THEN            ;PROCESS MULTIPLE COMMAND
30$
       CMP.B   #SECT.CONJ,D6  
       BEQ     DO.CONJ2           ;MEETS CONJUNCTION
       CMP.B   #SECT.VERB,D6      ;VERB
       BEQ     PREXIT
       TEST_W  D7                 ;E O S
       BNE     EXTRAFIX
       BRA     PFAIL 
       
*--------------------------------
       
PREXIT
       RET

      END

