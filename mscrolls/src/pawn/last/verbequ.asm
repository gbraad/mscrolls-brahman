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
*	@(#)verbequ.asm	2.2
*	1/23/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.VERBEQUATES
*
*--------------------------------
 
 
VNLOOK         EQU  0   ;THIS IS ZERO !
VNEXAMINE      EQU  1   
VNGET          EQU  2 
VNTAKE         EQU  3 
VNCARRY        EQU  4  
VNDROP         EQU  5  
VNLEAVE        EQU  6    
VNGO           EQU  7   
VNWALK         EQU  8  
VNENTER        EQU  9     
VNRUN          EQU  10    
VNSHIFT        EQU  11
VNMOVE         EQU  12   
VNLIFT         EQU  13
VNRAISE        EQU  14
VNLOWER        EQU  15
VNPUSH         EQU  16
VNPULL         EQU  17 
VNSMELL        EQU  18  
VNTASTE        EQU  19   
VNEAT          EQU  20   
VNDRINK        EQU  21   
VNPUT          EQU  22   
VNINSERT       EQU  23   
VNINVENTORY    EQU  24   
VNFIT          EQU  25    
VNPOUR         EQU  26
VNEMPTY        EQU  27
VNFILL         EQU  28 
VNMIX          EQU  29  
VNJUMP         EQU  30    
VNVAULT        EQU  31
VNTHROW        EQU  32    
VNPUNCH        EQU  33   
VNKICK         EQU  34    
VNFIGHT        EQU  35  
VNATTACK       EQU  36   
VNKILL         EQU  37   
VNBREAK        EQU  38 
VNSMASH        EQU  39    
VNDAMAGE       EQU  40    
VNREMOVE       EQU  41   
VNSCRAPE       EQU  42
VNLIGHT        EQU  43   
VNUNLIGHT      EQU  44
VNEXTINGUISH   EQU  45   
VNOPEN         EQU  46 
VNCLOSE        EQU  47   
VNSUSS         EQU  48 
VNSTATUS       EQU  49   
VNDEBUG        EQU  50  
VNFIRE         EQU  51   
VNSHOOT        EQU  52   
VNBLAST        EQU  53  
VNTWIST        EQU  54  
VNTURN         EQU  55   
VNSAY          EQU  56   
VNSHOUT        EQU  57    
VNYELL         EQU  58   
VNCRY          EQU  59  
VNCALL         EQU  60  
VNSTRANGLE     EQU  61  
VNCHOKE        EQU  62   
VNFEEL         EQU  63  
VNTOUCH        EQU  64     
VNWEAR         EQU  65  
VNREAD         EQU  66   
VNHIDE         EQU  67
VNHIT          EQU  68   
VNSTEAL        EQU  69   
VNBASH         EQU  70   
VNUNLOCK       EQU  71  
VNPICK         EQU  72  
VNTIE          EQU  73  
VNFIX          EQU  74  
VNJOIN         EQU  75    
VNUNTIE        EQU  76
VNFEED         EQU  77
VNBITE         EQU  78
VNUSE          EQU  79     
VNCROSS        EQU  80
VNSLIDE        EQU  81    
VNBLOW         EQU  82
VNPRESS        EQU  83
VNGIVE         EQU  84   
VNCLIMB        EQU  85   
VNBURN         EQU  86
VNIGNITE       EQU  87
VNCAST         EQU  88    
VNFOD          EQU  89
VNEXIT         EQU  90    
VNASK          EQU  91   
VNQUESTION     EQU  92   
VNMELT         EQU  93
VNLISTEN       EQU  94    
VNSEARCH       EQU  95
VNFIND         EQU  96  
VNGRAB         EQU  97
VNRUB          EQU  98    
VNSMOKE        EQU  99
VNWAIT         EQU  100    
VNWAKE         EQU  101
VNRAPE         EQU  102   
VNMEND         EQU  103    
VNLEVER        EQU  104
VNBUY          EQU  105   
VNPURCHASE     EQU  106
VNLEAN         EQU  107
VNRIDE         EQU  108
VNPLANT        EQU  109
VNLOCK         EQU  110
VNTEAR         EQU  111
VNRIP          EQU  112
VNCUT          EQU  113
VNKNOCK        EQU  114
VNPOINT        EQU  115
VNSHINE        EQU  116
VNSPLIT        EQU  117
VNUNMIX        EQU  118
VNSEPARATE     EQU  119
VNEXITS        EQU  120
VNCOVER       EQU  121
VNUNCOVER     EQU  122
VNSIT         EQU  123
VNLIE         EQU  124
VNKISS        EQU  125
VNVOTE        EQU  126
VNSTAND        EQU  127
VNMOUNT        EQU  128
VNTELL         EQU  129
VNOFFER       EQU  130
VNDISMOUNT    EQU  131
VNHELP        EQU  132
VNLAUGH       EQU  133
VNSHOW         EQU       134
       IFNE     YCHEAT
VNLOOP         EQU  VNSHOW+1
       ENDC

