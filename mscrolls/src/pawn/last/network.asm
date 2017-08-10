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
*	@(#)network.asm	2.2
*	1/21/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.NETWORK
*
*--------------------------------

	XDEF       NETWORK,NET_END
  
 
*                  N   NE  E   SE  S   SW  W   NW  U   D
 
NETWORK
  
	DC.B	$0C,$0F,$0F,$0F,$02,$05,$0B,$0B,$00,$00 ;ON THE PATH #1
	DC.B	$01,$0F,$0F,$10,$03,$06,$05,$0B,$00,$00 ;ON THE PATH #2
	DC.B	$02,$10,$10,$04,$04,$04,$06,$05,$00,$00 ;ON THE PATH #3
	DC.B	$04,$04,$04,$04,$04,$04,$04,$04,$04,$04 ;VOID #4
	DC.B	$0B,$01,$02,$03,$06,$08,$07,$0B,$FC,$00 ;FOREST #5
	DC.B	$05,$02,$03,$04,$04,$04,$08,$07,$FC,$00 ;FOREST #6
	DC.B	$0B,$0B,$05,$06,$08,$0A,$09,$09,$FC,$00 ;CLEARING #7
	DC.B	$07,$05,$06,$04,$04,$04,$0A,$09,$4D,$00 ;FOREST #8
	DC.B	$1B,$19,$0B,$07,$0A,$0A,$FE,$FB,$FC,$00 ;FOREST #9
	DC.B	$09,$07,$08,$04,$04,$04,$FE,$09,$FC,$00 ;FOREST #A
	DC.B	$18,$0D,$0C,$01,$07,$09,$09,$1B,$FC,$00 ;FOREST #B     
	DC.B	$0D,$0E,$0E,$0F,$01,$0B,$0B,$18,$00,$00 ;ON THE PATH #C
	DC.B	$1C,$0E,$0E,$0F,$0C,$0B,$18,$18,$00,$00 ;ON THE PATH #D
	DC.B	$00,$00,$13,$12,$0F,$0C,$0D,$1C,$00,$00 ;PLAIN #E
	DC.B	$0E,$13,$12,$11,$10,$02,$01,$0C,$00,$00 ;WILDERNESS #F
	DC.B	$0F,$12,$11,$04,$04,$04,$03,$05,$00,$00 ;PLAIN #10
	DC.B	$12,$FA,$FA,$04,$04,$04,$10,$0F,$00,$00 ;BANK #11
	DC.B	$13,$00,$14,$00,$11,$10,$0F,$0E,$00,$00 ;BRIDGE #12
	DC.B	$F7,$F7,$FA,$FA,$12,$0F,$0E,$0E,$00,$00 ;CLIFF #13
	DC.B	$FA,$FA,$15,$FA,$FA,$FA,$12,$FA,$00,$FA ;ON THE BRIDGE #14
	DC.B	$F8,$F8,$16,$F8,$F8,$FF,$14,$F8,$00,$00 ;PALACE GARDENS #15
	DC.B	$F8,$F8,$F2,$F8,$F8,$F8,$15,$F8,$00,$00 ;GATEWAY #16
	DC.B	$17,$17,$17,$17,$17,$17,$17,$17,$00,$00 ;MAZE #17
	DC.B	$F7,$1C,$0D,$0C,$0B,$0B,$19,$1A,$00,$00 ;HILLS #18
	DC.B	$1A,$18,$18,$0B,$0B,$09,$1B,$1B,$00,$00 ;HILLS #19
	DC.B	$F7,$F7,$18,$18,$19,$1B,$1B,$F7,$1D,$00 ;HILL #1A
	DC.B	$F7,$1A,$19,$0B,$09,$09,$09,$F7,$00,$00 ;HILLS #1B
	DC.B	$F7,$F7,$F7,$F7,$0D,$F7,$F7,$EE,$EE,$00 ;FOOTHILLS #1C
	DC.B	$F8,$F8,$1A,$F8,$F8,$F8,$F8,$F8,$00,$00 ;INSIDE THE HUT #1D
	DC.B	$F8,$FF,$F8,$F8,$F8,$F8,$F8,$F8,$00,$00 ;IN THE SHED #1E
*--------------------------------
  
*ROOM 1F
	DC.B	$ED,$00,$00,$1C,$F6,$F6,$F6,$F6,$00,$1C ;TRACK #1F
	DC.B	$F6,$21,$2B,$00,$1F,$F6,$F6,$F6,$21,$1F ;TRACK #20
	DC.B	$F6,$F6,$00,$00,$00,$20,$20,$F6,$22,$20 ;TRACK #21
	DC.B	$F6,$F6,$F6,$23,$23,$EC,$25,$00,$00,$21 ;PLATEAU #22
	DC.B	$22,$22,$F6,$F6,$F6,$F6,$24,$24,$00,$00 ;PLATEAU #23
	DC.B	$25,$26,$23,$00,$00,$00,$00,$25,$00,$00 ;PLATEAU #24
	DC.B	$F6,$F6,$22,$22,$24,$F6,$F6,$F6,$00,$00 ;PLATEAU #25
	DC.B	$F8,$F8,$F8,$F8,$24,$24,$2A,$F8,$27,$00 ;INSIDE THE TOWER #26   
	DC.B	$FF,$FA,$FA,$FA,$FA,$FA,$FA,$FA,$00,$26 ;LANDING #27
	DC.B	$F8,$F8,$29,$F8,$FF,$F8,$F8,$F8,$00,$00 ;ROOM OF INCARCERATION #28    
	DC.B	$FA,$FA,$FA,$FA,$FA,$FA,$28,$FA,$28,$23 ;OUTSIDE WALL #29  
	DC.B	$F8,$F8,$26,$F8,$F8,$F8,$F8,$F8,$00,$00 ;STORE ROOM #2A  
	DC.B	$F8,$F8,$2C,$F8,$F8,$F8,$20,$F8,$00,$00 ;CAVEMOUTH #2B 
	DC.B	$F8,$F8,$2D,$F8,$F8,$F8,$2B,$F8,$00,$00 ;CAVERN #2C 
	DC.B	$F8,$F8,$F8,$F8,$F8,$F8,$2C,$F8,$00,$2E ;CAVERN #2D 
	DC.B	$36,$F8,$2F,$F8,$32,$F8,$F8,$F8,$2D,$00 ;CAVE #2E 
	DC.B	$F8,$F8,$34,$F8,$F8,$F8,$2E,$F8,$00,$00 ;CORRIDOR #2F   
	DC.B	$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$5E,$34 ;VERTICAL SHAFT #30  
	DC.B	$17,$17,$F5,$F5,$34,$F5,$F5,$F8,$00,$00 ;CAVERN #31
	DC.B	$2E,$F8,$F8,$F8,$FF,$F8,$F8,$F8,$00,$00 ;ENTRANCE #32 
	DC.B	$FF,$F8,$F8,$F8,$FF,$F8,$F8,$F8,$00,$00 ;CORRIDOR #33  
	DC.B	$31,$31,$F8,$F8,$F8,$F8,$2F,$F8,$30,$00 ;RIVER #34
*ROOM #$35
	DC.B	$FF,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$00,$00 ;ROOM #35  
	DC.B	$37,$F8,$F8,$F8,$2E,$F8,$F8,$F8,$00,$00 ;PASSAGE #36  
	DC.B	$F8,$38,$F8,$F8,$36,$F8,$F8,$39,$00,$00 ;LABORATORY #37 
	DC.B	$F8,$F8,$F8,$F8,$2F,$37,$F8,$F8,$00,$00 ;ROOM #38
	DC.B	$F8,$F8,$F8,$37,$F8,$F8,$40,$F8,$00,$00 ;PASSAGE #39 
	DC.B	$F1,$F1,$F1,$F1,$F1,$F1,$3B,$F1,$00,$00 ;HELL #3A
	DC.B	$F8,$F8,$3A,$F8,$3C,$F8,$F8,$F8,$00,$3A ;CAVERN #3B 
	DC.B	$3B,$F8,$F8,$F8,$3D,$F8,$3F,$F8,$00,$00 ;CHAMBER #3C  
	DC.B	$3C,$00,$F5,$F5,$F5,$00,$00,$00,$00,$00 ;ROOM #3D  
	DC.B	$F8,$F8,$F8,$F8,$3F,$F8,$F8,$F8,$00,$00 ;ROOM #3E
	DC.B	$3E,$FA,$3C,$FA,$FA,$FA,$FA,$FA,$4B,$00 ;SHAFT #3F
*ROOM $40
	DC.B	$41,$FA,$39,$FA,$FA,$FA,$FA,$FA,$00,$FA ;LEDGE #40  
	DC.B	$42,$FA,$FA,$FA,$40,$FA,$FA,$FA,$00,$FA ;BRIDGE #41
	DC.B	$43,$FA,$FA,$FA,$41,$FA,$FA,$FA,$00,$FA ;BRIDGE #42
	DC.B	$F8,$47,$FA,$FA,$42,$FA,$FA,$44,$00,$FA ;LEDGE #43
	DC.B	$45,$F8,$F8,$43,$F8,$F8,$F8,$F8,$00,$00 ;CHAMBER #44
	DC.B	$46,$F8,$F8,$F8,$44,$F8,$F8,$F8,$00,$00 ;WORKSHOP #45
	DC.B	$45,$45,$45,$45,$45,$45,$45,$45,$00,$3D ;PLATFORM #46
	DC.B	$48,$F8,$F8,$F8,$43,$F8,$F8,$F8,$00,$00 ;TUNNEL #47
	DC.B	$00,$F8,$F8,$F8,$47,$F8,$F8,$F8,$00,$00 ;ROOM #48
	DC.B	$FA,$FA,$4A,$4A,$4A,$F8,$FA,$FA,$00,$4A ;LEDGE #49
	DC.B	$49,$F8,$FF,$F8,$F8,$F8,$49,$49,$00,$00 ;PASSAGE #4A    
	DC.B	$F8,$FA,$FA,$FA,$4C,$FA,$FF,$F8,$00,$3F ;SHAFT #4B 
	DC.B	$4B,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$00,$00 ;LEDGE #4C
	DC.B	$FA,$FA,$FF,$FA,$FA,$FA,$FA,$FA,$EA,$08 ;UP THE TREE #4D 
*ROOM $4E
	DC.B	$F8,$F8,$F8,$F8,$F8,$FF,$F8,$F8,$EA,$00 ;TREE TRUNK ROOM #4E
	DC.B	$F8,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$4E,$50 ;STAIRCASE ROOM #4F
	DC.B	$F8,$F8,$58,$F8,$F8,$F8,$51,$F8,$4F,$00 ;LOW CEILING ROOM #50
	DC.B	$F8,$F8,$50,$F8,$F8,$52,$F8,$55,$00,$00 ;CROSSROADS #51
	DC.B	$F8,$51,$F8,$F8,$F8,$F8,$FF,$F8,$00,$00 ;NE-SW PASSAGE #52
	DC.B	$F8,$F8,$FF,$F8,$F8,$F8,$54,$F8,$00,$00 ;LOUNGE #53
	DC.B	$F8,$F8,$53,$F8,$F8,$F8,$F8,$F8,$00,$00 ;KITCHEN #54
	DC.B	$FF,$F8,$F8,$51,$F8,$F8,$F8,$F8,$00,$00 ;SE-W PASSAGE #55
	DC.B	$F8,$F8,$F8,$F8,$FF,$F8,$F8,$F8,$00,$00 ;LIFT #56
	DC.B	$56,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$00,$00 ;ROCKFACE #57
	DC.B	$59,$F8,$5A,$F8,$F8,$F8,$50,$F8,$F8,$F8 ;WHITE ROOM #58  
	DC.B	$F8,$F8,$F8,$F8,$58,$F8,$F8,$F8,$00,$00 ;OFFICE #59   
	DC.B	$F8,$F8,$F8,$F8,$F8,$F8,$58,$F8,$00,$00 ;COMPUTER ROOM #5A   
	DC.B	$58,$F8,$F8,$F8,$F8,$F8,$F8,$F8,$00,$00 ;DEATH CHAMBER #5B   
	DC.B	$F8,$F8,$F8,$F8,$F8,$F8,$16,$F8,$00,$00 ;PALACE #5C  
	DC.B	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00 ;LEDGE(DEATH CHAMBER) #5D
	DC.B	$00,$00,$00,$00,$00,$00,$00,$00,$00,$30 ;RIVERSIDE CHAMBER #5E:

NET_END

	END

