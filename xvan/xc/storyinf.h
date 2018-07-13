
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

#if !defined(__storyinf)
#define __storyinf

/*************/
/* externals */
/*************/

extern FILE    *source_file;
extern FILE    *datafile;
extern int32_t  line_num;
extern int16_t  xvan_language;    /* init.c */
extern int16_t  story_language;   /* init.c */

extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**, fileList**); /* fileio.c */
void            CleanUpFileList(fileList**);                                             /* tables,c */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);            /* fileio.c */
extern void     ErrHdr(void);                                                            /* errors.c */
extern void     PrintError(int16_t, resultStruct*, char*);                               /* errors.c */
extern char     *TranslateKeyword(char*);                                                /* errors.c */

#endif
