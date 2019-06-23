
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

#if !defined(__prseloc)
#define __prseloc

/*************/
/* externals */
/*************/

extern int32_t  line_num;
extern int32_t  first_direction_id;
extern int32_t  nr_of_locations;

extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**, fileList**);  /* fileio.c */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);             /* fileio.c */
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);
extern int32_t  ParseDescriptions(char**, int32_t*, int32_t, FILE**, fileList**);
extern int32_t  ProcLocDSys(char*, int32_t);                                              /* tables.c */
extern int32_t  ProcExit(int32_t, int32_t, int32_t);
extern int32_t  ParseFlags(char**, int32_t*, int32_t, FILE**, fileList**);
extern int32_t  ParseAttributes(char**, int32_t*, int32_t, FILE**, fileList**);
extern int32_t  ParseTriggers(char**, int32_t*, int32_t, FILE**, fileList**);
extern int32_t  IsDirection(char*, int32_t*);
extern int32_t  StoreKeyword(int32_t);
extern void     ErrHdr(void);                                                             /* errors.c */
extern void     PrintError(int16_t, resultStruct*, char*);                                /* errors.c */
extern char     *TranslateKeyword(char*);                                                 /* errors.c */

#endif
