
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


#if !defined(__timers)
#define __timers

/*************************/
/* External declarations */
/*************************/

extern int32_t      nr_of_timers;                                                        /* fileio.c  */
extern timerInfo    *timers;                                                             /* fileio.c  */

extern void         Log(char*, char*, char*);                                            /* output.c  */
extern void         PushUndoItem(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);  /* undo.c    */  /* @!@ */
extern void         Output(void);                                                        /* output.c  */
extern void         PrintError(int16_t, resultStruct*, char*);                           /* errors.c  */
extern char         *TranslateKeyword(char*);                                            /* errors.c  */
extern resultStruct XeqTrigger(int32_t, int32_t, usrActionRec*, int32_t);                /* execute.c */
extern int32_t      LookUpId(char*);                                                     /* syntax.c  */

#endif
