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
*	@(#)wtable7.asm	2.13
*	8/28/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   WTABLE7
*
*--------------------------------

	XDEF    L.DIREC,INSULT,SynText
 
*
*  SECTION #7
*
	
L.DIREC 
 
	DC.B   'nort',$68+$80
	DC.B   'northeas',$74+$80
	DC.B   'eas',$74+$80
	DC.B   'southeas',$74+$80
	DC.B   'sout',$68+$80
	DC.B   'southwes',$74+$80
	DC.B   'wes',$74+$80
	DC.B   'northwes',$74+$80
	DC.B   'u',$70+$80
	DC.B   'dow',$6E+$80
	DC.B    $82              ;END OF SECTION

*
*
*  SECTION #8
*
*   Debugbits and DO.SING stuff go here
*
*


	DC.B   'NORMA',$4C+$80
	DC.B   'VERBOS',$45+$80
	DC.B   'BRIE',$46+$80
	DC.B   'VERSIO',$4E+$80
	DC.B   'SAV',$45+$80
	DC.B   'QUI',$54+$80
	DC.B   'SCOR',$45+$80
	DC.B   'RESTOR',$45+$80
	DC.B   'LOA',$44+$80
	DC.B   'RESTAR',$54+$80
	DC.B   'SERIA',$4C+$80
	DC.B   'I',$44+$80
	
	IFNE     YCHEAT
	 
	DC.B   'WHATI',$53+$80
	DC.B   'S',$47+$80
	DC.B   'WHEREI',$53+$80
	DC.B   'ZA',$50+$80
	DC.B   'GOT',$4F+$80
	DC.B   'TEL',$45+$80
	DC.B   'NOU',$4E+$80
	DC.B   'MS',$47+$80
	DC.B   'SC',$4E+$80
	
	ENDC
	
	DC.B   'P',$4E+$80
	DC.B   'PRONOUN',$53+$80
	
	IFNE     YCHEAT
	DC.B   'AD',$4A+$80
	ENDC

	IFNE	M68000
	DC.B   'LEF',$54+$80
	DC.B   'RIGH',$54+$80
	ENDC
	
	IFNE     YCHEAT
	
	DC.B   'VIDE',$4F+$80
	DC.B   'SUS',$4E+$80
	DC.B   'SUS',$52+$80
	DC.B   'SUSC',$52+$80
    
	ENDC
	
	IFNE	M68000
	DC.B   'CONTRAS',$54+$80
	ENDC
 
	IFNE     YCHEAT
	DC.B   'WOR',$44+$80
	DC.B   'ME',$4D+$80
	DC.B   'MO',$44+$80
	ENDC

      IFNE     AMIGA512&YCHEAT
	DC.B    'BPO',$4E+$80
	DC.B    'BPOF',$46+$80
	DC.B    'MIS',$43+$80
      ENDC

      IFNE    Graphics
	DC.B   'GRAPHIC',$53+$80
      IFNE   YCHEAT

	DC.B    'PICTUR',$45+$80

      ENDC

      IFNE	AMIGA512!ST520	  
	DC.B   'TEXTSIZ',$45+$80
      ENDC
	
	IFNE    Mono_Possible
	DC.B   'STIPPL',$45+$80
	DC.B   'DITHE',$52+$80
	DC.B   'FREEHAN',$44+$80
	ENDC

      ENDC

      IFNE    SpoolLen
	DC.B   'PRINTE',$52+$80
      ENDC

      IFNE    FunctionKeys
	DC.B   'FUNCTIO',$4E+$80
	DC.B   'FUNC',$54+$80
      ENDC

	DC.B    'CREDIT',$53+$80
	DC.B    'HIN',$54+$80

      IFNE     Wimp
	DC.B    'FOREGROUN',$44+$80
	DC.B    'BACKGROUN',$44+$80
	DC.B    'SCROL',$4C+$80
      ENDC

      IFNE     YCHEAT
	DC.B    'coun',$54+$80
      ENDC

      IFNE     SpeechAvail
	DC.B    'speec',$68+$80
      ENDC

      IFNE	PCW8256
       DC.B   'STIPPL',$45+$80
       DC.B   'DITHE',$52+$80
      ENDC

      IFNE	IBM
	DC.B	'TITL',$45+$80
	DC.B	'MOR',$45+$80
	DC.B	'PICTUR',$45+$80
	DC.B	'TEX',$54+$80
	DC.B	'FASTE',$52+$80
	DC.B	'SLOWE',$52+$80
      ENDC

      IFNE	YCHEAT
	DC.B	'SY',$6E+$80	;syn - list synonyms of xyzzy
	DC.B	'VNE',$54+$80	;vnet - verify network
      ENDC
       
      IFD	C64Pict
	DC.B    'PICTUR',$45+$80
      ENDC
	
	DC.B    $82              ;END OF SECTION
*
*  SECTION #9
*
	DC.B   'WHER',$45+$80
	DC.B   'WH',$4F+$80
	DC.B   'WHA',$54+$80
	DC.B   'WHE',$4E+$80
	DC.B   'WH',$59+$80
	DC.B   'HO',$57+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #10
*
	DC.B   'A',$4D+$80
	DC.B   'AR',$45+$80
	DC.B   'I',$53+$80
	DC.B   'WER',$45+$80
	DC.B   'WA',$53+$80
	DC.B    $82              ;END OF SECTION



*************************************************************
*
*  SECTION #11 synonyms
*

SynText
	include	"wtable11misc.i"
	include "wtable11noun.i"


	DC.B    $82                      ;END OF SECTION

*************************************************************
*
*  SECTION #12 
*
	DC.B   'YO',$55+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #13
*
	DC.B   'WAN',$54+$80
	DC.B   'WIS',$48+$80
	DC.B   'LIK',$45+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #14
*
	DC.B   'THIN',$4B+$80
	DC.B   'THOUGH',$54+$80
	DC.B   'KNO',$57+$80
	DC.B   'KNE',$57+$80
	DC.B   'BORIN',$47+$80
	DC.B   'BORE',$44+$80
	DC.B   'SOLV',$45+$80
	DC.B   'WOR',$4B+$80
	DC.B   'UNDERSTAN',$44+$80
	DC.B   'SE',$45+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #15
*
	DC.B   'NO',$54+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #16
*
	
INSULT
	
	DC.B   'wall',$79+$80
	DC.B   'prat',$74+$80
	DC.B   'bastar',$64+$80
	DC.B   'pric',$6B+$80
	DC.B   'cun',$74+$80
	DC.B   'so',$64+$80
	DC.B   'shi',$74+$80
	DC.B   'wan',$6B+$80
	DC.B   'wanke',$72+$80
	DC.B   'idio',$74+$80
	DC.B   'foo',$6C+$80
	DC.B   'oa',$66+$80
	DC.B   'perver',$74+$80
	DC.B   'tosse',$72+$80
	DC.B   'flowerpo',$74+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #17
*
	DC.B   'HELL',$4F+$80
	DC.B   'H',$49+$80
	DC.B   'HOWD',$59+$80
	DC.B   'HIY',$41+$80
	DC.B   'GREETING',$53+$80
	DC.B    $82              ;END OF SECTION

*************************************************************
*
*  SECTION #18  'sets' or multiples
*

	include	"wtable18.i"

	DC.B    $82              ;END OF SECTION


*
*  SECTION #19
*
	DC.B   'MUS',$54+$80
	DC.B   'CA',$4E+$80
	DC.B   'MA',$59+$80
	DC.B   'WIL',$4C+$80
	DC.B   'SHAL',$4C+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #20
*
	DC.B   'MUC',$48+$80
	DC.B   'MAN',$59+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #21
*
	DC.B   'YE',$53+$80
	DC.B   'N',$4F+$80
	DC.B    $82              ;END OF SECTION
*
* SECTION #22
*
	DC.B   'THA',$54+$80
	DC.B   'WHIC',$48+$80
	DC.B   'WO',$54+$80
	DC.B   $82
	
*
* SECTION #23
*
	DC.B   'ON',$45+$80
	DC.B   $82
