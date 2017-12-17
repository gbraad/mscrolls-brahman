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
*	@(#)getdata.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.GETDATA
*
*--------------------------------



	include	"equates.asm"
	include "macros.asm"
	include	"nounequ2.i"       ;JUST FOR NMAXNOUN


        XREF    DERR,D.NOUNS,DNOUNBUF,VALID,VECT.V,NPC.DATA,NPC.LIST
	XREF	PRTTXT,LINE,P.TN,PRTHEX

        XDEF    GETDATA,GETNOUNS,GETNOUN2,GETNPCNO,GETRELN,GETVECT,GETNPC



       IFEQ    FastBits


*--------------------------------
*
*    the famous getdata....
*
* Given D0 returns A0 -> to noundata
*
*--------------------------------

GETDATA
 
       AND.L   #$7FFF,D0            ;MASK
       MOVE.W  D0,-(A7)
       MULU    #14,D0               ;EACH RECORD 14 BYTES LONG
       LEA     D.NOUNS-14(A4),A0    ;ADJUST SINCE NOUN#0 NOT REAL
       ADD.L   D0,A0
       MOVE.W  (A7)+,D0
       RET
 
*--------------------------------
* 
* GETNOUNS - GETS ALL THE NOUNS
*            WHICH HAVE D2.B AS
*            THE DATA IN BYTE
*            D1.B OF THEIR DATA
* GETNOUN2-  Get all nouns (ie including those that are
*	     not local )
*
*--------------------------------
 
 
GETNOUNS

       CLR.W   D6

GETNOUN2

       AND.L   #$FF,D1                 ;BYTE ONLY
       AND.L   #$7FFF,D0
       PUSH_L  D0-D5/D7/A0/A2-A6 ;SAVE
       LEA     D.NOUNS(A4),A0
       ADD.L   D1,A0
       MOVE.W  #1,D3
       LEA     DNOUNBUF(A4),A1
10$ 
       TEST_B  D5                      ;IF WORD OR BYTE
       BNE.S   30$
       CMP.B   (A0),D2
11$
       BNE.S   20$
       TEST_B  D6
       BNE.S   12$
       EXG     D0,D3
       DO	VALID
       EXG     D0,D3
       TEST_B  D7
       BNE.S   20$
12$
       MOVE.W  D3,(A1)+
20$
       ADD.L   #14,A0
       ADDQ.W  #1,D3  
       CMP.W   #NMAXNOUN,D3
       BLT.S   10$    
       CLR.W   (A1)
       MOVE.L  A0,A1
       PULL_L  D0-D5/D7/A0/A2-A6    ;BACK
       LEA     DNOUNBUF(A4),A1
       RET
30$
       MOVE.W  (A0),D4
       AND.W   #$3FFF,D4
       CMP.W   D4,D2
       BRA.S   11$


       ENDC	

*--------------------------------
   
GETNPC

       ;; NPC in D0
       ;; NPC DATA returned in A3
        
       PUSH_L  D3/D4/A0/A1
       CALL_S  GETNPCNO
       LEA     NPC.DATA(A4),A3
       LSL.L   #2,D4
       ADD.L   D4,A3
       PULL_L  D3/D4/A0/A1
       RET
       
       
*--------------------------------
       
	IFNE	0		;old way...       

GETNPCNO
       
       
*RETURNS NPC NO. IN D4
*NO REGISTER PRESERVE
       
       CLR.W   D3
       CLR.L   D4
       LEA     D.NOUNS(A4),A1
       ADDQ.L  #4,A1
20$
       BTST    #6,(A1)
       BEQ.S   10$
       ADDQ.W  #1,D4
10$
       ADD.L   #$E,A1
       ADDQ.W  #1,D3
       CMP.W   D0,D3
       BLT.S   20$
       SUBQ.W  #1,D4
       BMI     DERR
       RET

	ENDC

       
*RETURNS NPC NO. IN D4.L !!!!!!
*NO REGISTER PRESERVE
       
GETNPCNO 

	LEA	NPC.LIST(A4),A1		;list of npc's NOUN numbers
	MOVEQ	#-1,D4			;NPC no. returned in D4
10$
	ADDQ.L	#1,D4			;but lowest is 0.
	CMP.W	(A1)+,D0		;($8000) terminator
	BHI.S	10$			;not this one, but not past either
    
    IFNE	YCHEAT

	BPL.S	90$
	PRINT	<'Iffy call to GETNPC for '>
	MOVE.L	D0,D6
	CALL	PRTHEX
	MSG	LINE

    ENDC

90$
	RET



*--------------------------------
*
* GETVECT - GET ADDR OF VERB
*           ROUTINE (VN# IN D2)
*
* ON EXIT: A1 = ADDR OF VERB (ABS)
*
*--------------------------------

* Warning!!!! Now PC relative - cos of alink - 9/8/86 Ken
 
GETVECT
       
       LSL.W   #1,D2              ;*2
       LEA     VECT.V(PC),A1
       LEA     0(A1,D2.W),A1		;get address our word is at.
       MOVE.W  (A1),D2
       AND.L   #$FFFF,D2          ; Now 16 bit UNSIGNED (pc) offsets.
       ADD.W   #$8000,D2          ;REALLY WORDWIZE !which are frigged a bit
       LEA     0(A1,D2.L),A1
       RET

*--------------------------------
       
GETRELN
       
       MOVEQ   #1,D5
       MOVEQ   #8,D1
       DOGO	GETNOUNS


******************************************************************************

         IFNE    FastBits 

       
*--------------------------------
*
*    the famous getdata....
*
* Given D0 returns A0 -> to noundata
*
*--------------------------------

GETDATA
        DC.W   $A0FE          ;non 68k GETDATA
        RET
 
*--------------------------------
* 
* GETNOUNS - GETS ALL THE NOUNS
*            WHICH HAVE D2.B AS
*            THE DATA IN BYTE
*            D1.B OF THEIR DATA
*
*--------------------------------
 
 
GETNOUNS

       CLR.W   D6

GETNOUN2

       AND.L   #$FF,D1
       AND.L   #$7FFF,D0
       PUSH_L  D0-D5/D7/A0/A2-A6 ;SAVE
       LEA     D.NOUNS(A4),A0
       ADD.L   D1,A0
       MOVEQ   #0,D3
       MOVE.W  #NMAXNOUN,D4
       LEA     DNOUNBUF(A4),A1
10$

       DC.W    $A0FA          ; Scan to next match
       BCC.S   17$            ; Branch if no match
       TEST_B  D6
       BNE.S   12$
       EXG     D0,D3
       DO	VALID
       EXG     D0,D3
       TEST_B  D7
       BNE.S   10$
12$
       MOVE.W  D3,(A1)+
       BRA.S   10$
       
17$
       CLR.W   (A1)

       PULL_L  D0-D5/D7/A0/A2-A6    ;BACK
       LEA     DNOUNBUF(A4),A1
       RET

      ENDC

      END
