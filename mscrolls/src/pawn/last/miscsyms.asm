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
*	@(#)miscsyms.asm	2.1
*	9/19/86
*
******************************************************

              SYNMACRO    NGURU-1,SECT.NOUN        ;SAGE
              SYNMACRO    NBAND-1,SECT.NOUN        ;BAND
              SYNMACRO    $41,SECT.NOUN            ;BARROW
              SYNMACRO    NPOUCH-1,SECT.NOUN       ;BAG
              SYNMACRO    NGATEWAY-1,SECT.NOUN     ;GATE
              SYNMACRO    DIR_N,SECT.DIR           ;N
              SYNMACRO    DIR_NE,SECT.DIR          ;NE
              SYNMACRO    DIR_E,SECT.DIR           ;E
              SYNMACRO    DIR_SE,SECT.DIR          ;SE
              SYNMACRO    DIR_S,SECT.DIR           ;S
              SYNMACRO    DIR_SW,SECT.DIR          ;SW
              SYNMACRO    DIR_W,SECT.DIR           ;W
              SYNMACRO    DIR_NW,SECT.DIR          ;NW
              SYNMACRO    DIR_U,SECT.DIR           ;U
              SYNMACRO    DIR_D,SECT.DIR           ;D
              SYNMACRO    VNINVENTORY,SECT.VERB    ;I
              SYNMACRO    VNLOOK,SECT.VERB         ;L
              SYNMACRO    0,SECT.CONJ              ;USING = WITH
              SYNMACRO    1,SECT.ADJ               ;EYE-DAZZLING
              SYNMACRO    $003,SECT.ADJ            ;DECAYING
              SYNMACRO    $005,SECT.ADJ            ;DREARY
              SYNMACRO    $008,SECT.ADJ            ;EXPANSE
              SYNMACRO    $03E,SECT.ADJ            ;SNOW
              SYNMACRO    $03E,SECT.ADJ            ;COVEREDE
              SYNMACRO    NBED-1,SECT.NOUN         ;POSTER = BED
              SYNMACRO    $03C,SECT.ADJ            ;STOUT
              SYNMACRO    $06B,SECT.ADJ            ;WINDING
              SYNMACRO    $062,SECT.ADJ            ;WISPY
              SYNMACRO    $04B,SECT.NOUN           ;BENCH
              SYNMACRO    $049,SECT.NOUN           ;FLOWERS
              SYNMACRO    9,SECT.SPEC              ;BUT
              SYNMACRO    NFLOORBOARD-1,SECT.NOUN  ;PLANKS
              SYNMACRO    $0EF,SECT.NOUN           ;PAPER (NOTE)
              SYNMACRO    $0E2,SECT.NOUN           ;KRONOS
              SYNMACRO    VNGET,SECT.VERB          ;G=GET
              SYNMACRO    VNDROP,SECT.VERB         ;DR=DROP
              SYNMACRO    5,SECT.CONJ              ;INSIDE=IN !
              SYNMACRO    VNEXIT,SECT.VERB         ;O=OUT=EXIT
              SYNMACRO    NDONKEY-1,SECT.NOUN
              SYNMACRO    NBOARD-1,SECT.NOUN       ;NOTICE BOARD
              SYNMACRO    NREM-1,SECT.NOUN         ;REM STATEMNT = STATEMENT
              SYNMACRO    VNCLOSE,SECT.VERB        ;SHUT = CLOSE
              SYNMACRO    NPLANT-1,SECT.NOUN       ;WEED
              SYNMACRO    NPLANT-1,SECT.NOUN       ;DOPE
              SYNMACRO    NPLANT-1,SECT.NOUN       ;CANNABIS
              SYNMACRO    NFLOORBOARD-1,SECT.NOUN  ;PLANKS
              SYNMACRO    1,SECT.TOBE              ;R=ARE
              SYNMACRO    2,SECT.CONJ              ;F = FROM
              SYNMACRO    0,SECT.CONJ              ;WI = WITH
              SYNMACRO    VNGET,SECT.VERB          ;ACCEPT - GET
              SYNMACRO    NBED-1,SECT.NOUN         ;BED POST = BED
              SYNMACRO    1,SECT.SPEC              ;THINGS = ALL
              SYNMACRO    NTOMES-1,SECT.NOUN       ;TOMES = BOOK
              SYNMACRO    NAEROBUTTON-1,SECT.NOUN  ;AEROSOUL BUTTON = NOZZLE
              SYNMACRO    VNEXIT,SECT.VERB         ;OUT=EXIT=O
              SYNMACRO    1,SECT.MODEL             ;COULD
              SYNMACRO    1,SECT.MODEL             ;DO
              SYNMACRO    1,SECT.MODEL             ;DOES
              SYNMACRO    1,SECT.MODEL             ;DID
              SYNMACRO    2,SECT.MODEL             ;MIGHT
              SYNMACRO    3,SECT.MODEL             ;WILL
              SYNMACRO    4,SECT.MODEL             ;SHOULD
              SYNMACRO    NALE-1,SECT.NOUN         ;BEER = ALE
              SYNMACRO    NGRAVEL-1,SECT.NOUN      ;STONES
              SYNMACRO    NROCKS-1,SECT.NOUN       ;ROCK
              SYNMACRO    NLEWIS-1,SECT.NOUN       ;JERRY
              SYNMACRO    NLEWIS-1,SECT.NOUN       ;LEE
              SYNMACRO    NLEWIS-1,SECT.NOUN       ;LEWIS
              SYNMACRO    NDOORS-1,SECT.NOUN       ;DOOR=DOORS (PERSPEX)
              SYNMACRO    NSHED-1,SECT.NOUN        ;TOOLSHED = SHED
              SYNMACRO    VNMEND,SECT.VERB
              SYNMACRO    0,SECT.YESNO
              SYNMACRO    0,SECT.YESNO             ;YEAH
              SYNMACRO    1,SECT.YESNO             ;NA
              SYNMACRO    NERIK-1,SECT.NOUN        ;KING=ERIK
              SYNMACRO    NSETTEE-1,SECT.NOUN
              SYNMACRO    NSHAPES-1,SECT.NOUN
              SYNMACRO    9,SECT.CONJ              ;BEHIND!
              SYNMACRO    VNINVENTORY,SECT.VERB
              SYNMACRO    NRABBIT-1,SECT.NOUN
              SYNMACRO    NRABBIT-1,SECT.NOUN
              SYNMACRO    NPLAYER-1,SECT.NOUN
              SYNMACRO    NPLAYER-1,SECT.NOUN
              SYNMACRO    NMAGNUM-1,SECT.NOUN
              SYNMACRO    1,SECT.TOBE              ;ARE
              SYNMACRO    2,SECT.TOBE              ;IS
              SYNMACRO    VNJUMP,SECT.VERB
              SYNMACRO    VNLAUGH,SECT.VERB
              SYNMACRO    NBUSHES-1,SECT.NOUN
              SYNMACRO    VNINVENTORY,SECT.VERB
              SYNMACRO    NCUPBOARD-1,SECT.NOUN
              SYNMACRO    NLAB-1,SECT.NOUN
              SYNMACRO    VNGO,SECT.VERB
              SYNMACRO    VNWEAR,SECT.VERB
              SYNMACRO    2,SECT.MULT              ;LIGHT = COLOURS
              SYNMACRO    VNMIX,SECT.VERB          ;MERGE = MIX
              SYNMACRO    9,SECT.CONJ              ;BELOW = UNDER
              SYNMACRO    VNFIX,SECT.VERB          ;ATTACH = TIE/FIX
              SYNMACRO    NGATEWAY-1,SECT.NOUN
              SYNMACRO    NFOUNTAIN-1,SECT.NOUN
              SYNMACRO    VNMIX,SECT.VERB          ;BLEND = MIX
              SYNMACRO    NCHIT-1,SECT.NOUN
              SYNMACRO    NCOIN-1,SECT.NOUN
              SYNMACRO    NLUMPS-1,SECT.NOUN
              SYNMACRO    5,SECT.CONT              ;DIE= QUIT
              SYNMACRO    NBRIDGE5-1,SECT.NOUN     ;CREAKS=BRIDGE (FOR LISTEN)
              SYNMACRO    $C,SECT.CONJ             ;ROUND= AROUND
              SYNMACRO    NARMS-1,SECT.NOUN
              SYNMACRO    0,SECT.MULT              ;EQIP= TOOLS
              SYNMACRO    NSHIRT-1,SECT.NOUN
              SYNMACRO    NFERGS-1,SECT.NOUN
              SYNMACRO    NSTAIRCASE-1,SECT.NOUN
              SYNMACRO    VNSMELL,SECT.VERB        ;SNIFF = SMELL
              SYNMACRO    $2E,SECT.ADJ             ;MINE - MY
              SYNMACRO    2,SECT.MULT
              SYNMACRO    NLIFT-1,SECT.NOUN
              SYNMACRO    VNSAY,SECT.VERB
*	      SYNMACRO	  NDOOR-1,SECT.NOUN to stop can see doorS here
              SYNMACRO    NFLOORBOARD-1,SECT.NOUN  ;board
	SYNMACRO	  NNOTE-1,SECT.NOUN
    	      SYNMACRO	  VNWAIT,SECT.VERB


* no END statement - this file is INCLUDE'd by COMMON.ASM

