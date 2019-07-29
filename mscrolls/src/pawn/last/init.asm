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
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.INIT
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"roomequ.asm"
	include	"nounequ1.asm"


	XDEF    INIT

      IFNE   RestartRAM

	XREF   SLADDR,INITG.E,NEWGAME

      ENDC

	XREF   RND.CHIN,RND.D1,RNDGEN,INITRND3,STWINDOW,ROOM,BIOS
	XREF   OUTMODE,SUBJECT,P.LOC,SCRPNT,LJPOS,POS,DO.SCN
	XREF   PEND2,NPCINRM,LINE,REALBAD,SCREEN,PRTTXT,INITRND
	XREF   D.NOUNS

	XREF	BIOSA4,INBUFF

INIT
*	LEA     BIOS(PC),A4
	CALL	BIOSA4

     IFEQ    M68000

	LEA     D.NOUNS-14(A4),A0	;noun data

	XREF	MessageCode,SpaceTable,MinSpaceEntry
        XREF    SLADDR,SLADDR.E,MsgMSGBASE,MsgSCNBASE

	LEA	SLADDR(A4),A1		;start of restart/save area
	LEA	SLADDR.E(A4),A2		;end of same
	SUB.L	A1,A2
	MOVE.L	A2,D0                   ; save_len

	LEA	MessageCode(PC),A3	;code to deal with MSG macros
	LEA	SpaceTable,A5		;for clever F-lines
	ADD.L	A4,A5
	MOVE.L	#MinSpaceEntry,D7	;F-lines =< this are messages
    MOVE.L  #MsgMSGBASE,D6
    MOVE.L  #MsgSCNBASE,D5

    ;; These are now used to pass clonedata and maxnoun
    ;;  used to be A6 and D5.
    SUB.L   A2,A2               ;clear

    ;; pass anyway!
*  	MOVE.W	#NMAXNOUN,D4        ; not for PAWN
    
	IFD	wanted_CLONES
	XREF	CLONEDATA
    LEA	CLONEDATA(A4),A2
	ENDC

	DC.W    $A0FD               ;Set base address of noun data - SETNOUNS

     ENDC

      IFNE    QL128

	CLR.B   $18063             ;SENSIBLE VIDEO
	
      IFNE     PROD
 
	MOVE    #0,CCR             ;KILL X BIT !
	MOVE.W  #0,RND.CHIN(A4)
	CALL    INITRND            ;RANDOMISE
	LEA     RND.D1(A4),A3      ;WHERE TO BEGIN
	LEA     $40000,A1          ;WHERE TO END   
10$  
	CALL    RNDGEN
11$
	MOVEQ   #$0F,D1    
12$
	EOR.W   D0,(A3)+   
	DBRA    D1,12$
	CMP.L   A3,A1
	BPL.S   10$  
	LEA     $10+20$/16*16(PC),A3
	LEA     INITRND3(A4),A1      ;INITRND-32
15$
	CALL    RNDGEN
16$
	MOVEQ   #$0F,D1
17$
	EOR.W   D0,(A3)+
	DBRA    D1,17$
	CMP.L   A3,A1
	BPL.S   15$  
20$
      ENDC
     ENDC
INIT2
     IFNE    RestartRAM          ;If RAM restartable then save modifiable bits

	LEA     SLADDR(PC),A0       ;for RESTART game
	LEA     NEWGAME,A1          ;Range errors!
	ADD.L   A4,A1
	LEA     INITG.E(PC),A2      ;End of initial game area excludes funct's
10$
	MOVE.B  (A0)+,(A1)+
	CMP.L   A0,A2
	BNE.S   10$

     ENDC

  
	CALL    STWINDOW
	MOVE.W  #RNSTART,ROOM(A4)   ;FIRST ROOM
	CLR.W   OUTMODE(A4)         ;SET TO NORMAL
	CLR.W   SUBJECT(A4)         ;TO PLAYER
	MOVEQ   #NONPATH,D0         ;FIRST ROOM
	CALL    P.LOC               ;LOOKS ODD WITHOUT
	MOVE.W  #1,SCRPNT(A4)
	CLR.W   LJPOS(A4)
	CLR.W   POS(A4)
	CALL    INITRND
	MOVE.W  #-1,PEND2(A4)
	MOVE.W  #-1,NPCINRM(A4)
        IFNE  Remastered

        ;; call dictionary lookup with a random word "A"
        ;; *before* generating initial text. This will initialise the back-end
      	LEA     INBUFF(A4),A6
        MOVE.L  A6,A0
        MOVE.B  #'A',(A0)+
        CLR.B   (A0)
	DO	SEARCH
        
        ENDC
 
 
	SCN     384                ;COPYRIGHT ETC
   
	SCN     179                ;INITIAL SPEIL
	SCN     180                ;..YAWN...
	CALL    LINE

     IFNE    PROD
	BRA     REALBAD            ;SET STACK + PUSH SCREEN ADDR
     ENDC


     IFEQ    PROD
	BRA     SCREEN
     ENDC

	END

