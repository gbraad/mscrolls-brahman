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
*	@(#)sets.asm	2.1
*	9/19/86
*
******************************************************


* table of multiples 

              DC.W    NHOE,NTROWEL,NRAKE,0
              DC.W    NMKEY,NWKEY,NKEY,0           ;METAL,WOODEN,BLUE KEYS
              DC.W    NBLUE,NGREEN,NRED,NWHITE,0
              DC.W    NBOTTLE1,NBOTTLE2,NBOTTLE3,NBOTTLE4,0   ;BOTTLES
              DC.W    NRICE,NRATIONS,NCARROT,0                ;FOOD


         IFNE     YCHEAT             ;NOT FOR THE PUNTERS!
 
              DC.W    NGUARDS
              DC.W    NJOHN
              DC.W    NDONKEY
              DC.W    NADVENT
              DC.W    NHORSE
              DC.W    NGURU
              DC.W    NBATS
              DC.W    NDEVIL
              DC.W    NDEMONS
              DC.W    NLEWIS
              DC.W    NDRAGON
              DC.W    NRABBIT
              DC.W    NPORTER
              DC.W    NSNOWMAN
              DC.W    NPRINCESS
              DC.W    NSHADOWS
              DC.W    NKRONOS
              DC.W    NALCH
              DC.W    0
 
         ENDC
         
              DC.W    NSHIRT,NJEANS,NCLOAK,NHAT2,NDRESS,0
 
              DC.W    NHAT,NHAT2
              DC.W    NHARDHAT,0
 
              DC.W    NBUTTON1,NBUTTON2
              DC.W    NAEROBUTTON
              DC.W    NLIFTBUTTON,0
 
              DC.W    NCHEST,NCHEST2,0
 
              DC.W    NSBALBX,NLBALBX,NBOXES,0
              DC.W    NLAMP,NLAMPS,NLANTERN,0
              DC.W    NWALL,NLWALL,NSWALL,NPWALL,0
* doors
		DC.W	NDOOR,NDOORS,NDOOR2,NDDOORS
		DC.W	NTREEDOOR,NSHEDDOOR,NFRONTDOOR
		DC.W	NLIFTDOOR,NHANDLES,0

* included by COMMON.ASM
