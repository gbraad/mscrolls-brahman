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
*	@(#)macros.asm	2.14
*	5/16/87
*
******************************************************

*--------------------------------
*
*  SRC.ADV.MACROS
*
*--------------------------------
 
*--------------------------------
*
* BHS - isnt a  legal opcode, so it will be soon...
*

BHS   MACRO
      BCC     \1
      ENDM

*--------------------------------
*
* ALIGN - PAD TO EVEN ADDRESS
*
*--------------------------------
 
ALIGN   MACRO
        CNOP   0,2
        ENDM
 
*--------------------------------
*
* PRINT  - DO A PRTTXT
*
*--------------------------------

PRINT   MACRO
        CALL  PRTTXT
        DC.B  \1
        DC.B  0
        ALIGN
        ENDM
 
*--------------------------------
*
* WORD - DO ASCII+00 TERM
*
*--------------------------------
 
WORD   MACRO 
       DC.B   \1,0
       ENDM

*--------------------------------
*
* SCORE - ADD TO SCORE (ONLY
*          DOES IT ONCE)
*
*--------------------------------

SCORE   MACRO
        CALL    DO.SCORE
        DC.W    SC.\1
        ENDM
 
*--------------------------------
*
* SCN - PRINT SCENARIO MSG
*
*      SCN    XYZ
*
*--------------------------------


BeSure	MACRO	* a label
	IFND	\1
	XREF	\1
	ENDC
	ENDM

	IFEQ	M68000

SCN	MACRO
	BeSure	MsgSCNBASE
	DC.W     $F000+MsgSCNBASE+\1
	ENDM

	ENDC

	IFNE	M68000

SCN	MACRO
	CALL	DO.SCN
	DC.W	\1
	ENDM
	
	ENDC

*--------------------------------

      IFEQ	M68000

NPCMSG	MACRO
	BeSure	MsgNPCBASE
	DC.W	$F000+MsgNPCBASE+\1
	ENDM

      ENDC

      IFNE	M68000

NPCMSG	MACRO
	IFND	DO.NPCMSG
	XREF	DO.NPCMSG
	ENDC
	CALL	DO.NPCMSG
	DC.W	\1
	ENDM
	
      ENDC

*--------------------------------
*
* MSG - PRINT PROGRAM MSG
*
*      MSG    XYZ   = CALLW.XYZ (FOR REAL 68K MACHINES)
*
*--------------------------------


	IFEQ	M68000

MSG	MACRO
	BeSure	M.\1
	DC.W	$F000+M.\1
	ENDM

	ENDC

	IFNE	M68000

MSG	MACRO
*	BeSure	W.\1
*	CALL	W.\1
	BeSure	M.\1
	BeSure	DO.MSG
	CALL	DO.MSG
	DC.W	M.\1
	ENDM

	ENDC
    
* MSGGO - instead og BRA W.xyzzy

	IFEQ	M68000

MSGGO	MACRO
	BeSure	M.\1
	DC.W	$F800+M.\1
	ENDM

	ENDC

	IFNE	M68000

MSGGO	MACRO
*	BRA	W.\1
	MSG	\1
	RET
	ENDM

	ENDC
    

*--------------------------------
*
* SETNE - CLEAR ZERO FLAG
*
*
*--------------------------------
       
SETNE   MACRO
        MOVE    #0,CCR             ;CLEAR ZERO FLAG
        ENDM

*--------------------------------
*
* SETEQ - SET ZERO FLAG
*
*
*--------------------------------
       
SETEQ   MACRO
        MOVE    #4,CCR             ;SET ZERO FLAG
        ENDM

*--------------------------------
*
* GETINFO - Macro equivilent of GETDATA
*
*--------------------------------

	IFEQ	FastBits

GETINFO  MACRO
         CALL     GETDATA
         ENDM

	ENDC

	IFNE	FastBits

GETINFO	 MACRO
	 DC.W	$A0FE
 	 ENDM

	ENDC



***********************************************************
*
* TProt  - see if they've knobbled our read disk bits
*
* usage:  TProt n
*
* where n between 1 and 9
*
************************************************************


TESTPROT MACRO

         IFNE     ABSOLUTELY_THE_LAST_LINK

         XREF     FRIG_\1,CHK.FRIG

         MOVE.L   D1,-(A7)
         JSR      CHK.FRIG(A4)
         CMP.L    #MAGIC_NORM,D1
         BEQ.S    TProt1\@

TProt0\@
         PULL_L   D0-D7/A0-A6       ;this should kill it

TProt1\@
         MOVE.L   #\1,FRIG_\1(A4)
         JSR      CHK.FRIG(A4)
         CMP.L    #MAGIC_FRIG\1,D1
         BNE.S    TProt0\@
         CLR.L    FRIG_\1(A4)
         MOVE.L   (A7)+,D1

         ENDC

         ENDM

BRAL	MACRO	* routine
	XREF	_\1
	JMP	_\1(A4)
	ENDM

BSRL	MACRO	* routine
	XREF	_\1
	JSR	_\1(A4)
	ENDM



* POPSP - used to lose a return address from stack, esp. in specials!
POPSP	MACRO
	ADDQ.W	#4,SP
	ENDM


******************************************
* MACROS used to make tables for
* the 'MessageFromTable' routine.
******************************************

MFT_Noun MACRO
	DC.W	N\1+HI
	ENDM

MFT_OtherNoun MACRO
	DC.W	N\1,N\2+HI
	ENDM

MFT_Scen MACRO
	DC.W	N\1,\2
	ENDM

MFT_End	MACRO
	DC.W	0
	ENDM

IfVerb	MACRO
	IFD	VN\1
	ENDM

*       
* SVERB is used to build lists of verb bytes. A byte is only laid down
* if the required verb is defined already.
* 

SVERB	MACRO

	IFD	VN\1		; if verb defined

	  DC.B	  VN\1

	ENDC

	ENDM


	IFEQ		M68000


DO	MACRO	* routine name
	BeSure	DO_Num_\1
	DC.W	$F000+DO_Num_\1
	ENDM

DOGO	MACRO	* routine name
	BeSure	DO_Num_\1
	DC.W	$F800+DO_Num_\1
	ENDM

RND_D1	MACRO
	DC.W	$A0F6
	ENDM

	ENDC

	IFNE	M68000

DO	MACRO
	CALL	\1
	ENDM

DOGO	MACRO
	BRA	\1
	ENDM
		
RND_D1	MACRO
	CALL	RND.D1
	ENDM

	ENDC


Connect	MACRO	* room1,room2,dir
	CALL	CONNECT3
	DC.B	RN\1,DIR_\3,RN\2,0
	ENDM
        
TST_BROKEN	MACRO
		BTST	#2,(A0)
		ENDM
TST_CLOSED	MACRO
		BTST	#5,(A0)
		ENDM
TST_CONTAINED	MACRO
		BTST	#5,6(A0)
		ENDM
TST_DEAD	MACRO
		BTST	#1,(A0)
		ENDM
TST_EXPLORED	MACRO
		BTST	#4,4(A0)
		ENDM
TST_HIDDEN	MACRO
		BTST	#1,6(A0)
		ENDM
TST_INITIALDONE	MACRO
		BTST	#1,5(A0)
		ENDM
TST_INSEPARABLE	MACRO
		BTST	#4,6(A0)
		ENDM
TST_INTRINSIC	MACRO
		BTST	#3,6(A0)
		ENDM
TST_ISAROOM	MACRO
		BTST	#6,6(A0)
		ENDM
TST_LIGHTABLE	MACRO
		BTST	#7,4(A0)
		ENDM
TST_LIT	MACRO
		BTST	#7,(A0)
		ENDM
TST_LITROOM	MACRO
		BTST	#2,5(A0)
		ENDM
TST_LOCALCONTENTS	MACRO
		BTST	#3,(A0)
		ENDM
TST_LOCKED	MACRO
		BTST	#6,(A0)
		ENDM
TST_MONEY	MACRO
		BTST	#3,4(A0)
		ENDM
TST_MOVEABLE	MACRO
		BTST	#4,3(A0)
		ENDM
TST_NPC	MACRO
		BTST	#6,4(A0)
		ENDM
TST_NPCREFUSES	MACRO
		BTST	#5,4(A0)
		ENDM
TST_OPEN	MACRO
		BTST	#4,(A0)
		ENDM
TST_OUT	MACRO
		BTST	#7,6(A0)
		ENDM
TST_PLURAL	MACRO
		BTST	#0,(A0)
		ENDM
TST_PSEUDO	MACRO
		BTST	#0,5(A0)
		ENDM
TST_SITTABLE	MACRO
		BTST	#3,5(A0)
		ENDM
TST_WET         MACRO
		BTST	#5,3(A0)
		ENDM
TST_WORNBYPLAYER	MACRO
		BTST	#2,6(A0)
		ENDM
TST_RELATED	MACRO
		BTST	#0,6(A0)
		ENDM

* this is for remaster

InfoAtScreen        EQU     1
InfoAtCommand       EQU     2
InfoDump            EQU     3
InfoHeartBeat       EQU     $ff    

InfoTrap  MACRO
          DC.W  $A0DC
          ENDM    

    
