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
*	@(#)parssub1.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PARSER.SUB1
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include	"macros.asm"

      XDEF     GETPRON,VERBOWN

      XREF     OLDBUF,NOUNBUF,GETDATA,GETNPC,PRONHIM,PRONHER,VERB


 
*--------------------------------
*
* GETPRON
*
* TAKES CURRENT NOUN(S)
* TO USE AS PRONOUNS NEXT TIME
*
*--------------------------------
 
GETPRON
 
       PUSH_L  D0/A0-A1/A3
 
       LEA     OLDBUF(A4),A0
       LEA     NOUNBUF(A4),A1
10$
       MOVE.W  (A1)+,(A0)+        ;'THEM' LIST
       BNE.S   10$
 
       LEA     OLDBUF(A4),A1
20$
       MOVE.W  (A1)+,D0
       BEQ.S   90$
       GETINFO
       BTST    #6,4(A0)           ;NPC?
       BEQ.S   20$                ;NO = NOT 'HIM' OR 'HER'
       DO	GETNPC
       BTST    #0,2(A3)           ;1=MALE, 0=FEMALE
       BEQ.S   30$
       MOVE.W  D0,PRONHIM(A4)
       BRA.S   20$
30$
       MOVE.W  D0,PRONHER(A4)
       BRA.S   20$
 
90$
       PULL_L  D0/A0-A1/A3
       RET
 
 
*--------------------------------
*
* VERBOWN - SUSSES WHETHER TO
*           IGNORE OWNERSHIP
*           OF A NOUN
*
* ON EXIT - BEQ = IGNORE
*         - BNE = DONT IGNORE
*
*--------------------------------
 
VERBOWN
 
       PUSH_L  D0/D5/A0
       MOVE.W  VERB(A4),D5
       LEA     20$(PC),A0         ;START OF VERB LIST
       MOVEQ   #30$-20$-1,D0      ;NO OF VERBS -1 
10$
       CMP.B   (A0)+,D5           ;RIGHT VERB?
       DBEQ    D0,10$             ;IF YES, FALL THROUGH
       PULL_L  D0/D5/A0
       RET
 
20$
       
       SVERB	ASK
       SVERB	BUY
       SVERB	GIVE
       SVERB	LISTEN
       SVERB	STEAL
       SVERB	WAIT
       SVERB	FIND
       SVERB	VOTE
30$
       ALIGN

      END
 
*--------------------------------:

