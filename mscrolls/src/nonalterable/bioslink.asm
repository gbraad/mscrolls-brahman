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
*	@(#)bioslink.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*-----------------------------
*
*     BIOSLINK
*
*-----------------------------
*
* TAKE CARE!! Moving some of this into areas saved for SAVE/RESTORE
*             will cause nasty things of a great order to occur!
*
* esp.  FILENO (open file handle!!)
*       PICTNUM (picture on screen NOW!)
*       PROT_FLG(Have they done the protection?)
*
*************************************************************************

 
	include	"equates.asm"

     IFNE      BookProtection
         XDEF        PROT_FLG
     ENDC


         XREF       COUT,GETLINE

      IFNE     QL128
         XDEF       CURSX,CURSY
      ENDC

         XDEF      BIOSQUIT,RND.CHIN

      IFNE     Wimp
         XDEF       VOLDBUF
      ENDC

      IFEQ     DosPresent
         XDEF       DEST,DRIVE
         XDEF       FILESIZE,READIT,WRITEIT
      ENDC


         XDEF       SWIDTH,VWIDTH,LEFTM,SLSIZE2,SLSIZE20,FILENO,TOPCURS1
         XDEF       DISPTYPE

      IFNE     FunctionKeys
         XDEF       FUNCTNO,FUNCPNTR
      ENDC


      IFNE     Graphics
         XDEF        PICTNUM
      ENDC


      IFNE     SpoolLen
         XDEF        PRINTER
      IFNE	INOUTLINK

	 XDEF	SpoolPos,PrtThere,SpoolOff

      ENDC

      ENDC

      IFNE	INOUTLINK

      XDEF	MORE_CNT,CWID_FLG,INBUFF,OLDTEXT,CHANNEL

      IFNE	ST520
      XDEF	FREE_FLG,CURSX,STIP_FLG,SCR_IGNO,DITH_FLG,CONTERM,COL_FLG
      ENDC

      ENDC


      IFNE      BookProtection

PROT_FLG:   DC.W  0

	IFEQ	M68000
	XDEF	ProtData
ProtData	DC.L	0
	ENDC

      ENDC

SLSIZE20 EQU 20
SLSIZE2  EQU 20*$100

TOPCURS1    EQU   $00000001
TOPCURS2    EQU   $00300001
	
	IFNE	INOUTLINK
MORE_CNT	DC.W	0
CWID_FLG	DC.W	0
CHANNEL		DC.W	0
OLDTEXT		DS.B	BUFFLEN
INBUFF		DS.B	BUFFLEN
	ENDC

	IFNE	INOUTLINK
	IFNE	SpoolLen
SpoolPos	DC.L	0
PrtThere	DC.W	0
SpoolOff	DC.L	0
	ENDC
	ENDC

	IFNE	INOUTLINK
	IFNE	ST520
CONTERM		EQU	$484
FREE_FLG	DC.B	0
STIP_FLG	DC.B	10
DITH_FLG	DC.B	0
COL_FLG		DC.B	0
SCR_IGNO	DC.W	0
CURSX		DC.W	0
	ENDC
	ENDC


LEFTM       DS.W  1
SWIDTH      DC.W  WIDTH.DEF-1
VWIDTH      DC.W  WIDTH.DEF
LINBOT      DS.W  1         ;N/A IN ATARI?
QLSCREEN    DS.L  1
SCNTOP      DS.L  1
SCNBOT      DS.L  1


        XDEF       BIOS.COU,BIOS.INP
        
BIOS.COU    BRA.L COUT      ;DS.L  1
BIOS.INP    BRA.L GETLINE   ;DS.L  1
            DS.L  1        ;BIOS STUFF USES THIS (I THINK)

      IFNE  QL128
CURSX       DS.W  1        ;CURSX & CURSY MUST REMAIN IN ORDER
CURSY       DS.W  1
      ENDC

      IFEQ DosPresent
READIT      DS.L  1
WRITEIT     DS.L  1
GETCHAR     DS.L  1
      ENDC
        
BUFFER

           IFNE  Wimp
VOLDBUF     DS.B  BUFFLEN
           ENDC

	IFNE	QL128
OLDTEXT     DS.B  BUFFLEN		;these 'happen' to match
INTEXT      DS.B  BUFFLEN		;where real BIOS version lives
					;in ye olde QL version
	ENDC

           IFEQ   DosPresent
DEST        DS.L  1
DRIVE       DS.W  1
FILENO      DS.W  1        ;Used to store file handle in ATARI
FILESIZE    DS.W  1
BAD.COUNT   DS.W  1
SECT.COUNT  DS.W  1
ROT.COUNT   DS.W  1
HEADBUF     DS.W  10
           ENDC

DISPTYPE    DS.B  1
DISPTYPE2   DS.B  1
EOR.COUT    DS.W  1
RND.CHIN    DS.W  1
BIOSQUIT    DS.L  1

           IFNE   DosPresent
FILENO      DS.W  1        ;Used to store file handle in ATARI
           ENDC

           IFNE   Graphics
PICTNUM     DC.B  0     ;Current picture on screen
           ENDC

           IFNE   SpoolLen
PRINTER     DC.B  0     ;0=> no script, -1=>script
           ENDC

           IFNE   FunctionKeys
FUNCTNO     DC.W  0
FUNCPNTR    DC.W  0
           ENDC

            END
