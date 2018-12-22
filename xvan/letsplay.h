
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018, 2019 Marnix van den Bos.             */
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


#if !defined(__letsplay)
#define __letsplay

/*************************/
/* External declarations */
/*************************/

extern int32_t   nr_of_locs;                                   /* fileio.c */
extern int32_t   nr_of_objs;                                   /* fileio.c */
extern dirInfo   *obj_dir;                                     /* fileio.c */
extern storyInfo story_info;                                   /* init.c   */

extern char    *outputline;                                    /* init.c   */

extern void    Output(char*, int);                             /* output.c   */
extern void    PrintError(int16_t, resultStruct*, char*);      /* errors.c   */
extern void    PrintString(char*, int);                        /* output.c   */
extern int32_t ProcessInput(char*);                            /* input.c    */
extern int32_t FilledOut(extendedSysDescr*);                   /* trnslate.c */
extern int32_t CountSubjectsInParsedInput(parsedInput*);       /* trnslate.h */
extern int32_t SplitSubject(parsedInput*, int32_t);            /* trnslate.c */
extern void    PrintNotFound(extendedSysDescr*);               /* trnslate.c */
extern int32_t CheckDoTimers(void);                            /* timers.c   */
extern int32_t HandleTimers(usrActionRec*, int32_t);           /* timers.c   */
extern void    InitParsedInput(parsedInput*);                  /* syntax.c   */
extern void    PrintParsedInput(parsedInput*);                 /* debug.c    */
extern int32_t InitUsrActionRec(usrActionRec*);                /* trnslate.c */
extern int32_t PrintUsrActionRec(usrActionRec*);               /* debug.c    */
extern int32_t ParseInput(char*, parsedInput*, int32_t);       /* syntax.c   */
extern int32_t ReplaceItObjects(usrActionRec*);                /* execute.c  */
extern int32_t XeqPrologue(int32_t);                           /* execute.c  */
extern int32_t XeqEpilogue(int32_t);                           /* execute.c  */
extern int32_t XeqActionRec(usrActionRec*, int32_t*, int32_t); /* execute.c  */
extern int32_t XeqVerbDefault(usrActionRec*, int32_t);         /* execute.c  */
extern int32_t ContList(int32_t, int32_t*, int32_t);           /* sysfunc.c  */

extern resultStruct Translate(parsedInput*, int32_t, usrActionRec*, char*);  /* trnslate.c */

#endif
