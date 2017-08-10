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


*******************************************************************
*
*	@(#)protlist.asm	2.7
*	5/16/87
*
*******************************************************************


	SECTION	"ADV",code


*******************************************************************
*
*     PROTLIST.ASM
*     AJS July 1986
*
*
* This is where the list of words for the protections
* is kept...
* There are two macros the first one given a sectino and word no 
* from search will insert a macro
* the second given the word name will insert the dc.w statement
*****************************************************************




   XDEF   PRNUM,PRSTART


SW      MACRO  * Section Word

\1      EQU    $\2<<10!$\3

        ENDM


PLW     MACRO   * Page No, Line No Word no (664)

        DC.W    \1<<10!\2<<4!\3

        ENDM


PRSTART:

   SW   PRARMS,6,105
   SW   PRATTACK,4,24
   SW   PRBED,6,8b
   SW   PRBLOW,4,52
   SW   PRBLUE,5,1B
   SW   PRBOX,6,135
   SW   PRBOXES,6,a7
   SW   PRBURN,4,56
   SW   PRBUSHES,6,4a
   SW   PRCAVE,6,61
   SW   PRCHAMBER,6,6f
   SW   PRCHEST,6,a9
   SW   PRCLOAK,6,e4
   SW   PRCLOSE,4,2f
   SW   PRCLOUD,6,8d
   SW   PRCOIN,6,ff
   SW   PRCROSS,4,50
   SW   PRCRY,4,3b
   SW   PRDAMAGE,4,28
   SW   PRDESK,6,132
   SW   PRDOORS,6,97
   SW   PRDRAGON,6,c1
   SW   PRDRESS,6,f8
   SW   PRDUST,6,10c
   SW   PREMPTY,4,1b
   SW   PRENTRANCE,6,65
   SW   PRERIK,6,f3
   SW   PREYES,6,b1
   SW   PRFEET,6,104
   SW   PRFIGHT,4,23		;was 4,123 hmmmm - Ken
   SW   PRFILL,4,1c
   SW   PRFIRE,4,33
   SW   PRFIT,4,19
   SW   PRFOG,6,4e
   SW   PRFOOTHILLS,6,1b
   SW   PRGARDENS,6,14
   SW   PRGET,4,2
   SW   PRGNOMES,6,14f
   SW   PRGO,4,7
   SW   PRGOLD,6,153
   SW   PRGOOD,6,14a
   SW   PRGREEN,5,2c
   SW   PRGRINGO,6,14e
   SW   PRGUARDS,6,27
   SW   PRHAND,6,103
   SW   PRHAT,6,c7
   SW   PRHEAD,6,107
   SW   PRHEADS,6,b6
   SW   PRHELL,6,6d
   SW   PRHIDE,4,43
   SW   PRHIGH,5,1e
   SW   PRHILLS,6,17
   SW   PRHIT,4,44
   SW   PRHORSE,6,3a
   SW   PRJOIN,4,4b
   SW   PRJUMP,4,1e
   SW   PRKEROVNIA,5,a2		;iffy - search sees KEROVNIAN
   SW   PRLABORATORY,6,6a
   SW   PRLEAVE,4,6
   SW   PRLIFE,6,14c
   SW   PRLIFT,4,d
   SW   PRLIGHT,4,2b
   SW   PRLOOK,4,0
   SW   PRMAGICIAN,6,e2
   SW   PRMAZE,6,16
   SW   PRMIX,4,1d
   SW   PROFFICE,6,11c
   SW   PRPALACE,5,b
   SW   PRPERSON,6,151
   SW   PRPOCKET,6,da
   SW   PRPOOL,6,8f
   SW   PRPOT,5,13
   SW   PRPOTION,5,4d
   SW   PRPOUR,4,1a
   SW   PRPRESS,4,53
   SW   PRPRINCESS,6,db
   SW   PRPUT,4,16
   SW   PRRABBIT,6,c8
   SW   PRREAD,4,42
   SW   PRRED,5,33
   SW   PRREMOVE,4,29
   SW   PRROOM,6,68
   SW   PRROSE,5,2b
   SW   PRRUN,4,a
   SW   PRSAY,4,28
   SW   PRSCRAPE,4,2a
   SW   PRSLOPE,6,88
   SW   PRSNOW,5,36
   SW   PRSNOWMAN,6,d9
   SW   PRSTAIRCASE,5,d5
   SW   PRSTEAL,4,45
   SW   PRSTONE,5,9
   SW   PRSYMBOLS,6,f7
   SW   PRTABLE,6,c6
   SW   PRTAKE,4,3
   SW   PRTHRONE,6,b0
   SW   PRTHROW,4,20
   SW   PRTOWER,6,86
   SW   PRTREE,5,3f
   SW   PRTREES,6,139		;was 6,239 !!!!! - Ken
   SW   PRTURN,4,37
   SW   PRUSE,4,4f
   SW   PRVOICE,6,148
   SW   PRWAND,5,bb
   SW   PRWATER,5,c3
   SW   PRWEAR,4,41
   SW   PRWHISKY,5,c1
   SW   PRWHITE,5,2
   SW   PRWILDERNESS,6,e
   SW   PRWINDOW,6,145
   SW   PRWINDOWS,6,99
   SW   PRWRITING,6,21


        DC.W    PRARMS
        PLW     11,23,6
        DC.W    PRARMS
        PLW     12,12,4
        DC.W    PRARMS
        PLW     14,16,8
        DC.W    PRARMS
        PLW     25,6,12
        DC.W    PRATTACK
        PLW     37,21,7
        DC.W    PRBED
        PLW     33,9,7
        DC.W    PRBED
        PLW     36,8,10
        DC.W    PRBED
        PLW     43,13,3
        DC.W    PRBLOW
        PLW     28,8,2
        DC.W    PRBLUE
        PLW     11,22,9
        DC.W    PRBLUE
        PLW     11,24,10
        DC.W    PRBLUE
        PLW     11,27,11
        DC.W    PRBLUE
        PLW     17,19,11
        DC.W    PRBLUE
        PLW     19,2,5
        DC.W    PRBLUE
        PLW     2,10,11
        DC.W    PRBLUE
        PLW     31,23,8
        DC.W    PRBLUE
        PLW     38,21,4
        DC.W    PRBLUE
        PLW     39,3,6
        DC.W    PRBLUE
        PLW     40,16,3
        DC.W    PRBLUE
        PLW     40,4,7
        DC.W    PRBLUE
        PLW     40,9,5
        DC.W    PRBLUE
        PLW     44,3,2
        DC.W    PRBLUE
        PLW     5,2,1
        DC.W    PRBOX
        PLW     7,15,14
        DC.W    PRBOXES
        PLW     37,13,11
        DC.W    PRBOXES
        PLW     37,15,11
        DC.W    PRBURN
        PLW     2,26,13
        DC.W    PRBUSHES
        PLW     28,16,9
        DC.W    PRCAVE
        PLW     16,14,11
        DC.W    PRCAVE
        PLW     19,14,8
        DC.W    PRCAVE
        PLW     19,4,6
        DC.W    PRCHAMBER
        PLW     34,12,1
        DC.W    PRCHEST
        PLW     13,2,6
        DC.W    PRCHEST
        PLW     16,6,2
        DC.W    PRCHEST
        PLW     33,11,10
        DC.W    PRCHEST
        PLW     33,9,2
        DC.W    PRCLOAK
        PLW     41,19,4
        DC.W    PRCLOSE
        PLW     1,9,2
        DC.W    PRCLOSE
        PLW     28,4,13
        DC.W    PRCLOUD
        PLW     42,17,10
        DC.W    PRCOIN
        PLW     1,2,6
        DC.W    PRCROSS
        PLW     2,21,1
        DC.W    PRCRY
        PLW     20,18,1
        DC.W    PRCRY
        PLW     41,5,3
        DC.W    PRDAMAGE
        PLW     7,12,9
        DC.W    PRDESK
        PLW     20,26,7
        DC.W    PRDOORS
        PLW     35,15,5
        DC.W    PRDRAGON
        PLW     16,11,4
        DC.W    PRDRAGON
        PLW     16,12,14
        DC.W    PRDRAGON
        PLW     16,16,2
        DC.W    PRDRAGON
        PLW     16,23,4
        DC.W    PRDRAGON
        PLW     17,29,2
        DC.W    PRDRAGON
        PLW     18,28,8
        DC.W    PRDRAGON
        PLW     19,19,7
        DC.W    PRDRAGON
        PLW     20,14,11
        DC.W    PRDRAGON
        PLW     21,1,1
        DC.W    PRDRAGON
        PLW     21,17,3
        DC.W    PRDRAGON
        PLW     22,4,12
        DC.W    PRDRAGON
        PLW     22,6,6
        DC.W    PRDRAGON
        PLW     23,13,12
        DC.W    PRDRESS
        PLW     12,7,14
        DC.W    PRDUST
        PLW     42,17,12
        DC.W    PRDUST
        PLW     7,5,7
        DC.W    PREMPTY
        PLW     23,9,2
        DC.W    PREMPTY
        PLW     27,23,9
        DC.W    PREMPTY
        PLW     30,10,9
        DC.W    PREMPTY
        PLW     5,13,6
        DC.W    PRENTRANCE
        PLW     19,4,3
        DC.W    PRERIK
        PLW     1,3,4
        DC.W    PRERIK
        PLW     11,1,1
        DC.W    PRERIK
        PLW     11,11,3
        DC.W    PRERIK
        PLW     11,29,1
        DC.W    PRERIK
        PLW     13,1,6
        DC.W    PRERIK
        PLW     2,28,9
        DC.W    PRERIK
        PLW     2,4,2
        DC.W    PRERIK
        PLW     24,4,13
        DC.W    PRERIK
        PLW     26,13,2
        DC.W    PRERIK
        PLW     26,17,2
        DC.W    PRERIK
        PLW     26,20,14
        DC.W    PRERIK
        PLW     28,20,12
        DC.W    PRERIK
        PLW     3,5,8
        DC.W    PRERIK
        PLW     30,20,7
        DC.W    PRERIK
        PLW     31,11,10
        DC.W    PRERIK
        PLW     32,24,6
        DC.W    PRERIK
        PLW     34,13,1
        DC.W    PRERIK
        PLW     36,8,6
        DC.W    PRERIK
        PLW     37,21,1
        DC.W    PRERIK
        PLW     38,5,1
        DC.W    PRERIK
        PLW     39,13,4
        DC.W    PRERIK
        PLW     39,19,6
        DC.W    PRERIK
        PLW     4,27,12
        DC.W    PRERIK
        PLW     4,7,1
        DC.W    PRERIK
        PLW     40,27,8
        DC.W    PRERIK
        PLW     41,16,1
        DC.W    PRERIK
        PLW     41,20,5
        DC.W    PRERIK
        PLW     41,7,6
        DC.W    PRERIK
        PLW     42,10,1
        DC.W    PRERIK
        PLW     42,14,1
        DC.W    PRERIK
        PLW     43,24,5
        DC.W    PRERIK
        PLW     43,8,1
        DC.W    PRERIK
        PLW     5,1,3
        DC.W    PRERIK
        PLW     5,11,11
        DC.W    PRERIK
        PLW     5,13,7
        DC.W    PRERIK
        PLW     5,6,1
        DC.W    PRERIK
        PLW     6,7,3
        DC.W    PRERIK
        PLW     7,9,12
        DC.W    PRERIK
        PLW     8,8,7
        DC.W    PREYES
        PLW     10,11,4
        DC.W    PREYES
        PLW     11,17,4
        DC.W    PREYES
        PLW     12,13,6
        DC.W    PREYES
        PLW     13,17,7
        DC.W    PREYES
        PLW     13,18,11
        DC.W    PREYES
        PLW     13,6,4
        DC.W    PREYES
        PLW     14,18,1
        DC.W    PREYES
        PLW     16,19,3
        DC.W    PREYES
        PLW     16,2,15
        DC.W    PREYES
        PLW     20,11,1
        DC.W    PREYES
        PLW     21,3,3
        DC.W    PREYES
        PLW     33,16,10
        DC.W    PREYES
        PLW     38,28,4
        DC.W    PREYES
        PLW     4,18,5
        DC.W    PREYES
        PLW     4,23,6
        DC.W    PREYES
        PLW     40,15,9
        DC.W    PREYES
        PLW     44,13,4
        DC.W    PRFEET
        PLW     19,17,8
        DC.W    PRFEET
        PLW     22,2,3
        DC.W    PRFEET
        PLW     27,4,6
        DC.W    PRFEET
        PLW     30,15,5
        DC.W    PRFEET
        PLW     34,20,5
        DC.W    PRFEET
        PLW     34,5,2
        DC.W    PRFEET
        PLW     40,12,5
        DC.W    PRFEET
        PLW     41,18,14
        DC.W    PRFEET
        PLW     44,10,8
        DC.W    PRFIGHT
        PLW     7,22,7
        DC.W    PRFIGHT
        PLW     9,14,2
        DC.W    PRFILL
        PLW     7,24,12
        DC.W    PRFIRE
        PLW     17,8,9
        DC.W    PRFIRE
        PLW     35,15,11
        DC.W    PRFIRE
        PLW     35,20,12
        DC.W    PRFIRE
        PLW     35,27,3
        DC.W    PRFIRE
        PLW     36,1,4
        DC.W    PRFIT
        PLW     25,7,9
        DC.W    PRFIT
        PLW     31,12,13
        DC.W    PRFOG
        PLW     17,11,4
        DC.W    PRFOG
        PLW     17,16,2
        DC.W    PRFOG
        PLW     22,8,2
        DC.W    PRFOOTHILLS
        PLW     17,22,10
        DC.W    PRFOOTHILLS
        PLW     44,7,12
        DC.W    PRGARDENS
        PLW     25,16,2
        DC.W    PRGARDENS
        PLW     40,4,1
        DC.W    PRGARDENS
        PLW     7,14,9
        DC.W    PRGARDENS
        PLW     8,2,6
        DC.W    PRGET
        PLW     21,10,4
        DC.W    PRGET
        PLW     22,19,2
        DC.W    PRGET
        PLW     23,1,2
        DC.W    PRGET
        PLW     23,10,2
        DC.W    PRGET
        PLW     28,4,11
        DC.W    PRGNOMES
        PLW     1,9,8
        DC.W    PRGNOMES
        PLW     2,5,12
        DC.W    PRGNOMES
        PLW     3,4,3
        DC.W    PRGO
        PLW     19,6,10
        DC.W    PRGO
        PLW     19,6,4
        DC.W    PRGO
        PLW     19,7,1
        DC.W    PRGO
        PLW     19,7,6
        DC.W    PRGO
        PLW     21,8,2
        DC.W    PRGO
        PLW     35,23,12
        DC.W    PRGO
        PLW     37,2,9
        DC.W    PRGO
        PLW     40,25,10
        DC.W    PRGO
        PLW     42,4,7
        DC.W    PRGO
        PLW     44,16,10
        DC.W    PRGOLD
        PLW     12,29,7
        DC.W    PRGOLD
        PLW     12,7,12
        DC.W    PRGOLD
        PLW     17,15,10
        DC.W    PRGOLD
        PLW     19,2,7
        DC.W    PRGOLD
        PLW     20,9,7
        DC.W    PRGOLD
        PLW     21,14,1
        DC.W    PRGOLD
        PLW     37,23,12
        DC.W    PRGOLD
        PLW     38,15,9
        DC.W    PRGOLD
        PLW     38,17,4
        DC.W    PRGOOD
        PLW     13,2,11
        DC.W    PRGREEN
        PLW     13,11,3
        DC.W    PRGREEN
        PLW     16,16,9
        DC.W    PRGREEN
        PLW     20,20,1
        DC.W    PRGREEN
        PLW     29,12,5
        DC.W    PRGREEN
        PLW     38,26,13
        DC.W    PRGREEN
        PLW     39,18,13
        DC.W    PRGREEN
        PLW     5,3,2
        DC.W    PRGREEN
        PLW     6,14,8
        DC.W    PRGRINGO
        PLW     17,28,6
        DC.W    PRGRINGO
        PLW     18,11,13
        DC.W    PRGRINGO
        PLW     18,2,10
        DC.W    PRGRINGO
        PLW     18,26,8
        DC.W    PRGRINGO
        PLW     18,8,9
        DC.W    PRGRINGO
        PLW     19,22,5
        DC.W    PRGRINGO
        PLW     2,7,10
        DC.W    PRGRINGO
        PLW     20,3,11
        DC.W    PRGRINGO
        PLW     21,11,7
        DC.W    PRGRINGO
        PLW     21,15,3
        DC.W    PRGRINGO
        PLW     21,18,5
        DC.W    PRGRINGO
        PLW     21,2,1
        DC.W    PRGRINGO
        PLW     21,27,12
        DC.W    PRGRINGO
        PLW     22,12,6
        DC.W    PRGRINGO
        PLW     22,20,7
        DC.W    PRGRINGO
        PLW     22,24,1
        DC.W    PRGRINGO
        PLW     22,6,1
        DC.W    PRGRINGO
        PLW     23,8,3
        DC.W    PRGRINGO
        PLW     24,18,4
        DC.W    PRGRINGO
        PLW     26,20,15
        DC.W    PRGRINGO
        PLW     3,6,2
        DC.W    PRGRINGO
        PLW     30,2,2
        DC.W    PRGRINGO
        PLW     30,21,7
        DC.W    PRGRINGO
        PLW     30,4,4
        DC.W    PRGRINGO
        PLW     31,19,10
        DC.W    PRGRINGO
        PLW     31,28,7
        DC.W    PRGRINGO
        PLW     31,6,8
        DC.W    PRGRINGO
        PLW     31,8,3
        DC.W    PRGRINGO
        PLW     32,12,7
        DC.W    PRGRINGO
        PLW     32,2,12
        DC.W    PRGRINGO
        PLW     34,14,9
        DC.W    PRGRINGO
        PLW     34,21,4
        DC.W    PRGRINGO
        PLW     34,23,3
        DC.W    PRGRINGO
        PLW     34,26,2
        DC.W    PRGRINGO
        PLW     34,4,11
        DC.W    PRGRINGO
        PLW     35,10,4
        DC.W    PRGRINGO
        PLW     35,22,1
        DC.W    PRGRINGO
        PLW     35,24,1
        DC.W    PRGRINGO
        PLW     36,1,10
        DC.W    PRGRINGO
        PLW     36,9,3
        DC.W    PRGRINGO
        PLW     37,2,1
        DC.W    PRGRINGO
        PLW     4,2,10
        DC.W    PRGRINGO
        PLW     4,9,8
        DC.W    PRGRINGO
        PLW     42,6,3
        DC.W    PRGRINGO
        PLW     8,11,11
        DC.W    PRGRINGO
        PLW     8,12,2
        DC.W    PRGRINGO
        PLW     8,14,12
        DC.W    PRGRINGO
        PLW     8,26,1
        DC.W    PRGRINGO
        PLW     9,4,7
        DC.W    PRGRINGO
        PLW     9,7,8
        DC.W    PRGUARDS
        PLW     11,30,2
        DC.W    PRGUARDS
        PLW     39,1,2
        DC.W    PRHAND
        PLW     13,20,8
        DC.W    PRHAND
        PLW     17,12,4
        DC.W    PRHAND
        PLW     20,23,13
        DC.W    PRHAND
        PLW     21,6,1
        DC.W    PRHAND
        PLW     26,12,1
        DC.W    PRHAND
        PLW     27,8,9
        DC.W    PRHAND
        PLW     30,19,5
        DC.W    PRHAND
        PLW     31,15,5
        DC.W    PRHAND
        PLW     34,5,5
        DC.W    PRHAND
        PLW     41,17,5
        DC.W    PRHAT
        PLW     19,4,12
        DC.W    PRHAT
        PLW     36,3,4
        DC.W    PRHAT
        PLW     6,14,10
        DC.W    PRHEAD
        PLW     16,5,6
        DC.W    PRHEAD
        PLW     33,29,14
        DC.W    PRHEAD
        PLW     38,17,13
        DC.W    PRHEAD
        PLW     38,29,9
        DC.W    PRHEAD
        PLW     38,8,10
        DC.W    PRHEAD
        PLW     39,10,2
        DC.W    PRHEAD
        PLW     43,10,7
        DC.W    PRHEAD
        PLW     5,6,4
        DC.W    PRHEAD
        PLW     7,17,6
        DC.W    PRHEAD
        PLW     9,5,2
        DC.W    PRHEADS
        PLW     7,11,8
        DC.W    PRHELL
        PLW     23,10,14
        DC.W    PRHIDE
        PLW     12,16,5
        DC.W    PRHIGH
        PLW     10,5,10
        DC.W    PRHIGH
        PLW     18,21,2
        DC.W    PRHIGH
        PLW     27,25,1
        DC.W    PRHIGH
        PLW     28,26,1
        DC.W    PRHIGH
        PLW     3,23,8
        DC.W    PRHIGH
        PLW     31,8,7
        DC.W    PRHIGH
        PLW     38,27,5
        DC.W    PRHILLS
        PLW     18,15,11
        DC.W    PRHILLS
        PLW     19,27,6
        DC.W    PRHIT
        PLW     28,6,12
        DC.W    PRHIT
        PLW     4,1,6
        DC.W    PRHORSE
        PLW     37,22,13
        DC.W    PRHORSE
        PLW     7,16,9
        DC.W    PRJOIN
        PLW     24,10,3
        DC.W    PRJOIN
        PLW     38,12,9
        DC.W    PRJOIN
        PLW     38,9,8
        DC.W    PRJUMP
        PLW     16,22,1
        DC.W    PRKEROVNIA
        PLW     1,2,15
        DC.W    PRKEROVNIA
        PLW     1,4,10
        DC.W    PRKEROVNIA
        PLW     1,8,10
        DC.W    PRKEROVNIA
        PLW     12,20,3
        DC.W    PRKEROVNIA
        PLW     12,27,11
        DC.W    PRKEROVNIA
        PLW     13,24,8
        DC.W    PRKEROVNIA
        PLW     14,13,1
        DC.W    PRKEROVNIA
        PLW     15,7,1
        DC.W    PRKEROVNIA
        PLW     16,23,11
        DC.W    PRKEROVNIA
        PLW     17,11,9
        DC.W    PRKEROVNIA
        PLW     18,10,1
        DC.W    PRKEROVNIA
        PLW     18,5,10
        DC.W    PRKEROVNIA
        PLW     2,1,9
        DC.W    PRKEROVNIA
        PLW     2,16,6
        DC.W    PRKEROVNIA
        PLW     2,27,11
        DC.W    PRKEROVNIA
        PLW     23,2,6
        DC.W    PRKEROVNIA
        PLW     24,2,8
        DC.W    PRKEROVNIA
        PLW     26,6,8
        DC.W    PRKEROVNIA
        PLW     3,9,6
        DC.W    PRKEROVNIA
        PLW     30,14,13
        DC.W    PRKEROVNIA
        PLW     32,19,1
        DC.W    PRKEROVNIA
        PLW     32,24,12
        DC.W    PRKEROVNIA
        PLW     32,29,12
        DC.W    PRKEROVNIA
        PLW     32,4,11
        DC.W    PRKEROVNIA
        PLW     33,26,3
        DC.W    PRKEROVNIA
        PLW     34,6,11
        DC.W    PRKEROVNIA
        PLW     35,2,2
        DC.W    PRKEROVNIA
        PLW     35,8,1
        DC.W    PRKEROVNIA
        PLW     37,15,9
        DC.W    PRKEROVNIA
        PLW     39,19,10
        DC.W    PRKEROVNIA
        PLW     39,8,4
        DC.W    PRKEROVNIA
        PLW     4,16,3
        DC.W    PRKEROVNIA
        PLW     4,5,4
        DC.W    PRKEROVNIA
        PLW     41,2,5
        DC.W    PRKEROVNIA
        PLW     41,9,3
        DC.W    PRKEROVNIA
        PLW     43,13,8
        DC.W    PRKEROVNIA
        PLW     5,23,6
        DC.W    PRKEROVNIA
        PLW     5,8,16
        DC.W    PRKEROVNIA
        PLW     8,12,9
        DC.W    PRKEROVNIA
        PLW     8,8,5
        DC.W    PRLABORATORY
        PLW     6,5,4
        DC.W    PRLEAVE
        PLW     16,2,9
        DC.W    PRLEAVE
        PLW     24,9,9
        DC.W    PRLEAVE
        PLW     32,11,9
        DC.W    PRLEAVE
        PLW     35,1,9
        DC.W    PRLEAVE
        PLW     38,10,9
        DC.W    PRLEAVE
        PLW     40,17,3
        DC.W    PRLEAVE
        PLW     40,23,3
        DC.W    PRLEAVE
        PLW     41,16,7
        DC.W    PRLEAVE
        PLW     42,3,11
        DC.W    PRLIFE
        PLW     26,26,8
        DC.W    PRLIFE
        PLW     30,13,10
        DC.W    PRLIFE
        PLW     40,14,13
        DC.W    PRLIFE
        PLW     43,20,6
        DC.W    PRLIFE
        PLW     43,9,7
        DC.W    PRLIFE
        PLW     7,12,13
        DC.W    PRLIFT
        PLW     25,15,1
        DC.W    PRLIGHT
        PLW     3,2,10
        DC.W    PRLIGHT
        PLW     35,22,11
        DC.W    PRLIGHT
        PLW     40,4,9
        DC.W    PRLOOK
        PLW     16,20,4
        DC.W    PRLOOK
        PLW     33,19,4
        DC.W    PRLOOK
        PLW     43,15,9
        DC.W    PRLOOK
        PLW     44,12,3
        DC.W    PRMAGICIAN
        PLW     16,10,5
        DC.W    PRMAGICIAN
        PLW     21,7,11
        DC.W    PRMAGICIAN
        PLW     22,17,7
        DC.W    PRMAGICIAN
        PLW     22,22,3
        DC.W    PRMAGICIAN
        PLW     34,16,7
        DC.W    PRMAGICIAN
        PLW     35,5,8
        DC.W    PRMAGICIAN
        PLW     6,11,6
        DC.W    PRMAGICIAN
        PLW     6,9,5
        DC.W    PRMAZE
        PLW     2,8,7
        DC.W    PRMIX
        PLW     22,4,8
        DC.W    PROFFICE
        PLW     30,21,10
        DC.W    PROFFICE
        PLW     33,2,10
        DC.W    PRPALACE
        PLW     10,20,4
        DC.W    PRPALACE
        PLW     10,5,6
        DC.W    PRPALACE
        PLW     15,9,10
        DC.W    PRPALACE
        PLW     17,22,2
        DC.W    PRPALACE
        PLW     18,16,4
        DC.W    PRPALACE
        PLW     22,9,1
        DC.W    PRPALACE
        PLW     23,11,5
        DC.W    PRPALACE
        PLW     24,6,8
        DC.W    PRPALACE
        PLW     25,12,8
        DC.W    PRPALACE
        PLW     25,13,4
        DC.W    PRPALACE
        PLW     25,2,10
        DC.W    PRPALACE
        PLW     25,25,10
        DC.W    PRPALACE
        PLW     26,9,11
        DC.W    PRPALACE
        PLW     29,2,7
        DC.W    PRPALACE
        PLW     29,5,4
        DC.W    PRPALACE
        PLW     32,11,6
        DC.W    PRPALACE
        PLW     35,1,11
        DC.W    PRPALACE
        PLW     38,10,11
        DC.W    PRPALACE
        PLW     38,12,3
        DC.W    PRPALACE
        PLW     39,25,7
        DC.W    PRPALACE
        PLW     39,27,12
        DC.W    PRPALACE
        PLW     4,21,12
        DC.W    PRPALACE
        PLW     40,3,5
        DC.W    PRPALACE
        PLW     42,13,9
        DC.W    PRPALACE
        PLW     6,8,3
        DC.W    PRPALACE
        PLW     7,10,10
        DC.W    PRPALACE
        PLW     7,13,12
        DC.W    PRPALACE
        PLW     7,13,3
        DC.W    PRPALACE
        PLW     9,15,9
        DC.W    PRPERSON
        PLW     18,8,10
        DC.W    PRPERSON
        PLW     25,9,7
        DC.W    PRPERSON
        PLW     30,9,4
        DC.W    PRPERSON
        PLW     31,28,2
        DC.W    PRPERSON
        PLW     39,20,6
        DC.W    PRPERSON
        PLW     39,21,9
        DC.W    PRPOCKET
        PLW     20,23,5
        DC.W    PRPOCKET
        PLW     22,25,1
        DC.W    PRPOOL
        PLW     26,23,11
        DC.W    PRPOOL
        PLW     27,14,2
        DC.W    PRPOOL
        PLW     27,23,4
        DC.W    PRPOOL
        PLW     27,6,6
        DC.W    PRPOOL
        PLW     28,13,9
        DC.W    PRPOOL
        PLW     40,1,1
        DC.W    PRPOT
        PLW     26,20,13
        DC.W    PRPOTION
        PLW     17,10,8
        DC.W    PRPOTION
        PLW     17,18,12
        DC.W    PRPOTION
        PLW     17,23,1
        DC.W    PRPOTION
        PLW     17,5,10
        DC.W    PRPOTION
        PLW     17,7,14
        DC.W    PRPOTION
        PLW     18,18,9
        DC.W    PRPOTION
        PLW     18,21,10
        DC.W    PRPOTION
        PLW     22,4,6
        DC.W    PRPOTION
        PLW     30,22,4
        DC.W    PRPOTION
        PLW     31,4,6
        DC.W    PRPOUR
        PLW     21,24,12
        DC.W    PRPRESS
        PLW     28,26,10
        DC.W    PRPRINCESS
        PLW     10,2,12
        DC.W    PRPRINCESS
        PLW     11,12,2
        DC.W    PRPRINCESS
        PLW     13,6,12
        DC.W    PRPRINCESS
        PLW     27,15,9
        DC.W    PRPRINCESS
        PLW     29,4,7
        DC.W    PRPRINCESS
        PLW     33,14,1
        DC.W    PRPRINCESS
        PLW     33,23,8
        DC.W    PRPRINCESS
        PLW     35,7,6
        DC.W    PRPRINCESS
        PLW     37,9,9
        DC.W    PRPRINCESS
        PLW     39,26,3
        DC.W    PRPRINCESS
        PLW     4,26,10
        DC.W    PRPRINCESS
        PLW     4,4,1
        DC.W    PRPRINCESS
        PLW     40,14,1
        DC.W    PRPRINCESS
        PLW     42,13,4
        DC.W    PRPRINCESS
        PLW     42,16,2
        DC.W    PRPRINCESS
        PLW     43,12,4
        DC.W    PRPRINCESS
        PLW     43,23,8
        DC.W    PRPRINCESS
        PLW     43,4,2
        DC.W    PRPUT
        PLW     21,9,1
        DC.W    PRPUT
        PLW     27,8,7
        DC.W    PRPUT
        PLW     33,8,9
        DC.W    PRPUT
        PLW     36,8,8
        DC.W    PRRABBIT
        PLW     2,21,4
        DC.W    PRRABBIT
        PLW     2,23,11
        DC.W    PRREAD
        PLW     28,1,3
        DC.W    PRREAD
        PLW     31,7,7
        DC.W    PRREAD
        PLW     9,15,4
        DC.W    PRRED
        PLW     14,2,10
        DC.W    PRRED
        PLW     16,17,3
        DC.W    PRRED
        PLW     2,1,11
        DC.W    PRRED
        PLW     22,13,9
        DC.W    PRRED
        PLW     35,27,2
        DC.W    PRRED
        PLW     38,15,7
        DC.W    PRRED
        PLW     38,17,6
        DC.W    PRRED
        PLW     40,15,8
        DC.W    PRREMOVE
        PLW     33,2,5
        DC.W    PRROOM
        PLW     10,4,5
        DC.W    PRROOM
        PLW     11,18,1
        DC.W    PRROOM
        PLW     11,21,12
        DC.W    PRROOM
        PLW     13,7,5
        DC.W    PRROOM
        PLW     17,24,9
        DC.W    PRROOM
        PLW     21,15,7
        DC.W    PRROOM
        PLW     21,16,8
        DC.W    PRROOM
        PLW     21,27,11
        DC.W    PRROOM
        PLW     23,6,10
        DC.W    PRROOM
        PLW     28,13,12
        DC.W    PRROOM
        PLW     31,4,14
        DC.W    PRROOM
        PLW     35,19,12
        DC.W    PRROOM
        PLW     41,16,9
        DC.W    PRROOM
        PLW     41,19,12
        DC.W    PRROOM
        PLW     5,13,4
        DC.W    PRROOM
        PLW     5,2,4
        DC.W    PRROSE
        PLW     15,2,6
        DC.W    PRROSE
        PLW     21,2,2
        DC.W    PRROSE
        PLW     27,19,4
        DC.W    PRROSE
        PLW     38,24,6
        DC.W    PRROSE
        PLW     4,22,5
        DC.W    PRROSE
        PLW     40,12,2
        DC.W    PRROSE
        PLW     40,4,11
        DC.W    PRROSE
        PLW     41,18,11
        DC.W    PRRUN
        PLW     18,13,4
        DC.W    PRRUN
        PLW     2,10,5
        DC.W    PRRUN
        PLW     4,2,8
        DC.W    PRRUN
        PLW     42,19,2
        DC.W    PRSAY
        PLW     22,17,4
        DC.W    PRSAY
        PLW     27,26,4
        DC.W    PRSCRAPE
        PLW     42,5,1
        DC.W    PRSLOPE
        PLW     19,21,2
        DC.W    PRSNOW
        PLW     44,3,4
        DC.W    PRSNOWMAN
        PLW     44,10,5
        DC.W    PRSNOWMAN
        PLW     44,20,4
        DC.W    PRSTAIRCASE
        PLW     19,18,10
        DC.W    PRSTEAL
        PLW     20,21,4
        DC.W    PRSTONE
        PLW     10,6,6
        DC.W    PRSTONE
        PLW     19,1,12
        DC.W    PRSTONE
        PLW     20,4,2
        DC.W    PRSTONE
        PLW     39,3,9
        DC.W    PRSTONE
        PLW     42,4,4
        DC.W    PRSYMBOLS
        PLW     12,3,8
        DC.W    PRTABLE
        PLW     11,28,13
        DC.W    PRTABLE
        PLW     12,4,12
        DC.W    PRTABLE
        PLW     17,24,3
        DC.W    PRTAKE
        PLW     12,13,10
        DC.W    PRTAKE
        PLW     13,15,6
        DC.W    PRTAKE
        PLW     16,2,7
        DC.W    PRTAKE
        PLW     17,10,3
        DC.W    PRTAKE
        PLW     17,7,8
        DC.W    PRTAKE
        PLW     18,5,8
        DC.W    PRTAKE
        PLW     26,14,11
        DC.W    PRTAKE
        PLW     3,13,7
        DC.W    PRTAKE
        PLW     32,14,6
        DC.W    PRTAKE
        PLW     33,26,9
        DC.W    PRTAKE
        PLW     34,21,3
        DC.W    PRTAKE
        PLW     39,14,4
        DC.W    PRTAKE
        PLW     39,18,2
        DC.W    PRTAKE
        PLW     4,8,10
        DC.W    PRTAKE
        PLW     43,9,11
        DC.W    PRTHRONE
        PLW     12,10,11
        DC.W    PRTHRONE
        PLW     3,27,9
        DC.W    PRTHRONE
        PLW     34,25,12
        DC.W    PRTHROW
        PLW     8,17,5
        DC.W    PRTOWER
        PLW     25,4,8
        DC.W    PRTREE
        PLW     43,20,7
        DC.W    PRTREES
        PLW     24,22,6
        DC.W    PRTREES
        PLW     30,4,7
        DC.W    PRTREES
        PLW     43,19,9
        DC.W    PRTREES
        PLW     43,21,4
        DC.W    PRTURN
        PLW     37,18,10
        DC.W    PRTURN
        PLW     7,8,1
        DC.W    PRUSE
        PLW     17,13,9
        DC.W    PRUSE
        PLW     19,19,1
        DC.W    PRUSE
        PLW     20,24,8
        DC.W    PRUSE
        PLW     24,12,3
        DC.W    PRUSE
        PLW     28,27,2
        DC.W    PRUSE
        PLW     33,17,13
        DC.W    PRUSE
        PLW     6,7,8
        DC.W    PRVOICE
        PLW     13,23,10
        DC.W    PRVOICE
        PLW     14,9,7
        DC.W    PRVOICE
        PLW     34,15,12
        DC.W    PRVOICE
        PLW     40,9,8
        DC.W    PRWAND
        PLW     20,22,10
        DC.W    PRWAND
        PLW     21,5,9
        DC.W    PRWAND
        PLW     22,24,11
        DC.W    PRWATER
        PLW     10,12,6
        DC.W    PRWATER
        PLW     26,23,7
        DC.W    PRWATER
        PLW     27,12,9
        DC.W    PRWATER
        PLW     27,24,3
        DC.W    PRWATER
        PLW     27,5,10
        DC.W    PRWATER
        PLW     28,10,7
        DC.W    PRWATER
        PLW     28,3,7
        DC.W    PRWATER
        PLW     28,8,11
        DC.W    PRWATER
        PLW     30,22,11
        DC.W    PRWATER
        PLW     31,10,5
        DC.W    PRWATER
        PLW     40,13,2
        DC.W    PRWATER
        PLW     40,9,12
        DC.W    PRWEAR
        PLW     38,6,4
        DC.W    PRWHISKY
        PLW     21,20,3
        DC.W    PRWHISKY
        PLW     31,9,4
        DC.W    PRWHITE
        PLW     21,24,5
        DC.W    PRWHITE
        PLW     25,1,12
        DC.W    PRWHITE
        PLW     25,18,3
        DC.W    PRWHITE
        PLW     25,8,1
        DC.W    PRWHITE
        PLW     25,8,3
        DC.W    PRWHITE
        PLW     5,18,6
        DC.W    PRWILDERNESS
        PLW     2,17,11
        DC.W    PRWILDERNESS
        PLW     2,25,6
        DC.W    PRWINDOW
        PLW     10,8,10
        DC.W    PRWINDOW
        PLW     21,1,8
        DC.W    PRWINDOWS
        PLW     10,7,1
        DC.W    PRWINDOWS
        PLW     11,18,11
        DC.W    PRWINDOWS
        PLW     11,20,4
        DC.W    PRWINDOWS
        PLW     20,25,3
        DC.W    PRWRITING
        PLW     33,13,1























































































PREND:
PRNUM   EQU   ((PREND-PRSTART)/4)-1        ;the number of checks.
   END

