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
*	@(#)data1.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.DATA1
*
*--------------------------------


	include	"equates.asm"
	include	"macros.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"msgequref.i"            ;Need M.AND !

       XREF     NOUNBUF


       XDEF   SLADDR,GRAPHICS
       XDEF   WORD_,VERB,NOUN,CONJ,SECTION_,EXNUM,RNOUNRES,RNOUNLST
       XDEF   ADDNUM,ADDNOUN,POSWRD,EXTRA,EXTRAPNT,ADJPNTR
       XDEF   ENERGY,CLOCK,ROOM,OUTMODE,SUBJECT,SUBJECT2
       XDEF   WCARR,STACK,TYPE,POSSESS,TALKME,YNHANDLR
       XDEF   DATIVE,PARSTO,RIDPRT,DIED,HAVEWON,USER.DB
       XDEF   RELTYPE,DBSFLG,ZAP,TEMP,ERRHOLD,SLGAMENO
       XDEF   LAST.CH,LAST.CH2,CAPITAL,PUNCT,NOTRAIL,NOTRAIL2
       XDEF   INITNUM,M_STACK,LJPOS,PLURAL
       XDEF   PLY.TIED,ADVERB,WHOOPS,PRONHIM
       XDEF   PRONHER,WRDPNT,LISTWRD,LISWRD,SUBOBJ,GNGTEMP,TMPPNTR
       XDEF   FROMWCH,BRKWCH,RELSTK,EXAMPLE,DEPTH,PULFAIL9
       XDEF   LASTNPC,ADJFLG,OLDVERB,C.DATA,C.STRENG
       XDEF   C.HP,C.STRMOD,C.STATUS,C.TIME,RIDING,GETNUM
       XDEF   CHARCOUN,ADJNOUN,PEND,PEND2,VERB2,SCRPNT
       XDEF   ADDPNTR,CONJPNTR,LIGHT,POS,TIMERS,TIMER0
       XDEF   TIMER1,TIMER2,TIMER3,TIMER4,TIMER5,TIMER6
       XDEF   TIMER7,TIMER8,TIMER9,TIMERA,TIMERB,TIMERC
       XDEF   TIMERD,TIMERE,TIMERF,NEG_,WIDTH,CWID_FLG
       XDEF   RIGHTM,LEFTM.SV,DISP.SV,STOPRON,SEATED,LASTA0
       XDEF   IGNORQ,IGNORAJ,SCORE_,SP3.FLAG,SCANFLAG,F_PROCBS
       XDEF   REGSVA5.,NMOVEFLG,NEXITFLG,MOVE.ADV,RIDFLG,F_PWALL
       XDEF   NPCINRM,CONVNPC,PRINC.CO,F_PORT,F_PORT2,JON.FLG
       XDEF   F_GURU,F_ROSE,F_GUARDS,F_KRONOS,F_ADV,COUNT.AD
       XDEF   F_PDDOOR,F_KMEET2,F_KMEET3,F_DRAG,F_DRAG2,F_DRAG3
       XDEF   F_DEVIL,COUNT.DE,F_ALCH,F_ALCH2,F_LIFTUP,F_LIFTP
       XDEF   F_VSHAFT,F_BOULDE,WASMOD2,F_COLD,F_AERO,F_KRDEAT
       XDEF   GameFinished
       XDEF   F_TRADE,F_ADVCOL,F_COLOUR,F_PRFOLL,F_LIFT,KM.ROOM
       XDEF   KM.TIME,TBL.ATTA,SP.AT.HJ,SP.AT.AD,SP.AT.DE,SUM.CHK
       XDEF   EOR.CHK,MULT.CHK,ZERO.CHK,SUML.CHK
       XDEF   REMASTER,CANTSAVE

	 IFEQ	INOUTLINK

	XDEF	CWID_FLG,CHANNEL

	 ENDC

SLADDR

GameFinished	DC.B	0
WORD_                 DC.L    0
VERB                  DC.L    0  
NOUN                  DC.L    0
CONJ                  DC.L    0
SECTION_              DC.L    0
EXNUM                 DC.W    0
ADDNUM                DC.L    0
ADDNOUN               DC.L    0
POSWRD                DC.L    0
EXTRA                 DC.W    0
EXTRAPNT              DC.L    0
ADJPNTR               DC.L    0
ENERGY                DC.W    0
CLOCK                 DC.W    0
ROOM                  DC.W    0
OUTMODE               DC.W    0
SUBJECT               DC.W    0
SUBJECT2              DC.W    0
WCARR                 DC.W    0
STACK                 DS.B    12
TYPE                  DC.W    0
POSSESS               DC.W    0
TALKME                DC.W    0
YNHANDLR              DC.L    0       ;ADDRES OF YES/NO HANDLER 0= COPOUT 
DATIVE                DC.L    0
PARSTO                DC.L    0       ;AS USED BY THE INFAMOUS PARVERB
RIDPRT                DC.W    0
DIED                  DC.W    0
HAVEWON               DC.W    0
USER.DB               DC.W    0     ;<0 = HAS ONE AND IS IN DB (HOHO) MODE
RELTYPE               DC.W    0
DBSFLG                DC.W    0
 
ZAP                   DC.W    0
TEMP                  DC.W    0
ERRHOLD               DC.L    0 
       
SLGAMENO       DC.B '1N'   ;SAVE/LOAD GAME DEFAULTS    (ASCII BYTES)
               DC.B 'R '   ;R - RESTART IF U DIE (OR QUIT)
	       DC.B ' 1'   ;spare space and drive no.
 
LAST.CH               DC.B    0   ;FOR MULTI LFS
LAST.CH2              DC.B    0   ;FOR MUTLI SPACES
CAPITAL               DC.B    0   ;CHAROUT FLAGS IF NEXT CHAR TO BE UPPER CASE
PUNCT                 DC.B    0   ;DECOM FLAGS IF IT SHOULD ADD A SPACE
		 IFEQ	INOUTLINK
CHANNEL               DC.B    0    
		 ENDC
NOTRAIL               DC.B    0   ;SET = LOSE SPACE AT END OF PRT.NOUN
NOTRAIL2	DC.B	0	;set =>REALLY don't (for P.LISTSTOP!)
INITNUM               DC.B    0

		IFEQ	YCHEAT
GRAPHICS              DC.B    -1  ;EQ => NO GRAPHICS, NE => GRAPHICS ON
	ENDC
        
		IFNE	YCHEAT
GRAPHICS              DC.B    -1   ;EQ => NO GRAPHICS, NE => GRAPHICS ON
		ENDC
REMASTER              DC.B    -1  ; remaster/modern mode                

               ALIGN
M_STACK               DC.L    0   ;MSGWRITE PUTS RETURN ADDRESS HERE
LJPOS                 DC.W    0    
PLURAL                DC.W    0
CANTSAVE        DC.W    0       ; suspend autosave
  
        
       IFNE     YCHEAT

  XDEF   CHEAT,CHEAT2,CHEAT.LI,DBUG.ON,CHEATNUM



CHEAT                 DC.W    0
CHEAT2                DC.W    0     ;DOESN'T NOBBLE VALID
CHEAT.LI              DC.W    0     ;LET THERE BE LIGHT..
DBUG.ON               DC.W    0     ;0 = NO CHEAT OR DEBUGBITS!
CHEATNUM              DC.W    0     ;NE => print message numbers at FMSG
       
       ENDC
       
PLY.TIED              DC.W    0    ;LIMIT TO NO OF LEVELS OF TIED-NON LOCAL OBJ
ADVERB                DC.W    0
WHOOPS                DC.W    0    ;SET TO TELL PROCESS THAT VERB RECONS
*                                  ;WHOLE COMAND IS SCREWED
PRONHIM               DC.W    0
PRONHER               DC.W    0  
WRDPNT                DC.L    0    ;POINTER TO CURRENT/LAST WORD IN INBUFF
LISWRD
LISTWRD          DC.W    M.AND  ;MSG # USED BY P.LIST AT END OF MSG (NORM AND)
SUBOBJ           DC.W    0      ;FLAG TO USE OBJECTIVE PRONOUNS (PRT.SUB/SUBOBJ)
GNGTEMP          DC.W    0        ;GET NOUN GROUP FLAG
TMPPNTR          DC.L    NOUNBUF  ;TEMP POINTER TO NOUN BUFFER
FROMWCH               DC.W    0
BRKWCH                DC.W    0
 
RELSTK                DC.L    0   ;STACK POINTER SAVED IN RELPHRASE STUFF?
EXAMPLE               DC.W    0   ;GOD ONLY KNOWS!!
DEPTH                 DC.W    0   ;COUNT OF NO. OF PLY RELPHRASES WE ARE AT
 
PULFAIL9              DC.L    0   ;SOME RET ADDR USED IN RELPHRASES (PULFAIL)
 
 
LASTNPC               DC.W    0   ;LAST NPC WE TALKED TO?
ADJFLG                DC.W    0   ;FLAG USED IN SEARCH
 
OLDVERB               DC.W    -1
C.DATA
C.STRENG              DC.B    $7F,$00
C.HP                  DC.B    $7F,$00
C.STRMOD              DC.W    0
C.STATUS              DC.W    0
C.TIME                DC.W    0
RIDING                DC.W    0        
GETNUM                DC.W    0
CHARCOUN              DC.W    0
ADJNOUN               DC.W    0
PEND                  DC.W    0
PEND2                 DC.W    0   ;CONJ USED IN A 'CONJ WHAT' FROM CHK4PART
VERB2                 DC.W    0
SCRPNT                DC.W    0
ADDPNTR               DC.L    0
CONJPNTR              DC.L    0
LIGHT                 DC.W    0 
POS                   DC.W    0
TIMERS
TIMER0         DC.B       $00     ;DON'T USE
TIMER1         DC.B       $00
TIMER2         DC.B       $00
TIMER3         DC.B       $00
TIMER4         DC.B       $00
TIMER5         DC.B       $00
TIMER6         DC.B       $00
TIMER7         DC.B       $00
TIMER8         DC.B       $00
TIMER9         DC.B       $00
TIMERA         DC.B       $00
TIMERB         DC.B       $00
TIMERC         DC.B       $00
TIMERD         DC.B       $00
TIMERE         DC.B       $00
TIMERF         DC.B       $00
 
NEG_                  DC.W    0
WIDTH                 DC.W    WIDTH.DEF ;DEFAULT WIDTH
CWID_FLG              DC.W    0          ;FLAG for wide colour text

       IFEQ     QL128
RIGHTM                DC.W    WIDTH.DEF-1
       ENDC

       IFNE     QL128
RIGHTM                DC.W    WIDTH.DEF-4
       ENDC

LEFTM.SV              DC.W    0            ;LEFT MARGIN SAVE HERE WI SAVED GAME
DISP.SV               DC.W    0            ;DISPLAY TYPE
 
STOPRON               DC.L    0
RNOUNRES              DC.W    0
RNOUNLST              DC.W    0
SEATED                DC.W    0

LASTA0                DC.L    0   ;!!!! LASTA0 IS 6BYTES!! (A0/D0 OF LAST
                      DC.W    0   ;            NOUN VALID CONSIDERED)

IGNORQ                DC.W    0   ;LAST IGNORED QUANTIFIER
IGNORAJ               DC.W    0   ;LAST IGNORED ADVERB
SCORE_                DC.W    0
  
  
SP3.FLAG              DC.W    0   ;SUPPRESS SCAN'S MESSAGES
SCANFLAG              DC.W    0   ;SUPPRESS HIDDEN FLAG IN VALID
F_PROCBS              DC.W    0
REGSVA5.              DS.B    8
NMOVEFLG              DC.W    0   ;FLAG FOR NONMOVABLE FIXTURE SYSTEMS
NEXITFLG              DC.W    0   ;STOPS PEOPLE DRAGING THE FURNITURE ABOUT !
  
   
MOVE.ADV              DC.W    0
RIDFLG                DC.W    0
F_PWALL               DC.W    0   ;FLAG TO LET SETMODE KNOW ABOUT WALL
NPCINRM               DC.W    0
CONVNPC               DC.L    0
                      DC.L    0
PRINC.CO              DC.W    0   ;COUNT COP-OUTS
F_PORT                DC.W    0   ;LET SPECIALS TALK TO SETMODE
F_PORT2               DC.W    0
JON.FLG               DC.W    0
F_GURU                DC.W    0
F_ROSE                DC.W    0
F_GUARDS              DC.W    0
F_KRONOS              DC.W    0
F_ADV                 DC.W    0
COUNT.AD              DC.W    0
F_PDDOOR              DC.W    -1  ;STAGE OF PORTER SPEC.
F_KMEET2              DC.W    0
F_KMEET3              DC.W    0
F_DRAG                DC.B    0   ; DITTO
F_DRAG2               DC.B    0
F_DRAG3               DC.B    0
F_DEVIL               DC.B    0
COUNT.DE              DC.W    0
F_ALCH                DC.B    0   ;ALCHEMISTS STATE (-1=LET PAST)
F_ALCH2               DC.B    0   ;REALLY A COUNTER
F_LIFTUP              DC.B    0   ;SET WHEN LIFT ARRIVES
F_LIFTP               DC.B    0   ;SET WHEN LIFT AT TOP
F_VSHAFT              DC.B    0   ;SET WHEN LAVA FLOODED
F_BOULDE              DC.B    0
WASMOD2               DC.W    $200
F_COLD                DC.W    0    ;HOW MUCH WE'VE FROZEN SO FAR..
 
F_AERO                DC.B    1    ;STATE OF AEROSOUL (HOW MUCH USED)
F_KRDEAT              DC.B    0    ;1 AS KRONOS DIES - FF WHEN SOUL ZAPPED
F_TRADE               DC.W    0    ;<0 = BOUGHT FROM JOHN
F_ADVCOL              DC.B    0    ;ADV BEEN DEAD TO LONG TO GET SOUL
F_COLOUR              DC.B    0    ;USED BY MIX - BIT 2=RED,1=BLUE,0=GREEN
F_PRFOLL              DC.B    0    ;USED WITHIN SETMODE FOR PRINCESS
F_LIFT                DC.B    0
               ALIGN

KM.ROOM        DS.B  2             ;ROOM WHERE U FIRST MEET KRONOS
KM.TIME        DS.B  2             ;MOVE NO. OF SAME
   
               ALIGN
 
*--------------------------------
*
*  TBL.ATTA
*
* FORMAT:     /NOUN#,/MSG#
*
* IF MSG# HIGH BIT IS LOW U DIE!
*
*--------------------------------
  
 
TBL.ATTA
       
              DC.W    NALCH,228+HI     ;U ALSO GET THROWN OUT THE ROOM
              DC.W    NSNOWMAN,39+HI
              DC.W    NGURU,4
              DC.W    NPORTER,217+HI
              DC.W    NGUARDS,88
              DC.W    NDRAGON,77
SP.AT.HJ
              DC.W    NJOHN,60+HI ;GETS CHANGED - DIE SECOND TIME!
SP.AT.AD
              DC.W    NADVENT,43+HI ;   DITTO
SP.AT.DE
              DC.W    NDEVIL,178+HI ;    DITTO
              DC.W    NDEMONS,69
              DC.W    NLEWIS,285
              DC.W    NKRONOS,92+HI  -HUGH
              DC.W    NHORSE,215
              DC.L    0
 
 
*--------------------------------
*
*  CHK.LIST - LIST OF WORDS
*             USED BY THE VARIOUS
*             CHECKSUM ROUTINES
*             TO ENSURE VALID 
*             LOAD GAME
*
*--------------------------------
 
 
SUM.CHK               DC.W    0            ;SIMPLE ADDITION
EOR.CHK               DC.W    0            ;EOR
MULT.CHK              DC.W    0            ;MULTIPLY EM!
ZERO.CHK              DC.W    0            ;ZERO COUNT
SUML.CHK              DC.L    0            ;REAL SUM (LONG WORD)


      END

