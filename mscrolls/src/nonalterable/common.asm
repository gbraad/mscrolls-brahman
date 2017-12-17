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
*	@(#)common.asm	2.28
*	7/28/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*      SRC.ADV.COMMON1
*
*--------------------------------
   
*--------------------------------
*
* COMMON ROUTINES - *NOT* SAVED
* WITH SAVE/LOAD GAME
*
* NOT TO BE MODIFIED BY PROG!!
*
*--------------------------------
 
	include	"equates.asm"
	include	"macros.asm"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"verbequ.i"
	include	"roomequ.i"

	IFEQ	THE_PAWN
	include "adjequ.i"
	ENDC


      XDEF   RelBuf,RelPnt,RelPnt2

      IFNE   SpoolLen

      XDEF   SpoolPos,SpoolOff,PrtThere

      ENDC

      XDEF   BUFSIZE,NOUNBUF,NOUNBUF2,NOUNBUF3,EXBUFF,TEMPBUF
      XDEF   MATCHBUF,OLDBUF,ADJBUF,ADDBUF,CONJBUF,LOOPBUF
      XDEF   STOWRD,DNOUNBUF,WRDTABLE,ST_
      XDEF   YDONT,HAVE,SUCCD,MU,SYN_L,MULT.TAB
      XDEF   VLIST.GO,VLST.GO2,VLEN.GO,VLEN.GO2,L.WHSAT,VLST.INV
      XDEF   VLEN.INV,SINGV.TB,VLN.SING,INIT.TAB,MLISTNGO,L.DATA,L.DATA2
      XDEF   MOVE.TAB,PP.OP.CL,DOORS,TBL.DOOR,SP.JVTBL,JVTBLEND
      XDEF   VLT.DARK,VLN.DARK,MVERB_L,VLIST.2P,TWOPV_L,GLV.L_,GLV_L  ;EQUIV
      XDEF   BRK.VERB,VLST2.P
      XDEF   OLDTEXT,INBUFF,XRAY_VLIST,SING.EX,FoundTO,QuoteFlag

FoundTO		DC.B	0
QuoteFlag	DC.B	0		;bit 0 NE => need to print an end quote

BUFSIZE        EQU  100
 
NOUNBUF        DS.B  BUFSIZE
NOUNBUF2       DS.B  BUFSIZE/2 
NOUNBUF3       DS.B  BUFSIZE/2  
EXBUFF         DS.B  BUFSIZE/2   

      IFNE	TimeCode

	XDEF	NumIndex,NumBuf,NumBUFSIZE,NumFlag,WhichNum,Waiting

NumBUFSIZE	EQU	40		;size of numbers buffer
Waiting		DC.W	0		;how long to wait
NumIndex	DC.W	0		;INPUTFRM's position in it
WhichNum	DC.W	0		;pos in buffer of next to use
NumFlag		DC.W	0		;Flag used creating said buffer
NumBuf		DS.B	NumBUFSIZE	;the buffer

      ENDC
   
	        DC.W  -1               ;fake terminator, so as stuff searching
	                               ;back up this stack has something to hit
RelBuf         DS.B  256              ;going to stick all nouns considered by
	                               ;relphrase handlers here, so as we can
	                               ;look back to resolve which xyzzy
RelPnt         DC.W  0                ;Pointer into above
RelPnt2        DC.W  0                ;temp version of the pointer

      IFNE     SpoolLen

SpoolPos       DC.L  0                ;current pos in printer buffer of stuff
	                               ;going into it
SpoolOff       DC.L  0                ; .. and of stuff going out
PrtThere       DC.W  0                ;have we found a printer

      ENDC
  
TEMPBUF        DS.B  BUFSIZE
MATCHBUF       DS.B  BUFSIZE	;WORST CASE - DOOR = 8 CASES
OLDBUF         DS.B  BUFSIZE
ADJBUF         DS.B  50
ADDBUF         DS.B  20
CONJBUF        DS.B  20
LOOPBUF        DS.B  26
STOWRD         DS.B  BIGWRD
DNOUNBUF       DS.B  BUFSIZE
  

 
OLDTEXT         DS.B  BUFFLEN
INBUFF          DS.B  BUFFLEN

 
WRDTABLE
	       DC.W    YDONT-ST_
	       DC.W    HAVE-ST_
	       DC.W    SUCCD-ST_
 
 
ST_
YDONT
	DC.B       'don''t '              ;I DON'T
	DC.B       $00
	DC.B       'doesn''t '
	DC.B       $00
HAVE
	DC.B       'have '             ;YOU HAVE 
	DC.B       $00
	DC.B       'has '
	DC.B       $00
SUCCD
	DC.B       'succeed@ '           ;YOU SUCCEED    
	DC.B       $00
	DC.B       'succeeds '
	DC.B       $00
 
MU     ALIGN
 
*--------------------------------
*
* SYN.L - TABLE OF SYNONYMS
*
* FORMAT - WORD,WORD
*
* FIRST WORD = ITEM NO IN SECTION
*              =======
*
* THUS IT IS NOT THE NOUN# ETC
*
* SECOND WORD = SECTION NO.
*
* SECTION NO.S     0-QUANTIFIERS
* 1-ADVERBS..    2-CONJUCTIONS
* 3-SPECIALS     4-VERBS
* 5-ADJ          6-NOUNS
* 7-DIRECTIONS   8-CONTROL
* 9-QUESTIONS   10-TO BE
* 11-SYNONYMS(!)
*
*--------------------------------

SYNMACRO      MACRO

	       DC.W        (\1)*$20+\2

	       ENDM

	IFNE	YCHEAT&VAX

SYNMACRO2     MACRO

	       DC.W        (\1)*$20+\2

	       ENDM
	
	ENDC

	IFEQ	YCHEAT&VAX

SYNMACRO2	MACRO

		ENDM

	ENDC

	ALIGN

SYN_L

	INCLUDE	"miscsyms.asm"
	INCLUDE	"nounsyms.i"

	XDEF	SYN_L_Len
SYN_L_Len	EQU	(*-SYN_L)/2



*--------------------------------
*
* MULT.TAB - zero terminated lists of nouns (noun#'s - words)
*            in each set (multiple) as wtable12 order
*
*********************************

	ALIGN


MULT.TAB

	INCLUDE	"sets.i"


*--------------------------------
*
*  VLIST.GO - VERBS WHICH MAY BE
*             FOLLOWED BY A 
*             DIRECION N,S,E,W..
*
*--------------------------------
 
 
VLIST.GO
 
	       DC.B    -1       ;JUST DIR GIVEN
 
VLST.GO2
 
* VLST.GO2 - AS USED BY VALID

	SVERB	GO
	SVERB	LEAVE
	SVERB	WALK
	SVERB	ENTER
	SVERB	RUN
	SVERB	EXIT
	SVERB	CLIMB
	SVERB	JUMP 
	SVERB	PADDLE
	
VLEN.GO       EQU     *-VLIST.GO-1
VLEN.GO2      EQU     *-VLST.GO2-1


L.WHSAT
	SVERB	SAY
	SVERB	SHOUT
	SVERB	CALL
	SVERB	ASK
	SVERB	TELL
	SVERB	LOOK
	SVERB	EXAMINE
	SVERB	LISTEN
	SVERB	THROW
	SVERB	SUSS
	SVERB	SIT
	DC.B	-1
*
* List of verbs where the INDIRECT object must a) be reachable and
* b) not owned by an npc.
*
	
	XDEF	VLIST.NOREACHD5

VLIST.NOREACHD5

	SVERB	PUT
	SVERB	STICK
	SVERB	FIX
	SVERB	TIE
	SVERB	DROP
	SVERB	EMPTY
	SVERB	FILL
	SVERB	POUR
	SVERB	INSERT
	SVERB	LOCK
	SVERB	UNLOCK
	SVERB	OPEN
	SVERB	CLOSE

	DC.B	-1

*--------------------------------
*
* VLST.INV -  VERBS WHICH REFER
*             TO ALL ON INVENT
*             RATHER THAN ALL IN
*             ROOM WHEN USED WITH
*             ALL.
*
*--------------------------------

VLST.INV

	SVERB	DROP
	SVERB	LEAVE
	SVERB	INSERT
	SVERB	FIT
	SVERB	FILL
	SVERB	TIE
	SVERB	FIX
	SVERB	GIVE
	SVERB	SHOW
	SVERB	FEED
	SVERB	DRINK
	SVERB	EAT
	SVERB	OPEN
	SVERB	CLOSE
	SVERB	LOCK
	SVERB	UNLOCK
	SVERB	PUT
	SVERB	THROW
	SVERB	ROLL
	SVERB	SHAKE
 
VLEN.INV	EQU	*-VLST.INV-1	;NO. OF VERBS IN LIST
  
	ALIGN
  
*--------------------------------
*
* SINGV.TAB - LIST OF VERBS WHICH
*             ARE ALLOWED WITH
*             NO NOUNS, EG, I
*             L, STATUS ETC..
*
*--------------------------------

SINGV.TB
 
	SVERB	INVENTORY
	SVERB	STATUS
	SVERB	DEBUG
	SVERB	EXIT
	SVERB	JUMP
	SVERB	EXITS
	SVERB	WAIT
	SVERB	SIT
	SVERB	GET
	SVERB	STAND
	SVERB	RIDE
	SVERB	DISMOUNT
	SVERB	HELP
	SVERB	LAUGH
	SVERB	LISTEN
	SVERB	KICK
	SVERB	PUNCH
	SVERB	VOTE
	SVERB	KNOCK
	SVERB	PRAY
	SVERB	SLEEP
	SVERB	DIG
	SVERB	SWIM

; Added for GM

	SVERB	DIAL
	SVERB	STAY
	SVERB	FOLLOW

; In GM, smell is allowed as a singv.

	IFD	GreenMagic
	SVERB	SMELL
	ENDC
 
VLN.SING      EQU       *-SINGV.TB-1 ;NO. OF VERBS (-1)

*--------------------------------
*
*	singv exeptions
*	eg. get
*	can say  get {on/off/in} but not just 'get'
*

SING.EX

	SVERB	GET
	DC.B	0		;zero terminated
				;NB: does not apply to LOOK

	ALIGN

*--------------------------------
*
*  LOOK TABLES - ASCII !!
*
*--------------------------------
   
 
L.DATA

	WORD   <'LOCKED '>
	WORD   <'CLOSED '>
	WORD   <'OPEN '>
	DC.B   $00           ;NULL
	WORD   <'DAMAGED '>
	WORD   <'DEAD '>
	DC.B   $00           ;NULL

L.DATA2

	WORD   <'HAPPY'>
	WORD   <'ANGRY'>
	WORD   <'VIOLENT'>
	WORD   <'ASLEEP'>
	WORD   <'UNCONSCIOUS'>
	WORD   <'PEACEFUL'>
	WORD   <'EXCITED'>
	WORD   <'FRIENDLY'>
	WORD   <'DRUNK'>
	WORD   <'SAD'>
	WORD   <'TIRED'>
	WORD   <'SUFFERING'>
	WORD   <'FRIGHTENED'>
	WORD   <'#2'>
	WORD   <'MISCHEVIOUS'>
	ALIGN

 
*--------------------------------
*
* VLT.DARK -   VERBS WHICH GO
*              BUMP IN THE NIGHT
*
* CAN'T USE THESE IN A DARK ROOM
* EVEN IF THEY ARE LIT,ON INV,ETC
*
*--------------------------------
 
 
VLT.DARK
	SVERB	LOOK
	SVERB	EXITS
	SVERB	EXIT
	SVERB	EXAMINE 

VLN.DARK	EQU	*-VLT.DARK-1	;NO OF ALLOWED VERBS (-1)
  
*--------------------------------
	
MVERB_L
 
	SVERB	PUT             ;PUT X DOWN
	SVERB	PICK            ;PICK X UP
	SVERB	TOUCH           ;TOUCH X UP
	SVERB	PULL            ;PULL X UP/DOWN
	SVERB	LIFT
	SVERB	CLIMB
	      
	       DC.B    -1
	
*--------------------------------
	
VLIST.2P
VLST2.P
*VERBS TAKING 2PARTS ONLY
	
	SVERB	LOOK
	SVERB	EXAMINE
	SVERB	GET
	SVERB	TAKE
	SVERB	CARRY

	DC.B    -1
	 
*--------------------------------
	
	ALIGN

*--------------------------------
	
TWOPV_L
	
*LIST OF TWO WORD VERBS
	
*FORMAT:
* PARTICLE.W,NEWVERB.B,OLDVERB.B
	
*WHERE THE PARTICLE IS IN
*ADVERB TESTABLE FORMAT
*IE
* PL= CONJ WORD
* MI= DIRECTION

	IfVerb	MOVE

	DC.W	TO
	DC.B	VNENTER,VNMOVE		;move to == enter

	ENDC

	IfVerb	ENTER

	DC.W	INTO
	DC.B	VNENTER,VNGET		;get into == enter

	ENDC

	IfVerb	PICK	

	DC.W    DIR_U+HI            ;PICK UP = GET
	DC.B    VNGET,VNPICK

	ENDC

	IfVerb	CLIMB

	DC.W    DIR_U+HI            ;CLIMB UP = CLIMB
	DC.B    VNCLIMB,VNCLIMB

	ENDC

	IfVerb	CLIMB

	DC.W    DIR_D+HI            ;DITTO CLIMB DOWN (ESP. ROPE)
	DC.B    VNCLIMB,VNCLIMB

	ENDC

	IfVerb	WEAR

	DC.W    DIR_U+HI            ;PULL X UP = WEAR
	DC.B    VNWEAR,VNPULL

	ENDC

	IfVerb	PULL

	DC.W    DIR_D+HI            ;PULL X DOWN = REMOVE
	DC.B    VNREMOVE,VNPULL

	ENDC

	IfVerb	LIFT

	DC.W    DIR_U+HI            ;LIFT X UP = LIFT
	DC.B    VNLIFT,VNLIFT

	ENDC

	IfVerb	RAPE
	
	DC.W    DIR_U+HI            ;KNOCK UP = RAPE
	DC.B    VNRAPE,VNKNOCK

	ENDC

	IfVerb	MAKELOCAL		;should take prioity if avail

	DC.W	FOR			;look for == makelocal
	DC.B	VNMAKELOCAL,VNLOOK

	DC.W	TO
	DC.B	VNMAKELOCAL,VNGO	;go to == makelocal (safe?)

	ENDC

	IfVerb	FIND

	DC.W    FOR                 ;LOOK FOR = FIND
	DC.B    VNFIND,VNLOOK

	ENDC

	IfVerb	RIDE

	DC.W    ON                  ;GO ON = RIDE
	DC.B    VNRIDE,VNGO


	DC.W    DIR_U+HI            ;GO UP = RIDE
	DC.B    VNRIDE,VNGO


	DC.W    ON                  ;GET ON = RIDE
	DC.B    VNRIDE,VNGET


	DC.W    ON
	DC.B    VNRIDE,VNJUMP

	ENDC

	IFND	VNRIDE			;ie if ride is out

	DC.W    ON                  ;GO ON = CLIMB
	DC.B    VNCLIMB,VNGO


	DC.W    DIR_U+HI            ;GO UP = CLIMB
	DC.B    VNCLIMB,VNGO


	DC.W    ON                  ;GET ON = CLIMB
	DC.B    VNCLIMB,VNGET


	DC.W    ON
	DC.B    VNCLIMB,VNJUMP

	ENDC

	IfVerb	DISMOUNT

	DC.W    DIR_D+HI            ;GET DOWN = DISMOUNT
	DC.B    VNDISMOUNT,VNGET


	DC.W    DIR_U+HI            ;GET UP = DISMOUNT
	DC.B    VNDISMOUNT,VNGET


	DC.W    OFF                 ;GET OFF = DISMOUNT
	DC.B    VNDISMOUNT,VNGET

	ENDC

	IFND	DISMOUNT

	DC.W    DIR_D+HI            ;GET DOWN = STAND
	DC.B    VNSTAND,VNGET


	DC.W    DIR_U+HI            ;GET UP = STAND
	DC.B    VNSTAND,VNGET


	DC.W    OFF                 ;GET OFF = STAND
	DC.B    VNSTAND,VNGET

	ENDC

	IfVerb	PUT

	DC.W    DIR_D+HI            ;PUT DOWN = LEAVE
	DC.B    VNLEAVE,VNPUT


	DC.W    ON                  ;PUT ON = WEAR
	DC.B    VNWEAR,VNPUT

	ENDC

	IfVerb	THROW

	DC.W    OFF                 ;THROW OFF = REMOVE
	DC.B    VNREMOVE,VNTHROW


	DC.W    DIR_D+HI            ;THROW DOWN = DROP
	DC.B    VNDROP,VNTHROW

	ENDC

	IfVerb	MOVE	

	DC.W    IN
	DC.B    VNGO,VNMOVE

	ENDC

	IfVerb	JUMP

	DC.W    IN
	DC.B    VNGO,VNJUMP

	ENDC

	IfVerb	REMOVE	

	DC.W    OFF
	DC.B    VNREMOVE,VNTAKE

	ENDC

	IfVerb	LEAVE	

	DC.W    VNEXIT
	DC.B    VNLEAVE,VNGO ;FUDGE - VNEXIT = OUT,GO OUT = LEAVE

	ENDC


	DC.L    0
	
	
*--------------------------------

GLV_L       
GLV.L_
	
*GLOBAL VERB LIST
* VERBS WHOSE DO IS 'GLOBAL'

	SVERB	FIND
	SVERB	VOTE
	SVERB	WAIT       
	SVERB	MAKELOCAL
	SVERB	TRACE
	SVERB	NPCDEBUG
	SVERB	STACKTRACE
	SVERB	SNOOP
	SVERB	MINDREAD

	DC.B	0
	
	
*--------------------------------
*
* LIST OF VERBS WHICH CANNOT
* BE USED ON BROKEN ITEMS
*
*--------------------------------
 
BRK.VERB
 
	SVERB	PULL
	SVERB	FIX
	SVERB	TIE
	SVERB	JOIN
	SVERB	BREAK
	SVERB	SMASH
	SVERB	DAMAGE
	SVERB	LIGHT
	SVERB	OPEN
	SVERB	CLOSE
	SVERB	TWIST
	SVERB	TURN
	SVERB	WEAR
	SVERB	UNLOCK
	SVERB	PICK
	SVERB	SLIDE
	SVERB	CLIMB
	SVERB	HIT

	DC.B    0
 


*--------------------------------------- 

XRAY_VLIST

* a zterm list of verbs allowing refernece to the 
* contents of countainers that are not get wo opening.

	SVERB	BUY
	SVERB	GIVE

	DC.B	0
*
* TRANS_VLIST
*
* a -1 term list of verbs which may be used on nouns which are inside closed
* and transparent containers
* 

	XDEF	TRANS_VLIST

TRANS_VLIST

	SVERB	LOOK
	SVERB	EXAMINE
	SVERB	SUSS

	SVERB	READ

	SVERB	SAY
	SVERB	SHOUT
	SVERB	CALL
	SVERB	YELL
	SVERB	CRY

	SVERB	SING

	SVERB	ASK
	SVERB	QUESTION
	SVERB	LISTEN

*	SVERB	FEED		;could cause hassle either way
*	SVERB	GIVE

	DC.B	-1		;terminator


*--------------------------------------- 

      IFEQ	THE_PAWN

	XDEF	NearbyVerbs,VLEN_NBVerb

NearbyVerbs

* These are verbs that can be used with nouns which are in (directly or
* indirectly) nearby rooms (ie: adjacent rooms)

*	SVERB	THROW		;more hassle than it's worth now
	SVERB	SHOUT
	SVERB	FOLLOW

VLEN_NBVerb	EQU	*-NearbyVerbs-1

      ENDC

*--------------------------------------- 

	ALIGN
	INCLUDE	"scendata.i"


	 END
