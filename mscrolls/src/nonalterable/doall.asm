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
*	@(#)doall.asm	2.5
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*-------------------------------
*
* SRC.ADV.DO.ALL
*
*--------------------------------


	include	"equates.asm"
	include	"macros.asm"

      XREF  NOUN,ROOM,VERBTYPE,SUBJECT,SUBJECT2,MULT.WRD,BICHK
      XREF  GETWRD,RELTYPE,GETADJPH,OLDBUF
      XREF  GETDATA,CONJBUF,MUST4,SEATED,GETRELN
      XREF  INCNOUN,INCNOUN2
      XREF  RELWRD,DO.ADJ,WSYN2,GNG2,P.NO.TNG
      XREF  D72,GET_CONT

      XDEF  DO.ALL

 
DO.ALL 

       MOVE.W  NOUN(A4),-(A7)      ;Save no, of nouns found in noun phrase
       MOVE.L  A6,-(A7)            ;Save pointer into input stream

DO.ALL2

       DO	GETWRD	             ;get a word from input stream
       CMP.B   #1,D7               ;only error allowed is end of line (D7=1)
       BHI     EXIT.BAD            ;Ouch! - bad error condition
       CALL    RELWRD              ;Deal with relative phrases
       CMP.B   #1,D7               ;ditto previous error condition
       BHI     EXIT.BAD
       CMP.B   #SECT.CONJ,D6       ;Conj?
       BNE     09$                 ;no

* Found a conj, so deal with stuff like 'l all on xyzzy', 'l all in xyzzy' etc

       MOVEQ   #$3,D1              ;Bit mask for 'RELTYPE'
       CMP.W   #2,D5               ;FROM
       BEQ.S   02$
       CMP.W   #5,D5               ;IN
       BNE.S   51$                 ;NO
       MOVEQ   #1,D1               ;1=INSIDE bit mask
       BRA.S   02$
51$
       CMP.W   #$3,D5              ;ON
       BNE     09$   
       MOVEQ   #2,D1               ;1= ON TOP OF bit mask 
02$
       MOVE.B  D1,RELTYPE(A4)
       DO	GETADJPH            ;Get  a noun
       CMP.B   #1,D7               ;ERR? (other than end of input line)
       BHI     EXIT.BAD
       CMP.B   #SECT.NOUN,D6       ;NOUN?
       BEQ.S   03$                 ;OK
       CMP.W   #SECT.SPEC,D6
       BNE.S   01$
       CMP.W   #3,D5               ;IT?
       BNE.S   01$
       MOVE.W  OLDBUF(A4),D5
       SUBQ.W  #1,D5
       BRA.S   03$
01$
       CMP.B   #SECT.ADJ,D6        ;ADJ?
       BNE.S   06$
       CALL    DO.ADJ
       BNE.S   03$
06$
       MOVE.W  (A7)+,D0            ;DUMP #
       MOVE.L  (A7)+,A6            ;OLD
       BRA     WSYN2               ;Do a 'I think there is a word ...' error
03$
       MOVE.W  D5,D0
       ADDQ    #1,D0
       GETINFO
       BTST    #0,RELTYPE(A4)      ;INCLUDE CONTENTS?
       BEQ.S   40$                 ;NO
       CALL    INCCONT
40$
       BTST    #1,RELTYPE(A4)      ;INCLUDE ON?
       BEQ.S   41$
       CALL    INCREL
41$
       MOVE.L  (A7)+,D1            ;LOOSE IT
       TEST_W  CONJBUF(A4)         ;HAS A CONJ BEEN ACCEPTED YET?
       BPL     30$                 ;YES, INTERPRET AS RELPH
       CALL    MUST4               ;MUST BE A RELPH?
       BEQ     30$                 ;YES
       MOVE.L  A6,-(A7)
       DO	GETWRD
       CMP.B   #1,D7
       BHI     EXIT.BAD
       MOVE.L  (A7)+,A6
       BNE     30$                 ;MORE=RELPH
       MOVE.L  D1,A6               ;RESTORE OLD TXT PNTR
       CLR.W   NOUN(A4)            ;ZAP ANY BUFFER
       BRA.S   11$                 ;ASSUME ALL MEANS (ALL)
09$
       CMP.B   #SECT.MULT,D6       ;MULT
       BEQ     DO.ALL.MULT         ;Even stack then go to MULT.WRD

       CMP.B   #SECT.SPEC,D6       ;test for 'all of ...'
       BNE.S   07$
       CMP.W   #11,D5              ;'of'
       BEQ     DO.ALL2             ;Try again
07$
       MOVE.L  (A7)+,A6            ;Recover word old text pointer
       BRA.S   11$                 ;Not a mult - treat as n/a

32$
       MOVEQ   #1,D3               ;SET FLG
       BRA     GNG2                ;Get noun group
11$  
       CALL    VERBTYPE            ;An 'on inv' or 'in room' verb?
       TEST_B  D7
       BEQ.S   50$                 ;EQ => 'in room'
       MOVE.W  SUBJECT(A4),D2      ;NE => 'on inv'
       BRA.S   12$
50$
       TEST_W  SUBJECT(A4)         ;If 'in room' then are we an npc?
       BNE.S   13$                 ;yes => use room else are we sitting?
       MOVE.W  SEATED(A4),D2       ;if sitting use seat not room
       BNE.S   12$                 ;Yes, use seat
13$
       MOVE.W  ROOM(A4),D2         ;TARGET
12$
       DO	GETRELN	            ;look for nouns related to D2, ->A1
10$                            
       MOVE.W  (A1)+,D0            ;Get a noun
       BEQ.S   30$                 ;end of buffer (0.W)
       CMP.W   SUBJECT(A4),D0      ;NOT ALLOWED TO INCLUDE ITSELF
       BEQ.S   10$                 ;ignore
       GETINFO                     ;get noundata
       MOVE.W  6(A0),D1            ;grab location info and sec. locator
       TEST_B  D7 
       BEQ.S   70$                 ;GET
       CMP.W   SUBJECT2(A4),D1
       BNE.S   20$
       BRA.S   71$                 ;PASS
70$
       CLR.W   D2
       TEST_W  SUBJECT(A4)
       BNE.S   72$                 ;= IN ROOM
       TEST_W  SEATED(A4)
       BEQ.S   72$                 ;=IN ROOM
       MOVE.W  #$100,D2            ;ELSE RELATED
72$
       CMP.W   D2,D1               ;SAME RELATION?
       BNE.S   20$                 ;NO, FAIL
71$
    
       BTST    #0,5(A0)            ;PSEUDOS NEVER (!!!) ALLOWED
       BNE.S   20$
       MOVE.W  D0,D5
       DO	INCNOUN2            ;TO NOUNBUF
20$                              
       BRA.S   10$                 ;NEXT
30$
       MOVE.W  (A7)+,D0            ;Get back no. of nouns in phrase
       CMP.W   NOUN(A4),D0         ;Has it changed ?
       BNE.S   32$                 ;Yes - carry on
       BRA     P.NO.TNG            ;No - say 'Doesn't seem to be anything..'

EXIT.BAD

       ADDQ.L  #6,A7               ;Clean up stack
       DOGO	D72                 ;put D7 = 2  (return error)
 

***************************************
*
* DO.ALL.MULT
*
* Process stuff like 'look all keys'
*
* cleans saved A6.L, NOUN(A4).W off stack
* then calls BICHK and goes to MULT.WRD
*
* Idea is really to lose 'ALL' that we came across
*
***************************************

DO.ALL.MULT

       ADDQ.L #6,A7           ;Clean up stack
       CALL   BICHK           ;Emulate GetNounGroup stuff
       BRA    MULT.WRD        ;Get relevant nouns



*--------------------------------
*
*   Include contained nouns  - ALL IN xyzzy
*
*********************************

INCCONT
       
       DO	GET_CONT
       BCS.S   05$                 ;ERROR
04$
       MOVE.W  (A1)+,D5            ;IN LIST
       BEQ.S   05$
       BTST    #15,D5              ;VALID?
       BNE.S   04$
       DO	INCNOUN2            ;TO LIST
       BRA.S   04$
05$
       RET

*--------------------------------
*
* Include related nouns  - ALL ON|FROM xyzzy
*
*********************************


INCREL
       
       MOVE.W  D0,D2
 
       DO	GETRELN	            ;TO A1
10$
       MOVE.W  (A1)+,D0            ;GET ONE
       BEQ.S   90$                 ;DONE
       BMI.S   10$                 ;INCASE MASKED OUT
       GETINFO
       CMP.B   #1,6(A0)            ;MUST BE SIMPLE RELATED
       BNE.S   10$                 ;NO GOOD.
       MOVE.W  D0,D5
       DO	INCNOUN2
       BRA.S   10$                 ;NEXT
90$
       RET
       
*--------------------------------

       END


