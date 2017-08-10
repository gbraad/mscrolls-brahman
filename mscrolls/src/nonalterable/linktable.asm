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


**********************************************************
*
*	@(#)linktable.asm	1.5
*	5/16/87
*
**********************************************************

	SECTION	"ADV",code

* trying to avoid 16bit range errors...
*
* the XDEFs in here get used by BRAL/BSRL (longish BRA and BSR)

Global	MACRO	* real routine
	XDEF	_\1
	XREF	\1
_\1	BRA	\1
	ENDM

LinkTable

	Global	P.Description
	Global	P.DESC
	Global	P.LIST
	Global	P.LIST2
	Global	P.LOR
	Global	P.QN
	Global	P.NQN
	Global	P.NOUN
	Global	P.NOUN2
	Global	NONLY
	Global	P.TN
	Global	P.QUANT
	Global	P.PRN
	Global	P.OBJPRN
	Global	ISARE
	Global	P.PV
	Global	P.VQN
	Global	P.SUBOB
	Global	P.SUB
	Global	DERR
	Global	STOPBRK
	Global	YCANTW
	Global	CARRIED
	Global	P.BOTN
	Global	P.XFERGS
	Global	P.NO.TNG
	Global	P.NOTANY
	Global	ISDOFRE
	Global	HOLDE
	Global	UNHOLDE
	Global	P.SUBV
	Global	P.NFND
	Global	P.NQEN
	Global	SCANSV
	Global	DBS
	Global	OK
	Global	CALCWHT
	Global	P.BRAKTS
	Global	GET_CONT
	Global	R.NOUN
	Global	R.NOUN2
	Global	SPACE
	Global	ROOTNUM
	Global	P.MSG
	Global	P.SCN
	Global	DO.FIX
	Global	SETA1
	Global	P.CNTO
	Global	P.FROM
	Global	P.WITH
	Global	CONNECT
	Global	CONNECT2
	Global	SCAN
	Global	P.Refresh
	Global	ForceRefresh
	Global	P.EXITS
	Global	CALCNET
	Global	PCOK
	Global	MIXD0D5
	Global	FIXCHK
	Global	SETPRON
	Global	IFLIQUID
	Global	WHENSAT
	Global	SATMSG
	Global	ONLYREL
	Global	TIETOG
	Global	ISTIETOG
	Global	SETADN
	Global	USE
	Global	BUFSHUF
	Global	MOVBUF
	Global	DOSCORE2
	Global	DOSCORE
	Global	DO.SCORE
	Global	P.NORCH
	Global	DO.SCN
	Global	ISANYREL
	Global	TOPNPC
	Global	SETOUT
	Global	PRTWRD
	Global	PRTWRD2
	Global	WHRIDING
	Global	P.CONJ
	Global	PRTNUM
	Global	PRTTXT
	Global	PRTBYTE
	Global	PRINTWRD
	Global	P.ANOUN
	Global	P.NONLY
	Global	P.IDFY
	Global	P.CWHT
	Global	CHK4PT
	Global	CHK6PART
	Global	P.TAB
	Global	ISAREN
	Global	FIXCONT
	Global	FNDEND
	Global	FIXSYS
	Global	YCANT
	Global	P.BTN
	Global	P.PARTICLE
	Global	P.VERB
	Global	FULLSTOP
	Global	BSFULLSTOP
	Global	P.VERBING
	Global	CHAROUTL
	Global	CHAROUT
	Global	CHAROUT2
	Global	CHAR.SUB
	Global	FNDSEAT
	Global	P.BYAN
	Global	P.SEPA
	Global	DEAD
	Global	P.TNIN
	Global	P.BTNI
	Global	PHEXBYTE
	Global	PHEXWORD
	Global	PRTHEX
	Global	REALBAD
	Global	SCREEN
	Global	PROCESS
	Global	PROC.LAB
	Global	SC70
	Global	GETDATA
	Global	GETNOUNS
	Global	GETNOUN2
	Global	GETNPCNO
	Global	GETRELN
	Global	GETVECT
	Global	GETNPC
	Global	PROC.EX
	Global	SINGV
	Global	P.INIT
	Global	SETPAUSE
	Global	PROT1
	Global	FINNPC
	Global	KILLNOTNPC
	Global	KILLNPC
	Global	KILLPSEUDO
	Global	SUSSINV
	Global	FINRELNPC
	Global	ISDOOR
	Global	P.MHAV
	Global	INITRND
	Global	RND.D1
	Global	RNDGEN
	Global	RNDNUM
	Global	RND.DATA

	END
