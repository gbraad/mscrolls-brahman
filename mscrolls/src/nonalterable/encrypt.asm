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


SECTION		"ADV",CODE

*--------------------------------
* 
* ENCRYPT - EORS PICTURE READ CODE
*          BITS WI RANDOM NO.S
*
*--------------------------------

       XDEF    ENCRYPT,FRIG_1,FRIG_2,FRIG_3,FRIG_4,FRIG_5,FRIG_6
       XDEF    FRIG_7,FRIG_8,FRIG_9,CHK.FRIG

       XREF    RND.DATA,RND.CHIN,ENC_POS1,ENC_POS2,RNDGEN,INITRND
 
       
FRIG_1   DC.L  0
FRIG_2   DC.L  0
FRIG_3   DC.L  0
FRIG_4   DC.L  0
FRIG_5   DC.L  0
FRIG_6   DC.L  0
FRIG_7   DC.L  0
FRIG_8   DC.L  0
FRIG_9   DC.L  0

ENCRYPT
 
       MOVE.W  RND.CHIN(PC),-(A7)
       MOVE.L  RND.DATA(PC),-(A7)   ;SAVE THE OLD RND NO. DATA
       MOVE.L  RND.DATA+4(PC),-(A7)
       MOVE.W  RND.DATA+8(PC),-(A7)

       LEA     ENCRYPT(PC),A0
       MOVE.L  (A0)+,RND.DATA+6(A4)
       MOVE.L  (A0)+,RND.DATA(A4)
       MOVE.W  (A0)+,RND.CHIN(A4)
       MOVE.W  (A0)+,RND.DATA+4(A4)
 
       CALL    INITRND              ;RANDOMISE
       LEA     ENC_POS1(PC),A0      ;WHERE TO START
       MOVE.W  #ENC_POS2,D3         ;NO. OF WORDS TO DO
       SUB.W   #ENC_POS1,D3         ;All this is the linkers fault!
       LSR.W   #1,D3                ;DIV 2
       
10$
       CALL    RNDGEN               ;RND WORD - D0
       EOR.W   D0,(A0)+             ;ZAP A WORD
       DBRA    D3,10$               ;DO UNTIL END OF SAVED AREA
 
       MOVE.W  (A7)+,RND.DATA+8(A4) ;RECOVER OLD SEED ETC
       MOVE.L  (A7)+,RND.DATA+4(A4)
       MOVE.L  (A7)+,RND.DATA(A4)
       MOVE.W  (A7)+,RND.CHIN(A4)
 
       RET
 
*--------------------------------
*
* CHK.DO - CALCULATE CHECKSUMS
*
* RETURNS D1.W = ADDITION.W
*         D2.W = EOR.W
*         D3.W = MULT.W
*         D4.L = ADDITION.L
*         D6.W = NO. OF ZEROS
*
*--------------------------------
 
CHK.FRIG

       PUSH_L  D0/D2-D6/A0

       LEA     ENC_POS1(PC),A0
       MOVE.W  #ENC_POS3,D0       ;Calculate NO. OF WORDS
       SUB.W   #ENC_POS1,D0       ;Bloody stupid linker!
       LSR.W   #1,D0              ;DIV 2
                              
       MOVEQ   #0,D1
       MOVEQ   #0,D2
       MOVEQ   #1,D3              ;MULT!
       MOVEQ   #0,D4
       MOVEQ   #0,D6              ;COUNT ZEROS
 
       MOVEQ   #0,D5              ;SCRATCH
10$
       MOVE.W  (A0)+,D5           ;SAVE LOTS OF MEM ACCESS
       ADD.W   D5,D1              ;WORD ADD
       EOR.W   D5,D2              ;EOR
       ADD.L   D5,D4  
       TEST_W  D5                 ;ZERO?
       BEQ.S   20$
       MULU    D5,D3              ;MULT IF NOT ZERO
       SWAP    D3                 ;AVOID LOTS OF TRAILING ZEROS
       ADDQ.W  #1,D3              ;AVOID ZEROS
       BRA.S   30$
20$
       ADDQ.W  #1,D6              ;OK, ADD TO ZERO COUNT
30$
       DBRA    D0,10$             ;NEXT..
       ADD.L   D6,D1
       ADD.L   D5,D1
       ADD.L   D4,D1
       ADD.L   D3,D1
       ADD.L   D2,D1

       PULL_L  D0/D2-D6/A0
       RET

ENC_POS3

       END
