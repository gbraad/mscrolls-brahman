
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
/*                                                                      */
/* <marnix@xvan.nl>                                                     */
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

#if !defined(__parsedict)
#define __parsedict

/*************/
/* Externals */
/*************/

extern int32_t line_num;
extern int32_t nr_of_syn_verbs;     /* tables.c */
extern int32_t nr_of_directions;    /* tables.c */
extern char    current_filename[];  /* init.c */
extern FILE    *datafile;

extern char    *strlwr(char*);
extern char    *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);              /* fileio.c  */
extern int32_t IsValidKeyword(int32_t);                                                   /* keyword.c */
extern int32_t ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**, fileList**);   /* fileio.c  */
extern int32_t GetNewWordId(int32_t*);
extern int32_t GetNewVerbId(int32_t*);
extern int32_t ProcWordInfo(wordInfo*);
extern int32_t CreateWordTable(void);
extern int32_t InitVerbDir(void);
extern void    ErrHdr(void);                                                              /* errors.c */
extern void    PrintError(int16_t, resultStruct*, char*);                                 /* errors.c */
extern char    *TranslateKeyword(char*);                                                  /* errors.c */

#endif
