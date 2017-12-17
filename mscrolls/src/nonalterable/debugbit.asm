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

*--------------------------------
*
* DEBUGBITS
*
*--------------------------------

  
	SECTION  "ADV",CODE

	include  "equates.asm"
	include  "verbequ.asm"
	include  "macros.asm"
	include  "scen.i"

	XREF   MSGPNTR,UNDOHINT
        XREF   SP.DEATH

   IFNE  BookProtection
	XREF    PROTECT2
   ENDC

   IFNE  SpeechAvail
	XREF   SpeechOn
   ENDC


   IFEQ	QL128
	XREF   READFILE,WRITEFIL
   ENDC

   IFNE  Graphics
	XREF    CWID_FLG,GRAPHICS,PICTURES,LOAD_PIC

    IFNE  Mono_Possible
	 XREF   SWHEIGHT,MAKESTIP,MAKEDITH,MAKEFREE
	 XREF   DITH_FLG,COL_FLG,STIP_FLG,FREE_FLG
    ENDC
   ENDC

   IFNE  Wimp!IBM
	XREF   SCR_IGNO
   ENDC
   
   IFNE	Wimp
	XREF	DO_FOREG,DO_BACKG,DO_CONTR,DO_SCROL
    

      IFEQ   MAC512
	XREF    GRA_EXIT,MS_END
      ENDC
   ENDC

   IFNE  SpoolLen
	XREF   PRINTER
   ENDC

   IFNE  RestartRAM
	XREF   NEWGAME
   ENDC

   IFNE  FunctionKeys
	XREF   FUNCBASE
   ENDC

   IFEQ  DosPresent
	XREF   READIT,FILESIZE,DEST,FILENO,DRIVE,BIOSQUIT,WRITEIT
   ENDC

   IFNE  ST520
	XREF    CONTERM
   ENDC

   IFNE  AMIGA512
	XREF    AD_COUT,NewCLIST,OldCLIST
   ENDC

	XREF   INPUT,INBUFF,INITG.E,P.LOC,P.NOUN,DISPTYPE,CHAROUT,INIT
	XREF   POSWRD,GETDATA,FNDWRD,GETWRD,MATCHBUF,LINE,PRTTXT,SC70
	XREF   STWINDOW,SUM.CHK,SLADDR,SLADDR.E,CHANNEL,RND.CHIN
	XREF   RND.DATA,RNDGEN,INITRND,SLGAMENO,CHAR.SUB,LEFTM,DISP.SV
	XREF   LEFTM.SV,SWIDTH,VWIDTH
	XREF   P.DESC,DIED,USER.DB,SCREEN,W.LETOFF,BIOSA6,BIOS,DBUG.ON
	XREF   W.ENSURE,W.QUIT,PRTNUM,SCORE_,OUTMODE,R.NOUN,R.NOUN2,W.OFF
	XREF   GETVECT,W.BADW,W.HER,W.HIM,P.LIST,P.TN,PRONHIM,PRONHER,W.ON
	XREF   OLDBUF,W.IT,P.SCN,P.MSG,SPACE,ROOM,RIGHTM,PRTHEX,WIDTH
	XREF   SLSIZE2,SLSIZE20,W.PDR2,W.NOCHAN,W.THEM,BEGLNK,VERB,SEARCH,OK
	XREF   PHEXBYTE,PHEXWORD,OLDTEXT,W.USURE,GETCHAR,W.DEADED
	XREF   ADJ.TBL,DO.SCN,TBL.WRD,LJPOS,LAST.CH2,SEARCH2,NETWORK,NET_END
	XREF	P.EXITS

      IFNE	YCHEAT
	XREF	CHEAT
      ENDC

	XDEF   DEBUGBIT,DO.SING,DO.DEATH,Switch,DO.QUIT
	XDEF	UTEXIT,DBEXIT,RUSURE

DO.SING
DEBUGBIT

   IFNE    YCHEAT
	CMP.B   #Adj_word_num,D5              ;ADJ
	BEQ.S   04$    
   ENDC

	DO	FNDWRD              ;GET A6= START OF NUM
	MOVE.L  A6,POSWRD(A4)       ;SAVE FOR LATER!
	CALL    GETHEX
	MOVE.L  D6,D0
04$
	
	LSL.W   #1,D5
	LEA     DBLIST(PC),A0
	MOVE.W  0(A0,D5.W),D5
	JMP     0(A0,D5.W)
05$
	DOGO	DBEXIT
 
   IFNE    YCHEAT
 
HEXLIST
 
	DC.B    '0123456789ABCDEF'
 
GETHEX
	MOVE.L	D5,-(A7)		;used as a space flag
	MOVEQ	#0,D6
	MOVE.L	D6,D5
10$
	MOVE.B  (A6)+,D1
	CMP.B   #$40,D1
	BMI.S   15$
	AND.B   #$5F,D1             ;Lower case it if ness.
15$
      IFNE	TimeCode
	CMP.B	#' ',D1
	BNE.S	17$
	BSET	#0,D5
	BEQ.S	10$			;forget one space
      ENDC
17$
	LEA     HEXLIST(PC),A0
	MOVEQ   #15,D0
20$
	CMP.B   0(A0,D0),D1
	DBEQ    D0,20$
	TEST_W  D0
	BMI.S   30$                 ;END OF HEX
	LSL.L   #4,D6   
	OR.B    D0,D6    
	MOVEQ	#0,D5			;fod space's flag
	BRA.S   10$ 
30$
	SUBQ.L  #1,A6
	MOVE.L	(A7)+,D5
	RET
 
   ENDC

      IFEQ    YCHEAT
 
* IF NOT YCHEAT THEN 'GETHEX' GETS DEC!!!!

MUL10	MACRO
	LSL.W	#1,\1
	MOVE.W  \1,-(A7)
	LSL.W   #2,\1
	ADD.W   (A7)+,\1
	ENDM

 
GETHEX

      IFNE	M68000
 
	MOVE.B  (A6)+,D6              ;GET FIRST BYTE
	MOVE.B  (A6)+,D1              ;GET SECOND BYTE
	MOVE.B  (A6)+,D2              ;bloody rg AND b
	CMP.B   #$30,D6
	BLT.S   80$                   ;EXIT RESULT=0
	CMP.B   #$39,D6
	BGT.S   80$
	AND.W   #$F,D6
	CMP.B   #$30,D1
	BLT.S   10$
	CMP.B   #$39,D1
	BGT.S   10$
	AND.W   #$F,D1
	CMP.B   #$30,D2
	BLT.S   05$
	CMP.B   #$39,D2
	BGT.S   05$
	AND.W   #$F,D2
	MUL10	D6
	MUL10	D1
	BRA.S   06$
05$
	MOVEQ   #0,D2
06$
	MUL10	D6
	ADD.W   D1,D6
	ADD.W   D2,D6
	RET
10$
	AND.W   #$0F,D6
	RET
80$ 
   ENDC
	RET
	
	
   ENDC
  
DBLIST
	 DC.W    UTNORMAL-DBLIST
	 DC.W    UTVERBOSE-DBLIST
	 DC.W    UTBRIEF-DBLIST
	 DC.W    UTVERSION-DBLIST
	 DC.W    UTSAVE-DBLIST
	 DC.W    UTQUIT-DBLIST
	 DC.W    UTSCORE-DBLIST
	 DC.W    UTRESTORE-DBLIST
	 DC.W    UTRESTORE-DBLIST
	 DC.W    UTRESTART-DBLIST
	 DC.W    UTSERIAL-DBLIST
	 DC.W    UTID-DBLIST
 
    IFNE     YCHEAT
  	 DC.W    DBWHATIS-DBLIST
	 DC.W    DBSG-DBLIST
	 DC.W    DBWHEREIS-DBLIST
	 DC.W    DBZAP-DBLIST
	 DC.W    DBGOTO-DBLIST
	 DC.W    DBTELE-DBLIST
	 DC.W    DBNOUN-DBLIST
	 DC.W    DBMSG-DBLIST
	 DC.W    DBSCN-DBLIST
      ENDC
  
	 DC.W    DBPN-DBLIST
	 DC.W    DBPN-DBLIST          ;INSTEAD OF 'REAL' SYNONYM
  
    IFNE     YCHEAT
Adj_word_num EQU (*-DBLIST)/2
	 DC.W    DBADJ-DBLIST
    ENDC
	
    IFNE	M68000
	 DC.W    DBLEFT-DBLIST
	 DC.W    DBRIGHT-DBLIST
    ENDC
	
    IFNE     YCHEAT
	 DC.W    DBVIDEO-DBLIST
	 DC.W    DBSUSN-DBLIST
	 DC.W    DBSUSR-DBLIST
	 DC.W    DBSUSCR-DBLIST
    ENDC
    
    IFNE	M68000
	 DC.W    DBCONTRAST-DBLIST
    ENDC

    IFNE     YCHEAT
	 DC.W    DBWORD-DBLIST
	 DC.W    DBMEM-DBLIST
	 DC.W    DBSET-DBLIST
    ENDC

      IFNE      AMIGA512&YCHEAT
	 DC.W    DBBPON-DBLIST
	 DC.W    DBBPOFF-DBLIST
	 DC.W    DBMISC-DBLIST
      ENDC

    IFNE     Graphics
	DC.W    UTGRAPHICS-DBLIST
        
      IFNE    YCHEAT
	 DC.W    DBPICTURE-DBLIST
      ENDC

      IFNE	AMIGA512!ST520
	 DC.W    UTSWHEIGHT-DBLIST
      ENDC

      IFNE    Mono_Possible
	 DC.W    UTMAKESTIP-DBLIST
	 DC.W    UTMAKEDITH-DBLIST
	 DC.W    UTMAKEFREE-DBLIST
      ENDC

    ENDC

    IFNE     SpoolLen
	 DC.W    UTPRINTER-DBLIST
    ENDC

    IFNE     FunctionKeys
	 DC.W    UTFUNCT-DBLIST
	 DC.W    UTFUNCT-DBLIST            ;less hassle than synonym
    ENDC

	 DC.W    UTCREDIT-DBLIST
	 DC.W    UTHINT-DBLIST

    IFNE     Wimp                         ;poor definition
	 DC.W    UTFOREGROUND-DBLIST
	 DC.W    UTBACKGROUND-DBLIST
	 DC.W    UTSCROLLCOLOUR-DBLIST
    ENDC

    IFNE     YCHEAT
	 DC.W    DBCOUNT-DBLIST
    ENDC

    IFNE     SpeechAvail
	 DC.W    UTSPEECH-DBLIST
    ENDC


    IFNE	PCW8256				;hmmmmm
	DC.W	PCWSTIPPLE-DBLIST
	DC.W	PCWDITHER-DBLIST
    ENDC

    IFNE	IBM
	DC.W	IBMTITLE-DBLIST
	DC.W	IBMMORE-DBLIST
	DC.W	IBMPICTURE-DBLIST
	DC.W	IBMTEXT-DBLIST
	DC.W	IBMFASTER-DBLIST
	DC.W	IBMSLOWER-DBLIST
    ENDC


    IFNE	YCHEAT
	DC.W	DBSYN-DBLIST
	DC.W	DBVNET-DBLIST
    ENDC

    IFD	C64Pict
	DC.W	DBC64Pict-DBLIST
    ENDC


    IFNE	IBM
	XREF	IsTextOnly,GET_TITLE,P.IDFY,LOAD_PIC2

IBMMORE

	LEA	INBUFF(A4),A6
	CALL	GETWRD			;find word# of MORE
	
	MOVE.W	D5,-(A7)		;cos we know picture,text follow MORE
	CALL	GETWRD			;in wtable
	SUB.W	(A7)+,D5		;so 1=>picture 2=>text
	CMP.W	#SECT.CONT,D6		;is it a control word?
	BNE.S	90$
	CMP.W	#1,D5			;EQ=> picture
	BNE.S	10$
	MOVEQ	#4,D2
	BRA.S	IBM_DOIT
10$
	CMP.W	#2,D5			;EQ=> text
	BNE.S	90$			;NE => what?
	MOVEQ	#2,D2
	BRA.S	IBM_DOIT
90$
	CALL_S	CheckTextOnly
	CALL	P.IDFY
	BRA	UTEXIT


IBMPICTURE

	MOVEQ	#3,D2
	BRA.S	IBM_DOIT

IBMTEXT

	MOVEQ	#1,D2
	BRA.S	IBM_DOIT

IBMFASTER

	MOVEQ	#5,D2
	BRA.S	IBM_DOIT

IBMSLOWER

	MOVEQ	#6,D2

IBM_DOIT

	CALL_S	CheckTextOnly
	CALL	LOAD_PIC2
	BRA	UTEXIT


IBMTITLE
	
	CALL_S	CheckTextOnly
	CALL	GET_TITLE
	BRA	UTEXIT
    
CheckTextOnly

	TEST_B	IsTextOnly(A4)
	BNE.S	10$			;EQ => looks like an EGA version
	RET
10$
      IFNE	THE_PAWN
	PRINT	<'This is a text-only version. Please refer to the packaging '>
	PRINT	<'for details of hardware requirements and installation '>
	PRINT	<'procedures for graphics versions.^'>
      ENDC

      IFEQ	THE_PAWN
	MSG	TEXTONLY
      ENDC

	ADDQ.W	#4,A7			;POPSP
	BRA	UTEXIT

    ENDC

    IFNE	PCW8256

* hacks for 'semi-mono' (ie no freehand) PCW8256 machine

	XREF	DitherFlag,PICTNUM

PCWSTIPPLE

	ST	GRAPHICS(A4)
	BCLR	#0,DitherFlag(A4)
	BEQ.S	10$
	ST	PICTNUM(A4)
	CALL	PICTURES
10$
	MSG	PCWSTIPPLE		;PRINT	<'Using stippled pictures.^'>
	DOGO	DBEXIT
	
PCWDITHER

	ST	GRAPHICS(A4)
	BSET	#0,DitherFlag(A4)
	BNE.S	10$
	ST	PICTNUM(A4)
	CALL	PICTURES
10$
	MSG	DITHER		;PRINT	<'Using dithered pictures.^'>
	DOGO	DBEXIT
	

    ENDC

*

*
* Switch - sets a toggle byte according to next word in input stream
*
* if word = 'off' sets to zero
*           'on' sets to FF
* if none changes state (also if a random word)
*
* On entry A0 points at the flag byte
* 
* On exit D0 stomped D5/D6 reflect word/section
*

Switch

	MOVE.L	POSWRD(A4),A6
	PUSH_L 	D0-D4/A0-A5	;carefull - search WILL fod A0!
	DO	SEARCH
	PULL_L 	D0-D4/A0-A5
	CMP.W	#SECT.CONJ,D6
	BNE.S	Toggle
	CMP.W	#ON,D5
	BEQ.S	10$
	CMP.W	#OFF,D5
	BNE.S	Toggle
	MOVEQ	#0,D0
	BRA.S	20$		;do
10$
	MOVEQ	#-1,D0
20$
	MOVE.B	D0,(A0)
	RET
    
Toggle
	NOT.B	(A0)
	RET



    IFNE       SpeechAvail

UTSPEECH

	IFD	Amiga_Demo_PCW

	 PRINT	  <'The noise here is much too loud - sorry no speech!!^'>

	 DOGO	UTEXIT

	ENDC

	PEA      UTEXIT(PC)
	LEA      SpeechOn(A4),A0
	CALL	Switch
	PRINT    <'^Speech '>
	TEST_B   SpeechOn(A4)
	BEQ.S	10$
	MSGGO	ON
10$
	MSGGO	OFF


    ENDC


    IFNE    YCHEAT

*
* DBCOUNT - returns no. of sections and words in the current word table
*

DBCOUNT

	CALL    TST.DBUG
	LEA     TBL.WRD(A4),A0
	MOVEQ   #0,D5             ;word count
	MOVEQ   #0,D6             ;section count
	MOVEQ	#0,D4	
	MOVEQ	#0,D7
10$
	MOVE.B  (A0)+,D0          ;find a hibit (word terminator)
	BPL.S   10$
	CMP.B   #$81,D0           ;end of table?
	BEQ.S   90$
	CMP.B   #$82,D0           ;end of section?
	BNE.S   20$
15$
	ADDQ.L  #1,D6             ;up the section count
	PUSH_L	D5/D6
	SUB.L	D4,D5
	PRINT	<'^Section # '>
	PUSH_L	D0-D7/A0-A6
	SUBQ.L	#1,D6
	CALL	PRTHEX
	PULL_L	D0-D7/A0-A6
	PRINT	<' words = '>
	MOVE.L	D5,D6
	PUSH_L	D0-D7/A0-A6
	CALL	PRTHEX
	PULL_L	D0-D7/A0-A6
	PULL_L	D5/D6
	MOVE.L	D5,D4

	BRA     10$
20$
	ADDQ.L  #1,D5             ;up word count
	BRA     10$
90$

      IFEQ	THE_PAWN

	BSET	#0,D7		;two table terminators
	BEQ.S	15$

      ENDC

	PRINT   <'^SECTIONS = '>
	MOVE.W  D5,-(A7)
	CALL    PHEXBYTE
	MOVE.W  (A7)+,D6
	PRINT   <'^WORDS = '>
	CALL    PHEXWORD
	DOGO	DBEXIT


      ENDC


TST.DBUG
 
    IFNE     YCHEAT
 
	TEST_W  DBUG.ON(A4)        ;AUTHORISED USER OF DBUGBITS?
	BNE.S   90$                ;YES = OK
 
	PRINT   <'NO'>
	MOVE.L  (A7)+,D0
	DOGO	DBEXIT
	  
90$
    ENDC
	
	RET
 
 
    IFNE     YCHEAT
	 
DBWHATIS
 
	CALL    TST.DBUG           ;ARE THEY ALLOWED???
	GETINFO        
	DO	P.TN
	DOGO	DBEXIT
 
DBSG
 
	CALL    TST.DBUG
	GETINFO        
	BTST    #6,6(A0)            ;ROOM???
	BNE.S   10$
	CLR.L   6(A0)
	PRINT   <'OK..^'>
05$
	DO	P.TN
	DOGO	DBEXIT
 
10$
	MSG     NOCHAN
	BRA.S   05$
 
DBWHEREIS
	
    IFNE     0
  
	CALL    TST.DBUG
	GETINFO        
	MOVE.W  6(A0),D1
	BNE.S   10$
	MOVE.W  8(A0),D2
	BEQ.S   50$
	DO	R.NOUN2
	BRA     DBWHATIS
10$
	BTST    #7,6(A0)
	BEQ.S   20$
	PRINT   <'OUT'>
	BRA.S   99$
 
50$
	PRINT   <'INVENT!'>
	BRA     DBWHATIS
20$
	BTST    #6,6(A0)
	BEQ.S   30$
	PRINT   <'IT IS - '>
   
	MOVE.W  8(A0),D0
	BRA     DBWHATIS 
30$            
	PRINT   <'ATT TO '>
	MOVE.W  8(A0),D0
	BRA     DBWHATIS    
40$
	PRINT   <'PSEUDO'>
99$
 
    ENDC
	
	DOGO	DBEXIT
	
 
DBGOTO
 
	CALL    TST.DBUG
	GETINFO        
	BTST    #6,6(A0)            ;ROOM?
	BEQ     DBWHATIS 
	PRINT   <'PUFF..'>
	MOVE.W  8(A0),ROOM(A4)
	DOGO	DBEXIT
 
DBZAP
 
	CALL    TST.DBUG
	MOVE.W  D0,-(A7)
	DO	FNDWRD
	CALL    GETHEX
	MOVE.W  D6,-(A7)
	DO	FNDWRD
	CALL    GETHEX
	MOVE.W  (A7)+,D2
	MOVE.W  (A7)+,D0
	GETINFO        
	ADD.W   D2,A0
	MOVE.B  D6,(A0)
	BRA     DBWHATIS
 
    ENDC


*--------------------------------
*
*  SRC.ADV.DEBUGBITS2
*
*--------------------------------
 
  
    IFNE     YCHEAT
 
 
DBTELE
 
	CALL    TST.DBUG
	TEST_W  D0
	BNE.S	10$
	MOVE.L	POSWRD(A4),A6
	DO	SEARCH
	MOVE.L	#-1,(A2)
	LEA	MATCHBUF(A4),A2
	CMP.W	#SECT.NOUN,(A2)+
	BNE.S	20$			;ouch!
	MOVE.W	(A2)+,D0
	ADDQ.W	#1,D0
	GETINFO
	BTST	#6,6(A0)		;room?
	BEQ.S	20$
	MOVE.W	8(A0),D0
	PRINT	<'Puff..^'>
10$
	MOVE.W	D0,ROOM(A4)
20$
	DO	R.NOUN
	BRA     DBWHATIS
 
 
DBNOUN
 
	CALL    TST.DBUG
	MOVE.W  D0,D1
	CALL    P.DESC
	MSG    SPACE
	MSG    SPACE
 
	BRA     DBWHATIS            ;BACK TO SCREEN
 
DBMSG
 
	CALL    TST.DBUG
	MOVE.W  D0,D1
	DO	P.MSG
	DOGO	DBEXIT
 
DBSCN
 
	CALL    TST.DBUG
	MOVE.W  D0,D1
	DO	P.SCN
	DOGO	DBEXIT
  
    ENDC
	
 
 
DBPN
	MSG	LINE
	MSG     IT
	CALL_S  DBDASH
	MOVE.W  OLDBUF(A4),D0
	BEQ.S   10$
	DO	P.TN
10$
	MSG	LINE
	MSG     THEM
	CALL_S  DBDASH
	LEA     OLDBUF(A4),A0
	TEST_W  (A0)
	BEQ.S   20$
	DO	P.LIST
20$
	MSG	LINE
	MSG     HIM
	CALL_S  DBDASH
	MOVE.W  PRONHIM(A4),D0
	BEQ.S   30$
	DO	P.TN
30$
	MSG	LINE
	MSG     HER
	CALL_S  DBDASH
	MOVE.W  PRONHER(A4),D0
	BEQ.S   40$
	DO	P.TN
40$
	MSG	LINE  
 
 
DBEXIT

	MOVE.L  (A7)+,D0            ;POP ONCE..
	BRA     SC70                ;AND GO.W/O CLOCKING
    
DBDASH
	
	PRINT   <' - '>
	RET
  
	
    IFNE    YCHEAT
	
 
*--------------------------------
 
	XDEF     DBADJ

DBADJ
 
	CALL    TST.DBUG
	DO	GETWRD
	TEST_W  D7
	BEQ.S   10$
	PRINT   <'EOS!'>
05$
	DOGO	DBEXIT
10$
	CMP.W   #SECT.ADJ,D6
	BEQ.S   20$
	PRINT   <'NOT ADJ!'>
	BRA.S   05$                 ;EXIT
20$
	ADDQ.W  #4,D5               ;NUMBERS FUDGED
	LEA     ADJ.TBL(A4),A1
	MOVEQ   #1,D0               ;NOUN #
30$
	MOVE.B  (A1)+,D1            ;GET ADJ#
	BEQ.S   50$                 ;NEXT NOUN#
	CMP.B   #-1,D1              ;END OF TABLE?
	BEQ.S   90$
	CMP.B   D1,D5 
	BNE.S   30$
	GETINFO        
	DO	P.TN
	MSG    SPACE 
	MOVE.L  D0,D6
	PUSH_L  D0-D7/A0-A6         ;HMM,....
	CALL    PRTHEX
	PULL_L  D0-D7/A0-A6
	MSG	LINE
	BRA.S   30$
50$
	ADDQ.W  #1,D0
	BRA.S   30$
90$
	BRA.S   05$

    ENDC

    IFNE	M68000

DBLEFT 
 
	MOVE.W  RIGHTM(A4),D1
	BRA.S   DBWIDTH

DBRIGHT

   IFNE    ST520

	MOVEQ   #76,D1

   ENDC

   IFNE    QL128

	MOVEQ   #60,D1

   ENDC

	SUB.W   D0,D1                 ;SET UP RIGHT MARGIN
	MOVE.W  LEFTM(A4),D0          ;GET LEFT MARGIN

DBWIDTH

      IFNE     QL128

	MOVE.W  D1,D2                 ;COPY RIGHT MARGIN
	SUB.W   D0,D2                 ;CALC WIDTH
	CMP.W   #$10,D2               ;BIG ENOUGH?
	BPL.S   10$                   ;OK
	MSG     BADW                  ;BAD WIDTH
	BRA.S   90$
10$
	MOVE.W  D2,WIDTH(A4)
	ADDQ.W  #2,D2
	MOVE.W  D2,SWIDTH(A4)
	ADDQ.W  #1,D2
	MOVE.W  D2,VWIDTH(A4)
	MOVE.W  D0,LEFTM(A4)
	MOVE.W  D1,RIGHTM(A4)
	CALL    STWINDOW
90$

   ENDC

	DOGO	DBEXIT

    ENDC

    IFEQ    MAC512
    IFNE    Wimp                          ;must suss better def..

UTFOREGROUND

	CALL    DO_FOREG
	DOGO     UTEXIT

UTBACKGROUND

	CALL    DO_BACKG
	DOGO     UTEXIT

UTSCROLLCOLOUR

	CALL    DO_SCROL
	DOGO     UTEXIT

    ENDC
    ENDC

    IFNE	MAC512

UTFOREGROUND
UTBACKGROUND
UTSCROLLCOLOUR

	DOGO	UTEXIT

    ENDC

    IFNE	M68000			;rael tack definiti

DBCONTRAST

    IFNE    ST520  ; Wimp                          ;not a good definition.....

	CALL    DO_CONTR

    ENDC

    IFEQ    Wimp

	NOT.B   DISPTYPE(A4)

    ENDC

	DOGO	DBEXIT

    ENDC

    IFNE     YCHEAT

DBVIDEO

   IFNE     QL128
	CALL    TST.DBUG
	MOVEQ   #0,D1
	BCHG    #0,20$          ;!! URK !!!!
	BNE.S   10$
	MOVEQ   #-$80,D1
10$
	MOVE.B  D1,$18063       ;QL video bit
   ENDC

	DOGO	DBEXIT

   IFNE  QL128
20$
	DS.B     2
   ENDC


DBSUSN 
	CALL    TST.DBUG
	GETINFO                    ;SUSS NOUN #
	MOVE.W  #VNSUSS,D2
	MOVE.W  D2,VERB(A4)
	DO	GETVECT
	JSR     (A1)
	DOGO	DBEXIT

DBSUSCR 
 
	MOVE.W  ROOM(A4),D0        ;SUSS CURRENT ROOM
DBSUSR
	MOVE.W  D0,D2              ;SUSS ROOM #
	DO	R.NOUN2
	BRA     DBSUSN

*
* verify network - (Really exits but by saying 'what rooms come here?')
*

DBVNET

	MOVE.W	ROOM(A4),D7		;room we're in NOW

	LEA	NETWORK(A4),A0
	LEA	NET_END(A4),A1
	MOVEQ	#1,D0			;rooms start at 1
10$
	MOVEQ	#Sizeof_NET-1,D1	;no of exits/room (DBRA'ed)
20$
	CMP.B	(A0)+,D7
	BEQ.S	30$
	DBRA	D1,20$
25$
	ADDQ.W	#1,D0			;keep track of room#
	CMP.L	A0,A1			;finished?
	BNE.S	10$

	MOVE.W	CHEAT(A4),-(A7)
	ST	CHEAT(A4)
	CALL	P.EXITS			;list exits from room
	MOVE.W	(A7)+,CHEAT(A4)	

	DOGO	DBEXIT			;yes, bye
30$
	PUSH_L	D0-D1/D7/A0-A1		;overkill
	MOVE.W	D0,D6
	CALL	PRTHEX
	MSG	SPACE
	MOVE.W	D0,D2
	DO	R.NOUN2
	DO	P.TN
	MSG	LINE
	PULL_L	D0-D1/D7/A0-A1
40$
	SUBQ.W	#1,D1
	BMI.S	25$
50$
	TEST_B	(A0)+
	DBRA	D1,50$
	BRA.S	25$



DBSYN
	CALL    TST.DBUG
	MOVE.L  POSWRD(A4),A6
	DO	SEARCH
	MOVE.W  #-1,(A2)
	LEA     MATCHBUF(PC),A2
10$
	MOVE.W  (A2)+,D6
	BMI.S   20$
	PRINT   <'^SECT #'>
	CALL    PHEXBYTE
	MOVE.W  (A2)+,D6
	BMI.S   20$
	PRINT   <'   WORD #'>
	CALL    PHEXWORD
	PRINT	<' :  '>

	MOVE.W	-2(A2),D6		;word#
	LSL.W	#5,D6
	OR.W	-4(A2),D6		;section#


	XREF	SYN_L_Len,SYN_L,SynText,P.TAB

	MOVE.W	#SYN_L_Len,D0
	LEA	SYN_L(A4),A0
	LEA	SynText(A4),A1
	MOVEQ	#0,D1
11$
	CMP.W	(A0)+,D6
	BNE.S	12$
	PUSH_L	A1/D1
	CALL	P.TAB
	PULL_L	A1/D1
12$
	ADDQ.W	#1,D1
	DBRA	D0,11$

	BRA.S   10$
20$
	DOGO	DBEXIT




DBWORD
	CALL    TST.DBUG
	MOVE.L  POSWRD(A4),A6
	DO	SEARCH
	MOVE.W  #-1,(A2)
	LEA     MATCHBUF(PC),A2
10$
	MSG	LINE
	MOVE.W  (A2)+,D6
	BMI.S   20$
	PRINT   <'SECT #'>
	CALL    PHEXBYTE
	MOVE.W  (A2)+,D6
	BMI.S   20$
	PRINT   <'   WORD #'>
	CALL    PHEXWORD
	BRA.S   10$
20$

      IFEQ	THE_PAWN

	XREF	Section24

	MOVE.L  POSWRD(A4),A6
	LEA	Section24(A4),A3
	MOVE.W	#SECT.SCEN,D6
	DO	SEARCH2
	MOVE.W  #-1,(A2)
	LEA     MATCHBUF(PC),A2
30$
	MSG	LINE
	MOVE.W  (A2)+,D6
	BMI.S   90$
	PRINT   <'SECT #'>
	CALL    PHEXBYTE
	MOVE.W  (A2)+,D6
	BMI.S   90$
	PRINT   <'   WORD #'>
	CALL    PHEXWORD
	BRA.S   30$

      ENDC
90$
	DOGO	DBEXIT
DBMEM
	CALL    TST.DBUG
	ADD.L   A4,D0
	MOVE.L  D0,D6
	CALL    PRTHEX
	PRINT   <': '>
	MOVE.L  D0,A0
	MOVE.L  (A0),D6
	CALL    PRTHEX
	DOGO	DBEXIT
DBSET
	CALL    TST.DBUG
	MOVE.L  D0,-(A7)
	DO	FNDWRD
	CALL    GETHEX
	MOVE.L  (A7)+,A0
	MOVE.W  D6,0(A4,A0.L)
	DOGO	DBEXIT

    ENDC

UTEXIT
	 DOGO     DBEXIT

  
UTNORMAL  

	MOVEQ   #0,D5                 ;SET TO NORMAL
	BRA.S   UTOUTMODE

UTVERBOSE

	MOVEQ   #1,D5                 ;SET TO VERBOSE
	BRA.S   UTOUTMODE

UTBRIEF

	MOVEQ   #2,D5                 ;SET TO BRIEF

UTOUTMODE

	MOVE.B  D5,OUTMODE(A4)
	CALL    OK                    ;DOES ANYTHING ELSE CALL OK?
	DOGO     UTEXIT

UTSERIAL
UTID 

    IFNE       YCHEAT 
	PRINT   <'NOT YET!'>
	
	DOGO	UTEXIT

    ENDC

UTVERSION

	FIXED_SCN	Copyright	;COPYRIGHT NOTICE ETC

    IFNE    YCHEAT

	PRINT   <'YCHEAT ON'>

    ENDC

	DOGO	UTEXIT   

UTCREDIT

	FIXED_SCN	Credits		;credits text
	DOGO	UTEXIT

UTHINT

      IFD	Amiga_Demo_PCW

	PRINT	<'Please ask one of the Rainbird staff for more information about The Pawn^'>
	DOGO	UTEXIT

      ENDC

10$

	MOVE.L   POSWRD(A4),A6        ;Get addr of filename
	TEST_B   (A6)                 ;We do have something?
	BNE.S    90$                  ;NE => yes
	MSG	ENTERHINT		<'^Please enter hint :  '>

  IFNE   Wimp!IBM

	ST       SCR_IGNO(A4)         ;Ignore input from pulldown scrolls for a bit

  ENDC

	CALL     INPUT                ;Get a line from keyboard

  IFNE   Wimp!IBM

	SF       SCR_IGNO(A4)         ;Stop ignoring scrolls

  ENDC

	LEA      INBUFF(A4),A6        ;Find new text
	DO	FNDWRD               ;lose initial spaces
	MOVE.L   A6,POSWRD(A4)        ;Make sure rest of world knows

	BRA.S    10$                  ;check this one is ok

90$

	CALL     UNDOHINT
	DOGO	UTEXIT

UTSCORE
	 IFNE	THE_PAWN
	
 
	FIXED_SCN	Score1		;'YOUR SCORE_ IS..'
	MOVE.W  SCORE_(A4),D6

      IFNE    M68000
	MULU    #5,D6
      ENDC

      IFEQ    M68000
	MOVE.W  D6,-(SP)
	ASL.W   #2,D6
	ADD.W   (SP)+,D6
      ENDC

	CMP.W   #350,D6               ;SCORE_ TOO MUCH?
	BLS.S   10$                   ;NOPE - OK
	MOVE.W  #349,D6               ;YES - MAKE IT MAX -1!
10$
	CALL    PRTNUM                ;PRINT SCORE
	FIXED_SCN	Score2		;'FROM A POSSIBLE 350.'
	DOGO	UTEXIT
	 ENDC

	 IFEQ	THE_PAWN 

	XREF	ScoreCode

	CALL	ScoreCode
	DOGO	UTEXIT

	 ENDC



      IFEQ     M68000

UTQUIT
	LEA	UTEXIT(PC),A0


        ;; this is the emu version
        
DO.QUIT 

        
	MSG     QUIT                  ;'DO U WANT TO QUIT OR RESTART
	LEA     SLGAMENO+2(A4),A2     ;POINT AT 'R' - RESTART DEFAULT
	CALL    GETCHAR               ;DO CURSOR ETC
	AND.B   #$5F,D1               ;SORT CASE


	CMP.B   #'Q',D1               ;IF NOT QUIT THEN RESTART
	BNE.S   UTRESTART2            ;OK THEN

	MOVE.L  A0,-(A7)              ;Where to return to if not sure
	DO	RUSURE

	DC.W	$A0ED		      ;non 68k quit call (fod memory etc)

	
UTRESTART
	
	LEA	UTEXIT(PC),A0

UTRESTART2

      IFNE	PCW8256&THE_PAWN
	XREF	INIT.Restart
	XDEF	UTSAVE,UTRESTORE
	ST	RestartFlag(A4)
	CALL	UTRESTORE
	SF	RestartFlag(A4)
	ADDQ.L  #4,A7                   ;ADJUST SP
	BRA     INIT.Restart            ;RESTART GAME
      ENDC

	DO	RUSURE

      IFNE	SPECTRUM
	LEA      SLADDR(A4),A1        ;Start of buffer
	MOVE.L   #SLADDR.E,D1         ;End of buffer
	SUB.L    #SLADDR,D1           ;Calculate length
	MOVE.L   POSWRD(A4),A0        ;Pointer to name
      ENDC

      IFNE	BookProtection
	XREF	PROT_FLG		
	MOVE.B	PROT_FLG(A4),D7		;to avoid forgetting
      ENDC

	DC.W    $A0EE			;non 68k restart call

      IFNE	SPECTRUM
	CALL	CRYPT                ;Decode game
	CALL	CHK.DATA             ;Check if its been messed around
	ADD.L	#4,A7			;pop
	BRA	INIT
      ENDC

      IFNE	BookProtection
      MOVE.B	D7,PROT_FLG(A4)
      ENDC

	BRA	INIT

      ENDC


      IFNE     AMIGA512!MAC512!ST520

 
UTQUIT
 
	LEA     UTEXIT(PC),A0         ;SAVE RET ADDR in case not sure

        ;; this is NOT the emu version
DO.QUIT

	MSG     QUIT                  ;'DO U WANT TO QUIT OR RESTART
	LEA     SLGAMENO+2(A4),A2     ;POINT AT 'R' - RESTART DEFAULT
	CALL    GETCHAR               ;DO CURSOR ETC
	CMP.B   #10,D1                ;if its a CR don't print it
	BEQ.S   10$
	DO	CHAR.SUB
	AND.B   #$5F,D1               ;SORT CASE
10$
*	CMP.B   #'L',D1
*	BEQ     UTRESTORE2

	CMP.B   #'Q',D1               ;IF NOT QUIT THEN RESTART
	BNE.S   UTRESTART2            ;OK THEN

	MOVE.L  A0,-(A7)              ;Where to return to if not sure
	DO	RUSURE

	IFD	Amiga_Demo_PCW

	BRA	UTRESTART2

	ENDC


     
	 IFNE    MAC512

	XREF     MACQUIT
	
	BRA      MACQUIT

	 ENDC

	FIXED_SCN	Removedisk
20$
	BRA.S    20$




UTRESTART

	LEA     UTEXIT(PC),A0        ;Return address in case of mistake

UTRESTART2

	DO	RUSURE                  ;goes (A0) if unsure

	LEA     SLADDR(A4),A0
	LEA     NEWGAME,A1              ;Range errors!
	ADD.L   A4,A1
	LEA     INITG.E(A4),A2
10$
	MOVE.B  (A1)+,(A0)+
	CMP.L   A0,A2
	BNE.S   10$

	ADDQ.L  #4,A7                   ;ADJUST SP
	BRA     INIT                    ;RESTART GAME


      ENDC




   IFNE    0		;	ST520


 
UTQUIT
 
	LEA     UTEXIT(PC),A0         ;SAVE RET ADDR in case not sure

        ;; this is NOT the emu version
DO.QUIT
 
	MSG     QUIT                  ;'DO U WANT TO QUIT OR RESTART
	LEA     SLGAMENO+2(A4),A2     ;POINT AT 'R' - RESTART DEFAULT
	CALL    GETCHAR               ;DO CURSOR ETC
	CMP.B   #10,D1                ;if its a CR don't print it
	BEQ.S   10$
	DO	CHAR.SUB
	AND.B   #$5F,D1               ;SORT CASE
10$
*	CMP.B   #'L',D1
*	BEQ     UTRESTORE2

	CMP.B   #'Q',D1               ;IF NOT QUIT THEN RESTART
	BNE.S   UTRESTART2            ;OK THEN

	MOVE.L  A0,-(A7)              ;Where to return to if not sure
	DO	RUSURE

	CALL	BIOSA6
	CALL	MS_END            ;END OF MOUSE
	CLR.L   -(SP)
	MOVE.W  #$20,-(SP)            ;supervisor mode
	TRAP    #1
	ADD.W   #6,SP
	BCLR    #3,CONTERM            ;fix control key to normal (for ED.PRG)
	MOVE.L  D0,-(SP)
	CALL    GRA_EXIT
	MOVE.W  #$20,-(SP)
	TRAP    #1
	ADD.W   #6,SP

	CLR.W   -(A7)
	TRAP    #1

* (FUDGE FOR TIME BEING)

   IFNE     0

RESET_

	LEA    MSGPNTR+40000,A0             ;TOP OF PROG
	ADD.L  A4,A0
	LEA    UTRESTART(PC),A1
10$
	CLR.W  -(A0)
	CMP.L  A0,A1
	BNE.S  10$
	LEA    (A4),A0
	LEA    RESET_(PC),A1
20$
	CLR.W  (A0)+
	CMP.L  A0,A1
	BNE.S  20$

	LEA    30$(PC),A0
	MOVE.B #$41,1(A0)
	MOVE.B #$4E,(A0)+
30$
	ADDQ.W   #1,A0                ;dummy instruction
   ENDC

*     TRAP    #1


UTRESTART

	LEA     UTEXIT(PC),A0        ;Return address in case of mistake

UTRESTART2

	DO	RUSURE                  ;goes (A0) if unsure

	LEA     SLADDR(A4),A0
	LEA     NEWGAME,A1              ;Range errors!
	ADD.L   A4,A1
	LEA     INITG.E(A4),A2
10$
	MOVE.B  (A1)+,(A0)+
	CMP.L   A0,A2
	BNE.S   10$

	ADDQ.L  #4,A7                   ;ADJUST SP
	BRA     INIT                    ;RESTART GAME

      ENDC                             ;END OF ST520 BLOCK



WRITE_ON                               ;Ken did this frig.....

	MSGGO	ON_STOP

   IFNE     AMIGA512&YCHEAT

* Set a 'breakpoint'

	XREF     BP_ADDRESS,BP_CONTENTS,BP_DISP,DO_MISC


DBBPON

	MOVE.L   D0,BP_ADDRESS(A4)          ;save addr of fodded word
	MOVE.L   0(A4,D0.L),BP_CONTENTS(A4) ;get contents of long word to fod
	MOVE.L   BP_CODE(PC),0(A4,D0.L)     ;fod code
	PRINT    'Done!^'
	DOGO	DBEXIT

DBBPOFF

	MOVE.L   BP_ADDRESS(A4),D0
	MOVE.L   BP_CONTENTS(A4),0(A4,D0.L)
	DOGO	DBEXIT


BP_CODE

	JSR      BP_DISP(A6)            ;this is inserted as a breakpoint

DBMISC

	CALL     DO_MISC                ;This lives in CONTROL.AMG
	                             ;since it has the relevent INC files
	DOGO	DBEXIT




   ENDC


   IFNE     Graphics

   IFNE    YCHEAT

DBPICTURE

      	CALL    TST.DBUG
        MOVEQ   #1,D1                   ;show pic code
	CALL    LOAD_PIC                ;Get picture as per D0 (starts at 0)
	DOGO	UTEXIT

   ENDC

	IFNE	C128!C64

	IFD	C64Pict

DBC64Pict

10$

	MOVE.B	(A6)+,D0
	CMP.B	#' ',D0
	BEQ.S	10$

	SUBQ.B	#1,D0

	AND.W	#$1F,D0


	CALL    LOAD_PIC                ;Get picture as per D0 (starts at 0)
	DOGO	UTEXIT

	ENDC

	XREF	GRAPH64

UTGRAPHICS	CALL	GRAPH64
		DOGO	UTEXIT

	ENDC

	IFEQ	C128!C64

UTGRAPHICS

	PEA     UTEXIT(PC)
	MSG	GRAPHICS		;'Graphics '
	LEA     GRAPHICS(A4),A0         ;Toggle flag (default = on)
	CALL    Switch
	TEST_B  (A0)

	BEQ.S   10$			; Now -1 => graphics on
	MSG	ON_STOP			;      0 => graphics off
	MOVEQ	#1,D1			;so loadpic loads!! (non 68k)
	BRA     PICTURES                ;just turned graphics on, check if right

10$
      IFEQ	M68000			;ie not amiga/st/mac
	MOVEQ	#0,D1			;turn piccy off
	CALL	LOAD_PIC
      ENDC

	MSGGO	OFF


	ENDC

	IFNE	AMIGA512!ST520

UTSWHEIGHT

	PEA     UTEXIT(PC)
	
10$
      IFNE     Mono_Possible

	TEST_B  COL_FLG(A4)
	BEQ     SWHEIGHT

      ENDC

	MOVE.W  #WIDTH.OPT,WIDTH(A4)
	NOT.B   CWID_FLG(A4)
	BNE.S   90$
	MOVE.W  #WIDTH.DEF,WIDTH(A4)
90$
	MSGGO	TEXTSIZE		;<'Text size changed. ^'>

	ENDC
	 
	 IFNE     Mono_Possible

UTMAKEDITH

	PEA     UTEXIT(PC)
	TEST_B  COL_FLG(A4)
	BEQ.S   10$
	RET
10$
	MOVE.L  #MAKEDITH,A0
	JSR     0(A4,A0.L)
	TEST_B  DITH_FLG(A4)
	BNE     WRITE_ON
	MSGGO	OFF

UTMAKEFREE

	PEA     UTEXIT(PC)
	TEST_B  COL_FLG(A4)
	BEQ.S   10$
	RET
10$
	MSG	FREEHAND			;<'Freehand ' >
	MOVE.L  #MAKEFREE,A0
	JSR     0(A4,A0.L)
	TEST_B  FREE_FLG(A4)
	BNE     WRITE_ON
	MSGGO	OFF
	

UTMAKESTIP

	PEA     UTEXIT(PC)
	TEST_B  COL_FLG(A4)
	BEQ.S   10$
	RET
10$
	MSG	STIPPLE				;<'Stipple ' >

	MOVE.L  #MAKESTIP,A0
	JSR     0(A4,A0.L)
	TEST_B  STIP_FLG(A4)
	BNE     WRITE_ON
	MSGGO	OFF

   ENDC

   ENDC


   IFNE     SpoolLen

UTPRINTER

      IFD	Amiga_Demo_PCW

	PRINT	<'Not in a demo!^'>
	DOGO	UTEXIT

      ENDC

	PEA     UTEXIT(PC)
	LEA     PRINTER(A4),A0            ;Toggle flag (default = on)
	CALL	Switch

      IFEQ	M68000		;for Apples and the like

	MOVE.B  PRINTER(A4),D0

	DC.W    $A0EF		;printer trap (takes D0.B)

	AND.B   D0,D7		;D0 EQ=> we just fodded printer so error n/a
	BEQ.S   10$
	
	SF      PRINTER(A4)
	MSGGO	NOPRINTER		;<'Unable to find printer^'>

      ENDC

10$
	MSG	PRINTER			;<'Printer '>

      IFNE     AMIGA512

   XREF  PrtBAD


	TEST_B  PrtBAD(A4)
	BEQ.S   20$
	SF      PRINTER(A4)
	MSGGO	OPEN_FAILED		;<'open failed.'>

      ENDC

20$
	TEST_B  PRINTER(A4)
	BNE     WRITE_ON                    ;Now -1 => printer on
	MSGGO	OFF                       ;     0 => printer off

   ENDC

   IFEQ		DosPresent
 
UTSAVE
 
	CALL    SAVEGAME
	DOGO	UTEXIT
 
UTRESTORE
 
	CALL    LOADGAME
	CLR.B   OLDTEXT(A4)           ;USED AS SECTOR 0 BUFFER!

	DOGO	UTEXIT

UTRESTORE2
 
	LEA	UTRESTORE2(PC),A0
	CALL    GETGAME
	CLR.B   OLDTEXT(A4)           ;USED AS SECTOR 0 BUFFER!

	DOGO	UTEXIT
 
    ENDC


    IFNE	QL128
 
UTQUIT
 
	LEA     UTEXIT(PC),A0         ;SAVE RET ADDR
 
DO.QUIT
 
	MSG	QUIT                ;'DO U WANT TO QUIT OR RESTART
	LEA     SLGAMENO+2(A4),A2     ;POINT AT 'R' - RESTART DEFAULT
	CALL    GETCHAR               ;DO CURSOR ETC
	DO	CHAR.SUB
	AND.B   #$5F,D1               ;SORT CASE
	CMP.B   #'Q',D1               ;IF NOT QUIT THEN RESTART
	BNE.S   UTRESTART2            ;OK THEN
	MOVE.L  A0,-(A7)              ;SOMEWHERE TO RETURN TO
	DO	RUSURE                ;FIND OUT
	RESET                         ;YES!!!.....
  
 
UTRESTART 
	
	PEA     UTEXIT(PC)
	DO	RUSURE                ;DO Y REALLY WANT TO?
 
UTRESTART2
 
	MSG	ENSURE              ;MAKE SURE 'THE PAWN...
	LEA     SLGAMENO+4(A4),A2        ;POINT AT A SPACE
	CALL    GETCHAR               ;WAIT FOR CHAR
	MOVEQ   #4,D0                 ;= FILE #5
	MOVE.W  #1,DRIVE(A4)          ;DRIVE #
 
	BRA     BIOS
 
    ENDC

DO.DEATH

        XREF    CANTSAVE,C.STATUS,UndoDiff,EXTRA
        
	MSG	DEADED

       	CLR.W   DIED(A4)
        CLR.W   CANTSAVE(A4)
        CLR.W   C.STATUS(A4)
        CLR.W   EXTRA(A4)            ; kill any trailing commands
	
    IFNE    YCHEAT
	TEST_W  DBUG.ON(A4)
	BNE.S   10$
    ENDC
	
	TEST_W  USER.DB(A4)
	BEQ.S   20$
10$
	MSG	LETOFF
        CALL    SP.DEATH
	DOGO	SCREEN                ;STACK SHOULD BE OK
 
20$
	MSG	LETOFF
        CALL    SP.DEATH
        BRA     UndoDiff              ;apply undo & go to screen (sc70)

        PEA	30$(PC)
30$
	LEA     30$(PC),A0            ;COME HERE IF WILL NOT SAY OK

	BRA    DO.QUIT

*--------------------------------
*
*  SRC.ADV.SAVE/LOADGAME
*
*--------------------------------
 

    IFEQ	DosPresent
 
BD.MEDIUM.SV
 
	CALL    CRYPT                 ;RECOVER GAME DATA
	
	MSG	OBSAVE 			;<'^OUCH! - UNABLE TO SAVE^'>
	RET   
 
SAVEGAME

   IFNE   QL128
	MOVE.W  LEFTM(A4),LEFTM.SV(A4)         ;PRESERVE LEFT MARGIN ETC ON
	MOVE.B  DISPTYPE(A4),DISP.SV(A4)       ;SAVE GAME
   ENDC

	LEA     SAVE_EXIT(PC),A0
	CALL    SLOT.RUSURE           ;GET GAME #, DO U SURE?
	 
	MOVE.B  SLGAMENO+5(A4),D1	;get drive no.
	AND.W   #$0F,D1
	MOVE.W  D1,DRIVE(A4)
 
	MOVE.W  D0,FILENO(A4)         ;FILE NO TO GET
	LEA     SLADDR(A4),A0         ;START OF DATA AREA
	MOVE.L  A0,DEST(A4)
 
	MOVE.L  #SLADDR.E,D0		;calc save game size
	SUB.L   #SLADDR,D0

    IFNE     QL128
	LSR.L   #8,D0			;div by 512
	LSR.L   #1,D0
    ENDC

	MOVE.W  D0,FILESIZE(A4)
	
    IFEQ    QL128
	MOVE.B  SLGAMENO(A4),INBUFF(A4)	     ;game no. (ascii 1-8)
	MOVE.B  SLGAMENO+5(A4),INBUFF+1(A4)  ;drive no. (ascii 1/2)
	CLR.B   INBUFF+2(A4)		     ;zero at end for luck
    ENDC

	MOVE.L  A4,-(A7)              ;OUCH IF STOMPED!!
	CALL    CALC.DATA             ;CALCULATE SOME CHK SUMS
	CALL    CRYPT                 ;ENCRYPT GAME

    IFNE    QL128
	CALL    WRITEIT               ;SAVE GAME
    ENDC

    IFEQ    QL128
	LEA	INBUFF(A4),A0		;point at 'filename'
	LEA     SLADDR(A4),A1		;point at area to save/load
	MOVE.W  FILESIZE(PC),D1         ;no. of bytes to do
	CALL    WRITEFIL
    ENDC
 
	MOVE.L  (A7)+,A4
	CMP.W   #-1,D7
	BEQ     BD.MEDIUM.SV
	CALL    CRYPT                 ;DECRYPT GAME & EXIT

SAVE_EXIT

	RET
 
LOADGAME

	LEA	LOAD_EXIT(PC),A0
	 
GETGAME

	CALL    SLOT.RUSURE           ;GET GAME #, DO U SURE?
	MOVE.B  SLGAMENO+5(A4),D1	;get drive no.
	AND.W   #$0F,D1
	MOVE.W  D1,DRIVE(A4)
 
	MOVE.W  D0,FILENO(A4)         ;FILE NO TO GET
	LEA     SLADDR(A4),A0         ;START OF DATA AREA
	MOVE.L  A0,DEST(A4)
 
	MOVE.L  #SLADDR.E,D0		;calc save game size
	SUB.L   #SLADDR,D0

    IFNE    QL128
	LSR.L   #8,D0			;div by 512
	LSR.L   #1,D0
    ENDC

	MOVE.W  D0,FILESIZE(A4)
	MOVE.L  A4,-(A7)              ;MIGHT BE *VERY* PAINFUL
 
    IFNE   QL128
	CALL    READIT
    ENDC

    IFEQ    QL128
	MOVE.B  SLGAMENO(A4),INBUFF(A4)	     ;game no. (ascii 1-8)
	MOVE.B  SLGAMENO+5(A4),INBUFF+1(A4)  ;drive no. (ascii 1/2)
	CLR.B   INBUFF+2(A4)		     ;zero at end for luck

	LEA	INBUFF(A4),A0		;point at 'filename'
	LEA     SLADDR(A4),A1		;point at area to save/load
	MOVE.W  FILESIZE(PC),D1         ;no. of bytes to do
	CALL    READFILE
    ENDC

	MOVE.L  (A7)+,A4
	
    IFNE	QL128       
	CMP.W   #-1,D7                ;IO ERROR
	BEQ     BD.MEDIUM.LD          ;YES
	
	CMP.W   #-2,D7                ;NOT FOUND?
	BEQ     NT.FOUND.LD
    ENDC

    IFEQ	QL128
	TEST_B	D7
	BEQ.S	10$
	MSG	OBLOAD			;<'Ouch - bad load'>
	BRA.S	LOAD_EXIT
10$
    ENDC
	
	CALL    CRYPT                 ;DECRYPT
	CALL    CHK.DATA              ;SEE IF IT'S A VALID GAME
 
    IFNE	BookProtection
	CALL	PROTECT2
    ENDC


* RETURN = YES OK

    IFNE  QL128
 
	MOVE.W  LEFTM.SV(A4),LEFTM(A4)
	MOVE.B  DISP.SV(A4),DISPTYPE(A4)       ;RECOVER SCREEN INFO
 
	MOVE.W  WIDTH(A4),D1          ;YET MORE..
	ADDQ.W  #2,D1
	MOVE.W  D1,SWIDTH(A4)
	ADDQ.W  #1,D1
	MOVE.W  D1,VWIDTH(A4)
 
	CALL    STWINDOW              ;SINCE WE JUST ZAPPED WIDTH
*                                    ;BETTER CLEAR UP SCREEN
 
      ENDC

LOAD_EXIT
 
	RET

* P.DEFAULT	prints default msg for people who press RETURN

P.DEFAULT

	PRINT	<'^(Defaulting to '>
	MOVE.W	D1,-(A7)
	MOVE.B	(A2),D1
	DO	CHAROUT
	MOVE.W	(A7)+,D1
	PRINT	<')^'>
	RET

 
SLOT.RUSURE
 

*      CALL    W.PDR2                ;PUT UDV IN D2, WHICH GAME?

	IFNE	APPLE2
 
05$
	LEA     SLGAMENO+5(A4),A2     ;point at default drive no.
	PRINT   <'^Drive no. (1/2) ?'>
	CALL    GETCHAR
	CMP.B   #10,D1                ;CR = USE DEFAULT
	BEQ.S   07$
	CMP.B   #$31,D1               ;one is minimum
	BLT.S   05$                   ;NO GOOD = TRY AGAIN
	CMP.B   #$32,D1               ;two IS MAX
	BGT.S   05$
	MOVE.B  D1,(A2)
	BRA.S	10$
07$
	CALL	P.DEFAULT		;print '(Defaulting to .)'

	ENDC
10$

	PRINT   <'^Game no. (1-8) ?'>
	LEA     SLGAMENO(A4),A2       ;POINT AT GAME #
	CALL    GETCHAR               ;DO CURSOR WI DEFAULT NO., GET D1
	CMP.B   #10,D1                ;CR = USE DEFAULT
	BEQ.S   15$
	CMP.B   #$31,D1               ;one is minimum
	BLT.S   10$                   ;NO GOOD = TRY AGAIN
	CMP.B   #$38,D1               ;eight IS MAX
	BGT.S   10$
	MOVE.B  D1,(A2)               ;SAVE FOR DEFAULT LATER
	BRA.S	20$
15$
	CALL	P.DEFAULT
20$
	MOVE.B  (A2),D1               ;GET THE CHAR WE WILL USE
	MOVE.B  D1,D0                 ;COS D1 WILL GET STOMPED SOON
	AND.B   #$0F,D0               ;LOSE MISC ASCII BITS
	MSG	LINE                  ;LF
	
    ENDC

	
RUSURE

	MSG	USURE               ;'U SURE (Y/N)?
 
	LEA     SLGAMENO+1(A4),A2     ;POINT AT "N" (DEFAULT IS NO)
	CALL    GETCHAR               ;DO CURSE ETC, BYTE TO D1

      IFNE    M68000
	CMP.B   #10,D1                ;Lose CR for printing
	BEQ.S   10$ 
	DO	CHAR.SUB              ;PRINT WHATEVER THEY TYPE
10$
      ENDC

	MSG	LINE                  ;SO WE DON'T UPSET LEFT JUST
	AND.B   #$5F,D1               ;FIX CASE
	CMP.B   #'Y',D1
	BEQ.S   90$                   ;YEA - DO IT !!!!
 
      IFNE      AMIGA512
	JSR     NewCLIST(A4)          ;our screen please!
      ENDC

	MOVE.L  (A7)+,D0              ;FORGET IT
	JMP     (A0)
90$
	RET
 
 
*--------------------------------
* 
*  CRYPT - EORS SAVE/LOAD GAME
*          BITS WI RANDOM NO.S
*
*--------------------------------
 
 
CRYPT

      IFNE	VAX
	RET
      ENDC

      IFNE    M68000 
	
	LEA	RND.CHIN(PC),A5
	LEA	RND.DATA(PC),A6

	MOVE.W  (A5),-(A7)
	MOVE.L  (A6),-(A7)          ;SAVE THE OLD RND NO. DATA
	MOVE.L  4(A6),-(A7)
	MOVE.W  8(A6),-(A7)

	MOVEM.L	A5/A6,-(A7)
 
	MOVE.L  #$12345678,(A6)        ;PUT PRE-SET NO.S THERE
	MOVE.L  #$9ABCDEF0,4(A6)
	MOVE.W  #12345,8(A6)
	MOVE.W  #6789,(A5)
 
	CALL    INITRND               ;RANDOMISE
	LEA     SLADDR(A4),A0         ;WHERE TO START
	MOVE.L  #SLADDR.E,D3          ;NO. OF WORDS TO DO
	SUB.L   #SLADDR,D3            ;All this is the linkers fault!
	LSR.L   #1,D3                 ;DIV 2
	
10$
	CALL    RNDGEN                ;RND WORD - D0
	EOR.W   D0,(A0)+              ;ZAP A WORD
	DBRA    D3,10$                ;DO UNTIL END OF SAVED AREA

	MOVEM.L	(A7)+,A5/A6
 
	MOVE.W  (A7)+,8(A6)  ;RECOVER OLD SEED ETC
	MOVE.L  (A7)+,4(A6)
	MOVE.L  (A7)+,(A6)
	MOVE.W  (A7)+,(A5)
 
      ENDC

      IFEQ    M68000
	LEA	SLADDR(A4),A0
	MOVE.L   #SLADDR.E,D3
	SUB.L    #SLADDR,D3
	LSR.L    #2,D3			; Long word encrypt

	MOVE.L   #'Pawn',D0

10$:
	EOR.L	D0,(A0)+
	DBRA	D3,10$

      ENDC
	RET
 
 
BD.MEDIUM.LD

    IFNE    ST520&YCHEAT

	DC.W    $4AFC

    ENDC

* if we get here then the state of global variables is VERY undefined
* (cos we just loaded a crap game (perhaps even code/piccy/text...))
*
* So we have to clean up a bit even before we can tell the punter what does...

	CLR.W   LJPOS(A4)             ;How many chars were in left just buffer
	CLR.B   LAST.CH2(A4)          ;Previous char
	CLR.B   CHANNEL(A4)           ;Avoid forced caps
	
      IFNE    Graphics

	CLR.B   CWID_FLG(A4)          ;make 'normal' height/width

      ENDC

	MOVE.W  #WIDTH.DEF,WIDTH(A4)  ;and more than one word per line...

	PRINT   <'^Fatal error during load^^'>


      IFNE      AMIGA512

	JSR      NewCLIST(A4)          ;our screen please!

      ENDC

	FIXED_SCN	Removedisk

    IFNE	MAC512

	MOVE.L	#100000,D0
05$	
	SUBQ.L	#1,D0
	BNE.S	05$

	BRA	MACQUIT

    ENDC
	

10$
	BRA.S    10$

 
NT.FOUND.LD
 
	PRINT     <'^Not found^'>

      IFNE      AMIGA512

	JSR      NewCLIST(A4)            ;our screen please!

      ENDC

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
 
CHK.DO

      IFNE	VAX
	MOVEQ	#0,D5
	RET
      ENDC
	
	LEA     SLADDR(A4),A0            ;START OF SAVE/LOAD AREA
	MOVE.L  #SLADDR.E,D0             ;Calculate NO. OF WORDS
	SUB.L   #SLADDR,D0               ;Bloody stupid linker!

	MOVEQ   #0,D5                 ;SCRATCH

      IFNE    M68000

	LSR.L   #1,D0                    ;DIV 2 for word wise
	
	LEA     SUM.CHK(A4),A1
	CLR.L   (A1)+
	CLR.L   (A1)+   
	CLR.L   (A1)
  
	MOVEQ   #0,D1
	MOVEQ   #0,D2
	MOVEQ   #1,D3                 ;MULT!
	MOVEQ   #0,D4
	MOVEQ   #0,D6                 ;COUNT ZEROS
 
10$:
	MOVE.W  (A0)+,D5              ;SAVE LOTS OF MEM ACCESS
	ADD.W   D5,D1                 ;WORD ADD
	EOR.W   D5,D2                 ;EOR
	ADD.L   D5,D4  
	TEST_W  D5                    ;ZERO?
	BEQ.S   20$
	MULU    D5,D3                 ;MULT IF NOT ZERO
	SWAP    D3                    ;AVOID LOTS OF TRAILING ZEROS
	ADDQ.W  #1,D3                 ;AVOID ZEROS
	BRA.S   30$
20$
	ADDQ.W  #1,D6                 ;OK, ADD TO ZERO COUNT
30$
	DBRA    D0,10$

      ENDC

      IFEQ    M68000

	CLR.L	SUM.CHK(A4)
	LSR.L	#2,D0			;div 4 for LONG wordwise
	SUBQ.W	#2,D0			;safety margin!	
10$:
	ADD.L	(A0)+,D5
	DBRA    D0,10$                ;NEXT..

      ENDC

	RET
 
  
CALC.DATA
 
	CALL_S  CHK.DO                ;CLEAR CHECKSUMS + CALC NEW ONES
	LEA     SUM.CHK(PC),A0        ;START OF BLOCK

      IFNE	M68000
	MOVE.W  D1,(A0)+              ;SUM.WORD
	MOVE.W  D2,(A0)+              ;EOR.WORD
	MOVE.W  D3,(A0)+              ;MULT.WORD
	MOVE.W  D6,(A0)+              ;ZERO COUNT
	MOVE.L  D4,(A0)               ;SUM.LONG
      ENDC

      IFEQ	M68000
	MOVE.L	D5,(A0)
      ENDC

	RET                           ;BYE BYE
 
  
CHK.DATA
 
	LEA     SUM.CHK(PC),A1        ;SAVE LOADED SET

      IFEQ	M68000
	MOVE.L	(A1),-(A7)		;push sum (long)
	CLR.L	(A1)+			; clear it as it was originally

* AJS changed this because it was checksumming it first with SUM.CHK 
* cleared - and then with SUM.CHK with the checksum in it !!!!!!!
* THIS WAS A VICIOUS BUG!!!
      ENDC

      IFNE	M68000
	MOVE.L  (A1)+,-(A7)           ;PUSH SUM & EOR
	MOVE.L  (A1)+,-(A7)           ; ..  MULT & ZERO COUNT
	MOVE.L  (A1)+,-(A7)           ; ..  LONG SUM
      ENDC
 
	CALL_S  CHK.DO                ;CLEAR CHECKSUMS + CALC NEW ONES
    
      IFNE	M68000
	LEA     SUM.CHK(PC),A0        ;START OF BLOCK
	CMP.L   (A7)+,D4              ;CHECK LONG SUM
	BNE.S   10$
	CMP.W   (A7)+,D3              ; & MULT
	BNE.S   20$
	CMP.W   (A7)+,D6              ; & ZERO COUNT
	BNE.S   30$
	CMP.W   (A7)+,D1              ; & SUM.W
	BNE.S   40$
	CMP.W   (A7)+,D2              ; & EOR.W
	BNE.S   50$
      ENDC

      IFEQ	M68000
	CMP.L	(A7)+,D5
	BNE.S	50$
      ENDC

 
* GET HERE = OK
 
	RET                           ;EXIT - GOOD

      IFNE	M68000
   
10$
	TEST_W  (A7)+                 ;STACK=STACK+2
20$
	TEST_W  (A7)+                 ; DITTO
30$
	TEST_W  (A7)+                 ; DITTO
40$
	TEST_W  (A7)+                 ;LAST OF THESE

      ENDC

50$
	TEST_L  (A7)+                 ;THIS POPS THE RET
	BRA      BD.MEDIUM.LD         ;EXIT - BAD



   IFNE     DosPresent

UTRESTORE

	LEA      UTEXIT(PC),A0        ;if unsure exit

      IFD	Amiga_Demo_PCW
	PRINT	<'Not in a demo!^'>
	DOGO	UTEXIT
      ENDC

      IFD	DEMO_for_PCW
	PRINT	<'Not in a demo!^'>
	DOGO	UTEXIT
      ENDC

UTRESTORE2

      IFD	DEMO_for_PCW
	PRINT	<'Not in a demo!^'>
	DC.W	$A0EE
      ENDC

      IFD	Amiga_Demo_PCW
	PRINT	<'Not in a demo!^'>
	BRA	UTRESTART2
      ENDC

      IFNE      AMIGA512
	MOVE.L   A4,A6
	MOVEQ    #10,D1
	JSR      AD_COUT(A6)
	JSR      OldCLIST(A6)         ;AmigaDos screen please!
      ENDC

      IFEQ     MAC512
	CALL     CHECK_FILE_NAME      ;Make sure we have a filename of some sort
	LEA	UTEXIT(PC),A0		; because check_file.. fucks it
	DO	RUSURE
      ENDC

      IFNE	CPC6128
	CALL	GetSavedGameDisk
      ENDC

	LEA      SLADDR(A4),A1        ;Start of buffer
	MOVE.L   #SLADDR.E,D1         ;End of buffer
	SUB.L    #SLADDR,D1           ;Calculate length
	MOVE.L   POSWRD(A4),A0        ;Pointer to name

	CALL     READFILE
	TEST_B   D7                   ;Error?
	BEQ.S    10$

	MSG	OBLOAD			;<'Ouch - bad load^'>
	TEST_W	DIED(A4)		;in case they chose 'LOAD' option
	BNE	UTRESTORE2		;from a death message
					;stop them cancelling the load
	BRA.S    20$                  ;Only decode if we really loaded a game
10$
	CALL     CRYPT                ;Decode game
	CALL     CHK.DATA             ;Check if its been messed around

20$
      IFNE	PCW8256
	TEST_B	RestartFlag(A4)
	BEQ.S	30$
	RET
30$
      ENDC

      IFNE      AMIGA512
	JSR      NewCLIST(A4)         ;our screen please!
      ENDC
        
      IFNE      BookProtection
	CALL     PROTECT2
      ENDC
        
	DOGO	UTEXIT

UTSAVE

      IFD	Amiga_Demo_PCW
	PRINT	<'Not in a demo!^'>
	DOGO	UTEXIT
      ENDC

      IFD	DEMO_for_PCW
	PRINT	<'Not in a demo!^'>
	DOGO	UTEXIT
      ENDC

      IFNE      AMIGA512
	MOVE.L   A4,A6
	MOVEQ    #10,D1
	JSR      AD_COUT(A6)
	JSR      OldCLIST(A4)   ;AmigaDos screen please!
      ENDC

      IFEQ     MAC512
	CALL     CHECK_FILE_NAME         ;If we don't have a file name get one
	LEA      UTEXIT(PC),A0           ;if unsure exit
	DO	RUSURE
      ENDC

      IFNE	CPC6128
	CALL	GetSavedGameDisk
      ENDC

	MOVE.L   POSWRD(A4),-(A7)        ;Save pointer to filename
	CALL     CALC.DATA               ;Calculate checksums
	CALL     CRYPT                   ;Encode game info

	LEA      SLADDR(A4),A1           ;Start of buffer
	MOVE.L   #SLADDR.E,D1            ;End of buffer
	SUB.L    #SLADDR,D1              ;Calculate length
	MOVE.L   (A7)+,A0                ;Pointer to name
	CALL     WRITEFIL

	MOVE.L   D7,-(A7)
	CALL     CRYPT                   ;Decode game again
	CALL     CHK.DATA
	MOVE.L   (A7)+,D7

	TEST_B   D7                   ;Error?
	BEQ.S    10$                  ;EQ=> nope

	MSG	OBSAVE			;<'Ouch - bad save^'>
10$
      IFNE	PCW8256
	TEST_B	RestartFlag(A4)
	BEQ.S	20$
	RET
20$
      ENDC

      IFNE      AMIGA512
	JSR      NewCLIST(A4)         ;our screen please!
      ENDC

	DOGO	UTEXIT

      IFNE	CPC6128
GetSavedGameDisk

	XREF	GETCHAR2

* THIS message MUST stay as a print text, cos msgs from disk won't be avail!

* WARNING!!! don't put a line feed on the end of this!!

	PRINT	<'Please insert your saved games disk.^Press return.'>
	MOVE.L	D1,-(A7)
10$
	CALL	GETCHAR2
	CMP.B	#10,D1
	BNE.S	10$
	MOVE.L	(A7)+,D1
	CALL	LINE
	RET

      ENDC

CHECK_FILE_NAME

      IFNE	PCW8256

	XREF	RestartFlag

	TEST_B	RestartFlag(A4)
	BEQ.S	Real_CHECK_FILENAME
	LEA	RestartName(PC),A0
	MOVE.L	A0,POSWRD(A4)
	RET

RestartName	DC.B	' M:THEPAWN.GAM',0,0

Real_CHECK_FILENAME

      ENDC

	MOVE.L   A0,-(A7)

	BRA.S    20$
10$
	MOVE.L   POSWRD(A4),A6        ;Get addr of filename
	TEST_B   (A6)                 ;We do have something?
	BNE.S    90$                  ;NE => yes
20$

	MSG	PLSENTFIL		;<'^Please enter filename :  '>

  IFNE   Wimp!IBM
	ST       SCR_IGNO(A4)         ;Ignore input from pulldown scrolls for a bit
  ENDC

	CALL     INPUT                ;Get a line from keyboard

  IFNE   Wimp!IBM
	SF       SCR_IGNO(A4)         ;Stop ignoring scrolls
  ENDC

	LEA      INBUFF(A4),A6        ;Find new text
	DO	FNDWRD               ;lose initial spaces
	MOVE.L   A6,POSWRD(A4)        ;Make sure rest of world knows

     IFNE	VAX

	TEST_B	(A6)+
	BEQ.S	10$
30$
	TEST_B	(A6)+
	BNE.S	30$
	MOVE.B	#'.',-1(A6)
	MOVE.B	#'S',(A6)+
	MOVE.B	#'A',(A6)+
	MOVE.B	#'V',(A6)+
	CLR.B	(A6)

      ENDC

	BRA.S    10$                  ;check this one is ok
90$
	MOVE.L   (A7)+,A0
	RET

   ENDC


   IFNE     FunctionKeys

*  Function keys stuff (setting up funct. key)

UTFUNCT

	SUBQ.B   #1,D0
	BMI.S    10$                  ;Ouch! Bad function key no. (too low)
	CMP.B    #FunctionKeys,D0
	BMI.S    20$                  ;MI=>ok
10$
	MSG	BADFUNCT		;<'Bad function key number'>
	BRA.S    90$                  ;Exit
20$
	MSG	ENTERFUNCT		;<'Please type function key text : '>
	MOVE.B   D0,-(A7)             ;Better save funct. no.
	CALL     INPUT                ;Leaves text in inbuff
	MOVE.B   (A7)+,D0
	LSL.W    #8,D0                ;256 bytes per key
	LEA      FUNCBASE(A4),A0
	ADD.W    D0,A0
	LEA      INBUFF(A4),A1
30$
	MOVE.B   (A1)+,(A0)+          ;Copy till zero terminator
	BNE.S    30$
90$
	DOGO      UTEXIT

   ENDC

	END
