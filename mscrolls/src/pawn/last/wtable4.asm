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
*	@(#)wtable4.asm	1.1
*	1/22/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*    SRC.ADV.WTABLE4
*
*--------------------------------

*
*  SECTION #4
*
* Verbs


WTAB_VERBS


	DC.B   'loo',$6B+$80
	DC.B   'examin',$65+$80
	DC.B   'ge',$74+$80
	DC.B   'tak',$65+$80
	DC.B   'carr',$79+$80
	DC.B   'dro',$70+$80
	DC.B   'leav',$65+$80
	DC.B   'g',$6F+$80
	DC.B   'wal',$6B+$80
	DC.B   'ente',$72+$80
	DC.B   'ru',$6E+$80
	DC.B   'shif',$74+$80
	DC.B   'mov',$65+$80
	DC.B   'lif',$74+$80
	DC.B   'rais',$65+$80
	DC.B   'lowe',$72+$80
	DC.B   'pus',$68+$80
	DC.B   'pul',$6C+$80
	DC.B   'smel',$6C+$80
	DC.B   'tast',$65+$80
	DC.B   'ea',$74+$80
	DC.B   'drin',$6B+$80
	DC.B   'pu',$74+$80
	DC.B   'inser',$74+$80
	DC.B   'inventor',$79+$80
	DC.B   'fi',$74+$80
	DC.B   'pou',$72+$80
	DC.B   'empt',$79+$80
	DC.B   'fil',$6C+$80
	DC.B   'mi',$78+$80
	DC.B   'jum',$70+$80
	DC.B   'vaul',$74+$80
	DC.B   'thro',$77+$80
	DC.B   'punc',$68+$80
	DC.B   'kic',$6B+$80
	DC.B   'figh',$74+$80
	DC.B   'attac',$6B+$80
	DC.B   'kil',$6C+$80
	DC.B   'brea',$6B+$80
	DC.B   'smas',$68+$80
	DC.B   'damag',$65+$80
	DC.B   'remov',$65+$80
	DC.B   'scrap',$65+$80
	DC.B   'ligh',$74+$80
	DC.B   'unligh',$74+$80
	DC.B   'extinguis',$68+$80
	DC.B   'ope',$6E+$80
	DC.B   'clos',$65+$80

      IFNE    YCHEAT

	DC.B   'SUS',$53+$80
	DC.B   'STATU',$53+$80
	DC.B   'DEBU',$47+$80

      ENDC

      IFEQ     YCHEAT

	DC.B    ' ',$80,' ',$80
	DC.B    'debu',$67+$80

      ENDC

	DC.B   'fir',$65+$80
	DC.B   'shoo',$74+$80
	DC.B   'blas',$74+$80
	DC.B   'twis',$74+$80
	DC.B   'tur',$6E+$80
	DC.B   'sa',$79+$80
	DC.B   'shou',$74+$80
	DC.B   'yel',$6C+$80
	DC.B   'cr',$79+$80
	DC.B   'cal',$6C+$80
	DC.B   'strangl',$65+$80
	DC.B   'chok',$65+$80
	DC.B   'fee',$6C+$80
	DC.B   'touc',$68+$80
	DC.B   'wea',$72+$80
	DC.B   'rea',$64+$80
	DC.B   'hid',$65+$80
	DC.B   'hi',$74+$80
	DC.B   'stea',$6C+$80
	DC.B   'bas',$68+$80
	DC.B   'unloc',$6B+$80
	DC.B   'pic',$6B+$80
	DC.B   'ti',$65+$80
	DC.B   'fi',$78+$80
	DC.B   'joi',$6E+$80
	DC.B   'unti',$65+$80
	DC.B   'fee',$64+$80
	DC.B   'bit',$65+$80
	DC.B   'us',$65+$80
	DC.B   'cros',$73+$80
	DC.B   'slid',$65+$80
	DC.B   'blo',$77+$80
	DC.B   'pres',$73+$80
	DC.B   'giv',$65+$80
	DC.B   'clim',$62+$80
	DC.B   'bur',$6E+$80
	DC.B   'ignit',$65+$80
	DC.B   'cas',$74+$80

      IFEQ    YCHEAT

	DC.B   $20

      ENDC


	DC.B   'fo',$64+$80
	DC.B   'exi',$74+$80
	DC.B   'as',$6B+$80
	DC.B   'questio',$6E+$80
	DC.B   'mel',$74+$80
	DC.B   'liste',$6E+$80
	DC.B   'searc',$68+$80
	DC.B   'fin',$64+$80
	DC.B   'gra',$62+$80
	DC.B   'ru',$62+$80
	DC.B   'smok',$65+$80
	DC.B   'wai',$74+$80
	DC.B   'wak',$65+$80
	DC.B   'rap',$65+$80
	DC.B   'men',$64+$80
	DC.B   'leve',$72+$80
	DC.B   'bu',$79+$80
	DC.B   'purchas',$65+$80
	DC.B   'lea',$6E+$80
	DC.B   'rid',$65+$80
	DC.B   'plan',$74+$80
	DC.B   'loc',$6B+$80
	DC.B   'tea',$72+$80
	DC.B   'ri',$70+$80
	DC.B   'cu',$74+$80
	DC.B   'knoc',$6B+$80
	DC.B   'poin',$74+$80
	DC.B   'shin',$65+$80
	DC.B   'spli',$74+$80
	DC.B   'unmi',$78+$80
	DC.B   'separat',$65+$80
	DC.B   'exit',$73+$80
	DC.B   'cove',$72+$80
	DC.B   'uncove',$72+$80
	DC.B   'si',$74+$80
	DC.B   'li',$65+$80
	DC.B   'kis',$73+$80
	DC.B   'vot',$65+$80
	DC.B   'stan',$64+$80
	DC.B   'moun',$74+$80
	DC.B   'tel',$6C+$80
	DC.B   'offe',$72+$80
	DC.B   'dismoun',$74+$80
	DC.B   'hel',$70+$80
	DC.B   'laug',$68+$80
	DC.B   'sho',$77+$80

      IFNE   YCHEAT
	DC.B   'loo',$70+$80
      ENDC

      IFEQ	THE_PAWN
	DC.B	'di',$67+$80		;dig
	DC.B	'be',$74+$80		;bet
	DC.B	'sto',$70+$80		;stop
	DC.B	'pla',$79+$80		;play
	DC.B	'swi',$6D+$80		;swim
	DC.B	'flus',$68+$80		;flush
	DC.B	'pis',$73+$80		;piss
	DC.B	'slee',$70+$80		;sleep
	DC.B	'se',$74+$80		;set	(as in time)
	DC.B	'win',$64+$80		;wind
	DC.B	'che',$77+$80		;chew
	DC.B	'reflec',$74+$80	;reflect
	DC.B	'hol',$64+$80		;hold
	DC.B	'plac',$65+$80		;place
	DC.B	'positio',$6E+$80	;position
	DC.B	'illuminat',$65+$80	;illuminate
	DC.B	'catc',$68+$80		;catch
	DC.B	'pra',$79+$80		;pray
	DC.B	'cu',$65+$80		;cue (=hit)
	DC.B	'po',$74+$80		;pot (=hit)
	DC.B	'shak',$65+$80		;shake
	DC.B	'rol',$6C+$80		;roll (die)

      ENDC

	DC.B    $82              ;END OF SECTION:

