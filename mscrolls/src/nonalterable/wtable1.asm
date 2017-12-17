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
*	@(#)wtable1.asm	2.13
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*    SRC.ADV.WTABLE1.4
*
*--------------------------------


	XDEF   TBL.WRD,CONJST,WTAB_VERBS
 
TBL.WRD
 
*
*  SECTION #0
*
	DC.B   'TH',$45+$80
	DC.B   $41+$80
	DC.B   'SOM',$45+$80
	DC.B   'A',$4E+$80
	DC.B   'FE',$57+$80
	DC.B   'MOS',$54+$80
	DC.B   'THI',$53+$80
	DC.B    $82              ;END OF SECTION
*
*  SECTION #1
*
	DC.B   'quietl',$79+$80
	DC.B   'carefull',$79+$80
	DC.B   'quickl',$79+$80
	DC.B   'pleas',$65+$80
	DC.B   'GENTL',$59+$80

      IFEQ	THE_PAWN

	DC.B	$30+$80		;people want numbers (!?!)
	DC.B	$31+$80
	DC.B	$32+$80
	DC.B	$33+$80
	DC.B	$34+$80
	DC.B	$35+$80
	DC.B	$36+$80
	DC.B	$37+$80
	DC.B	$38+$80
	DC.B	$39+$80
	DC.B	$3A+$80		;':'

      ENDC

	DC.B    $82              ;END OF SECTION
*
*  SECTION #2
*
	
CONJST
	
	DC.B   'wit',$68+$80
	DC.B   't',$6F+$80
	DC.B   'fro',$6D+$80
	DC.B   'o',$6E+$80
	DC.B   'int',$6F+$80
	DC.B   'i',$6E+$80
	DC.B   'fo',$72+$80
	DC.B   'b',$79+$80
	DC.B   'ove',$72+$80
	DC.B   'unde',$72+$80
	DC.B   'acros',$73+$80
	DC.B   'a',$74+$80
	DC.B   'aroun',$64+$80
	DC.B   'WITHI',$4E+$80
	DC.B   'throug',$68+$80
	DC.B   'of',$66+$80
	DC.B   'ont',$6F+$80
	DC.B   'agains',$74+$80
	DC.B   'abou',$74+$80
	DC.B   'TOGETHE',$52+$80

      IFEQ	THE_PAWN
	DC.B	'UNTI',$4C+$80
      ENDC

	DC.B    $82              ;END OF SECTION
*
*  SECTION #3
*
	DC.B   'an',$64+$80
	DC.B   'al',$6C+$80
	DC.B   'everythin',$67+$80
	DC.B   'i',$74+$80
	DC.B   'the',$6D+$80
	DC.B   'he',$72+$80
	DC.B   'hi',$6D+$80
	DC.B   'agai',$6E+$80
	DC.B   'the',$6E+$80
	DC.B   'excep',$74+$80
	DC.B   'content',$73+$80
	DC.B   'o',$66+$80
	DC.B    $82              ;END OF SECTION
