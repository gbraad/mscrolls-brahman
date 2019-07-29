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
*	@(#)pictcode.asm	2.10
*	8/25/87
*
******************************************************

		SECTION		"ADV",CODE

************************************************
*
*  PICTCODE  module to deal with all the code
*            needed to suss whether a picture
*            is required in this room
*
* Called from SCREEN when printing a room description
*
* Bits dealing wi the GRAPHICS toggle are in DEBUGBIT module
* stuff relating to C64 (cameos!!) happens in c64/.../cameos.asm
*
***********************************************


	include	"equates.asm"
	include	"macros.asm"

	XREF	ROOM,GRAPHICS,CHAROUT,LINE,PRTTXT
	XREF	PICTNUM,LOAD_PIC,PICTURE_INDEX,OLDPICTNUM
    XREF    REMASTER

	XDEF	PICTURES


*---------------------------------
*
* PICTURES - called from SCREEN,LOOK (or P.DESC?)
*
*---------------------------------

REGLIST  REG   D0-D7/A0-A6          ;Must be reduced


PICTURES

	TEST_B   GRAPHICS(A4)         ;Does the punter want pictures?
	BEQ.S    99$                  ;EQ => no

	MOVEM.L  REGLIST,-(A7)

	LEA      PICTURE_INDEX(PC),A0 ;For the time being we'll use
	MOVE.W   ROOM(A4),D0          ;an array of picture no.s indexed by
	                             ;room no. Maybe change this to be an
	                             ;array of pointers to the start of the
	IFNE	THE_PAWN
	MOVE.B   -1(A0,D0.W),D0       ;pictures on the disk itself.
    CLR.W   D1                    ; version=0
*    BRA     80$                   ;draw pic D0 (if zero, no pic).
	ENDC

	 IFEQ	THE_PAWN

* From Jinxter onwards the pictures work as 3 bytes:
* the first is picture number the second the room number
* the third is the version, 0=> original
* on a -1 terminated list.
* 
10$
    MOVE.W  (A0),D1         ;-1 terminated
    BMI.S   20$            ;no picture
    ADDQ.L  #3,A0
    CMP.B   D0,D1           ; is the room the same?
    BNE.S   10$
    MOVE.B  -1(A0),D1               ;version
    TEST_B  D1                      ;version 0 = classic
    BEQ.S   30$                     ; accept
    CMP.B   REMASTER(A4),D1         ; otherwise version matches
    BNE.S   10$
30$
    MOVE.B  -3(A0),D0       ; D0 = picture number, D1 = version
    BRA.S   80$
20$        
    CLR.W   D0                      ;no pic, picnum = 0
    ENDC

80$
    CALL_S   DRAWPIC              ;Display the picture in D0.B, Version D1.B
    MOVEM.L  (A7)+,REGLIST
99$
    RET

    XDEF    DRAWPIC
    XREF    SP.PICTURES

DRAWPIC

    ;;  D0 = picture# (1-based, but can also be zero)
    ;;  D1 = version

    CALL    SP.PICTURES     ;can change d0/d1

    CMP.B    PICTNUM(A4),D0       ;No point in redrawing same picture
    BEQ.S    90$
        
    TEST_B  D0              ; picture?
    BNE.S   10$             ;yes
    CLR.B   PICTNUM(A4)
    MOVEQ   #0,D1          ;signal hide pic
    EXT.W   D0
    BRA.S   80$         ; load null => hide
10$        
    MOVE.B   PICTNUM(A4),OLDPICTNUM(A4) ;so loadpic can put back if err
    MOVE.B   D0,PICTNUM(A4)
    EXT.W    D0
    SUBQ.W   #1,D0          ; adjust picture# to be 0 based
    MOVE.B   D1,D2          ; d2 = picture version
    MOVEQ    #1,D1          ;non 68k (z80 mainly) load_pic param
80$        
    CALL     LOAD_PIC
90$
    RET

*---------------------------------

    XDEF GetPictureVersion

GetPictureVersion

* look up picture number D0 (zero based)
* return version number in D2
* if picture number does not exist, D2 = -1

    MOVE.L   A0,-(A7)
    LEA      PICTURE_INDEX(PC),A0
10$
    MOVE.B  (A0),D2        ;get pic#, -1 terminated
    BMI.S   20$            ;done, return -1
    ADDQ.L  #3,A0          ;next slot
    SUBQ.B  #1,D2          ;make zero-based
    CMP.B   D0,D2          ;is our number?
    BNE.S   10$            ;no
    MOVE.B  -1(A0),D2      ;version
    TEST_B  D2             ;version 0 = classic
    BEQ.S   20$            ;accept
    CMP.B   REMASTER(A4),D2 ; else accept version that matches remaster
    BNE.S   10$
20$
    MOVE.L  (A7)+,A0
    RET               

 
*---------------------------------

	END
