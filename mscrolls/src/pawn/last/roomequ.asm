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
*	@(#)roomequ.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*    SRC.ADV.ROOMEQUATES
*
*--------------------------------
 
RNONPATH           EQU   $1
RNONPATH2          EQU   $2
RNONPATH3          EQU   $3
RNVOID             EQU   $4
RNFOREST2          EQU   $5
RNFOREST3          EQU   $6
RNCLEARING         EQU   $7
RNFOREST4          EQU   $8
RNFOREST5          EQU   $9
RNFOREST6          EQU   $A
RNFOREST7          EQU   $B
RNONPATH4          EQU   $C
RNONPATH5          EQU   $D
RNPLAIN2           EQU   $E
RNWILDERNESS       EQU   $F
RNPLAIN3           EQU   $10
RNSCLIFF           EQU   $11
RNBRIDGE2          EQU   $12
RNNCLIFF           EQU   $13
RNONBRIDGE         EQU   $14
RNGARDENS          EQU   $15
RNGATEWAY          EQU   $16
RNMAZE             EQU   $17
RNHILLS            EQU   $18
RNHILLS2           EQU   $19
RNHILL             EQU   $1A
RNHILLS3           EQU   $1B
RNFOOTHILLS        EQU   $1C
RNINHUT            EQU   $1D
RNINSHED           EQU   $1E
RNTRACK            EQU   $1F
RNTRACK2           EQU   $20
RNTRACK3           EQU   $21
RNPLATEAU          EQU   $22
RNPLATEAU2         EQU   $23
RNPLATEAU3         EQU   $24
RNPLATEAU4         EQU   $25
RNINTOWER          EQU   $26
RNLANDING          EQU   $27
RNJAIL             EQU   $28
RNWALL             EQU   $29
RNSTORE            EQU   $2A
RNCAVEMOUTH        EQU   $2B
RNCAVERN           EQU   $2C
RNCAVERN2          EQU   $2D
RNCAVE             EQU   $2E
RNCORRIDOR         EQU   $2F
RNVSHAFT           EQU   $30
RNCAVERN4          EQU   $31
RNENTRANCE         EQU   $32
RNCORRIDOR2        EQU   $33
RNRIVER            EQU   $34
RNROOM             EQU   $35
RNPASSAGE          EQU   $36
RNLAB              EQU   $37
RNROOM2            EQU   $38
RNCAVERN5          EQU   $39
RNHELL             EQU   $3A
RNCAVERN6          EQU   $3B
RNCHAMBER          EQU   $3C
RNCIRCROOM         EQU   $3D
RNROOM4            EQU   $3E
RNSHAFT            EQU   $3F
RNLEDGE            EQU   $40
RNBRIDGE3          EQU   $41
RNBRIDGE4          EQU   $42
RNLEDGE2           EQU   $43
RNCHAMBER2         EQU   $44
RNWORKSHOP         EQU   $45
RNPLATFORM         EQU   $46
RNTUNNEL           EQU   $47
RNROOM5            EQU   $48
RNLEDGE3           EQU   $49
RNPASSAGE2         EQU   $4A
RNSHAFT2           EQU   $4B
RNLEDGE4           EQU   $4C
RNUPTREE           EQU   $4D
RNTREETRUNK        EQU   $4E
RNSTAIRCASE        EQU   $4F
RNLOWCEILING       EQU   $50
RNCROSSROADS       EQU   $51
RNPASSAGE3         EQU   $52 
RNLOUNGE           EQU   $53
RNKITCHEN          EQU   $54
RNPASSAGE4         EQU   $55 
RNLIFT             EQU   $56
RNROCKFACE         EQU   $57
RNWHITEROOM        EQU   $58
RNOFFICE           EQU   $59
RNBOOTH            EQU   $5A 
RNCHAMBER4         EQU   $5B ;DEATH CHAMBER (BACONBURGER)
RNPALACE           EQU   $5C
RNLEDGE5           EQU   $5D
RNCHAMBER3         EQU   $5E ;TORTURE CHAMBER

RNSTART        EQU  RNONPATH

*--------------------------------
               
NROOMS         EQU       $5E

