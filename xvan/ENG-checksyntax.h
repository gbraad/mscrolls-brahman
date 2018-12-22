
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


#if !defined(__ENG_checksyntax)
#define __ENG_checksyntax

/*************************/
/* External declarations */
/*************************/

extern verbDir *verb_dir;                               /* fileio.c */
extern char    *outputline;                             /* init.c   */

extern void    Output(char*, int);                      /* output.c */
extern int32_t LookUpId(char*);                         /* syntax.c */
extern int32_t NextWordId(char**, int32_t*, int32_t*);  /* syntax.c */
extern void    PrintParsedInput(parsedInput*);          /* debug.c  */
extern void    PrintUsrActionRec(usrActionRec*);        /* debug.c  */

#endif
