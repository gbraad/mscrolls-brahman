/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
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
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for posterity and
 * historical reference.
 */

#pragma once

#undef FALSE
#define FALSE 0

#undef TRUE
#define TRUE  1

#define i_end -1
#define i_none 0
#define i_dbit 1
#define i_sbit 2
#define i_movep 3
#define i_ori 4
#define i_andi 5
#define i_subi 6
#define i_addi 7
#define i_eori 8
#define i_cmpi 9
#define i_move 10
#define i_negx 13
#define i_mfsr 14
#define i_clr 15
#define i_neg 16
#define i_mtccr 17
#define i_not 18
#define i_mtsr 19
#define i_nbcd 20
#define i_pea 21
#define i_swap 22
#define i_mmfreg 23
#define i_extw 24
#define i_extl 25
#define i_tst 26
#define i_tas 27
#define i_mmtreg 28
#define i_trap 29
#define i_link 30
#define i_unlk 31
#define i_mtusp 32
#define i_mfusp 33
#define i_reset 34
#define i_nop 35
#define i_stop 36
#define i_rte 37
#define i_rts 38
#define i_trapv 39
#define i_rtr 40
#define i_jsr 41
#define i_jmp 42
#define i_chk 43
#define i_lea 44
#define i_addq 45
#define i_subq 46
#define i_scc 47
#define i_dbcc 48
#define i_bcc 49
#define i_moveq 50
#define i_or 51
#define i_divu 52
#define i_divs 53
#define i_sbcd 54
#define i_sub 55
#define i_subx 56
#define i_cmp 57
#define i_cmpm 58
#define i_eor 59
#define i_and 60
#define i_mulu 61
#define i_muls 62
#define i_abcd 63
#define i_exgd 64
#define i_exga 65
#define i_exgm 66
#define i_add 67
#define i_addx 68
#define i_dshift 69
#define i_mshift 70
#define i_adda 71
#define i_cmpa 72
#define i_movea 73
#define i_suba 74
#define i_SEARCH 75
#define i_GETDATA 76
#define i_SETNOUNS 77
#define i_NOUNSCAN 78
#define i_WORDSCAN 79
#define i_GETNOUNS 80
#define i_QCARRIED 81
#define i_TEXT 82
#define i_RANDOM1 83
#define i_RANDOM2 84
#define i_LOADFILE 85
#define i_SAVEFILE 86
#define i_CHAROUT2 87
#define i_CHAROUT 88
#define i_CHARIN 89
#define i_A_line 90
#define i_F_line 91
#define i_eoriCCR 92
#define i_andiCCR 93
#define i_oriCCR 94
#define i_GETTIED 95
#define i_FINDZERO 96
#define i_RET 97
#define i_GRAPHICS 98
#define i_PRINTER 99
#define i_RESTART 100
#define i_QUIT 101
#define i_CALL 102
#define i_TEST 103
#define i_PUSHM 104
#define i_PULLM 105
/*
#define	i_GETWORD	106
#define i_VALID_NP	107
#define i_CHK4PT	108
#define i_GETNPC	109
#define	i_GETRELN	110
#define i_GET_CONT	111
#define i_DBS		112
#define i_ISARE		113
#define i_ISAREN	114
#define i_P_BTN		115
#define i_P_SCN		116
#define i_P_MSG		117
#define	i_P_NOUN	118
#define i_P_SUB		119
#define i_P_SUBV	120
#define i_SETOUT	121
#define i_P_TN		122
#define i_P_TNSTOP	123
#define i_UTIL_BS	124
#define i_FULLSTOP	125
#define i_BSFULLSTOP	126
#define i_CARRIED	127
#define	i_CALCNET	129
#define	i_CHAR_SUB	130
#define	i_CHKNPC	131
#define	i_CLRADJ	132
#define	i_D72	133
#define	i_DBEXIT	134
#define	i_DEAD	135
#define	i_DECODE	136
#define	i_FIXCHK	137
#define	i_FIXCONT	138
#define	i_FIXIREL	139
#define	i_FIXSYS	140
#define	i_FNDWRD	141
#define	i_GETADJPH	142
#define	i_DOGETNOUNS	143
#define	i_GETVECT	144
#define	i_INCNOUN2	145
#define	i_ISDOFRE	146
#define	i_MNPH	147
#define	i_NOERR	148
#define	i_P_FROM	149
#define	i_P_LIST	150
#define	i_P_OBJPRN	151
#define	i_P_PRN	152
#define	i_P_QN	153
#define	i_P_SAY	154
#define	i_P_TNIN	155
#define	i_P_TSNOUN	156
#define	i_P_VAGE	157
#define	i_P_VERB	158
#define	i_PR20	159
#define	i_R_NOUN	160
#define	i_R_NOUN2	161
#define	i_RUSURE	162
#define	i_SCREEN	163
#define	i_DOSEARCH	164
#define	i_DOSEARCH2	165
#define	i_TOPLOC	166
#define	i_UTEXIT	167
#define	i_VALID	168
#define	i_YCANT	169
#define	i_YCANTNOW	170
#define	i_YESERR	171
#define	i_YCANTW	172
#define	i_P_SAY2	173
#define	i_GETNOUN2	174
#define	i_P_LISTSTOP	175
#define	i_VALID_WO	176
*/
#define i_MSG	128
#define i_DO		176
#define i_illegal	177
#define i_GETWTAB	178
#define i_PUTWTAB	179
#define i_PRINTWORD	180
#define i_COPYWORD	181
#define i_SETNE		182
#define i_SETEQ		183
#define i_SETNE_RET	184
#define i_SETEQ_RET	185
#define i_POPSP_RET	186
#define i_MAKE_DIFF	187
#define i_UNDO_DIFF	188
#define i_GET_NEXT_EV	189
#define i_PROMPT_EV	190
#define i_NOTIFY	191
