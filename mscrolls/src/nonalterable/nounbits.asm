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
*	@(#)nounbits.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*****************************************
*
*   NOUNBITS    Here live the equates
*               for what each bit/byte/field
*               in the noundata is for
*
*   generally put the size at the end of the
*   lable name eg.
*
*   LIT_BIT   is a bit, meaning obj is lit
*   SIZE_NIB  is a nibble (0-15) mask ie $0F, or $F0
*   LOC_WORD  is a word
*
****************************************




STATUS_BYTE    EQU      0

LIT_BIT        EQU      7
LOCKED_BIT     EQU      6
CLOSED_BIT     EQU      5
OPEN_BIT       EQU      4
GWO_BIT        EQU      3
BROKEN_BIT     EQU      2
DEAD_BIT       EQU      1
PLURAL_BIT     EQU      0


PHYSCHAR_BYTE  EQU      1

HARD_NIB       EQU      $F0
STRENTH_NIB    EQU      $0F


SIZEWGHT_BYTE  EQU      2

SIZE_NIB       EQU      $F0
WGHT_NIB       EQU      $0F


SHAPETEX_BYTE  EQU      3

SHAPE_NIB      EQU      $F0
TEXT_NIB       EQU      $0F


EXTRA_BYTE     EQU      4

LIGHTABLE_BIT  EQU      7
NPC_BIT        EQU      6
WARY_BIT       EQU      5
EXPLORED_BIT   EQU      4
MONEY_BIT      EQU      3

VALUE_FIELD    EQU      $07


SPARE_BYTE     EQU      5     ;in the good ol' days this had 6 spare bits!!

NOADJ_BIT      EQU      7     ;Don't use SEARCH to see if any noun with same
                              ;'root' word local - saves time in P.NOUN

GBACK_BIT      EQU      6     ;NPC will give it back if given to NPC

FORCEP_BIT     EQU      5     ;Deals with 'semi-' plurals, eg - some armour

ADJON_BIT      EQU      4     ;Force adjective always printed wi this noun

SIT_BIT        EQU      3     ;You can sit on this object

LIGHT_BIT      EQU      2     ;Light room/more lit/unlit/lightable info

INIT_BIT       EQU      1     ;Set once initial state description has happened

PSEUDO_BIT     EQU      0     ;This is a pseudo item!




LOCINFO_BYTE   EQU      6

OUT_BIT        EQU      7     ;Set => not in the game just now

ROOM_BIT       EQU      6     ;This noun is a room

CONT_BIT       EQU      5     ;This noun is contained

INSEP_BIT      EQU      4     ;Relationship is INSEPERABLE

INTRIN_BIT     EQU      3     ;Item is intrinsic (arm/leg/head etc)

WORN_BIT       EQU      2     ;Item is worn by the player

HIDDEN_BIT     EQU      1     ;Item found if its host is seached,tasted, etc

RELATED_BIT    EQU      0     ;Item is related to another (noun# in LOC_WORD)



SECLOC_BYTE    EQU      7     ;The secondary locator. The room# of the room
                              ;containing the particular occurance of the
                              ;pseudo to which the noun is attached/related

LOC_WORD       EQU      8     ;Noun#, room#, pseudo pointer


DATA_BYTE      EQU      10

FEETLEG_BIT    EQU      7     ;This clothing covers feet & legs
GROIN_BIT      EQU      6     ;       ..            groin
ARMS_BIT       EQU      5     ;       ..            arms
HEAD_BIT       EQU      4     ;       ..            head  (ie a hat)
CHEST_BIT      EQU      3     ;       ..            chest

CVOLM_FIELD    EQU      $07   ;Current volume free(+1) of a container
                              ;1 => full  (7 max) 0 => not a container


DATA2_BYTE     EQU      11

EDIBLE_FIELD   EQU      $E0   ;edibility 0-7

COMB_FIELD     EQU      $18   ;Combustibility 0-3

WEAR_FIELD     EQU      $06   ;Wearable effectiveness 0-3 (quality as armour)

NPCWORN_BIT    EQU      0     ;Set => worn by an NPC


FIX_WORD       EQU      12    ;Bottom 14 bits are noun# of obj this is tied to

NUNFIX_BIT     EQU      7     ;This object is not unfixable  (ie may not be
                              ;unfixed




