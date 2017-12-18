/**
 *
 * Copyright (c) 2017 Voidware Ltd.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code as
 * defined in and that are subject to the Voidware Public Source Licence version
 * 1.0 (the 'Licence'). You may not use this file except in compliance with the
 * Licence or with expressly written permission from Voidware.  Please obtain a
 * copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
 * it before using this file.
 *
 * THIS IS NOT FREE SOFTWARE.
 * 
 * The Original Code and all software distributed under the Licence are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Please see the Licence for the specific language governing rights and 
 * limitations under the Licence.
 *
 * contact@strandgames.com
 *
 */

/* This software was originally written and owned By Magnetic Scrolls Ltd.
 * It has now been released under a public source licence so that anyone
 * can read it and learn.   
 */

/* types of addressing mode */

#define M_DREG		0		/* Dn */
#define M_AREG		1		/* An */
#define M_AREG_IND	2		/* (An) */
#define M_POSTINC	3		/* (An)+ */
#define M_PREDEC	4		/* -(An) */
#define M_AREG_OFF	5		/* d16(An) */
#define M_AREG_OFF_IND	6	/* d8(An,Rn.S) */
#define M_PC		7
#define 	M_ABS16	0		/* addr16 */
#define		M_ABS32	1		/* addr32 */
#define 	M_PCREL	2		/* d16(PC) */
#define 	M_PCREL_IND	3	/* d8(PC,Rn.S) */
#define 	M_IMMED	4		/* #d */
#define M_CCR		8		/* CCR */
#define M_SR		9		/* SR */
#define M_USP		10		/* USP */
#define M_BITFIELD	11		/* either Dn or 5 bit immediate */
#define M_CONTROL	12		/* control register for movec */
