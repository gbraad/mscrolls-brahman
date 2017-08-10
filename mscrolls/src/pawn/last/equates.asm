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
*       @(#)equates.asm 2.1
*       9/19/86
*
******************************************************

                SECTION         "ADV",CODE

*--------------------------------
*
* SRC.ADV.EQUATES
*
*--------------------------------


*******************************
*
*
* THIS WAS IN THE ASM FILE ON THE APPLE...
*
*

TRUE     EQU     1
FALSE    EQU     0

ABSOLUTELY_THE_LAST_LINK EQU FALSE      ;+/- 1 (or 2)
MAGIC_NORM     EQU   $4FE0C2B5

MAGIC_FRIG1    EQU   $4FE0C2B5
MAGIC_FRIG2    EQU   $4FE0C2B6
MAGIC_FRIG3    EQU   $4FE0C2B5
MAGIC_FRIG4    EQU   04
MAGIC_FRIG5    EQU   05
MAGIC_FRIG6    EQU   06
MAGIC_FRIG7    EQU   07
MAGIC_FRIG8    EQU   08
MAGIC_FRIG9    EQU   09


THE_PAWN EQU      TRUE


YCHEAT   EQU      TRUE            ;Debug bits on?
PROD     EQU      TRUE

PROD_TEST EQU     FALSE           ;nearly production!

QL128    EQU      FALSE
ST520    EQU      FALSE
AMIGA512 EQU      FALSE
AMIGA256 EQU      FALSE           ;hohohoho
C64      EQU      FALSE
C128     EQU      FALSE
APPLE2   EQU      FALSE
BBC_SWR  EQU      FALSE           ;SWR=sideways ram
CPC6128  EQU      FALSE       
APPLEII  EQU      FALSE
MAC512   EQU      FALSE
IBM      EQU      FALSE           ;Hmmmmmmmmmmmmm
VAX      EQU      TRUE
ATARI    EQU      FALSE
PCW8256  EQU      FALSE
SPECTRUM EQU      FALSE
GENERIC  EQU      FALSE
IIGS     EQU      FALSE

TimeCode EQU      FALSE
NAMEDPICS       EQU     FALSE
*
*
*
********************************
*
*
* And now, the target machine profiles...
*

      IFNE     ST520
 
SpoolLen       EQU    6200          ;printer buffer = wiz pic pallet
RECLIM         EQU    12            ;max relphrase depth
WIDTH.DEF      EQU    79            ;chars per line (as used in left just)
WIDTH.OPT      EQU    52            ;narrow mode for STM (television)
BUFFLEN        EQU    $100          ;size of input buffer
FunctionKeys   EQU    10            ;yes, we have 10 of them
M68000         EQU    TRUE          ;if this puzzles you please
                                    ;jump out of a window 
DosPresent     EQU    TRUE          ;use real IO with filenames and stuff
Graphics       EQU    TRUE          ;do we get the pictures?
RestartRAM     EQU    TRUE          ;is the restart from a RAM image of game
DummyText      EQU    FALSE
MemoryScarce   EQU    FALSE         ;Last resort, lose stuff like COLOR=COLOUR
TimeInfo       EQU    FALSE         ;so as we can see how long various stuff is
ESC_edit       EQU    TRUE          ;re-edit last line with ESC
Wimp           EQU    TRUE          ;Apple Corp. allowing
Mono_Possible  EQU    TRUE          ;Can we possibly run this in Monochrome?
SpeechAvail    EQU    FALSE


      ENDC



      IFNE     QL128
 
SpoolLen       EQU    FALSE         ;no printer buffer
RECLIM         EQU    6             ;max relphrase depth
WIDTH.DEF      EQU    52            ;chars per line (as used in left just)
BUFFLEN        EQU    $100          ;size of input buffer
FunctionKeys   EQU    FALSE         ;no function keys
M68000         EQU    TRUE          ;ok, so it's a slight exaggeration
DosPresent     EQU    FALSE         ;saved games use numbers
Graphics       EQU    FALSE
RestartRAM     EQU    FALSE
DummyText      EQU    FALSE
MemoryScarce   EQU    FALSE         ;Last resort, lose stuff like COLOR=COLOUR
TimeInfo       EQU    FALSE         ;so as we can see how long various stuff is
ESC_edit       EQU    FALSE         ;re-edit last line with ESC
Wimp           EQU    FALSE
Mono_Possible  EQU    FALSE         ;Can we possibly run this in Monochrome?

      ENDC
 
  
      IFNE     BBC_SWR!VAX
 
BookProtection EQU   FALSE 
SpoolLen       EQU    FALSE         ;no printer buffer
RECLIM         EQU    6             ;max relphrase depth
WIDTH.DEF      EQU    39            ;chars per line (as used in left just)
BUFFLEN        EQU    $80           ;size of input buffer
FunctionKeys   EQU    FALSE         ;no function keys
M68000         EQU    FALSE
DosPresent     EQU    TRUE
Graphics       EQU    TRUE
RestartRAM     EQU    FALSE
DummyText      EQU    FALSE         ;!!!! atomic bomb !! drugs !!!!
ExternText     EQU    TRUE          ; Text is external to this one!
MemoryScarce   EQU    FALSE         ;Last resort, lose stuff like COLOR=COLOUR
TimeInfo       EQU    FALSE         ;so as we can see how long various stuff is
ESC_edit       EQU    FALSE         ;re-edit last line with ESC
Wimp           EQU    FALSE
Mono_Possible  EQU    FALSE         ;Can we possibly run this in Monochrome?
FastBits       EQU    TRUE          ; Yes, 'cos we emulate search, etc.
SpeechAvail    EQU    FALSE          ;THE most useless feature of all time...
INOUTLINK      EQU     FALSE

        ENDC

*--------------------------------
*
* direction numbers
* 
 
DIR_N   EQU     0
DIR_NE  EQU     1
DIR_E   EQU     2
DIR_SE  EQU     3
DIR_S   EQU     4
DIR_SW  EQU     5
DIR_W   EQU     6
DIR_NW  EQU     7
DIR_U   EQU     8
DIR_D   EQU     9                       

        IFEQ    THE_PAWN

DIR_IN          EQU     10              ;in/out not reported by P.EXITS
DIR_OUT         EQU     11

Sizeof_NET      EQU     12              ;bytes per room in network

        ENDC

        IFNE    THE_PAWN

Sizeof_NET      EQU     10

        ENDC

*--------------------------------
*
* misc 'conj' section word numbers
*
        
WITH            EQU     0
TO              EQU     1
FROM            EQU     2
ON              EQU     3
INTO            EQU     4
IN              EQU     5
FOR             EQU     6
BY              EQU     7
OVER            EQU     8
UNDER           EQU     9
ACROSS          EQU     10
AT              EQU     11
AROUND          EQU     12
WITHIN          EQU     13
THROUGH         EQU     14
OFF             EQU     15
ONTO            EQU     16
AGAINST         EQU     17
ABOUT           EQU     18
TOGETHER        EQU     19
UNTIL           EQU     20
BEHIND          EQU     21
AFTER           EQU     22
UPSIDEDOWN      EQU     23
 
*--------------------------------
*
*  section numbers, NB cannot simply re-order, eg code assumes
*                               nouns follow verbs and adjectives
*
 
SECT.QUANT      EQU     0                       ;quantifiers - a,the,some,an,etc
SECT.ADV        EQU     1                       ;real adverbs - carefully,quietly,etc
SECT.CONJ       EQU     2
SECT.SPEC       EQU     3
SECT.VERB       EQU     4
SECT.ADJ        EQU     5
SECT.NOUN       EQU     6
SECT.DIR        EQU     7
SECT.CONT       EQU     8
SECT.QUEST      EQU     9
SECT.TOBE       EQU     10
SECT.SYN        EQU     11
SECT.YOU        EQU     12
SECT.WANT       EQU     13
SECT.THINK      EQU     14
SECT.NOT        EQU     15
SECT.INSULT     EQU     16
SECT.HELLO      EQU     17
SECT.MULT       EQU     18
SECT.MODEL      EQU     19
SECT.VAL        EQU     20
SECT.YESNO      EQU     21 
SECT.REL        EQU     22      
SECT.ONE        EQU     23 
SECT.PPRN       EQU     24
SECT.WBITS      EQU     25
SECT.STATE      EQU     26
SECT.NUM        EQU     27
SECT.SCEN       EQU     28              ;scenary nouns 

*--------------------------------
 
CONT.MASK       EQU     $20000000       ;bits used in location info
REL.MASK        EQU     $01000000
HIDN.MASK       EQU     $02000000
WORN.MASK       EQU     $04000000
INSP.MASK       EQU     $10000000

*--------------------------------
*
* now the REAl equates
*
 
MAXWGHT        EQU  $0E           ;max weight u can carry
HI             EQU  $8000         ;used to set hi bits
SIGCHAR        EQU  7             ;no of chars search looks at
BIGWRD         EQU  26            ;size of left just buffer
SLIM           EQU  3             ;no. of chars nec. for plural checks
Remastered     EQU  TRUE


