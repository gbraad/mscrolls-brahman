
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

#if !defined(__prsstory)
#define __prsstory

/*************/
/* Externals */
/*************/

extern int32_t line_num;
extern int32_t nr_of_syn_verbs;
extern char    current_filename[];  /* init.c */
extern FILE    *source_file;
extern FILE    *datafile;

extern char    *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);             /* fileio.c  */
extern int32_t IsValidKeyword(int32_t);                                                  /* keyword.c */
extern int32_t ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**, fileList**);  /* fileio.c  */
extern int32_t InitVerbDir(void);
extern int32_t InitExits(void);    /* tables.c */
extern int32_t AddWord(char*, char*, int32_t, int32_t, int32_t*, int32_t);
extern int32_t ParseWordGroup(char**, int32_t*, int32_t, FILE**, fileList**);
extern int32_t CreateWordTable(void);
extern int32_t ParseLoc(int64_t, FILE**, fileList**);
extern int32_t ParseObj(int64_t, FILE**, fileList**);
extern int32_t ParseCDescrs(char**, int32_t*, FILE**, fileList**);
extern int32_t FindCTriggerIds(char**, int32_t*, int32_t, FILE**, fileList**);
extern int32_t ParseCTrigs(char**, int32_t*, FILE**, fileList**);
extern int32_t ParseAttributes(char**, int32_t*, int32_t, FILE**, fileList**);
extern int32_t ParseFlags(char**, int32_t*, int32_t, FILE**, fileList**);
extern int32_t ParseVerb(int64_t, int32_t, FILE**, fileList**);
extern int32_t ParseTimers(char**, int32_t*, FILE**, fileList**);
extern int32_t StoreString(char*);
extern void    ErrHdr(void);                               /* errors.c */
extern void    PrintError(int16_t, resultStruct*, char*);  /* errors.c */

#endif
