
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix.home@gmail.com>                                              */
/*                                                                      */
/* This file is part of XVAN, an Interactive Fiction authoring system.  */
/*                                                                      */
/* XVAN is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, version 3.                             */
/*                                                                      */
/* XVAN is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with XVAN.  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                      */
/************************************************************************/


#if !defined(__arithmtc)
#define __arithmtc

/*************************/
/* External declarations */
/*************************/

extern timerInfo *timers;
extern short     debug_level;                                                       /* debugcmd.c */

extern void     PushUndoItem(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t); /* undo.c     */  /* @!@ */
extern void     Output(char*);                                                      /* output.c   */
extern void     PrintError(int16_t, resultStruct*, char*);                          /* errors.c   */
extern int32_t  GetPar(int32_t*, int32_t*, int32_t*, char**, int32_t**);            /* sysfunc.c  */
extern int32_t  GetLvaluePar(int32_t*, int32_t*, int32_t*, char**, int32_t**);      /* sysfunc.c  */
extern int32_t  GetAttributeInfo(int32_t, int32_t, attrInfo**, int32_t*);           /* sysfunc.c  */
extern int32_t  CheckPars(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);    /* checkpar.c */
extern int32_t  NextOpcode(int32_t**);
extern void     ConvSpecId(int32_t*, int32_t*);                                     /* sysfunc.c  */
extern void     DebugLevel_2_pars(char*, resultStruct*, int);                       /* debugcmd.c  */
extern void     DebugLevel_2_result(resultStruct);                                  /* debugcmd.c  */
extern int32_t  IsTimerId(int32_t);

#endif
