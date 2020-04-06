
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

#if !defined(__prsdescr)
#define __prsdescr

#define MAX_COND_LEN		100 /* also defined in prsefun.h and prsassgn.h */

/*************/
/* Externals */
/*************/

extern int32_t     line_num;

extern int32_t  ChangeInputFile(char**, int32_t*, int32_t, int32_t, FILE**, fileList**);  /* fileio.c */
extern char     *GetNextWord(int32_t*, int32_t, int32_t, FILE**, fileList**);             /* fileio.c */
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);                            /* tables.c */
extern int32_t  ProcLocDSys(char*, int32_t);
extern int32_t  ProcObjDSys(char*, int32_t);
extern int32_t  IsLocId(int32_t);
extern int32_t  StoreId(int32_t);
extern int32_t  StoreString(char*);
extern void     ErrHdr(void);                                                             /* errors.c */
extern void     PrintError(int16_t, resultStruct*, char*);                                /*errors.c */

#endif
