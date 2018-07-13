
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

#if !defined(__tables)
#define __tables


#define TERMINATOR   "******"
#define UNKNOWN      -1

/*************/
/* externals */
/*************/

extern FILE     *source_file;
extern char     current_filename[];
extern int16_t  xvan_language;
extern int32_t  line_num;
extern int32_t  total_lines;                                /* init.c     */
extern int32_t  first_direction_id;
extern dirData  dir_data;                                   /* fileio.c   */

extern char     entrance_trigger[];  /* predefs.c */
extern char     exit_trigger[];      /* predefs.c */
extern char     default_trigger[];   /* predefs.c */

extern void     ENG_CheckLocWildCard(char*, resultStruct*);
extern void     ENG_CheckObjWildCard(char*, resultStruct*);
extern void     NL_CheckLocWildCard(char*, resultStruct*);
extern void     NL_CheckObjWildCard(char*, resultStruct*);
extern char     *strlwr(char*);                             /* keyword.c  */
extern char     *TranslateKeyword(char*);                   /* errors.c   */
extern int32_t  ParseDSys(char*, extendedSysDescr*);        /* prsedsys.c */
extern void     ErrHdr(void);                               /* errors.c   */
extern void     PrintError(int16_t, resultStruct*, char*);  /* errors.c   */

#endif
