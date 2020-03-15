
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


#if !defined(__init)
#define __init

/************/
/* Ddefines */
/************/

#define STACK_SIZE  50        /* max 50 ints on stack.     */
                              /* Also defined in sysfunc.h */
#define DATAFILE "out.dat"    /* storyfile                 */

/**************************/
/* External declarations */
/*************************/

extern void Log(char*, char*, char*);

extern short    transcript;                                 /* letsplay.c */

extern int32_t  InitUndoStack(void);                        /* undo.c      */  /* @!@ */
extern void     PrintError(int16_t, resultStruct*, char*);  /* errors.c    */
extern void     *Output(void);                              /* output.c    */
extern char     *ResetString(char*);                        /* jsonbld.c   */
extern char     *AddToString(char*, char*);                 /* descr2str.c */
extern int32_t  ReadStoryInfo(storyInfo*);                  /* fileio.c    */
extern int32_t  InitDirs(void);                             /* fileio.c    */
extern int32_t  InitVerbs(void);                            /* fileio.c    */
extern int32_t  InitLocations(void);                        /* fileio.c    */
extern int32_t  InitObjects(void);                          /* fileio.c    */
extern int32_t  InitAttributes(void);                       /* fileio.c    */
extern void     ProcLFlagVal(int32_t, int32_t);
extern int32_t  LetsPlay(void);                             /* letsplay.c  */
extern void     CleanUp(void);                              /* cleanup.c   */
extern void     PrintString(char*, int);                    /* output.c    */

/* IFI stuff */
extern int32_t  ValidateJson(char*);
extern int32_t  CheckIFI(char*);

#endif
