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
*	@(#)verblist.asm	2.19
*	7/28/87
*
******************************************************

		SECTION		"ADV",CODE

	include	"equates.asm"
	include	"verbequ.asm"


	XDEF	VECT.V
	XREF	NOVERB


UseVerb	MACRO	* verbequ, addr. of code [,must be TRUE expression]

	IFD	VN\1

	 IFC	'','\3'
	  IFND	\2
	   XREF	\2
	  ENDC
	  DC.W	\2-$8000
	 ENDC

	 IFNC	'','\3'
	 AnOther	\1,\2,\3
	 ENDC

	ENDC

	ENDM

AnOther	MACRO
	  IFNE	\3
	   IFND	\2
	    XREF	\2
	   ENDC
	   DC.W	\2-$8000
	  ENDC
	  IFEQ	\3
	  DC.W	NOVERB-$8000
	  ENDC
	ENDM
*
* VECT.V - used by GETVECT to calculate the addr. of verb routines
*

	DC.W	NOVERB-$8000		;incase VERB(A4)=-1

VECT.V	

	UseVerb	LOOK,LOOK
	UseVerb	EXAMINE,LOOK
	UseVerb	GET,GET_
	UseVerb	TAKE,GET_
	UseVerb	CARRY,GET_
	UseVerb	DROP,DROP
	UseVerb	LEAVE,LEAVE
	UseVerb	GO,GO.N
	UseVerb	WALK,GO.N
	UseVerb	ENTER,GO.N
	UseVerb	RUN,GO.N
	UseVerb	SHIFT,V.MOVE
	UseVerb	MOVE,V.MOVE
	UseVerb	LIFT,V.LIFT
	UseVerb	RAISE,V.LIFT
	UseVerb	LOWER,V.LOWER
	UseVerb	PUSH,V.PUSH
	UseVerb	PULL,V.PULL
	UseVerb	SMELL,V.SMELL
	UseVerb	TASTE,V.TASTE
	UseVerb	EAT,V.EAT
	UseVerb	DRINK,V.DRINK
	UseVerb	PUT,V.PUT
	UseVerb	INSERT,V.PUT
	UseVerb	INVENTORY,INVENT
	UseVerb	FIT,FIX
	UseVerb	POUR,NOVERB
	UseVerb	EMPTY,EMPTY
	UseVerb	FILL,V.FILL
	UseVerb	MIX,V.MIX
	UseVerb	JUMP,V.JUMP
	UseVerb	VAULT,V.VAULT
	UseVerb	THROW,THROW
	UseVerb	PUNCH,V.PUNCH
	UseVerb	KICK,V.KICK
	UseVerb	FIGHT,FIGHT
	UseVerb	ATTACK,FIGHT
	UseVerb	KILL,V.KILL
	UseVerb	BREAK,V.BREAK
	UseVerb	SMASH,V.BREAK
	UseVerb	DAMAGE,V.BREAK
	UseVerb	REMOVE,REMOVE
	UseVerb	SCRAPE,REMOVE
	UseVerb	LIGHT,V.LIGHT
	UseVerb	UNLIGHT,V.UNLIGHT,THE_PAWN-TRUE
	UseVerb	EXTINGUISH,V.UNLIGHT,THE_PAWN-TRUE
	UseVerb	OPEN,OPEN
	UseVerb	CLOSE,CLOSE
	UseVerb	SUSS,SUSS,YCHEAT
	UseVerb	STATUS,NOVERB
	UseVerb	DEBUG,V.CHEAT
	UseVerb	FIRE,V.FIRE
	UseVerb	SHOOT,V.FIRE
	UseVerb	BLAST,V.FIRE
	UseVerb	TWIST,V.TWIST
	UseVerb	TURN,V.TURN
	UseVerb	SAY,SAY
	UseVerb	SHOUT,SAY
	UseVerb	YELL,SAY
	UseVerb	CRY,SAY
	UseVerb	CALL,SAY
	UseVerb	STRANGLE,V.STRANG
	UseVerb	CHOKE,V.STRANG
	UseVerb	FEEL,V.TOUCH
	UseVerb	TOUCH,V.TOUCH
	UseVerb	WEAR,WEAR
	UseVerb	READ,V.READ
	UseVerb	HIDE,V.HIDE
	UseVerb	HIT,HIT
	UseVerb	STEAL,V.STEAL
	UseVerb	BASH,HIT
	UseVerb	UNLOCK,UNLOCK
	UseVerb	PICK,PICK
	UseVerb	TIE,FIX
	UseVerb	FIX,FIX
	UseVerb	JOIN,V.JOIN
	UseVerb	UNTIE,UNTIE
	UseVerb	FEED,V.FEED
	UseVerb	BITE,V.BITE
	UseVerb	USE,NOVERB
	UseVerb	CROSS,V.CROSS
	UseVerb	SLIDE,V.SLIDE
	UseVerb	BLOW,V.BLOW
	UseVerb	PRESS,V.PRESS
	UseVerb	GIVE,GIVE
	UseVerb	CLIMB,GO.N
	UseVerb	BURN,V.LIGHT
	UseVerb	IGNITE,V.LIGHT
	UseVerb	CAST,V.CAST
	UseVerb	FOD,V.FOD,YCHEAT
	UseVerb	EXIT,V.EXIT
	UseVerb	ASK,V.ASK
	UseVerb	QUESTION,V.ASK
	UseVerb	MELT,V.MELT
	UseVerb	LISTEN,V.LISTEN
	UseVerb	SEARCH,V.FIND
	UseVerb	FIND,V.FIND
	UseVerb	GRAB,V.GRAB
	UseVerb	RUB,V.RUB
	UseVerb	SMOKE,V.SMOKE
	UseVerb	WAIT,V.WAIT
	UseVerb	WAKE,V.WAKE
	UseVerb	RAPE,V.RAPE
	UseVerb	MEND,V.MEND
	UseVerb	LEVER,V.LEVER
	UseVerb	BUY,V.BUY
	UseVerb	PURCHASE,V.BUY
	UseVerb	LEAN,V.LEAN
	UseVerb	RIDE,V.RIDE
	UseVerb	PLANT,V.PLANT
	UseVerb	LOCK,V.LOCK
	UseVerb	TEAR,V.TEAR
	UseVerb	RIP,V.TEAR
	UseVerb	CUT,V.CUT
	UseVerb	KNOCK,V.KNOCK
	UseVerb	POINT,V.POINT
	UseVerb	SHINE,V.SHINE
	UseVerb	SPLIT,V.SPLIT
	UseVerb	UNMIX,V.SPLIT
	UseVerb	SEPARATE,V.SPLIT
	UseVerb	EXITS,V.EXITS
	UseVerb	COVER,V.COVER
	UseVerb	UNCOVER,UNTIE
	UseVerb	SIT,V.SIT
	UseVerb	LIE,NOVERB
	UseVerb	KISS,V.KISS
	UseVerb	VOTE,V.VOTE
	UseVerb	STAND,V.STAND
	UseVerb	MOUNT,V.RIDE
	UseVerb	TELL,V.ASK
	UseVerb	OFFER,GIVE
	UseVerb	DISMOUNT,V.DISMOU
	UseVerb	HELP,V.HELP
	UseVerb	LAUGH,V.LAUGH
	UseVerb	SHOW,V.SHOW
	UseVerb	LOOP,V.LOOP,YCHEAT
	UseVerb	DIG,V.DIG
	UseVerb	BET,V.BET
	UseVerb	STOP,V.STOP
	UseVerb	PLAY,V.PLAY
	UseVerb	SWIM,V.SWIM
	UseVerb	FLUSH,V.FLUSH
	UseVerb	PISS,V.PISS
	UseVerb	SLEEP,V.SLEEP
	UseVerb	SET,V.SET
	UseVerb	WIND,V.WIND
	UseVerb	CHEW,V.CHEW
	UseVerb	REFLECT,V.REFLECT
	UseVerb	HOLD,V.HOLD
	UseVerb	PLACE,V.PUT
	UseVerb	POSITION,V.PUT
	UseVerb	ILLUMINATE,V.ILLUMINATE
	UseVerb	CATCH,V.CATCH
	UseVerb	PRAY,V.PRAY
	UseVerb	CUE,HIT
	UseVerb	POT,HIT
	UseVerb	SHAKE,V.SHAKE
	UseVerb	ROLL,V.ROLL
	UseVerb	FISH,V.FISH
	UseVerb	SING,V.SING
	UseVerb MAKELOCAL,V.MAKELOCAL
	UseVerb	STICK,V.PUTON

	UseVerb	DIAL,V.DIAL
	UseVerb	STAY,V.STAY
	UseVerb	FOLLOW,V.FOLLOW
	UseVerb	STRIKE,V.STRIKE
	UseVerb WARM,V.WARM
	UseVerb PADDLE,V.PADDLE

* Anita's verbs (NPCs)

      IFD	wantNEWNPCS
	include	"nnpcequates.asm"

	UseVerb	SNOOP,V.NPCSNOOP,NPCSnoop
	UseVerb	MINDREAD,V.NPCMINDREAD,NPCMindread

	UseVerb	TRACE,V.NPCTRACE,NPCTrace
	UseVerb	NPCDEBUG,V.NPCDEBUG,NPCDebug
	UseVerb	STACKTRACE,V.NPCSTACKTRACE,NPCStackTrace

      ENDC

	END
