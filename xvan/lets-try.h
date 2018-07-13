
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 Marnix van den Bos.                   */
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


#if !defined(__lets_try)
#define __lets_try


extern int32_t   nr_of_locs;  /* fileio.c */
extern int32_t   nr_of_objs;  /* fileio.c */
extern dirInfo   *obj_dir;    /* fileio.c */
extern verbDir   *verb_dir;   /* fileio.c */

extern int32_t subject;       /* Current subject(s).          */
extern int32_t capital;       /* Start printing with a capital character.  */


extern int32_t CheckDoTimers(void);                            /* timers.c  */
extern int32_t HandleTimers(usrActionRec*, int32_t);           /* timers.c  */
extern int32_t XeqPrologue(int32_t);                           /* execute.c */
extern int32_t XeqEpilogue(int32_t);                           /* execute.c */
extern int32_t XeqActionRec(usrActionRec*, int32_t*, int32_t); /* execute.c */
extern int32_t XeqVerbDefault(usrActionRec*, int32_t);         /* execute.c */
extern int32_t ContList(int32_t, int32_t*, int32_t);           /* sysfunc.c */

extern char    *TranslateKeyword(char*);                       /* errors.c  */
extern void    PrintError(int16_t, resultStruct*, char*);      /* errors.c  */

#endif
