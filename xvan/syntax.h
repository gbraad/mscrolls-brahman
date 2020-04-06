
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


#if !defined(__syntax)
#define __syntax

/*************************/
/* External declarations */
/*************************/

extern storyInfo story_info;                                /* init.c   */

extern int32_t   nr_of_words;                               /* fileio.c */
extern wordTable *word_table;                               /* dirs.c   */
extern verbDir   *verb_dir;                                 /* fileio.c */
extern dirInfo   *loc_dir;                                  /* dirs.c   */
extern dirInfo   *obj_dir;                                  /* dirs.c   */

extern int32_t   ENG_CheckSyntax(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, parsedInput*);
extern int32_t   NL_CheckSyntax(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, parsedInput*);

extern int32_t   CheckPlural(char*);                        /* plural.c   */
extern void      PrintError(int16_t, resultStruct*, char*); /* errors.c   */
extern int32_t   StringToNum(char*, int32_t*);              /* trnslate.c */
extern int32_t   IsLocId(int32_t);                          /* execute.c  */
extern int32_t   IsObjId(int32_t);                          /* execute.c  */
extern void      PrintWord(int32_t, int);                   /* output.c   */
extern void      InitSysDescr(sysDescr*);                    /* fileio.c   */   /* @!@ */
extern void      PrintSysDescr(sysDescr*, int);             /* output.c   */
extern void      PrintParsedInput(parsedInput*);            /* debug.c    */
extern void      PrintUsrActionRec(usrActionRec*);          /* debug.c    */

#endif
