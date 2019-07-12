
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


#if !defined(__NL_moreinfo)
#define __NL_moreinfo


/**************************/
/* External declarations. */
/**************************/

extern dirInfo *loc_dir;
extern dirInfo *obj_dir;

extern int32_t capital;                                       /* letsplay.c   */
extern int32_t actor;
extern char    *outputline;                                   /* init.c       */
extern char    *prompt;                                       /* letsplay.c   */
extern void    NL_PrintArticle(sysDescr*, int32_t, int);      /* nl-article.c */
extern void    Output(void);                                  /* output.c     */
extern int32_t IsLocId(int32_t);

extern void    Output(void);                                  /* output.c     */
extern void    PrintId(int32_t, int);                         /* output.c     */
extern void    PrintWord(int32_t, int);                       /* output.c     */
extern void    PrintExtendedSysDescr(extendedSysDescr*, int); /* output.c     */
extern void    PrintString(char*, int);                       /* output.c     */
extern int32_t LookUpId(char*);
extern void    GetAddtlInput(kvPair*, char*, int32_t);        /* input.c      */
extern void    ResetKVPair(kvPair*);                          /* json.c       */

#endif
