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
*	@(#)wtable6.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*      SRC.ADV.WTABLE6A
*
*--------------------------------
 
       XDEF    LIST.N
 
*
*  SECTION #6
*
       
LIST.N
         
       DC.B   'On The Pat',$68+$80
       DC.B   'On The Pat',$68+$80
       DC.B   'On The Pat',$68+$80
       DC.B   'Voi',$64+$80
       DC.B   'Fores',$74+$80
       DC.B   'Fores',$74+$80
       DC.B   'Clearin',$67+$80
       DC.B   'Fores',$74+$80
       DC.B   'Fores',$74+$80
       DC.B   'Fores',$74+$80
       DC.B   'Fores',$74+$80
       DC.B   'On The Pat',$68+$80
       DC.B   'On The Pat',$68+$80
       DC.B   'Plai',$6E+$80
       DC.B   'Wildernes',$73+$80
       DC.B   'Plai',$6E+$80
       DC.B   'Ban',$6B+$80
       DC.B   'Stone Bridg',$65+$80
       DC.B   'Cliff',$20+$80
       DC.B   'On The Bridg',$65+$80
       DC.B   'Garden',$73+$80
       DC.B   'Gatewa',$79+$80
       DC.B   'Maz',$65+$80
       DC.B   'Hill',$73+$80
       DC.B   'Hill',$73+$80
       DC.B   'Hil',$6C+$80
       DC.B   'Hill',$73+$80
       DC.B   'Foothill',$73+$80
       DC.B   'Inside The Hu',$74+$80
       DC.B   'In The She',$64+$80
       DC.B   'shir',$74+$80
       DC.B   'lin',$65+$80
       DC.B   'notic',$65+$80
       DC.B   'writin',$67+$80
       DC.B   'bridg',$65+$80
       DC.B   'po',$74+$80
       DC.B   'she',$64+$80
       DC.B   'terrai',$6E+$80
       DC.B   'archwa',$79+$80
       DC.B   'guard',$73+$80
       DC.B   'emblem',$73+$80
       DC.B   'pike',$73+$80
       DC.B   'hu',$74+$80
       DC.B   'mu',$64+$80
       DC.B   'plan',$74+$80
       DC.B   'jean',$73+$80
       DC.B   'armou',$72+$80
       DC.B   'ponch',$6F+$80
       DC.B   'stetso',$6E+$80
       DC.B   'cheroo',$74+$80
       DC.B   'magnu',$6D+$80
       DC.B   'Joh',$6E+$80
       DC.B   'car',$74+$80
       DC.B   'donke',$79+$80
       DC.B   'wal',$6C+$80
       DC.B   'wir',$65+$80
       DC.B   'fountai',$6E+$80
       DC.B   'adventure',$72+$80
       DC.B   'hors',$65+$80
       DC.B   'grave',$6C+$80
       DC.B   'ho',$65+$80
       DC.B   'pouc',$68+$80
       DC.B   'trowe',$6C+$80
       DC.B   'rak',$65+$80
       DC.B   'clif',$66+$80
       DC.B   'wheelbarro',$77+$80
       DC.B   'blu',$65+$80
       DC.B   'gree',$6E+$80
       DC.B   're',$64+$80
       DC.B   'gur',$75+$80
       DC.B   'rob',$65+$80
       DC.B   'bow',$6C+$80
       DC.B   'boulder',$73+$80
       DC.B   'border',$73+$80
       DC.B   'bushe',$73+$80
       DC.B   'workbenc',$68+$80
       DC.B   'wristban',$64+$80
       DC.B   'waterfal',$6C+$80
       DC.B   'fo',$67+$80
       DC.B   'pat',$68+$80
       DC.B   'ric',$65+$80
       DC.B   'potio',$6E+$80
       DC.B   'Track',$20+$80
       DC.B   'Track',$20+$80
       DC.B   'Track',$20+$80
       DC.B   'Platea',$75+$80
       DC.B   'Platea',$75+$80
       DC.B   'Platea',$75+$80
       DC.B   'Platea',$75+$80
       DC.B   'Inside The Towe',$72+$80
       DC.B   'Landin',$67+$80
       DC.B   'Room Of Incarceratio',$6E+$80
       DC.B   'Wal',$6C+$80
       DC.B   'Stor',$65+$80
       DC.B   'Cavemout',$68+$80
       DC.B   'Caver',$6E+$80
       DC.B   'Caver',$6E+$80
       DC.B   'Cav',$65+$80
       DC.B   'Corrido',$72+$80
       DC.B   'Shaf',$74+$80
       DC.B   'Caver',$6E+$80
       DC.B   'Entranc',$65+$80
       DC.B   'Corrido',$72+$80
       DC.B   'Lava Rive',$72+$80
       DC.B   'Roo',$6D+$80
       DC.B   'Passag',$65+$80
       DC.B   'Laborator',$79+$80
       DC.B   'Roo',$6D+$80
       DC.B   'Passag',$65+$80
       DC.B   'Hel',$6C+$80
       DC.B   'Caver',$6E+$80
       DC.B   'Chambe',$72+$80
       DC.B   'Roo',$6D+$80
       DC.B   'Annex',$65+$80
       DC.B   'Shaf',$74+$80
       DC.B   'High Ledg',$65+$80
       DC.B   'Rope Bridg',$65+$80
       DC.B   'Rope Bridg',$65+$80
       DC.B   'High Ledg',$65+$80
       DC.B   'Chambe',$72+$80
       DC.B   'Worksho',$70+$80
       DC.B   'Circular Platfor',$6D+$80
       DC.B   'Tunne',$6C+$80
       DC.B   'Roo',$6D+$80
       DC.B   'Ledg',$65+$80
       DC.B   'Passag',$65+$80
       DC.B   'Shaf',$74+$80
       DC.B   'Ledg',$65+$80
       DC.B   'Chambe',$72+$80
       DC.B   'Trac',$6B+$80
       DC.B   'Rockfal',$6C+$80
       DC.B   'Cav',$65+$80
       DC.B   'Stairwa',$79+$80
       DC.B   'Platea',$75+$80
       DC.B   'Towe',$72+$80
       DC.B   'sno',$77+$80
       DC.B   'slop',$65+$80
       DC.B   'doo',$72+$80
       DC.B   'strap',$73+$80
       DC.B   'be',$64+$80
       DC.B   'boot',$73+$80
       DC.B   'rop',$65+$80
       DC.B   'bat',$73+$80
       DC.B   'poo',$6C+$80
       DC.B   'stalactite',$73+$80
       DC.B   'stalagmite',$73+$80
       DC.B   're',$6D+$80
       DC.B   'rive',$72+$80
       DC.B   'lav',$61+$80
       DC.B   'pedesta',$6C+$80
       DC.B   'notic',$65+$80
       DC.B   'door',$73+$80
       DC.B   'lamp',$73+$80
       DC.B   'window',$73+$80
       DC.B   'doo',$72+$80
       DC.B   'pape',$72+$80
       DC.B   'listin',$67+$80
       DC.B   'fa',$6E+$80
       DC.B   'as',$68+$80
       DC.B   'computer',$73+$80
       DC.B   'peripheral',$73+$80
       DC.B   'ma',$74+$80
       DC.B   'limbeck',$73+$80
       DC.B   'flask',$73+$80
       DC.B   'liqui',$64+$80
       DC.B   'tube',$73+$80
       DC.B   'vapou',$72+$80
       DC.B   'boxe',$73+$80
       DC.B   'jar',$73+$80
       DC.B   'ches',$74+$80
       DC.B   'tur',$66+$80
       DC.B   'bottl',$65+$80
       DC.B   'geyser',$73+$80
       DC.B   'vitrio',$6C+$80
       DC.B   'scythe',$73+$80
       DC.B   'Devi',$6C+$80
       DC.B   'thron',$65+$80
       DC.B   'eye',$73+$80
       DC.B   'demon',$73+$80
       DC.B   'corpse',$73+$80
       DC.B   'noose',$73+$80
       DC.B   'fridg',$65+$80
       DC.B   'head',$73+$80
       DC.B   'ke',$79+$80
       DC.B   'crystal',$73+$80
       DC.B   'pian',$6F+$80

*--------------------------------
*
*      SRC.ADV.WTABLE6B
*
*--------------------------------
  
 
       DC.B   'Jerry Lee Lewi',$73+$80
       DC.B   'shaf',$74+$80
       DC.B   'ladde',$72+$80
       DC.B   'clou',$64+$80
       DC.B   'ledg',$65+$80
       DC.B   'bridg',$65+$80
       DC.B   'plank',$73+$80
       DC.B   'drago',$6E+$80
       DC.B   'treasur',$65+$80
       DC.B   'shadow',$73+$80
       DC.B   'ke',$79+$80
       DC.B   'lam',$70+$80
       DC.B   'tabl',$65+$80
       DC.B   'ha',$74+$80
       DC.B   'rabbi',$74+$80
       DC.B   'bookcase',$73+$80
       DC.B   'tome',$73+$80
       DC.B   'nich',$65+$80
       DC.B   'platfor',$6D+$80
       DC.B   'knocke',$72+$80
       DC.B   'post',$73+$80
       DC.B   'cupboar',$64+$80
       DC.B   'hoo',$6B+$80
       DC.B   'past',$65+$80
       DC.B   'wallpape',$72+$80
       DC.B   'door',$73+$80
       DC.B   'porte',$72+$80
       DC.B   'scream',$73+$80
       DC.B   'wal',$6C+$80
       DC.B   'rive',$72+$80
       DC.B   'wate',$72+$80
       DC.B   'snowma',$6E+$80
       DC.B   'pocke',$74+$80
       DC.B   'princes',$73+$80
       DC.B   'graffit',$69+$80
       DC.B   'pape',$72+$80
       DC.B   'chi',$74+$80
       DC.B   'wate',$72+$80
       DC.B   'shape',$73+$80
       DC.B   'aerosou',$6C+$80
       DC.B   'magicia',$6E+$80
       DC.B   'ha',$74+$80
       DC.B   'cloa',$6B+$80
       DC.B   'rac',$6B+$80
       DC.B   'wan',$64+$80
       DC.B   'ches',$74+$80
       DC.B   'nich',$65+$80
       DC.B   'rat',$73+$80
       DC.B   'tabl',$65+$80
       DC.B   'alchemist',$73+$80
       DC.B   'rock',$73+$80
       DC.B   'stum',$70+$80
       DC.B   'wand',$73+$80
       DC.B   'not',$65+$80
       DC.B   'ravin',$65+$80
       DC.B   'whisk',$79+$80
       DC.B   'cupboar',$64+$80
       DC.B   'Eri',$6B+$80
       DC.B   'pris',$6D+$80
       DC.B   'ke',$79+$80
       DC.B   'wate',$72+$80
       DC.B   'symbol',$73+$80
       DC.B   'dres',$73+$80
       DC.B   'sea',$6C+$80
       DC.B   'ration',$73+$80
       DC.B   'butto',$6E+$80
       DC.B   'armou',$72+$80
       DC.B   'bottl',$65+$80
       DC.B   'ros',$65+$80
       DC.B   'coi',$6E+$80
       DC.B   'al',$65+$80
       DC.B   'flas',$6B+$80
       DC.B   'poste',$72+$80
       DC.B   'han',$64+$80
       DC.B   'fee',$74+$80
       DC.B   'arm',$73+$80
       DC.B   'fis',$74+$80
       DC.B   'hea',$64+$80
       DC.B   'bel',$74+$80
       DC.B   'bottl',$65+$80
       DC.B   'quil',$74+$80
       DC.B   'bottl',$65+$80
       DC.B   'dus',$74+$80
       DC.B   'tre',$65+$80
       DC.B   'Up The Tre',$65+$80
       DC.B   'doo',$72+$80
       DC.B   'doo',$72+$80
       DC.B   'Roo',$6D+$80
       DC.B   'Roo',$6D+$80
       DC.B   'Roo',$6D+$80
       DC.B   'Crossroad',$73+$80
       DC.B   'Passag',$65+$80
       DC.B   'Loung',$65+$80
       DC.B   'Kitche',$6E+$80
       DC.B   'Passag',$65+$80
       DC.B   'Lif',$74+$80
       DC.B   'Rockfac',$65+$80
       DC.B   'Roo',$6D+$80
       DC.B   'Offic',$65+$80
       DC.B   'Boot',$68+$80
       DC.B   'Chambe',$72+$80
       DC.B   'board',$73+$80
       DC.B   'staircas',$65+$80
       DC.B   'whit',$65+$80
       DC.B   'tre',$65+$80
       DC.B   'writin',$67+$80
       DC.B   'doo',$72+$80
       DC.B   'sette',$65+$80
       DC.B   'cushion',$73+$80
       DC.B   'ha',$74+$80
       DC.B   'hook',$73+$80
       DC.B   'stov',$65+$80
       DC.B   'teapo',$74+$80
       DC.B   'carro',$74+$80
       DC.B   'messag',$65+$80
       DC.B   'doo',$72+$80
       DC.B   'workto',$70+$80
       DC.B   'plaqu',$65+$80
       DC.B   'butto',$6E+$80
       DC.B   'lump',$73+$80
       DC.B   'des',$6B+$80
       DC.B   'saf',$65+$80
       DC.B   'pape',$72+$80
       DC.B   'bo',$78+$80
       DC.B   'tabl',$65+$80
       DC.B   'ru',$67+$80
       DC.B   'lanter',$6E+$80
       DC.B   'tree',$73+$80
       DC.B   'Ledg',$65+$80
       DC.B   'playe',$72+$80
       DC.B   'spel',$6C+$80
       DC.B   'palac',$65+$80           ;PSEUDO
       DC.B   'In Palac',$65+$80        ;ROOM
       DC.B   'hea',$64+$80
       DC.B   'bo',$78+$80
       DC.B   'butto',$6E+$80
       DC.B   'door',$73+$80
       DC.B   'wal',$6C+$80
       DC.B   'wal',$6C+$80
       DC.B   'windo',$77+$80
       DC.B   'windo',$77+$80
       DC.B   'butto',$6E+$80
       DC.B   'voic',$65+$80
       DC.B   'trap-doo',$72+$80
       DC.B   'goo',$64+$80
       DC.B   'evi',$6C+$80
       DC.B   'lif',$65+$80
       DC.B   'deat',$68+$80
       DC.B   'gring',$6F+$80
       DC.B   'gnome',$73+$80
       DC.B   'Kerovni',$61+$80
       DC.B   'perso',$6E+$80
       DC.B   'ferg',$73+$80
       DC.B   'gol',$64+$80
       DC.B   'alchem',$79+$80
       DC.B    $82              ;END OF SECTION

