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
*	@(#)pictures.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE


	include	"equates.asm"

	XDEF	PICTURE_INDEX

*
* PICTURES.ASM - the scenario dependant bit of the pictures code
*		 (really data!)


PICTURE_INDEX


	DC.B	5				;ON THE PATH #1
	DC.B	0				;ON THE PATH #2
	DC.B	0				;ON THE PATH #3
	DC.B	0				;VOID #4
	DC.B	28				;FOREST #5
	DC.B	28				;FOREST #6
	DC.B	6				;CLEARING #7
	DC.B	12				;FOREST #8
	DC.B	28				;FOREST #9
	DC.B	28				;FOREST #A
	DC.B	28				;FOREST #B
	DC.B	0				;ON THE PATH #C
	DC.B	0				;ON THE PATH #D
	DC.B	25				;PLAIN #E
	DC.B	24				;WILDERNESS #F
	DC.B	24				;PLAIN #10
	DC.B	30				;BANK #11
	DC.B	11				;BRIDGE #12
	DC.B	0				;CLIFF #13
	DC.B	11				;ON THE BRIDGE #14
	DC.B	1				;PALACE GARDENS #15
	DC.B	0				;GATEWAY #16
	DC.B	0				;MAZE #17
	DC.B	0				;HILLS #18
	DC.B	0				;HILLS #19
	DC.B	0				;HILL #1A
	DC.B	0				;HILLS #1B
	DC.B	20				;FOOTHILLS #1C
	DC.B	8				;INSIDE THE HUT #1D
	DC.B	0				;IN THE SHED #1E
	DC.B	0				;TRACK #1F
	DC.B	0				;TRACK #20
	DC.B	10				;TRACK #21
	DC.B	0				;PLATEAU #22
	DC.B	0				;PLATEAU #23
	DC.B	15				;PLATEAU #24
	DC.B	0				;PLATEAU #25
	DC.B	0				;INSIDE THE TOWER #26
	DC.B	0				;LANDING #27
	DC.B	9				;ROOM OF INCARCERATION #28
	DC.B	0				;OUTSIDE WALL #29
	DC.B	0				;STORE ROOM #2A
	DC.B	0				;CAVEMOUTH #2B
	DC.B	0				;CAVERN #2C
	DC.B	22				;CAVERN #2D
	DC.B	0				;CAVE #2E
	DC.B	0				;CORRIDOR #2F
	DC.B	0				;VERTICAL SHAFT #30
	DC.B	0				;CAVERN #31
	DC.B	0				;ENTRANCE #32
	DC.B	0				;CORRIDOR #33
	DC.B	0				;RIVER #34
	DC.B	0				;ROOM #35
	DC.B	0				;PASSAGE #36
	DC.B	7				;LABORATORY #37
	DC.B	29				;ROOM #38
	DC.B	0				;PASSAGE #39
	DC.B	4				;HELL #3A
	DC.B	23				;CAVERN #3B
	DC.B	0				;CHAMBER #3C
	DC.B	0				;ROOM #3D
	DC.B	16				;ROOM #3E
	DC.B	0				;SHAFT #3F
	DC.B	0				;LEDGE #40
	DC.B	2				;BRIDGE #41
	DC.B	0				;BRIDGE #42
	DC.B	0				;LEDGE #43
	DC.B	14				;CHAMBER #44
	DC.B	3				;WORKSHOP #45
	DC.B	0				;PLATFORM #46
	DC.B	27				;TUNNEL #47
	DC.B	26				;ROOM #48
	DC.B	13				;LEDGE #49
	DC.B	0				;PASSAGE #4A
	DC.B	17				;SHAFT #4B
	DC.B	0				;LEDGE #4C
	DC.B	0				;UP THE TREE #4D
	DC.B	0				;TREE TRUNK ROOM #4E
	DC.B	19				;STAIRCASE ROOM #4F
	DC.B	0				;LOW CEILING ROOM #50
	DC.B	0				;CROSSROADS #51
	DC.B	0				;NE-SW PASSAGE #52
	DC.B	18				;LOUNGE #53
	DC.B	0				;KITCHEN #54
	DC.B	0				;SE-W PASSAGE #55
	DC.B	0				;LIFT #56
	DC.B	0				;ROCKFACE #57
	DC.B	0				;WHITE ROOM #58
	DC.B	0				;OFFICE #59
	DC.B	21				;voting booth #5A
	DC.B	0				;DEATH CHAMBER #5B!
	DC.B	0				;PALACE #5C
	DC.B	0				;LEDGE(DEATH CHAMBER) #5D
	DC.B	0				;RIVERSIDE CHAMBER #5E


		END

